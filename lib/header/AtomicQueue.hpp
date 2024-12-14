#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>

template<typename T>
class AtomicQueue
{
private:
    std::queue<T> queue;
    mutable std::mutex mtx;
    std::condition_variable cv;

public:
    void push(const T &item);
    std::optional<T> try_pop();
    T wait_and_pop();
    bool empty() const;
};
