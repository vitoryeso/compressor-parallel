#pragma once

#include "CompressionAlgorithm.h"
#include "LZ78_Node.h"
#include "LZ78_Tuple.h"

#include <cstdint>
#include <string>

class LZ78 : public CompressionAlgorithm
{
public:
    /*
    File structure:

    [tuples]:

        [dictionary index]
        (variable-width value)

        [next token]
        (8-bit value)

    If the last tuple is [variable-width dictionary index][EOF], it is trimmed
    to [variable-width dictionary index] - a denormalized tuple
    */
    void compress_file(std::string file_in, std::string file_out);
    void decompress_file(std::string file_in, std::string file_out);

    LZ78();
    ~LZ78();

private:
    char* buffer;
    uint32_t write_pointer;
    uint32_t read_pointer;
    uint32_t dictionary_index;

    const uint32_t BUFFER_SIZE = 65536;

    LZ78_Tuple* build_tuple(LZ78_Node* current_node, bool* denormalized);
    int32_t read_input_file(std::ifstream& fs_in, uint32_t length);
    char read_buffer();
    uint32_t buffer_bytes_available();

    LZ78_Node* find_dictionary_entry(uint32_t id, LZ78_Node* dictionary,
                                     std::vector<uint8_t>* path);

    uint8_t bits_per_dictionary_id();
};
