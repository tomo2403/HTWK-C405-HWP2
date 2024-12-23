#pragma once

#include <atomic>

class AtomicBoolean {

private:
    std::atomic<bool> value;

public:
    AtomicBoolean();
    operator bool() const;
    AtomicBoolean& operator=(bool newValue);
};