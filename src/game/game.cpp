#include "game.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "../const.h"
#include "util/position_helper.h"
#include "game/player.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace game
{
    constexpr auto kSkipMessage       = "置く場所がありません!「{}」の手番はパスされました(Enterで次へ)";
    constexpr auto kUndoMessage       = "{}「待った!!」（戻しました）";
    constexpr auto kIllegalPosMessage = "そこには置けませんが?（#^ω^）";
    constexpr auto kCantUndo          = "これ以上戻せません";
    constexpr auto kPutMessage        = "{}が{}に置きました";

    std::unique_ptr<spdlog::logger> MakeLogger()
    {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::err);
        console_sink->set_pattern("[game] [%^%l%$] %v");

        auto logger = std::make_unique<spdlog::logger>("game", console_sink);
        logger->set_level(spdlog::level::debug);
        return logger;
    }

    Game::Game(Player* blackPlayer, Player* whitePlayer)
        : blackPlayer_(blackPlayer), whitePlayer_(whitePlayer)
    {
        logger_ = MakeLogger();
    }

    void Game::Reset()
    {
        board_->Reset();
    }

    void Game::Play()
    {
        Reset();

        logger_->info("対局開始");

        MainLoop();

        ShowResult();
    }

    void Game::MainLoop()
    {
        Player* currentPlayer;
        Player* opponentPlayer;

        logger_->enable_backtrace(100);

        while (!board_->IsFinished())
        {
            currentPlayer  = GetCurrentPlayer();
            opponentPlayer = GetOpponentPlayer();
            board_->Print();
            logger_->dump_backtrace();

            if (!board_->CanPut())
            {
                logger_->info(kSkipMessage, currentPlayer->GetName());
                board_->Skip();
                continue;
            }

            const Position pos = currentPlayer->Think(board_->GetOwn(), board_->GetOpp());

            if (pos == Position::Undo)
            {
                if (board_->UndoWhileSameColor())
                {
                    logger_->info(kUndoMessage, currentPlayer->GetName());
                }
                else
                {
                    logger_->info(kCantUndo);
                }
                continue;
            }

            if (!board_->CheckLegalMove(pos))
            {
                logger_->info(kIllegalPosMessage);
                continue;
            }

            logger_ ->info(kPutMessage, currentPlayer->GetName(), PositionHelper::ToString(pos));

            board_->Put(pos);

            opponentPlayer->OnOpponentPut(pos);
        }
        logger_->disable_backtrace();
    }

    void Game::ShowResult()
    {
        board_->Print();
        int numBlack = board_->StoneCount(Color::Black);
        int numWhite = board_->StoneCount(Color::White);

        if (numBlack == numWhite)
        {
            std::cout << "引き分け！！\n";
        }
        else if (numBlack > numWhite)
        {
            std::cout << kBlackIcon << "の勝ち！\n";
        }
        else
        {
            std::cout << kWhiteIcon << "の勝ち！\n";
        }
        printf("Enterで終了\n");
        std::cin.get();
        std::cin.get();
    }

    Player* Game::GetCurrentPlayer()
    {
        return board_->GetSide() == Color::Black ? blackPlayer_ : whitePlayer_;
    }

    Player* Game::GetOpponentPlayer()
    {
        return board_->GetSide() == Color::Black ? whitePlayer_ : blackPlayer_;
    }
}