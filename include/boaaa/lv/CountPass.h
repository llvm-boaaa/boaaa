#ifndef COUNT_PASS_H
#define COUNT_PASS_H

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ combine versions

//define LLVMModule
#include "include_versions/LLVM_Module.inc"
// include llvm/Pass.h
#include "include_versions/LLVM_include_Pass_h.inc"
//define LLVMModulePass
#include "include_versions/LLVM_FunctionPass.inc"
//define LLVMRegisterPass
#include "include_versions/LLVM_RegisterPass.inc"
//define LLVMFunction
#include "include_versions/LLVM_Function.inc"
//include LoopInfoWrapperPass
#include "include_versions/LLVM_include_LoopInfo.inc"


#ifdef LLVM_VERSION_ERROR_CODE
	LLVM_VERSION_ERROR_CODE
#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

namespace boaaa {

	struct CountPass : public LLVMFunctionPass
	{
		static char ID;
		CountPass();

		uint64_t function_count = 0;
		uint64_t basic_block_count = 0;
		uint64_t instruction_count = 0;
		uint64_t loop_count = 0;

		bool runOnFunction(LLVMFunction& F) override;

		void printResult(std::ostream& stream) {
			stream	<< "Module Report LLVM_" << LLVM_VERSION << ":\n";
			stream	<< "Functions    #" << function_count << "\n"
					<< "BasicBlocks  #" << basic_block_count << "\n"
					<< "Loops        #" << loop_count << "\n"
					<< "Instructions #" << instruction_count << "\n\n";
		}

		void getAnalysisUsage(llvm::AnalysisUsage& AU) const override;
	};
}

namespace llvm {
	LLVMFunctionPass* createCountPass();
	void initializeCountPassPass(llvm::PassRegistry& Registry);
}


#endif