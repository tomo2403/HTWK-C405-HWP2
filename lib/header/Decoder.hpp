#pragma once

#include "Codec.hpp"

class Decoder : public Codec
{
private:
	bool EscapedModeIsActive;
	bool everythingReceived;
	bool packetComplete;

	uint8_t dataVectorBuffer;
	uint8_t dataVectorBufferShiftCount;

	std::vector<uint8_t> &dataVector;

	static bool partnerIsReady;

	void processCommand(const uint8_t &command);

	void writeToDataVector(const uint8_t &nibble);

public:
	explicit Decoder(std::vector<uint8_t> &dataVector);

	void nextNibble(const uint8_t &nibble);

	[[nodiscard]] bool hasData() const;

	[[nodiscard]] static bool connectionIsOnline();

	void flushBufferIntoDataVector();

	PostPacket decodeAll(std::vector<uint8_t> &data);
};