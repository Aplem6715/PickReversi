#include "record/record.h"

int main()
{
    record::MatchBook book;
    book.ReadAscii("resource/test.txt");
    book.WriteAscii("resource/test_out.txt");
    return 0;
}