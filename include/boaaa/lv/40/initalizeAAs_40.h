#ifndef BOAAA_LV_INITIALIZE_AAS_40_H
#define BOAAA_LV_INITIALIZE_AAS_40_H

#include "boaaa/commandline_types.h"
#include "boaaa/support/__STRINGIFY.h"

namespace boaaa 
{
	enum LLVM_40_AA : aa_id
	{
		BASIC      = LLVM_40 + LLVM_ORIGINAL_ANALYSIS::BASIC_AA,
		CFL_ANDERS = LLVM_40 + LLVM_ORIGINAL_ANALYSIS::CFL_ANDERS_AA,
		CFL_STEENS = LLVM_40 + LLVM_ORIGINAL_ANALYSIS::CFL_STEENS_AA,
		OBJ_CARC   = LLVM_40 + LLVM_ORIGINAL_ANALYSIS::OBJ_CARC_AA,
		SCEV       = LLVM_40 + LLVM_ORIGINAL_ANALYSIS::SCEV_AA,
		SCOPEDNA   = LLVM_40 + LLVM_ORIGINAL_ANALYSIS::SCOPED_NA_AA,
		TBAA       = LLVM_40 + LLVM_ORIGINAL_ANALYSIS::TBAA_AA,
	};

	namespace _40 {

		static char BASIC[]      = __STRINGIFY(BASIC);
		static char CFL_ANDERS[] = __STRINGIFY(CFL-ANDERS);
		static char CFL_STEENS[] = __STRINGIFY(CFL-STEENS);
		static char OBJ_CARC[]   = __STRINGIFY(OBJ-CARC);
		static char SCEV[]       = __STRINGIFY(SCEV);
		static char SCOPEDNA[]   = __STRINGIFY(SCOPEDNA);
		static char TBAA[]       = __STRINGIFY(TBAA);
	}

	inline cl_aa_store getInitalizedAAs_40()
	{
		using L4 = LLVM_40_AA;

		cl_aa_store raa;
		raa.push_back(registeredAA(&_40::BASIC[0],      static_cast<aa_id>(L4::BASIC),      getDescription(L4::BASIC)));
		//raa.push_back(registeredAA(&_40::CFL_ANDERS[0], static_cast<aa_id>(L4::CFL_ANDERS), getDescription(L4::CFL_ANDERS)));
		raa.push_back(registeredAA(&_40::CFL_STEENS[0], static_cast<aa_id>(L4::CFL_STEENS), getDescription(L4::CFL_STEENS)));
		raa.push_back(registeredAA(&_40::OBJ_CARC[0],   static_cast<aa_id>(L4::OBJ_CARC),   getDescription(L4::OBJ_CARC)));
		raa.push_back(registeredAA(&_40::SCEV[0],       static_cast<aa_id>(L4::SCEV),       getDescription(L4::SCEV)));
		//raa.push_back(registeredAA(&_40::SCOPEDNA[0],   static_cast<aa_id>(L4::SCOPEDNA),   getDescription(L4::SCOPEDNA)));
		//raa.push_back(registeredAA(&_40::TBAA[0],       static_cast<aa_id>(L4::TBAA),       getDescription(L4::TBAA)));
		return raa;
	}
}

#endif //!BOAAA_LV_INITALIZE_AAS_40_H