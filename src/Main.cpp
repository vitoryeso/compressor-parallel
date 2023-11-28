#include "CompressionAlgorithm.h"
#include "Huffman.h"
#include "LZ77.h"
#include "LZ78.h"
#include "LZW.h"

#include <cstring>
#include <getopt.h>
#include <iostream>
#include <string>

int main(int argc, char** argv)
{
    int compress = -1;
    CompressionAlgorithm* algorithm = NULL;
    std::string file_in;
    std::string file_out;

    int c;

    while (1)
    {
        static struct option long_options[] = {
            {"algorithm",  required_argument, 0, 'a'},
            {"input",      required_argument, 0, 'i'},
            {"output",     required_argument, 0, 'o'},
            {"help",       no_argument,       0, 'h'},
            {0,            0,                 0, 0  }
        };

        c = getopt_long(argc, argv, "cda:i:o:h", long_options, NULL);

        if (c == -1)
            break;

        switch (c)
        {
            case 'a':
                //Algorithm
                if (algorithm)
                {
                    delete algorithm;
                    algorithm = NULL;
                }
                if (strcmp(optarg, "huffman") == 0)
                {
                    algorithm = new Huffman();
                    break;
                }
                if (strcmp(optarg, "lz77") == 0)
                {
                    algorithm = new LZ77();
                    break;
                }
                if (strcmp(optarg, "lz78") == 0)
                {
                    algorithm = new LZ78();
                    break;
                }
                if (strcmp(optarg, "lzw") == 0)
                {
                    algorithm = new LZW();
                    break;
                }
                std::cout << "Algorithm not supported: " << optarg << "\n";
                return 1;
            case 'i':
                //Input file
                file_in = optarg;
                break;
            case 'o':
                //Output file
                file_out = optarg;
                break;
            case 'h':
                //Help
                std::cout << "\n"
                             "Usage: compressor "
                             "(-a <algorithm> | --algorithm <algorithm>) "
                             "(-i <file> | --input <file>) "
                             "(-o <file> | --output <file>)\n"
                             "\n"
                             "Options:\n"
                             "-a <algorithm>, --algorithm <algorithm>   Algorithm to use\n"
                             "-i <file>, --input <file>                 Input file\n"
                             "-o <file>, --output <file>                Output file\n"
                             "\n"
                             "Supported algorithms:\n"
                             "huffman  Canonical Huffman\n"
                             "lz77     Lempel-Ziv 77\n"
                             "lz78     Lempel-Ziv 78\n"
                             "lzw      Lempel-Ziv-Welch\n"
                             "\n";
                delete algorithm;
                return 0;
            case '?':
                //Unknown argument
                delete algorithm;
                return 1;
            default:
                abort();
        }
    }

    if (!algorithm)
    {
        //Missing algorithm
        std::cout << "Missing argument: (-a <algorithm> | --algorithm <algorithm>)\n";
        return 1;
    }

    if (file_in.empty())
    {
        //Missing input file
        std::cout << "Missing argument: (-i <file> | --input <file>)\n";
        delete algorithm;
        return 1;
    }

    if (file_out.empty())
    {
        //Missing output file
        std::cout << "Missing argument: (-o <file> | --output <file>)\n";
        delete algorithm;
        return 1;
    }

    algorithm->decompress(file_in, file_out);

    delete algorithm;

    //Exit program
    return 0;
}
