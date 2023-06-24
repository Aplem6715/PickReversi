#ifndef RECORD_BATCHER_H
#define RECORD_BATCHER_H

#include "../eval/pattern.h"
#include "batch_data.h"
#include "board/stone.h"
#include "train_const.h"
#include "util/storage.h"
#include <functional>
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

    class BatchBuffer;

    /// データを読み込み，バッチ分割する。
    /// ・一度ロードしたデータは内部ストレージに記録され，Clearを実行するまで保存される。
    /// ・phaseごとにバッファを持ち，初回バッチ取得時にシャッフルされる。
    class ReplayBuffer
    {
        /// 例(擬似コード)：
        ///     Load(book)
        ///     for epoch:
        ///         int n = GetNumBatches(phase)
        ///         for n:
        ///             GetBatch(batch)
        ///             Train(batch)
    public:
        ReplayBuffer(int batchSize, int bufferSize);

        void Load(const MatchBook& book);
        void Load1Match(const MatchRecord& record);

        bool IsBufferFull(int phase) { return buffer_[phase]->IsFull(); }
        void Clear(int phase);

        BatchBuffer* GetBatchBuffer(int phase) { return buffer_[phase]; }

    private:
        BatchBuffer* buffer_[kNumPhase];
        // sizeof(TrainRecord) = 24[byte] * 1 << 24 => 402[MB]
        Storage<TrainRecord, kStorageCapacity> storage_;

        void AddNewAllSymmetry(stone_t own, stone_t opp, int diff, int nbEmpty);
        void AddNewRecord(stone_t own, stone_t opp, int diff, int nbEmpty);
    };
}

#endif