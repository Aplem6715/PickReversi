#ifndef REGRESSOR_H
#define REGRESSOR_H

#include "board/bit.h"
#include "evaluator.h"
#include "pattern.h"
#include "pos2pattern.h"
#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>

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

        void Reload(stone_t own, stone_t opp);
        void ResetState();

        inline score_t Evaluate(int phase);

        inline void Update(const Position pos, stone_t flips);
        inline void Restore(const Position pos, stone_t flips);
        inline void UpdatePass();

        bool Save(const std::string& path);
        bool Load(const std::string& path);

    private:
        // 各パターンのステート（石の状態）
        // ステートはパターンに応じてシフト済み
        std::array<state_t, kPatternNum> state_;

        // weight[side][phase][state[patternId]]
        // キャッシュに乗るようにまとめてalloc
        int16_t*** weight_;

        Side side_;

        inline void SwapSide();

        inline void PutOwn(const Position pos);
        inline void PutOpp(const Position pos);
        inline void FlipOwn(const Position pos);
        inline void FlipOpp(const Position pos);

        inline void RestorePutOwn(const Position pos);
        inline void RestorePutOpp(const Position pos);
        inline void RestoreFlipOwn(const Position pos);
        inline void RestoreFlipOpp(const Position pos);

#if TRAIN_BUILD
        friend class train::PatternTrainer;
#endif // TRAIN_BUILD
    };

    inline score_t PatternEval::Evaluate(int phase)
    {
        const auto weight = weight_[static_cast<int>(side_)][phase];

        int sum = 0;
        for (const auto& s : state_)
        {
            sum += weight[s];
        }

        const int sign = (std::signbit(sum) ? -1 : 1);
        // 四捨五入
        sum += sign * kWeightOneStone / 2;
        sum /= kWeightOneStone;

        return static_cast<score_t>(std::clamp(sum, (int)kEvalMin, (int)kEvalMax));
    }

    inline void PatternEval::Update(const Position pos, stone_t flips)
    {
        if (side_ == Side::Own)
        {
            PutOwn(pos);

            Position flipPos;
            for_bits(flipPos, flips)
            {
                FlipOwn(flipPos);
            }
        }
        else
        {
            PutOpp(pos);

            Position flipPos;
            for_bits(flipPos, flips)
            {
                FlipOpp(flipPos);
            }
        }
        SwapSide();
    }

    inline void PatternEval::Restore(const Position pos, stone_t flips)
    {
        if (side_ == Side::Own)
        {
            RestorePutOwn(pos);

            Position flipPos;
            for_bits(flipPos, flips)
            {
                RestoreFlipOwn(flipPos);
            }
        }
        else
        {
            RestorePutOpp(pos);

            Position flipPos;
            for_bits(flipPos, flips)
            {
                RestoreFlipOpp(flipPos);
            }
        }
        SwapSide();
    }

    inline void PatternEval::UpdatePass()
    {
        SwapSide();
    }

    inline void PatternEval::SwapSide()
    {
        side_ = static_cast<Side>(static_cast<int>(side_) ^ 1);
    }

    inline void PatternEval::PutOwn(const Position pos)
    {
        const PosToPattern& pos2pat = kPos2Pattern[static_cast<int>(pos)];
        const int nbPattern         = pos2pat.numPattern;

        for (int i = 0; i < nbPattern; ++i)
        {
            // 0(empty) -> 1(own)
            state_[pos2pat.pattern[i].uid] += pos2pat.pattern[i].idx;

            // range check (0 ~ MaxIndex)
            assert(state_[pos2pat.pattern[i].uid] - kPatternOffset[pos2pat.pattern[i].uid] < kShapeIndexMax[kPattern2Shape[pos2pat.pattern[i].uid]]);
        }

        // TODO: 検証: こっちのほうがループ展開されやすそう
        // for (int i = 0; i < kPosPatternMax; ++i)
        // {
        //     if (i < nbPattern)
        //     {
        //         // 0(empty) -> 1(own)
        //         state_[pos2pat.pattern[i].uid] += pos2pat.pattern[i].idx;
        //         // range check (0 ~ MaxIndex)
        //         assert(state_[pos2pat.pattern[i].uid] - kPatternOffset[pos2pat.pattern[i].uid] < kShapeIndexMax[kPattern2Shape[pos2pat.pattern[i].uid]]);
        //     }
        // }
    }

    inline void PatternEval::PutOpp(const Position pos)
    {
        const PosToPattern& pos2pat = kPos2Pattern[static_cast<int>(pos)];
        int nbPattern               = pos2pat.numPattern;

        for (int i = 0; i < nbPattern; ++i)
        {
            // 0(empty) -> 2(opp)
            state_[pos2pat.pattern[i].uid] += 2 * pos2pat.pattern[i].idx;

            // range check (0 ~ MaxIndex)
            assert(state_[pos2pat.pattern[i].uid] - kPatternOffset[pos2pat.pattern[i].uid] < kShapeIndexMax[kPattern2Shape[pos2pat.pattern[i].uid]]);
        }

        // TODO: 検証: こっちのほうがループ展開されやすそう
        // for (int i = 0; i < kPosPatternMax; ++i)
        // {
        //     if (i < nbPattern)
        //     {
    }

    inline void PatternEval::FlipOwn(const Position pos)
    {
        const PosToPattern& pos2pat = kPos2Pattern[static_cast<int>(pos)];
        int nbPattern               = pos2pat.numPattern;

        for (int i = 0; i < nbPattern; ++i)
        {
            // 2(opp) -> 1(own)
            state_[pos2pat.pattern[i].uid] -= pos2pat.pattern[i].idx;

            // range check (0 ~ MaxIndex)
            assert(state_[pos2pat.pattern[i].uid] - kPatternOffset[pos2pat.pattern[i].uid] < kShapeIndexMax[kPattern2Shape[pos2pat.pattern[i].uid]]);
        }
        
        // TODO: 検証: こっちのほうがループ展開されやすそう
        // for (int i = 0; i < kPosPatternMax; ++i)
        // {
        //     if (i < nbPattern)
        //     {
    }

    inline void PatternEval::FlipOpp(const Position pos)
    {
        const PosToPattern& pos2pat = kPos2Pattern[static_cast<int>(pos)];
        int nbPattern               = pos2pat.numPattern;

        for (int i = 0; i < nbPattern; ++i)
        {
            // 1(own) -> 2(opp)
            state_[pos2pat.pattern[i].uid] += pos2pat.pattern[i].idx;

            // range check (0 ~ MaxIndex)
            assert(state_[pos2pat.pattern[i].uid] - kPatternOffset[pos2pat.pattern[i].uid] < kShapeIndexMax[kPattern2Shape[pos2pat.pattern[i].uid]]);
        }
        
        // TODO: 検証: こっちのほうがループ展開されやすそう
        // for (int i = 0; i < kPosPatternMax; ++i)
        // {
        //     if (i < nbPattern)
        //     {
    }

    inline void PatternEval::RestorePutOwn(const Position pos)
    {
        const PosToPattern& pos2pat = kPos2Pattern[static_cast<int>(pos)];
        int nbPattern               = pos2pat.numPattern;

        for (int i = 0; i < nbPattern; ++i)
        {
            // 1(own) -> 0(opp)
            state_[pos2pat.pattern[i].uid] -= pos2pat.pattern[i].idx;

            // range check (0 ~ MaxIndex)
            assert(state_[pos2pat.pattern[i].uid] - kPatternOffset[pos2pat.pattern[i].uid] < kShapeIndexMax[kPattern2Shape[pos2pat.pattern[i].uid]]);
        }
        
        // TODO: 検証: こっちのほうがループ展開されやすそう
        // for (int i = 0; i < kPosPatternMax; ++i)
        // {
        //     if (i < nbPattern)
        //     {
    }

    inline void PatternEval::RestorePutOpp(const Position pos)
    {
        const PosToPattern& pos2pat = kPos2Pattern[static_cast<int>(pos)];
        int nbPattern               = pos2pat.numPattern;

        for (int i = 0; i < nbPattern; ++i)
        {
            // 2(own) -> 0(opp)
            state_[pos2pat.pattern[i].uid] -= 2 * pos2pat.pattern[i].idx;

            // range check (0 ~ MaxIndex)
            assert(state_[pos2pat.pattern[i].uid] - kPatternOffset[pos2pat.pattern[i].uid] < kShapeIndexMax[kPattern2Shape[pos2pat.pattern[i].uid]]);
        }
        
        // TODO: 検証: こっちのほうがループ展開されやすそう
        // for (int i = 0; i < kPosPatternMax; ++i)
        // {
        //     if (i < nbPattern)
        //     {
    }

    inline void PatternEval::RestoreFlipOwn(const Position pos)
    {
        const PosToPattern& pos2pat = kPos2Pattern[static_cast<int>(pos)];
        int nbPattern               = pos2pat.numPattern;

        for (int i = 0; i < nbPattern; ++i)
        {
            // 1(own) -> 0(empty)
            state_[pos2pat.pattern[i].uid] -= pos2pat.pattern[i].idx;

            // range check (0 ~ MaxIndex)
            assert(state_[pos2pat.pattern[i].uid] - kPatternOffset[pos2pat.pattern[i].uid] < kShapeIndexMax[kPattern2Shape[pos2pat.pattern[i].uid]]);
        }
        
        // TODO: 検証: こっちのほうがループ展開されやすそう
        // for (int i = 0; i < kPosPatternMax; ++i)
        // {
        //     if (i < nbPattern)
        //     {
    }

    inline void PatternEval::RestoreFlipOpp(const Position pos)
    {
        const PosToPattern& pos2pat = kPos2Pattern[static_cast<int>(pos)];
        int nbPattern               = pos2pat.numPattern;

        for (int i = 0; i < nbPattern; ++i)
        {
            // 2(opp) -> 0(empty)
            state_[pos2pat.pattern[i].uid] -= 2 * pos2pat.pattern[i].idx;

            // range check (0 ~ MaxIndex)
            assert(state_[pos2pat.pattern[i].uid] - kPatternOffset[pos2pat.pattern[i].uid] < kShapeIndexMax[kPattern2Shape[pos2pat.pattern[i].uid]]);
        }
        
        // TODO: 検証: こっちのほうがループ展開されやすそう
        // for (int i = 0; i < kPosPatternMax; ++i)
        // {
        //     if (i < nbPattern)
        //     {
    }

}

#endif