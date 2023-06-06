#ifndef SEARCH_OPTION_H
#define SEARCH_OPTION_H

#include "../const.h"

namespace solver
{
    enum class SearchMethod : uint8_t
    {
        MinMax,
        AlphaBeta,
        NegaScout,
    };

    struct SearchOption
    {
        uint8_t midDepth_;
        uint8_t endDepth_;
        SearchMethod method_;
    };

    constexpr SearchOption DEFAULT_OPTION = {
        .midDepth_ = 4,
        .endDepth_ = 8,
    };
} // namespace solver

#endif