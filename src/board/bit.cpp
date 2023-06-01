
#include "bit.h"
#include <assert.h>

/**
 * @brief Ascii位置情報から位置番号を計算
 *
 * @param ascii Ascii位置情報
 * @return uint8_t 位置インデックス（位置番号
 */
Position PosIndexFromAscii(std::string ascii)
{
    int x, y;

    if (ascii[0] == 'U' || ascii[0] == 'u')
        return UNDO_POS;

    if (ascii.length() < 2)
    {
        return INVALID_POS;
    }

    if (ascii[0] >= 'A' && ascii[0] <= 'H')
    {
        x = ascii[0] - 'A';
    }
    else if (ascii[0] >= 'a' && ascii[0] <= 'h')
    {
        x = ascii[0] - 'a';
    }
    else
    {
        return NOMOVE_POS;
    }

    if (ascii[1] >= '1' && ascii[1] <= '8')
    {
        y = ascii[1] - '1';
    }
    else
    {
        return NOMOVE_POS;
    }

    return static_cast<Position>(63 - (x + y * 8));
}

/**
 * @brief 位置インデックスから64bit位置情報を計算
 *
 * @param posIdx 位置インデックス
 * @return uint64_t bit位置
 */
uint64_t CalcPosBit(const unsigned char posIdx)
{
    return (uint64_t)0x0000000000000001 << posIdx;
}

/**
 * @brief 位置インデックスに対応するAscii位置情報を計算
 *
 * @param posIdx 位置インデックス
 * @param x X座標出力
 * @param y y座標出力
 */
void CalcPosAscii(const unsigned char posIdx, char *const x, int *const y)
{
    *x = 'H' - posIdx % 8;
    *y = 8 - (posIdx / 8);
}
