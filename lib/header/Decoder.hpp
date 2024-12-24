#pragma once

#include <stack>
#include "NibbleCompressor.hpp"
#include "BlockType.hpp"
#include "IDecoderObserver.hpp"
#include "CodecCommand.hpp"

class Decoder
{
	struct Task
	{
		NibbleCompressor nibbleCompressor;
		BlockType blockType;

		explicit Task(const BlockType &blockType);
	};

	// How is Data without proper start-Sequence treated?

	bool escapeModeActive{};
	uint8_t previousNibble{};

	std::vector<IDecoderObserver *> observers;
	std::stack<Task> taskStack;

	void processCommand(const CodecCommand &command);

	void processBeginDataBlockCommand();

	void processBeginControlBlockCommand();

	void processEndBlockCommand();

	void processInsertPrevNibbleAgainCommand();

	void processInsertEscSeqAsDefaultCommand();

public:
	Decoder();

	void addObserver(IDecoderObserver *observer);

	void removeObserver(IDecoderObserver *observer);

	void nextNibble(const uint8_t &nibble);
};
