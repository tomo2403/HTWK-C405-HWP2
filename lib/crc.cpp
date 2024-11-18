#include "crc.hpp"

CRC::CRC(uint32_t polynomial, uint32_t initialValue)
{
	this->polynomial = polynomial;
	this->initialValue = initialValue;
}

uint32_t CRC::calculateCRC(const myVector8 &data) const
{
	uint32_t crc = initialValue;

	for (uint8_t byte: data)
	{
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

bool CRC::validateCRC(const myVector8 &data, uint32_t receivedCRC) const
{
	uint32_t calculatedCRC = calculateCRC(data);
	return (calculatedCRC == receivedCRC);
}