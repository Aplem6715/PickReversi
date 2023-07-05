#ifndef MOVELIST_H
#define MOVELIST_H

#include "const.h"

namespace solver
{

    template <class Evaluator>
    class Searcher;

    struct HashData;

    struct Move
    {
        stone_t flips_;
        int value_;
        Position pos_;
        Move* next_;

        template <class Evaluator>
        void Evaluate(Searcher<Evaluator>& searcher, const HashData& hashData);
    };

    constexpr Move kDefaultMove = {0, INT32_MIN, Position::Invalid, nullptr};

    struct MoveList
    {
    public:
        // 順不定
        Move moves_[kMaxMove];
        int length_;
        bool evaluated_;

        Move* lastMove_;

    public:
        inline Move* GetNextBest();
        inline Move* GetNext();
        bool IsEmpty() { return length_ == 0; }

        template <class Evaluator>
        void Evaluate(Searcher<Evaluator>& searcher, const HashData& hashData)
        {
            for (auto move = moves_->next_; move; move = move->next_)
            {
                move->Evaluate<Evaluator>(searcher, hashData);
            }
            evaluated_ = true;
        }
    };

    inline Move* solver::MoveList::GetNextBest()
    {
        if (!lastMove_->next_)
        {
            return nullptr;
        }

        if (!evaluated_)
        {
            return GetNext();
        }

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
        }
        // 次の着手
        lastMove_ = lastMove_->next_;
        return lastMove_;
    }

    inline Move* solver::MoveList::GetNext()
    {
        lastMove_ = lastMove_->next_;
        return lastMove_;
    }
}

#endif