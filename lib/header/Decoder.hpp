#pragma once

#include "Codec.hpp"
#include "IDecoderObserver.hpp"

class Decoder : public Codec
{
private:
	bool EscapedModeIsActive;
	bool dataVectorIsLocked = true;

	uint8_t dataVectorBuffer;
	uint8_t dataVectorBufferShiftCount;

	std::vector<uint8_t> dataVector;

	std::vector<IDecoderObserver*> observers;

	void processCommand(const uint8_t &command);

	void writeToDataVector(const uint8_t &nibble);

	void flushBufferIntoDataVector();

	void initialize();

public:
	Decoder();

	void addObserver(IDecoderObserver* observer);

	void removeObserver(IDecoderObserver* observer);

	void nextNibble(const uint8_t &nibble);

	std::vector<uint8_t> getDataVector();
};