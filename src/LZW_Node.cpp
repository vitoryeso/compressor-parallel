#include "LZW_Node.h"

#include <cstddef>

LZW_Node::LZW_Node(uint32_t id)
{
    this->id = id;
}

LZW_Node::~LZW_Node()
{
    std::vector<uint8_t>().swap(token);
}
