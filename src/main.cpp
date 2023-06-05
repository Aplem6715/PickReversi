#include "game/game.h"
#include <iostream>
#include <stdio.h>

int main()
{
    auto game = new game::Game(game::PlayerType::Console, game::PlayerType::AI);
    game->Play();
    delete game;

    return 0;
}
