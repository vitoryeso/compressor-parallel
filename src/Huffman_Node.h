#pragma once

#include <cstdint>

class Huffman_Node
{
public:
    int32_t id;
    uint64_t occurrences;
    bool is_leaf_node;
    Huffman_Node* left;
    Huffman_Node* right;
    Huffman_Node* parent;

    Huffman_Node(int32_t id, bool is_leaf_node);
    ~Huffman_Node();
};
