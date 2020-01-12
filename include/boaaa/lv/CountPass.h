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

#ifdef LLVM_VERSION_ERROR_CODE
LLVM_VERSION_ERROR_CODE
#endif
}
using namespace _a;
//using namespace _b;
using namespace _c;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

namespace boaaa {
	
	class CountPass : public LLVMModulePass
	{
		static char ID;
		CountPass() : LLVMModulePass(ID) {}
	};

}

#endif