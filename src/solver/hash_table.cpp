#include "hash_table.h"
#include <cassert>

namespace solver
{
    HashTable::HashTable(uint64_t size)
    {
        assert(popcnt(size) == 1);
        size_  = size;
        mask_  = size - 1;
        pairs_ = new HashPair[size];
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
            pairs_->key_   = {0};
            pairs_->value_ = kInitHashData;
        }
        version_ = 0;
        PROFILE(*prof_ = bench::kHashProfInit);
    }

    void HashTable::VersionUp()
    {
        if(version_ == UINT8_MAX) Clear();
        ++version_;
    }

    bool HashTable::TryGetValue(const board::Stone& stones, uint64_t hashCode, HashData* data)
    {
        uint64_t index = hashCode & mask_;
        auto pair      = &pairs_[index];

        if (pair->key_ == stones)
        {
            PROFILE(++prof_->hit);
            *data = pair->value_;
            data->usedVer_ = version_;
            return true;
        }

        index = ReHash(hashCode) & mask_;
        pair  = &pairs_[index];
        if (pair->key_ == stones)
        {
            PROFILE(++prof_->hit);
            *data = pair->value_;
            data->usedVer_ = version_;
            return true;
        }

        return false;
    }

    void HashTable::Add(const board::Stone& stones, uint64_t hashCode, score32_t upper, score32_t lower, score32_t score, Position move, uint8_t cost, uint8_t depth)
    {
        uint64_t index = hashCode & mask_;

        // 1st try
        HashPair* pair1 = &pairs_[index];
        if (pair1->key_ == stones)
        {
            PROFILE(++prof_->hit);
            pair1->value_.Update(upper, lower, score, move, version_, cost, depth);
            return;
        }

        // 2nd try
        HashPair* pair2 = &pairs_[ReHash(index)];
        if (pair2->key_ == stones)
        {
            PROFILE(++prof_->hit);
            pair2->value_.Update(upper, lower, score, move, version_, cost, depth);
            return;
        }

        // fail 新しく登録（優先度の低い方に上書き）
        HashPair* rewriteTarget = (pair1->value_.GetPriority() < pair2->value_.GetPriority()) ? pair1 : pair2;

        rewriteTarget->key_ = stones;
        rewriteTarget->value_.Rewrite(upper, lower, score, move, version_, cost, depth);

        PROFILE(
            if (rewriteTarget->value_.depth_ == 0) {
                ++prof_->used;
            } else {
                ++prof_->collide;
            } //
        )
    }
}