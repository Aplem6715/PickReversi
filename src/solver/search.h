#ifndef SEARCH_H
#define SEARCH_H

#include <vector>

#include "board/stone.h"
#include "eval/evaluator.h"
#include "eval/pos_eval.h"
#include "hash_table.h"
#include "movelist.h"
#include "search_option.h"

#if ENABLE_PROFILE
#include "bench/bench_result.h"
#endif

namespace solver
{
    using namespace board;
    using namespace eval;

    struct SearchResult;

    class Searcher
    {
    public:
        Searcher();
        ~Searcher();

        void Reset();
        void Setup(SearchOption option);
        void SetStones(stone_t own, stone_t opp);

        /// @brief 最善手を検索
        /// @return 最善手
        void Search(stone_t own, stone_t opp, SearchResult* result);

        const int GetNumEmpty() const { return nbEmpty_; }
        const Stone& GetStone() const { return stones_; }
        Evaluator& GetEval() { return eval_; }

    private:
        // 盤面
        Stone stones_;
        // 評価関数
        PositionEvaluator eval_;
        // 置換表
        HashTable* table_;
        // 探索設定
        SearchOption option_ = kDefaultOption;
        // 空きマス数
        uint8_t nbEmpty_;
        // 最後の探索が中盤探索だったか
        bool wasMidSearch_;

#if ENABLE_PROFILE
    private:
        bench::Profile prof_;

    public:
        const bench::Profile& GetProfile() { return prof_; }
#endif

    private:
        void OnEnterMidSearch();

        void OnEnterEndSearch();

        /// @brief 手のリストを作成
        /// @param moveList 手の追加先
        void MakeMoveList(MoveList* moveList);

        /* 中盤探索(search_mid.cpp) */
        /// @brief 中盤探索ルート
        void MidRoot(SearchResult* result);

        /// @brief 中盤探索MinMax法（主にテストベース用，カットなしの正しい探索と探索速度のベースを提供）
        score32_t MidMinMax(int depth, bool passed);

        /// @brief 中盤αβ探索
        score32_t MidAlphaBeta(const score32_t up_limit, const score32_t low_limit, const int depth, const bool passed);

        /* 終盤探索(search_end.cpp) */
        /// @brief 終盤探索ルート
        void EndRoot(SearchResult* result);

        /// @brief 終盤探索MinMax法（主にテストベース用，カットなしの正しい探索と探索速度のベースを提供）
        score32_t EndMinMax(int depth, bool passed);

        /// @brief 終盤αβ探索
        score32_t EndAlphaBeta(const score32_t upper, const score32_t lower, const int depth, const bool passed);

        /* 探索中に使用するinline関数 */

        void Update(const Move* move, bool updateEval)
        {
            --nbEmpty_;
            const uint64_t posBit = PosToBit(move->pos_);
            stones_.Update(posBit, move->flips_);
            if (updateEval)
            {
                eval_.Update(posBit, move->flips_);
            }
        }

        void Restore(const Move* move, bool updateEval)
        {
            ++nbEmpty_;
            const uint64_t posBit = PosToBit(move->pos_);
            stones_.Restore(posBit, move->flips_);
            if (updateEval)
            {
                eval_.Restore(posBit, move->flips_);
            }
        }

        void UpdatePass()
        {
            stones_.Swap();
            eval_.UpdatePass();
        }

        score32_t WinJudge()
        {
            auto diff = stones_.GetCountDiff();
            if (diff > 0)
            {
                return kEvalMax;
            }
            else if (diff < 0)
            {
                return kEvalMin;
            }
            else
            {
                return 0;
            }
        }

        score32_t WinJudgeEnd()
        {
            auto diff = stones_.GetCountDiff();
            // 空きマスは勝った方に加算される
            if (diff > 0)
            {
                return diff + nbEmpty_;
            }
            else if (diff < 0)
            {
                return diff - nbEmpty_;
            }
            else
            {
                return 0;
            }
        }
    };
}
#endif