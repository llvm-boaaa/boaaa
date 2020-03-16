#include "boaaa/lv/EvaluationPass.h"

using namespace boaaa;

bool isInterestingPointer(llvm::Value* V) {
    return V->getType()->isPointerTy()
        && !llvm::isa<llvm::ConstantPointerNull>(V);
}

std::string formatProzent(uint64_t count, uint64_t total) {
    std::string out = "[XXX.XX%]";
    out[1] = '0' + (count / total);
    out[2] = '0' + ((count * 10LL / total) % 10);
    out[3] = '0' + ((count * 100LL / total) % 10);

    out[5] = '0' + ((count * 1000LL / total) % 10);
    out[6] = '0' + ((count * 10000LL / total) % 10);
    return out;
}

void EvaluationPassImpl::evaluateAAResultOnModule(LLVMModule& M, llvm::AAResults &AAResult) {
    //TODO remove
#ifdef LLVM_VERSION_90
    using namespace llvm;
    llvm::DataLayout DL = M.getDataLayout();

    for (LLVMFunction& F : M)
    {
        LLVMSetVector<Value*> Pointers;
        LLVMSmallSetVector<CallBase*, 16> Calls; //llvm 50 CallInst
        LLVMSetVector<Value*> Loads;
        LLVMSetVector<Value*> Stores;

        FunctionCount++;

        //argument pointes
        for (auto& I : F.args())
            if (I.getType()->isPointerTy())
                Pointers.insert(&I);

        for (auto I = inst_begin(F), E = inst_end(F); I != E; ++I) {
            if (I->getType()->isPointerTy()) // Add all pointer instructions.
                Pointers.insert(&*I);
            if (isa<LoadInst>(&*I))
                Loads.insert(&*I);
            if (isa<StoreInst>(&*I))
                Stores.insert(&*I);
            Instruction& Inst = *I;
            if (auto* Call = dyn_cast<CallBase>(&Inst)) {
                Value* Callee = Call->getCalledValue();
                // Skip actual functions for direct function calls.
                if (!isa<LLVMFunction>(Callee) && isInterestingPointer(Callee))
                    Pointers.insert(Callee);
                // Consider formals.
                for (Use& DataOp : Call->data_ops())
                    if (isInterestingPointer(DataOp))
                        Pointers.insert(DataOp);
                Calls.insert(Call);
            }
            else {
                // Consider all operands.
                for (Instruction::op_iterator OI = Inst.op_begin(), OE = Inst.op_end();
                    OI != OE; ++OI)
                    if (isInterestingPointer(*OI))
                        Pointers.insert(*OI);
            }
        }

        // iterate over the worklist, and run the full (n^2)/2 disambiguations
        for (SetVector<Value*>::iterator I1 = Pointers.begin(), E = Pointers.end();
            I1 != E; ++I1) {
            auto I1Size = LocationSize::unknown(); //MemoryLocation
            Type* I1ElTy = cast<PointerType>((*I1)->getType())->getElementType();
            if (I1ElTy->isSized())
                I1Size = LocationSize::precise(DL.getTypeStoreSize(I1ElTy));

            for (SetVector<Value*>::iterator I2 = Pointers.begin(); I2 != I1; ++I2) {
                auto I2Size = LocationSize::unknown();
                Type* I2ElTy = cast<PointerType>((*I2)->getType())->getElementType();
                if (I2ElTy->isSized())
                    I2Size = LocationSize::precise(DL.getTypeStoreSize(I2ElTy));

                AliasResult AR = AAResult.alias(*I1, I1Size, *I2, I2Size);
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
        for (Value* Load : Loads) {
            for (Value* Store : Stores) {
                AliasResult AR = AAResult.alias(MemoryLocation::get(cast<LoadInst>(Load)),
                    MemoryLocation::get(cast<StoreInst>(Store)));
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
        for (SetVector<Value*>::iterator I1 = Stores.begin(), E = Stores.end();
            I1 != E; ++I1) {
            for (SetVector<Value*>::iterator I2 = Stores.begin(); I2 != I1; ++I2) {
                AliasResult AR = AAResult.alias(MemoryLocation::get(cast<StoreInst>(*I1)),
                    MemoryLocation::get(cast<StoreInst>(*I2)));
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
        for (CallBase* Call : Calls) {
            for (auto Pointer : Pointers) {
                auto Size = LocationSize::unknown();
                Type* ElTy = cast<PointerType>(Pointer->getType())->getElementType();
                if (ElTy->isSized())
                    Size = LocationSize::precise(DL.getTypeStoreSize(ElTy));

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
        }

        // Mod/ref alias analysis: compare all pairs of calls
        for (CallBase* CallA : Calls) {
            for (CallBase* CallB : Calls) {
                if (CallA == CallB)
                    continue;
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
                }
            }
        }
    }
#endif
}

void EvaluationPassImpl::evaluateAAResultOnFunction(LLVMFunction& F, llvm::AAResults& AAResult)
{
#ifdef LLVM_VERSION_90
    using namespace llvm;
    llvm::DataLayout DL = F.getParent()->getDataLayout();

    LLVMSetVector<Value*> Pointers;
    LLVMSmallSetVector<CallBase*, 16> Calls;
    LLVMSetVector<Value*> Loads;
    LLVMSetVector<Value*> Stores;

    ++FunctionCount;

    //argument pointes
    for (auto& I : F.args())
        if (I.getType()->isPointerTy())
            Pointers.insert(&I);

    for (auto I = inst_begin(F), E = inst_end(F); I != E; ++I) {
        if (I->getType()->isPointerTy()) // Add all pointer instructions.
            Pointers.insert(&*I);
        if (isa<LoadInst>(&*I))
            Loads.insert(&*I);
        if (isa<StoreInst>(&*I))
            Stores.insert(&*I);
        Instruction& Inst = *I;
        if (auto* Call = dyn_cast<CallBase>(&Inst)) {
            Value* Callee = Call->getCalledValue();
            // Skip actual functions for direct function calls.
            if (!isa<Function>(Callee) && isInterestingPointer(Callee))
                Pointers.insert(Callee);
            // Consider formals.
            for (Use& DataOp : Call->data_ops())
                if (isInterestingPointer(DataOp))
                    Pointers.insert(DataOp);
            Calls.insert(Call);
        }
        else {
            // Consider all operands.
            for (Instruction::op_iterator OI = Inst.op_begin(), OE = Inst.op_end();
                OI != OE; ++OI)
                if (isInterestingPointer(*OI))
                    Pointers.insert(*OI);
        }
    }

    // iterate over the worklist, and run the full (n^2)/2 disambiguations
    for (SetVector<Value*>::iterator I1 = Pointers.begin(), E = Pointers.end();
        I1 != E; ++I1) {
        auto I1Size = LocationSize::unknown();
        Type* I1ElTy = cast<PointerType>((*I1)->getType())->getElementType();
        if (I1ElTy->isSized())
            I1Size = LocationSize::precise(DL.getTypeStoreSize(I1ElTy));

        for (SetVector<Value*>::iterator I2 = Pointers.begin(); I2 != I1; ++I2) {
            auto I2Size = LocationSize::unknown();
            Type* I2ElTy = cast<PointerType>((*I2)->getType())->getElementType();
            if (I2ElTy->isSized())
                I2Size = LocationSize::precise(DL.getTypeStoreSize(I2ElTy));

            AliasResult AR = AAResult.alias(*I1, I1Size, *I2, I2Size);
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
    for (Value* Load : Loads) {
        for (Value* Store : Stores) {
            AliasResult AR = AAResult.alias(MemoryLocation::get(cast<LoadInst>(Load)),
                MemoryLocation::get(cast<StoreInst>(Store)));
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
    for (SetVector<Value*>::iterator I1 = Stores.begin(), E = Stores.end();
        I1 != E; ++I1) {
        for (SetVector<Value*>::iterator I2 = Stores.begin(); I2 != I1; ++I2) {
            AliasResult AR = AAResult.alias(MemoryLocation::get(cast<StoreInst>(*I1)),
                MemoryLocation::get(cast<StoreInst>(*I2)));
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
    for (CallBase* Call : Calls) {
        for (auto Pointer : Pointers) {
            auto Size = LocationSize::unknown();
            Type* ElTy = cast<PointerType>(Pointer->getType())->getElementType();
            if (ElTy->isSized())
                Size = LocationSize::precise(DL.getTypeStoreSize(ElTy));

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
    }

    // Mod/ref alias analysis: compare all pairs of calls
    for (CallBase* CallA : Calls) {
        for (CallBase* CallB : Calls) {
            if (CallA == CallB)
                continue;
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
            }
        }
    }
#endif
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
    stream << "No Alias     : " << formatProzent(NoAliasCount, AliasSum) << " " << NoAliasCount << "\n";
    stream << "May Alias    : " << formatProzent(MayAliasCount, AliasSum) << " " << MayAliasCount << "\n";
    stream << "Partial Alias: " << formatProzent(PartialAliasCount, AliasSum) << " " << PartialAliasCount << "\n";
    stream << "Must Alias   : " << formatProzent(MustAliasCount, AliasSum) << " " << MustAliasCount << "\n";
    stream << "\n";

    int64_t ModRefSum = NoModRefCount + RefCount + ModCount + ModRefCount + MustCount + MustRefCount + MustModCount + MustModRefCount;

    stream << "ModRefSum    : " << "         " << " " << ModRefSum << "\n";
    stream << "\n";
    stream << "No ModRef    : " << formatProzent(NoModRefCount, ModRefSum) << " " << NoModRefCount << "\n";
    stream << "Mod          : " << formatProzent(ModCount, ModRefSum) << " " << ModCount << "\n";
    stream << "Ref          : " << formatProzent(RefCount, ModRefSum) << " " << RefCount << "\n";
    stream << "ModRef       : " << formatProzent(ModRefCount, ModRefSum) << " " << ModRefCount << "\n";
    stream << "Must         : " << formatProzent(MustCount, ModRefSum) << " " << MustCount << "\n";
    stream << "MustMod      : " << formatProzent(MustModCount, ModRefSum) << " " << MustModCount << "\n";
    stream << "MustRef      : " << formatProzent(MustRefCount, ModRefSum) << " " << MustRefCount << "\n";
    stream << "MustModRef   : " << formatProzent(MustModRefCount, ModRefSum) << " " << MustModRefCount << "\n";
}