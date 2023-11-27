#include "Huffman_Node.h"

#include <cstddef>

Huffman_Node::Huffman_Node(int32_t id, bool is_leaf_node)
{
    this->occurrences = 0;
    this->id = id;
    this->is_leaf_node = is_leaf_node;
    this->left = NULL;
    this->right = NULL;
    this->parent = NULL;
}

Huffman_Node::~Huffman_Node()
{
    if (this->left)
        delete this->left;
    if (this->right)
        delete this->right;
}
