#include "../header/Codec.hpp"
#include <iostream>
#include <bitset>

Codec::Codec()
{ }

bool Codec::hasNegatedNibbles(const uint8_t &byte)
{
	uint8_t highNibble = byte >> 4;
	uint8_t negatedLowNibble = ~byte & 0x0F;

	return highNibble == negatedLowNibble;
}

bool Codec::hasEqualNibbles(const uint8_t &byte)
{
	uint8_t highNibble = byte >> 4;
	uint8_t lowNibble = byte & 0x0F;

	return highNibble == lowNibble;
}

uint8_t Codec::negateLowNibble(const uint8_t &byte)
{
	uint8_t highNibble = byte | 0x0F;
	uint8_t lowNibble = byte & 0x0F;

	return highNibble & ~lowNibble;
}

void Codec::leftShiftByteIntoBuffer(const uint8_t &byte)
{
	buffer <<= 8;
	buffer |= byte;
}

void Codec::leftShiftNibbleIntoBuffer(const uint8_t &byte)
{
	buffer <<= 4;
	buffer |= (byte & 0x0F);
}

void Codec::zeroBuffer()
{
	buffer = 0x00000000;
}

uint8_t Codec::getByteSlice(const uint8_t &startBit) const
{
	if (startBit < 0 || startBit > 24)
	{
		throw std::invalid_argument("Start must define an 8-bit slice within the buffer.");
	}

	return static_cast<uint8_t>((buffer >> startBit) & 0x000000FF);
}

uint8_t Codec::getNibbleSlice(const uint8_t &startBit) const
{
	if (startBit < 0 || startBit > 28)
	{
		throw std::invalid_argument("Start must define an 4-bit slice within the buffer.");
	}

	return static_cast<uint8_t>((buffer >> startBit) & 0x0000000F);
}

bool Codec::areNegated(const uint8_t &nibbleOne, const uint8_t &nibbleTwo)
{
	return ((~nibbleOne) & 0x0F) == nibbleTwo;
}

void Codec::negateNibbleInBuffer(const uint8_t &startBit)
{

	if (startBit > 28)
	{
		throw std::out_of_range("Start bit cannot be higher than 28.");
	}

	const auto originalNibble = static_cast<uint8_t>((buffer >> startBit) & 0x0F);
	const uint8_t negatedNibble = ~originalNibble & 0x0F;

	const uint32_t mask = ~(0x0F << startBit);

	buffer &= mask;
	buffer |= (static_cast<uint32_t>(negatedNibble) << startBit);
}

void Codec::gracefullyInsertNibbleIntoBuffer(const uint8_t &nibble, const uint8_t &startBit)
{
	uint32_t bufferTmp = buffer;

	buffer >>= startBit-4;
	buffer &= 0xFFFFFFF0;
	buffer |= nibble & 0x0F;
	buffer <<= startBit;

	uint32_t mask = 0x00000000;
	for (int i = startBit-4; i >= 0; i = i-4)
	{
		mask |= (0x0F << i);
	}

	bufferTmp &= mask;
	buffer |= bufferTmp;
	bufferEndBit += 4;
}

uint8_t Codec::currentNibble()
{
	return getNibbleSlice(bufferEndBit - 3);
}
uint8_t Codec::currentByte()
{
	return getByteSlice(bufferEndBit - 7);
}