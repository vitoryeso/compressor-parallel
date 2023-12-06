#pragma once

#include <cstdint>

class Huffman_Codebook
{
public:
    uint16_t size;
    uint32_t* codes;
    uint8_t* codes_length;
    uint8_t* codes_value;

    Huffman_Codebook(uint16_t size);
    ~Huffman_Codebook();
};
