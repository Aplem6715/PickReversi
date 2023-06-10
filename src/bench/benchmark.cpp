
#include "solver/search.h"
#include "solver/search_result.h"

#include "bench_result.h"
#include "board/board.h"
#include "game/record.h"

#include <iostream>

namespace bench
{
    constexpr int kNumPut  = 2;
    constexpr int kNumIter = 3;

    Position BenchOnePut(board::Board& board, std::ofstream& file)
    {
        solver::Searcher searcher[1];
        solver::SearchResult result[1];
        Position pos;

        for (int i = 0; i < kNumIter; i++)
        {
            std::cout << i;
            searcher->Reset();
            searcher->Search(board.GetOwn(), board.GetOpp(), result);

            pos = result->GetBestMove()->pos_;

            WriteBenchResult(pos, searcher->GetProfile(), file);
        }
        std::cout << '\n';
        return pos;
    }

    void BenchOneGame(const game::MatchRecord& record, std::ofstream& file)
    {
        board::Board board[1];

        board->Reset();
        for (int i = 0; i < record.nMoves_; ++i)
        {
            Position pos = record.moves_[i];
            board->Put(pos);
        }

        for (int i = 0; i < kNumPut; ++i)
        {
            auto pos = BenchOnePut(*board, file);
            board->Put(pos);
        }
        file << '\n';
    }

    void BenchMatches(const game::MatchBook& book, std::string resultFilePath)
    {
        std::ofstream file(resultFilePath);
        WriteCSVHeader(file);
        for (const auto& r : book.GetRecords())
        {
            BenchOneGame(r, file);
        }
    }
}

int main()
{
    game::MatchBook book;
    book.ReadAscii("resource/bench/bench_case.txt");
    bench::BenchMatches(book, "resource/bench/alpha_beta.csv");
    return 0;
}