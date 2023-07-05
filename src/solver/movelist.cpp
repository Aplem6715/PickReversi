#include "movelist.h"
#include "eval/pattern_eval.h"
#include "eval/pos_eval.h"
#include "hash.h"
#include "search.h"

namespace solver
{
    template void Move::Evaluate<eval::PositionEval>(Searcher<eval::PositionEval>& searcher, const HashData& hashData);
    template void Move::Evaluate<eval::PatternEval>(Searcher<eval::PatternEval>& searcher, const HashData& hashData);

    template <class Evaluator>
    void Move::Evaluate(Searcher<Evaluator>& searcher, const HashData& hashData)
    {
        constexpr int kWipeoutOrder      = 30;
        constexpr int kBestMoveOrder     = 29;
        constexpr int kSecondMoveOrder   = 28;
        constexpr int kMobilityOrder     = 16;
        constexpr int kOneStepScoreOrder = 8;
        constexpr int kPosScoreOrder     = 0;

        const board::Stone& stone = searcher.GetStone();

        // 全消し
        if (flips_ == stone.opp_)
        {
            value_ = (1 << kWipeoutOrder);
            return;
        }

        // 最善手
        if (hashData.bestMove_ != Position::NoMove)
        {
            value_ = (1 << kBestMoveOrder);
            return;
        }

        if (hashData.secondMove_ != Position::NoMove)
        {
            value_ = (1 << kSecondMoveOrder);
            return;
        }
        else
        {
            const Position pos    = pos_;
            const uint64_t posBit = PosToBit(pos);
            const uint64_t flips  = flips_;

            board::Stone nextStone = stone;
            nextStone.Update(posBit, flips);

            int value;

            // 着手可能数での評価=速度優先探索（24~16bit）
            {
                const uint64_t nextMob = nextStone.CalcMobility();
                // 正の値にするためのバイアス
                int mobCount = kMaxMove + 4 /*角ボーナス分*/;
                mobCount -= CountBits(nextMob);
                mobCount -= CountBits(nextMob & 0x8100000000000081); // 角ボーナス
                // 相手の手が多い＝マイナス
                value = mobCount * (1 << kMobilityOrder);
            }

            // 一手読みのスコア付け（16~8bit目)
            {
                auto& eval = searcher.GetEval();
                // 着手して相手のターンに進める
                eval.Update(pos, flips);
                const score_t score = eval.Evaluate(Phase(searcher.GetNumEmpty() - 1));
                eval.Restore(pos, flips);
                value -= score * (1 << kOneStepScoreOrder);
            }

            // 着手位置でスコア付け(8~0bit)
            {
                value += eval::ValueTable[static_cast<int>(pos)] * (1 << kPosScoreOrder);
            }

            value_ = value;
        }
    }
}
