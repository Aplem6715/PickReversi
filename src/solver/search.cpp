#include "search.h"
#include "board/bit.h"
#include "board/stone.h"
#include "movelist.h"
#include "search_result.h"

namespace solver
{
    Searcher::Searcher()
    {
    }

    Searcher::~Searcher()
    {
    }

    void Searcher::Reset()
    {
    }

    void Searcher::Setup(SearchOption option)
    {
        option_ = option;
    }

    void Searcher::SetStones(stone_t own, stone_t opp)
    {
        nbEmpty_ = CountBits(~(own | opp));

        stones_->own_ = own;
        stones_->opp_ = opp;

        eval_->Reload(own, opp, Side::Own);
    }

    Position Searcher::Search()
    {
        Position pos = Position::NoMove;

        if (nbEmpty_ <= option_.endDepth_)
        {
            // pos = EndRoot();
        }
        else
        {
            // pos = MidRoot();
        }

        return pos;
    }

    Position Searcher::Search(std::vector<float> ratio)
    {
        // TODO: implement
        return Position::NoMove;
    }

    void Searcher::MakeMoveList(MoveList* moveList)
    {
        Move* cursor  = moveList->moves_;
        Move* prev    = nullptr;
        Stone* stones = stones_;
        uint64_t mob  = CalcMobility64(stones->own_, stones->opp_);
        int nbMove    = 0;

        Position pos;
        for_bits(pos, mob)
        {
            ++nbMove;
            cursor->pos_   = pos;
            cursor->value_ = 0;
            cursor->flips_ = stones->CalcFlip(pos);

            if (prev)
            {
                prev->next_ = cursor;
            }
            prev = cursor;
        }
        prev->next_       = nullptr;
        moveList->length_ = nbMove;
    }

    /* 中盤探索 */

    void Searcher::MidRoot(SearchResult* result)
    {
        score_t lower     = EvalMin;
        score_t upper     = EvalMax;
        score_t bestScore = EvalInvalid;

        MakeMoveList(result->moveList_);
        MoveList* moveList = result->moveList_;

        while (Move* move = moveList->GetNextBest())
        {
            UpdateMid(move);
            score_t score = -MidMinMax(option_.midDepth_, false);
            RestoreMid(move);

            if (score > bestScore)
            {
                bestScore    = score;
                move->value_ = score;
            }
        }
    }

    void Searcher::UpdateMid(const Move* move)
    {
        const uint64_t posBit = PosToBit(move->pos_);
        stones_->Update(posBit, move->flips_);
        --nbEmpty_;
    }

    void Searcher::RestoreMid(const Move* move)
    {
        const uint64_t posBit = PosToBit(move->pos_);
        stones_->Restore(posBit, move->flips_);
        ++nbEmpty_;
    }

    void Searcher::PassMid()
    {
        stones_->Swap();
    }

    score_t Searcher::MidMinMax(int depth, bool passed)
    {
        if (depth == 0)
        {
            return eval_->Evaluate(nbEmpty_);
        }

        score_t bestScore;
        MoveList moveList[1];

        MakeMoveList(moveList);

        if (moveList->IsEmpty())
        {
            if (passed)
            {
                return WinJudge();
            }
            else
            {
                PassMid();
                bestScore = -MidMinMax(depth - 1, true);
                PassMid();
            }
        }
        else
        {
            while (const Move* move = moveList->GetNextBest())
            {
                UpdateMid(move);
                const score_t score = -MidMinMax(depth - 1, false);
                RestoreMid(move);

                if (score > bestScore)
                {
                    bestScore = score;
                }
            }
        }

        return bestScore;
    }
}
