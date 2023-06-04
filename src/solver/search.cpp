#include "search.h"
#include "board/bit.h"
#include "movelist.h"

namespace solver
{
    Searcher::Searcher()
    {
    }

    Searcher::~Searcher()
    {
    }

    void Searcher::Reset()
    {
    }

    void Searcher::Setup(SearchOption option)
    {
        option_ = option;
    }

    void Searcher::SetStones(stone_t own, stone_t opp)
    {
        nbEmpty_ = CountBits(~(own | opp));

        stones_->own_ = own;
        stones_->opp_ = opp;

        eval_->Reload(own, opp, Side::Own);
    }

    Position Searcher::Search()
    {
        Position pos = Position::NoMove;

        if (nbEmpty_ <= option_.endDepth_)
        {
            // pos = EndRoot();
        }
        else
        {
            // pos = MidRoot();
        }

        return pos;
    }

    Position Searcher::Search(std::vector<float> ratio)
    {
        // TODO: implement
        return Position::NoMove;
    }

    void Searcher::MakeMoveList(MoveList* moveList)
    {
        Move* cursor  = moveList->moves_;
        Move* prev    = nullptr;
        Stone* stones = stones_;
        uint64_t mob  = CalcMobility64(stones->own_, stones->opp_);
        int nbMove    = 0;

        Position pos;
        for_bits(pos, mob)
        {
            ++nbMove;
            cursor->pos_   = pos;
            cursor->value_ = 0;
            cursor->flips_ = stones->CalcFlip(pos);

            if (prev)
            {
                prev->next_ = cursor;
            }
            prev = cursor;
        }
        prev->next_       = nullptr;
        moveList->length_ = nbMove;
    }
}
