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
            // パス
            if (!board.CanPut())
            {
                board.ChangeSide();
            }

            assert(board.CheckLegalMove(*pos));
            board.Put(*pos);

            if (i >= record.nRandMoves_)
            {
                const int nbEmpty      = 60 - i - 1 /*上でPutした分-1*/;
                const Color side       = board.GetSide();
                const int resultForOwn = result * (side == Color::Black ? 1 : -1);

                assert(nbEmpty == CountBits(board.GetOwn() | board.GetOpp()));

                // 自分視点
                AddNewAllSymmetry(board.GetOwn(), board.GetOpp(), resultForOwn, nbEmpty);
                // 相手視点
                AddNewAllSymmetry(board.GetOpp(), board.GetOwn(), -resultForOwn, nbEmpty);
            }

            ++pos;
        }
    }

    void ReplayBuffer::Clear(int phase)
    {
        buffer_[phase]->Clear();
    }

    void ReplayBuffer::AddNewAllSymmetry(stone_t own, stone_t opp, int diff, int nbEmpty)
    {
        AddNewRecord(own, opp, diff, nbEmpty);
        // パターン側で対称になっているものはインデックスを統一しているため
        // 対称系は不要
        // AddNewRecord(ReverseH(own), ReverseH(opp), diff, nbEmpty);
        // AddNewRecord(ReverseV(own), ReverseV(opp), diff, nbEmpty);
        // AddNewRecord(ReverseDiagUp(own), ReverseDiagUp(opp), diff, nbEmpty);
        // AddNewRecord(ReverseDiagDown(own), ReverseDiagDown(opp), diff, nbEmpty);
        // AddNewRecord(RotateR(own), RotateR(opp), diff, nbEmpty);
        // AddNewRecord(RotateL(own), RotateL(opp), diff, nbEmpty);
        // AddNewRecord(Rotate180(own), Rotate180(opp), diff, nbEmpty);
    }

    void ReplayBuffer::AddNewRecord(stone_t own, stone_t opp, int diff, int nbEmpty)
    {
        Stone&& stone      = {own, opp};
        TrainRecord record = {stone, diff};

        const int first = Phase(nbEmpty - kSmoothRange);
        const int last  = Phase(nbEmpty + kSmoothRange);

        for (int phase = first; phase <= last; ++phase)
        {
            buffer_[phase]->Add(record);
        }
    }
}

#endif
