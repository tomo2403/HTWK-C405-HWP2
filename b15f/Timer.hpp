# include "../lib/lib.hpp"
#include <chrono>

class Timer
{
    private:
        bool running = false;
        std::chrono::steady_clock::time_point startTime;
    
    public:
        void start();
        void stop();
        int elapsedSeconds();
};