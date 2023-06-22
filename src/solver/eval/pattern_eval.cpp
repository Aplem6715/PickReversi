#include "pattern_eval.h"
#include "board/bit.h"
#include <fstream>

namespace eval
{

    PatternEval::PatternEval()
    {
        // weightのメモリ確保．
        // キャッシュに乗るようにまとめて確保する
        // (本体は [weight_[0][0] = ...] の行)
        weight_ = AllocPatternWeight<int16_t>();
    }

    PatternEval::PatternEval(const std::string& path) : PatternEval()
    {
        // TODO: implement
    }

    PatternEval::~PatternEval()
    {
        FreePatternWeight(weight_);
        weight_ = nullptr;
    }

    void PatternEval::Reload(stone_t own, stone_t opp)
    {
        ResetState();

        Position pos;
        for_bits(pos, own)
        {
            PutOwn(pos);
        }

        for_bits(pos, opp)
        {
            PutOpp(pos);
        }

        side_ = Side::Own;
    }

    void PatternEval::ResetState()
    {
        for (int p = 0; p < kPatternNum; ++p)
        {
            state_[p] = kPatternOffset[p];
        }
    }

    bool PatternEval::Save(const std::string& path)
    {
        std::ofstream f(path, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
        if (f.fail())
        {
            return false;
        }
        f.write(reinterpret_cast<const char*>(weight_[0][0]), 2 * kNumPhase * kNumWeight * sizeof(int16_t));
        return true;
    }

    bool PatternEval::Load(const std::string& path)
    {
        std::ifstream f(path, std::ios_base::in | std::ios_base::binary);
        if (f.fail())
        {
            return false;
        }
        f.read(reinterpret_cast<char*>(weight_[0][0]), 2 * kNumPhase * kNumWeight * sizeof(int16_t));
        return true;
    }
}