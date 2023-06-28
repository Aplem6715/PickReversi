#ifndef SEARCH_H
#define SEARCH_H

#include <vector>
#include <cassert>

#include "board/stone.h"
#include "hash_table.h"
#include "movelist.h"
#include "search_option.h"

#if ENABLE_PROFILE
#include "bench/bench_result.h"
#endif

// TODO: UpとLowの引数指定順番を逆に

namespace solver
{
    struct SearchResult;

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
#if ENABLE_DEPTH_TEMPLATE
        template <int depth>
        score_t MidAlphaBeta(const score_t up_limit, const score_t low_limit, const bool passed);
#else
        score_t MidAlphaBeta(const score_t up_limit, const score_t low_limit, const int depth, const bool passed);
#endif

/// @brief 中盤PVS探索
#if ENABLE_DEPTH_TEMPLATE
        template <int depth>
        score_t MidPVS(const score_t up_limit, const score_t low_limit, const bool passed);
#else
        score_t MidPVS(const score_t up_limit, const score_t low_limit, const int depth, const bool passed);
#endif

/// @brief 中盤PVS探索
#if ENABLE_DEPTH_TEMPLATE
        template <int depth>
        score_t MidNWS(const score_t up_limit, const bool passed);
#else
        score_t MidNWS(const score_t up_limit, const int depth, const bool passed);
#endif

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