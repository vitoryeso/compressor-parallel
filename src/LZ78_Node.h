#pragma once

#include <cstdint>
#include <vector>

class LZ78_Node
{
public:
    uint32_t id;
    uint8_t token;
    std::vector<LZ78_Node*> leaves;

    LZ78_Node(uint32_t id, uint8_t token);
    ~LZ78_Node();

    static LZ78_Node* find_node_with_token(LZ78_Node* current_node, uint8_t token);
};
