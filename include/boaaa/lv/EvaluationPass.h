#ifndef EVALUATION_PASS_H
#define EVALUATION_PASS_H

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ combine versions

//define LLVMFunction
#include "include_versions/LLVM_Function.inc"
//define LLVMModule
#include "include_versions/LLVM_Module.inc"
//include llvm/Pass.h
#include "include_versions/LLVM_include_Pass_h.inc"
//define LLVMModulePass
#include "include_versions/LLVM_ModulePass.inc"
//define LLVMRegisterPass
#include "include_versions/LLVM_RegisterPass.inc"
//define LLVMSetVector
#include "include_versions/LLVM_SetVector.inc"
//define LLVMSmallSetVector
#include "include_versions/LLVM_SmallSetVector.inc"

#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Constants.h"
#include "llvm/Analysis/AliasAnalysis.h"

#include <iostream>

#ifdef LLVM_VERSION_ERROR_CODE
LLVM_VERSION_ERROR_CODE
#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

namespace boaaa {

	class AAResultEvaluationPassImpl
	{
    public:
		AAResultEvaluationPassImpl() : FunctionCount(), NoAliasCount(), MayAliasCount(), PartialAliasCount(), 
			MustAliasCount(), NoModRefCount(), ModCount(), RefCount(), ModRefCount(), MustCount(), 
			MustRefCount(),	MustModCount(), MustModRefCount() {}

        void evaluateAAResult(llvm::AAResults &AAResult, LLVMModule& M);
		void evaluateAAResultOnFunction(llvm::AAResults& AAResult, LLVMFunction& F);

		void printResult(std::ostream &stream);

	private:
		int64_t FunctionCount;
		int64_t NoAliasCount, MayAliasCount, PartialAliasCount, MustAliasCount;
		int64_t NoModRefCount, ModCount, RefCount, ModRefCount;
		int64_t MustCount, MustRefCount, MustModCount, MustModRefCount;
	};
}


#endif // !EVALUATION_PASS_H