
#include "bit.h"
#include <assert.h>

/**
 * @brief 位置インデックスに対応するAscii位置情報を計算
 *
 * @param posIdx 位置インデックス
 * @param x X座標出力
 * @param y y座標出力
 */
void CalcPosAscii(const Position posIdx, char* const x, int* const y)
{
    *x = 'H' - static_cast<int>(posIdx) % 8;
    *y = 8 - (static_cast<int>(posIdx) / 8);
}
