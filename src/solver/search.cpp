#include "search.h"
#include "board/bit.h"
#include "board/stone.h"
#include "eval/pos_eval.h"
#include "movelist.h"
#include "search_result.h"

#include <cassert>

#if ENABLE_DEPTH_TEMPLATE
#define MidAlphaBetaSwitch(upLimit, lowLimit, depth, passed) MidAlphaBeta<depth>(upLimit, lowLimit, passed)
#else
#define MidAlphaBetaSwitch(upLimit, lowLimit, depth, passed) MidAlphaBeta(upLimit, lowLimit, depth, passed)
#endif

namespace solver
{
    template class Searcher<eval::PositionEval>;

    constexpr int kHashDepth  = 4;
    constexpr int kOrderDepth = 6;

    template <int N>
    concept hash_enable_depth = requires() {
        N > kHashDepth;
    };

    template <int N>
    concept order_enable_depth = requires() {
        N > kOrderDepth;
    };

    using Stone = board::Stone;

    /// @brief ハッシュデータを元に探索範囲を狭める（またはカット）
    /// @param hashData ハッシュデータ
    /// @param depth 探索深度
    /// @param lower 下限値
    /// @param upper 上限値
    /// @param score スコア
    /// @return カットされるならtrue
    inline bool ApplyHashRange(const HashData& hashData, const int depth, score_t* lower, score_t* upper, score_t* score)
    {
        assert(hashData.lower_ <= hashData.upper_);

        if (hashData.depth_ == depth)
        {
            if (hashData.lower_ == hashData.upper_)
            {
                *score = hashData.upper_;
                return true;
            }
            if (hashData.upper_ <= *lower)
            {
                *score = hashData.upper_;
                return true;
            }
            if (hashData.lower_ >= *upper)
            {
                *score = hashData.lower_;
                return true;
            }

            *lower = std::max(*lower, static_cast<score_t>(hashData.lower_));
            *upper = std::min(*upper, static_cast<score_t>(hashData.upper_));
        }

        return false;
    }

    template <class Evaluator>
    Searcher<Evaluator>::Searcher()
    {
        option_ = option_;
        table_  = new HashTable(option_.hashSize_);
        PROFILE(table_->BindProf(&prof_.hash));
    }

    template <class Evaluator>
    Searcher<Evaluator>::~Searcher()
    {
        delete table_;
    }

    template <class Evaluator>
    void Searcher<Evaluator>::Reset()
    {
        table_->Clear();
        PROFILE(prof_ = bench::kProfileInit);
    }

    template <class Evaluator>
    void Searcher<Evaluator>::Setup(SearchOption option)
    {
        option_ = option;
        delete table_;
        table_ = new HashTable(option.hashSize_);
    }

    template <class Evaluator>
    void Searcher<Evaluator>::SetStones(stone_t own, stone_t opp)
    {
        nbEmpty_ = CountBits(~(own | opp));

        stones_.own_ = own;
        stones_.opp_ = opp;

        eval_.Reload(own, opp);
    }

    template <class Evaluator>
    void Searcher<Evaluator>::Search(stone_t own, stone_t opp, SearchResult* result)
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

        table_->VersionUp();

        PROFILE(
            std::chrono::time_point<std::chrono::high_resolution_clock> end_time = std::chrono::high_resolution_clock::now();
            prof_.duration                                                       = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() / 1000.0;
            prof_.pos                                                            = pos;
            prof_.score                                                          = result->GetBestMove()->value_;)
    }

    template <class Evaluator>
    void Searcher<Evaluator>::OnEnterMidSearch()
    {
        table_->ClearScore();
    }

    template <class Evaluator>
    void Searcher<Evaluator>::OnEnterEndSearch()
    {
        table_->ClearScore();
    }

    template <class Evaluator>
    void Searcher<Evaluator>::MakeMoveList(MoveList* moveList)
    {
        Move* prev    = moveList->moves_;
        Move* cursor  = moveList->moves_ + 1;
        Stone* stones = &stones_;
        uint64_t mob  = CalcMobility64(stones->own_, stones->opp_);
        int nbMove    = 0;

        *prev = kDefaultMove;

        Position pos;
        for_bits(pos, mob)
        {
            ++nbMove;
            cursor->pos_   = pos;
            cursor->value_ = INT32_MIN;
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

    template <class Evaluator>
    void Searcher<Evaluator>::MidRoot(SearchResult* result)
    {
        score_t lower     = kEvalMin - 1;
        score_t upper     = kEvalMax + 1;
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
                    score = -MidAlphaBeta(-lower, -upper, depth - 1, false);
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
        const uint64_t hashCode = USE_HASH ? GetHashCode(stones_) : 0;
        if (USE_HASH && depth >= option_.midHashDepth)
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
                const score_t score = -MidAlphaBetaSwitch(-lower, -upper, depth, false);
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

    /* 終盤探索 */

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
