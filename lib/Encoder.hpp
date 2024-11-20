#pragma once

#include <optional>

#include "Codec.hpp"

class Encoder : public Codec
{
private:
	uint32_t dataVectorOffset_Index = 0;
	uint16_t escScanningBuffer = 0x00;
	uint8_t escScannerNibbleCount = 0;
	uint8_t escScannerNibbleNotToEscape = 0;
	uint64_t globalNibbleCount = 0;
	bool escScanningBufferHasData = true;
	uint64_t globalBitCount = 0;
	bool stopSoon = false;
	
	bool clock;

	/**
	 * @brief Stellt sicher, dass der Buffer immer min. 2 Nibble hält, in dem es ggf. neue Rohdaten anhängt.
	 *
	 * @details
	 * Zur korrekten Kodierung des Buffers ist es notwenig, dass dieser mindestens 2 Nibble (also ein Byte) an Daten hält.
	 * Diese Methode überprüft dies und hängt falls nötig neue Daten um, um wieder auf mindestens ein Byte im Buffer zu kommen.
	 * Die Methode gibt TRUE zurück, falls nach ihrem Ausführen mindestens ein Byte gebuffert ist. Die Rückgabe FALSE lässt sich so
	 * interpretieren, dass es sich bei dem verbleibenden Nibble im Buffer um das absolut letzte Nibbel der Daten handelt.
	 * Es gibt keine neuen Rohdaten mehr.
	 *
	 * @return TRUE, wenn Buffer nach Ausführung min. 2 Nibble hat; FALSE, sonst.
	 */
	uint8_t getNewRawNibble();
	
	void insertIntoBuffer(const command &command, const uint8_t &atBit);

	void insertIntoBuffer(const uint8_t &byte, const uint8_t &atBit);

	void insertSTOP();

	uint8_t currentThreeBit();

public:

	Encoder(uint8_t escapeSequence, std::vector<uint8_t> &dataVector);

	bool hasData();

	uint8_t nextByte();

	uint8_t nextNibble();
};