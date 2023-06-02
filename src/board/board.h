#ifndef BOARD_H
#define BOARD_H

#include "../const.h"
#include <stdint.h>

// 最大履歴数（64マスで初期4マス埋まっているので60手分必要）
constexpr unsigned char HIST_LENGTH = 60;

struct History
{
    uint64_t flip;
    Position pos;
    Color side;
};

class Board
{
public:
    Board();
    ~Board();

    uint64_t GetOwn() { return _side == Color::Black ? _black : _white; }
    uint64_t GetOpp() { return _side == Color::Black ? _white : _black; }
    Color GetSide() { return _side; }

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
    uint64_t _white;
    uint64_t _black;

    Color _side;
    History _history[HIST_LENGTH];
    int _nbPlayed;
};

#endif