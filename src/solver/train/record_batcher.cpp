#include "record_batcher.h"
#include "board/board.h"
#include "game/record.h"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <random>

namespace train
{
    using Board = board::Board;
    using Stone = board::Stone;

    RecordBatcher::RecordBatcher(int batchSize)
        : batchSize_(batchSize)
    {
    }

    void RecordBatcher::Load(const MatchBook& book)
    {
        for (const auto rec : book.GetRecords())
        {
            Load1Match(rec);
        }
        isDirty_ = true;
    }

    void RecordBatcher::Load1Match(const MatchRecord& record)
    {
        Board board;
        int result = record.finalDiff_;

        const Position* pos = record.moves_;
        for (int i = 0; i < record.nMoves_; ++i)
        {
            board.Put(*pos);

            if (i >= record.nRandMoves_)
            {
                const int phase        = Phase(60 - i - 1 /*上でPutした分-1*/);
                const Color side       = board.GetSide();
                const int resultForOwn = result * (side == Color::Black ? 1 : -1);

                TrainRecord* dataPtr;
                auto&& stone = Stone{board.GetOwn(), board.GetOpp()};
                dataPtr      = storage_.Store({stone, resultForOwn});
                buffer_[phase].push_back(dataPtr);

                // 反転
                stone   = Stone{board.GetOpp(), board.GetOwn()};
                dataPtr = storage_.Store({stone, -resultForOwn});
                buffer_[phase].push_back(dataPtr);
            }

            ++pos;
        }
    }

    bool RecordBatcher::ExtractBatch(int phase, Batch& batch)
    {
        return ExtractBatch(phase, batchSize_, batch);
    }

    bool RecordBatcher::ExtractBatch(int phase, int batchSize, Batch& batch)
    {
        auto& target = buffer_[phase];
        if (target.size() < batchSize)
        {
            return false;
        }
        if (isDirty_)
        {
            Shuffle();
        }

        auto start = target.end() - batchSize;
        auto end   = target.end();
        std::move(start, end, std::back_inserter(batch));

        target.resize(target.size() - batchSize);

        return true;
    }

    void RecordBatcher::Shuffle()
    {
        std::random_device seed_gen;
        std::mt19937 engine(seed_gen());
        for (int p = 0; p < kNumPhase; ++p)
        {
            std::shuffle(buffer_[p].begin(), buffer_[p].end(), engine);
        }
    }

    void RecordBatcher::ReleaseBatch(int phase, Batch& batch)
    {
        for (auto b : batch)
        {
            storage_.Remove(b);
        }
    }
}
