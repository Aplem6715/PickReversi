#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "hash.h"

#if ENABLE_PROFILE
#include "bench/bench_result.h"
#endif

namespace solver
{
    class HashTable
    {
    public:
        HashTable(uint64_t size);
        ~HashTable();

        void Clear();
        void VersionUp();
        bool TryGetValue(const board::Stone& stones, uint64_t hashCode, HashData* data);
        void Add(const board::Stone& stones,
                 uint64_t hashCode,
                 score32_t upper,
                 score32_t lower,
                 score32_t score,
                 Position move,
                 uint8_t cost,
                 uint8_t depth);

    private:
        uint64_t size_;
        uint64_t mask_;
        HashPair* pairs_;
        uint8_t version_;

        PROFILE(bench::HashProfile* prof_);

        /// データ位置再探索（偶奇反転で隣の要素を見る。近いのでキャッシュに乗る）
        uint64_t ReHash(uint64_t hashCode) { return hashCode ^ 1; }
    };
} // namespace solver

#endif