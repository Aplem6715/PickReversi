#ifndef STONE_H
#define STONE_H

#include <cstdint>
#include "bit.h"

struct Stone
{
public:
	uint64_t own;
	uint64_t opp;

	void Swap();
	void Update(uint64_t pos, uint64_t flips);
	void Restore(uint64_t pos, uint64_t flips);

	uint64_t CalcFlip(const Position index) { return CalcFlip64(own, opp, index); }
	uint64_t CalcMobility() { return CalcMobility64(own, opp); }
};

#endif
