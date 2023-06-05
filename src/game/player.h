#ifndef PLAYER_H
#define PLAYER_H

#include "const.h"
#include "util/color_helper.h"
#include <memory>
#include <string>

namespace spdlog
{
    class logger;
}

namespace game
{
    class Player
    {
    public:
        virtual Position Think(uint64_t own, uint64_t opp) = 0;

        virtual void SetLogger(std::shared_ptr<spdlog::logger> logger) { logger_ = logger; }
        virtual void SetColor(Color color) { color_ = color; }
        virtual void NotifyUndo(){};
        virtual void OnOpponentPut(Position pos) {}

        /// @brief プレイヤー名を取得（デフォルトは色名）
        virtual const std::wstring GetName() { return ColorHelper::ToString(color_); }

    protected:
        Color color_;
        std::string name_;
        std::shared_ptr<spdlog::logger> logger_;
    };
}

#endif