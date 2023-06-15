#include "pattern_trainer.h"
#include "../eval/pattern_eval.h"
#include "../eval/pattern_formatter.h"
#include "record_batcher.h"
#include <cmath>
#include <fstream>

namespace train
{
    PatternTrainer::PatternTrainer(PatternEval* eval)
        : eval_(eval)
    {
        trainWeights_ = eval::AllocPatternWeight<TrainWeight>();
    }

    PatternTrainer::~PatternTrainer()
    {
        eval::FreePatternWeight(trainWeights_);
        trainWeights_ = nullptr;
    }

    void PatternTrainer::Train(const Batch& batchData, int phase)
    {
        for (const auto& record : batchData)
        {
            const auto stone = record->stone;
            eval_->Reload(stone.own_, stone.opp_);
            int pred = eval_->Evaluate(phase);
            int diff = record->result - pred;

            const auto& state = eval_->state_;
            Train(state, phase, diff);
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
        for (int i = 0; i < states.size(); ++i)
        {
            int state = states[i];
            UnifySymmetry(i, &state);
            trainWeights_[0][phase][state].AddGrad(diff);
        }
    }

    void PatternTrainer::ApplyWeight()
    {
        TrainWeight* source = trainWeights_[0][0];
        short* target       = eval_->weight_[0][0];

        for (int i = 0; i < kWeightDataSize; i++)
        {
            double srcWeight = source[i].weight_;

            // Remap [-1 ~ 1] → [-64 ~ 64] → [-64*StoneVal ~ 64*StoneVal]
            srcWeight *= 64;
            srcWeight *= kWeightOneStone;
            srcWeight = std::round(srcWeight);

            target[i] = srcWeight;
        }
    }

    void PatternTrainer::ShareWeight(short* target)
    {
        constexpr int kSideOffset = kNumPhase * kNumWeight;
        for (int p = 0; p < kPatternNum; ++p)
        {
            int shape  = kPattern2Shape[p];
            int offset = kPatternOffset[p];
            for (int i = 0; i < kShapeIndexMax[shape]; i++)
            {
                int symmIndex = GetSymmetry(p, i);
                int oppIndex  = GetFlipPattern(p, i);
                if (i != symmIndex)
                {
                    target[offset + symmIndex] = target[offset + i];
                }
                if (i != oppIndex)
                {
                    target[kSideOffset + offset + oppIndex] = target[kSideOffset + offset + i];
                }
            }
        }
    }

    bool PatternTrainer::SaveCheckpoint(const std::string& path)
    {
        std::ofstream f(path, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
        if (f.fail())
        {
            return false;
        }
        f.write(reinterpret_cast<const char*>(trainWeights_[0][0]), kWeightDataSize * sizeof(TrainWeight));
        return true;
    }

    bool PatternTrainer::LoadCheckpoint(const std::string& path)
    {
        std::ifstream f(path, std::ios_base::in | std::ios_base::binary);
        if (f.fail())
        {
            return false;
        }
        f.read(reinterpret_cast<char*>(trainWeights_[0][0]), kWeightDataSize * sizeof(TrainWeight));

        // 累積勾配は0初期化（もったいないけど捨てる）
        for (size_t i = 0; i < kWeightDataSize; ++i)
        {
            trainWeights_[0][0][i].gradSum_ = 0;
        }

        return true;
    }
}