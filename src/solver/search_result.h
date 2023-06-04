#ifndef SEARCH_RESULT_H
#define SEARCH_RESULT_H

namespace solver
{
    struct Move;
    struct MoveList;

    struct SearchResult
    {
        MoveList* moveList_;

        Move* GetBestMove();
    };
} // namespace solver


#endif