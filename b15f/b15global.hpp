#pragma once
#include "B15Fake.hpp"

// #define DEBUG_MODE

#ifdef DEBUG_MODE
    extern volatile uint8_t DDRA;
    extern volatile uint8_t PORTA;
#endif