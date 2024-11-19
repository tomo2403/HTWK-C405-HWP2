#include "Encoder.hpp"
#include "Decoder.hpp"
#include <bitset>

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>

int main() {
    // Dateinamen für Ein- und Ausgabe
    const std::string inputFileName = "../random_data.bin";
    const std::string outputFileName_Enc = "../output_Enc.bin";
    const std::string outputFileName_Dec = "../output_Dec.bin";

    // Vektor zum Speichern der Binärdaten
    std::vector<uint8_t> data;

    // Datei einlesen
    std::ifstream inputFile(inputFileName, std::ios::binary);
    if (!inputFile) {
        std::cerr << "Fehler beim Öffnen der Eingabedatei!" << std::endl;
        return 1;
    }

    // Daten in den Vektor einlesen
    inputFile.seekg(0, std::ios::end);
    size_t fileSize = inputFile.tellg();
    inputFile.seekg(0, std::ios::beg);
    data.resize(fileSize);
    inputFile.read(reinterpret_cast<char*>(data.data()), fileSize);
    inputFile.close();

    // Hier erfolgt die Bearbeitung des Vektors
    Encoder enc = Encoder(0x80, data);

    std::vector<uint8_t> encData = std::vector<uint8_t>();
    std::vector<uint8_t> decData = std::vector<uint8_t>();
    
    Decoder dec = Decoder(0x80, decData);
    
    uint8_t nextNibble;
    while (enc.hasData())
    {
        nextNibble = enc.nextNibble().value();
        encData.push_back(nextNibble);

        dec.nextNibble(nextNibble);
    }
    dec.flushBufferIntoDataVector();


    // Datei schreiben
    std::ofstream outputFile_one(outputFileName_Enc, std::ios::binary);
    if (!outputFile_one) {
        std::cerr << "Fehler beim Öffnen der Ausgabedatei!" << std::endl;
        return 1;
    }

    outputFile_one.write(reinterpret_cast<const char*>(encData.data()), encData.size());
    outputFile_one.close();

        // Datei schreiben
    std::ofstream outputFile_two(outputFileName_Dec, std::ios::binary);
    if (!outputFile_two) {
        std::cerr << "Fehler beim Öffnen der Ausgabedatei!" << std::endl;
        return 1;
    }

    outputFile_two.write(reinterpret_cast<const char*>(decData.data()), decData.size());
    outputFile_two.close();

    std::cout << "Datei erfolgreich verarbeitet und gespeichert!" << std::endl;
    return 0;
}

/*
int main(int, char**){
    
    std::vector<uint8_t> RawBytes = {0xcb, 0xc7, 0x87, 0x1e};
    // std::vector<uint8_t> RawBytes = {0x67, 0xde, 0x1d, 0x29};
    std::vector<uint8_t> decodedBytes = std::vector<uint8_t>();
    Encoder enc = Encoder(0x80, RawBytes);
    Decoder dec = Decoder(0x80, decodedBytes);

	std::cout << "Encoded:" << std::endl;
	while (enc.hasData())
	{
		uint8_t nextNibble = enc.nextNibble().value();
		std::cout << std::bitset<8>(nextNibble) << std::endl;
		dec.nextNibble(nextNibble);
	}

	std::cout << "Decoded:" << std::endl;
	dec.flushBufferIntoDataVector();
	for (uint8_t byte: decodedBytes)
	{
		std::cout << std::bitset<8>(byte) << std::endl;
	}
}
*/
