#include <algorithm>

#include "../header/Decoder.hpp"
#include "../header/CodecCommand.hpp"

Decoder::Task::Task(const BlockType &blockType)
    : blockType(blockType), nibbleCompressor(NibbleCompressor())
{
}

Decoder::Decoder()
{
}

void Decoder::addObserver(IDecoderObserver *observer)
{
	observers.push_back(observer);
}

void Decoder::removeObserver(IDecoderObserver *observer)
{
	observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
}

void Decoder::nextNibble(const uint8_t &nibble)
{
	if (nibble == CodecCommand::escapeSequence)
	{
		escapeModeActive = true;
	}
	else if (escapeModeActive)
	{
		processCommand(static_cast<CodecCommand>(nibble));
		escapeModeActive = false;
	}
	else
	{
		previousNibble = nibble;
		taskStack.top().nibbleCompressor.pushBack(nibble);
	}
}

void Decoder::processCommand(const CodecCommand &command)
{
	switch (command)
	{
	case beginDataBlockDefault:
	case beginDataBlockFallback:
		processBeginDataBlockCommand();
		break;

	case beginControlBlockDefault:
	case beginControlBlockFallback:
		processBeginControlBlockCommand();
		break;

	case endBlockDefault:
	case endBlockFallback:
		processEndBlockCommand();
		break;

	case insertPrevNibbleAgainDefault:
	case insertPrevNibbleAgainFallback:
		processInsertPrevNibbleAgainCommand();
		break;

	case insertEscSeqAsDataDefault:
	case insertEscSeqAsDataFallback:
		processInsertEscSeqAsDefaultCommand();
		break;

	default:
		break;
	}
}

void Decoder::processBeginDataBlockCommand()
{
	taskStack.push(Task(BlockType::dataBlock));
}

void Decoder::processBeginControlBlockCommand()
{
	taskStack.push(Task(BlockType::controlBlock));
}

void Decoder::processEndBlockCommand()
{
	for (auto observer : observers)
	{
		observer->endBlockReceived(taskStack.top().blockType, taskStack.top().nibbleCompressor.getData());
	}

	taskStack.pop();
}

void Decoder::processInsertPrevNibbleAgainCommand()
{
	taskStack.top().nibbleCompressor.pushBack(previousNibble);
}

void Decoder::processInsertEscSeqAsDefaultCommand()
{
	taskStack.top().nibbleCompressor.pushBack(static_cast<uint8_t>(static_cast<uint8_t>(CodecCommand::escapeSequence)));
}
