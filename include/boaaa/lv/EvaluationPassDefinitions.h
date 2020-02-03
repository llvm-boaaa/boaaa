#ifndef BOAAA_LV_EVALUATION_PASS_DEFINITIONS_H
#define BOAAA_LV_EVALUATION_PASS_DEFINITIONS_H

#include "include_versions/LLVM_include_Pass_h.inc"
#include "include_versions/LLVM_Module.inc"
#include "include_versions/LLVM_ModulePass.inc"

#include "boaaa/lv/EvaluationPass.h"


//Analysises
#include "llvm/Analysis/ScalarEvolutionAliasAnalysis.h"

namespace boaaa {


	/*
	 * Instanziation of all EvaluationPasses in Alphabetic order
	 */


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++SVECAA

	class SVECAAEvalWrapperPass : public LLVMModulePass {
	public:
		static char ID;

		SVECAAEvalWrapperPass();

		bool runOnModule(LLVMModule& M) override;
		void getAnalysisUsage(llvm::AnalysisUsage& AU) const override;

	private:
		AAResultEvaluationPassImpl impl;
	};

	LLVMModulePass* createSVECAAEVALWrapperPass();


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++SVECAA

}


#endif