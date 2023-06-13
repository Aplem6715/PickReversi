#include "ai_player.h"

#include "solver/search.h"
#include "solver/search_result.h"
#include "solver/eval/pos_eval.h"
#include "game/logger.h"
// #include <spdlog/spdlog.h>

namespace game
{
    AIPlayer::AIPlayer()
    {
        searcher_ = new solver::Searcher<eval::PositionEval>();
    }

    AIPlayer::~AIPlayer()
    {
        delete searcher_;
    }

    Position game::AIPlayer::Think(uint64_t own, uint64_t opp)
    {
        solver::SearchResult result;
        searcher_->Search(own, opp, &result);

        auto best = result.GetBestMove();
        logger_->Info("score: " + std::to_string(best->value_));
        return best->pos_;
    }
}