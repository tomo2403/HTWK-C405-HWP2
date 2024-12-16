#pragma once

#include <stack>
#include <mutex>

#include "BlockType.hpp"
#include "DataStorage.hpp"
#include <functional>

class Encoder
{
private:
	struct Task
	{
		DataStorage dataStorage;
		BlockType blockType;
		bool startSequenceSent;
		std::function<void()> callback;

		Task(const BlockType &blockType, const std::vector<uint8_t> &dataVector_byte, const std::function<void()> &callback);
	};

	uint8_t escNibbleQueue{};
	uint8_t previousNibble{};

	std::function<void()> callbackQueue;

	std::stack<Task> taskStack;

	uint8_t determineStartCommand();

	uint8_t determineEndCommand();

	uint8_t determinePrevNibbleAgainCommand();

	uint8_t determineEscSeqAsDataCommand();

public:
	void pushBlock(const BlockType &blockType, const std::vector<uint8_t> &data, const std::function<void()> &callback = nullptr);

	void forcePushBlock(const BlockType &blockType, const std::vector<uint8_t> &data, const std::function<void()> &callback = nullptr);

	[[nodiscard]] bool hasData();

	uint8_t nextNibble();

	[[deprecated("Use pushBlock(...) instead")]] void inputDataBlock(const std::vector<uint8_t> &dataVector);

	[[deprecated("Use pushBlock(...) instead")]] void interruptWithControlBlock(const std::vector<uint8_t> &controlVector);
};
