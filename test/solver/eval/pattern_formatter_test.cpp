
#include "solver/eval/pattern.h"
#include "solver/eval/pattern_formatter.h"
#include "gtest/gtest.h"
#include <array>
#include <vector>

#if TRAIN_BUILD

namespace eval
{

    constexpr int ToBase3(std::initializer_list<int> list)
    {
        int ret = 0;
        for (auto n : list)
        {
            ret *= 3;
            ret += n;
        }
        return ret;
    }

    template <int N>
    constexpr std::array<int, N> ToBase3Array(int n)
    {
        std::array<int, N> ret = {0};
        for (int i = 0; i < N; ++i)
        {
            ret[N - i - 1] = n % 3;
            n /= 3;
        }
        return ret;
    }

    TEST(PatternFormatter, GetOrder3)
    {
        constexpr int idx    = ToBase3({0, 0, 2, 1, 0, 1});
        constexpr int expect = 4;

        constexpr int result = GetOrder3(idx);

        EXPECT_EQ(result, expect);
    }

    TEST(PatternFormatter, GetReverse3)
    {
        constexpr int idx    = ToBase3({0, 0, 2, 1, 0, 1});
        constexpr int expect = ToBase3({1, 0, 1, 2, 0, 0});

        constexpr int result = GetReverse3(idx, 6);

        EXPECT_EQ(result, expect);
    }

    TEST(PatternFormatter, Symmetry4)
    {
        constexpr int idx    = ToBase3({2, 1, 0, 1});
        constexpr int expect = ToBase3({1, 0, 1, 2});

        constexpr int symm = kSymmetry4[idx];

        EXPECT_EQ(symm, expect);
    }

    TEST(PatternFormatter, Symmetry5)
    {
        constexpr int idx    = ToBase3({2, 1, 2, 2, 1});
        constexpr int expect = ToBase3({1, 2, 2, 1, 2});

        constexpr int symm = kSymmetry5[idx];

        EXPECT_EQ(symm, expect);
    }

    TEST(PatternFormatter, Symmetry6)
    {
        constexpr int idx    = ToBase3({2, 1, 0, 2, 1, 0});
        constexpr int expect = ToBase3({0, 1, 2, 0, 1, 2});

        constexpr int symm = kSymmetry6[idx];

        EXPECT_EQ(symm, expect);
    }

    TEST(PatternFormatter, Symmetry7)
    {
        constexpr int idx    = ToBase3({2, 1, 0, 2, 1, 0, 1});
        constexpr int expect = ToBase3({1, 0, 1, 2, 0, 1, 2});

        constexpr int symm = kSymmetry7[idx];

        EXPECT_EQ(symm, expect);
    }

    TEST(PatternFormatter, Symmetry8)
    {
        constexpr int idx    = ToBase3({2, 1, 0, 2, 1, 0, 2, 1});
        constexpr int expect = ToBase3({1, 2, 0, 1, 2, 0, 1, 2});

        constexpr int symm = kSymmetry8[idx];

        EXPECT_EQ(symm, expect);
    }

    TEST(PatternFormatter, CornerSymmetry)
    {
        // 0 1 2     0 3 6
        // 3 4 5  →  1 4 7
        // 6 7 8     2 5 8

        constexpr int idx    = ToBase3({2, 1, 0, 2, 1, 0, 2, 1, 0});
        constexpr int expect = ToBase3({2, 2, 2, 1, 1, 1, 0, 0, 0});

        constexpr int symm = kSymmetryCorner[idx];

        EXPECT_EQ(symm, expect);
    }

    TEST(PatternFormatter, ArrowSymmetry)
    {
        // 4 6 7 8 9        5 3 2 1 0       4 3 2 1 0
        // 3 5              6 4             6 5
        // 2            →   7           →   7
        // 1                8               8
        // 0                9               9

        constexpr int idx    = ToBase3({2, 1, 0, 2, 1, 0, 2, 1, 0, 2});
        constexpr int expect = ToBase3({2, 0, 1, 2, 1, 0, 2, 0, 1, 2});

        constexpr int symm = kSymmetryArrow[idx];

        EXPECT_EQ(symm, expect);
    }

    TEST(PatternFormatter, OpponentIndex)
    {
        constexpr int idx    = ToBase3({2, 1, 0, 2, 1, 0, 2, 1, 0, 2});
        constexpr int expect = ToBase3({1, 2, 0, 1, 2, 0, 1, 2, 0, 1});

        constexpr int symm = OpponentIndex(idx, 10);

        EXPECT_EQ(symm, expect);
    }

    TEST(Pattern, BuildTest)
    {
        int16_t*** weight = AllocPatternWeight<int16_t>();

        for (size_t i = 0; i < 2 * kNumPhase * kNumWeight; ++i)
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

                EXPECT_EQ(weight[0][0][offset + state], weight[0][0][offset + symmIndex]) << "対称チェック";
                EXPECT_EQ(weight[0][0][offset + state], -weight[0][0][offset + oppState]) << "反転チェック";
                EXPECT_EQ(weight[0][0][offset + state], -weight[0][0][offset + oppSymm]) << "対称反転チェック";

                constexpr int oppOffset = kNumPhase * kNumWeight;
                // 相手視点(weight[1][*][*]) 評価値は反転
                EXPECT_EQ(weight[0][0][offset + state], -weight[0][0][oppOffset + offset + state]) << "敵視点チェック";
                EXPECT_EQ(weight[0][0][offset + state], -weight[0][0][oppOffset + offset + symmIndex]) << "敵視点対称チェック";
                EXPECT_EQ(weight[0][0][offset + state], weight[0][0][oppOffset + offset + oppState]) << "敵視点反転チェック";
                EXPECT_EQ(weight[0][0][offset + state], weight[0][0][oppOffset + offset + oppSymm]) << "敵視点対称反転チェック";
            }
        }
    }
}

#endif
