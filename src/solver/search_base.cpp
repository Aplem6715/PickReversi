#include "board/bit.h"
#include "board/stone.h"
#include "eval/pos_eval.h"
#include "movelist.h"
#include "search.h"
#include "search_result.h"

#include <cassert>

namespace solver
{
    template class Searcher<eval::PositionEval>;

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
}
