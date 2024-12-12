#pragma once

#include <stack>

#include "Encoder.hpp"
#include "DataStorage.hpp"

class Encoder
{
private:
	struct Task
	{
		DataStorage dataStorage;
		BlockType blockType;
		bool startSequenceSent;
	};

	uint8_t escNibbleQueue;
	uint8_t previousNibble;
	
	std::stack<Task> taskStack;

	uint8_t determineStartCommand();
	uint8_t determineEndCommand();
	uint8_t determinePrevNibbleAgainCommand();
	uint8_t determineEscSeqAsDataCommand();
	
public:
	// [[deprecated("Use pushBlock(...) instead")]] void inputDataBlock(const std::vector<uint8_t> &dataVector);

	// [[deprecated("Use pushBlock(...) instead")]] void interruptWithControlBlock(const std::vector<uint8_t> &controlVector);

	void pushBlock(const BlockType &blockType, const std::vector<uint8_t> &data);

	void forcePushBlock(const BlockType &blockType, const std::vector<uint8_t> &data);

	bool hasData();

	uint8_t nextNibble();

	std::vector<uint8_t> encodeAll();
};