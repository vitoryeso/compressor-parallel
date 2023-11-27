#pragma once

#include <cstdint>

class LZ78_Tuple
{
public:
    uint32_t id;
    uint8_t token;

    LZ78_Tuple();
    LZ78_Tuple(uint32_t id, uint8_t token);
};
