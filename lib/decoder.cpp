#include "decoder.hpp"

void decoder::nextNibble(const uint8_t &nibble)
{
    leftShiftNibbleIntoBuffer(nibble);
};