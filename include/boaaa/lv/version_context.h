#ifndef BOAAA_VERSION_CONTEXT_H
#define BOAAA_VERSION_CONTEXT_H

#include <memory>
#include <type_traits>

#include "boaaa/lv/sup_LLVM_VERSION.h"
#include "boaaa/vp/StringRefVersionParser.h"

namespace boaaa {
	struct version_context;
#ifdef NOT_SUPPORTED_LLVM_VERSION
	struct version_context {};
}
#else
}

#include "boaaa/lv/include_versions/BOAAA_StringRefVP.inc"
#include "boaaa/lv/include_versions/LLVM_Module.inc"
#include "boaaa/lv/include_versions/LLVM_LLVMContext.inc"

#ifdef LLVM_VERSION_ERROR_CODE
LLVM_VERSION_ERROR_CODE
#endif

/*
#ifdef LLVM_VERSION_40
//#include "llvm/IR/Module.h"
#include "boaaa/lv/40/StringRefVersionParser40.h"
using BOAAAStringRefVP = boaaa::StringRefVP40;
#endif
#ifdef LLVM_VERSION_50
//#include "llvm/IR/Module.h"
#include "boaaa/lv/50/StringRefVersionParser50.h"
using BOAAAStringRefVP = boaaa::StringRefVP50;
#endif
#ifdef LLVM_VERSION_90
//#include "llvm/IR/Module.h"
#include "boaaa/lv/90/StringRefVersionParser90.h"
using BOAAAStringRefVP = boaaa::StringRefVP90;
#endif
*/

//using LLVMModule = llvm::Module;
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ combine versions
//define BOAAAStringRefVP
//#include "include_versions/BOAAA_StringRefVP.inc"

//define LLVMModule
//#include "include_versions/LLVM_Module.inc"


#ifdef LLVM_VERSION_ERROR_CODE
	LLVM_VERSION_ERROR_CODE
#endif

//#define BOAAA_MACROLIST_LIST_VER_MAX 0002
//#include "boaaa/lv/macrolist/MACROLIST_EVAL.inc"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

namespace boaaa {

	struct version_context
	{
//-------------------------------------------------------LLVM_VERSION dependent
		
		BOAAAStringRefVP* string_ref_vp;
		std::unique_ptr<LLVMModule> loaded_module;
		std::unique_ptr<LLVMLLVMContext> context_to_module;

//-----------------------------------------------------LLVM_VERSION independent
		std::ostream* basic_ostream;
		bool del_strm_after_use;
		
		version_context()
		{
			string_ref_vp = nullptr;
			loaded_module.reset(nullptr);
			context_to_module.reset(nullptr);
            basic_ostream = nullptr; //new dump_ostream(std::_Uninitialized::_Noinit);
			del_strm_after_use = false;
		}

	};
}

#endif // !NOT_SUPPORTED_LLVM_VERSION

#endif // !BOAAA_VERSION_CONTEXT
