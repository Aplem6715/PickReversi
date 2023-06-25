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

    double PatternTrainer::Run(BatchBuffer& buffer, double* trainMAE, std::ofstream& outCSV)
    {
        const int numTrain = buffer.GetNumBatches() - kNumTestBatch;
        Batch batch;

        buffer.Shuffle();

        double mae = 0;
        for (int i = 0; i < numTrain; ++i)
        {
            /*  Train  */
            if (buffer.GetBatch(i, batch))
            {
                *trainMAE = Train(batch, buffer.GetPhase());
            }
            else
            {
                assert(false);
            }

            /* Test */
            double sumMAE = 0;
            const int end = buffer.GetNumBatches();
            for (int t = numTrain; t < end; ++t)
            {
                if (buffer.GetBatch(t, batch))
                {
                    sumMAE += Test(batch, buffer.GetPhase());
                }
            }
            mae = sumMAE / kNumTestBatch;
            outCSV << *trainMAE << "," << mae << std::endl;
        }

        return mae;
    }

    double PatternTrainer::Train(const Batch& batchData, int phase)
    {
        double diffSum = 0;
        for (const auto& record : batchData)
        {
            const auto stone = record->stone;
            eval_->Reload(stone.own_, stone.opp_);
            int pred = eval_->Evaluate(phase);
            int diff = record->result - pred;
            diffSum += std::abs(diff);
            if (diff != 0)
            {
                const auto& state = eval_->state_;
                Train(state, phase, diff);
            }
        }

        for (int i = 0; i < kNumWeight; i++)
        {
            trainWeights_[0][phase][i].UpdateAdam();
        }

        ApplyWeight(phase);
        BuildWeight(eval_->weight_[0][0]);
        return diffSum / batchData.size();
    }

    void PatternTrainer::Train(const std::array<state_t, kPatternNum> states, int phase, int diff)
    {
        // 出現していないweightはdiff * 0なので
        // そもそも更新の必要がない

        // 出現したパターンのステートについて勾配を記録
        for (int pattern = 0; pattern < states.size(); ++pattern)
        {
            const uint32_t offset = kPatternOffset[pattern];
            const state_t state   = states[pattern] - offset;
            const state_t symm    = GetSymmetryShape(kPattern2Shape[pattern], state);

            // 対称パターンのほうがstateが小さいときはそっちを採用.
            const state_t targetIndex = std::min(state, symm);

            trainWeights_[0][phase][offset + targetIndex].AddGrad(diff / 64.0);
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

    void PatternTrainer::ApplyWeight(int phase)
    {
        TrainWeight* source = trainWeights_[0][phase];
        int16_t* target     = eval_->weight_[0][phase];

        for (int i = 0; i < kNumWeight; i++)
        {
            double srcWeight = source[i].weight_;

            // Remap [-1 ~ 1] → [-64 ~ 64] → [-64*StoneVal ~ 64*StoneVal]
            srcWeight *= 64;
            srcWeight *= kWeightOneStone;
            srcWeight = std::round(srcWeight);
            srcWeight = std::clamp(srcWeight, static_cast<double>(INT16_MIN), static_cast<double>(INT16_MAX));

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