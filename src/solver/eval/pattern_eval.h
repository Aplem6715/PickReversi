﻿#ifndef REGRESSOR_H
#define REGRESSOR_H

#include "evaluator.h"
#include "pattern.h"
#include "pos2pattern.h"
#include <array>

#if TRAIN_BUILD
namespace train
{
    class PatternTrainer;
}
#endif

namespace eval
{
    class PatternEval
    {
    public:
        PatternEval();
        PatternEval(const std::string& path);
        ~PatternEval();

        score_t Evaluate(int phase);

        void Reload(stone_t own, stone_t opp);
        void ResetState();

        inline void Update(Position pos, stone_t flips);
        inline void Restore(Position pos, stone_t flips);
        inline void UpdatePass();

        bool Save(const std::string& path);
        bool Load(const std::string& path);

    private:
        // 各パターンのステート（石の状態）
        // ステートはパターンに応じてシフト済み
        std::array<int, kPatternNum> state_;

        // weight[side][phase][state[patternId]]
        // キャッシュに乗るようにまとめてalloc
        short*** weight_;

        Side side_;

        inline void PutOwn(Position pos);
        inline void PutOpp(Position pos);
        inline void FlipOwn(Position pos);
        inline void FlipOpp(Position pos);

#if TRAIN_BUILD
        friend class train::PatternTrainer;
#endif // TRAIN_BUILD
    };
}

#endif