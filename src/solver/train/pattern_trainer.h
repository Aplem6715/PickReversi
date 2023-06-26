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
        uint32_t gradCount_;
        uint32_t numUpdate_;

        TrainWeight() : weight_(0), gradSum_(0), moment_(0), v_(0), gradCount_(0), numUpdate_(0) {}

        void AddGrad(double grad)
        {
            gradSum_ += grad;
            ++gradCount_;
        }

        void UpdateAdam()
        {
            if (gradSum_ == 0 || gradCount_ == 0)
            {
                return;
            }
            const double grad = gradSum_ / gradCount_;
            ++numUpdate_;

            moment_ = kAdamBeta1 * moment_ + (1 - kAdamBeta1) * grad;
            v_      = kAdamBeta2 * v_ + (1 - kAdamBeta2) * grad * grad;

            const double m = moment_ / (1.0f - std::pow(kAdamBeta1, numUpdate_));
            const double v = v_ / (1.0f - std::pow(kAdamBeta2, numUpdate_));

            weight_ = weight_ - kAdamAlpha * m / (sqrt(v) + kAdamEps);

            // 更新したらリセット
            gradSum_   = 0;
            gradCount_ = 0;
        }
    };

    class PatternTrainer
    {
    public:
        PatternTrainer(PatternEval* eval);
        ~PatternTrainer();

        double Run(BatchBuffer& buffer, double* trainMAE, std::ofstream& outCsv);

        bool SaveCheckpoint(const std::string& path);
        bool LoadCheckpoint(const std::string& path);

    private:
        PatternEval* eval_;

        // 学習用weight
        TrainWeight*** trainWeights_;

        double Train(const Batch batchData, int phase);
        void Train(const std::array<state_t, kPatternNum> states, int phase, int diff);
        
        void ApplyWeight(int phase);
    };

}

#endif
#endif
