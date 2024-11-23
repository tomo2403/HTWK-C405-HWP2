#pragma once

#include <vector>
#include <cstdint>
#include "Packets.hpp"

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
	explicit CRC(uint32_t polynomial, uint32_t initialValue = 0x00000000);

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


	/**
	 * @brief Berechnet 32-Bit CRC-Wert über ein PrePacket.
	 *
	 * @details
	 * Diese Funktion berechnet den CRC-Wert für ein gegebenes PrePacket.
	 * Die Berechnung erfolgt über die Nachahmung eines (Hardware-)Schieberegisters.
	 *
	 * @param p Das PrePacket, über welches der CRC-Wert berechnet werden soll.
	 * @return uint32_t; 32-Bit CRC-Wert des PrePackets.
	 */
	uint32_t calculateCRC(PrePacket &p) const;

	/**
	 * @brief Validiert CRC-Wert.
	 * @param data Daten, zu welchen ein CRC-Wert validiert werden soll.
	 * @param receivedCRC CRC-Wert, welcher zu den gegebenen Daten vorliegt.
	 * @see calculateCRC(myVector8) - Wird zur Berechnung des CRC-Wertes genutzt, mit dem der gegebene CRC-Wert verglichen wird.
	 * @return bool; TRUE, falls CRC-Wert zu Daten passt; FALSE, sonst
	 */
	[[nodiscard]] bool validateCRC(const std::vector<uint8_t> &data, uint32_t receivedCRC) const;

	[[nodiscard]] bool validateCRC(PostPacket &p) const;

private:
	uint32_t polynomial;
	uint32_t initialValue;
};
