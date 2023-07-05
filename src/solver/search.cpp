#include "search.h"
#include "board/bit.h"
#include "board/stone.h"
#include "eval/pattern_eval.h"
#include "eval/pos_eval.h"
#include "movelist.h"
#include "search_result.h"

#include <cassert>

namespace solver
{
    template class Searcher<eval::PositionEval>;
    template class Searcher<eval::PatternEval>;

    constexpr int kHashDepth  = 4;
    constexpr int kOrderDepth = 6;

    template <int N>
    concept hash_enable_depth = requires() {
        N > kHashDepth;
    };

    template <int N>
    concept order_enable_depth = requires() {
        N > kOrderDepth;
    };

    using Stone = board::Stone;

    template <class Evaluator>
    Searcher<Evaluator>::Searcher()
    {
        option_ = option_;
        table_  = new HashTable(option_.hashSize_);
        PROFILE(table_->BindProf(&prof_.hash));
    }

    template <class Evaluator>
    Searcher<Evaluator>::~Searcher()
    {
        delete table_;
    }

    template <class Evaluator>
    void Searcher<Evaluator>::Reset()
    {
        table_->Clear();
        PROFILE(prof_ = bench::kProfileInit);
    }

    template <class Evaluator>
    void Searcher<Evaluator>::Setup(SearchOption option)
    {
        option_ = option;
        delete table_;
        table_ = new HashTable(option.hashSize_);
    }

    template <class Evaluator>
    void Searcher<Evaluator>::SetStones(stone_t own, stone_t opp)
    {
        nbEmpty_ = CountBits(~(own | opp));

        stones_.own_ = own;
        stones_.opp_ = opp;

        eval_.Reload(own, opp);
    }

    template <class Evaluator>
    void Searcher<Evaluator>::Search(stone_t own, stone_t opp, SearchResult* result)
    {
        Position pos = Position::NoMove;

        PROFILE(
            std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
            start_time = std::chrono::high_resolution_clock::now();)

        SetStones(own, opp);
        if (nbEmpty_ <= option_.endDepth_)
        {
            EndRoot(result);
            PROFILE(prof_.depth = option_.endDepth_);
        }
        else
        {
            MidRoot(result);
            PROFILE(prof_.depth = option_.midDepth_);
        }

        table_->VersionUp();

        PROFILE(
            std::chrono::time_point<std::chrono::high_resolution_clock> end_time = std::chrono::high_resolution_clock::now();
            prof_.duration                                                       = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() / 1000.0;
            prof_.pos                                                            = pos;
            prof_.score                                                          = result->GetBestMove()->value_;)
    }

    template <class Evaluator>
    void Searcher<Evaluator>::OnEnterMidSearch()
    {
        table_->ClearScore();
    }

    template <class Evaluator>
    void Searcher<Evaluator>::OnEnterEndSearch()
    {
        table_->ClearScore();
    }

    template <class Evaluator>
    void Searcher<Evaluator>::MakeMoveList(MoveList* moveList)
    {
        Move* prev    = moveList->moves_;
        Move* cursor  = moveList->moves_ + 1;
        Stone* stones = &stones_;
        uint64_t mob  = CalcMobility64(stones->own_, stones->opp_);
        int nbMove    = 0;

        *prev = kDefaultMove;

        Position pos;
        for_bits(pos, mob)
        {
            ++nbMove;
            cursor->pos_   = pos;
            cursor->value_ = INT32_MIN;
            cursor->flips_ = stones->CalcFlip(pos);

            prev->next_ = cursor;
            prev        = prev->next_;
            ++cursor;
        }
        prev->next_          = nullptr;
        moveList->length_    = nbMove;
        moveList->lastMove_  = moveList->moves_;
        moveList->evaluated_ = false;
    }

}
