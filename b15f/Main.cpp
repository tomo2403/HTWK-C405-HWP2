#include <b15f/b15f.h>

#include "../lib/header/Encoder.hpp"
#include "../lib/header/Decoder.hpp"
#include "../lib/header/CRC.hpp"

std::vector<uint8_t> getBinaryInput()
{
	std::cerr << "[DEBUG] Getting input data..." << std::endl;
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
	std::cerr << "[INFO ] Input data processed!" << std::endl;
	return data;
}

void setBinaryOutput(const std::vector<uint8_t> &data)
{
	std::cerr << "[DEBUG] Writing output..." << std::endl;
	std::cout.write(reinterpret_cast<const char *>(data.data()), data.size());
	std::cerr << "[INFO ] Output written!" << std::endl;
}

/*
std::vector<uint8_t> extractSubvector(const std::vector<uint8_t>& original, size_t start, size_t end) {
    // Überprüfen, ob die Indizes gültig sind
    if (start > end || end >= original.size()) {
        throw std::out_of_range("Ungültige Indizes für den Teilvektor.");
    }

    // Erstellen des Teilvektors mit der entsprechenden Größe
    std::vector<uint8_t> subvector(original.begin() + start, original.begin() + end + 1);
    return subvector;
}


std::vector<uint8_t> insertCounter(std::vector<uint8_t> vector, const uint32_t counter) 
{
    uint8_t valuesToAdd[3] = {0};
    valuesToAdd[0] = static_cast<uint8_t>((counter >> 16) & 0x0F); 
    valuesToAdd[1] = static_cast<uint8_t>((counter >> 8) & 0x0F);
    valuesToAdd[2] = static_cast<uint8_t>((counter >> 0) & 0x0F);

    // Füge die neuen Werte am Anfang des Vektors ein
    vector.insert(vector.begin(), valuesToAdd, valuesToAdd + 3);

    // Rückgabe des modifizierten Vektors
    return vector;
}

std::vector<uint8_t> insertCRC(std::vector<uint8_t> vector, const uint32_t crc) 
{
    uint8_t valuesToAdd[4] = {0};
    valuesToAdd[0] = static_cast<uint8_t>((crc >> 24) & 0x0F); 
    valuesToAdd[1] = static_cast<uint8_t>((crc >> 16) & 0x0F);
    valuesToAdd[2] = static_cast<uint8_t>((crc >> 8) & 0x0F);
    valuesToAdd[3] = static_cast<uint8_t>((crc >> 0) & 0x0F);

    // Füge die neuen Werte am Anfang des Vektors ein
    vector.insert(vector.end(), valuesToAdd, valuesToAdd + 4);

    // Rückgabe des modifizierten Vektors
    return vector;
}
*/

std::vector<uint8_t> makePackage(const std::vector<uint8_t>& original, const uint32_t startIndex, const uint32_t endIndex, const uint32_t counter)
{
    if (startIndex > endIndex || endIndex >= original.size()) {
        throw std::out_of_range("Ungültige Indizes für den Teilvektor.");
    }

    std::vector<uint8_t> package;
    package.reserve(endIndex - startIndex + 8);

    // Counter anfügen
    package.push_back(static_cast<uint8_t>((counter >> 16) & 0xFF));
    package.push_back(static_cast<uint8_t>((counter >> 8) & 0xFF));
    package.push_back(static_cast<uint8_t>((counter >> 0) & 0xFF));

    // Teilvektor aus dem originalen Vektor einfügen
    package.insert(package.end(), original.begin() + startIndex, original.begin() + endIndex + 1);

    // crc berechnen
    CRC crcCalculator = CRC(0x1234, 0xFFFF);
    uint32_t crc = crcCalculator.calculateCRC(package);

    // crc anfügen
    package.push_back(static_cast<uint8_t>((crc >> 24) & 0xFF));
    package.push_back(static_cast<uint8_t>((crc >> 16) & 0xFF));
    package.push_back(static_cast<uint8_t>((crc >> 8) & 0xFF));
    package.push_back(static_cast<uint8_t>((crc >> 0) & 0xFF));

    return package;
}

void outbound(const std::vector<uint8_t> &inputData)
{
    static uint32_t counter;
    static std::vector<uint8_t> package;

    static Encoder encoder = Encoder(makePackage(inputData, counter*64, counter*64+64, counter));
}

void inbound()
{
    static uint32_t counter = 0;
}

int main()
{
    B15F &drv = B15F::getInstance();

    std::vector<uint8_t> inputData = getBinaryInput();

    // Encoder encoder = Encoder();
    // Decoder decoder = Decoder();
    
    
    
    uint32_t dataPacketCounter = 0;

    while (true)
    {
        outbound(inputData);
        inbound();
    }
}