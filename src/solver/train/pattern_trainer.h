#ifndef PATTERN_TRAINER_H
#define PATTERN_TRAINER_H

#include "../eval/pattern.h"
#include "../eval/pos2pattern.h"

namespace train
{
    using namespace eval;

    class PatternTrainer
    {
    public:
        PatternTrainer();
        ~PatternTrainer();

    private:
        double* trainWeight_[2][kNumPhase];
        double* gradSum[2][kNumPhase];
        double* moment_[2][kNumPhase];
        double* v_[2][kNumPhase];
    };

}
#endif