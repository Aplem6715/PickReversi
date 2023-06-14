#include "pattern_trainer.h"
#include "../eval/pattern_eval.h"
#include "record_batcher.h"

namespace train
{
    PatternTrainer::PatternTrainer()
    {
        trainWeights_ = eval::AllocPatternWeight<TrainWeight>();
    }

    PatternTrainer::~PatternTrainer()
    {
        eval::FreePatternWeight(trainWeights_);
    }

    void PatternTrainer::Train(const Batch& batchData, int phase)
    {
        for (const auto& record : batchData)
        {
            const auto stone = record->stone;
            eval_->Reload(stone.own_, stone.opp_);
            int pred = eval_->Evaluate(phase);

            const auto& state = eval_->state_;
            Train(state, phase, record->result - pred);
        }

        for (int i = 0; i < kNumWeight; i++)
        {
            trainWeights_[0][phase][i].UpdateAdam();
        }
    }

    void PatternTrainer::Train(const std::array<int, kPatternNum> states, int phase, int diff)
    {
        // 出現していないweightはdiff * 0なので
        // そもそも更新の必要がない

        // 出現したパターンのステートについて勾配を記録
        for (const auto state : states)
        {
            trainWeights_[0][phase][state].AddGrad(diff);
        }
    }
}