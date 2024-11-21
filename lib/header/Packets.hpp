#pragma once

#ifdef ARDUINO

#include <Arduino.h>
#include <Vector.h>

typedef Vector<uint8_t> vector8;
#else

#include <cstdint>
#include <vector>

typedef std::vector<uint8_t> vector8;
#endif

struct PrePacket
{
	uint8_t index;
	uint32_t crc;
	vector8 data;
};

struct StreamPacket
{
	uint8_t channel;
	uint8_t dataLength;
	vector8 data;
};
