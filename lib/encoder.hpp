#pragma once

#include <optional>

#include "codec.hpp"

class encoder : public codec {
private:
    uint32_t dataVectorOffset_Index = 0;
    uint8_t bitsNotToEscape = 0;
    
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
    bool fetchDataIfBufferTooSmall();

    void InsertEscapeIfNecessary();

    void insertIntoBuffer(const command &command, const uint8_t &atBit);
    void insertIntoBuffer(const uint8_t &byte, const uint8_t &atBit);

public:

    encoder(uint8_t escapeSequence, std::vector<uint8_t> &dataVector);
    bool hasData();

    std::optional<uint8_t> nextByte();
    std::optional<uint8_t> nextNibble();
};