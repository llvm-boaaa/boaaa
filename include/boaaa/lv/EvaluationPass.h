#ifndef EVALUATION_PASS_H
#define EVALUATION_PASS_H

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ combine versions

//define LLVMAAResults
#include "include_versions/LLVM_AAResults.inc"
//define LLVMAliasResult
#include "include_versions/LLVM_AliasResult.inc"
//define LLVMCallUnifyer
#include "include_versions/LLVM_CallVersionUnifyer.inc"
//define LLVMFunction
#include "include_versions/LLVM_Function.inc"
//define LLVMModule
#include "include_versions/LLVM_Module.inc"
//include llvm/Pass.h
#include "include_versions/LLVM_include_Pass_h.inc"
//define LLVMInstruction
#include "include_versions/LLVM_Instruction.inc"
//define LLVMLoadInst
#include "include_versions/LLVM_LoadInst.inc"
//define LLVMLocationSize
#include "include_versions/LLVM_LocationSize.inc"
//define LLVMMemoryLocation
#include "include_versions/LLVM_MemoryLocation.inc"
//define LLVMModulePass
#include "include_versions/LLVM_ModulePass.inc"
//define LLVMRegisterPass
#include "include_versions/LLVM_RegisterPass.inc"
//define LLVMSetVector
#include "include_versions/LLVM_SetVector.inc"
//define LLVMSmallSetVector
#include "include_versions/LLVM_SmallSetVector.inc"
//define LLVMStoreInst
#include "include_versions/LLVM_StoreInst.inc"
//define LLVMType
#include "include_versions/LLVM_Type.inc"
//define LLVMUse
#include "include_versions/LLVM_Use.inc"
//define LLVMValue
#include "include_versions/LLVM_Value.inc"

#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Constants.h"
#include "llvm/Analysis/AliasAnalysis.h"

#include "boaaa/support/AutoDeleter.h"

#include <iostream>

#ifdef LLVM_VERSION_ERROR_CODE
LLVM_VERSION_ERROR_CODE
#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

namespace boaaa {

	class EvaluationPassImpl
	{
    public:
		EvaluationPassImpl() : FunctionCount(), NoAliasCount(), MayAliasCount(), PartialAliasCount(), 
			MustAliasCount(), NoModRefCount(), ModCount(), RefCount(), ModRefCount(), MustCount(), 
			MustRefCount(),	MustModCount(), MustModRefCount() {}

        void evaluateAAResultOnModule(LLVMModule& M, llvm::AAResults &AAResult);
		void evaluateAAResultOnFunction(LLVMFunction& F, llvm::AAResults& AAResult);

		void printResult(std::ostream &stream);

	private:
		int64_t FunctionCount;
		int64_t NoAliasCount, MayAliasCount, PartialAliasCount, MustAliasCount;
		int64_t NoModRefCount, ModCount, RefCount, ModRefCount;
		int64_t MustCount, MustRefCount, MustModCount, MustModRefCount;
	};
}


#endif // !EVALUATION_PASS_H