#pragma once

#include "Codec.hpp"
#include "DecoderObserver.hpp"

class Decoder : public Codec
{
private:
	struct Storage
	{
		uint32_t buffer{};
		bool previousNibbleExists{};
		uint8_t previousNibble{};
		int8_t bufferEndBit{};
		bool EscapedModeIsActive{};
		uint8_t dataVectorBuffer{};
		uint8_t dataVectorBufferShiftCount{};
		std::vector<uint8_t> dataVector;
		BlockType currentBlockType;
	};

	bool EscapedModeIsActive{};
	bool dataVectorIsLocked = true;

	uint8_t dataVectorBuffer{};
	uint8_t dataVectorBufferShiftCount{};

	std::vector<uint8_t> dataVector;

	std::vector<DecoderObserver *> observers;

	BlockType currentBlockType;

	bool storageHoldsData = false;
	Storage storage = Storage();

	void processCommand(const uint8_t &command);

	void writeToDataVector(const uint8_t &nibble);

	void flushBufferIntoDataVector();

	void initialize();

	void saveCurrentAttributes();

	void restoreSavedAttributes();

public:
	Decoder();

	void addObserver(DecoderObserver *observer);

	void removeObserver(DecoderObserver *observer);

	void nextNibble(const uint8_t &nibble);
};