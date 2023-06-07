#ifndef POSITION_HELPER_H
#define POSITION_HELPER_H

#include "const.h"
#include <string>

class PositionHelper
{
public:
    static const std::string ToString(Position pos)
    {
        return kPosNames[static_cast<int>(pos)];
    }
    
    /**
     * @brief Ascii位置情報から位置番号を計算
     *
     * @param ascii Ascii位置情報
     * @return uint8_t 位置インデックス（位置番号
     */
    static Position PosIndexFromAscii(std::string_view ascii);

    static int PositionsFromAscii(std::string_view ascii, Position* moves);
};

#endif
