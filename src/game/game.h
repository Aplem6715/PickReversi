#ifndef GAME_H
#define GAME_H

#include "board/board.h"
#include "spdlog/spdlog.h"
#include <memory>

namespace game
{
    using namespace board;

    class Player;

    class Game
    {
    public:
        Game(Player* blackPlayer, Player* whitePlayer);
        ~Game() {}

        void Reset();
        void Play();
        void SetBlackPlayer(Player* player) { blackPlayer_ = player; }
        void SetWhitePlayer(Player* player) { whitePlayer_ = player; }

    private:
        Board board_[1];
        Player* blackPlayer_;
        Player* whitePlayer_;
        std::unique_ptr<spdlog::logger> logger_;

    private:
        void MainLoop();
        void ShowResult();

        Player* GetCurrentPlayer();
        Player* GetOpponentPlayer();
    };
}
#endif