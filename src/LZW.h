#pragma once

#include "CompressionAlgorithm.h"
#include "Hash.h"

#include <cstdint>
#include <string>

class LZW : public CompressionAlgorithm
{
public:
    /*
    File structure:

    [dictionary index]
    (multiple variable-width values)

    Initial dictionary: All possible bytes (256 entries)
    Initial code width: 9 bits
    */
    void compress_file(std::string file_in, std::string file_out);
    void decompress_file(std::string file_in, std::string file_out);

    LZW();
    ~LZW();

private:
    char* buffer;
    uint32_t write_pointer;
    uint32_t read_pointer;
    uint32_t dictionary_index;

    const uint32_t BUFFER_SIZE = 65536;

    int32_t read_input_file(std::ifstream& fs_in, uint32_t length);
    char read_buffer();
    void unread_buffer();

    uint32_t buffer_bytes_available();

    uint8_t bits_per_dictionary_id();

    struct HashAlgorithm
    {
        uint32_t operator()(const std::vector<uint8_t>& data) const
        {
            return Hash::FNV_1a_32b(data);
        }
    };
};
