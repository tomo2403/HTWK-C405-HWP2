#include "CRC.hpp"

CRC::CRC(size_t dataLength, uint32_t polynomial, uint32_t initialValue)
	: polynomial(polynomial), initialValue(initialValue), dataLength(dataLength)
{
}

uint32_t CRC::calculateCRC(const uint8_t data[]) const
{
	uint32_t crc = initialValue;

	for (size_t j = 0; j < dataLength; j++)
	{
		uint8_t byte = data[j];
		for (uint8_t i = 0; i < 8; i++)
		{
			uint8_t currentBit = (byte >> (7 - i)) & 1;
			if (((crc >> 31) & 1) != currentBit)
			{
				crc = (crc << 1) ^ polynomial;
			}
			else
			{
				crc <<= 1;
			}
		}
	}

	return crc;
}

bool CRC::validateCRC(const uint8_t data[], uint32_t receivedCRC) const
{
	uint32_t calculatedCRC = calculateCRC(data);
	return (calculatedCRC == receivedCRC);
}