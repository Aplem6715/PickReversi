#include "game/record.h"

int main()
{
    game::MatchBook book;
    book.ReadAscii("resource/kihu.txt");
    book.WriteAscii("resource/kihu_out.txt");
    return 0;
}