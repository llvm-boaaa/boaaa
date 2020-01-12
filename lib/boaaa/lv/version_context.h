#ifndef BOAAA_VERSION_CONTEXT_H
#define BOAAA_VERSION_CONTEXT_H

#include "boaaa/lv/sup_LLVM_VERSION.h"
#include "boaaa/support/dump_ostream.h"
#include "boaaa/vp/StringRefVersionParser.h"

namespace boaaa
{
	struct version_context;
#ifdef NOT_SUPPORTED_LLVM_VERSION
	struct version_context {};
#else
	struct version_context
	{
//-------------------------------------------------------LLVM_VERSION dependent

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ string_ref_vp
#define LLVM_VERSION_CALL_FILE __FILE__
#define LLVM_VERSION_CALL_LINE __LINE__

#define LLVM_VERSION_40_INCLUDE "40/StringRefVersionParser40.h"
#define LLVM_VERSION_50_INCLUDE "50/StringRefVersionParser50.h"
#define LLVM_VERSION_90_INCLUDE "90/StringRefVersionParser90.h"

#define LLVM_VERSION_40_UNIFY std::add_pointer_t<StringRefVP40>
#define LLVM_VERSION_50_UNIFY std::add_pointer_t<StringRefVP50>
#define LLVM_VERSION_90_UNIFY std::add_pointer_t<StringRefVP90>
#define LLVM_VERSION_UNIFY_FINALIZE string_ref_vp;
#include "boaaa/lv/unify_LLVM_VERSION.def"
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ string_ref_vp */

//-----------------------------------------------------LLVM_VERSION independent
		std::ostream* basic_ostream;
		bool del_strm_after_use;
		
		version_context()
		{
			basic_ostream = new dump_ostream(std::_Uninitialized::_Noinit);
			del_strm_after_use = false;
			string_ref_vp = nullptr;
		}

	};
}

#endif // !NOT_SUPPORTED_LLVM_VERSION

#endif // !BOAAA_VERSION_CONTEXT
