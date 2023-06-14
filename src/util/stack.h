#ifndef STACK_H
#define STACK_H

#include <stack>
#include <vector>

template <typename T>
class Stack : protected std::stack<T, std::vector<T>>
{
public:
    using std::stack<T, std::vector<T>>::empty;
    using std::stack<T, std::vector<T>>::size;
    using std::stack<T, std::vector<T>>::top;
    using std::stack<T, std::vector<T>>::push;
    using std::stack<T, std::vector<T>>::pop;

    void reserve(std::size_t n) { this->c.reserve(n); }
};

#endif
