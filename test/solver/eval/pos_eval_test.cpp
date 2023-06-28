
#include "gtest/gtest.h"

#include "board/board.h"
#include "solver/eval/pos_eval.h"

constexpr stone_t INIT_BLACK = 0x0000000810000000;
constexpr stone_t INIT_WHITE = 0x0000001008000000;

namespace eval
{
    using namespace board;

    class PositionEvaluator_Test : public ::testing::Test
    {
    public:
        stone_t GetOwn() { return _eval._own; }
        stone_t GetOpp() { return _eval._opp; }

    protected:
        PositionEval _eval;
    };

    TEST_F(PositionEvaluator_Test, Update)
    {
        _eval.Reload(INIT_BLACK, INIT_WHITE);

        Position pos = Position::C4;
        stone_t posBit   = 1 << static_cast<int>(pos);
        stone_t flips = 1 << static_cast<int>(Position::D4);

        stone_t flippedBlack = INIT_BLACK ^ flips | posBit;
        stone_t flippedWhite = INIT_WHITE ^ flips;

        _eval.Update(pos, flips);
        EXPECT_EQ(GetOwn(), flippedWhite);
        EXPECT_EQ(GetOpp(), flippedBlack);

        _eval.Restore(pos, flips);
        EXPECT_EQ(GetOwn(), INIT_BLACK);
        EXPECT_EQ(GetOpp(), INIT_WHITE);
    }

    TEST_F(PositionEvaluator_Test, ValueCheck)
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

        _eval.Reload(board->GetOwn(), board->GetOpp());
        EXPECT_EQ(_eval.Evaluate(0), -33);
    }
}
