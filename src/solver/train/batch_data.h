#if TRAIN_BUILD

#ifndef BATCH_DATA_H
#define BATCH_DATA_H

#include "board/stone.h"
#include "train_const.h"
#include <vector>
#include <array>

namespace train
{
    struct TrainRecord
    {
        // 盤面
        board::Stone stone;
        // 最終石差（own視点）
        int result;
    };

    using Batch  = std::array<const TrainRecord*, kBatchSize>;
    using Buffer = std::vector<const TrainRecord*>;

    class BatchBuffer
    {
    public:
        BatchBuffer(int batchSize, int bufferSize, int phase) : batchSize_(batchSize), bufferSize_(bufferSize), phase_(phase), isDirty_(false){};

        bool GetBatch(int batchId, Batch& batch);

        // clang-format off
        size_t          Size()          const { return buffer_.size(); }
        const Buffer&   GetBuffer()     const { return buffer_; }
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

#pragma region for Friend
        void Add(const TrainRecord* record);
        void Clear() { buffer_.clear(); }
#pragma endregion

        void Shuffle();

        friend class ReplayBuffer;
    };
}

#endif
#endif
