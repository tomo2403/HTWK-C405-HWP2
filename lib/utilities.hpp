#pragma once

#include <vector>
#include <cstdint>

namespace utilities
{
    std::vector<uint8_t> extractSubvector(const std::vector<uint8_t>& vec, std::size_t start, std::size_t length);
}
