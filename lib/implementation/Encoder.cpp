#include "../header/Encoder.hpp"

#include <stdexcept>

#include "../header/CodecCommand.hpp"

Encoder::Task::Task(const BlockType &blockType, const std::vector<uint8_t> &dataVector_byte, const std::function<void()> &callback)
	: dataStorage(DataStorage(dataVector_byte)),
	  blockType(blockType),
	  startSequenceSent(false),
	  callback(callback)
{
}

void Encoder::forcePushBlock(const BlockType &blockType, const std::vector<uint8_t> &data, const std::function<void()> &callback)
{
	taskStack.emplace(blockType, data, callback);
}

void Encoder::pushBlock(const BlockType &blockType, const std::vector<uint8_t> &data, const std::function<void()> &callback)
{
	if (taskStack.size() >= 2)
	{
		throw std::runtime_error(
			"Encoder: WARNING - The Encoder currently has two or more tasks in the stack. Adding more tasks may affect the stability of the transmission. If you still wish to add a task, use forcePushBlock(...) to bypass this check.");
	}

	if (blockType == controlBlock && callback != nullptr)
	{
		throw std::invalid_argument("Encoder: WARNING - A control block does not require a callback function. Callbacks are intended for data blocks only. If you still wish to add a callback, use forcePushBlock(...) to bypass this check.");
	}

	forcePushBlock(blockType, data, callback);
}


bool Encoder::hasData()
{
	return !taskStack.empty() || escNibbleQueue != 0x00;
}

uint8_t Encoder::nextNibble()
{
	if (!hasData())
	{
		throw std::out_of_range("Encoder: No data to encode.");
	}

	// The task giver can request a callback to be executed, if the
	// data (handed in for encoding) is fully sent.
	// This condition can only become true, if a callback was provided and
	// if the nibbleToReturn is the very last nibble (the endCommand) of a
	// block.
	if(callbackQueue != nullptr)
	{
		callbackQueue();
		callbackQueue = nullptr;
	}

	uint8_t nibbleToReturn = 0x00;

	// The previous edge case required extensive escaping,
	// the rest of its escape-sequence is being sent here,
	// before the next data nibble can be precessed.
	if (escNibbleQueue != 0x00)
	{
		nibbleToReturn = escNibbleQueue & 0x0F;
		escNibbleQueue = 0x00;
	}
	// The current task has not yet sent a start-sequence.
	else if (!taskStack.top().startSequenceSent)
	{
		escNibbleQueue = determineStartCommand();
		taskStack.top().startSequenceSent = true;
		nibbleToReturn = escapeSequence;
	}
	// Edge-Case: Esc-Seq appears a regular data nibble.
	else if (previousNibble == escapeSequence)
	{
		nibbleToReturn = determineEscSeqAsDataCommand();
	}
	// The current task has no more data.
	else if (taskStack.top().dataStorage.empty())
	{
		callbackQueue = taskStack.top().callback;	
		taskStack.pop();
		escNibbleQueue = determineEndCommand();
		nibbleToReturn = escapeSequence;
	}
	// Edge-Case: Same nibble in succession.
	else if (previousNibble == taskStack.top().dataStorage.peek_nibble())
	{
		taskStack.top().dataStorage.pop_nibble();
		escNibbleQueue = determinePrevNibbleAgainCommand();
		nibbleToReturn = escapeSequence;
	}
	// Default-Case
	else
	{
		nibbleToReturn = taskStack.top().dataStorage.pop_nibble();
	}

	previousNibble = nibbleToReturn;
	return nibbleToReturn;
}

uint8_t Encoder::determineStartCommand()
{
	if (taskStack.top().blockType == dataBlock)
	{
		return taskStack.top().dataStorage.peek_nibble() == beginDataBlockDefault ? beginDataBlockFallback : beginControlBlockDefault;
	}

	return taskStack.top().dataStorage.peek_nibble() == beginControlBlockDefault ? beginControlBlockFallback : beginControlBlockDefault;
}

uint8_t Encoder::determineEndCommand()
{
	if (taskStack.empty())
	{
		return endBlockDefault;
	}

	return taskStack.top().dataStorage.empty() || taskStack.top().dataStorage.peek_nibble() != endBlockDefault
		       ? endBlockFallback
		       : endBlockDefault;
}

uint8_t Encoder::determinePrevNibbleAgainCommand()
{
	if (taskStack.top().dataStorage.empty())
	{
		return insertPrevNibbleAgainDefault;
	}

	return taskStack.top().dataStorage.peek_nibble() == insertPrevNibbleAgainDefault
		       ? insertPrevNibbleAgainFallback
		       : insertPrevNibbleAgainDefault;
}

uint8_t Encoder::determineEscSeqAsDataCommand()
{
	if (taskStack.top().dataStorage.empty())
	{
		return insertEscSeqAsDataDefault;
	}

	return taskStack.top().dataStorage.peek_nibble() == insertEscSeqAsDataDefault ? insertEscSeqAsDataFallback : insertEscSeqAsDataDefault;
}

// ==============================================================================================
// DEPRECATED methods DEPRECATED methods DEPRECATED methods DEPRECATED methods DEPRECATED methods 
// ==============================================================================================

void Encoder::inputDataBlock(const std::vector<uint8_t> &dataVector)
{
	pushBlock(dataBlock, dataVector);
}

void Encoder::interruptWithControlBlock(const std::vector<uint8_t> &controlVector)
{
	pushBlock(controlBlock, controlVector);
}
