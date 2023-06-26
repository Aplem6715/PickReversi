#if TRAIN_BUILD

#include "batch_data.h"
#include <algorithm>
#include <iterator>
#include <stdexcept>

namespace train
{
    BatchBuffer::BatchBuffer(int batchSize, int bufferSize, int phase)
        : batchSize_(batchSize), bufferSize_(bufferSize), phase_(phase), isDirty_(false)
    {
        randEngine_.seed(kSeed);
        buffer_.reserve(kReplayBufferSize*2);
    }

    Batch BatchBuffer::GetBatch(int batchId)
    {
        if (batchId < 0 || batchId > GetNumBatches())
        {
            throw std::out_of_range("batchId out of range");
        }

        if (isDirty_)
        {
            Shuffle();
        }

        const auto batchHead = batchId * batchSize_;
        return std::span<TrainRecord>{buffer_}.subspan(batchHead, kBatchSize);
    }

    void BatchBuffer::Add(const TrainRecord& record)
    {
        buffer_.push_back(record);
        isDirty_ = true;
    }

    void BatchBuffer::Shuffle()
    {
        for (int p = 0; p < kNumPhase; ++p)
        {
            std::shuffle(buffer_.begin(), buffer_.end(), randEngine_);
        }
        isDirty_ = false;
    }
}

#endif
