#ifndef PATTERN_TRAINER_H
#define PATTERN_TRAINER_H

#if TRAIN_BUILD

#include "../eval/pattern.h"
#include "../eval/pos2pattern.h"
#include "batch_data.h"
#include "train_const.h"
#include <functional>
#include <math.h>
#include <vector>

namespace eval
{
    class PatternEval;
}

namespace game
{
    struct MatchBook;
}

namespace train
{
    using namespace eval;

    struct TrainWeight
    {
        double weight_;
        double gradSum_;
        double moment_;
        double v_;
        uint32_t numUpdate;

        TrainWeight() : weight_(0), gradSum_(0), moment_(0), v_(0), numUpdate(0) {}

        void AddGrad(double grad) { gradSum_ += grad; }
        void UpdateAdam()
        {
            if (gradSum_ == 0)
            {
                return;
            }
            moment_ = kAdamBeta1 * moment_ + (1 - kAdamBeta1) * gradSum_;
            v_      = kAdamBeta2 * v_ + (1 - kAdamBeta2) * gradSum_ * gradSum_;

            const double m = moment_ / (1.0f - std::pow(kAdamBeta1, numUpdate));
            const double v = v_ / (1.0f - std::pow(kAdamBeta2, numUpdate));

            weight_ = weight_ - kAdamAlpha * m / (sqrt(v) + kAdamEps);
            // 更新したらリセット
            gradSum_ = 0;
        }
    };

    class PatternTrainer
    {
    public:
        PatternTrainer(PatternEval* eval);
        ~PatternTrainer();

        double Run(BatchBuffer& buffer, double testRatio);

        bool SaveCheckpoint(const std::string& path);
        bool LoadCheckpoint(const std::string& path);

    private:
        PatternEval* eval_;

        // 学習用weight
        TrainWeight*** trainWeights_;

        void Train(const Batch& batchData, int phase);
        void Train(const std::array<int, kPatternNum> states, int phase, int diff);
        double Test(const Batch& testData, int phase);

        void ApplyWeight();
    };

}

#endif
#endif
