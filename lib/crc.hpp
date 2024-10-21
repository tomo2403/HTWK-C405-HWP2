#pragma once

#ifndef ARDUINO
#include <Arduino.h>
#include <Vector.h>
typedef Vector myVector;
#else
#include <vector>
#include <cstdint>
typedef std::vector myVector;
#endif

class CRC
{
public:
	explicit CRC(uint32_t polynomial, uint32_t initialValue = 0x00000000);

	uint32_t calculateCRC(const std::vector<uint8_t>& data) const;

	bool validateCRC(const std::vector<uint8_t> &data, uint32_t receivedCRC) const;

private:
	uint32_t polynomial;
	uint32_t initialValue;
};
