#include "boaaa/lv/CountPass.h"
using namespace boaaa;
char CountPass::ID = 0;


namespace llvm {

	INITIALIZE_PASS_BEGIN(CountPass, "count", "counts Functions, BasicBlocks, Loops and Instructions", true, true)
	INITIALIZE_PASS_DEPENDENCY(LoopInfoWrapperPass)
	INITIALIZE_PASS_END(CountPass, "count", "counts Functions, BasicBlocks, Loops and Instructions", true, true)
}

namespace boaaa {
	CountPass::CountPass() : LLVMFunctionPass(ID)
	{
		llvm::initializeCountPassPass(*llvm::PassRegistry::getPassRegistry());
		function_count = 0;
		basic_block_count = 0;
		instruction_count = 0;
		loop_count = 0;
	}

	void CountPass::getAnalysisUsage(llvm::AnalysisUsage& AU) const
	{
		AU.addRequiredTransitive<llvm::LoopInfoWrapperPass>();
		AU.setPreservesAll();
	}

	bool CountPass::runOnFunction(LLVMFunction& F) {

		llvm::LoopInfo& LI = getAnalysis<llvm::LoopInfoWrapperPass>().getLoopInfo();
		for (llvm::Loop* L : LI) {
			loop_count++;
		}

		for (llvm::BasicBlock& BB : F.getBasicBlockList()) {

			for (llvm::Instruction& I : BB.getInstList()) {
				instruction_count++;
			}
			basic_block_count++;
		}
		function_count++;

		return false;
	}
}

namespace llvm {
	LLVMFunctionPass* createCountPass() { return new CountPass(); }
}