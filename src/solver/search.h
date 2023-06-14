#ifndef SEARCH_H
#define SEARCH_H

#include <vector>

#include "board/stone.h"
#include "hash_table.h"
#include "movelist.h"
#include "search_option.h"

#if ENABLE_PROFILE
#include "bench/bench_result.h"
#endif

namespace solver
{
    struct SearchResult;

    template <class Evaluator>
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
        const board::Stone& GetStone() const { return stones_; }
        Evaluator& GetEval() { return eval_; }

    private:
        // 盤面
        board::Stone stones_;
        // 評価関数
        Evaluator eval_;
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
        score_t MidMinMax(int depth, bool passed);

        /// @brief 中盤αβ探索
        score_t MidAlphaBeta(const score_t up_limit, const score_t low_limit, const int depth, const bool passed);

        /* 終盤探索(search_end.cpp) */
        /// @brief 終盤探索ルート
        void EndRoot(SearchResult* result);

        /// @brief 終盤探索MinMax法（主にテストベース用，カットなしの正しい探索と探索速度のベースを提供）
        score_t EndMinMax(int depth, bool passed);

        /// @brief 終盤αβ探索
        score_t EndAlphaBeta(const score_t upper, const score_t lower, const int depth, const bool passed);

        /* 探索中に使用するinline関数 */

        void Update(const Move* move, bool updateEval)
        {
            --nbEmpty_;
            const auto& pos  = move->pos_;
            const auto& flip = move->flips_;
            stones_.Update(pos, flip);
            if (updateEval)
            {
                eval_.Update(pos, flip);
            }
        }

        void Restore(const Move* move, bool updateEval)
        {
            ++nbEmpty_;
            const auto& pos  = move->pos_;
            const auto& flip = move->flips_;
            stones_.Restore(pos, flip);
            if (updateEval)
            {
                eval_.Restore(pos, flip);
            }
        }

        void UpdatePass()
        {
            stones_.Swap();
            eval_.UpdatePass();
        }

        score_t WinJudge()
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

        score_t WinJudgeEnd()
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

} // namespace solver
#endif