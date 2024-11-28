#include "lib.hpp"

namespace ioManager
{
	std::vector<uint8_t> getBinaryInput()
	{
		Logger(DEBUG) << "Reading input data...";
		std::vector<uint8_t> data;
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

	void setBinaryOutput(const std::vector<uint8_t> &data)
	{
		Logger(DEBUG) << "Writing output...";
		std::cout.write(reinterpret_cast<const char *>(data.data()), data.size());
		Logger(INFO) << "Output written!";
	}

	std::vector<uint8_t> extractSubvector(const std::vector<uint8_t>& vec, size_t start, size_t length)
	{
		if (start >= vec.size()) {
			return {};
		}
		
		if (start + length > vec.size()) {
			length = vec.size() - start;
		}

		return std::vector<uint8_t>(vec.begin() + start, vec.begin() + start + length);
	}
}
