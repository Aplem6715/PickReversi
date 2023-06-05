#include "game/game.h"
#include <iostream>
#include <stdio.h>

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

#include "game/buffer_sink.h"

#include "util/position_helper.h"

const std::wstring kPutMessage = L"{}に置きました";

std::unique_ptr<spdlog::logger> MakeLogger()
{
    auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    consoleSink->set_level(spdlog::level::debug);
    consoleSink->set_pattern("[game] [%^%l%$] %v");

    auto bufferSink = std::make_shared<BufferSink_st>(consoleSink);

    auto logger = std::make_unique<spdlog::logger>("game", bufferSink);
    logger->set_level(spdlog::level::debug);
    return logger;
}

int main()
{
    // auto logger = MakeLogger();
    // logger->info("ああああ");
    // logger->info(kPutMessage, PositionHelper::ToString(Position::A1));
    // logger->info(kPutMessage, PositionHelper::ToString(Position::A1));
    // logger->info(kPutMessage, PositionHelper::ToString(Position::A1));
    // logger->flush();

    auto game = new game::Game(game::PlayerType::Console, game::PlayerType::Console);
    game->Play();
    delete game;

    return 0;
}
