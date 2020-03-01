#ifndef BOAAA_LV_INITIALIZE_AAS_90_H
#define BOAAA_LV_INITIALIZE_AAS_90_H

#include "boaaa/commandline_types.h"

namespace boaaa
{
	enum LLVM_90_AA : aa_id
	{
		BASIC_AA = LLVM_90 + 1
	};

	inline cl_aa_store getInitalizedAAs_90()
	{
		cl_aa_store raa;
		//raa.push_back(registeredAA(BASIC_AA, ))
		return raa;
	}
}

#endif //!BOAAA_LV_INITALIZE_AAS_90_H