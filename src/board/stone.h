#ifndef STONE_H
#define STONE_H

#include "bit.h"
#include <cstdint>

namespace board
{
    struct Stone
    {
    public:
        uint64_t own_;
        uint64_t opp_;

        void Swap();
        void Update(uint64_t pos, uint64_t flips);
        void Restore(uint64_t pos, uint64_t flips);
        int GetCountDiff();

        uint64_t CalcFlip(const Position index) { return CalcFlip64(own_, opp_, index); }
        uint64_t CalcMobility() { return CalcMobility64(own_, opp_); }
    };

    inline void Stone::Swap()
    {
        // 勝手に最適化されるだろう
        auto temp = own_;
        own_      = opp_;
        opp_      = temp;
    }

    inline void Stone::Update(uint64_t pos, uint64_t flips)
    {
        own_ ^= (flips | pos);
        opp_ ^= flips;
        Swap();
    }

    inline void Stone::Restore(uint64_t pos, uint64_t flips)
    {
        Swap();
        own_ ^= (flips | pos);
        opp_ ^= flips;
    }

    inline int Stone::GetCountDiff()
    {
        return CountBits(own_) - CountBits(opp_);
    }
}
#endif
