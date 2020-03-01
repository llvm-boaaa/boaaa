#ifndef BOAAA_COMMANDLINE_TYPES_H
#define BOAAA_COMMANDLINE_TYPES_H

#include <inttypes.h>
#include <vector>

#include "boaaa/support/data_store.h"

namespace boaaa
{
	/*
	 * LLVM_VERSION := MajorMinor (real llvm version Major.Minor.Patch)
	 * [LLVM_VERSION][______Data__________________________________]
	 * [12 bit      ][      20 bit                                ]
	 * 8 bit Major, 4 bit Minor
	 */
	typedef int32_t aa_id;

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
		LLVM_80  =  (8 << 24) + (0 << 20),
		LLVM_90  =  (9 << 24) + (0 << 20),
		LLVM_100 = (10 << 24) + (0 << 20),
		LLVM_110 = (11 << 24) + (0 << 20)
	};
}

#endif //!BOAAA_COMMANDLINE_TYPES_H