#ifndef PLAYER_H
#define PLAYER_H

#include "const.h"
#include "util/color_helper.h"
#include <memory>
#include <string>

namespace game
{
    class Logger;

    class Player
    {
    public:
        virtual Position Think(uint64_t own, uint64_t opp) = 0;

        virtual void SetLogger(std::shared_ptr<Logger> logger) { logger_ = logger; }
        virtual void SetColor(Color color) { color_ = color; }
        virtual void NotifyUndo(){};
        virtual void OnOpponentPut(Position pos) {}

        /// @brief プレイヤー名を取得（デフォルトは色名）
        virtual const std::string GetName() { return ColorHelper::ToString(color_); }

    protected:
        Color color_;
        std::string name_;
        std::shared_ptr<Logger> logger_;
    };
}

#endif