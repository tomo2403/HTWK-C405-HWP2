#pragma once

#ifdef ARDUINO
#include <Arduino.h>
#include <Vector.h>
typedef Vector<uint8_t> myVector8;
#else
#include <vector>
#include <cstdint>
typedef std::vector myVector;
#endif

class CRC
{
public:
	explicit CRC(uint32_t polynomial, uint32_t initialValue = 0x00000000);

	uint32_t calculateCRC(const myVector8& data) const;

	bool validateCRC(const myVector8 &data, uint32_t receivedCRC) const;

private:
	uint32_t polynomial;
	uint32_t initialValue;
};
