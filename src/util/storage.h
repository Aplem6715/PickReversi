#ifndef STORAGE_H
#define STORAGE_H

#include <array>
#include <cassert>
#include "util/stack.h"

template <typename T, size_t Size>
class Storage
{
public:
    Storage();

    T* Store(T&& item);
    bool Remove(const T* item);
    bool Remove(size_t index);

private:
    std::array<T, Size> storage_;
    Stack<size_t> openIndices_;
};

template <typename T, size_t Size>
inline Storage<T, Size>::Storage()
{
    // スタックの一番上にインデックスの0が来るよう逆順に登録
    openIndices_.reserve(Size);
    for (int i = Size - 1; i >= 0; --i)
    {
        openIndices_.push(i);
    }
}

template <typename T, size_t Size>
inline T* Storage<T, Size>::Store(T&& item)
{
    assert(openIndices_.size() > 0);
    auto index = openIndices_.top();
    openIndices_.pop();
    storage_[index] = item;
    return storage_.data() + index;
}

template <typename T, size_t Size>
inline bool Storage<T, Size>::Remove(const T* item)
{
    return Remove(item - storage_.data());
}

template <typename T, size_t Size>
inline bool Storage<T, Size>::Remove(size_t index)
{
    openIndices_.push(index);
    return true;
}

#endif
