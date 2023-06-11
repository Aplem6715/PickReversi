#ifndef SEARCH_OPTION_H
#define SEARCH_OPTION_H

#include "../const.h"

#define USE_HASH 1
#define USE_ORDER 1

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
        uint64_t hashSize_;
        SearchMethod method_;

        int midHashDepth;
        int endHashDepth;
        int midOrderingDepth;
        int endOrderingDepth;
    };

    constexpr SearchOption DEFAULT_OPTION = {
        .midDepth_ = 10,
        .endDepth_ = 18,
        .hashSize_ = 1 << 24, // ~= 16[MB]
        .method_   = SearchMethod::AlphaBeta,

        .midHashDepth     = 2,
        .endHashDepth     = 6,
        .midOrderingDepth = 4,
        .endOrderingDepth = 6,
    };

    constexpr SearchOption kAccuracyTestOption = {
        .midDepth_ = 8,
        .endDepth_ = 12,
        .hashSize_ = 1 << 24, // ~= 16[MB]
        .method_   = SearchMethod::AlphaBeta,

        .midHashDepth     = 2,
        .endHashDepth     = 6,
        .midOrderingDepth = 4,
        .endOrderingDepth = 6,
    };
    
} // namespace solver

#endif