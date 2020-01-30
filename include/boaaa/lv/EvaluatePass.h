#ifndef EVALUATION_PASS_H
#define EVALUATION_PASS_H

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ combine versions

//define LLVMModule
#include "include_versions/LLVM_Module.inc"
//include llvm/Pass.h
#include "include_versions/LLVM_include_Pass_h.inc"
//define LLVMModulePass
#include "include_versions/LLVM_ModulePass.inc"
//define LLVMRegisterPass
#include "include_versions/LLVM_RegisterPass.inc"

#ifdef LLVM_VERSION_ERROR_CODE
	LLVM_VERSION_ERROR_CODE
#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

namespace boaaa {

	struct AAEvaluationPass : public LLVMModulePass
	{
		static char ID;
		AAEvaluationPass() : LLVMModulePass(ID) {}

		bool runOnModule(LLVMModule& M) override {

			return false;
		}
	};

	char AAEvaluationPass::ID = 0;

	//static LLVMRegisterPass<AAEvaluationPass> X("aaeval", "AAEvaluation Pass", true, false);	
}


#endif // !EVALUATION_PASS_H