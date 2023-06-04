#ifndef SEARCH_H
#define SEARCH_H

#include <vector>

#include "board/stone.h"
#include "eval/pos_eval.h"
#include "search_option.h"

namespace solver
{
    using namespace board;
    using namespace eval;

    struct SearchResult;
    struct MoveList;
    struct Move;

    /// 1石あたりの評価値
    constexpr int EvalStone   = 10;
    constexpr int EvalMax     = EvalStone * 64;
    constexpr int EvalMin     = -EvalMax;
    constexpr int EvalInvalid = -999;

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
        Position Search();

        /// @brief 着手位置を検索
        /// @param ratio 選択確率の配列（idx0から1st率，2nd率...）
        /// @return 選択された着手位置
        Position Search(std::vector<float> ratio);

    private:
        // 盤面
        Stone stones_[1];
        // 評価関数
        PositionEvaluator eval_[1];
        // 探索設定
        SearchOption option_ = DEFAULT_OPTION;
        // 探索深度
        uint8_t depth_;
        // 空きマス数
        uint8_t nbEmpty_;

    private:
        /// @brief 手のリストを作成
        /// @param moveList 手の追加先
        void MakeMoveList(MoveList* moveList);

        /* 中盤探索(search_mid.cpp) */
        /// @brief 中盤探索ルート
        void MidRoot(SearchResult* result);

        void UpdateMid(const Move* move);

        void RestoreMid(const Move* move);

        void PassMid();

        /// @brief 中盤探索MinMax法（主にテストベース用，カットなしの正しい探索と探索速度のベースを提供）
        score_t MidMinMax(int depth, bool passed);

        /// @brief 中盤αβ探索
        score_t MidAlphaBeta(score_t upper, score_t lower, int depth, bool passed);

        /* 終盤探索(search_end.cpp) */
        /// @brief 終盤探索ルート
        void EndRoot(SearchResult* result);

        void UpdateEnd(Move const* move);

        void RestoreEnd(Move const* move);

        /// @brief 終盤探索MinMax法（主にテストベース用，カットなしの正しい探索と探索速度のベースを提供）
        score_t EndMinMax(int depth, bool passed);

        /// @brief 終盤αβ探索
        score_t EndAlphaBeta(score_t upper, score_t lower, int depth, bool passed);

        score_t WinJudge()
        {
            auto diff = CountBits(stones_->own_) - CountBits(stones_->opp_);
            if (diff > 0)
            {
                return EvalMax;
            }
            else if (diff < 0)
            {
                return EvalMin;
            }
            else
            {
                return 0;
            }
        }
    };
}
#endif