#pragma once

#include "Codec.hpp"
#include "IDecoderObserver.hpp"

class Decoder
{
private:
	struct Task
	{
		NibbleCompressor nibbleCompressor;
		BlockType blockType;

		// Task(const BlockType &blockType);
	};

	// How is Data without propper start-Sequence treated?
	
	bool escapeModeActive;
	std::vector<IDecoderObserver *> observers;

	std::stack<Task> taskStack;

public:
	Decoder();

	void addObserver(IDecoderObserver *observer);

	void removeObserver(IDecoderObserver *observer);

	void nextNibble(const uint8_t &nibble);
};