#ifndef BOAAA_XXHASH_H
#define BOAAA_XXHASH_H

#include <inttypes.h>
#if !defined(__LINUX__)
#include <stddef.h>
#endif

namespace boaaa
{
	uint64_t xxhash(const uint8_t* beg, size_t len, uint64_t Seed = 0);	
}


#endif