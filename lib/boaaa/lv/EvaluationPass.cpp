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
    evaluated = false;

    checkInitSets();
    unionfind_map* aa_set = initAASet(F.getGUID());
    unionfind_map* ex_aa_set = initEXAASet(F.getGUID());
    evaluation_sets* no_aa_set = initNoAASets(F.getGUID(), container.sum_all);
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
            size_t valX = pointer_offset + x;
            size_t valY = pointer_offset + y;
            switch (AR) {
            case NoAlias:
                ++NoAliasCount;
                //add valX/valY add first, to assure it is the head how represent the set
                (*no_aa_set)[valX]->concat(valX, valY);
                (*no_aa_set)[valY]->concat(valY, valX);
                break;
            case MayAlias:
                ++MayAliasCount;
                ex_aa_set->concat(valX, valY);
                break;
            case PartialAlias:
                ++PartialAliasCount;
                aa_set->concat(valX, valY);
                ex_aa_set->concat(valX, valY);
                break;
            case MustAlias:
                ++MustAliasCount;
                aa_set->concat(valX, valY);
                ex_aa_set->concat(valX, valY);
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
            size_t valX = store_offset + x;
            size_t valY = load_offset + y;
            switch (AR) {
            case NoAlias:
                ++NoAliasCount;
                //add valX/valY add first, to assure it is the head how represent the set
                (*no_aa_set)[valX]->concat(valX, valY);
                (*no_aa_set)[valY]->concat(valY, valX);
                break;
            case MayAlias:
                ++MayAliasCount;
                ex_aa_set->concat(valX, valY);
                break;
            case PartialAlias:
                ++PartialAliasCount;
                aa_set->concat(valX, valY);
                ex_aa_set->concat(valX, valY);
                break;
            case MustAlias:
                ++MustAliasCount;
                aa_set->concat(valX, valY);
                ex_aa_set->concat(valX, valY);
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
            size_t valX = store_offset + x;
            size_t valY = store_offset + y;
            switch (AR) {
            case NoAlias:
                ++NoAliasCount;
                //add valX/valY add first, to assure it is the head how represent the set
                (*no_aa_set)[valX]->concat(valX, valY);
                (*no_aa_set)[valY]->concat(valY, valX);
                break;
            case MayAlias:
                ++MayAliasCount;
                ex_aa_set->concat(valX, valY);
                break;
            case PartialAlias:
                ++PartialAliasCount;
                aa_set->concat(valX, valY);
                ex_aa_set->concat(valX, valY);
                break;
            case MustAlias:
                ++MustAliasCount;
                aa_set->concat(valX, valY);
                ex_aa_set->concat(valX, valY);
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

void EvaluationPassImpl::evaluateResult()
{
    uint64_t sum_alias = 0, sum_ex_alias = 0, sum_no_alias = 0;
    uint64_t sum_alias_squared = 0, sum_ex_alias_squared = 0, sum_no_alias_squared = 0;

    AliasSetCount = 0;
    NoAliasSetCount = 0;

    for (_raw_type_inst(alias_set)::iterator it = alias_set->begin(), end = alias_set->end(); it != end; ++it)
    {
        AliasSetCount += it->second->headssize();
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

    for (_raw_type_inst(ex_alias_set)::iterator it = ex_alias_set->begin(), end = ex_alias_set->end(); it != end; ++it)
    {
        ExAliasSetCount += it->second->headssize();
        for (_raw_type_inst(it->second->heads()) it2 = it->second->heads(), end2 = it->second->headsend(); it2 != end2; ++it2)
        {
            size_t size = (*it2)->size();
            sum_ex_alias += size;
            sum_ex_alias_squared += size * size;
        }
    }

    if (sum_alias <= 20000) {
        for (_raw_type_inst(no_alias_set)::iterator it = no_alias_set->begin(), end = no_alias_set->end(); it != end; ++it)
        {
            size_t num = it->second->size();
            std::vector<bool> check(num);
            for (int i = 0; i < num; i++)
                check[i] = true;

            evaluation_sets* set = it->second.get();
            for (int i = 0; i < num; i++)
            {
                //allready checked
                if (!check[i] || (*set)[i]->size() == 0) continue;
                assert((*set)[i]->headssize() == 1);
                std::set<size_t> ids;
                unionfind_map* map = (*set)[i].get();
                bool check_added = false;
                size_t size = map->size();

                for (unionfind_map::const_iterator it = map->begin(), end = map->end(); it != end; ++it)
                {
                    size_t id = it->second->value();
                    assert((*set)[id]->headssize() == 1);
                    unionfind_map* comp = (*set)[id].get();
                    if (comp->size() == map->size()) {
                        //compare
                        //fill one time if needed
                        if (ids.size() < map->size()) {
                            ids.clear();
                            for (unionfind_map::const_iterator iit = map->begin(), iend = map->end(); iit != iend; iit++)
                            {
                                ids.insert(iit->second->value());
                            }
                        }

                        bool check_all = true;
                        //check set[i]\i == set[id]\id
                        for (unionfind_map::const_iterator iit = comp->begin(), iend = comp->end(); iit != iend; iit++)
                        {
                            //skip own id
                            if ((*set)[id]->begin()->first == iit->second->value()) continue;

                            if (ids.find(iit->second->value()) == ids.end()) {
                                check_all = false;
                                break;
                            }
                        }

                        if (check_all) {
                            continue;
                        }

                        if (check[id])
                        {
                            check[id] = false;
                        }
                        else
                        {
                            //skip
                            check_added = true;
                        }
                    }
                }

                //already added
                if (check_added) continue;

                NoAliasSetCount++;
                sum_no_alias += size;
                sum_no_alias_squared += size * size;
            }
        }
    }

    MeanAlias = (double)sum_alias / (double)AliasSetCount;
    VarAlias = ((double)(sum_alias_squared - sum_alias)) / (double)AliasSetCount;
    MeanExAlias = (double)sum_ex_alias / (double)ExAliasSetCount;
    VarExAlias = ((double)(sum_ex_alias_squared - sum_ex_alias)) / (double)ExAliasSetCount;
    MeanNoAlias = (double)sum_no_alias / (double)NoAliasSetCount;
    VarNoAlias = ((double)(sum_alias_squared - sum_no_alias)) / (double)NoAliasSetCount;

    evaluated = true;
}

void EvaluationPassImpl::printResult(std::ostream &stream) {

    stream << "Alias Time    : " << m_seconds_alias << "," << (m_millis_alias < 100 ? "0" : "") << +(m_millis_alias < 10 ? "0" : "") << (int)m_millis_alias
                                                    << "." << (m_micros_alias < 100 ? "0" : "") << +(m_micros_alias < 10 ? "0" : "") << (int)m_micros_alias
                                                    << "." << (m_nanos_alias  < 100 ? "0" : "") << +(m_nanos_alias  < 10 ? "0" : "") << (int)m_nanos_alias << "\n";

    stream << "ModRef Time   : " << m_seconds_modref << "," << (m_millis_modref < 100 ? "0" : "") << +(m_millis_modref < 10 ? "0" : "") << (int)m_millis_modref
                                                     << "." << (m_micros_modref < 100 ? "0" : "") << +(m_micros_modref < 10 ? "0" : "") << (int)m_micros_modref
                                                     << "." << (m_nanos_modref  < 100 ? "0" : "") << +(m_nanos_modref  < 10 ? "0" : "") << (int)m_nanos_modref << "\n";
    stream << "=========================================\n";

    if (!evaluated)
        evaluateResult();

    uint64_t AliasSum = NoAliasCount + MayAliasCount + PartialAliasCount + MustAliasCount;

    if (AliasSum == 0) {
        stream << "AliasSum = 0 ...skipping\n";
        return;
    }
    stream << "No Alias      : " << formatPercentage(NoAliasCount, AliasSum)        << " " << NoAliasCount << "\n";
    stream << "May Alias     : " << formatPercentage(MayAliasCount, AliasSum)       << " " << MayAliasCount << "\n";
    stream << "Partial Alias : " << formatPercentage(PartialAliasCount, AliasSum)   << " " << PartialAliasCount << "\n";
    stream << "Must Alias    : " << formatPercentage(MustAliasCount, AliasSum)      << " " << MustAliasCount << "\n";
    stream << "-----------------------------------------\n";
    stream << "AliasSum      : " << "         " << " " << AliasSum << "\n";

    stream << "=========================================\n";

    uint64_t ModRefSum = NoModRefCount + RefCount + ModCount + ModRefCount + MustCount + MustRefCount + MustModCount + MustModRefCount;

    if (ModRefSum == 0) {
        stream << "ModRefSum = 0 ...skipping\n";
        return;
    }

    stream << "No ModRef     : " << formatPercentage(NoModRefCount, ModRefSum)      << " " << NoModRefCount << "\n";
    stream << "Mod           : " << formatPercentage(ModCount, ModRefSum)           << " " << ModCount << "\n";
    stream << "Ref           : " << formatPercentage(RefCount, ModRefSum)           << " " << RefCount << "\n";
    stream << "ModRef        : " << formatPercentage(ModRefCount, ModRefSum)        << " " << ModRefCount << "\n";
    stream << "Must          : " << formatPercentage(MustCount, ModRefSum)          << " " << MustCount << "\n";
    stream << "MustMod       : " << formatPercentage(MustModCount, ModRefSum)       << " " << MustModCount << "\n";
    stream << "MustRef       : " << formatPercentage(MustRefCount, ModRefSum)       << " " << MustRefCount << "\n";
    stream << "MustModRef    : " << formatPercentage(MustModRefCount, ModRefSum)    << " " << MustModRefCount << "\n";
    stream << "-----------------------------------------\n";
    stream << "ModRefSum     : " << "         " << " " << ModRefSum << "\n";

    stream << "=========================================\n";

    stream << "Alias Sets    : " << "         " << " "  << AliasSetCount << "\n";
    stream << "mean, var     : " << MeanAlias   << ", " << VarAlias << "\n";
    stream << "Ex. Alias Sets: " << "         " << " "  << ExAliasSetCount << "\n";
    stream << "mean, var     : " << MeanExAlias << ", " << VarExAlias << "\n";
    stream << "No Alias Sets : " << "         " << " "  << NoAliasSetCount << "\n";
    stream << "mean, var     : " << MeanNoAlias << ", " << VarNoAlias << "\n";
    stream << "\n";
}

void EvaluationPassImpl::printToEvalRes(EvaluationResult& er)
{
    if (!evaluated)
        evaluateResult();

    uint64_t AliasSum = NoAliasCount + MayAliasCount + PartialAliasCount + MustAliasCount;
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
    er.set_alias_sets(AliasSetCount);
    er.set_mean_alias_sets(MeanAlias);
    er.set_var_alias_sets(VarAlias);

    er.set_ex_alias_sets(ExAliasSetCount);
    er.set_mean_ex_alias_sets(MeanExAlias);
    er.set_var_ex_alias_sets(VarExAlias);

    er.set_no_alias_sets(NoAliasSetCount);
    er.set_mean_no_alias_sets(MeanNoAlias);
    er.set_var_no_alias_sets(VarNoAlias);
}