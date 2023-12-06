#include "Hash.h"

uint32_t Hash::FNV_1a_32b(const std::vector<uint8_t>& data)
{
    uint32_t hash = FNV_OFFSET_BASIS_32B;

    for (uint32_t i = 0; i < data.size(); i++)
    {
        hash ^= data[i];
        hash *= FNV_PRIME_32B;
    }

    return hash;
}
