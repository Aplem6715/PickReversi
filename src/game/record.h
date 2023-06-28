#ifndef RECORD_H
#define RECORD_H

#include "const.h"
#include <vector>

namespace game
{
    struct MatchRecord
    {
        Position moves_[60] = {Position::NoMove};
        int nRandMoves_     = 0;
        int nMoves_         = 0;
        int finalDiff_      = 0;
    };

    class MatchBook
    {
    public:
        const std::vector<MatchRecord>& GetRecords() const { return records_; }
        std::vector<MatchRecord>& GetRecordEditable() { return records_; }

        void AddRecord(const MatchRecord& record);
        void Clear() { records_.clear(); }

        void Read(std::string path);
        void Write(std::string path);

        void ReadAscii(std::string path);
        void WriteAscii(std::string path);

    private:
        std::vector<MatchRecord> records_;
    };
}

#endif