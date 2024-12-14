#pragma once

#include <cstdint>
#include <vector>

#include "header/BlockType.hpp"
#include "header/Codec.hpp"
#include "header/CodecCommand.hpp"
#include "header/ComManager.hpp"
#include "header/ControlPanel.hpp"
#include "header/CRC.hpp"
#include "header/Decoder.hpp"
#include "header/DecoderObserver.hpp"
#include "header/Encoder.hpp"
#include "header/ICommunicationInterface.hpp"
#include "header/Logger.hpp"
#include "header/ThreadSafeQueue.hpp"

namespace ioManager
{
	std::vector<uint8_t> getBinaryInput();

	void setBinaryOutput(const std::vector<uint8_t> &data);

	std::vector<uint8_t> extractSubvector(const std::vector<uint8_t>& vec, size_t start, size_t length);
}