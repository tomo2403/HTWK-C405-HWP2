#include "Codec.hpp"

class Decoder : public Codec
{
private:
	bool EscapedModeIsActive = false;
	bool flippedPevNibble = false;
	uint8_t nibblesNotToDecode = 0;

	uint8_t dataVectorBuffer = 0x00;
	uint8_t dataVectorBufferShiftCount = 0;

	bool processCommand(const uint8_t &command);

	void writeToDataVector(const uint8_t &nibble);

public:
	Decoder(uint8_t escapeSequence, std::vector<uint8_t> &dataVector);

	void nextNibble(const uint8_t &nibble);

	void flushBufferIntoDataVector();
};