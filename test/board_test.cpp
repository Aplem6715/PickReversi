#include "../src/board/board.h"
#include "gtest/gtest.h"

constexpr uint64_t INIT_BLACK = 0x0000000810000000;
constexpr uint64_t INIT_WHITE = 0x0000001008000000;

using namespace board;

TEST(Board, Put)
{
    auto board = new Board();
    board->Put(Position::C4);
    // board->Print();

    uint64_t flip         = 1 << static_cast<int>(Position::D4);
    uint64_t flippedBlack = INIT_BLACK ^ flip | 1 << static_cast<int>(Position::C4);
    uint64_t flippedWhite = INIT_WHITE ^ flip;
    ASSERT_EQ(board->GetStone(Color::Black), flippedBlack);
    ASSERT_EQ(board->GetStone(Color::White), flippedWhite);
}

TEST(Board, Undo)
{
    auto board = new Board();
    board->Put(Position::C4);
    board->Put(Position::C3);
    board->Put(Position::D3);
    board->Put(Position::E3);
    board->Put(Position::F4);
    board->Put(Position::F5);
    board->Put(Position::D6);
    board->Put(Position::E6);
    // board->Print();

    board->Undo(); // C4
    board->Undo(); // C3
    board->Undo(); // D3
    board->Undo(); // E3
    board->Undo(); // F4
    board->Undo(); // F5
    board->Undo(); // D6
    board->Undo(); // E6
    // board->Print();

    // これ以上戻せない
    ASSERT_FALSE(board->Undo());

    ASSERT_EQ(board->GetStone(Color::Black), INIT_BLACK);
    ASSERT_EQ(board->GetStone(Color::White), INIT_WHITE);
}

TEST(Board, SkipBlock)
{
    auto board = new Board();
    board->Put(Position::C4);

    ASSERT_FALSE(board->Skip());
    ASSERT_FALSE(board->Skip());
}

TEST(Board, StoneCount)
{
    auto board = new Board();
    board->Put(Position::C4);
    board->Put(Position::C3);
    board->Put(Position::D3);
    board->Put(Position::E3);
    board->Put(Position::F4);
    board->Put(Position::F5);
    board->Put(Position::D6);
    board->Put(Position::E6);
    board->Put(Position::C2);
    board->Put(Position::B1);
    board->Put(Position::C1);
    board->Put(Position::B2);
    board->Put(Position::A1);
    // board->Print();

    //   A B C D E F G H
    // 1 X X X - - - - - 1
    // 2 - O X . - - - - 2
    // 3 - . O O O . . - 3
    // 4 - . X O O X . - 4
    // 5 - . . X O O . - 5
    // 6 - - . X O - - - 6
    // 7 - - . . - - - - 7
    // 8 - - - - - - - - 8
    //   A B C D E F G H

    ASSERT_EQ(board->StoneCount(Color::Black), 8);
    ASSERT_EQ(board->StoneCount(Color::White), 9);
}
