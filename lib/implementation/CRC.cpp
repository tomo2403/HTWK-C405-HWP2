#include "../header/CRC.hpp"

uint32_t CRC::calculateCRC(const std::vector<uint8_t> &data)
{
	uint32_t crc = initialValue;

	for (const uint8_t byte: data)
	{
		for (uint8_t i = 0; i < 8; i++)
		{
			if (const uint8_t currentBit = (byte >> (7 - i)) & 1; ((crc >> 31) & 1) != currentBit)
			{
				crc = crc << 1 ^ polynomial;
			}
			else
			{
				crc <<= 1;
			}
		}
	}

	return crc;
}

void CRC::attachCRC(std::vector<uint8_t> &data)
{
	const uint32_t crcValue = calculateCRC(data);
	data.push_back(crcValue >> 24);
	data.push_back(crcValue >> 16 & 0xFF);
	data.push_back(crcValue >> 8 & 0xFF);
	data.push_back(crcValue & 0xFF);
}

bool CRC::validateCRC(const std::vector<uint8_t> &data, const uint32_t receivedCRC)
{
	const uint32_t calculatedCRC = calculateCRC(data);
	return calculatedCRC == receivedCRC;
}
