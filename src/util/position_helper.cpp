#include "position_helper.h"

Position PositionHelper::PosIndexFromAscii(std::string_view ascii)
{
    int x, y;

    if (ascii[0] == 'U' || ascii[0] == 'u')
        return Position::Undo;

    if (ascii.length() < 2)
    {
        return Position::Invalid;
    }

    if (ascii[0] >= 'A' && ascii[0] <= 'H')
    {
        x = ascii[0] - 'A';
    }
    else if (ascii[0] >= 'a' && ascii[0] <= 'h')
    {
        x = ascii[0] - 'a';
    }
    else
    {
        return Position::NoMove;
    }

    if (ascii[1] >= '1' && ascii[1] <= '8')
    {
        y = ascii[1] - '1';
    }
    else
    {
        return Position::NoMove;
    }

    return static_cast<Position>(x + y * 8);
}

int PositionHelper::PositionsFromAscii(std::string_view ascii, Position* moves)
{
    int nbMoves = 0;
    for (int i = 0; i < ascii.length(); i += 2)
    {
        moves[i / 2] = PosIndexFromAscii(ascii.substr(i, 2));
        ++nbMoves;
    }
    return nbMoves;
}
