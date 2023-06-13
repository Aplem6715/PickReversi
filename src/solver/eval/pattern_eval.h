#ifndef REGRESSOR_H
#define REGRESSOR_H

#include "evaluator.h"
#include "pattern.h"
#include "pos2pattern.h"

namespace eval
{
    class PatternEval
    {
    public:
        PatternEval();
        PatternEval(const std::string& path);
        ~PatternEval();

        score32_t Evaluate(int nEmpty);

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
        unsigned int state_[kPatternNum];

        // weight[side][phase][state[patternId]]
        short *weight_[2][kNumPhase];

        Side side_;

        inline void PutOwn(Position pos);
        inline void PutOpp(Position pos);
        inline void FlipOwn(Position pos);
        inline void FlipOpp(Position pos);
    };
}

#endif