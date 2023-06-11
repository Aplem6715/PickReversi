#ifndef SEARCH_OPTION_H
#define SEARCH_OPTION_H

#include "../const.h"

#define USE_HASH 1
#define USE_ORDER 1

namespace solver
{
    constexpr int kMidHashDepth  = 2;
    constexpr int kEndHashDepth  = 6;
    constexpr int kMidOrderingDepth = 4;
    constexpr int kEndOrderingDepth = 6;

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
        uint64_t hashSize_;
        SearchMethod method_;
    };

    constexpr SearchOption DEFAULT_OPTION = {
        .midDepth_ = 10,
        .endDepth_ = 18,
        .hashSize_ = 1 << 24, // ~= 16[MB]
        .method_   = SearchMethod::AlphaBeta,
    };
} // namespace solver

#endif