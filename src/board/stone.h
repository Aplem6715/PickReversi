#ifndef STONE_H
#define STONE_H

#include "bit.h"
#include <cstdint>

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

inline void Stone::Swap()
{
    // 勝手に最適化されるだろう
    auto temp = own;
    own       = opp;
    opp       = temp;
}

inline void Stone::Update(uint64_t pos, uint64_t flips)
{
    own = own ^ (flips | pos);
    opp = opp ^ flips;
    Swap();
}

inline void Stone::Restore(uint64_t pos, uint64_t flips)
{
    Swap();
    own = own ^ flips;
    opp = opp ^ (flips | pos);
}

#endif
