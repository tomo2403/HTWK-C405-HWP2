#include "lib.hpp"

#include <iostream>
#include <fstream>

#include "header/Logger.hpp"

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


	std::vector<uint8_t> readFile(const std::string& filename) {
		std::vector<uint8_t> buffer;

		std::ifstream file(filename, std::ios::binary);
		if (!file) {
			std::cerr << "Fehler beim Öffnen der Datei: " << filename << std::endl;
			return buffer;
		}

		file.seekg(0, std::ios::end);
		const size_t fileSize = file.tellg();
		file.seekg(0, std::ios::beg);

		buffer.resize(fileSize);
		file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
		file.close();
		return buffer;
	}
}
