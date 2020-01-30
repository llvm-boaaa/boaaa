#ifndef EVALUATION_PASS_H
#define EVALUATION_PASS_H

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ combine versions
namespace {
	namespace _a { //define LLVMModule
#include "include_versions/LLVM_Module.inc"
#include "unify_LLVM_VERSION.def"
	}

	namespace _b {
#include "include_versions/LLVM_include_Pass_h.inc"
#include "unify_LLVM_VERSION.def"
	}

	namespace _c { //define LLVMModulePass
#include "include_versions/LLVM_ModulePass.inc"
#include "unify_LLVM_VERSION.def"
	}
	namespace _d { //define LLVMRegisterPass
#include "include_versions/LLVM_RegisterPass.inc"
#include "unify_LLVM_VERSION.def"
	}

#ifdef LLVM_VERSION_ERROR_CODE
	LLVM_VERSION_ERROR_CODE
#endif

#include "boaaa/lv/macrolist/MACROLLIST_EVAL.inc"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct AAEvaluationPass : public LLVMModulePass
	{
		static char ID;
		AAEvaluationPass() : LLVMModulePass(ID) {}

		bool runOnModule(LLVMModule& M) override {

			return false;
		}
	};
}

char AAEvaluationPass::ID = 0;

//static LLVMRegisterPass<AAEvaluationPass> X("aaeval", "AAEvaluation Pass", true, false);

#endif // !EVALUATION_PASS_H