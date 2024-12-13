#include "../lib.hpp"

Encoder::Task::Task(const BlockType &blockType, const std::vector<uint8_t>& dataVector_byte)
    : dataStorage(DataStorage(dataVector_byte)),
      blockType(blockType),
      startSequenceSent(false)
{}

void Encoder::forcePushBlock(const BlockType &blockType, const std::vector<uint8_t> &data)
{
	taskStack.push(Task(blockType, data));
}

void Encoder::pushBlock(const BlockType &blockType, const std::vector<uint8_t> &data)
{
    if (taskStack.size() >= 2)
    {
        throw std::runtime_error("Encoder: WARNING - The Encoder currently has two or more tasks in the stack. Adding more tasks may affect the stability of the transmission. If you still wish to add a task, use forcePushBlock(...) to bypass this check.");
    }
	forcePushBlock(blockType, data);
}


bool Encoder::hasData()
{
	return taskStack.size() > 0 || escNibbleQueue != 0x00;
}

uint8_t Encoder::nextNibble()
{
	if (!hasData())
	{
		throw std::out_of_range("Encoder: No data to encode.");
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
		nibbleToReturn = CodecCommand::escapeSequence;
	}
	// Edge-Case: Esc-Seq appears a regular data nibble.
	else if (previousNibble == CodecCommand::escapeSequence)
	{
		nibbleToReturn = determineEscSeqAsDataCommand();
	}
	// The current task has no more data.
	else if (taskStack.top().dataStorage.empty())
	{
		taskStack.pop();
		escNibbleQueue = determineEndCommand();
		nibbleToReturn = CodecCommand::escapeSequence;
	}
	// Edge-Case: Same nibble in succession.
	else if (previousNibble == taskStack.top().dataStorage.peek_nibble())
	{
		taskStack.top().dataStorage.pop_nibble();
		escNibbleQueue = determinePrevNibbleAgainCommand();
		nibbleToReturn = CodecCommand::escapeSequence;
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
	if (taskStack.top().blockType == BlockType::dataBlock)
	{
		if (taskStack.top().dataStorage.peek_nibble() == CodecCommand::beginDataBlockDefault)
		{
			return CodecCommand::beginDataBlockFallback;
		}
		else
		{
			return CodecCommand::beginControlBlockDefault;
		}
	}
	else
	{
		if (taskStack.top().dataStorage.peek_nibble() == CodecCommand::beginControlBlockDefault)
		{
			return CodecCommand::beginControlBlockFallback;
		}
		else
		{
			return CodecCommand::beginControlBlockDefault;
		}
	}
}

uint8_t Encoder::determineEndCommand()
{
	if (taskStack.size() == 0)
	{
		return CodecCommand::endBlockDefault; 
	}
	else if (taskStack.top().dataStorage.empty() || taskStack.top().dataStorage.peek_nibble() != CodecCommand::endBlockDefault)
	{
		return CodecCommand::endBlockDefault;
	}
	else
	{
		return CodecCommand::endBlockDefault;
	}
}

uint8_t Encoder::determinePrevNibbleAgainCommand()
{
	if (taskStack.top().dataStorage.empty())
	{
		return CodecCommand::insertPrevNibbleAgainDefault;
	}
	else if (taskStack.top().dataStorage.peek_nibble() == CodecCommand::insertPrevNibbleAgainDefault)
	{
		return CodecCommand::insertPrevNibbleAgainFallback;
	}
	else
	{
		return CodecCommand::insertPrevNibbleAgainDefault;
	}
}

uint8_t Encoder::determineEscSeqAsDataCommand()
{
	if (taskStack.top().dataStorage.empty())
	{
		return CodecCommand::insertEscSeqAsDataDefault;
	}
	else if (taskStack.top().dataStorage.peek_nibble() == CodecCommand::insertEscSeqAsDataDefault)
	{
		return CodecCommand::insertEscSeqAsDataFallback;
	}
	else
	{
		return CodecCommand::insertEscSeqAsDataDefault;
	}
}

// ==============================================================================================
// DEPRECATED methods DEPRECATED methods DEPRECATED methods DEPRECATED methods DEPRECATED methods 
// ==============================================================================================

void Encoder::inputDataBlock(const std::vector<uint8_t> &dataVector)
{
	pushBlock(BlockType::dataBlock, dataVector);
}

void Encoder::interruptWithControlBlock(const std::vector<uint8_t> &controlVector)
{
	pushBlock(BlockType::controlBlock, controlVector);
}