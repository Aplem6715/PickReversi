
#include "board/board.h"
#include "game/record.h"
#include "solver/search.h"
#include "solver/search_option.h"
#include "solver/search_result.h"
#include "gtest/gtest.h"

// テストデータ生成用
// TEST(Search, MakeFixedStates)
// {
//     game::MatchBook book;
//     solver::Searcher searcher[1];
//     auto option    = solver::kAccuracyTestOption;
//     option.method_ = solver::SearchMethod::MinMax;
//     searcher->Setup(option);
//     book.ReadAscii("resource/test/search_test_source.txt");

//     int i = 0;
//     for (auto& rec : book.GetRecordEditable())
//     {
//         std::cout << "Progress: " << i << "\r";
//         ++i;
//         board::Board board[1];
//         board->Reset();
//         for (int i = 0; i < rec.nMoves_; ++i)
//         {
//             Position pos = rec.moves_[i];
//             board->Put(pos);
//         }

//         solver::SearchResult result;
//         searcher->Reset();
//         searcher->Search(board->GetOwn(), board->GetOpp(), &result);

//         auto bestMove           = result.GetBestMove();
//         rec.finalDiff_          = bestMove->value_;
//         rec.moves_[rec.nMoves_] = bestMove->pos_;
//         ++rec.nMoves_;
//     }

//     book.WriteAscii("resource/test/search_test.txt");
// }

TEST(Search, ReproducibilityCheck)
{
    game::MatchBook book;
    solver::Searcher searcher[1];
    searcher->Setup(solver::kAccuracyTestOption);
    book.ReadAscii("resource/test/search_test.txt");

    int i = 0;
    for (auto& rec : book.GetRecordEditable())
    {
        std::cout << "Progress: " << i << "\r";
        ++i;
        board::Board board[1];
        board->Reset();
        for (int i = 0; i < rec.nRandMoves_; ++i)
        {
            Position pos = rec.moves_[i];
            board->Put(pos);
        }
        Position basePos = rec.moves_[rec.nRandMoves_];

        solver::SearchResult result;
        searcher->Reset();
        searcher->Search(board->GetOwn(), board->GetOpp(), &result);
        auto bestMove = result.GetBestMove();

        // EXPECT_EQ(bestMove->pos_, basePos); // 探索順番によっては評価値が同じな違う手が選ばれることもある
        EXPECT_EQ(bestMove->value_, rec.finalDiff_) << "[Case]: " << i;
    }
}
