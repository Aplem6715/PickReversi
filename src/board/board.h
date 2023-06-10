#ifndef BOARD_H
#define BOARD_H

#include "../const.h"
#include <stdint.h>

namespace board
{

    // 最大履歴数（64マスで初期4マス埋まっているので60手分必要）
    constexpr unsigned char HIST_LENGTH = 60;

    struct History
    {
        uint64_t flip_;
        Position pos_;
        Color side_;
    };

    class Board
    {
    public:
        Board();
        ~Board();

        uint64_t GetOwn() { return side_ == Color::Black ? black_ : white_; }
        uint64_t GetOpp() { return side_ == Color::Black ? white_ : black_; }
        Color GetSide() { return side_; }

        void Reset();
        bool Skip();
        void Print();
        void ChangeSide();

        uint64_t Put(Position pos);
        bool Undo();
        bool UndoWhileSameColor();
        int StoneCount(Color color);
        uint64_t GetStone(Color color);
        bool CanPut();
        bool CheckLegalMove(Position pos);
        bool IsFinished();

    private:
        uint64_t white_;
        uint64_t black_;

        Color side_;
        History history_[HIST_LENGTH];
        int nbPlayed_;
    };
}

#endif
