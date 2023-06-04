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
        black_    = INIT_BLACK;
        white_    = INIT_WHITE;
        side_     = Color::Black;
        nbPlayed_ = 0;
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
                if (cursor & black_)
                {
                    std::cout << BLACK_CUI_ICON;
                }
                else if (cursor & white_)
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
                  << BLACK_CUI_ICON << ":" << (int)CountBits(black_) << "     "
                  << WHITE_CUI_ICON << ":" << (int)CountBits(white_) << "\n";
        std::cout << "\n";
    }

    void Board::ChangeSide()
    {
        side_ = side_ == Color::White ? Color::Black : Color::White;
    }

    uint64_t Board::Put(Position pos)
    {
        uint64_t flip;
        if (side_ == Color::Black)
        {
            flip   = CalcFlip64(black_, white_, pos);
            black_ = black_ ^ flip ^ PosToBit(pos);
            white_ = white_ ^ flip;
        }
        else
        {
            flip   = CalcFlip64(white_, black_, pos);
            black_ = black_ ^ flip;
            white_ = white_ ^ flip ^ PosToBit(pos);
        }
        // 着手情報を保存（どっちが，どこに打ち，どこを反転させたか）
        history_[nbPlayed_].side_ = side_;
        history_[nbPlayed_].pos_  = pos;
        history_[nbPlayed_].flip_ = flip;
        nbPlayed_++;
        ChangeSide();
        return flip;
    }

    bool Board::Undo()
    {
        if (nbPlayed_ > 0)
        {
            nbPlayed_--;

            Color hist_turn = history_[nbPlayed_].side_;
            uint64_t flip   = history_[nbPlayed_].flip_;
            uint64_t posBit = PosToBit(history_[nbPlayed_].pos_);

            if (hist_turn == Color::Black)
            {
                black_ = black_ ^ flip ^ posBit;
                white_ = white_ ^ flip;
            }
            else
            {
                black_ = black_ ^ flip;
                white_ = white_ ^ flip ^ posBit;
            }
            side_ = hist_turn;
            return true;
        }
        return false;
    }

    bool Board::UndoWhileSameColor()
    {
        Color oppTurn = AntiColor(side_);
        if (Undo())
        {
            while (side_ == oppTurn)
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
            return CountBits(black_);
        }
        else
        {
            return CountBits(white_);
        }
    }

    // TODO: .hに
    uint64_t Board::GetStone(Color color)
    {
        return color == Color::Black ? black_ : white_;
    }

    bool Board::CanPut()
    {
        return CalcMobility64(GetOwn(), GetOpp()) != 0;
    }

    bool Board::CheckLegalMove(Position pos)
    {
        if (nbPlayed_ >= HIST_LENGTH)
        {
            return false;
        }
        uint64_t mob = CalcMobility64(GetOwn(), GetOpp());
        uint64_t put = PosToBit(pos);
        return (mob & put) != 0;
    }

    bool Board::IsFinished()
    {
        if (nbPlayed_ >= HIST_LENGTH)
        {
            return true;
        }

        return (CalcMobility64(black_, white_) == 0) &&
               (CalcMobility64(white_, black_) == 0);
    }
}
