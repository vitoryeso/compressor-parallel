#pragma once

#include <cstdint>
#include <vector>

class Hash
{
public:
    static uint32_t FNV_1a_32b(const std::vector<uint8_t>& data);

private:
    static const uint32_t FNV_PRIME_32B = 0x01000193;
    static const uint32_t FNV_OFFSET_BASIS_32B = 0x811C9DC5;
};
