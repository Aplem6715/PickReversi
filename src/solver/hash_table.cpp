#include "hash_table.h"
#include <cassert>
#include <random>

namespace solver
{
    constexpr uint64_t kRawHashMinTrueBits = 8;

    void InitRawHash()
    {
        std::mt19937_64 gen(kSeed);
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
        ++version_;
        if (version_ > UINT8_MAX )
            Clear();
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