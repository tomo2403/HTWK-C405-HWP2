#pragma once

#include <cstdint>
#include <vector>

namespace ioManager
{
	std::vector <uint8_t> getBinaryInput();

	void setBinaryOutput(const std::vector <uint8_t> &data);
}