#pragma once

#include <cstdint>
#include <vector>

#include "header/BlockType.hpp"
#include "header/CodecCommand.hpp"
#include "header/CRC.hpp"
#include "header/IDecoderObserver.hpp"
#include "header/Logger.hpp"
#include "header/ThreadSafeQueue.hpp"
#include "header/ControlPanel.hpp"
#include "header/Encoder.hpp"
#include "header/Decoder.hpp"

namespace ioManager
{
	std::vector<uint8_t> getBinaryInput();

	void setBinaryOutput(const std::vector<uint8_t> &data);
}