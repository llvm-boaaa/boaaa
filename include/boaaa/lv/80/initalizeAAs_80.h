#ifndef BOAAA_LV_INITIALIZE_AAS_80_H
#define BOAAA_LV_INITIALIZE_AAS_80_H

#include "boaaa/commandline_types.h"
#include "boaaa/support/__STRINGIFY.h"

namespace boaaa
{
	enum LLVM_80_AA : aa_id
	{
		BASIC      = LLVM_80 + LLVM_ORIGINAL_ANALYSIS::BASIC_AA,
		CFL_ANDERS = LLVM_80 + LLVM_ORIGINAL_ANALYSIS::CFL_ANDERS_AA,
		CFL_STEENS = LLVM_80 + LLVM_ORIGINAL_ANALYSIS::CFL_STEENS_AA,
		OBJ_CARC   = LLVM_80 + LLVM_ORIGINAL_ANALYSIS::OBJ_CARC_AA,
		SCEV       = LLVM_80 + LLVM_ORIGINAL_ANALYSIS::SCEV_AA,
		SCOPEDNA   = LLVM_80 + LLVM_ORIGINAL_ANALYSIS::SCOPED_NA_AA,
		TBAA       = LLVM_80 + LLVM_ORIGINAL_ANALYSIS::TBAA_AA,
	};

	namespace _80 {

		static char BASIC[]      = __STRINGIFY(BASIC);
		static char CFL_ANDERS[] = __STRINGIFY(CFL-ANDERS);
		static char CFL_STEENS[] = __STRINGIFY(CFL-STEENS);
		static char OBJ_CARC[]   = __STRINGIFY(OBJ-CARC);
		static char SCEV[]       = __STRINGIFY(SCEV);
		static char SCOPEDNA[]   = __STRINGIFY(SCOPEDNA);
		static char TBAA[]       = __STRINGIFY(TBAA);
	}

	inline cl_aa_store getInitalizedAAs_80()
	{
		using L8 = LLVM_80_AA;

		cl_aa_store raa;
		raa.push_back(registeredAA(&_80::BASIC[0],      static_cast<aa_id>(L8::BASIC),      getDescription(L8::BASIC)));
		raa.push_back(registeredAA(&_80::CFL_ANDERS[0], static_cast<aa_id>(L8::CFL_ANDERS), getDescription(L8::CFL_ANDERS)));
		raa.push_back(registeredAA(&_80::CFL_STEENS[0], static_cast<aa_id>(L8::CFL_STEENS), getDescription(L8::CFL_STEENS)));
		raa.push_back(registeredAA(&_80::OBJ_CARC[0],   static_cast<aa_id>(L8::OBJ_CARC),   getDescription(L8::OBJ_CARC)));
		raa.push_back(registeredAA(&_80::SCEV[0],       static_cast<aa_id>(L8::SCEV),       getDescription(L8::SCEV)));
		raa.push_back(registeredAA(&_80::SCOPEDNA[0],   static_cast<aa_id>(L8::SCOPEDNA),   getDescription(L8::SCOPEDNA)));
		raa.push_back(registeredAA(&_80::TBAA[0],       static_cast<aa_id>(L8::TBAA),       getDescription(L8::TBAA)));
		return raa;
	}
}

#endif //!BOAAA_LV_INITALIZE_AAS_80_H