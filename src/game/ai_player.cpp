#include "ai_player.h"

#include "game/logger.h"
#include "solver/eval/pattern_eval.h"
#include "solver/eval/pos_eval.h"
#include "solver/search.h"
#include "solver/search_result.h"

namespace game
{
    template class AIPlayer<eval::PositionEval>;
    template class AIPlayer<eval::PatternEval>;

    template <class Evaluator>
    AIPlayer<Evaluator>::AIPlayer()
    {
        searcher_ = new solver::Searcher<Evaluator>();
    }

    template <class Evaluator>
    AIPlayer<Evaluator>::~AIPlayer()
    {
        delete searcher_;
    }

    template <class Evaluator>
    Position AIPlayer<Evaluator>::Think(uint64_t own, uint64_t opp)
    {
        solver::SearchResult result;
        searcher_->Search(own, opp, &result);

        auto best = result.GetBestMove();
        logger_->Info("score: " + std::to_string(best->value_));
        return best->pos_;
    }
}