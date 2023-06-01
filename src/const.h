#ifndef CONST_H
#define CONST_H

#define GLOBAL_SEED 42

#include <string>

// 最大の着手可能位置数(46以下であることは証明されているらしい)
// 実際に見つかった盤面は（手順無視で）34が最大？手順通りだと30?
constexpr int MAX_MOVES = 40;

constexpr char BLACK_CUI_ICON[] = "##";
constexpr char WHITE_CUI_ICON[] = "--";

constexpr int OWN = 0;
constexpr int OPP = 1;

enum Color
{
	BLACK,
	WHITE,
};

// clang-format off
enum Position
{
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8,
    NOMOVE_POS, PASS_POS, UNDO_POS, INVALID_POS, POS_MAX
};
// clang-format on

// clang-format off
constexpr auto POS_NAMES = {
	"A1", "B1", "C1", "D1", "E1", "F1", "G1", "H1",
	"A2", "B2", "C2", "D2", "E2", "F2", "G2", "H2",
	"A3", "B3", "C3", "D3", "E3", "F3", "G3", "H3",
	"A4", "B4", "C4", "D4", "E4", "F4", "G4", "H4",
	"A5", "B5", "C5", "D5", "E5", "F5", "G5", "H5",
	"A6", "B6", "C6", "D6", "E6", "F6", "G6", "H6",
	"A7", "B7", "C7", "D7", "E7", "F7", "G7", "H7",
	"A8", "B8", "C8", "D8", "E8", "F8", "G8", "H8",
	"なにもない",
	"パス",
	"戻す",
	"無効な位置",
	"けつばん"
};
// clang-format on

#endif