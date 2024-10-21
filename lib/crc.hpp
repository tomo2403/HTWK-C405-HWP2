#pragma once

#include <cstdint>
#include <vector>

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