#if TRAIN_BUILD

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

    double PatternTrainer::Run(BatchBuffer& buffer, double testRatio)
    {
        const int numTrain = buffer.GetNumBatches() * (1.0 - testRatio);
        const int numTest  = buffer.GetNumBatches() - numTrain;
        Batch batch;

        /*  Train  */

        for (int i = 0; i < numTrain; ++i)
        {
            if (buffer.GetBatch(i, batch))
            {
                Train(batch, buffer.GetPhase());
            }
            else
            {
                assert(false);
            }
        }

        /*  Test  */

        double sumMAE = 0;
        for (int i = 0; i < numTest; ++i)
        {
            if (buffer.GetBatch(numTrain + i, batch))
            {
                sumMAE += Test(batch, buffer.GetPhase());
            }
        }
        return sumMAE / numTest;
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

        ApplyWeight();
        BuildWeight(eval_->weight_[0][0]);
    }

    void PatternTrainer::Train(const std::array<int, kPatternNum> states, int phase, int diff)
    {
        // 出現していないweightはdiff * 0なので
        // そもそも更新の必要がない

        // 出現したパターンのステートについて勾配を記録
        for (int i = 0; i < states.size(); ++i)
        {
            const int offset   = kPatternOffset[i];
            const int state    = states[i] - offset;
            const int symm     = GetSymmetry(i, state);
            const int oppState = GetFlipPattern(i, state);
            const int oppSymm  = GetSymmetry(i, oppState);

            const int ownIndex    = std::min(state, symm);
            const int oppIndex    = std::min(oppState, oppSymm);
            const int targetIndex = std::min(ownIndex, oppIndex);
            const bool isOpp      = targetIndex == oppIndex;
            const int grad        = (isOpp ? -1 : 1) * diff;

            // 敵味方反転パターンのほうがstateが小さいときはそっちを採用.
            // スコアを逆転して勾配計算.
            trainWeights_[0][phase][offset + targetIndex].AddGrad(grad);
        }
    }

    double PatternTrainer::Test(const Batch& testData, int phase)
    {
        double mae = 0;
        for (const auto& record : testData)
        {
            const auto stone = record->stone;
            eval_->Reload(stone.own_, stone.opp_);
            int pred = eval_->Evaluate(phase);
            mae += std::abs(record->result - pred);
        }
        mae /= testData.size();
        return mae;
    }

    void PatternTrainer::ApplyWeight()
    {
        TrainWeight* source = trainWeights_[0][0];
        int16_t* target     = eval_->weight_[0][0];

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

#endif