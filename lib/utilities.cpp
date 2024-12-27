#include "utilities.hpp"

std::vector<uint8_t> utilities::extractSubvector(const std::vector<uint8_t>& vec, std::size_t start, std::size_t length)
{
    if (start >= vec.size()) return {};
    length = std::min(length, vec.size() - start);
    return {vec.begin() + start, vec.begin() + start + length};
}