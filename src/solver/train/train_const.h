#ifndef TRAIN_CONST_H
#define TRAIN_CONST_H

#if TRAIN_BUILD

namespace train
{
    // sizeof(TrainRecord) = 24[byte] * 1 << 24 => 約400[MB]
    constexpr int kStorageCapacity = 1 << 24;

    // kSmoothRange個隣り合った開きマスのフェーズにも同じ学習データを追加する
    // スムージング・データのかさ増し（隣の開きマス数が同じフェーズ所属なら無視）
    constexpr int kSmoothRange = 2;

    constexpr int kBatchSize = 128;
    // 1学習で何Batch使うか
    constexpr int kIteration = 100;
    // バッファがこのサイズ以上溜まったら学習・消化
    constexpr int kReplayBufferSize = kBatchSize * kIteration;
    // 同じデータを何回使い回すか
    constexpr int kEpoch = 10;

    // 学習・テストデータ比率（テストデータ側の割合を指定）
    constexpr double kTestRatio = 0.1;

    // Adamハイパーパラメーター
    constexpr double kAdamBeta1 = 0.9;
    constexpr double kAdamBeta2 = 0.999;
    constexpr double kAdamAlpha = 0.001;
    constexpr double kAdamEps   = 1e-12;
}

#endif // namespace train
#endif // TRAIN_BUILD
