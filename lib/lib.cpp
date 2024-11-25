#include "header/Logger.hpp"
#include "header/CRC.hpp"

namespace ioManager
{
	std::vector <uint8_t> getBinaryInput()
	{
		Logger(DEBUG) << "Reading input data...";
		std::vector <uint8_t> data;
		char ch;
		while (std::cin.get(ch))
		{
			data.push_back(static_cast<uint8_t>(ch));
		}
		if (std::cin.eof())
		{
			std::cin.clear();
		}
		Logger(INFO) << "Input data processed!";
		return data;
	}

	void setBinaryOutput(const std::vector <uint8_t> &data)
	{
		Logger(DEBUG) << "Writing output...";
		std::cout.write(reinterpret_cast<const char *>(data.data()), data.size());
		Logger(INFO) << "Output written!";
	}
}
