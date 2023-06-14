#ifndef POS_EVAL_H
#define POS_EVAL_H

#include "const.h"

namespace eval
{
    extern const score_t ValueTable[64];

    class PositionEval
    {
    public:
        score_t Evaluate(int phase);

        void Reload(stone_t own, stone_t opp);
        void Update(Position pos, stone_t flips);
        void Restore(Position pos, stone_t flips);
        void UpdatePass();

    private:
        void Swap()
        {
            // 勝手に最適化されるだろう
            auto temp = _own;
            _own      = _opp;
            _opp      = temp;
        };

    private:
        stone_t _own;
        stone_t _opp;

#ifdef UNIT_TEST
        friend class PositionEvaluator_Test;
#endif
    };
}
#endif
