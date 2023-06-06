#include "ai_player.h"

#include "solver/search.h"
#include "solver/search_result.h"
#include <spdlog/spdlog.h>

namespace game
{
    using namespace solver;

    AIPlayer::AIPlayer()
    {
        searcher_ = new Searcher();
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
        logger_->info("score: {}", best->value_);
        return best->pos_;
    }
}