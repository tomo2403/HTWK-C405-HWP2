#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>

class AtomicBoolean {
private:
    std::atomic<bool> value;

public:
    AtomicBoolean() : value(false) {}

    void set(bool newValue) {
        value.store(newValue, std::memory_order_relaxed);
    }

    bool get() const {
        return value.load(std::memory_order_relaxed);
    }
};