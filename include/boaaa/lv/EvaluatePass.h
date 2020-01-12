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

#ifdef LLVM_VERSION_ERROR_CODE
LLVM_VERSION_ERROR_CODE
#endif
}
using namespace _a;
using namespace _c;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

namespace boaaa {

	class EvaluationPass : public LLVMModulePass
	{

	};

}

#endif // !EVALUATION_PASS_H