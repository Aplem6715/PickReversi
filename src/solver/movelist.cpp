#include "movelist.h"

namespace solver
{

    Move* solver::MoveList::GetNextBest()
    {
        // 最善手を取得
        Move* best = lastMove_;
        for (Move* target = lastMove_; target->next_ != nullptr; target = target->next_)
        {
            if (target->next_->value_ > best->next_->value_)
            {
                best = target;
            }
        }

        if (best != lastMove_)
        {
            // bestを切り離す
            Move* tmp   = best->next_;
            best->next_ = tmp->next_;
            tmp->next_  = lastMove_->next_;

            // 繋げ直し
            lastMove_->next_ = tmp;
            // 次の着手
            lastMove_        = tmp;
        }

        return lastMove_;
    }

    Move* solver::MoveList::GetNext()
    {
        lastMove_  = lastMove_->next_;
        return lastMove_;
    }
}
