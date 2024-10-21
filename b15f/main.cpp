#include <b15f/b15f.h>
#include <iostream>
#include <fstream>
#include <string>

#include "../lib/crc.hpp"

std::vector<uint8_t> toHalfBytes(std::ifstream *data)
{
	std::vector<uint8_t> halfBytes;
	char byte;

	while (data.get(byte))
	{
		uint8_t highNibble = (byte & 0xF0) >> 4; // obere 4 Bit
		uint8_t lowNibble = byte & 0x0F;         // untere 4 Bit

		halfBytes.push_back(highNibble);
		halfBytes.push_back(lowNibble);
	}

	return halfBytes;
}

int main(int argc, char *argv[])
{
	B15F &drv = B15F::getInstance();
	drv.setRegister(&DDRA, 0x0F);

	std::string path;

	// Prüfen, ob Argumente vorhanden sind
	if (argc > 1)
	{
		for (int i = 1; i < argc; ++i)
		{
			std::string arg = argv[i];

			// Überprüfen, ob -p oder --path verwendet wird
			if ((arg == "-p" || arg == "--path") && i + 1 < argc)
			{
				path = argv[i + 1];
				i++; // Überspringe den nächsten Eintrag, da er der Pfad ist
			}
		}
	}

	// Ausgabe des Ergebnisses
	if (!path.empty())
	{
		std::cout << "Pfad: " << path << std::endl;

		std::ifstream file(path, std::ios::binary);
		if (!file)
		{
			std::cerr << "Fehler beim Öffnen der Datei: " << path << std::endl;
			return 1;
		}

		std::vector<uint8_t> blocks = toHalfBytes(&file);
		file.close();
	}
	else
	{
		std::cout << "Kein Pfad angegeben." << std::endl;
	}

	drv.setRegister(&PORTA, 0x05);

//    while (true) {
//        drv.setRegister(&PORTA, 0x00);
//        cout << "0" << endl;
//        //drv.delay_ms(500);
//        drv.setRegister(&PORTA, 0xA0);
//        cout << "1" << endl;
//        //drv.delay_ms(500);
//    }

	return 0;
}
