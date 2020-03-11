#ifndef BOAAA_LV_INITIALIZE_AAS_90_H
#define BOAAA_LV_INITIALIZE_AAS_90_H

#include "boaaa/commandline_types.h"
#include "boaaa/support/__STRINGIFY.h"

namespace boaaa
{
	enum LLVM_90_AA : aa_id
	{
		BASIC      = LLVM_90 + LLVM_ORIGINAL_ANALYSIS::BASIC_AA,
		CFL_ANDERS = LLVM_90 + LLVM_ORIGINAL_ANALYSIS::CFL_ANDERS_AA,
		CFL_STEENS = LLVM_90 + LLVM_ORIGINAL_ANALYSIS::CFL_STEENS_AA,
		OBJ_CARC   = LLVM_90 + LLVM_ORIGINAL_ANALYSIS::OBJ_CARC_AA,
		SCEV       = LLVM_90 + LLVM_ORIGINAL_ANALYSIS::SCEV_AA,
		SCOPEDNA   = LLVM_90 + LLVM_ORIGINAL_ANALYSIS::SCOPED_NA_AA,
		TBAA       = LLVM_90 + LLVM_ORIGINAL_ANALYSIS::TBAA_AA,
	};

	namespace _90 {
		static char BASIC[]      = __STRINGIFY(BASIC);
		static char CFL_ANDERS[] = __STRINGIFY(CFL-ANDERS);
		static char CFL_STEENS[] = __STRINGIFY(CFL-STEENS);
		static char OBJ_CARC[]   = __STRINGIFY(OBJ-CARC);
		static char SCEV[]       = __STRINGIFY(SCEV);
		static char SCOPEDNA[]   = __STRINGIFY(SCOPEDNA);
		static char TBAA[]       = __STRINGIFY(TBAA);
	}

	inline cl_aa_store getInitalizedAAs_90()
	{
		using L9 = LLVM_90_AA;

		cl_aa_store raa;
		raa.push_back(registeredAA(&_90::BASIC[0],      static_cast<aa_id>(L9::BASIC),      getDescription(L9::BASIC)));
		//raa.push_back(registeredAA(&_90::CFL_ANDERS[0], static_cast<aa_id>(L9::CFL_ANDERS), getDescription(L9::CFL_ANDERS)));
		//raa.push_back(registeredAA(&_90::CFL_STEENS[0], static_cast<aa_id>(L9::CFL_STEENS), getDescription(L9::CFL_STEENS)));
		//raa.push_back(registeredAA(&_90::OBJ_CARC[0],   static_cast<aa_id>(L9::OBJ_CARC),   getDescription(L9::OBJ_CARC)));
		raa.push_back(registeredAA(&_90::SCEV[0],       static_cast<aa_id>(L9::SCEV),       getDescription(L9::SCEV)));
		//raa.push_back(registeredAA(&_90::SCOPEDNA[0],   static_cast<aa_id>(L9::SCOPEDNA),   getDescription(L9::SCOPEDNA)));
		//raa.push_back(registeredAA(&_90::TBAA[0],       static_cast<aa_id>(L9::TBAA),       getDescription(L9::TBAA)));
		return raa;
	}
}

#endif //!BOAAA_LV_INITALIZE_AAS_90_H