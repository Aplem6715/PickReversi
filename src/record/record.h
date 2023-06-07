#ifndef RECORD_H
#define RECORD_H

#include "const.h"
#include <vector>

namespace record
{
    struct MatchRecord
    {
        Position moves_[60];
        int nRandMoves_;
        int nMoves_{0};
        int finalDiff_;
    };

    class MatchBook
    {
    public:
        const std::vector<MatchRecord>& GetRecords() const { return records_; }

        void Read(std::string path);
        void Write(std::string path);

        void ReadAscii(std::string path);
        void WriteAscii(std::string path);

    private:
        std::vector<MatchRecord> records_;
    };
}

#endif