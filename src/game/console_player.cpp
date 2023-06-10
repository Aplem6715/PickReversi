#include "console_player.h"
#include "game/logger.h"
#include "util/position_helper.h"
#include <iostream>
// #include <spdlog/spdlog.h>

namespace game
{
    Position game::ConsolePlayer::Think(uint64_t own, uint64_t opp)
    {
        std::string str_pos;
        Position pos;

        while (true)
        {
            printf("位置を入力してください（A1～H8）:");
            std::cin >> str_pos;

            pos = PositionHelper::PosIndexFromAscii(str_pos);

            if (pos == Position::NoMove)
            {
                printf("(%c, %c) には置けません\n", str_pos[0], str_pos[1]);
            }
            else
            {
                return pos;
            }
        }
    }

    void game::ConsolePlayer::NotifyUndo()
    {
        logger_->Info("入力待ち");
        std::cin.get();
    }
}
