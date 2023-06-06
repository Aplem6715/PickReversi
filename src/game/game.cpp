#include "game.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "../const.h"
#include "buffer_sink.h"
#include "game/ai_player.h"
#include "game/console_player.h"
#include "game/player.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "util/position_helper.h"

namespace game
{
    const std::wstring kSkipMessage       = L"置く場所がありません!{}の手番はパスされました(Enterで次へ)";
    const std::wstring kUndoMessage       = L"「待った!!」（戻しました）";
    const std::wstring kIllegalPosMessage = L"{}には置けませんが?（#^ω^）";
    const std::wstring kCantUndo          = L"これ以上戻せません";
    const std::wstring kPutMessage        = L"{}が{}に置きました";

    std::unique_ptr<spdlog::logger> MakeLogger()
    {
        auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_st>();
        consoleSink->set_level(spdlog::level::debug);
        consoleSink->set_pattern("[game] [%^%l%$] %v");

        auto bufferSink = std::make_shared<BufferSink_st>(consoleSink);

        auto logger = std::make_unique<spdlog::logger>("game", bufferSink);
        logger->set_level(spdlog::level::debug);
        return logger;
    }

    Game::Game(PlayerType blackPlayer, PlayerType whitePlayer)
    {
        logger_ = MakeLogger();
        SetBlackPlayer(MakePlayer(blackPlayer));
        SetWhitePlayer(MakePlayer(whitePlayer));
        logger_->flush();
    }

    void Game::Reset()
    {
        board_->Reset();
    }

    void Game::Play()
    {
        if (!blackPlayer_ || !whitePlayer_)
        {
            return;
        }

        Reset();

        logger_->info("対局開始");

        MainLoop();

        ShowResult();
    }

    void Game::SetBlackPlayer(Player* player)
    {
        blackPlayer_ = player;
        if (blackPlayer_)
        {
            blackPlayer_->SetColor(Color::Black);
        }
    }

    void Game::SetWhitePlayer(Player* player)
    {
        whitePlayer_ = player;
        if (whitePlayer_)
        {
            whitePlayer_->SetColor(Color::White);
        }
    }

    void Game::MainLoop()
    {
        Player* currentPlayer;
        Player* opponentPlayer;

        while (!board_->IsFinished())
        {
            currentPlayer  = GetCurrentPlayer();
            opponentPlayer = GetOpponentPlayer();
            board_->Print();
            logger_->flush();

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
                logger_->info(kIllegalPosMessage, PositionHelper::ToString(pos));
                continue;
            }

            logger_->info(kPutMessage, currentPlayer->GetName(), PositionHelper::ToString(pos));

            board_->Put(pos);

            opponentPlayer->OnOpponentPut(pos);
        }
    }

    void Game::ShowResult()
    {
        board_->Print();
        int numBlack = board_->StoneCount(Color::Black);
        int numWhite = board_->StoneCount(Color::White);

        if (numBlack == numWhite)
        {
            logger_->info("引き分け！！");
        }
        else if (numBlack > numWhite)
        {
            logger_->info(L"{}の勝ち!", kBlackIcon);
        }
        else
        {
            logger_->info(L"{}の勝ち!", kWhiteIcon);
        }
        logger_->info("Enterで終了");
        std::cin.get();
        std::cin.get();
    }

    Player* Game::MakePlayer(PlayerType type)
    {
        Player* ret = nullptr;
        switch (type)
        {
        case PlayerType::Console:
            ret = new ConsolePlayer();
            break;

        case PlayerType::AI:
            ret = new AIPlayer();
            break;

        default:
            logger_->error("Player type:{} not defined", static_cast<int>(type));
            break;
        }

        if (ret)
        {
            ret->SetLogger(logger_);
        }
        return ret;
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