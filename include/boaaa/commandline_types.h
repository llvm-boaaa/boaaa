#ifndef BOAAA_COMMANDLINE_TYPES_H
#define BOAAA_COMMANDLINE_TYPES_H

#include <inttypes.h>
#include <vector>

#include "boaaa/support/data_store.h"

namespace boaaa
{
	/*
	 * LLVM_VERSION := MajorMinor (real llvm version Major.Minor.Patch)
	 * [+-][LLVM_VERSION][__________Data__________]
	 * [1b][   11 bit   ][         20 bit         ]
	 * 8 bit Major, 4 bit Minor
	 */
	typedef int32_t aa_id;

	static aa_id version_mask = 0xFFF00000;
	static aa_id      aa_mask = 0x000FFFFF;

	typedef boaaa::data_store<char*, aa_id, char*>	registeredAA;
	typedef std::vector<registeredAA>				cl_aa_store;


	//only versions that are considered to get used in the future, but all versions are possible to add to the enum
	enum LLVM_VERSIONS : aa_id 
	{
		LLVM_30  =  (3 << 24) + (0 << 20),
		LLVM_35  =  (3 << 24) + (5 << 20),
		LLVM_40  =  (4 << 24) + (0 << 20),
		LLVM_50  =  (5 << 24) + (0 << 20),
		LLVM_60  =  (6 << 24) + (0 << 20),
		LLVM_70  =  (7 << 24) + (0 << 20),
		LLVM_71  =  (7 << 24) + (1 << 20),
		LLVM_80  =  (8 << 24) + (0 << 20),
		LLVM_90  =  (9 << 24) + (0 << 20),
		LLVM_100 = (10 << 24) + (0 << 20),
		LLVM_110 = (11 << 24) + (0 << 20)
	};

	enum LLVM_ORIGINAL_ANALYSIS : aa_id
	{
		BASIC_AA		= 0,
		CFL_ANDERS_AA,
		CFL_STEENS_AA,
		OBJ_CARC_AA,
		SCEV_AA,
		SCOPED_NA_AA,
		TBAA_AA
	};

	enum LLVM_ORIGINAL_ANALYSIS_COMBINED : aa_id
	{
		CLANG_COMBINED_ANALYSIS = LLVM_ORIGINAL_ANALYSIS::TBAA_AA + 1,
		
	};

	inline char* getDescription(aa_id aa_id)
	{
		using LOA = LLVM_ORIGINAL_ANALYSIS;

		switch (aa_id & aa_mask)
		{
		case LOA::BASIC_AA:
			return "LLVM_ORIGINAL Basic Alias Analysis";
		case LOA::CFL_ANDERS_AA:
			return "LLVM_ORIGINAL Andersen Alias Analysis";
		case LOA::CFL_STEENS_AA:
			return "LLVM ORIGINAL Steensgaard`s Alias Analysis";
		case LOA::OBJ_CARC_AA:
			return "LLVM ORIGINAL ARC-aware Alias Analysis (only for objectiv-c)";
		case LOA::SCEV_AA:
			return "LLVM ORIGINAL Scalar Evolution Alias Analysis";
		case LOA::SCOPED_NA_AA:
			return "LLVM ORIGINAL Scoped No Alias Alias Analysis";
		case LOA::TBAA_AA:
			return "LLVM ORIGINAL Type Based Alias Analysis";
		}
		return "unknown analysis id, if this is shown in -h, there is something wrong configured!";
	}

	inline char* getCombinedDescription(aa_id aa_id)
	{
		using LOAC = LLVM_ORIGINAL_ANALYSIS_COMBINED;

		switch (aa_id & aa_mask)
		{
		case LOAC::CLANG_COMBINED_ANALYSIS:
			return "LLVM ORIGINAL COMBINED BasicAA->NoAilas->TBAA->CFLSteens";
		}

		return "unknown analysis id, if this is shown in -h, there is something wrong configured!";
	}
}

#endif //!BOAAA_COMMANDLINE_TYPES_H