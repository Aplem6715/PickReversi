#ifndef RECORD_BATCHER_H
#define RECORD_BATCHER_H

#include "../eval/pattern.h"
#include "board/stone.h"
#include "util/storage.h"
#include <stack>
#include <vector>

namespace game
{
    struct MatchRecord;
    class MatchBook;
}

namespace train
{
    using namespace eval;
    using namespace game;

    // sizeof(TrainRecord) = 24[byte] * 1 << 24 => 402[MB]
    constexpr int kStorageCapacity = 1 << 24;

    struct TrainRecord
    {
        // 盤面
        board::Stone stone;
        // 最終石差（own視点）
        int result;
    };

    using Batch = std::vector<const TrainRecord*>;

    class RecordBatcher
    {
    public:
        RecordBatcher(int batchSize);

        void Load(const MatchBook& book);
        void Load1Match(const MatchRecord& record);

        bool CanMakeBatch(int phase) { return buffer_[phase].size() > batchSize_; }
        bool ExtractBatch(int phase, Batch& batch);
        bool ExtractBatch(int phase, int batchSize, Batch& batch);
        void Shuffle();

        void ReleaseBatch(int phase, Batch& batch);

    private:
        const int batchSize_;
        bool isDirty_;
        Batch buffer_[kNumPhase];
        // sizeof(TrainRecord) = 24[byte] * 1 << 24 => 402[MB]
        Storage<TrainRecord, kStorageCapacity> storage_;
    };
}

#endif