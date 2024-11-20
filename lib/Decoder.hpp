#include "Codec.hpp"

class Decoder : public Codec
{
private:
	bool EscapedModeIsActive = false;
	bool flippedPevNibble = false;
	uint8_t nibblesNotToDecode = 0;
	uint8_t nibbleNotToFlip = 0;
	bool escAllowed = true;
	uint8_t timesToRun = 0;
	bool waitForEsc = true;
	bool everythingReceived = false;

	uint8_t dataVectorBuffer = 0x00;
	uint8_t dataVectorBufferShiftCount = 0;

	bool partnerIsReady = false;

	void processCommand(const uint8_t &command);

	void writeToDataVector(const uint8_t &nibble);

public:
	Decoder(uint8_t escapeSequence, std::vector<uint8_t> &dataVector);

	void nextNibble(const uint8_t &nibble);

	[[nodiscard]] bool hasData() const;

	[[nodiscard]] bool connectionIsOnline() const;

	void flushBufferIntoDataVector();
};