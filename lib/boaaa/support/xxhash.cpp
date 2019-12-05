#include "boaaa/support/xxhash.h"

using namespace boaaa;

static uint64_t rotl64(uint64_t X, size_t R) {
	return (X << R) | (X >> (64 - R));
}

static const uint64_t PRIME64_1 = 11400714785074694791ULL;
static const uint64_t PRIME64_2 = 14029467366897019727ULL;
static const uint64_t PRIME64_3 = 1609587929392839161ULL;
static const uint64_t PRIME64_4 = 9650029242287828579ULL;
static const uint64_t PRIME64_5 = 2870177450012600261ULL;

static uint64_t round(uint64_t Acc, uint64_t Input) {
	Acc += Input * PRIME64_2;
	Acc = rotl64(Acc, 31);
	Acc *= PRIME64_1;
	return Acc;
}

static uint64_t mergeRound(uint64_t Acc, uint64_t Val) {
	Val = round(0, Val);
	Acc ^= Val;
	Acc = Acc * PRIME64_1 + PRIME64_4;
	return Acc;
}

static uint32_t read32(const uint8_t* p)
{
	return *((uint32_t*)p);
}

static uint64_t read64(const uint8_t* p)
{
	return *((uint64_t*)p);
}

uint64_t xxhash(const uint8_t* beg, size_t len, uint64_t Seed)
{
	const uint8_t* P = beg;
	const uint8_t* const BEnd = beg + len;
	uint64_t H64;

	if (len >= 32) {
		const unsigned char* const Limit = BEnd - 32;
		uint64_t V1 = Seed + PRIME64_1 + PRIME64_2;
		uint64_t V2 = Seed + PRIME64_2;
		uint64_t V3 = Seed + 0;
		uint64_t V4 = Seed - PRIME64_1;

		do {
			V1 = round(V1, read64(P));
			P += 8;
			V2 = round(V2, read64(P));
			P += 8;
			V3 = round(V3, read64(P));
			P += 8;
			V4 = round(V4, read64(P));
			P += 8;
		} while (P <= Limit);

		H64 = rotl64(V1, 1) + rotl64(V2, 7) + rotl64(V3, 12) + rotl64(V4, 18);
		H64 = mergeRound(H64, V1);
		H64 = mergeRound(H64, V2);
		H64 = mergeRound(H64, V3);
		H64 = mergeRound(H64, V4);

	}
	else {
		H64 = Seed + PRIME64_5;
	}

	H64 += (uint64_t)len;

	while (P + 8 <= BEnd) {
		uint64_t const K1 = round(0, read64(P));
		H64 ^= K1;
		H64 = rotl64(H64, 27) * PRIME64_1 + PRIME64_4;
		P += 8;
	}

	if (P + 4 <= BEnd) {
		H64 ^= (uint64_t)(read32(P)) * PRIME64_1;
		H64 = rotl64(H64, 23) * PRIME64_2 + PRIME64_3;
		P += 4;
	}

	while (P < BEnd) {
		H64 ^= (*P) * PRIME64_5;
		H64 = rotl64(H64, 11) * PRIME64_1;
		P++;
	}

	H64 ^= H64 >> 33;
	H64 *= PRIME64_2;
	H64 ^= H64 >> 29;
	H64 *= PRIME64_3;
	H64 ^= H64 >> 32;

	return H64;
}