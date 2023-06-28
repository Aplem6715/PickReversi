#ifndef TRAIN_CONST_H
#define TRAIN_CONST_H

#if TRAIN_BUILD

namespace train
{
    // kSmoothRange個隣り合った開きマスのフェーズにも同じ学習データを追加する
    // スムージング・データのかさ増し（隣の開きマス数が同じフェーズ所属なら無視）
    constexpr int kSmoothRange = 2;

    constexpr int kBatchSize = 4096;
    constexpr int kIteration = 50;
    // データロード後，バッファがこのサイズ以上溜まっていたら学習・消化
    constexpr int kReplayBufferSize = kBatchSize * kIteration;
    // 同じデータを何回使い回すか
    constexpr int kEpoch = 5;

    // Adamハイパーパラメーター
    constexpr double kAdamBeta1 = 0.9;
    constexpr double kAdamBeta2 = 0.999;
    constexpr double kAdamAlpha = 0.0001;
    constexpr double kAdamEps   = 1e-7;
}

#endif // namespace train
#endif // TRAIN_BUILD
