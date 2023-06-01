#ifndef STONE_H
#define STONE_H

#include <cstdint>

struct Stone
{
public:
	uint64_t own;
	uint64_t opp;

	void Swap();
	void Update(uint64_t pos, uint64_t flips);
	void Restore(uint64_t pos, uint64_t flips);
};

#endif
