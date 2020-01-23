#ifndef BOAAA_VERSION_CONTEXT_H
#define BOAAA_VERSION_CONTEXT_H

#include <memory>
#include <type_traits>

#include "boaaa/lv/sup_LLVM_VERSION.h"
#include "boaaa/support/dump_ostream.h"
#include "boaaa/vp/StringRefVersionParser.h"

namespace boaaa {
	struct version_context;
#ifdef NOT_SUPPORTED_LLVM_VERSION
	struct version_context {};
}
#else
}
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ combine versions

namespace {
	namespace _a { //define BOAAAStringRefVP
#include "include_versions/BOAAA_StringRefVP.inc"
#include "unify_LLVM_VERSION.def"
	}

	namespace _b { //define LLVMModule
//#include "include_versions/LLVM_Module.inc"
//#include "unify_LLVM_VERSION.def"
	}

#ifdef LLVM_VERSION_ERROR_CODE
	LLVM_VERSION_ERROR_CODE
#endif

}

using namespace _a;
using namespace _b;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

namespace boaaa {

	struct version_context
	{
//-------------------------------------------------------LLVM_VERSION dependent
		
		BOAAAStringRefVP* string_ref_vp;
		//std::unique_ptr<LLVMModule> loaded_module;

//-----------------------------------------------------LLVM_VERSION independent
		std::ostream* basic_ostream;
		bool del_strm_after_use;
		
		version_context()
		{
			basic_ostream = new dump_ostream(std::_Uninitialized::_Noinit);
			//loaded_module.reset(nullptr);
			del_strm_after_use = false;
			//string_ref_vp = nullptr;
		}

	};
}

#endif // !NOT_SUPPORTED_LLVM_VERSION

#endif // !BOAAA_VERSION_CONTEXT
