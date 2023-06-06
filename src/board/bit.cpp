
#include "bit.h"
#include <assert.h>

namespace board
{

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
            return Position::Undo;

        if (ascii.length() < 2)
        {
            return Position::Invalid;
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
            return Position::NoMove;
        }

        if (ascii[1] >= '1' && ascii[1] <= '8')
        {
            y = ascii[1] - '1';
        }
        else
        {
            return Position::NoMove;
        }

        return static_cast<Position>(x + y * 8);
    }

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
}
