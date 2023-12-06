#pragma once

#include "CompressionAlgorithm.h"

#include <cstdint>
#include <string>

class LZ77 : public CompressionAlgorithm
{
public:
    /*
    File structure:

    [triples]:

        [distance]
        (16-bit value)

        [length]
        (8-bit value)

        [next_byte]
        (8-bit value)
    */
    void compress_file(std::string file_in, std::string file_out);
    void decompress_file(std::string file_in, std::string file_out);

    LZ77();
    ~LZ77();

private:
    char* buffer;
    uint32_t write_pointer;
    uint32_t read_pointer;

    const uint32_t BUFFER_SIZE_DICTIONARY = 65535;
    const uint32_t BUFFER_SIZE_LOOK_AHEAD = 256;
    const uint32_t BUFFER_SIZE = (BUFFER_SIZE_DICTIONARY + BUFFER_SIZE_LOOK_AHEAD);

    int32_t read_input_file(std::ifstream& fs_in, uint32_t length);
    char read_buffer_dictionary(uint32_t position);
    char read_buffer_look_ahead(uint32_t position);
    void write_triple(std::ofstream& fs_out, uint16_t distance, uint8_t length, uint8_t next_byte);

    //Note: Minimum buffer size is 4 bytes
    int32_t read_triple(std::ifstream& fs_in, char* buffer, uint16_t* distance, uint8_t* length,
                        uint8_t* next_byte);

    uint32_t buffer_bytes_available();
};
