
#ifndef TRAIN_BUILD

#include "game/game.h"
#include "game/record.h"
#include <iostream>
#include <stdio.h>

int main()
{
    auto game = new game::Game(game::PlayerType::Console, game::PlayerType::AI);
    game->Play();
    game::MatchBook book;
    book.AddRecord(game->GetRecord());
    book.WriteAscii("resource/kihu.txt");
    delete game;

    return 0;
}

#endif