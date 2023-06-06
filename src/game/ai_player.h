#ifndef AI_PLAYER_H
#define AI_PLAYER_H

#include "player.h"

namespace solver
{
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
        solver::Searcher* searcher_;
    };
}

#endif
