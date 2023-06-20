
#include "../../../src/solver/eval/pattern.h"
#include "../../../src/solver/eval/pattern_formatter.h"
#include "gtest/gtest.h"
#include <array>
#include <vector>

namespace eval
{

    TEST(Pattern, BuildTest)
    {
        uint16_t*** weight = AllocPatternWeight<uint16_t>();

        for (size_t i = 0; i < 2 * kNumPhase * kNumWeight)
        {
            weight[0][0][i] = i;
        }

        for (int p = 0; p < kPatternNum; ++p)
        {
            int shape  = kPattern2Shape[p];
            int offset = kPatternOffset[p];
            for (int state = 0; state < kShapeIndexMax[shape]; state++)
            {
                // 反転，対称パターンで最も小さいインデックスのweightを持ってくる
                // 反転パターンについては評価値を±反転させる
                int symmIndex = GetSymmetry(p, state);
                int oppState  = GetFlipPattern(p, state);
                int oppSymm   = GetSymmetry(p, oppState);

                EXPECT_EQ(weight[offset + state], weight[offset + symmIndex]) << "対称チェック";
                EXPECT_EQ(weight[offset + state], -weight[offset + oppState]) << "反転チェック";
                EXPECT_EQ(weight[offset + state], -weight[offset + oppSymm]) << "対称反転チェック";

                constexpr int oppOffset = kNumPhase * kNumWeight;
                // 相手視点(weight[1][*][*]) 評価値は反転
                EXPECT_EQ(weight[offset + state], -weight[oppOffset + offset + state]) << "敵視点チェック";
                EXPECT_EQ(weight[offset + state], -weight[oppOffset + offset + symmIndex]) << "敵視点対称チェック";
                EXPECT_EQ(weight[offset + state], weight[oppOffset + offset + oppState]) << "敵視点反転チェック";
                EXPECT_EQ(weight[offset + state], weight[oppOffset + offset + oppSymm]) << "敵視点対称反転チェック";
            }
        }
    }

}
