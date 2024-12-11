#include "../lib.hpp"

void Encoder::forcePushBlock(const BlockType &blockType, const std::vector<uint8_t> &data)
{
	Task task;
	task.dataStorage = DataStorage(data);
	task.blockType = blockType;
	task.startSequenceSent = false;
	task.endSequenceIsInTransmission = false;
	
	taskStack.push(task);
}

void Encoder::pushBlock(const BlockType &blockType, const std::vector<uint8_t> &data)
{
    if (taskStack.size() >= 2)
    {
        throw std::runtime_error("Warning: The Encoder currently has two or more tasks in the stack. Adding more tasks may affect the stability of the transmission. If you still wish to add a task, consider using forcePushBlock() to bypass this check.");
    }
	forcePushBlock(blockType, data);
}


bool Encoder::hasData()
{
	return taskStack.size() > 0;
}

uint8_t Encoder::nextNibble()
{
	// TODO: Prev Nibble is being set nowhere -> loop!

	if (!hasData())
	{
		throw std::out_of_range("The Encoder does not have data.");
	}

	// The previous edge case required extensive escaping,
	// the rest of its escape-sequence is being sent here,
	// before the next data nibble can be precessed.
	if (escNibbleQueue != 0x00)
	{
		if (taskStack.top().endSequenceIsInTransmission)
		{
			taskStack.pop();
		}
		
		const uint8_t tmpEscNibbleQueue = escNibbleQueue;
		escNibbleQueue = 0x00;
		return tmpEscNibbleQueue & 0x0F;
	}

	// The current task has no more data.
	if (taskStack.top().dataStorage.empty())
	{
		escNibbleQueue = determineEndCommand();
		taskStack.top().endSequenceIsInTransmission = true;
		return CodecCommand::escapeSequence;
	}

	// The current task has not yet sent a start-seq.
	if (!taskStack.top().startSequenceSent)
	{
		escNibbleQueue = determineStartCommand();
		taskStack.top().startSequenceSent = true;
		return CodecCommand::escapeSequence;
	}

	// Edge-Case: Same nibble back-to-back.
	if (previousNibble == taskStack.top().dataStorage.peek_nibble())
	{
		escNibbleQueue = determinePrevNibbleAgainCommand();
		return CodecCommand::escapeSequence;
	}

	// Edge-Case: Esc-Seq appears a regular data nibble.
	if (previousNibble == CodecCommand::escapeSequence)
	{
		return determineEscSeqAsDataCommand();
	}

	// Default-Case
	return taskStack.top().dataStorage.pop_nibble();
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
	if (taskStack.top().dataStorage.peek_nibble() == CodecCommand::endBlockDefault)
	{
		return CodecCommand::endBlockFallback;
	}
	else
	{
		return CodecCommand::endBlockDefault;
	}
}

uint8_t Encoder::determinePrevNibbleAgainCommand()
{
	if (taskStack.top().dataStorage.peek_nibble() == CodecCommand::insertPrevNibbleAgainDefault)
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
	if (taskStack.top().dataStorage.peek_nibble() == CodecCommand::insertEscSeqAsDataDefault)
	{
		return CodecCommand::insertEscSeqAsDataFallback;
	}
	else
	{
		return CodecCommand::insertEscSeqAsDataDefault;
	}
}