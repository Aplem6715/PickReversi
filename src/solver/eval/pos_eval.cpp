#include "pos_eval.h"
#include "board/bit.h"
#include <algorithm>

namespace eval
{

    // clang-format off
    extern const score32_t ValueTable[64] = {
         30, -12,   0,  -1,  -1,   0, -12,  30,
        -12, -15,  -3,  -3,  -3,  -3, -15, -12,
          0,  -3,   0,  -1,  -1,   0,  -3,   0,
         -1,  -3,  -1,  -1,  -1,  -1,  -3,  -1,
         -1,  -3,  -1,  -1,  -1,  -1,  -3,  -1,
          0,  -3,   0,  -1,  -1,   0,  -3,   0,
        -12, -15,  -3,  -3,  -3,  -3, -15, -12,
         30, -12,   0,  -1,  -1,   0, -12,  30,
    };
    // clang-format on

    score32_t PositionEval::Evaluate(int nEmpty)
    {
        score32_t score     = 0;
        const stone_t own = _own;
        const stone_t opp = _opp;
        for (int i = 0; i < kBoardSize * kBoardSize; i++)
        {
            score += ((own >> i) & 1) * ValueTable[i];
        }
        for (int i = 0; i < kBoardSize * kBoardSize; i++)
        {
            score -= ((opp >> i) & 1) * ValueTable[i];
        }

        score = std::clamp(score, kEvalMin, kEvalMax);

        return score;
    }

    void PositionEval::Reload(stone_t own, stone_t opp)
    {
        _own = own;
        _opp = opp;
    }

    void PositionEval::Update(Position pos, stone_t flips)
    {
        _own = _own ^ (flips | PosToBit(pos));
        _opp = _opp ^ flips;
        Swap();
    }

    void PositionEval::Restore(Position pos, stone_t flips)
    {
        Swap();
        _own = _own ^ (flips | PosToBit(pos));
        _opp = _opp ^ flips;
    }

    void PositionEval::UpdatePass()
    {
        Swap();
    }

}
