
#if TRAIN_BUILD

#include "../eval/pattern_eval.h"
#include "game/record.h"
#include "pattern_trainer.h"
#include "record_batcher.h"
#include "train_const.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

namespace train
{
    constexpr auto kTrainDataDir = "resource/train";
    constexpr auto kLogDir       = "log/log.txt";

    namespace fs = std::filesystem;

    void getFilePaths(const std::string& dir, std::vector<std::string>* filePaths)
    {
        for (const auto& entry : fs::directory_iterator(dir))
        {
            if (fs::is_regular_file(entry.path()))
            {
                filePaths->push_back(entry.path().string());
            }
        }
    }

    void Train()
    {
        PatternEval eval_;
        PatternTrainer trainer(&eval_);
        game::MatchBook book;
        std::vector<std::string> allFiles;
        std::array<int, kNumPhase> trainCount = {0};

        auto buffer = new ReplayBuffer(kBatchSize, kReplayBufferSize);

        getFilePaths(kTrainDataDir, &allFiles);

        std::ofstream logfile(kLogDir, std::ios::trunc); // 追記モードでファイルを開く
        for (auto& file : allFiles)
        {
            book.ReadAscii(file);
            buffer->Load(book);

            logfile << "Read Book: " << file << std::endl;
            for (int phase = 0; phase < kNumPhase; ++phase)
            {
                logfile << "phase " << phase << ": " << buffer->GetBatchBuffer(phase)->Size() << std::endl;
            }
            logfile << std::endl;

            // すべてのフェーズを確認して，バッファーがいっぱいになっていたら学習
            for (int phase = 0; phase < kNumPhase; ++phase)
            {
                if (buffer->IsBufferFull(phase))
                {
                    ++trainCount[phase];
                    logfile << "Phase " << phase << ": TrainCount = " << trainCount[phase] << std::endl;

                    for (int epoch = 0; epoch < kEpoch; ++epoch)
                    {
                        auto mae = trainer.Run(*buffer->GetBatchBuffer(phase), kTestRatio);
                        // Epochごとにログファイルにログを出力する
                        logfile << "\tEpoch " << epoch << ": MAE = " << mae << std::endl;
                    }
                    logfile << std::endl;

                    buffer->Clear(phase);
                }
            }
            logfile << std::endl;
            book.Clear();
        }

        logfile.close();
        delete buffer;
    }
}

int main()
{
    train::Train();
    return 0;
}

#endif