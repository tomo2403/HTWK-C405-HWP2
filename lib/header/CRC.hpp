#pragma once

#include <vector>
#include <cstdint>

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
	 * @brief Berechnet 32-Bit CRC-Wert über gegebene Daten.
	 *
	 * @details
	 * Die Brechung erfolgt über die Nachahmung eines (Hardware-)Schieberegisters. Der CRC wird inplace und
	 * ohne zusätzlichen Speicherbedarf berechnet. Jeder Funktionsaufruf führt zu einer neuen Berechnung.
	 *
	 * @param data Daten, über welche der CRC-Wert berechnet werden soll.
	 * @return uint32_t; (Nur) 32bit CRC-Wert der Daten.
	 */
	[[nodiscard]] uint32_t calculateCRC(const std::vector<uint8_t> &data) const;

	void attachCRC(std::vector<uint8_t> &data) const;

	/**
	 * @brief Validiert CRC-Wert.
	 * @param data Daten, zu welchen ein CRC-Wert validiert werden soll.
	 * @param receivedCRC CRC-Wert, welcher zu den gegebenen Daten vorliegt.
	 * @see calculateCRC(myVector8) - Wird zur Berechnung des CRC-Wertes genutzt, mit dem der gegebene CRC-Wert verglichen wird.
	 * @return bool; TRUE, falls CRC-Wert zu Daten passt; FALSE, sonst
	 */
	[[nodiscard]] bool validateCRC(const std::vector<uint8_t> &data, uint32_t receivedCRC) const;

private:
	uint32_t polynomial = 0x04c11db7;
	uint32_t initialValue = 0xffffffff;
};