#include "board/bit.h"
#include "board/stone.h"
#include "eval/pattern_eval.h"
#include "eval/pos_eval.h"
#include "movelist.h"
#include "search.h"
#include "search_result.h"

#if ENABLE_DEPTH_TEMPLATE
#define EndAlphaBetaSwitch(upLimit, lowLimit, depth, passed) EndAlphaBeta<depth>(upLimit, lowLimit, passed)
#define EndPVSSwitch(upLimit, lowLimit, depth, passed) EndPVS<depth>(upLimit, lowLimit, passed)
#define EndNWSSwitch(upLimit, depth, passed) EndNWS<depth>(upLimit, passed)
#else
#define EndAlphaBetaSwitch(upLimit, lowLimit, depth, passed) EndAlphaBeta(upLimit, lowLimit, depth, passed)
#define EndPVSSwitch(upLimit, lowLimit, depth, passed) EndPVS(upLimit, lowLimit, depth, passed)
#define EndNWSSwitch(upLimit, depth, passed) EndNWS(upLimit, depth, passed)
#endif

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
        if (USE_HASH && depth >= kEndHashDepth)
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
            Update(move);
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
            Restore(move);

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

        if (USE_HASH && depth >= kEndHashDepth)
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
                Update(move);
                const score_t score = -EndMinMax(depth - 1, false);
                Restore(move);

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
    score_t Searcher<Evaluator>::EndAlphaBeta(const score_t upLimit, const score_t lowLimit, const bool passed)
#else
    template <class Evaluator>
    score_t Searcher<Evaluator>::EndAlphaBeta(const score_t upLimit, const score_t lowLimit, const int depth, const bool passed)
#endif
    {
        if (depth == 0)
        {
            PROFILE(++prof_.leafCount);
            return stones_.GetCountDiff();
        }
        PROFILE(++prof_.nodeCount);

        score_t lower     = lowLimit;
        score_t upper     = upLimit;
        Position bestMove = Position::NoMove;

        HashData hashData;
        const uint64_t hashCode = USE_HASH ? GetHashCode(stones_) : 0;
        if (USE_HASH && depth >= kEndHashDepth)
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
            if (USE_ORDER && depth >= kEndOrderingDepth)
            {
                moveList->Evaluate(*this, hashData);
            }
            while (const Move* move = moveList->GetNextBest())
            {
                Update(move);
                const score_t score = -EndAlphaBeta(-lower, -upper, depth - 1, false);
                Restore(move);

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

        if (USE_HASH && depth >= kEndHashDepth)
        {
            table_->Add(stones_, hashCode, upper, lower, bestScore, bestMove, 0, depth);
        }

        return bestScore;
    }

#if ENABLE_DEPTH_TEMPLATE
    template <class Evaluator>
    template <int depth>
    score_t Searcher<Evaluator>::EndPVS(const score_t upLimit, const score_t lowLimit, const bool passed)
#else
    template <class Evaluator>
    score_t Searcher<Evaluator>::EndPVS(const score_t upLimit, const score_t lowLimit, const int depth, const bool passed)
#endif
    {
        if (depth == 0)
        {
            PROFILE(++prof_.leafCount);
            return stones_.GetCountDiff();
        }

        PROFILE(++prof_.nodeCount);

        score_t lower = lowLimit;
        score_t upper = upLimit;

        /* Hash Cut */
        HashData hashData;
        const bool useHash      = USE_HASH && depth >= kEndHashDepth;
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
                return stones_.GetCountDiff();
            }
            else
            {
                UpdatePass();
                bestScore = -EndPVSSwitch(-lower, -upper, depth, true);
                UpdatePass();
                bestMove = Position::Pass;
            }
        }
        else
        {
            if (USE_ORDER && depth >= kEndOrderingDepth)
            {
                moveList->Evaluate(*this, hashData);
            }

            // first move
            {
                const Move* firstMove = moveList->GetNextBest();
                Update(firstMove);
                bestScore = -EndPVSSwitch(-lower, -upper, depth - 1, false);
                Restore(firstMove);

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
                Update(move);
                score_t score = -EndNWSSwitch(-lower, depth - 1, false);
                if (score > lower && score < upper)
                {
                    score = -EndPVSSwitch(-lower, -upper, depth - 1, false);
                    if (score > lower)
                    {
                        lower = score;
                    }
                }
                Restore(move);

                if (score > bestScore)
                {
                    bestScore = score;
                    bestMove  = move->pos_;
                    if (score >= upper)
                    {
                        break;
                    }
                    // NWSに成功したらここを通らない(score >= upper = lower + 1になる)のでNWS失敗時のみこの処理をするように上の別ブロックに移動
                    // else if (score > lower)
                    // {
                    //     lower = bestScore;
                    // }
                }
            }
        }

        if (USE_HASH && depth >= kEndHashDepth)
        {
            table_->Add(stones_, hashCode, upLimit, lowLimit, bestScore, bestMove, 0, depth);
        }

        return bestScore;
    }

#if ENABLE_DEPTH_TEMPLATE
    template <class Evaluator>
    template <int depth>
    score_t Searcher<Evaluator>::EndNWS(const score_t upLimit, const bool passed)
#else
    template <class Evaluator>
    score_t Searcher<Evaluator>::EndNWS(const score_t upLimit, const int depth, const bool passed)
#endif
    {
        // 非テンプレート利用の際のために残しておく。
        // テンプレート利用時は最適化により消える（はず）
        if (depth == 0)
        {
            PROFILE(++prof_.leafCount);
            return stones_.GetCountDiff();
        }

        PROFILE(++prof_.nodeCount);

        const score_t lower = upLimit - 1;
        const score_t upper = upLimit;

        /* Hash Cut */
        HashData hashData;
        const bool useHash      = USE_HASH && depth >= kEndHashDepth;
        const uint64_t hashCode = useHash ? GetHashCode(stones_) : 0;
        if (useHash)
        {
            if (table_->TryGetValue(stones_, hashCode, &hashData))
            {
                score_t score;
                if (ApplyHashRangeNWS(hashData, depth, lower, &score))
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
                return stones_.GetCountDiff();
            }
            else
            {
                UpdatePass();
                bestScore = -EndNWSSwitch(-lower, depth, true);
                UpdatePass();
                bestMove = Position::Pass;
            }
        }
        else
        {
            if (USE_ORDER && depth >= kEndOrderingDepth)
            {
                moveList->Evaluate(*this, hashData);
            }
            while (const Move* move = moveList->GetNextBest())
            {
                Update(move);
                const score_t score = -EndNWSSwitch(-lower, depth - 1, false);
                Restore(move);

                if (score > bestScore)
                {
                    bestScore = score;
                    bestMove  = move->pos_;

                    if (bestScore >= upper) // bestScore >= lower + 1
                    {
                        break;
                    }
                    // [lower = upper - 1]より，bestScore > lowerのときbestScore >= upperを満たすためここには到達しない
                    // else if (bestScore > lower)
                    // {
                    //     lower = bestScore;
                    // }
                }
            }
        }

        if (USE_HASH && depth >= kEndHashDepth)
        {
            // fail-softなのでbeta cut(score >= upLimit)された場合でも
            // score>=upperとなりNullWindow用に狭められたupper(lower+1)は（hash.upper更新条件score<upperにより)更新されない
            // lowLimitについてはPVノード下限真値にあたるため，記録されたものが別ノードに適用されても問題ない
            table_->Add(stones_, hashCode, upLimit, upLimit - 1, bestScore, bestMove, 0, depth);
        }

        return bestScore;
    }

// template深度制限
#if ENABLE_DEPTH_TEMPLATE
    template <>
    template <>
    score_t Searcher<eval::PatternEval>::EndAlphaBeta<0>(const score_t upLimit, const score_t lowLimit, const bool passed)
    {
        PROFILE(++prof_.leafCount);
        return stones_.GetCountDiff();
    }
    template <>
    template <>
    score_t Searcher<eval::PositionEval>::EndAlphaBeta<0>(const score_t upLimit, const score_t lowLimit, const bool passed)
    {
        PROFILE(++prof_.leafCount);
        return stones_.GetCountDiff();
    }

    template <>
    template <>
    score_t Searcher<eval::PatternEval>::EndPVS<0>(const score_t upLimit, const score_t lowLimit, const bool passed)
    {
        PROFILE(++prof_.leafCount);
        return stones_.GetCountDiff();
    }
    template <>
    template <>
    score_t Searcher<eval::PositionEval>::EndPVS<0>(const score_t upLimit, const score_t lowLimit, const bool passed)
    {
        PROFILE(++prof_.leafCount);
        return stones_.GetCountDiff();
    }

    template <>
    template <>
    score_t Searcher<eval::PatternEval>::EndNWS<0>(const score_t upLimit, const bool passed)
    {
        PROFILE(++prof_.leafCount);
        return stones_.GetCountDiff();
    }
    template <>
    template <>
    score_t Searcher<eval::PositionEval>::EndNWS<0>(const score_t upLimit, const bool passed)
    {
        PROFILE(++prof_.leafCount);
        return stones_.GetCountDiff();
    }
#endif
}
