#include "crc.hpp"

CRC::CRC(uint32_t polynomial, uint32_t initialValue)
{
	this->polynomial = polynomial;
	this->initialValue = initialValue;
}

uint32_t CRC::calculateCRC(std::vector<uint8_t> data)
{
	uint32_t crc = initialValue;

	for (uint8_t i = 0; i < 4; i++)
	{
		data.push_back(0x0);
	}

	for (uint8_t byte: data)
	{
		for (uint8_t i = 0; i < 8; i++)
		{
			if (crc & 0x80000000)
			{
				crc <<= 1;
				uint8_t currentBit = (byte >> (7 - i)) & 0x1;
				crc |= currentBit;
				crc ^= polynomial;
			}
			else
			{
				crc <<= 1;
				uint8_t currentBit = (byte >> (7 - i)) & 0x1;
				crc |= currentBit;
			}
		}
	}

	return crc;
}

bool CRC::validateCRC(const std::vector<uint8_t> &data, uint32_t receivedCRC)
{
	uint32_t calculatedCRC = calculateCRC(data);
	return (calculatedCRC == receivedCRC);
}

/*
ChatGPT Lösung:

uint32_t CRC::calculateCRC(std::vector<uint8_t> data) 
{
    uint32_t crc = initialValue;

    // Entferne das Hinzufügen von 8 Nullen
    // for (uint8_t i = 0; i < 8; i++) {
    //     data.push_back(0x0);
    // }

    for (uint8_t byte : data)
    {
        crc ^= (byte << 24); // Setze die höchsten 8 Bits des CRC mit dem aktuellen Byte

        for (uint8_t i = 0; i < 8; i++)
        {
            if (crc & 0x80000000) // Überprüfe das höchste Bit
            {
                crc = (crc << 1) ^ polynomial; // Schiebe nach links und wende das Polynom an
            }
            else
            {
                crc <<= 1; // Schiebe nur nach links
            }
        }
    }

    return crc;
}
*/