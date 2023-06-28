#include "board/bit.h"
#include "board/stone.h"
#include "eval/pattern_eval.h"
#include "eval/pos_eval.h"
#include "movelist.h"
#include "search.h"
#include "search_result.h"

namespace solver
{
    template class Searcher<eval::PositionEval>;
    template class Searcher<eval::PatternEval>;

    template <class Evaluator>
    void Searcher<Evaluator>::OnEnterEndSearch()
    {
        table_->ClearScore();
    }

    template <class Evaluator>
    void Searcher<Evaluator>::EndRoot(SearchResult* result)
    {
        score_t lower     = kEvalMin - 1;
        score_t upper     = kEvalMax + 1;
        score_t bestScore = kEvalInvalid;
        Position bestMove = Position::NoMove;
        score_t score;
        int depth = nbEmpty_;

        if (wasMidSearch_)
        {
            wasMidSearch_ = false;
            OnEnterEndSearch();
        }

        // RootではHashカットせずMoveOrderingのみで使用
        HashData hashData;
        const uint64_t hashCode = USE_HASH ? GetHashCode(stones_) : 0;
        if (USE_HASH && depth >= option_.endHashDepth)
        {
            table_->TryGetValue(stones_, hashCode, &hashData);
        }
        else
        {
            hashData = kInitHashData;
        }

        MakeMoveList(result->moveList_);
        MoveList* moveList = result->moveList_;
        if (USE_ORDER)
        {
            moveList->Evaluate(*this, hashData);
        }

        while (Move* move = moveList->GetNextBest())
        {
            Update(move, true);
            {
                switch (option_.method_)
                {
                case SearchMethod::MinMax:
                    score = -EndMinMax(depth - 1, false);
                    break;
                case SearchMethod::AlphaBeta:
                    score = -EndAlphaBeta(-lower, -upper, depth - 1, false);
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
                bestMove  = move->pos_;
                // upper以上になること(=カット)はありえない
                if (bestScore > lower)
                {
                    lower = bestScore;
                }
            }
        }

        if (USE_HASH && depth >= option_.endHashDepth)
        {
            table_->Add(stones_, hashCode, upper, lower, bestScore, bestMove, 0, depth);
        }
    }

    template <class Evaluator>
    score_t Searcher<Evaluator>::EndMinMax(int depth, bool passed)
    {
        if (depth == 0)
        {
            PROFILE(++prof_.leafCount);
            return stones_.GetCountDiff();
        }

        PROFILE(++prof_.nodeCount);
        score_t bestScore = kEvalInvalid;
        MoveList moveList[1];

        MakeMoveList(moveList);

        if (moveList->IsEmpty())
        {
            if (passed)
            {
                return stones_.GetCountDiff();
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
                const score_t score = -EndMinMax(depth - 1, false);
                Restore(move, true);

                if (score > bestScore)
                {
                    bestScore = score;
                }
            }
        }

        return bestScore;
    }

    template <class Evaluator>
    score_t Searcher<Evaluator>::EndAlphaBeta(const score_t up_limit, const score_t low_limit, const int depth, const bool passed)
    {
        if (depth == 0)
        {
            PROFILE(++prof_.leafCount);
            return stones_.GetCountDiff();
        }
        PROFILE(++prof_.nodeCount);

        score_t lower     = low_limit;
        score_t upper     = up_limit;
        Position bestMove = Position::NoMove;

        HashData hashData;
        const uint64_t hashCode = USE_HASH ? GetHashCode(stones_) : 0;
        if (USE_HASH && depth >= option_.endHashDepth)
        {
            if (table_->TryGetValue(stones_, hashCode, &hashData))
            {
                score_t score;
                if (ApplyHashRange(hashData, depth, &lower, &upper, &score))
                {
                    return score;
                }
            }
        }
        else
        {
            hashData = kInitHashData;
        }

        score_t bestScore = kEvalInvalid;
        MoveList moveList[1];
        MakeMoveList(moveList);

        if (moveList->IsEmpty())
        {
            if (passed)
            {
                return stones_.GetCountDiff();
            }
            else
            {
                UpdatePass();
                bestScore = -EndAlphaBeta(-lower, -upper, depth, true);
                UpdatePass();
                bestMove = Position::Pass;
            }
        }
        else
        {
            if (USE_ORDER && depth >= option_.endOrderingDepth)
            {
                moveList->Evaluate(*this, hashData);
            }
            while (const Move* move = moveList->GetNextBest())
            {
                Update(move, true);
                const score_t score = -EndAlphaBeta(-lower, -upper, depth - 1, false);
                Restore(move, true);

                if (score > bestScore)
                {
                    bestScore = score;
                    bestMove  = move->pos_;

                    if (score >= upper)
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

        if (USE_HASH && depth >= option_.endHashDepth)
        {
            table_->Add(stones_, hashCode, upper, lower, bestScore, bestMove, 0, depth);
        }

        return bestScore;
    }
}
