#include "pattern_trainer.h"

namespace train
{
    PatternTrainer::PatternTrainer()
    {
        trainWeight_ = eval::AllocPatternWeight<double>();
        gradSum_     = eval::AllocPatternWeight<double>();
        moment_      = eval::AllocPatternWeight<double>();
        v_           = eval::AllocPatternWeight<double>();
    }

    PatternTrainer::~PatternTrainer()
    {
        eval::FreePatternWeight(trainWeight_);
        eval::FreePatternWeight(gradSum_);
        eval::FreePatternWeight(moment_);
        eval::FreePatternWeight(v_);
    }

}