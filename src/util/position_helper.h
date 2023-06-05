#ifndef POSITION_HELPER_H
#define POSITION_HELPER_H

#include "const.h"
#include <string>

class PositionHelper
{
public:
    static const std::string ToString(Position pos)
    {
        return POS_NAMES[static_cast<int>(pos)];
    }
};

#endif
