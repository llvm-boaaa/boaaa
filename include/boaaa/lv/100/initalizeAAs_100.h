#ifndef BOAAA_LV_INITIALIZE_AAS_100_H
#define BOAAA_LV_INITIALIZE_AAS_100_H

#include "boaaa/commandline_types.h"
#include "boaaa/support/__STRINGIFY.h"

namespace boaaa
{
	enum LLVM_100_AA : aa_id
	{
		BASIC =			LLVM_100 + LLVM_ORIGINAL_ANALYSIS::BASIC_AA,
		CFL_ANDERS =	LLVM_100 + LLVM_ORIGINAL_ANALYSIS::CFL_ANDERS_AA,
		CFL_STEENS =	LLVM_100 + LLVM_ORIGINAL_ANALYSIS::CFL_STEENS_AA,
		OBJ_CARC =		LLVM_100 + LLVM_ORIGINAL_ANALYSIS::OBJ_CARC_AA,
		SCEV =			LLVM_100 + LLVM_ORIGINAL_ANALYSIS::SCEV_AA,
		SCOPEDNA =		LLVM_100 + LLVM_ORIGINAL_ANALYSIS::SCOPED_NA_AA,
		TBAA =			LLVM_100 + LLVM_ORIGINAL_ANALYSIS::TBAA_AA,
	};

	namespace _100 {
		static char BASIC[] =			__STRINGIFY(BASIC);
		static char CFL_ANDERS[] =		__STRINGIFY(CFL - ANDERS);
		static char CFL_STEENS[] =		__STRINGIFY(CFL - STEENS);
		static char OBJ_CARC[] =		__STRINGIFY(OBJ - CARC);
		static char SCEV[] =			__STRINGIFY(SCEV);
		static char SCOPEDNA[] =		__STRINGIFY(SCOPEDNA);
		static char TBAA[] =			__STRINGIFY(TBAA);
	}

	inline cl_aa_store getInitalizedAAs_100()
	{
		using L10 = LLVM_100_AA;

		cl_aa_store raa;
		raa.push_back(registeredAA(&_100::BASIC[0],			static_cast<aa_id>(L10::BASIC),			getDescription(L10::BASIC)));
		raa.push_back(registeredAA(&_100::CFL_ANDERS[0],	static_cast<aa_id>(L10::CFL_ANDERS),	getDescription(L10::CFL_ANDERS)));
		raa.push_back(registeredAA(&_100::CFL_STEENS[0],	static_cast<aa_id>(L10::CFL_STEENS),	getDescription(L10::CFL_STEENS)));
		raa.push_back(registeredAA(&_100::OBJ_CARC[0],		static_cast<aa_id>(L10::OBJ_CARC),		getDescription(L10::OBJ_CARC)));
		raa.push_back(registeredAA(&_100::SCEV[0],			static_cast<aa_id>(L10::SCEV),			getDescription(L10::SCEV)));
		raa.push_back(registeredAA(&_100::SCOPEDNA[0],		static_cast<aa_id>(L10::SCOPEDNA),		getDescription(L10::SCOPEDNA)));
		raa.push_back(registeredAA(&_100::TBAA[0],			static_cast<aa_id>(L10::TBAA),			getDescription(L10::TBAA)));
		return raa;
	}
}

#endif //!BOAAA_LV_INITALIZE_AAS_100_H