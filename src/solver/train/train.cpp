
#if TRAIN_BUILD

#include "../eval/pattern_eval.h"
#include "game/record.h"
#include "pattern_trainer.h"
#include "record_batcher.h"
#include "train_const.h"

namespace train
{
    constexpr auto kTrainDataDir = "resource/train";

    void Train()
    {
        PatternEval eval_;
        PatternTrainer trainer(&eval_);
        game::MatchBook book;
        std::vector<std::string> allFiles;
        ReplayBuffer buffer(kBatchSize, kReplayBufferSize);

        // TODO: kTrainDataDirからファイル一覧を取得してallFilesに入れる

        for (auto& file : allFiles)
        {
            book.ReadAscii(file);
            buffer.Load(book);

            for (int phase = 0; phase < kNumPhase; ++phase)
            {
                if (buffer.IsBufferFull(phase))
                {
                    trainer.Run(*buffer.GetBatchBuffer(phase), kTestRatio);
                    buffer.Clear(phase);
                }
            }

            book.Clear();
        }
    }
}

int main()
{
    train::Train();
    return 0;
}

#endif