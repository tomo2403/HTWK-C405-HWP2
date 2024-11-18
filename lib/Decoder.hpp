#include "Codec.hpp"
#include "CommandObserver.cpp"

class Decoder : public Codec
{
private:
	Subject commandSubject;
	bool EscapedModeIsActive = false;
	bool flippedPevNibble = false;
	uint8_t nibblesNotToDecode = 0;

	uint8_t dataVectorBuffer = 0x00;
	uint8_t dataVectorBufferShiftCount = 0;

	bool partnerIsReady = false;

	void processCommand(const uint8_t &command);

	void writeToDataVector(const uint8_t &nibble);

public:
	Decoder(uint8_t escapeSequence, std::vector<uint8_t> &dataVector, CommandObserver *commandObserver);
	Decoder(uint8_t escapeSequence, std::vector<uint8_t> &dataVector);

	void nextNibble(const uint8_t &nibble);

	void flushBufferIntoDataVector();
};