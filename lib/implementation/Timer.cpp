#include "../header/Timer.hpp"

Timer::Timer()
    : isRunning(false)
{
}

void Timer::start()
{
    if (!running())
    {
        start_time_point = std::chrono::high_resolution_clock::now();
        isRunning = true;
    }
}

void Timer::stop()
{
    if (running())
    {
        end_time_point = std::chrono::high_resolution_clock::now();
        isRunning = false;
    }
}

double Timer::elapsed()
{
    if (isRunning)
    {
        end_time_point = std::chrono::high_resolution_clock::now();
    }
    return std::chrono::duration<double>(end_time_point - start_time_point).count();
}

bool Timer::running() const
{
    return isRunning;
}
