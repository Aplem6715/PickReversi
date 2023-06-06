#ifndef GAME_H
#define GAME_H

#include "board/board.h"
#include <spdlog/spdlog.h>
#include <memory>

namespace game
{
    using namespace board;

    class Player;

    enum class PlayerType
    {
        Console,
        AI,
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

    private:
        Board board_[1];
        Player* blackPlayer_;
        Player* whitePlayer_;
        std::shared_ptr<spdlog::logger> logger_;

    private:
        void MainLoop();
        void ShowResult();
        Player* MakePlayer(PlayerType type);

        Player* GetCurrentPlayer();
        Player* GetOpponentPlayer();
    };
}
#endif