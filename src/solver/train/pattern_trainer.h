#ifndef PATTERN_TRAINER_H
#define PATTERN_TRAINER_H

#include "../eval/pattern.h"
#include "../eval/pos2pattern.h"
#include <vector>

namespace eval
{
    class PatternEval;
}

namespace game
{
    struct MatchBook;
}

namespace train
{
    using namespace eval;

    class TrainRecord;
    class RecordBatcher;

    using Batch = std::vector<const TrainRecord*>;

    class PatternTrainer
    {
    public:
        PatternTrainer();
        ~PatternTrainer();

        void Train(const Batch& batchData);

    private:
        PatternEval* eval_;

        // 学習用weight
        double*** trainWeight_;
        double*** gradSum_;
        double*** moment_;
        double*** v_;
    };

}
#endif