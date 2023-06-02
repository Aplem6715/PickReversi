#include "stone.h"

void Stone::Swap()
{
    auto temp = own;
    own       = opp;
    opp       = temp;
}

void Stone::Update(uint64_t pos, uint64_t flips)
{
    this->own = this->opp ^ flips;
    this->opp = this->own ^ (flips | pos);
}

void Stone::Restore(uint64_t pos, uint64_t flips)
{
    this->own = this->opp ^ (flips | pos);
    this->opp = this->own ^ flips;
}
