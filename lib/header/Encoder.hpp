#pragma once

#include <stack>

#include "BlockType.hpp"
#include "DataStorage.hpp"

class Encoder
{
private:
	struct Task
	{
		DataStorage dataStorage;
		BlockType blockType;
		bool startSequenceSent;

		Task(const BlockType &blockType, const std::vector<uint8_t> &dataVector_byte);
	};

	uint8_t escNibbleQueue{};
	uint8_t previousNibble{};

	std::stack<Task> taskStack;

	std::mutex mtx;

	uint8_t determineStartCommand();

	uint8_t determineEndCommand();

	uint8_t determinePrevNibbleAgainCommand();

	uint8_t determineEscSeqAsDataCommand();

public:
	void pushBlock(const BlockType &blockType, const std::vector<uint8_t> &data);

	void forcePushBlock(const BlockType &blockType, const std::vector<uint8_t> &data);

	[[nodiscard]] bool hasData() const;

	uint8_t nextNibble();

	[[deprecated("Use pushBlock(...) instead")]] void inputDataBlock(const std::vector<uint8_t> &dataVector);

	[[deprecated("Use pushBlock(...) instead")]] void interruptWithControlBlock(const std::vector<uint8_t> &controlVector);
};
