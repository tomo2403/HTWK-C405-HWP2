#include "../header/AtomicQueue.hpp"

template<typename T>
void AtomicQueue<T>::push(const T &item)
{
    std::lock_guard lock(mtx);
    queue.push(item);
    cv.notify_one();
}

template<typename T>
std::optional<T> AtomicQueue<T>::try_pop()
{
    std::lock_guard lock(mtx);
    if (queue.empty())
    {
        return std::nullopt;
    }
    T item = queue.front();
    queue.pop();
    return item;
}

template<typename T>
T AtomicQueue<T>::wait_and_pop()
{
    std::unique_lock lock(mtx);
    cv.wait(lock, [this]()
    {
        return !queue.empty();
    });
    T item = queue.front();
    queue.pop();
    return item;
}

template<typename T>
bool AtomicQueue<T>::empty() const
{
    std::lock_guard lock(mtx);
    return queue.empty();
}
