#include "utilities.hpp"

std::vector<uint8_t> utilities::extractSubvector(const std::vector<uint8_t>& vec, size_t start, size_t length)
{
    if (start >= vec.size()) return {};
    length = std::min(length, vec.size() - start);
    return {vec.begin() + start, vec.begin() + start + length};
}