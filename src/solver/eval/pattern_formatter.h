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

    constexpr uint16_t GetReverse3(uint16_t i, uint16_t width)
    {
        uint16_t rev = 0;
        for (uint16_t r = 0; r < width; ++r)
        {
            rev *= 3;
            rev += i % 3;
            i /= 3;
        }
        return rev;
    }

    template <uint16_t N>
    constexpr const std::array<uint16_t, kPow3[N]>& GetSimpleSymmetry()
    {
        static constexpr std::array<uint16_t, kPow3[N]> kCache = []()
        {
            std::array<uint16_t, kPow3[N]> ret = {0};
            std::array<uint16_t, kPow3[N]> rev = {0};
            for (uint16_t i = 0; i < ret.size(); ++i)
            {
                if (rev[i] == 0)
                {
                    rev[i] = GetReverse3(i, N);
                }
                uint16_t r  = std::min(i, rev[i]);
                ret[i]      = r;
                ret[rev[i]] = r;
            }
            return ret;
        }();
        return kCache;
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
            ret[i] = std::min(i, reversed);
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

            ret[i] = std::min(i, rev);
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
    const uint16_t* kSymmetryPattern[kPatternNum] = {
        kSymmetry8.data(),      kSymmetry8.data(),      kSymmetry8.data(),      kSymmetry8.data(),      // LINE2
        kSymmetry8.data(),      kSymmetry8.data(),      kSymmetry8.data(),      kSymmetry8.data(),      // LINE3
        kSymmetry8.data(),      kSymmetry8.data(),      kSymmetry8.data(),      kSymmetry8.data(),      // LINE4
        kSymmetry4.data(),      kSymmetry4.data(),      kSymmetry4.data(),      kSymmetry4.data(),      // DIAG4
        kSymmetry5.data(),      kSymmetry5.data(),      kSymmetry5.data(),      kSymmetry5.data(),      // DIAG5
        kSymmetry6.data(),      kSymmetry6.data(),      kSymmetry6.data(),      kSymmetry6.data(),      // DIAG6
        kSymmetry7.data(),      kSymmetry7.data(),      kSymmetry7.data(),      kSymmetry7.data(),      // DIAG7
        kSymmetry8.data(),      kSymmetry8.data(),                                                      // DIAG8
        kSymmetry10.data(),     kSymmetry10.data(),     kSymmetry10.data(),     kSymmetry10.data(),     // EDGE
        kSymmetryCorner.data(), kSymmetryCorner.data(), kSymmetryCorner.data(), kSymmetryCorner.data(), // CORNER
        kSymmetryArrow.data(),  kSymmetryArrow.data(),  kSymmetryArrow.data(),  kSymmetryArrow.data(),  // ARROW
        kSymmetry10.data(),     kSymmetry10.data(),     kSymmetry10.data(),     kSymmetry10.data(),     // MIDDLE
        nullptr,                                                                                        // Mobility
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
    inline int OpponentIndex(int idx, const int digit)
    {
        const int oppN[] = {0, 2, 1};
        int ret          = 0;
        int shift;
        for (shift = 0; shift < digit; shift++)
        {
            ret += oppN[idx % 3] * kPow3[shift];
            idx /= 3;
        }
        return ret;
    }

    inline int GetSymmetry(int patternId, int state)
    {
        if (patternId == kPatternMobil)
            return state;
        return kSymmetryPattern[patternId][ReverseOffset(patternId, state)];
    }

    inline void UnifySymmetry(int patternId, int* state)
    {
        if (patternId == kPatternMobil)
            return;
        *state = GetSymmetry(patternId, *state);
    }

    inline int GetFlipPattern(int patternId, int state)
    {
        const int shape = kPattern2Shape[patternId];
        const int digit = kShapeDigits[shape];
        const int rev   = ReverseOffset(patternId, state);
        return OpponentIndex(rev, digit);
    }

    inline void FlipPattern(int patternId, int* state)
    {
        *state = GetFlipPattern(patternId, *state);
    }

}
#endif

#endif