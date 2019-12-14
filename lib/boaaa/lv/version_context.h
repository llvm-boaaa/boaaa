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
#ifdef LLVM_VERSION_40
#include "40/StringRefVersionParser40.h"
		StringRefVP40
#endif // LLVM_VERSION_40

#ifdef LLVM_VERSION_50
#include "50/StringRefVersionParser50.h"
		StringRefVP50
#endif // LLVM_VERSION_50

#ifdef LLVM_VERSION_90
#include "90/StringRefVersionParser90.h"
		StringRefVP90
#endif // LLVM_VERSION_90
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/ *string_ref_vp;

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
