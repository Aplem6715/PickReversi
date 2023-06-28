#include "game.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "../const.h"
#include "game/ai_player.h"
#include "game/console_player.h"
#include "game/player.h"
#include "solver/eval/pos_eval.h"
#include "solver/eval/pattern_eval.h"
#include "logger.h"
#include "util/position_helper.h"

namespace game
{
    // std::unique_ptr<spdlog::logger> MakeLogger()
    // {
    //     auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_st>();
    //     consoleSink->set_level(spdlog::level::debug);
    //     consoleSink->set_pattern("[game] [%^%l%$] %v");

    //     auto bufferSink = std::make_shared<BufferSink_st>(consoleSink);

    //     auto logger = std::make_unique<spdlog::logger>("game", bufferSink);
    //     logger->set_level(spdlog::level::debug);
    //     return logger;
    // }

    Game::Game(PlayerType blackPlayer, PlayerType whitePlayer)
    {
        logger_ = std::make_shared<Logger>();
        SetBlackPlayer(MakePlayer(blackPlayer));
        SetWhitePlayer(MakePlayer(whitePlayer));
        logger_->Flush();
    }

    void Game::Reset()
    {
        board_->Reset();
        record_ = MatchRecord();
    }

    void Game::Play()
    {
        if (!blackPlayer_ || !whitePlayer_)
        {
            return;
        }

        Reset();

        logger_->Info("対局開始");

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
            logger_->Flush(false);
            std::cout << '\n';

            if (!board_->CanPut())
            {
                logger_->Info("置く場所がありません!" + currentPlayer->GetName() + "の手番はパスされました(Enterで次へ)");
                board_->Skip();
                continue;
            }

            const Position pos = currentPlayer->Think(board_->GetOwn(), board_->GetOpp());

            if (pos == Position::Undo)
            {
                if (board_->UndoWhileSameColor())
                {
                    --record_.nMoves_;
                    logger_->Info("「待った!!」（戻しました）");
                }
                else
                {
                    logger_->Info("これ以上戻せません");
                }
                continue;
            }

            if (!board_->CheckLegalMove(pos))
            {
                logger_->Info(PositionHelper::ToString(pos) + "{}には置けませんが?（#^ω^）");
                continue;
            }

            logger_->Info(currentPlayer->GetName() + "が" + PositionHelper::ToString(pos) + "に置きました");

            board_->Put(pos);
            record_.moves_[record_.nMoves_] = pos;
            ++record_.nMoves_;

            opponentPlayer->OnOpponentPut(pos);
        }
        record_.finalDiff_ = board_->StoneCount(Color::Black) - board_->StoneCount(Color::White);
    }

    void Game::ShowResult()
    {
        board_->Print();
        int numBlack = board_->StoneCount(Color::Black);
        int numWhite = board_->StoneCount(Color::White);

        if (numBlack == numWhite)
        {
            logger_->Info("引き分け！！");
        }
        else if (numBlack > numWhite)
        {
            logger_->Info(kBlackIcon + "の勝ち!");
        }
        else
        {
            logger_->Info(kWhiteIcon + "{}の勝ち!");
        }
        logger_->Info("Enterで終了");
    }

    Player* Game::MakePlayer(PlayerType type)
    {
        Player* ret = nullptr;
        switch (type)
        {
        case PlayerType::Console:
            ret = new ConsolePlayer();
            break;

        case PlayerType::AI_Position:
            ret = new AIPlayer<eval::PositionEval>();
            break;

        case PlayerType::AI_Pattern:
            ret = new AIPlayer<eval::PatternEval>();
            break;

        default:
            logger_->Error("Player type:" + std::to_string(static_cast<int>(type)) + " not defined");
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