#include "game/record.h"

int main()
{
    game::MatchBook book;
    book.ReadAscii("resource/bench_case.txt");
    book.WriteAscii("resource/bench_out.txt");
    return 0;
}