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

        Move* lastMove_;

    public:
        Move* GetNextBest();
        Move* GetNext();
        bool IsEmpty() { return length_ == 0; }

        template <class Evaluator>
        void Evaluate(Searcher<Evaluator>& searcher, const HashData& hashData)
        {
            for (auto move = moves_->next_; move; move = move->next_)
            {
                move->Evaluate<Evaluator>(searcher, hashData);
            }
        }
    };

}

#endif