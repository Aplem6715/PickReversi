#include "search_result.h"

#include "movelist.h"

namespace solver
{
    Move* SearchResult::GetBestMove()
    {
        Move* best = moveList_->moves_;
        for (Move* move = moveList_->moves_; move != nullptr; move = move->next_)
        {
            if (move->value_ > best->value_)
            {
                best = move;
            }
        }
        return best;
    }
}
