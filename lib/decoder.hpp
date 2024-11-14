#include "codec.hpp"

class decoder : public codec
{
    void nextNibble(const uint8_t &nibble);
};