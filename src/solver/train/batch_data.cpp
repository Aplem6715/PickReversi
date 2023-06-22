#if TRAIN_BUILD

#include "batch_data.h"
#include <algorithm>
#include <iterator>
#include <random>

namespace train
{

    bool BatchBuffer::GetBatch(int batchId, Batch& batch)
    {
        if (batchId < 0 || batchId > GetNumBatches())
        {
            return false;
        }

        if (isDirty_)
        {
            Shuffle();
        }

        const auto batchHead = batchId * batchSize_;
        for (int i = 0; i < batchSize_; ++i)
        {
            batch[i] = buffer_[batchHead + i];
        }

        return true;
    }

    void BatchBuffer::Add(const TrainRecord* record)
    {
        buffer_.push_back(record);
        isDirty_ = true;
    }

    void BatchBuffer::Shuffle()
    {
        std::random_device seed_gen;
        std::mt19937 engine(seed_gen());
        for (int p = 0; p < kNumPhase; ++p)
        {
            std::shuffle(buffer_.begin(), buffer_.end(), engine);
        }
        isDirty_ = false;
    }
}

#endif
