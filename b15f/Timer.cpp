#include "Timer.hpp"

void Timer::start()
{
    startTime = std::chrono::steady_clock::now();
    running = true;
}

void Timer::stop() {
    running = false;
}

int Timer::elapsedSeconds() {
    if (!running) {
        return 0;
    }
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - startTime);
    return duration.count(); 
}