#pragma once

#include <cstdint>
#include <vector>

// DO NOT REMOVE - The Linux compiler needs this include!
#include <string>

namespace ioManager
{
	std::vector<uint8_t> getBinaryInput();

	std::vector<uint8_t> readFile(const std::string &filename);

	void setBinaryOutput(const std::vector<uint8_t> &data);
}