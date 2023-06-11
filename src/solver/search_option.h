#ifndef SEARCH_OPTION_H
#define SEARCH_OPTION_H

#include "../const.h"

namespace solver
{
    constexpr int kMidHashDepth = 2;
    constexpr int kEndHashDepth = 6;

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
        .midDepth_ = 8,
        .endDepth_ = 16,
        .hashSize_ = 1 << 24, // ~= 16[MB]
        .method_ = SearchMethod::AlphaBeta,
    };
} // namespace solver

#endif