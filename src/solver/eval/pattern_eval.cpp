#include "pattern_eval.h"

namespace eval
{

    PatternEval::PatternEval()
    {
        // weightのメモリ確保．
        // キャッシュに乗るようにまとめて確保する
        // (本体は [weight_[0][0] = ...] の行)
        weight_ = new short**[2];

        weight_[0] = new short*[2 * kNumPhase];
        weight_[1] = weight_[0] + kNumPhase;

        weight_[0][0] = new short[2 * kNumPhase * kNumWeight];
        weight_[1][0] = weight_[0][0] + kNumPhase * kNumWeight;

        for (int phase = 1; phase < kNumPhase; ++phase)
        {
            weight_[0][phase] = weight_[0][phase - 1] + kNumWeight;
            weight_[1][phase] = weight_[1][phase - 1] + kNumWeight;
        }
    }

    PatternEval::PatternEval(const std::string& path) : PatternEval()
    {
        // TODO: implement
    }

    PatternEval::~PatternEval()
    {
        delete[] weight_[0][0];
        delete[] weight_[0];
        delete[] weight_;
        weight_ = nullptr;
    }
}