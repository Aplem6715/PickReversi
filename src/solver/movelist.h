#ifndef MOVELIST_H
#define MOVELIST_H

#include "const.h"

namespace solver
{

    // 1盤面における最大着手可能位置数
    // 棋譜：f5f6e6f4g7c6g3e7d6f3e3d3b7d7c2g2g1c3b2b3b4f7g5c4c7c8e2
    // 参考：https://eukaryote.hateblo.jp/entry/2023/05/17/163629
    constexpr int MaxMove = 33;

    struct Move
    {
        stone_t flips_;
        int value_;
        Position pos_;
        Move* next_;
    };

    const Move kDefaultMove = {0, kEvalInvalid, Position::Invalid, nullptr};

    struct MoveList
    {
    public:
        // 順不定
        Move moves_[MaxMove];
        int length_;

        Move* lastMove_;

    public:
        Move* GetNextBest();
        Move* GetNext();
        bool IsEmpty() { return length_ == 0; }
    };

}

#endif