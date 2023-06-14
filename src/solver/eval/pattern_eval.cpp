#include "pattern_eval.h"

namespace eval
{

    PatternEval::PatternEval()
    {
        // weightのメモリ確保．
        // キャッシュに乗るようにまとめて確保する
        // (本体は [weight_[0][0] = ...] の行)
        weight_ = AllocPatternWeight<short>();
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
}