#pragma once

#include "CompressionAlgorithm.h"
#include "Huffman_Codebook.h"
#include "Huffman_Node.h"

#include <cstdint>
#include <string>

class Huffman : public CompressionAlgorithm
{
public:
    /*
    File structure:

    [size of dictionary - 1]
    (8-bit value)

    [size of original file]
    (56-bit value)

    [number of symbols with codeword length of 1...n]
    (multiple 8-bit values)

    [codeword values sorted in increasing order by codeword length and then by codeword value]
    (multiple 8-bit values)

    [compressed data]
    (multiple variable-width values)
    */
    void compress_file(std::string file_in, std::string file_out);
    void decompress_file(std::string file_in, std::string file_out);

    Huffman();
    ~Huffman();

private:
    const int BUFFER_SIZE = 4096;

    Huffman_Node** nodes;
    Huffman_Node* tree;
    uint16_t leaf_nodes;
    uint16_t compressed_data_starts_at;
    uint64_t original_file_size;

    Huffman_Codebook* codebook;

    void count_occurrences(std::string file);
    void sort_nodes();
    void construct_tree();
    void create_codebook();
    void create_canonical_codebook();
    void read_canonical_codebook(std::string file);

    void compress(std::string file_in, std::string file_out);
    void decompress(std::string file_in, std::string file_out);
};
