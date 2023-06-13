#ifndef AI_PLAYER_H
#define AI_PLAYER_H

#include "player.h"

namespace eval
{
    class PositionEval;
}

namespace solver
{
    template <class Evaluator>
    class Searcher;
}

namespace game
{

    class AIPlayer : public Player
    {
    public:
        AIPlayer();
        ~AIPlayer();

        virtual Position Think(uint64_t own, uint64_t opp) override;
        virtual void NotifyUndo() override {} /*なにもしない*/

    private:
        solver::Searcher<eval::PositionEval>* searcher_;
    };
}

#endif
