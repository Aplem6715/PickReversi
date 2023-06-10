#include "search.h"
#include "board/bit.h"
#include "board/stone.h"
#include "movelist.h"
#include "search_result.h"

namespace solver
{
    Searcher::Searcher()
    {
        option_ = option_;
    }

    Searcher::~Searcher()
    {
    }

    void Searcher::Reset()
    {
        PROFILE(prof_ = bench::kProfileInit);
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

    void Searcher::Search(stone_t own, stone_t opp, SearchResult* result)
    {
        Position pos = Position::NoMove;

        PROFILE(
            std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
            start_time = std::chrono::high_resolution_clock::now();)

        SetStones(own, opp);
        if (nbEmpty_ <= option_.endDepth_)
        {
            EndRoot(result);
            PROFILE(prof_.depth = option_.endDepth_);
        }
        else
        {
            MidRoot(result);
            PROFILE(prof_.depth = option_.midDepth_);
        }

        PROFILE(
            std::chrono::time_point<std::chrono::high_resolution_clock> end_time = std::chrono::high_resolution_clock::now();
            prof_.duration                                                       = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() / 1000.0;
            prof_.pos                                                            = pos;
            prof_.score                                                          = result->GetBestMove()->value_;)
    }

    void Searcher::MakeMoveList(MoveList* moveList)
    {
        Move* prev    = moveList->moves_;
        Move* cursor  = moveList->moves_ + 1;
        Stone* stones = stones_;
        uint64_t mob  = CalcMobility64(stones->own_, stones->opp_);
        int nbMove    = 0;

        *prev = kDefaultMove;

        Position pos;
        for_bits(pos, mob)
        {
            ++nbMove;
            cursor->pos_   = pos;
            cursor->value_ = 0;
            cursor->flips_ = stones->CalcFlip(pos);

            prev->next_ = cursor;
            prev        = prev->next_;
            ++cursor;
        }
        prev->next_         = nullptr;
        moveList->length_   = nbMove;
        moveList->lastMove_ = moveList->moves_;
    }

    /* 中盤探索 */

    void Searcher::MidRoot(SearchResult* result)
    {
        score32_t lower     = kEvalMin - 1;
        score32_t upper     = kEvalMax + 1;
        score32_t bestScore = kEvalInvalid;
        score32_t score;

        MakeMoveList(result->moveList_);
        MoveList* moveList = result->moveList_;

        while (Move* move = moveList->GetNextBest())
        {
            Update(move, true);
            {
                switch (option_.method_)
                {
                case SearchMethod::MinMax:
                    score = -MidMinMax(option_.midDepth_ - 1, false);
                    break;
                case SearchMethod::AlphaBeta:
                    score = -MidAlphaBeta(-lower, -upper, option_.midDepth_ - 1, false);
                    break;
                default:
                    throw std::invalid_argument("Invalid search method specified.");
                    break;
                }
            }
            Restore(move, true);

            move->value_ = score;

            if (score > bestScore)
            {
                bestScore = score;
                // upper以上になること(=カット)はありえない
                if (bestScore > lower)
                {
                    lower = bestScore;
                }
            }
        }
    }

    score32_t Searcher::MidMinMax(int depth, bool passed)
    {
        if (depth == 0)
        {
            PROFILE(++prof_.leafCount);
            return eval_->Evaluate(nbEmpty_);
        }

        PROFILE(++prof_.nodeCount);
        score32_t bestScore = kEvalInvalid;
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
                UpdatePass();
                bestScore = -MidMinMax(depth, true);
                UpdatePass();
            }
        }
        else
        {
            while (const Move* move = moveList->GetNextBest())
            {
                Update(move, true);
                const score32_t score = -MidMinMax(depth - 1, false);
                Restore(move, true);

                if (score > bestScore)
                {
                    bestScore = score;
                }
            }
        }

        return bestScore;
    }

    score32_t Searcher::MidAlphaBeta(score32_t up_limit, score32_t low_limit, int depth, bool passed)
    {
        if (depth == 0)
        {
            PROFILE(++prof_.leafCount);
            return eval_->Evaluate(nbEmpty_);
        }

        PROFILE(++prof_.nodeCount);
        score32_t bestScore = kEvalInvalid;
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
                UpdatePass();
                bestScore = -MidAlphaBeta(-low_limit, -up_limit, depth, true);
                UpdatePass();
            }
        }
        else
        {
            score32_t lower = low_limit;
            while (const Move* move = moveList->GetNextBest())
            {
                Update(move, true);
                const score32_t score = -MidAlphaBeta(-lower, -up_limit, depth - 1, false);
                Restore(move, true);

                if (score > bestScore)
                {
                    bestScore = score;

                    if (score >= up_limit)
                    {
                        break;
                    }
                    else if (bestScore > lower)
                    {
                        lower = bestScore;
                    }
                }
            }
        }

        return bestScore;
    }

    /* 終盤探索 */

    void Searcher::EndRoot(SearchResult* result)
    {
        score32_t lower     = kEvalMin - 1;
        score32_t upper     = kEvalMax + 1;
        score32_t bestScore = kEvalInvalid;
        score32_t score;

        MakeMoveList(result->moveList_);
        MoveList* moveList = result->moveList_;

        while (Move* move = moveList->GetNextBest())
        {
            Update(move, true);
            {
                switch (option_.method_)
                {
                case SearchMethod::MinMax:
                    score = -EndMinMax(nbEmpty_, false);
                    break;
                case SearchMethod::AlphaBeta:
                    score = -EndAlphaBeta(-lower, -upper, nbEmpty_, false);
                    break;
                default:
                    throw std::invalid_argument("Invalid search method specified.");
                    break;
                }
            }
            Restore(move, true);

            move->value_ = score;

            if (score > bestScore)
            {
                bestScore = score;
                // upper以上になること(=カット)はありえない
                if (bestScore > lower)
                {
                    lower = bestScore;
                }
            }
        }
    }

    score32_t Searcher::EndMinMax(int depth, bool passed)
    {
        if (depth == 0)
        {
            PROFILE(++prof_.leafCount);
            return stones_->GetCountDiff() * kEvalStone;
        }

        PROFILE(++prof_.nodeCount);
        score32_t bestScore = kEvalInvalid;
        MoveList moveList[1];

        MakeMoveList(moveList);

        if (moveList->IsEmpty())
        {
            if (passed)
            {
                return stones_->GetCountDiff() * kEvalStone;
            }
            else
            {
                UpdatePass();
                bestScore = -EndMinMax(depth, true);
                UpdatePass();
            }
        }
        else
        {
            while (const Move* move = moveList->GetNextBest())
            {
                Update(move, true);
                const score32_t score = -EndMinMax(depth - 1, false);
                Restore(move, true);

                if (score > bestScore)
                {
                    bestScore = score;
                }
            }
        }

        return bestScore;
    }

    score32_t Searcher::EndAlphaBeta(score32_t up_limit, score32_t low_limit, int depth, bool passed)
    {
        if (depth == 0)
        {
            PROFILE(++prof_.leafCount);
            return stones_->GetCountDiff() * kEvalStone;
        }

        PROFILE(++prof_.nodeCount);
        score32_t bestScore = kEvalInvalid;
        MoveList moveList[1];

        MakeMoveList(moveList);

        if (moveList->IsEmpty())
        {
            if (passed)
            {
                return stones_->GetCountDiff() * kEvalStone;
            }
            else
            {
                UpdatePass();
                bestScore = -EndAlphaBeta(-low_limit, -up_limit, depth, true);
                UpdatePass();
            }
        }
        else
        {
            score32_t lower = low_limit;
            while (const Move* move = moveList->GetNextBest())
            {
                Update(move, true);
                const score32_t score = -EndAlphaBeta(-lower, -up_limit, depth - 1, false);
                Restore(move, true);

                if (score > bestScore)
                {
                    bestScore = score;

                    if (score >= up_limit)
                    {
                        break;
                    }
                    else if (bestScore > lower)
                    {
                        lower = bestScore;
                    }
                }
            }
        }

        return bestScore;
    }
}
