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

void AAResultEvaluationPassImpl::evaluateAAResult(std::unique_ptr<llvm::AAResults> AAResult, LLVMModule& M) {
    //TODO remove
    using namespace llvm;
    llvm::DataLayout DL = M.getDataLayout();

    int64_t FunctionCount;
    int64_t NoAliasCount, MayAliasCount, PartialAliasCount, MustAliasCount;
    int64_t NoModRefCount, ModCount, RefCount, ModRefCount;
    int64_t MustCount, MustRefCount, MustModCount, MustModRefCount;

    LLVMSetVector<Value*> Pointers;
    LLVMSmallSetVector<CallBase*, 16> Calls;
    LLVMSetVector<Value*> Loads;
    LLVMSetVector<Value*> Stores;


    for (LLVMFunction& F : M)
    {
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

                AliasResult AR = AAResult->alias(*I1, I1Size, *I2, I2Size);
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
                AliasResult AR = AAResult->alias(MemoryLocation::get(cast<LoadInst>(Load)),
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
                AliasResult AR = AAResult->alias(MemoryLocation::get(cast<StoreInst>(*I1)),
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

                switch (AAResult->getModRefInfo(Call, Pointer, Size)) {
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
                switch (AAResult->getModRefInfo(CallA, CallB)) {
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

    AAResult.reset();

    uint64_t AliasSum = NoAliasCount + MayAliasCount + PartialAliasCount + MustAliasCount;

    std::cout << "AliasEvaluationPass Report:\n";
    std::cout << "AliasSum: " << "         " << AliasSum << "\n";
    std::cout << "\n";
    std::cout << "No Alias     : " << formatProzent(NoAliasCount, AliasSum) << " " << NoAliasCount << "\n";
    std::cout << "May Alias    : " << formatProzent(MayAliasCount, AliasSum) << " " << MayAliasCount << "\n";
    std::cout << "Partial Alias: " << formatProzent(PartialAliasCount, AliasSum) << " " << PartialAliasCount << "\n";
    std::cout << "Must Alias   : " << formatProzent(MustAliasCount, AliasSum) << " " << MustAliasCount << "\n";
    std::cout << "\n";

    int64_t ModRefSum = NoModRefCount + RefCount + ModCount + ModRefCount + MustCount + MustRefCount + MustModCount + MustModRefCount;

    std::cout << "ModRefSum    : " << "         " << " " << ModRefSum << "\n";
    std::cout << "\n";
    std::cout << "No ModRef    : " << formatProzent(NoModRefCount, ModRefSum) << " " << NoModRefCount << "\n";
    std::cout << "Mod          : " << formatProzent(ModCount, ModRefSum) << " " << ModCount << "\n";
    std::cout << "Ref          : " << formatProzent(RefCount, ModRefSum) << " " << RefCount << "\n";
    std::cout << "ModRef       : " << formatProzent(ModRefCount, ModRefSum) << " " << ModRefCount << "\n";
    std::cout << "Must         : " << formatProzent(MustCount, ModRefSum) << " " << MustCount << "\n";
    std::cout << "MustMod      : " << formatProzent(MustModCount, ModRefSum) << " " << MustModCount << "\n";
    std::cout << "MustRef      : " << formatProzent(MustRefCount, ModRefSum) << " " << MustRefCount << "\n";
    std::cout << "MustModRef   : " << formatProzent(MustModRefCount, ModRefSum) << " " << MustModRefCount << "\n";
}