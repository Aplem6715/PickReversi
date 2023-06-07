#include "record.h"
#include "util/position_helper.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

namespace record
{
    void MatchBook::Read(std::string path)
    {
        std::ifstream ifs(path, std::ios::binary);
        if (!ifs)
        {
            throw std::runtime_error("Failed to open file for reading: " + path);
        }

        int numBytes;
        ifs.read(reinterpret_cast<char*>(&numBytes), sizeof(int));

        records_.resize(numBytes / sizeof(MatchRecord));
        ifs.read(reinterpret_cast<char*>(records_.data()), numBytes);
    }

    void MatchBook::Write(std::string path)
    {
        std::ofstream ofs(path, std::ios::binary);
        if (!ofs)
        {
            throw std::runtime_error("Failed to open file for writing: " + path);
        }

        int numBytes = static_cast<int>(records_.size()) * sizeof(MatchRecord);
        ofs.write(reinterpret_cast<const char*>(&numBytes), sizeof(int));
        ofs.write(reinterpret_cast<const char*>(records_.data()), numBytes);
    }

    void MatchBook::ReadAscii(std::string path)
    {
        std::ifstream infile(path);
        std::string line;

        if (infile.fail())
        {
            fprintf(stderr, "Failed to open file.");
            return;
        }

        while (std::getline(infile, line))
        {
            std::stringstream ss(line);
            MatchRecord record;
            std::string moveStr;

            ss >> moveStr;
            {
                record.nRandMoves_ = PositionHelper::PositionsFromAscii(moveStr, record.moves_);
                record.nMoves_ += record.nRandMoves_;
            }
            ss >> moveStr;
            {
                record.nMoves_ += PositionHelper::PositionsFromAscii(moveStr, &record.moves_[record.nRandMoves_]);
            }
            ss >> record.finalDiff_;
            records_.push_back(record);
        }
        infile.close();
    }

    void MatchBook::WriteAscii(std::string path)
    {
        std::ofstream outfile(path);

        if (outfile.fail())
        {
            fprintf(stderr, "Failed to open file.");
            return;
        }

        for (const auto& record : records_)
        {
            std::wstringstream ss;

            // moves_を文字列に変換する
            for (int i = 0; i < record.nMoves_; i++)
            {
                if (i == record.nRandMoves_)
                {
                    ss << ' ';
                }
                ss << PositionHelper::ToString(record.moves_[i]);
            }

            ss << " " << record.finalDiff_ << "\n";

            std::wstring ws = ss.str();
            outfile << std::string(ws.begin(), ws.end());
        }
        outfile.close();
    }
}
