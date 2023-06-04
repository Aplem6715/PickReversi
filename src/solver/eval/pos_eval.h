﻿#ifndef POS_EVAL_H
#define POS_EVAL_H

#include "evaluator.h"

namespace eval
{
    class PositionEvaluator : Evaluator
    {
    public:
        virtual score_t Evaluate(int nEmpty) override;

        virtual void Reload(stone_t own, stone_t opp, Side side);
        virtual void Update(stone_t pos, stone_t flips);
        virtual void Restore(stone_t pos, stone_t flips);
        virtual void UpdatePass();

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