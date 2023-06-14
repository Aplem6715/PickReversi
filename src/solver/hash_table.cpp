#include "hash_table.h"
#include <cassert>
#include <random>

namespace solver
{
    constexpr uint64_t kRawHashMinTrueBits = 8;

    // RawHash[8行x2色][列内8石のパターン]
    uint64_t RawCode[8 * 2][1 << 8] = {0};

    void InitRawHash()
    {
        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_int_distribution<uint64_t> dis;

        for (int row = 0; row < 8 * 2; row++)
        {
            for (int b = 0; b < (1 << 8); b++)
            {
                do
                {
                    RawCode[row][b] = dis(gen);
                } while (CountBits(RawCode[row][b]) < kRawHashMinTrueBits);
            }
        }
    }

    uint64_t GetHashCode(const board::Stone& stones)
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

    HashTable::HashTable(uint64_t size)
    {
        assert(CountBits(size) == 1);
        size_  = size;
        mask_  = size - 1;
        pairs_ = new HashPair[size];

        if (RawCode[0][0] == 0)
        {
            InitRawHash();
        }

        PROFILE(prof_ = nullptr);
        Clear();
    }

    HashTable::~HashTable()
    {
        delete[] pairs_;
    }

    void HashTable::Clear()
    {
        for (int i = 0; i < size_; ++i)
        {
            pairs_[i].key_   = {0};
            pairs_[i].value_ = kInitHashData;
        }
        version_ = 0;
        PROFILE(if (prof_) { *prof_ = bench::kHashProfInit; });
    }

    void HashTable::ClearScore()
    {
        for (int i = 0; i < size_; ++i)
        {
            auto& hashData  = pairs_[i].value_;
            hashData.lower_ = static_cast<score8_t>(kEvalMin);
            hashData.upper_ = static_cast<score8_t>(kEvalMax);
        }
    }

    void HashTable::VersionUp()
    {
        if (version_ == UINT8_MAX)
            Clear();
        ++version_;
    }

    bool HashTable::TryGetValue(const board::Stone& stones, uint64_t hashCode, HashData* data)
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

    void HashTable::Add(const board::Stone& stones, uint64_t hashCode, score_t upper, score_t lower, score_t score, Position move, uint8_t cost, uint8_t depth)
    {
        uint64_t index = hashCode & mask_;

        // 1st try
        HashPair* pair1 = &pairs_[index];
        if (pair1->key_ == stones)
        {
            pair1->value_.Update(upper, lower, score, move, version_, cost, depth);
            return;
        }

        // 2nd try
        HashPair* pair2 = &pairs_[ReHash(index)];
        if (pair2->key_ == stones)
        {
            pair2->value_.Update(upper, lower, score, move, version_, cost, depth);
            return;
        }

        // fail 新しく登録（優先度の低い方に上書き）
        HashPair* rewriteTarget = (pair1->value_.GetPriority() <= pair2->value_.GetPriority()) ? pair1 : pair2;

        PROFILE(
            if (rewriteTarget->value_.depth_ == 0) {
                ++prof_->used;
            } else {
                ++prof_->collide;
            } //
        )

        rewriteTarget->key_ = stones;
        rewriteTarget->value_.Rewrite(upper, lower, score, move, version_, cost, depth);
    }

    PROFILE(
        void HashTable::BindProf(bench::HashProfile* prof) {
            prof_  = prof;
            *prof_ = bench::kHashProfInit;
        } //
    )

}