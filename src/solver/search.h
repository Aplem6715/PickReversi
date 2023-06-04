#ifndef SEARCH_H
#define SEARCH_H

#include <vector>
#include "board/stone.h"
#include "eval/evaluator.h"
#include "eval/pos_eval.h"
#include "search_option.h"

namespace solver
{
    using namespace board;
    using namespace eval;

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
        /// @param ratio 選択確率の配列（idx0から1st率，2nd率）
        /// @return 選択された着手位置
        Position Search(std::vector<float> ratio);

    private:

    private:
        // 盤面
        Stone stones_[1];
        // 評価関数
        PositionEvaluator eval_[1];
        // 探索深度
        uint8_t depth_;
        // 探索設定
        SearchOption option_ = DEFAULT_OPTION;
    };
}
#endif