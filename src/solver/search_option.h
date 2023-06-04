#ifndef SEARCH_OPTION_H
#define SEARCH_OPTION_H

#include "../const.h"

namespace solver
{
    struct SearchOption
    {
        uint8_t midDepth_;
        uint8_t endDepth_;
    };

    constexpr SearchOption DEFAULT_OPTION = {
        .midDepth_ = 8,
        .endDepth_ = 16,
    };
} // namespace solver

#endif