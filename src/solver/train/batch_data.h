#if TRAIN_BUILD

#ifndef BATCH_DATA_H
#define BATCH_DATA_H

#include "board/stone.h"
#include "train_const.h"
#include <array>
#include <random>
#include <span>
#include <vector>

namespace train
{
    struct TrainRecord
    {
        // 盤面
        board::Stone stone;
        // 最終石差（own視点）
        int result;
    };

    using Batch  = std::span<TrainRecord, std::dynamic_extent>;
    using Buffer = std::vector<TrainRecord>;

    class BatchBuffer
    {
    public:
        BatchBuffer(int batchSize, int bufferSize, int phase);

        Batch GetBatch(int batchId);
        void Shuffle();

        // clang-format off
        size_t          Size()          const { return buffer_.size(); }
        int             GetNumBatches() const { return buffer_.size() / batchSize_; }
        int             GetPhase()      const { return phase_; }
        bool            IsFull()        const { return GetNumBatches() >= bufferSize_; }
        // clang-format on

    private:
        Buffer buffer_;
        int batchSize_;
        int bufferSize_;
        int phase_;
        bool isDirty_;
        std::mt19937 randEngine_;

#pragma region for Friend
        void Add(const TrainRecord& record);
        void Clear() { buffer_.clear(); }
#pragma endregion

        friend class ReplayBuffer;
    };
}

#endif
#endif
