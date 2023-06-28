#include "board/bit.h"
#include "board/stone.h"
#include "eval/pattern_eval.h"
#include "eval/pos_eval.h"
#include "movelist.h"
#include "search.h"
#include "search_result.h"

#if ENABLE_DEPTH_TEMPLATE
#define MidAlphaBetaSwitch(upLimit, lowLimit, depth, passed) MidAlphaBeta<depth>(upLimit, lowLimit, passed)
#define MidPVSSwitch(upLimit, lowLimit, depth, passed) MidPVS<depth>(upLimit, lowLimit, passed)
#define MidNWSSwitch(upLimit, depth, passed) MidNWS<depth>(upLimit, passed)
#else
#define MidAlphaBetaSwitch(upLimit, lowLimit, depth, passed) MidAlphaBeta(upLimit, lowLimit, depth, passed)
#define MidPVSSwitch(upLimit, lowLimit, depth, passed) MidPVS(upLimit, lowLimit, depth, passed)
#define MidNWSSwitch(upLimit, depth, passed) MidNWS(upLimit, depth, passed)
#endif

namespace solver
{
    template class Searcher<eval::PositionEval>;
    template class Searcher<eval::PatternEval>;

    template <class Evaluator>
    void Searcher<Evaluator>::OnEnterMidSearch()
    {
        table_->ClearScore();
    }

    template <class Evaluator>
    void Searcher<Evaluator>::MidRoot(SearchResult* result)
    {
        score_t lower     = kEvalMin;
        score_t upper     = kEvalMax;
        score_t bestScore = kEvalInvalid;
        Position bestMove = Position::NoMove;
        score_t score;
        int depth = option_.midDepth_;

        if (!wasMidSearch_)
        {
            wasMidSearch_ = true;
            OnEnterMidSearch();
        }

        // RootではHashカットせずMoveOrderingのみで使用
        HashData hashData;
        const uint64_t hashCode = USE_HASH ? GetHashCode(stones_) : 0;
        if (USE_HASH && depth >= option_.midHashDepth)
        {
            table_->TryGetValue(stones_, hashCode, &hashData);
        }
        else
        {
            hashData = kInitHashData;
        }

        // Move ordering
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
                    score = -MidMinMax(depth - 1, false);
                    break;
                case SearchMethod::AlphaBeta:
                    score = -MidAlphaBetaSwitch(-lower, -upper, depth - 1, false);
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
                // upperカットはしない
                if (bestScore > lower)
                {
                    lower = bestScore;
                }
            }
        }

        if (USE_HASH && depth >= option_.midHashDepth)
        {
            table_->Add(stones_, hashCode, upper, lower, bestScore, bestMove, 0, depth);
        }
    }

    template <class Evaluator>
    score_t Searcher<Evaluator>::MidMinMax(int depth, bool passed)
    {
        if (depth == 0)
        {
            PROFILE(++prof_.leafCount);
            return eval_.Evaluate(Phase(nbEmpty_));
        }

        PROFILE(++prof_.nodeCount);
        score_t bestScore = kEvalInvalid;
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
                const score_t score = -MidMinMax(depth - 1, false);
                Restore(move, true);

                if (score > bestScore)
                {
                    bestScore = score;
                }
            }
        }

        return bestScore;
    }

#if ENABLE_DEPTH_TEMPLATE
    template <class Evaluator>
    template <int depth>
    score_t Searcher<Evaluator>::MidAlphaBeta(const score_t upLimit, const score_t lowLimit, const bool passed)
#else
    template <class Evaluator>
    score_t Searcher<Evaluator>::MidAlphaBeta(const score_t upLimit, const score_t lowLimit, const int depth, const bool passed)
#endif
    {
        if (depth == 0)
        {
            PROFILE(++prof_.leafCount);
            return eval_.Evaluate(Phase(nbEmpty_));
        }

        PROFILE(++prof_.nodeCount);

        score_t lower = lowLimit;
        score_t upper = upLimit;

        /* Hash Cut */
        HashData hashData;
        const bool useHash      = USE_HASH && depth >= option_.midHashDepth;
        const uint64_t hashCode = useHash ? GetHashCode(stones_) : 0;
        if (useHash)
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
        Position bestMove = Position::NoMove;
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
                bestScore = -MidAlphaBetaSwitch(-lower, -upper, depth, true);
                UpdatePass();
                bestMove = Position::Pass;
            }
        }
        else
        {
            if (USE_ORDER && depth >= option_.midOrderingDepth)
            {
                moveList->Evaluate(*this, hashData);
            }
            while (const Move* move = moveList->GetNextBest())
            {
                Update(move, true);
                const score_t score = -MidAlphaBetaSwitch(-lower, -upper, depth - 1, false);
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

        if (USE_HASH && depth >= option_.midHashDepth)
        {
            table_->Add(stones_, hashCode, upper, lower, bestScore, bestMove, 0, depth);
        }

        return bestScore;
    }

#if ENABLE_DEPTH_TEMPLATE
    template <class Evaluator>
    template <int depth>
    score_t Searcher<Evaluator>::MidPVS(const score_t upLimit, const score_t lowLimit, const bool passed)
#else
    template <class Evaluator>
    score_t Searcher<Evaluator>::MidPVS(const score_t upLimit, const score_t lowLimit, const int depth, const bool passed)
#endif
    {
        if (depth == 0)
        {
            PROFILE(++prof_.leafCount);
            return eval_.Evaluate(Phase(nbEmpty_));
        }

        PROFILE(++prof_.nodeCount);

        score_t lower = lowLimit;
        score_t upper = upLimit;

        /* Hash Cut */
        HashData hashData;
        const bool useHash      = USE_HASH && depth >= option_.midHashDepth;
        const uint64_t hashCode = useHash ? GetHashCode(stones_) : 0;
        if (useHash)
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
        Position bestMove = Position::NoMove;
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
                bestScore = -MidPVSSwitch(-lower, -upper, depth, true);
                UpdatePass();
                bestMove = Position::Pass;
            }
        }
        else
        {
            if (USE_ORDER && depth >= option_.midOrderingDepth)
            {
                moveList->Evaluate(*this, hashData);
            }

            // first move
            {
                const Move* firstMove = moveList->GetNextBest();
                Update(firstMove, true);
                bestScore = -MidPVSSwitch(-lower, -upper, depth - 1, false);
                Restore(firstMove, true);

                if (bestScore > lower)
                {
                    if (bestScore >= upper)
                    {
                        return bestScore;
                    }
                    lower = bestScore;
                }
            }

            // others
            while (const Move* move = moveList->GetNextBest())
            {
                Update(move, true);
                score_t score = -MidNWSSwitch(-lower, depth - 1, false);
                if (score > lower && score < upper)
                {
                    score = -MidPVSSwitch(-lower, -upper, depth - 1, false);
                    if (score > lower)
                    {
                        lower = score;
                    }
                }
                Restore(move, true);

                if (score > bestScore)
                {
                    bestScore = score;
                    bestMove  = move->pos_;
                    if (score >= upper)
                    {
                        break;
                    }
                    // NWSではここを通らないので再探索時のみlowerを更新するように上に移動
                    // else if (score > lower)
                    // {
                    //     lower = bestScore;
                    // }
                }
            }
        }

        if (USE_HASH && depth >= option_.midHashDepth)
        {
            table_->Add(stones_, hashCode, upLimit, lowLimit, bestScore, bestMove, 0, depth);
        }

        return bestScore;
    }

#if ENABLE_DEPTH_TEMPLATE
    template <class Evaluator>
    template <int depth>
    score_t Searcher<Evaluator>::MidNWS(const score_t upLimit, const bool passed)
#else
    template <class Evaluator>
    score_t Searcher<Evaluator>::MidNWS(const score_t upLimit, const int depth, const bool passed)
#endif
    {
        if (depth == 0)
        {
            PROFILE(++prof_.leafCount);
            return eval_.Evaluate(Phase(nbEmpty_));
        }

        PROFILE(++prof_.nodeCount);

        score_t lower = upLimit - 1;
        score_t upper = upLimit;

        /* Hash Cut */
        HashData hashData;
        const bool useHash      = USE_HASH && depth >= option_.midHashDepth;
        const uint64_t hashCode = useHash ? GetHashCode(stones_) : 0;
        if (useHash)
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
        Position bestMove = Position::NoMove;
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
                bestScore = -MidNWSSwitch(-lower, depth, true);
                UpdatePass();
                bestMove = Position::Pass;
            }
        }
        else
        {
            if (USE_ORDER && depth >= option_.midOrderingDepth)
            {
                moveList->Evaluate(*this, hashData);
            }
            while (const Move* move = moveList->GetNextBest())
            {
                Update(move, true);
                const score_t score = -MidNWSSwitch(-lower, depth, false);
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

        if (USE_HASH && depth >= option_.midHashDepth)
        {
            // fail-softなのでbeta cut(score >= upLimit)された場合でも
            // score < upperとなりhash.upperは更新されない（hash.lowerのみ更新）
            table_->Add(stones_, hashCode, upLimit, upLimit - 1, bestScore, bestMove, 0, depth);
        }

        return bestScore;
    }
}
