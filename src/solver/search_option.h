#ifndef SEARCH_OPTION_H
#define SEARCH_OPTION_H

#include "../const.h"

namespace solver
{
    struct SearchOption
    {
        uint8_t MidDepth;
        uint8_t EndDepth;
    };

    constexpr SearchOption DEFAULT_OPTION = {
        .MidDepth = 8,
        .EndDepth = 16,
    };
} // namespace solver

#endif