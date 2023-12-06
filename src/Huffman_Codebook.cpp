#include "Huffman_Codebook.h"

#include <cstdint>

Huffman_Codebook::Huffman_Codebook(uint16_t size)
{
    this->size = size;
    codes = new uint32_t[size];
    codes_length = new uint8_t[size];
    codes_value = new uint8_t[size];
}

Huffman_Codebook::~Huffman_Codebook()
{
    delete[] codes;
    delete[] codes_length;
    delete[] codes_value;
}
