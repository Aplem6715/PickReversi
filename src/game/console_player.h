#ifndef CONSOLE_PLAYER_H
#define CONSOLE_PLAYER_H

#include "player.h"

namespace game
{
    class ConsolePlayer : public Player
    {
    public:
        ConsolePlayer() {}
        ~ConsolePlayer() {}

        virtual Position Think(uint64_t own, uint64_t opp) override;
        virtual void NotifyUndo() override;
    };
}

#endif
