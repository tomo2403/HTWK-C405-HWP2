#include "../header/Decoder.hpp"
#include "../header/CodecCommand.hpp"

Decoder::Task::Task(const BlockType &blockType) : nibbleCompressor(NibbleCompressor()), blockType(blockType)
{
}

Decoder::Decoder() = default;

void Decoder::addObserver(IDecoderObserver *observer)
{
	observers.push_back(observer);
}

void Decoder::removeObserver(IDecoderObserver *observer)
{
	std::erase(observers, observer);
}

void Decoder::nextNibble(const uint8_t &nibble)
{
	if (nibble == escapeSequence)
	{
		escapeModeActive = true;
	}
	else if (escapeModeActive)
	{
		processCommand(static_cast<CodecCommand>(nibble));
		escapeModeActive = false;
	}
	// The following processing can only happen if at least one task is currently being
	// worked on. If this is not the case, a faulty command nibble has been read, and
	// it is simply ignored. Note that escape sequences and their follow-up commands
	// are always processed; otherwise, no task can be created in the first place.
	else if (!taskStack.empty())
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
		
		default:
			break;
	}

	// The following commands can only be processed if a task is currently being
	// worked on. If this is not the case, a faulty command nibble has been read. In
	// this case, it is simply ignored.
	if (taskStack.empty()) return;

	switch (command)
	{
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
	taskStack.emplace(dataBlock);
}

void Decoder::processBeginControlBlockCommand()
{
	taskStack.emplace(controlBlock);
}

void Decoder::processEndBlockCommand()
{
	for (const auto observer: observers)
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
	taskStack.top().nibbleCompressor.pushBack(escapeSequence);
}
