#ifndef PATTERN_H
#define PATTERN_H

#include "const.h"
#include <array>
#include <cstdint>

namespace eval
{
    constexpr int kNumPhase     = 15;
    constexpr int kNumPut1Phase = 60 / kNumPhase;

    constexpr int kPow0_0  = 0;
    constexpr int kPow3_0  = 1;
    constexpr int kPow3_1  = 3;
    constexpr int kPow3_2  = 9;
    constexpr int kPow3_3  = 27;
    constexpr int kPow3_4  = 81;
    constexpr int kPow3_5  = 243;
    constexpr int kPow3_6  = 729;
    constexpr int kPow3_7  = 2187;
    constexpr int kPow3_8  = 6561;
    constexpr int kPow3_9  = 19683;
    constexpr int kPow3_10 = 59049;
    constexpr int kPow3_11 = 177147;

    constexpr int kShapeLine2  = 0;
    constexpr int kShapeLine3  = 1;
    constexpr int kShapeLine4  = 2;
    constexpr int kShapeDiag4  = 3;
    constexpr int kShapeDiag5  = 4;
    constexpr int kShapeDiag6  = 5;
    constexpr int kShapeDiag7  = 6;
    constexpr int kShapeDiag8  = 7;
    constexpr int kShapeEdge   = 8;
    constexpr int kShapeCorner = 9;
    constexpr int kShapeArrow  = 10;
    constexpr int kShapeMiddle = 11;
    constexpr int kShapeNumMob = 12;
    constexpr int kShapeNum    = 13;

    // 3^9 x 4 = 78,732
    constexpr int kPatternLine2_1 = 0; // 3^9
    constexpr int kPatternLine2_2 = 1; // 3^9
    constexpr int kPatternLine2_3 = 2; // 3^9
    constexpr int kPatternLine2_4 = 3; // 3^9

    // 3^9 x 4 = 78,732
    constexpr int kPatternLine3_1 = 4; // 3^9
    constexpr int kPatternLine3_2 = 5; // 3^9
    constexpr int kPatternLine3_3 = 6; // 3^9
    constexpr int kPatternLine3_4 = 7; // 3^9

    // 3^9 x 4 = 78,732
    constexpr int kPatternLine4_1 = 8;  // 3^9
    constexpr int kPatternLine4_2 = 9;  // 3^9
    constexpr int kPatternLine4_3 = 10; // 3^9
    constexpr int kPatternLine4_4 = 11; // 3^9

    // 3^5 x 4 = 972
    constexpr int kPatternDiag4_1 = 12; // 3^5
    constexpr int kPatternDiag4_2 = 13; // 3^5
    constexpr int kPatternDiag4_3 = 14; // 3^5
    constexpr int kPatternDiag4_4 = 15; // 3^5

    // 3^6 x 4 = 2,916
    constexpr int kPatternDiag5_1 = 16; // 3^6
    constexpr int kPatternDiag5_2 = 17; // 3^6
    constexpr int kPatternDiag5_3 = 18; // 3^6
    constexpr int kPatternDiag5_4 = 19; // 3^6

    // 3^7 x 4 = 8,748
    constexpr int kPatternDiag6_1 = 20; // 3^6
    constexpr int kPatternDiag6_2 = 21;
    constexpr int kPatternDiag6_3 = 22;
    constexpr int kPatternDiag6_4 = 23;

    // 3^8 x 4 = 26,244
    constexpr int kPatternDiag7_1 = 24;
    constexpr int kPatternDiag7_2 = 25;
    constexpr int kPatternDiag7_3 = 26;
    constexpr int kPatternDiag7_4 = 27;

    // 3^9 x 2 = 39,366
    constexpr int kPatternDiag8_1 = 28;
    constexpr int kPatternDiag8_2 = 29;

    // 3^10 x 4 = 708,588
    constexpr int kPatternEdgeX_1 = 30;
    constexpr int kPatternEdgeX_2 = 31;
    constexpr int kPatternEdgeX_3 = 32;
    constexpr int kPatternEdgeX_4 = 33;

    // 3^9 x 4 = 236,196
    constexpr int kPatternConer_1 = 34;
    constexpr int kPatternConer_2 = 35;
    constexpr int kPatternConer_3 = 36;
    constexpr int kPatternConer_4 = 37;

    // 3^10 x 4 = 708,588
    constexpr int kPatternArrow_1 = 38;
    constexpr int kPatternArrow_2 = 39;
    constexpr int kPatternArrow_3 = 40;
    constexpr int kPatternArrow_4 = 41;

    // 3^10 x 4 = 708,588
    constexpr int kPatternMidle_1 = 42;
    constexpr int kPatternMidle_2 = 43;
    constexpr int kPatternMidle_3 = 44;
    constexpr int kPatternMidle_4 = 45;

    // 33(着手可能最大数)
    constexpr int kPatternMobil = 46;

    constexpr int kPatternNum = 47;

    constexpr uint16_t kPow3[] = {kPow3_0, kPow3_1, kPow3_2, kPow3_3,
                                  kPow3_4, kPow3_5, kPow3_6, kPow3_7,
                                  kPow3_8, kPow3_9, kPow3_10};

    constexpr uint8_t PatternId2Type[kPatternNum] = {
        kShapeLine2, kShapeLine2, kShapeLine2, kShapeLine2,     // LINE2
        kShapeLine3, kShapeLine3, kShapeLine3, kShapeLine3,     // LINE3
        kShapeLine4, kShapeLine4, kShapeLine4, kShapeLine4,     // LINE4
        kShapeDiag4, kShapeDiag4, kShapeDiag4, kShapeDiag4,     // DIAG4
        kShapeDiag5, kShapeDiag5, kShapeDiag5, kShapeDiag5,     // DIAG5
        kShapeDiag6, kShapeDiag6, kShapeDiag6, kShapeDiag6,     // DIAG6
        kShapeDiag7, kShapeDiag7, kShapeDiag7, kShapeDiag7,     // DIAG7
        kShapeDiag8, kShapeDiag8,                               // DIAG8
        kShapeEdge, kShapeEdge, kShapeEdge, kShapeEdge,         // EDGE
        kShapeCorner, kShapeCorner, kShapeCorner, kShapeCorner, // CORNER
        kShapeArrow, kShapeArrow, kShapeArrow, kShapeArrow,     // ARROW
        kShapeMiddle, kShapeMiddle, kShapeMiddle, kShapeMiddle, // MIDDLE
        kShapeNumMob,                                           // Mobility
    };

    constexpr std::array<uint32_t, kShapeNum> kPatternIndexMax = {
        kPow3_8,  // LINE2
        kPow3_8,  // LINE3
        kPow3_8,  // LINE4
        kPow3_4,  // DIAG4
        kPow3_5,  // DIAG5
        kPow3_6,  // DIAG6
        kPow3_7,  // DIAG7
        kPow3_8,  // DIAG8
        kPow3_10, // EDGE
        kPow3_9,  // CORNER
        kPow3_10, // ARROW
        kPow3_10, // MIDDLE
        kMaxMove, // Mobility(0~33)
    };

    // 累積計算
    constexpr std::array<uint32_t, kShapeNum> MakePatternHeadArray()
    {
        std::array<uint32_t, kShapeNum> ret = {0};

        uint32_t prev = 0;
        for (int p = 0; p < kShapeNum; ++p)
        {
            ret[p] = prev;
            prev   = prev + kPatternIndexMax[p];
        }
        return ret;
    }

    // 各パターンの先頭(0)のインデックス（kPatternIndexMaxを累積したもの）
    constexpr std::array<uint32_t, kShapeNum> kPatternIndexHead = MakePatternHeadArray();

    constexpr uint32_t CalcNumWeight()
    {
        uint32_t ret = 0;
        for (const auto max : kPatternIndexMax)
        {
            ret += max;
        }
        return ret;
    }

    constexpr std::array<uint32_t, kPatternNum> MakePatternOffset()
    {
        std::array<uint32_t, kPatternNum> ret = {0};
        for (int pid = 0; pid < kPatternNum; ++pid)
        {
            ret[pid] = kPatternIndexHead[PatternId2Type[pid]];
        }
        return ret;
    }

    constexpr uint32_t kNumWeight = CalcNumWeight();

    constexpr std::array<uint32_t, kPatternNum> kPatternOffset = MakePatternOffset();

    inline int
    Phase(int nbEmpty)
    {
        return nbEmpty / kNumPut1Phase;
    }

    template <typename T>
    inline T*** AllocPatternWeight()
    {
        // weightのメモリ確保．
        // キャッシュに乗るようにまとめて確保する
        // (本体は [weight_[0][0] = ...] の行)
        T*** weight_ = new T**[2];

        weight_[0] = new T*[2 * kNumPhase];
        weight_[1] = weight_[0] + kNumPhase;

        weight_[0][0] = new T[2 * kNumPhase * kNumWeight];
        weight_[1][0] = weight_[0][0] + kNumPhase * kNumWeight;

        for (int phase = 1; phase < kNumPhase; ++phase)
        {
            weight_[0][phase] = weight_[0][phase - 1] + kNumWeight;
            weight_[1][phase] = weight_[1][phase - 1] + kNumWeight;
        }
    }

    template <typename T>
    inline void FreePatternWeight(T*** weight)
    {
        delete[] weight[0][0];
        delete[] weight[0];
        delete[] weight;
    }

    /**
     * テスト済み
     * @brief 敵味方逆のパターンインデックスを取得する
     *
     * @param idx インデックス
     * @param digit インデックスの３進桁数
     * @return uint16 逆立場のインデックス
     */
    inline uint16_t OpponentIndex(uint16_t idx, uint8_t digit)
    {
        const uint16_t oppN[] = {0, 2, 1};
        uint16_t ret          = 0;
        uint8_t shift;
        for (shift = 0; shift < digit; shift++)
        {
            ret += oppN[idx % 3] * kPow3[shift];
            idx /= 3;
        }
        return ret;
    }
}

#endif