#pragma once

#include <string>

class CompressionAlgorithm
{
public:
    virtual void compress_file(std::string file_in, std::string file_out) = 0;
    virtual void decompress_file(std::string file_in, std::string file_out) = 0;

    virtual ~CompressionAlgorithm() {}
};
