﻿#ifndef GAME_H
#define GAME_H

#include "board/board.h"
#include "record.h"
#include <memory>

namespace game
{
    using namespace board;

    class Player;
    class Logger;

    enum class PlayerType
    {
        Console,
        AI_Position,
        AI_Pattern,
    };

    class Game
    {
    public:
        Game(PlayerType blackPlayer, PlayerType whitePlayer);
        ~Game() {}

        void Reset();
        void Play();
        void SetBlackPlayer(Player* player);
        void SetWhitePlayer(Player* player);
        const MatchRecord& GetRecord() { return record_; }

    private:
        Board board_[1];
        Player* blackPlayer_;
        Player* whitePlayer_;
        MatchRecord record_;
        std::shared_ptr<Logger> logger_;

    private:
        void MainLoop();
        void ShowResult();
        Player* MakePlayer(PlayerType type);

        Player* GetCurrentPlayer();
        Player* GetOpponentPlayer();
    };
}
#endif