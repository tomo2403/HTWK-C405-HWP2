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

enum PacketType
{
	Data = 0,
	Response = 1,
};

struct PrePacket
{
	uint8_t index;
	vector8 data;
	uint8_t crc;
};

struct StreamPacket
{
	uint8_t channel;
	PacketType type;
	uint8_t dataLength;
	vector8 data;
};
