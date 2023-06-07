#ifndef CONST_H
#define CONST_H

#define GLOBAL_SEED 42

#include "type.h"
#include <string>

// 盤面サイズ
constexpr unsigned char kBoardSize = 8;

const std::wstring kBlackIcon   = L"#";
const std::wstring kWhiteIcon   = L"o";
const std::wstring kInvalidIcon = L"Err";

/// 1石あたりの評価値
constexpr int kEvalStone   = 10;
constexpr int kEvalMax     = kEvalStone * 64;
constexpr int kEvalMin     = -kEvalMax;
constexpr int kEvalInvalid = -999;

enum class Side : uint8_t
{
    Own,
    Opp
};

enum class Color : uint8_t
{
    Black,
    White,
};

// clang-format off
// 着手位置
enum class Position: uint8_t
{
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8,
    NoMove, Pass, Undo, Invalid, Max
};
// clang-format on

// clang-format off
const std::wstring kPosNames[] = {
	L"A1", L"B1", L"C1", L"D1", L"E1", L"F1", L"G1", L"H1",
	L"A2", L"B2", L"C2", L"D2", L"E2", L"F2", L"G2", L"H2",
	L"A3", L"B3", L"C3", L"D3", L"E3", L"F3", L"G3", L"H3",
	L"A4", L"B4", L"C4", L"D4", L"E4", L"F4", L"G4", L"H4",
	L"A5", L"B5", L"C5", L"D5", L"E5", L"F5", L"G5", L"H5",
	L"A6", L"B6", L"C6", L"D6", L"E6", L"F6", L"G6", L"H6",
	L"A7", L"B7", L"C7", L"D7", L"E7", L"F7", L"G7", L"H7",
	L"A8", L"B8", L"C8", L"D8", L"E8", L"F8", L"G8", L"H8",
	L"なにもない",
	L"パス",
	L"戻す",
	L"無効な位置",
	L"けつばん"
};
// clang-format on

#endif