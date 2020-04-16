#include "boaaa/lv/EvaluationPass.h"

using namespace boaaa;

bool isInterestingPointer(LLVMValue* V) {
    return V->getType()->isPointerTy()
        && !llvm::isa<llvm::ConstantPointerNull>(V);
}

std::string formatPercentage(uint64_t count, uint64_t total) {
    std::string out = "[XXX.XX%]";
    out[1] = '0' + static_cast<char>((count / total));
    out[2] = '0' + static_cast<char>(((count * 10LL / total) % 10));
    out[3] = '0' + static_cast<char>(((count * 100LL / total) % 10));

    out[5] = '0' + static_cast<char>(((count * 1000LL / total) % 10));
    out[6] = '0' + static_cast<char>(((count * 10000LL / total) % 10));
    return out;
}

void EvaluationPassImpl::scanPointers(LLVMModule& M, evaluation_storage& storage)
{
    using namespace llvm;
    llvm::DataLayout DL = M.getDataLayout();

    for (LLVMFunction& F : M) {
        LLVMSetVector<LLVMValue*> Pointers;
        LLVMSmallSetVector<LLVMCallUnifyer*, 16> Calls;
        LLVMSetVector<LLVMValue*> Loads;
        LLVMSetVector<LLVMValue*> Stores;

        //argument pointes
        for (auto& I : F.args())
            if (I.getType()->isPointerTy())
                Pointers.insert(&I);

        for (auto I = inst_begin(F), E = inst_end(F); I != E; ++I) {
            if (I->getType()->isPointerTy()) // Add all pointer instructions.
                Pointers.insert(&*I);
            if (isa<LLVMLoadInst>(&*I))
                Loads.insert(&*I);
            if (isa<LLVMStoreInst>(&*I))
                Stores.insert(&*I);
            LLVMInstruction& Inst = *I;
#if LLVM_VERSION < 80
            //create on heap to use operator bool and then create obj again on heap to store it
            if (auto Call_ = CallSite(&Inst)) {
                auto* Call = new CallSite(&Inst);
#else
            if (auto* Call = dyn_cast<LLVMCallUnifyer>(&Inst)) {
#endif
                LLVMValue* Callee = Call->getCalledValue();
                // Skip actual functions for direct function calls.
                if (!isa<LLVMFunction>(Callee) && isInterestingPointer(Callee))
                    Pointers.insert(Callee);
                // Consider formals.
                for (LLVMUse& DataOp : Call->data_ops())
                    if (isInterestingPointer(DataOp))
                        Pointers.insert(DataOp);
                Calls.insert(Call);
            }
            else {
                // Consider all operands.
                for (LLVMInstruction::op_iterator OI = Inst.op_begin(), OE = Inst.op_end();
                    OI != OE; ++OI)
                    if (isInterestingPointer(*OI))
                        Pointers.insert(*OI);
            }
        }

        using PLS = boaaa::EvaluationContainer::PLS;

        storage.insert({ F.getGUID(), std::unique_ptr<boaaa::EvaluationContainer>(
            new boaaa::EvaluationContainer(Pointers.size(), Loads.size(), Stores.size(), Calls.size())) });
        boaaa::EvaluationContainer& c = *storage[F.getGUID()];
        using pvec = std::vector<LLVMValue*>;

        pvec vec_p = Pointers.takeVector();
        pvec vec_l = Loads.takeVector();
        pvec vec_s = Stores.takeVector();
        LLVMSmallVector<LLVMCallUnifyer*, 16> vec_c = Calls.takeVector();


        assert((c.store(PLS::P, vec_p.begin(), vec_p.end())));
        assert((c.store(PLS::L, vec_l.begin(), vec_l.end())));
        assert((c.store(PLS::S, vec_s.begin(), vec_s.end())));
        assert((c.storeCalls(vec_c.begin(), vec_c.end())));

#if LLVM_VERSION < 80
        //deletes all pointers before array gets destroyed, because autodelete gets destroyed before Calls,
        //only needed in 71 and earlier because in newer version Calls get casted and not created new. 
        c.autodeleter = new boaaa::support::AutoDeleter<LLVMCallUnifyer**>(c.calls, [num = c.num_calls](LLVMCallUnifyer** calls) { for (int i = 0; i < num; i++) delete calls[i]; });
#endif
    }
}

void EvaluationPassImpl::evaluateAAResultOnModule(LLVMModule& M, LLVMAAResults& AAResult, evaluation_storage& storage)
{
    //no checkInitSets call, becuase unneeded, call always evaluateAAResultOnFunction
    for (LLVMFunction& F : M)
        evaluateAAResultOnFunction(F, AAResult, *storage[F.getGUID()]);
}

void EvaluationPassImpl::evaluateAAResultOnFunction(LLVMFunction& F, LLVMAAResults& AAResult, boaaa::EvaluationContainer& container)
{
    checkInitSets();
    unionfind_map* aa_set = initAASet(F.getGUID());
    unionfind_map* no_aa_set = initNoAASets(F.getGUID());
    using namespace llvm;
    llvm::DataLayout DL = F.getParent()->getDataLayout();

    size_t pointer_offset = 0;
    size_t load_offset = container.num_pointers;
    size_t store_offset = container.num_pointers + container.num_loads;

    ++FunctionCount;

    // iterate over the worklist, and run the full (n^2)/2 disambiguations 
    for (int y = 0; y < container.num_pointers; y++) {
        LLVMValue* I1 = container.pointers[y];
#if LLVM_VERSION < 80
        uint64_t I1Size = LLVMMemoryLocation::UnknownSize;
        LLVMType* I1ElTy = cast<PointerType>(I1->getType())->getElementType();
        if (I1ElTy->isSized()) I1Size = DL.getTypeStoreSize(I1ElTy);
#else 
        auto I1Size = LLVMLocationSize::unknown();
        LLVMType* I1ElTy = cast<PointerType>(I1->getType())->getElementType();
        if (I1ElTy->isSized())
            I1Size = LLVMLocationSize::precise(DL.getTypeStoreSize(I1ElTy));
#endif
        for (int x = 0; x < y; x++) {
            LLVMValue* I2 = container.pointers[x];
#if LLVM_VERSION < 80
            uint64_t I2Size = LLVMMemoryLocation::UnknownSize;
            LLVMType* I2ElTy = cast<PointerType>(I2->getType())->getElementType();
            if (I2ElTy->isSized()) I2Size = DL.getTypeStoreSize(I2ElTy);
#else
            auto I2Size = LLVMLocationSize::unknown();
            LLVMType* I2ElTy = cast<PointerType>(I2->getType())->getElementType();
            if (I2ElTy->isSized())
                I2Size = LLVMLocationSize::precise(DL.getTypeStoreSize(I2ElTy));
#endif
            timestamp start = begin();
            LLVMAliasResult AR = AAResult.alias(I1, I1Size, I2, I2Size);
            addAliasTime(start);
            switch (AR) {
            case NoAlias:
                ++NoAliasCount;
                no_aa_set->concat(pointer_offset + x, pointer_offset + y);
                break;
            case MayAlias:
                ++MayAliasCount;
                break;
            case PartialAlias:
                ++PartialAliasCount;
                aa_set->concat(pointer_offset + x, pointer_offset + y);
                break;
            case MustAlias:
                ++MustAliasCount;
                aa_set->concat(pointer_offset + x, pointer_offset + y);
                break;
            }
        }
    }

    // iterate over all pairs of load, store
    for (int y = 0; y < container.num_loads; ++y) {
        LLVMValue* load = container.loads[y];
        for (int x = 0; x < container.num_stores; ++x) {
            LLVMValue* store = container.stores[x];

            timestamp start = begin();
            LLVMAliasResult AR = AAResult.alias(LLVMMemoryLocation::get(cast<LLVMLoadInst>(load)),
                LLVMMemoryLocation::get(cast<LLVMStoreInst>(store)));
            addAliasTime(start);
            switch (AR) {
            case NoAlias:
                ++NoAliasCount;
                no_aa_set->concat(store_offset + x, load_offset + y);
                break;
            case MayAlias:
                ++MayAliasCount;
                break;
            case PartialAlias:
                ++PartialAliasCount;
                aa_set->concat(store_offset + x, load_offset + y);
                break;
            case MustAlias:
                ++MustAliasCount;
                aa_set->concat(store_offset + x, load_offset + y);
                break;
            }
        }
    }

    // iterate over all pairs of store, store
    for (int y = 0; y < container.num_stores; ++y) {
        LLVMValue* I1 = container.stores[y];
        for (int x = 0; x < y; ++x) {
            LLVMValue* I2 = container.stores[x];

            timestamp start = begin();
            LLVMAliasResult AR = AAResult.alias(LLVMMemoryLocation::get(cast<LLVMStoreInst>(I1)),
                LLVMMemoryLocation::get(cast<LLVMStoreInst>(I2)));
            addAliasTime(start);
            switch (AR) {
            case NoAlias:
                ++NoAliasCount;
                no_aa_set->concat(store_offset + x, store_offset + y);
                break;
            case MayAlias:
                ++MayAliasCount;
                break;
            case PartialAlias:
                ++PartialAliasCount;
                aa_set->concat(store_offset + x, store_offset + y);
                break;
            case MustAlias:
                ++MustAliasCount;
                aa_set->concat(store_offset + x, store_offset + y);
                break;
            }
        }
    }

    // Mod/ref alias analysis: compare all pairs of calls and values
    for (int y = 0; y < container.num_calls; ++y) {
        LLVMCallUnifyer* call = container.calls[y];
        for (int x = 0; x < container.num_pointers; ++x) {
            LLVMValue* pointer = container.pointers[x];
#if LLVM_VERSION < 80
            uint64_t Size = LLVMMemoryLocation::UnknownSize;
            LLVMType* ElTy = cast<PointerType>(pointer->getType())->getElementType();
            if (ElTy->isSized()) Size = DL.getTypeStoreSize(ElTy);

            timestamp start = begin();
            LLVMModRefInfo info = AAResult.getModRefInfo(llvm::ImmutableCallSite(call->getInstruction()), pointer, Size);
            addModRefTime(start);
            switch (info) {
#else 
            auto Size = LLVMLocationSize::unknown();
            LLVMType* ElTy = cast<PointerType>(pointer->getType())->getElementType();
            if (ElTy->isSized())
                Size = LLVMLocationSize::precise(DL.getTypeStoreSize(ElTy));

            timestamp start = begin();
            LLVMModRefInfo info = AAResult.getModRefInfo(call, pointer, Size);
            addModRefTime(start);
            switch (info) {
#endif
#if LLVM_VERSION < 60
            case MRI_NoModRef:
                ++NoModRefCount;
                break;
            case MRI_Mod:
                ++ModCount;
                break;
            case MRI_Ref:
                ++RefCount;
                break;
            case MRI_ModRef:
                ++ModRefCount;
                break;
            }
        }

#else
            case ModRefInfo::NoModRef:
                ++NoModRefCount;
                break;
            case ModRefInfo::Mod:
                ++ModCount;
                break;
            case ModRefInfo::Ref:
                ++RefCount;
                break;
            case ModRefInfo::ModRef:
                ++ModRefCount;
                break;
            case ModRefInfo::Must:
                ++MustCount;
                break;
            case ModRefInfo::MustMod:
                ++MustModCount;
                break;
            case ModRefInfo::MustRef:
                ++MustRefCount;
                break;
            case ModRefInfo::MustModRef:
                ++MustModRefCount;
                break;
            }
        }
#endif
    }
    

    // Mod/ref alias analysis: compare all pairs of calls
    for (int y = 0; y < container.num_calls; ++y) {
        LLVMCallUnifyer* CallA = container.calls[y];
        for (int x = 0; x < container.num_calls; ++x) {
            if (y == x)
                continue;
            LLVMCallUnifyer* CallB = container.calls[x];

            timestamp start = begin();
            LLVMModRefInfo info =
#if LLVM_VERSION < 80
                AAResult.getModRefInfo(*CallA, *CallB);
#else
                AAResult.getModRefInfo(CallA, CallB);
#endif
            addModRefTime(start);
            switch (info) {
#if LLVM_VERSION < 60
            case MRI_NoModRef:
                ++NoModRefCount;
                break;
            case MRI_Mod:
                ++ModCount;
                break;
            case MRI_Ref:
                ++RefCount;
                break;
            case MRI_ModRef:
                ++ModRefCount;
                break;
#else 
            case ModRefInfo::NoModRef:
                ++NoModRefCount;
                break;
            case ModRefInfo::Mod:
                ++ModCount;
                break;
            case ModRefInfo::Ref:
                ++RefCount;
                break;
            case ModRefInfo::ModRef:
                ++ModRefCount;
                break;
            case ModRefInfo::Must:
                ++MustCount;
                break;
            case ModRefInfo::MustMod:
                ++MustModCount;
                break;
            case ModRefInfo::MustRef:
                ++MustRefCount;
                break;
            case ModRefInfo::MustModRef:
                ++MustModRefCount;
                break;
#endif
            }
        }
    }
}

void EvaluationPassImpl::printResult(std::ostream &stream) {

    uint64_t sum_alias = 0, sum_alias_head = 0, sum_no_alias = 0, sum_no_alias_head = 0;
    uint64_t sum_alias_squared = 0, sum_no_alias_squared = 0;

    for (_raw_type_inst(alias_set)::iterator it = alias_set->begin(), end = alias_set->end(); it != end; ++it)
    {
        sum_alias_head += it->second->headssize();
        for (_raw_type_inst(it->second->heads()) it2 = it->second->heads(), end2 = it->second->headsend(); it2 != end2; ++it2)
        {
            size_t size = (*it2)->size();
            sum_alias += size;
            sum_alias_squared += size * size;
        }

#ifdef DEBUG
        size_t check_num = 0;

        for (_raw_type_inst(it->second->begin()) it2 = it->second->begin(), end2 = it->second->end(); it2 != end2; ++it2)
        {
            if (it2->second->parent() == it2->second) {
                check_num++;
            }
        }

//        assert((check_num == it->second->headssize()));
#endif
    }

    for (_raw_type_inst(no_alias_set)::iterator it = no_alias_set->begin(), end = no_alias_set->end(); it != end; ++it)
    {
        sum_no_alias_head += it->second->headssize();
        for (_raw_type_inst(it->second->heads()) it2 = it->second->heads(), end2 = it->second->headsend(); it2 != end2; ++it2)
        {
            size_t size = (*it2)->size();
            sum_no_alias += size;
            sum_no_alias_squared += size * size;
        }

#ifdef DEBUG
        size_t check_num = 0;

        for (_raw_type_inst(it->second->begin()) it2 = it->second->begin(), end2 = it->second->end(); it2 != end2; ++it2)
        {
            if (it2->second->parent() == it2->second) {
                check_num++;
            }
        }

 //       assert((check_num == it->second->headssize()));
#endif
    }

    stream << "Alias Time    : " << m_seconds_alias << "," << (m_millis_alias < 100 ? "0" : "") << +(m_millis_alias < 10 ? "0" : "") << (int)m_millis_alias
                                                    << "." << (m_micros_alias < 100 ? "0" : "") << +(m_micros_alias < 10 ? "0" : "") << (int)m_micros_alias
                                                    << "." << (m_nanos_alias  < 100 ? "0" : "") << +(m_nanos_alias  < 10 ? "0" : "") << (int)m_nanos_alias << "\n";

    stream << "ModRef Time   : " << m_seconds_modref << "," << (m_millis_modref < 100 ? "0" : "") << +(m_millis_modref < 10 ? "0" : "") << (int)m_millis_modref
                                                     << "." << (m_micros_modref < 100 ? "0" : "") << +(m_micros_modref < 10 ? "0" : "") << (int)m_micros_modref
                                                     << "." << (m_nanos_modref  < 100 ? "0" : "") << +(m_nanos_modref  < 10 ? "0" : "") << (int)m_nanos_modref << "\n";
    stream << "=========================================\n";

    uint64_t AliasSum = NoAliasCount + MayAliasCount + PartialAliasCount + MustAliasCount;

    double mean_alias        = (double)sum_alias / (double)sum_alias_head;
    double variance_alias    = ((double)(sum_alias_squared - sum_alias)) / (double)sum_alias_head;
    double mean_no_alias     = (double)sum_no_alias / (double)sum_no_alias_head;
    double variance_no_alias = ((double)(sum_alias_squared - sum_no_alias)) / (double)sum_no_alias_head;

    if (AliasSum == 0) {
        stream << "AliasSum = 0 ...skipping\n";
        return;
    }
    stream << "No Alias      : " << formatPercentage(NoAliasCount, AliasSum) << " " << NoAliasCount << "\n";
    stream << "May Alias     : " << formatPercentage(MayAliasCount, AliasSum) << " " << MayAliasCount << "\n";
    stream << "Partial Alias : " << formatPercentage(PartialAliasCount, AliasSum) << " " << PartialAliasCount << "\n";
    stream << "Must Alias    : " << formatPercentage(MustAliasCount, AliasSum) << " " << MustAliasCount << "\n";
    stream << "-----------------------------------------\n";
    stream << "AliasSum      : " << "         " << " " << AliasSum << "\n";

    stream << "=========================================\n";

    uint64_t ModRefSum = NoModRefCount + RefCount + ModCount + ModRefCount + MustCount + MustRefCount + MustModCount + MustModRefCount;

    if (ModRefSum == 0) {
        stream << "ModRefSum = 0 ...skipping\n";
        return;
    }

    stream << "No ModRef     : " << formatPercentage(NoModRefCount, ModRefSum) << " " << NoModRefCount << "\n";
    stream << "Mod           : " << formatPercentage(ModCount, ModRefSum) << " " << ModCount << "\n";
    stream << "Ref           : " << formatPercentage(RefCount, ModRefSum) << " " << RefCount << "\n";
    stream << "ModRef        : " << formatPercentage(ModRefCount, ModRefSum) << " " << ModRefCount << "\n";
    stream << "Must          : " << formatPercentage(MustCount, ModRefSum) << " " << MustCount << "\n";
    stream << "MustMod       : " << formatPercentage(MustModCount, ModRefSum) << " " << MustModCount << "\n";
    stream << "MustRef       : " << formatPercentage(MustRefCount, ModRefSum) << " " << MustRefCount << "\n";
    stream << "MustModRef    : " << formatPercentage(MustModRefCount, ModRefSum) << " " << MustModRefCount << "\n";
    stream << "-----------------------------------------\n";
    stream << "ModRefSum     : " << "         " << " " << ModRefSum << "\n";

    stream << "=========================================\n";

    stream << "Alias Sets    : " << "         " << " " << sum_alias_head << "\n";
    stream << "mean, var     : " << mean_alias << ", " << variance_alias << "\n";
    stream << "No Alias Sets : " << "         " << " " << sum_no_alias_head << "\n";
    stream << "mean, var     : " << mean_no_alias << ", " << variance_no_alias << "\n";
    stream << "\n";
}

void EvaluationPassImpl::printToEvalRes(EvaluationResult& er)
{
    uint64_t sum_alias = 0, sum_alias_head = 0, sum_no_alias = 0, sum_no_alias_head = 0;
    uint64_t sum_alias_squared = 0, sum_no_alias_squared = 0;

    for (_raw_type_inst(alias_set)::iterator it = alias_set->begin(), end = alias_set->end(); it != end; ++it)
    {
        sum_alias_head += it->second->headssize();
        for (_raw_type_inst(it->second->heads()) it2 = it->second->heads(), end2 = it->second->headsend(); it2 != end2; ++it2)
        {
            size_t size = (*it2)->size();
            sum_alias += size;
            sum_alias_squared += size * size;
        }

#ifdef DEBUG
        size_t check_num = 0;

        for (_raw_type_inst(it->second->begin()) it2 = it->second->begin(), end2 = it->second->end(); it2 != end2; ++it2)
        {
            if (it2->second->parent() == it2->second) {
                check_num++;
            }
        }

//        assert((check_num == it->second->headssize()));
#endif
    }

    for (_raw_type_inst(no_alias_set)::iterator it = no_alias_set->begin(), end = no_alias_set->end(); it != end; ++it)
    {
        sum_no_alias_head += it->second->headssize();
        for (_raw_type_inst(it->second->heads()) it2 = it->second->heads(), end2 = it->second->headsend(); it2 != end2; ++it2)
        {
            size_t size = (*it2)->size();
            sum_no_alias += size;
            sum_no_alias_squared += size * size;
        }

#ifdef DEBUG
        size_t check_num = 0;

        for (_raw_type_inst(it->second->begin()) it2 = it->second->begin(), end2 = it->second->end(); it2 != end2; ++it2)
        {
            if (it2->second->parent() == it2->second) {
                check_num++;
            }
        }

//       assert((check_num == it->second->headssize()));
#endif
    }

    uint64_t AliasSum = NoAliasCount + MayAliasCount + PartialAliasCount + MustAliasCount;

    double mean_alias = (double)sum_alias / (double)sum_alias_head;
    double variance_alias = ((double)(sum_alias_squared - sum_alias)) / (double)sum_alias_head;
    double mean_no_alias = (double)sum_no_alias / (double)sum_no_alias_head;
    double variance_no_alias = ((double)(sum_alias_squared - sum_no_alias)) / (double)sum_no_alias_head;

    uint64_t ModRefSum = NoModRefCount + RefCount + ModCount + ModRefCount + MustCount + MustRefCount + MustModCount + MustModRefCount;

    //time
    er.set_alias_time_seconds(m_seconds_alias);
    er.set_alias_time_millis(m_millis_alias);
    er.set_alias_time_micros(m_micros_alias);
    er.set_alias_time_nanos(m_nanos_alias);

    er.set_modref_time_seconds(m_seconds_modref);
    er.set_modref_time_millis(m_millis_modref);
    er.set_modref_time_micros(m_millis_modref);
    er.set_modref_time_nanos(m_nanos_modref);

    //alias
    er.set_alias_sum(AliasSum);
    er.set_no_alias_count(NoAliasCount);
    er.set_may_alias_count(MayAliasCount);
    er.set_partial_alias_count(PartialAliasCount);
    er.set_must_alias_count(MustAliasCount);

    //modref
    er.set_modref_sum(ModRefSum);
    er.set_no_modref_count(NoModRefCount);
    er.set_mod_count(ModCount);
    er.set_ref_count(RefCount);
    er.set_modref_count(ModRefCount);
    er.set_must_count(MustCount);
    er.set_must_mod_count(MustModCount);
    er.set_must_ref_count(MustRefCount);
    er.set_must_modref_count(MustModRefCount);
   
    //alias set
    er.set_alias_sets(sum_alias_head);
    er.set_mean_alias_sets(mean_alias);
    er.set_var_alias_sets(variance_alias);

    er.set_no_alias_sets(sum_no_alias_head);
    er.set_mean_no_alias_sets(mean_no_alias);
    er.set_var_no_alias_sets(variance_no_alias);
}