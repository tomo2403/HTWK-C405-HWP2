#pragma once

#ifdef ARDUINO
#include <Arduino.h>
#include <Vector.h>
typedef Vector<uint8_t> myVector8;
#else
#include <vector>
#include <cstdint>
typedef std::vector<uint8_t> myVector8;
#endif

/**
 * @brief Klasse zum Generieren und Validieren eines CRC-Wertes.
 *
 * @details
 * Die Klasse unterstützt (aktuell) nur die Berechnung "gewöhnlicher" CRCs. Das TReflektieren der Eingabe oder Ausgabe
 * ist nicht (noch) nicht möglich. Bisher sind nur 32-Bit CRCs berechenbar.
 */
class CRC
{
public:
	/**
	 * @brief Konstruktor, erstellt neue "CRC-Fabrik"
	 * @param polynomial Generator-Polynom, mit welchem die CRC-Werte berechnet werden sollen.
	 * @param initialValue Initialwert den der CRC-Wert hat, noch bevor irgendeine Brechung durchgeführt wurde. Falls nicht gesetzt, wird dieser Wert "0x00000000".
	 */
	CRC(uint32_t polynomial, uint32_t initialValue = 0x00000000);

	/**
	 * @brief Berechnet 32-Bit CRC-Wert über gegebene Daten.
	 *
	 * @details
	 * Die Brechung erfolgt über die Nachahmung eines (Hardware-)Schieberegisters. Der CRC wird inplace und
	 * ohne zusätzlichen Speicherbedarf berechnet. Jeder Funktionsaufruf führt zu einer neuen Berechnung.
	 *
	 * @param data Daten, über welche der CRC-Wert berechnet werden soll.
	 * @return uint32_t; (Nur) 32bit CRC-Wert der Daten.
	 */
	uint32_t calculateCRC(const myVector8 &data) const;

	/**
	 * @brief Validiert CRC-Wert.
	 * @param data Daten, zu welchen ein CRC-Wert validiert werden soll.
	 * @param receivedCRC CRC-Wert, welcher zu den gegebenen Daten vorliegt.
	 * @see calculateCRC(myVector8) - Wird zur Berechnung des CRC-Wertes genutzt, mit dem der gegebene CRC-Wert verglichen wird.
	 * @return bool; TRUE, falls CRC-Wert zu Daten passt; FALSE, sonst
	 */
	bool validateCRC(const myVector8 &data, uint32_t receivedCRC) const;

private:
	uint32_t polynomial;
	uint32_t initialValue;
};
