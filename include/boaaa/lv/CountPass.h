#ifndef COUNT_PASS_H
#define COUNT_PASS_H

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ combine versions

//define LLVMModule
#include "include_versions/LLVM_Module.inc"
// include llvm/Pass.h
#include "include_versions/LLVM_include_Pass_h.inc"
//define LLVMModulePass
#include "include_versions/LLVM_ModulePass.inc"
//define LLVMRegisterPass
#include "include_versions/LLVM_RegisterPass.inc"
//define LLVMFunction
#include "include_versions/LLVM_Function.inc"

#ifdef LLVM_VERSION_ERROR_CODE
	LLVM_VERSION_ERROR_CODE
#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

namespace boaaa {

	struct CountPass : public LLVMModulePass
	{
		static char ID;
		CountPass() : LLVMModulePass(ID) {}

		uint64_t function_count = 0;
		uint64_t basic_block_count = 0;
		uint64_t instruction_count = 0;
		uint64_t loop_count = 0;

		bool runOnModule(LLVMModule& M) override {
			function_count = 0;
			basic_block_count = 0;
			instruction_count = 0;
			loop_count = 0;


			for (LLVMFunction& F : M.functions()) {

				llvm::LoopInfo& LI = getAnalysis<llvm::LoopInfoWrapperPass>(F).getLoopInfo();
				for (llvm::Loop* L : LI) {
					loop_count++;
				}

				for (llvm::BasicBlock &BB : F.getBasicBlockList()) {

					for (llvm::Instruction& I : BB.getInstList()) {
						instruction_count++;
					}
					basic_block_count++;
				}
				function_count++;
			}

			llvm::errs() << "Function #" << function_count << "\n"
				<< "BasicBlock #" << basic_block_count << "\n"
				<< "Instructions #" << instruction_count <<  "\n";

			return false;
		}
	};

	char CountPass::ID = 0;

	//static LLVMRegisterPass<CountPass> X("count", "CountPass", true, false);
}


#endif