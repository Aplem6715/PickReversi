#include <iostream>

#include "bit.h"
#include "board.h"

namespace board
{
    constexpr uint64_t INIT_BLACK = 0x0000000810000000;
    constexpr uint64_t INIT_WHITE = 0x0000001008000000;

    Board::Board() { Reset(); }

    Board::~Board()
    {
        // TODO
    }

    void Board::Reset()
    {
        _black    = INIT_BLACK;
        _white    = INIT_WHITE;
        _side     = Color::Black;
        _nbPlayed = 0;
    }

    bool Board::Skip()
    {
        // 置ける場所があるうちはスキップできない
        if (CanPut())
        {
            return false;
        }
        ChangeSide();
        return true;
    }

    void Board::Print()
    {
        uint64_t mobility = CalcMobility64(GetOwn(), GetOpp());

        uint64_t cursor = 0x0000000000000001;
        std::cout << "  A B C D E F G H\n";
        for (int y = 0; y < BOARD_SIZE; y++)
        {
            std::cout << y + 1 << " ";
            for (int x = 0; x < BOARD_SIZE; x++)
            {
                if (cursor & _black)
                {
                    std::cout << BLACK_CUI_ICON;
                }
                else if (cursor & _white)
                {
                    std::cout << WHITE_CUI_ICON;
                }
                else if (cursor & mobility)
                {
                    std::cout << ".";
                }
                else
                {
                    std::cout << "-";
                }
                std::cout << " ";
                cursor <<= 1;
            }
            std::cout << y + 1 << " ";
            std::cout << "\n";
        }
        std::cout << "  A B C D E F G H\n";
        std::cout << "    "
                  << BLACK_CUI_ICON << ":" << (int)CountBits(_black) << "     "
                  << WHITE_CUI_ICON << ":" << (int)CountBits(_white) << "\n";
        std::cout << "\n";
    }

    void Board::ChangeSide()
    {
        _side = _side == Color::White ? Color::Black : Color::White;
    }

    uint64_t Board::Put(Position pos)
    {
        uint64_t flip;
        if (_side == Color::Black)
        {
            flip   = CalcFlip64(_black, _white, pos);
            _black = _black ^ flip ^ CalcPosBit(pos);
            _white = _white ^ flip;
        }
        else
        {
            flip   = CalcFlip64(_white, _black, pos);
            _black = _black ^ flip;
            _white = _white ^ flip ^ CalcPosBit(pos);
        }
        // 着手情報を保存（どっちが，どこに打ち，どこを反転させたか）
        _history[_nbPlayed].side = _side;
        _history[_nbPlayed].pos  = pos;
        _history[_nbPlayed].flip = flip;
        _nbPlayed++;
        ChangeSide();
        return flip;
    }

    bool Board::Undo()
    {
        if (_nbPlayed > 0)
        {
            _nbPlayed--;

            Color hist_turn = _history[_nbPlayed].side;
            uint64_t flip   = _history[_nbPlayed].flip;
            uint64_t posBit = CalcPosBit(_history[_nbPlayed].pos);

            if (hist_turn == Color::Black)
            {
                _black = _black ^ flip ^ posBit;
                _white = _white ^ flip;
            }
            else
            {
                _black = _black ^ flip;
                _white = _white ^ flip ^ posBit;
            }
            _side = hist_turn;
            return true;
        }
        return false;
    }

    bool Board::UndoWhileSameColor()
    {
        Color oppTurn = AntiColor(_side);
        if (Undo())
        {
            while (_side == oppTurn)
            {
                if (!Undo())
                    return true;
            }
        }
        else
        {
            return false;
        }
        return true;
    }

    int Board::StoneCount(Color color)
    {
        if (color == Color::Black)
        {
            return CountBits(_black);
        }
        else
        {
            return CountBits(_white);
        }
    }

    // TODO: .hに
    uint64_t Board::GetStone(Color color)
    {
        return color == Color::Black ? _black : _white;
    }

    bool Board::CanPut()
    {
        return CalcMobility64(GetOwn(), GetOpp()) != 0;
    }

    bool Board::CheckLegalMove(Position pos)
    {
        if (_nbPlayed >= HIST_LENGTH)
        {
            return false;
        }
        uint64_t mob = CalcMobility64(GetOwn(), GetOpp());
        uint64_t put = CalcPosBit(pos);
        return (mob & put) != 0;
    }

    bool Board::IsFinished()
    {
        if (_nbPlayed >= HIST_LENGTH)
        {
            return true;
        }

        return (CalcMobility64(_black, _white) == 0) &&
               (CalcMobility64(_white, _black) == 0);
    }
}
