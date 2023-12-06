#include "LZ78_Node.h"

#include <cstddef>

LZ78_Node::LZ78_Node(uint32_t id, uint8_t token)
{
    this->id = id;
    this->token = token;
}

LZ78_Node::~LZ78_Node()
{
    for (uint16_t i = 0; i < leaves.size(); i++)
        delete leaves[i];
    std::vector<LZ78_Node*>().swap(leaves);
}

LZ78_Node* LZ78_Node::find_node_with_token(LZ78_Node* current_node, uint8_t token)
{
    for (uint16_t i = 0; i < current_node->leaves.size(); i++)
        if (current_node->leaves[i]->token == token)
            return current_node->leaves[i];
    return NULL;
}
