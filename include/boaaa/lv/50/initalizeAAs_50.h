#ifndef BOAAA_LV_INITIALIZE_AAS_50_H
#define BOAAA_LV_INITIALIZE_AAS_50_H

#include "boaaa/commandline_types.h"
#include "boaaa/support/__STRINGIFY.h"

namespace boaaa
{
	enum LLVM_50_AA : aa_id
	{
		BASIC      = LLVM_50 + LLVM_ORIGINAL_ANALYSIS::BASIC_AA,
		CFL_ANDERS = LLVM_50 + LLVM_ORIGINAL_ANALYSIS::CFL_ANDERS_AA,
		CFL_STEENS = LLVM_50 + LLVM_ORIGINAL_ANALYSIS::CFL_STEENS_AA,
		OBJ_CARC   = LLVM_50 + LLVM_ORIGINAL_ANALYSIS::OBJ_CARC_AA,
		SCEV       = LLVM_50 + LLVM_ORIGINAL_ANALYSIS::SCEV_AA,
		SCOPEDNOA  = LLVM_50 + LLVM_ORIGINAL_ANALYSIS::SCOPED_NA_AA,
		TBAA       = LLVM_50 + LLVM_ORIGINAL_ANALYSIS::TBAA_AA,
		CLANG	   = LLVM_50 + LLVM_ORIGINAL_ANALYSIS_COMBINED::CLANG_COMBINED_ANALYSIS,
#ifdef SEA_DSA
		SEA_DSA_CS,
		SEA_DSA_BU,
		SEA_DSA_CS_BUTD,
		SEA_DSA_CIS,
		SEA_DSA_FM,
#endif
	};

	namespace _50 {

		static char* BASIC				= __STRINGIFY(BASIC);
		static char* CFL_ANDERS 		= __STRINGIFY(CFL-ANDERS);
		static char* CFL_STEENS			= __STRINGIFY(CFL-STEENS);
		static char* OBJ_CARC			= __STRINGIFY(OBJ-CARC);
		static char* SCEV				= __STRINGIFY(SCEV);
		static char* SCOPEDNOA			= __STRINGIFY(SCOPEDNOA);
		static char* TBAA				= __STRINGIFY(TBAA);

		static char* CLANG			    = __STRINGIFY(CLANG-COMBINE);

		static char* SEA_DSA_CS			= __STRINGIFY(SEA-DSA-CS);
		static char* SEA_DSA_BU			= __STRINGIFY(SEA-DSA-BU);
		static char* SEA_DSA_CS_BUTD	= __STRINGIFY(SEA-DSA-CS-BUTD);
		static char* SEA_DSA_CIS		= __STRINGIFY(SEA-DSA-CIS);
		static char* SEA_DSA_FM			= __STRINGIFY(SEA-DSA-FM);
	}

	inline cl_aa_store getInitalizedAAs_50()
	{
		using L5 = LLVM_50_AA;

		cl_aa_store raa;
		raa.push_back(registeredAA(_50::BASIC,			static_cast<aa_id>(L5::BASIC),      getDescription(L5::BASIC)));
		raa.push_back(registeredAA(_50::CFL_ANDERS,		static_cast<aa_id>(L5::CFL_ANDERS), getDescription(L5::CFL_ANDERS)));
		raa.push_back(registeredAA(_50::CFL_STEENS,		static_cast<aa_id>(L5::CFL_STEENS), getDescription(L5::CFL_STEENS)));
		raa.push_back(registeredAA(_50::OBJ_CARC,		static_cast<aa_id>(L5::OBJ_CARC),   getDescription(L5::OBJ_CARC)));
		raa.push_back(registeredAA(_50::SCEV,			static_cast<aa_id>(L5::SCEV),       getDescription(L5::SCEV)));
		raa.push_back(registeredAA(_50::SCOPEDNOA,		static_cast<aa_id>(L5::SCOPEDNOA),  getDescription(L5::SCOPEDNOA)));
		raa.push_back(registeredAA(_50::TBAA,			static_cast<aa_id>(L5::TBAA),       getDescription(L5::TBAA)));

		raa.push_back(registeredAA(_50::CLANG,			static_cast<aa_id>(L5::CLANG),      getCombinedDescription(L5::CLANG)));
#ifdef SEA_DSA
		raa.push_back(registeredAA(_50::SEA_DSA_CS,			static_cast<aa_id>(L5::SEA_DSA_CS),			"external SeaDsa Context Sensitive Analysis"));
		raa.push_back(registeredAA(_50::SEA_DSA_CS_BUTD,	static_cast<aa_id>(L5::SEA_DSA_CS_BUTD),	"external SeaDsa Context Sensitive BottumUpTopDown Analysis"));
		raa.push_back(registeredAA(_50::SEA_DSA_BU,			static_cast<aa_id>(L5::SEA_DSA_BU),			"external SeaDsa BottomUp Analysis"));
		raa.push_back(registeredAA(_50::SEA_DSA_CIS,		static_cast<aa_id>(L5::SEA_DSA_CIS),		"external SeaDsa Context Insensitive Analysis"));
		raa.push_back(registeredAA(_50::SEA_DSA_FM,			static_cast<aa_id>(L5::SEA_DSA_FM),			"external SeaDsa Flat Memory Analysis"));
#endif
		return raa;
	}
}

#endif //!BOAAA_LV_INITALIZE_AAS_50_H