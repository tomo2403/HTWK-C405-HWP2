#include "../header/Timer.hpp"

Timer::Timer()
    : isRunning(false)
{
}

void Timer::start()
{
    start_time_point = std::chrono::high_resolution_clock::now();
    isRunning = true;
}

void Timer::stop()
{
    end_time_point = std::chrono::high_resolution_clock::now();
    isRunning = false;
}

double Timer::elapsed()
{
    if (isRunning) {
        end_time_point = std::chrono::high_resolution_clock::now();
    }
    return std::chrono::duration<double>(end_time_point - start_time_point).count();
}

bool Timer::running()
{
    return isRunning;
}