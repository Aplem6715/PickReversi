#if TRAIN_BUILD

#include "record_batcher.h"
#include "board/board.h"
#include "game/record.h"

#include <algorithm>
#include <cassert>
#include <iterator>

namespace train
{
    using Board = board::Board;
    using Stone = board::Stone;

    ReplayBuffer::ReplayBuffer(int batchSize, int bufferSize)
    {
        for (int phase = 0; phase < kNumPhase; ++phase)
        {
            buffer_[phase] = new BatchBuffer(batchSize, bufferSize, phase);
        }
    }

    void ReplayBuffer::Load(const MatchBook& book)
    {
        for (const auto rec : book.GetRecords())
        {
            Load1Match(rec);
        }
    }

    void ReplayBuffer::Load1Match(const MatchRecord& record)
    {
        Board board;
        int result = record.finalDiff_;

        const Position* pos = record.moves_;
        for (int i = 0; i < record.nMoves_; ++i)
        {
            board.Put(*pos);

            if (i >= record.nRandMoves_)
            {
                const int nbEmpty      = 60 - i - 1 /*上でPutした分-1*/;
                const int phase        = Phase(nbEmpty);
                const Color side       = board.GetSide();
                const int resultForOwn = result * (side == Color::Black ? 1 : -1);

                TrainRecord* dataPtr;
                auto&& stone = Stone{board.GetOwn(), board.GetOpp()};
                dataPtr      = storage_.Store({stone, resultForOwn});
                AddRecord(dataPtr, nbEmpty);

                // 反転
                stone   = Stone{board.GetOpp(), board.GetOwn()};
                dataPtr = storage_.Store({stone, -resultForOwn});
                AddRecord(dataPtr, nbEmpty);
            }

            ++pos;
        }
    }

    void ReplayBuffer::Clear(int phase)
    {
        for (auto b : buffer_[phase]->GetBuffer())
        {
            storage_.Remove(b);
        }
        buffer_[phase]->Clear();
    }

    void ReplayBuffer::AddRecord(const TrainRecord* record, int nbEmpty)
    {
        const int first = Phase(nbEmpty - kSmoothRange);
        const int last  = Phase(nbEmpty + kSmoothRange);

        for (int phase = first; phase <= last; ++phase)
        {
            buffer_[phase]->Add(record);
        }
    }
}

#endif
