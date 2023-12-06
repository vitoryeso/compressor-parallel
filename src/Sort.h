#pragma once

#include "Huffman_Codebook.h"
#include "Huffman_Node.h"

#include <cstdint>

class Sort
{
public:
    static void sort_nodes_by_occurrences(Huffman_Node** a, uint16_t size);
    static void sort_huffman_codebook_by_length_then_by_value_ascending(Huffman_Codebook* hc);
};
