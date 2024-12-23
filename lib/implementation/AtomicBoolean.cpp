#include "../header/AtomicBoolean.hpp"

AtomicBoolean::AtomicBoolean() : value(false) {}

AtomicBoolean::operator bool() const {
    return value.load(std::memory_order_relaxed);
}

AtomicBoolean& AtomicBoolean::operator=(bool newValue) {
    value.store(newValue, std::memory_order_relaxed);
    return *this;
}
