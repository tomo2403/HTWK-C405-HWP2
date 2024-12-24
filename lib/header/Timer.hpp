# pragma once

#include <chrono>

class Timer
{
    bool isRunning;
    std::chrono::high_resolution_clock::time_point start_time_point;
    std::chrono::high_resolution_clock::time_point end_time_point;

public:

    Timer();

    void start();

    void stop();

    // unit: seconds
    double elapsed();

    [[nodiscard]] bool running() const;
};