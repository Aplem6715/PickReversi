#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "../../const.h"

namespace eval
{
    class Evaluator
    {
    public:
        Evaluator(){};
        ~Evaluator(){};

        virtual score_t Evaluate(int nEmpty) = 0;

        virtual void Reload(stone_t own, stone_t opp, Side side) = 0;
        virtual void Update(stone_t pos, stone_t flips)          = 0;
        virtual void Restore(stone_t pos, stone_t flips)         = 0;
        virtual void UpdatePass()                                = 0;

        virtual void Clone(Evaluator* dst){};
    };

} // namespace eval

#endif