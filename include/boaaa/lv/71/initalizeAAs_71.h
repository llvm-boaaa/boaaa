#ifndef BOAAA_LV_INITIALIZE_AAS_71_H
#define BOAAA_LV_INITIALIZE_AAS_71_H

#include "boaaa/commandline_types.h"
#include "boaaa/support/__STRINGIFY.h"

namespace boaaa
{
	enum LLVM_71_AA : aa_id
	{
		BASIC      = LLVM_71 + LLVM_ORIGINAL_ANALYSIS::BASIC_AA,
		CFL_ANDERS = LLVM_71 + LLVM_ORIGINAL_ANALYSIS::CFL_ANDERS_AA,
		CFL_STEENS = LLVM_71 + LLVM_ORIGINAL_ANALYSIS::CFL_STEENS_AA,
		OBJ_CARC   = LLVM_71 + LLVM_ORIGINAL_ANALYSIS::OBJ_CARC_AA,
		SCEV       = LLVM_71 + LLVM_ORIGINAL_ANALYSIS::SCEV_AA,
		SCOPEDNA   = LLVM_71 + LLVM_ORIGINAL_ANALYSIS::SCOPED_NA_AA,
		TBAA       = LLVM_71 + LLVM_ORIGINAL_ANALYSIS::TBAA_AA,
	};

	namespace _71 {

		static char BASIC[]      = __STRINGIFY(BASIC);
		static char CFL_ANDERS[] = __STRINGIFY(CFL-ANDERS);
		static char CFL_STEENS[] = __STRINGIFY(CFL-STEENS);
		static char OBJ_CARC[]   = __STRINGIFY(OBJ-CARC);
		static char SCEV[]       = __STRINGIFY(SCEV);
		static char SCOPEDNA[]   = __STRINGIFY(SCOPEDNA);
		static char TBAA[]       = __STRINGIFY(TBAA);
	}

	inline cl_aa_store getInitalizedAAs_71()
	{
		using L7 = LLVM_71_AA;

		cl_aa_store raa;
		raa.push_back(registeredAA(&_71::BASIC[0],      static_cast<aa_id>(L7::BASIC),      getDescription(L7::BASIC)));
		raa.push_back(registeredAA(&_71::CFL_ANDERS[0], static_cast<aa_id>(L7::CFL_ANDERS), getDescription(L7::CFL_ANDERS)));
		raa.push_back(registeredAA(&_71::CFL_STEENS[0], static_cast<aa_id>(L7::CFL_STEENS), getDescription(L7::CFL_STEENS)));
		raa.push_back(registeredAA(&_71::OBJ_CARC[0],   static_cast<aa_id>(L7::OBJ_CARC),   getDescription(L7::OBJ_CARC)));
		raa.push_back(registeredAA(&_71::SCEV[0],       static_cast<aa_id>(L7::SCEV),       getDescription(L7::SCEV)));
		raa.push_back(registeredAA(&_71::SCOPEDNA[0],   static_cast<aa_id>(L7::SCOPEDNA),   getDescription(L7::SCOPEDNA)));
		raa.push_back(registeredAA(&_71::TBAA[0],       static_cast<aa_id>(L7::TBAA),       getDescription(L7::TBAA)));
		return raa;
	}
}

#endif //!BOAAA_LV_INITALIZE_AAS_71_H