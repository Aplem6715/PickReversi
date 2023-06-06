#ifndef SEARCH_RESULT_H
#define SEARCH_RESULT_H

#include "movelist.h"

namespace solver
{
    struct Move;

    struct SearchResult
    {
        MoveList moveList_[1];

        Move* GetBestMove();
    };
} // namespace solver


#endif