#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "bench/bench_result.h"
#include "hash.h"

namespace solver
{
    static uint64_t RawCode[8 * 2][1 << 8] = {0};

    void InitRawHash();

    inline uint64_t GetHashCode(const board::Stone& stones)
    {
        uint64_t code;

        // 128bit -> 16x8bit
        const uint8_t* cursor = (const uint8_t*)(&stones);

        code = RawCode[0][cursor[0]];
        code ^= RawCode[1][cursor[1]];
        code ^= RawCode[2][cursor[2]];
        code ^= RawCode[3][cursor[3]];
        code ^= RawCode[4][cursor[4]];
        code ^= RawCode[5][cursor[5]];
        code ^= RawCode[6][cursor[6]];
        code ^= RawCode[7][cursor[7]];
        code ^= RawCode[8][cursor[8]];
        code ^= RawCode[9][cursor[9]];
        code ^= RawCode[10][cursor[10]];
        code ^= RawCode[11][cursor[11]];
        code ^= RawCode[12][cursor[12]];
        code ^= RawCode[13][cursor[13]];
        code ^= RawCode[14][cursor[14]];
        code ^= RawCode[15][cursor[15]];

        return code;
    }

    /// @brief データ位置再探索（偶奇反転で隣の要素を見る。近いのでキャッシュに乗る）
    /// 配列終端のインデックスは0xFFFFFのようになるので^1しても範囲外参照しない
    /// @param hashCode
    /// @return
    inline uint64_t ReHash(uint64_t hashCode) { return hashCode ^ 1; }

    class HashTable
    {
    public:
        HashTable(uint64_t size);
        ~HashTable();

        void Clear();
        void ClearScore();
        void VersionUp();
        void Add(const board::Stone& stones,
                        uint64_t hashCode,
                        score_t upper,
                        score_t lower,
                        score_t score,
                        Position move,
                        uint8_t cost,
                        uint8_t depth);

        // カット前に呼ばれるためinline化(Add関数はカットされるノードでは呼ばれないためそこまで気にしない)
        inline bool TryGetValue(const board::Stone& stones, uint64_t hashCode, HashData* data);

        PROFILE(void BindProf(bench::HashProfile* prof);)

    private:
        uint64_t size_;
        uint64_t mask_;
        HashPair* pairs_;
        uint8_t version_;

        PROFILE(bench::HashProfile* prof_);
    };

    inline bool HashTable::TryGetValue(const board::Stone& stones, uint64_t hashCode, HashData* data)
    {
        uint64_t index = hashCode & mask_;
        auto pair      = &pairs_[index];

        if (pair->key_ == stones)
        {
            PROFILE(++prof_->hit);
            *data          = pair->value_;
            data->usedVer_ = version_;
            return true;
        }

        pair = &pairs_[ReHash(index)];
        if (pair->key_ == stones)
        {
            PROFILE(++prof_->hit2nd);
            *data          = pair->value_;
            data->usedVer_ = version_;
            return true;
        }

        *data = kInitHashData;
        return false;
    }
} // namespace solver

#endif