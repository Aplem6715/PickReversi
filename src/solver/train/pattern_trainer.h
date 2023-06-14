#ifndef PATTERN_TRAINER_H
#define PATTERN_TRAINER_H

#include "../eval/pattern.h"
#include "../eval/pos2pattern.h"
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

    struct TrainRecord;
    class RecordBatcher;

    using Batch = std::vector<const TrainRecord*>;

    constexpr double kAdamBeta1 = 0.9;
    constexpr double kAdamBeta2 = 0.999;
    constexpr double kAdamAlpha = 0.001;
    constexpr double kAdamEps   = 1e-12;

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

            const double fixMoment = moment_ / (1.0f - std::pow(kAdamBeta1, numUpdate));
            const double fixV      = v_ / (1.0f - std::pow(kAdamBeta2, numUpdate));

            weight_  = weight_ - kAdamAlpha * fixMoment / (sqrt(fixV) + kAdamEps);
            // 更新したらリセット
            gradSum_ = 0;
        }
    };

    class PatternTrainer
    {
    public:
        PatternTrainer();
        ~PatternTrainer();

        void Train(const Batch& batchData, int phase);

    private:
        PatternEval* eval_;

        // 学習用weight
        TrainWeight*** trainWeights_;

        void Train(const std::array<int, kPatternNum> states, int phase, int diff);
    };

}
#endif