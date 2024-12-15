#pragma once

#include <vector>
#include <cstdint>

namespace utilities
{
    std::vector<uint8_t> extractSubvector(const std::vector<uint8_t>& vec, size_t start, size_t length);
}
