#pragma once

#include <cstdint>
#include <vector>

class LZW_Node
{
public:
    uint32_t id;
    std::vector<uint8_t> token;

    LZW_Node(uint32_t id);
    ~LZW_Node();
};
