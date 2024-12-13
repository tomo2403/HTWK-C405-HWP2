#include <algorithm>

#include "../header/Decoder.hpp"
#include "../header/CodecCommand.hpp"

Decoder::Decoder()
{
}

void Decoder::addObserver(IDecoderObserver *observer)
{
	observers.push_back(observer);
}

void Decoder::removeObserver(IDecoderObserver *observer)
{
	observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
}

void Decoder::nextNibble(const uint8_t &nibble)
{
}
