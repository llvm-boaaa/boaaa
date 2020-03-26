#ifndef BOAAA_LV_INITIALIZE_AAS_60_H
#define BOAAA_LV_INITIALIZE_AAS_60_H

#include "boaaa/commandline_types.h"
#include "boaaa/support/__STRINGIFY.h"

namespace boaaa
{
	enum LLVM_60_AA : aa_id
	{
		BASIC      = LLVM_60 + LLVM_ORIGINAL_ANALYSIS::BASIC_AA,
		CFL_ANDERS = LLVM_60 + LLVM_ORIGINAL_ANALYSIS::CFL_ANDERS_AA,
		CFL_STEENS = LLVM_60 + LLVM_ORIGINAL_ANALYSIS::CFL_STEENS_AA,
		OBJ_CARC   = LLVM_60 + LLVM_ORIGINAL_ANALYSIS::OBJ_CARC_AA,
		SCEV       = LLVM_60 + LLVM_ORIGINAL_ANALYSIS::SCEV_AA,
		SCOPEDNA   = LLVM_60 + LLVM_ORIGINAL_ANALYSIS::SCOPED_NA_AA,
		TBAA       = LLVM_60 + LLVM_ORIGINAL_ANALYSIS::TBAA_AA,
	};

	namespace _60 {

		static char BASIC[]      = __STRINGIFY(BASIC);
		static char CFL_ANDERS[] = __STRINGIFY(CFL-ANDERS);
		static char CFL_STEENS[] = __STRINGIFY(CFL-STEENS);
		static char OBJ_CARC[]   = __STRINGIFY(OBJ-CARC);
		static char SCEV[]       = __STRINGIFY(SCEV);
		static char SCOPEDNA[]   = __STRINGIFY(SCOPEDNA);
		static char TBAA[]       = __STRINGIFY(TBAA);
	}

	inline cl_aa_store getInitalizedAAs_60()
	{
		using L6 = LLVM_60_AA;

		cl_aa_store raa;
		raa.push_back(registeredAA(&_60::BASIC[0],      static_cast<aa_id>(L6::BASIC),      getDescription(L6::BASIC)));
		raa.push_back(registeredAA(&_60::CFL_ANDERS[0], static_cast<aa_id>(L6::CFL_ANDERS), getDescription(L6::CFL_ANDERS)));
		raa.push_back(registeredAA(&_60::CFL_STEENS[0], static_cast<aa_id>(L6::CFL_STEENS), getDescription(L6::CFL_STEENS)));
		raa.push_back(registeredAA(&_60::OBJ_CARC[0],   static_cast<aa_id>(L6::OBJ_CARC),   getDescription(L6::OBJ_CARC)));
		raa.push_back(registeredAA(&_60::SCEV[0],       static_cast<aa_id>(L6::SCEV),       getDescription(L6::SCEV)));
		raa.push_back(registeredAA(&_60::SCOPEDNA[0],   static_cast<aa_id>(L6::SCOPEDNA),   getDescription(L6::SCOPEDNA)));
		raa.push_back(registeredAA(&_60::TBAA[0],       static_cast<aa_id>(L6::TBAA),       getDescription(L6::TBAA)));
		return raa;
	}
}

#endif //!BOAAA_LV_INITALIZE_AAS_60_H