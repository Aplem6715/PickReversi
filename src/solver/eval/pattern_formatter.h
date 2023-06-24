#ifndef PATTERN_FORMATTER_H
#define PATTERN_FORMATTER_H

#if TRAIN_BUILD

#include "pattern.h"
#include <array>

namespace eval
{
    constexpr uint16_t GetOrder3(uint16_t n)
    {
        uint16_t ret = 0;
        while (n > 0)
        {
            n /= 3;
            ++ret;
        }
        return ret;
    }

    constexpr uint16_t GetReverse3(uint16_t i, int width)
    {
        uint16_t rev = 0;
        for (int r = 0; r < width; ++r)
        {
            rev *= 3;
            rev += i % 3;
            i /= 3;
        }
        return rev;
    }

    template <int N>
    constexpr std::array<uint16_t, kPow3[N]> GetSimpleSymmetry()
    {
        std::array<uint16_t, kPow3[N]> ret = {0};
        for (uint16_t i = 0; i < ret.size(); ++i)
        {
            ret[i] = GetReverse3(i, N);
        }
        return ret;
    }

    template <size_t N>
    constexpr std::array<uint16_t, kPow3[N]> GetSymmetry(std::array<uint16_t, N> reverser)
    {
        std::array<uint16_t, kPow3[N]> ret = {0};
        for (uint16_t i = 0; i < kPow3[N]; ++i)
        {
            uint16_t source   = i;
            uint16_t reversed = 0;
            for (uint16_t order = 0; order < N; order++)
            {
                const uint16_t bit = source % 3;
                reversed += bit * kPow3[reverser[order]];
                source /= 3;
            }
            ret[i] = reversed;
        }
        return ret;
    }

    // 0 1 2     0 3 6
    // 3 4 5  →  1 4 7
    // 6 7 8     2 5 8
    constexpr std::array<uint16_t, kPow3_9> GetCornerSymmetry()
    {
        constexpr std::array<uint16_t, 9> reverser = {0, 3, 6, 1, 4, 7, 2, 5, 8};
        return GetSymmetry(reverser);
    }

    // 4 6 7 8 9        5 3 2 1 0       4 3 2 1 0
    // 3 5              6 4             6 5
    // 2            →   7           →   7
    // 1                8               8
    // 0                9               9
    constexpr std::array<uint16_t, kPow3_10> GetArrowSymmetry()
    {
        std::array<uint16_t, kPow3_10> ret = {0};
        for (uint16_t i = 0; i < ret.size(); ++i)
        {
            uint16_t rev = GetReverse3(i, 10);
            // 4bit目と5bit目を入れ替え
            const uint16_t bit4 = (rev / kPow3_4) % 3;
            const uint16_t bit5 = (rev / kPow3_5) % 3;
            rev -= bit4 * kPow3_4;
            rev -= bit5 * kPow3_5;
            rev += bit4 * kPow3_5;
            rev += bit5 * kPow3_4;

            ret[i] = rev;
        }
        return ret;
    }

    constexpr auto kSymmetry4  = GetSimpleSymmetry<4>();
    constexpr auto kSymmetry5  = GetSimpleSymmetry<5>();
    constexpr auto kSymmetry6  = GetSimpleSymmetry<6>();
    constexpr auto kSymmetry7  = GetSimpleSymmetry<7>();
    constexpr auto kSymmetry8  = GetSimpleSymmetry<8>();
    constexpr auto kSymmetry10 = GetSimpleSymmetry<10>();

    constexpr auto kSymmetryCorner = GetCornerSymmetry();
    constexpr auto kSymmetryArrow  = GetArrowSymmetry();

    // clang-format off
    static const uint16_t* kShapeSymmetry[kShapeNum] = {
        kSymmetry8.data(),       // LINE2
        kSymmetry8.data(),       // LINE3
        kSymmetry8.data(),       // LINE4
        kSymmetry4.data(),       // DIAG4
        kSymmetry5.data(),       // DIAG5
        kSymmetry6.data(),       // DIAG6
        kSymmetry7.data(),       // DIAG7
        kSymmetry8.data(),       // DIAG8
        kSymmetry10.data(),      // EDGE
        kSymmetryCorner.data(),  // CORNER
        kSymmetryArrow.data(),   // ARROW
        kSymmetry10.data(),      // MIDDLE
    };
    // clang-format on

    /**
     * テスト済み
     * @brief 敵味方逆のパターンインデックスを取得する
     *
     * @param idx インデックス
     * @param digit インデックスの3進桁数
     * @return 逆立場のインデックス
     */
    constexpr uint16_t OpponentIndex(uint16_t idx, const uint16_t digit)
    {
        constexpr uint16_t oppN[] = {0, 2, 1};
        uint16_t ret              = 0;
        for (uint16_t shift = 0; shift < digit; shift++)
        {
            ret += oppN[idx % 3] * kPow3[shift];
            idx /= 3;
        }
        return ret;
    }

    inline uint16_t GetSymmetryShape(uint16_t shapeId, uint16_t state)
    {
        return kShapeSymmetry[shapeId][state];
    }

    constexpr uint16_t GetFlipShape(uint16_t shape, uint16_t state)
    {
        const uint16_t digit = kShapeDigits[shape];
        return OpponentIndex(state, digit);
    }

    inline void BuildWeight(int16_t* target)
    {
        constexpr uint32_t kSideOffset = kNumPhase * kNumWeight;

        // 対称パターンへweightをコピー
        for (int phase = 0; phase < kNumPhase; ++phase)
        {
            const uint32_t phaseOffset = phase * kNumWeight;
            for (int shape = 0; shape < kShapeNum; ++shape)
            {
                uint32_t offset = phaseOffset + kShapeIndexHead[shape];
                for (uint32_t state = 0; state < kShapeIndexMax[shape]; state++)
                {
                    // 対称パターンと比較して小さいインデックスの方に統合
                    const uint16_t symmIndex = GetSymmetryShape(shape, state);

                    // MEMO:反転パターン（相手側）は学習データ作成時にボードまるごと反転したものを適用するのでここでは無視

                    if (symmIndex < state)
                    {
                        target[offset + state] = target[offset + symmIndex];
                    }
                    assert(offset + state <= kSideOffset - 1);
                    assert(offset + symmIndex <= kSideOffset - 1);
                }
            }

            // 相手にとってのweightを設定
            for (int shape = 0; shape < kShapeNum; ++shape)
            {
                uint32_t offset = phaseOffset + kShapeIndexHead[shape];
                for (uint32_t i = 0; i < kShapeIndexMax[shape]; i++)
                {
                    target[kSideOffset + offset + i] = -target[offset + i];
                }
            }
        }
    }
}
#endif

#endif