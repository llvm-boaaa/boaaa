#ifndef COUNT_PASS_H
#define COUNT_PASS_H

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

	namespace _e { //define LLVMFunction
#include "include_versions/LLVM_Function.inc"
#include "unify_LLVM_VERSION.def"
	}

#ifdef LLVM_VERSION_ERROR_CODE
	LLVM_VERSION_ERROR_CODE
#endif
}

using namespace _a;
using namespace _c;
using namespace _d;
using namespace _e;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
namespace boaaa {
	struct CountPass : public LLVMModulePass
	{
		static char ID;
		CountPass() : LLVMModulePass(ID) {}

		uint64_t function_count = 0;
		uint64_t basic_block_count = 0;
		uint64_t instruction_count = 0;

		bool runOnModule(LLVMModule& M) override {
			function_count = 0;
			basic_block_count = 0;
			instruction_count = 0;

			for (LLVMFunction& f : M.functions()) {
				for (llvm::BasicBlock &BB : f) {
					for (llvm::Instruction& I : BB) {
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
}

char boaaa::CountPass::ID = 0;

static LLVMRegisterPass<boaaa::CountPass> X("count", "CountPass", true, false);

#endif