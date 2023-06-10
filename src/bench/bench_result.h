#ifndef BENCH_RESULT_H
#define BENCH_RESULT_H

#include "const.h"
#include "util/position_helper.h"
#include <array>
#include <chrono>
#include <fstream>
#include <string>

#if ENABLE_PROFILE
#define PROFILE(x) x
#else
#define PROFILE(x)
#endif

namespace bench
{
    struct HashProfile
    {
        uint32_t used;
        uint32_t hit;
        uint32_t rehash;
        uint32_t collide;
    };

    struct Profile
    {
        int depth;
        Position pos;
        score_t score;
        double duration;
        uint64_t nodeCount;
        uint64_t leafCount;

        HashProfile hash;
    };

    constexpr Profile kProfileInit = {0};

    inline void WriteCSVHeader(std::ofstream& file)
    {
        file << "Pos, Depth, Score, Duration, Node, NPS, Leaf, Used, Hit, Rehash, Collide" << std::endl;
    }

    inline void WriteBenchResult(Position pos, const Profile& bench_result, std::ofstream& file)
    {
        file << PositionHelper::ToString(pos) << ","
             << bench_result.depth << ","
             << bench_result.score << ","
             << bench_result.duration << ","
             << bench_result.nodeCount << ","
             << bench_result.nodeCount / bench_result.duration << ","
             << bench_result.leafCount << ","
             << bench_result.hash.used << ","
             << bench_result.hash.hit << ","
             << bench_result.hash.rehash << ","
             << bench_result.hash.collide << std::endl;
    }

}

#endif
