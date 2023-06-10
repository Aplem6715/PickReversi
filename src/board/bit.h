#ifndef BIT_H
#define BIT_H

#include "../const.h"
#include <string>
#include <string_view>

#ifdef INTRINSIC
#include <intrin.h>
#endif

void CalcPosAscii(Position posIdx, char* const x, int* const y);

/**
 * @brief ビット演算関連の処理
 * 処理性能重視・インライン化のためにヘッダー実装
 */

/**
 * @brief 立っているビット数を数える
 *
 * 組み込み命令を使って高速にカウント（INTRINSIC)
 *
 * @param x ビット列64
 * @return uint8_t 立っているビット数
 */
inline uint64_t popcnt(uint64_t x)
{
#ifdef INTRINSIC
    return __popcnt64(x);
#else
    uint64_t c = 0;
    c          = (x & 0x5555555555555555) + ((x >> 1) & 0x5555555555555555);
    c          = (c & 0x3333333333333333) + ((c >> 2) & 0x3333333333333333);
    c          = (c & 0x0f0f0f0f0f0f0f0f) + ((c >> 4) & 0x0f0f0f0f0f0f0f0f);
    c          = (c & 0x00ff00ff00ff00ff) + ((c >> 8) & 0x00ff00ff00ff00ff);
    c          = (c & 0x0000ffff0000ffff) + ((c >> 16) & 0x0000ffff0000ffff);
    c          = (c & 0x00000000ffffffff) + ((c >> 32) & 0x00000000ffffffff);
    return c;
#endif
}

/**
 * @brief 最下位ビットの下側にある0の個数を取得
 *
 * @param x 64ビット列
 * @return uint8_t 末尾0の数
 */
inline uint8_t tzcnt(const uint64_t x)
{
#ifdef INTRINSIC
    return (uint8_t)__tzcnt_u64(x);
#else
    return popcnt(~x & (x - 1));
#endif
}

/**
 * @brief 最上位ビットの上側にある0の個数を取得
 *
 * @param x 64ビット列
 * @return uint8_t 先頭0の数
 */
inline uint8_t lzcnt(uint64_t x)
{
#ifdef INTRINSIC
    return (uint8_t)_lzcnt_u64(x);
#else
    x = x | (x >> 1);
    x = x | (x >> 2);
    x = x | (x >> 4);
    x = x | (x >> 8);
    x = x | (x >> 16);
    x = x | (x >> 32);
    return popcnt(~x);
#endif
}

// AVX2では全方向同時に計算できるので無効化
#if !defined(__AVX2__) || !defined(INTRINSIC)
/**
 * @brief 左側のビットに対する着手可能位置の計算
 *
 * @param own 自身の石情報
 * @param masked_opp 端マスクをかけた相手の石位置
 * @param empty 空きマス情報
 * @param dir 検索方向（タテ・ヨコ・ナナメ
 * @return uint64_t 左側着手可能位置
 */
inline uint64_t CalcMobilityL(const uint64_t own, const uint64_t masked_opp,
                              const uint64_t empty, const unsigned char dir)
{
    uint64_t tmp = masked_opp & (own << dir);
    tmp |= masked_opp & (tmp << dir);
    tmp |= masked_opp & (tmp << dir);
    tmp |= masked_opp & (tmp << dir);
    tmp |= masked_opp & (tmp << dir);
    tmp |= masked_opp & (tmp << dir);
    return empty & (tmp << dir);
}

/**
 * @brief 右側のビットに対する着手可能位置の計算
 *
 * @param own 自身の石情報
 * @param masked_opp 端マスクをかけた相手の石位置
 * @param empty 空きマス情報
 * @param dir 検索方向（タテ・ヨコ・ナナメ
 * @return uint64_t 右側着手可能位置
 */
inline uint64_t CalcMobilityR(const uint64_t own, const uint64_t masked_opp,
                              const uint64_t empty, const unsigned char dir)
{
    uint64_t tmp = masked_opp & (own >> dir);
    tmp |= masked_opp & (tmp >> dir);
    tmp |= masked_opp & (tmp >> dir);
    tmp |= masked_opp & (tmp >> dir);
    tmp |= masked_opp & (tmp >> dir);
    tmp |= masked_opp & (tmp >> dir);
    return empty & (tmp >> dir);
}
#endif

/**
 * @brief 着手可能位置の計算
 *
 * @param own 自分の石情報
 * @param opp 相手の石情報
 * @return uint64_t 着手可能位置ビット列
 */
inline uint64_t CalcMobility64(const uint64_t own, const uint64_t opp)
{
#if defined(__AVX2__) && defined(INTRINSIC)
    __m256i PP, mOO, MM, flip_l, flip_r, pre_l, pre_r, shift2;
    __m128i M;
    const __m256i shift1897 = _mm256_set_epi64x(7, 9, 8, 1);
    const __m256i mflipH    = _mm256_set_epi64x(0x7e7e7e7e7e7e7e7e, 0x7e7e7e7e7e7e7e7e, -1, 0x7e7e7e7e7e7e7e7e);

    PP  = _mm256_broadcastq_epi64(_mm_cvtsi64_si128(own));
    mOO = _mm256_and_si256(_mm256_broadcastq_epi64(_mm_cvtsi64_si128(opp)), mflipH);

    flip_l = _mm256_and_si256(mOO, _mm256_sllv_epi64(PP, shift1897));
    flip_r = _mm256_and_si256(mOO, _mm256_srlv_epi64(PP, shift1897));
    flip_l = _mm256_or_si256(flip_l, _mm256_and_si256(mOO, _mm256_sllv_epi64(flip_l, shift1897)));
    flip_r = _mm256_or_si256(flip_r, _mm256_and_si256(mOO, _mm256_srlv_epi64(flip_r, shift1897)));
    pre_l  = _mm256_and_si256(mOO, _mm256_sllv_epi64(mOO, shift1897));
    pre_r  = _mm256_srlv_epi64(pre_l, shift1897);
    shift2 = _mm256_add_epi64(shift1897, shift1897);
    flip_l = _mm256_or_si256(flip_l, _mm256_and_si256(pre_l, _mm256_sllv_epi64(flip_l, shift2)));
    flip_r = _mm256_or_si256(flip_r, _mm256_and_si256(pre_r, _mm256_srlv_epi64(flip_r, shift2)));
    flip_l = _mm256_or_si256(flip_l, _mm256_and_si256(pre_l, _mm256_sllv_epi64(flip_l, shift2)));
    flip_r = _mm256_or_si256(flip_r, _mm256_and_si256(pre_r, _mm256_srlv_epi64(flip_r, shift2)));
    MM     = _mm256_sllv_epi64(flip_l, shift1897);
    MM     = _mm256_or_si256(MM, _mm256_srlv_epi64(flip_r, shift1897));

    M = _mm_or_si128(_mm256_castsi256_si128(MM), _mm256_extracti128_si256(MM, 1));
    M = _mm_or_si128(M, _mm_unpackhi_epi64(M, M));
    return _mm_cvtsi128_si64(M) & ~(own | opp); // mask with empties
#else
    uint64_t empty = ~(own | opp);
    // 上下左右の壁をまたがないようにマスクをかける
    uint64_t mask_rl = opp & 0x7e7e7e7e7e7e7e7e;
    uint64_t mask_ud = opp & 0x00ffffffffffff00;
    uint64_t mask_al = opp & 0x007e7e7e7e7e7e00;

    // 8方向に対して着手可能位置を検索
    uint64_t mobilty = CalcMobilityL(own, mask_rl, empty, 1);
    mobilty |= CalcMobilityL(own, mask_ud, empty, 8);
    mobilty |= CalcMobilityL(own, mask_al, empty, 7);
    mobilty |= CalcMobilityL(own, mask_al, empty, 9);
    mobilty |= CalcMobilityR(own, mask_rl, empty, 1);
    mobilty |= CalcMobilityR(own, mask_ud, empty, 8);
    mobilty |= CalcMobilityR(own, mask_al, empty, 7);
    mobilty |= CalcMobilityR(own, mask_al, empty, 9);
    return mobilty;
#endif
}

/**
 * @brief 反転位置を計算
 *
 * TODO: SIMD化
 * 参考 FlipGenerator
 * https://primenumber.hatenadiary.jp/entry/2016/12/26/063226
 *
 * @param own 自身の石情報
 * @param opp 相手の石情報
 * @param pos 着手位置
 * @return uint64_t 反転位置bit
 */
inline uint64_t CalcFlip64(const uint64_t own, const uint64_t opp, const Position pos)
{
    uint64_t index = static_cast<uint64_t>(pos);

    uint64_t flipped[4];
    uint64_t oppM[4];
    uint64_t outflank[4];
    uint64_t mask[4];

    oppM[0] = opp;
    oppM[1] = opp & 0x7e7e7e7e7e7e7e7eULL;
    oppM[2] = opp & 0x7e7e7e7e7e7e7e7eULL;
    oppM[3] = opp & 0x7e7e7e7e7e7e7e7eULL;

    mask[0] = 0x0080808080808080ULL >> (63 - index);
    mask[1] = 0x7f00000000000000ULL >> (63 - index);
    mask[2] = 0x0102040810204000ULL >> (63 - index);
    mask[3] = 0x0040201008040201ULL >> (63 - index);

    outflank[0] = (0x8000000000000000UL >> lzcnt(~oppM[0] & mask[0])) & own;
    outflank[1] = (0x8000000000000000UL >> lzcnt(~oppM[1] & mask[1])) & own;
    outflank[2] = (0x8000000000000000UL >> lzcnt(~oppM[2] & mask[2])) & own;
    outflank[3] = (0x8000000000000000UL >> lzcnt(~oppM[3] & mask[3])) & own;

#pragma warning(push)
#pragma warning(disable : 4146)
    flipped[0] = (-outflank[0] * 2) & mask[0];
    flipped[1] = (-outflank[1] * 2) & mask[1];
    flipped[2] = (-outflank[2] * 2) & mask[2];
    flipped[3] = (-outflank[3] * 2) & mask[3];
#pragma warning(pop)

    mask[0] = 0x0101010101010100ULL << index;
    mask[1] = 0x00000000000000feULL << index;
    mask[2] = 0x0002040810204080ULL << index;
    mask[3] = 0x8040201008040200ULL << index;

    outflank[0] = mask[0] & ((oppM[0] | ~mask[0]) + 1) & own;
    outflank[1] = mask[1] & ((oppM[1] | ~mask[1]) + 1) & own;
    outflank[2] = mask[2] & ((oppM[2] | ~mask[2]) + 1) & own;
    outflank[3] = mask[3] & ((oppM[3] | ~mask[3]) + 1) & own;

    flipped[0] |= (outflank[0] - (outflank[0] != 0)) & mask[0];
    flipped[1] |= (outflank[1] - (outflank[1] != 0)) & mask[1];
    flipped[2] |= (outflank[2] - (outflank[2] != 0)) & mask[2];
    flipped[3] |= (outflank[3] - (outflank[3] != 0)) & mask[3];

    return flipped[0] | flipped[1] | flipped[2] | flipped[3];
}

/**
 * @brief 立っているビット数を数える
 *
 * @param stone 石情報
 * @return uint64_t 立っているビット数
 */
inline uint64_t CountBits(const uint64_t stone) { return popcnt(stone); }

#pragma warning(push)
#pragma warning(disable : 4146)
///@brief 64bit位置情報から位置インデックス（位置番号）を計算
///@param bit 64bit位置情報
///@return Position 位置
inline Position BitToPos(const uint64_t bit)
{
    return static_cast<Position>(tzcnt(bit));
}

/// @brief 位置インデックスをビット変換
/// @param posIdx 位置
inline uint64_t PosToBit(Position posIdx)
{
    return (uint64_t)0x0000000000000001 << static_cast<int>(posIdx);
}

inline Color AntiColor(Color color)
{
    auto iColor = static_cast<int>(color);
    return static_cast<Color>(iColor ^ 1);
}

inline uint64_t GetLSB(uint64_t bits)
{
    return (-bits & bits);
}

inline uint64_t PopLSB(uint64_t* bits)
{
    uint64_t lsb = GetLSB(*bits);
    *bits ^= lsb;
    return lsb;
}

inline Position NextIndex(uint64_t* bits)
{
    *bits &= *bits - 1;
    return BitToPos(*bits);
}

#define for_bits(pos, bits) for (pos = BitToPos(bits); bits; pos = NextIndex(&bits))

#pragma warning(pop)

#endif
