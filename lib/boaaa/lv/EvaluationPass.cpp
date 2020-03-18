#include "boaaa/lv/EvaluationPass.h"

using namespace boaaa;

bool isInterestingPointer(LLVMValue* V) {
    return V->getType()->isPointerTy()
        && !llvm::isa<llvm::ConstantPointerNull>(V);
}

std::string formatPercentage(uint64_t count, uint64_t total) {
    std::string out = "[XXX.XX%]";
    out[1] = '0' + (count / total);
    out[2] = '0' + ((count * 10LL / total) % 10);
    out[3] = '0' + ((count * 100LL / total) % 10);

    out[5] = '0' + ((count * 1000LL / total) % 10);
    out[6] = '0' + ((count * 10000LL / total) % 10);
    return out;
}

void EvaluationPassImpl::evaluateAAResultOnModule(LLVMModule& M, LLVMAAResults &AAResult) {
    for (LLVMFunction& F : M)
        evaluateAAResultOnFunction(F, AAResult);                 
}

void EvaluationPassImpl::evaluateAAResultOnFunction(LLVMFunction& F, LLVMAAResults& AAResult)
{
    using namespace llvm;
    llvm::DataLayout DL = F.getParent()->getDataLayout();

    LLVMSetVector<LLVMValue*> Pointers;
    LLVMSmallSetVector<LLVMCallUnifyer*, 16> Calls;
#if LLVM_VERSION < 80
    //deletes all pointers before vector gets destroyed, because autodelete gets destroyed before Calls
    boaaa::support::AutoDeleter<LLVMSmallSetVector<LLVMCallUnifyer*, 16>> 
           autodelete(Calls, [](LLVMSmallSetVector<LLVMCallUnifyer*, 16> v) {for (LLVMCallUnifyer* p : v) delete p; });
#endif
    LLVMSetVector<LLVMValue*> Loads;
    LLVMSetVector<LLVMValue*> Stores;

    ++FunctionCount;

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
        if (auto Call = new CallSite(&Inst)) {
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
    
#if LLVM_VERSION < 80
    for (SetVector<Value*>::iterator I1 = Pointers.begin(), E = Pointers.end();
        I1 != E; ++I1) {
        uint64_t I1Size = MemoryLocation::UnknownSize;
        Type* I1ElTy = cast<PointerType>((*I1)->getType())->getElementType();
        if (I1ElTy->isSized()) I1Size = DL.getTypeStoreSize(I1ElTy);

        for (SetVector<Value*>::iterator I2 = Pointers.begin(); I2 != I1; ++I2) {
            uint64_t I2Size = MemoryLocation::UnknownSize;
            Type* I2ElTy = cast<PointerType>((*I2)->getType())->getElementType();
            if (I2ElTy->isSized()) I2Size = DL.getTypeStoreSize(I2ElTy);
#else
    // iterate over the worklist, and run the full (n^2)/2 disambiguations
    for (LLVMSetVector<LLVMValue*>::iterator I1 = Pointers.begin(), E = Pointers.end();
        I1 != E; ++I1) {
        auto I1Size = LLVMLocationSize::unknown();
        LLVMType* I1ElTy = cast<PointerType>((*I1)->getType())->getElementType();
        if (I1ElTy->isSized())
            I1Size = LLVMLocationSize::precise(DL.getTypeStoreSize(I1ElTy));

        for (LLVMSetVector<LLVMValue*>::iterator I2 = Pointers.begin(); I2 != I1; ++I2) {
            auto I2Size = LLVMLocationSize::unknown();
            LLVMType* I2ElTy = cast<PointerType>((*I2)->getType())->getElementType();
            if (I2ElTy->isSized())
                I2Size = LLVMLocationSize::precise(DL.getTypeStoreSize(I2ElTy));
#endif
            LLVMAliasResult AR = AAResult.alias(*I1, I1Size, *I2, I2Size);
            switch (AR) {
            case NoAlias:
                ++NoAliasCount;
                break;
            case MayAlias:
                ++MayAliasCount;
                break;
            case PartialAlias:
                ++PartialAliasCount;
                break;
            case MustAlias:
                ++MustAliasCount;
                break;
            }
        }
    }

    // iterate over all pairs of load, store
    for (LLVMValue* Load : Loads) {
        for (LLVMValue* Store : Stores) {
            LLVMAliasResult AR = AAResult.alias(LLVMMemoryLocation::get(cast<LoadInst>(Load)),
                LLVMMemoryLocation::get(cast<StoreInst>(Store)));
            switch (AR) {
            case NoAlias:
                ++NoAliasCount;
                break;
            case MayAlias:
                ++MayAliasCount;
                break;
            case PartialAlias:
                ++PartialAliasCount;
                break;
            case MustAlias:
                ++MustAliasCount;
                break;
            }
        }
    }

    // iterate over all pairs of store, store
    for (LLVMSetVector<LLVMValue*>::iterator I1 = Stores.begin(), E = Stores.end();
        I1 != E; ++I1) {
        for (LLVMSetVector<LLVMValue*>::iterator I2 = Stores.begin(); I2 != I1; ++I2) {
            LLVMAliasResult AR = AAResult.alias(LLVMMemoryLocation::get(cast<StoreInst>(*I1)),
                LLVMMemoryLocation::get(cast<StoreInst>(*I2)));
            switch (AR) {
            case NoAlias:
                ++NoAliasCount;
                break;
            case MayAlias:
                ++MayAliasCount;
                break;
            case PartialAlias:
                ++PartialAliasCount;
                break;
            case MustAlias:
                ++MustAliasCount;
                break;
            }
        }
    }

    // Mod/ref alias analysis: compare all pairs of calls and values
    for (LLVMCallUnifyer* Call : Calls) {
#if LLVM_VERSION < 80
        for (auto Pointer : Pointers) {
            uint64_t Size = MemoryLocation::UnknownSize;
            Type* ElTy = cast<PointerType>(Pointer->getType())->getElementType();
            if (ElTy->isSized()) Size = DL.getTypeStoreSize(ElTy);

            switch (AAResult.getModRefInfo(llvm::ImmutableCallSite(Call->getInstruction()), Pointer, Size)) {
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
        for (auto Pointer : Pointers) {
            auto Size = LLVMLocationSize::unknown();
            LLVMType* ElTy = cast<PointerType>(Pointer->getType())->getElementType();
            if (ElTy->isSized())
                Size = LLVMLocationSize::precise(DL.getTypeStoreSize(ElTy));
            switch (AAResult.getModRefInfo(Call, Pointer, Size)) {
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
    for (auto CallA : Calls) {
        for (auto CallB : Calls) {
            if (CallA == CallB)
                continue;
#if LLVM_VERSION < 80
                switch (AAResult.getModRefInfo(*CallA, *CallB)) {
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
            switch (AAResult.getModRefInfo(CallA, CallB)) {
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
    uint64_t AliasSum = NoAliasCount + MayAliasCount + PartialAliasCount + MustAliasCount;

    stream << "AliasEvaluationPass Report:\n";

    if (AliasSum <= 0) {
        stream << "AliasSum = 0 ...skipping\n";
        return;
    }
    stream << "AliasSum     : " << "         " << " " << AliasSum << "\n";
    stream << "\n";
    stream << "No Alias     : " << formatPercentage(NoAliasCount, AliasSum) << " " << NoAliasCount << "\n";
    stream << "May Alias    : " << formatPercentage(MayAliasCount, AliasSum) << " " << MayAliasCount << "\n";
    stream << "Partial Alias: " << formatPercentage(PartialAliasCount, AliasSum) << " " << PartialAliasCount << "\n";
    stream << "Must Alias   : " << formatPercentage(MustAliasCount, AliasSum) << " " << MustAliasCount << "\n";
    stream << "\n";

    int64_t ModRefSum = NoModRefCount + RefCount + ModCount + ModRefCount + MustCount + MustRefCount + MustModCount + MustModRefCount;

    stream << "ModRefSum    : " << "         " << " " << ModRefSum << "\n";
    stream << "\n";
    stream << "No ModRef    : " << formatPercentage(NoModRefCount, ModRefSum) << " " << NoModRefCount << "\n";
    stream << "Mod          : " << formatPercentage(ModCount, ModRefSum) << " " << ModCount << "\n";
    stream << "Ref          : " << formatPercentage(RefCount, ModRefSum) << " " << RefCount << "\n";
    stream << "ModRef       : " << formatPercentage(ModRefCount, ModRefSum) << " " << ModRefCount << "\n";
    stream << "Must         : " << formatPercentage(MustCount, ModRefSum) << " " << MustCount << "\n";
    stream << "MustMod      : " << formatPercentage(MustModCount, ModRefSum) << " " << MustModCount << "\n";
    stream << "MustRef      : " << formatPercentage(MustRefCount, ModRefSum) << " " << MustRefCount << "\n";
    stream << "MustModRef   : " << formatPercentage(MustModRefCount, ModRefSum) << " " << MustModRefCount << "\n";
}