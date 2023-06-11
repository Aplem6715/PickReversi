#ifndef POS_EVAL_H
#define POS_EVAL_H

#include "evaluator.h"

namespace eval
{
    extern const score32_t ValueTable[64];
    
    class PositionEvaluator : public Evaluator
    {
    public:
        virtual score32_t Evaluate(int nEmpty) override;

        virtual void Reload(stone_t own, stone_t opp, Side side) override;
        virtual void Update(stone_t pos, stone_t flips) override;
        virtual void Restore(stone_t pos, stone_t flips) override;
        virtual void UpdatePass() override;

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
