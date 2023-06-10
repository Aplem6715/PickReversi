#ifndef HASH_H
#define HASH_H

#include "board/stone.h"
#include "const.h"

namespace solver
{

    struct HashData
    {
        uint8_t usedVer_;
        uint8_t cost_;
        uint8_t depth_;
        score8_t lower_;
        score8_t upper_;
        Position bestMove_;
        Position secondMove_;

    public:
        /// @brief 新たなデータで上書き
        /// @param upper 評価値上限
        /// @param lower 評価値下限
        /// @param score 評価値
        /// @param move 見つかった最善手
        /// @param version 探索バージョン（手番のたびに+1）
        /// @param cost 探索コスト（log2）
        /// @param depth 深度
        inline void Rewrite(score32_t upper, score32_t lower, score32_t score, Position move, uint8_t version, uint8_t cost, uint8_t depth);

        /// @brief データを更新
        /// @param upper 評価値上限
        /// @param lower 評価値下限
        /// @param score 評価値
        /// @param move 見つかった最善手
        /// @param version 探索バージョン（手番のたびに+1）
        /// @param cost 探索コスト（log2）
        /// @param depth 深度
        inline void Update(score32_t upper, score32_t lower, score32_t score, Position move, uint8_t version, uint8_t cost, uint8_t depth);

        /// @brief データ優先度を取得
        inline uint32_t GetPriority() const;

    private:
        /// @brief データを更新
        /// @param upper 評価値上限
        /// @param lower 評価値下限
        /// @param score 評価値
        /// @param move 見つかった最善手
        inline void Update(score32_t upper, score32_t lower, score32_t score, Position move);

        /// @brief 深度を更新
        /// @param upper 評価値上限
        /// @param lower 評価値下限
        /// @param score 評価値
        /// @param move 見つかった最善手
        /// @param depth 新しい深度
        inline void Deepen(score32_t upper, score32_t lower, score32_t score, Position move, uint8_t depth);
    };

    constexpr HashData kInitHashData = {
        0,
        0,
        0,
        static_cast<score8_t>(kEvalMin),
        static_cast<score8_t>(kEvalMax),
        Position::NoMove,
        Position::NoMove,
    };

    struct HashPair
    {
        board::Stone key_;
        HashData value_;
    };

    void HashData::Update(score32_t upper, score32_t lower, score32_t score, Position move, uint8_t version, uint8_t cost, uint8_t depth)
    {
        if (depth_ == depth)
        {
            Update(upper, lower, score, move);
        }
        else
        {
            Deepen(upper, lower, score, move, depth);
        }
        usedVer_ = version;
        cost_    = std::max(cost_, cost);

        if (lower_ > upper_)
        {
            Rewrite(upper, lower, score, move, version, cost, depth);
        }
    }

    uint32_t HashData::GetPriority() const
    {
        return (static_cast<uint32_t>(usedVer_) << 16) + (static_cast<uint32_t>(cost_) << 8) + depth_;
    }

    void HashData::Rewrite(score32_t upper, score32_t lower, score32_t score, Position move, uint8_t version, uint8_t cost, uint8_t depth)
    {
        this->upper_ = score < upper ? score : kEvalMax;
        this->lower_ = score > lower ? score : kEvalMin;

        const bool validMove = (score > lower || score == kEvalMin) && this->bestMove_ != move;
        this->bestMove_      = validMove ? move : Position::NoMove;
        this->secondMove_    = Position::NoMove;

        this->usedVer_ = version;
        this->cost_    = cost;
        this->depth_   = depth;
    }

    void HashData::Update(score32_t upper, score32_t lower, score32_t score, Position move)
    {
        if (score < upper && score < this->upper_)
            this->upper_ = score;
        if (score > lower && score > this->lower_)
            this->lower_ = score;
        if ((score > lower || score == kEvalMin) && this->bestMove_ != move)
        {
            this->secondMove_ = this->bestMove_;
            this->bestMove_   = move;
        }
    }

    void HashData::Deepen(score32_t upper, score32_t lower, score32_t score, Position move, uint8_t depth)
    {
        this->upper_ = score < upper ? score : kEvalMax;
        this->lower_ = score > lower ? score : kEvalMin;
        if ((score > lower || score == kEvalMin) && this->bestMove_ != move)
        {
            this->secondMove_ = this->bestMove_;
            this->bestMove_   = move;
        }
        this->depth_ = depth;
    }
}
#endif