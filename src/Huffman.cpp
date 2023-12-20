#include "Huffman.h"

#include "Huffman_Codebook.h"
#include "Huffman_Node.h"
#include "Sort.h"

#include <bitset>
#include <fstream>
#include <iostream>
#include <queue>
#include <string>
#include <omp.h>
#include <cstring>

void Huffman::count_occurrences(std::string file)
{
    //Count byte occurrences in a file
    std::ifstream fs(file, std::ifstream::binary);
    char* buffer = new char[BUFFER_SIZE];

    while (fs.good())
    {
        fs.read(buffer, BUFFER_SIZE);
        int32_t bytes_read = fs.gcount();

        for (int32_t i = 0; i < bytes_read; i++)
            nodes[(uint8_t)buffer[i]]->occurrences++;
    }

    delete[] buffer;
}

void Huffman::sort_nodes()
{
    //Sort nodes descending by occurrences
    Sort::sort_nodes_by_occurrences(nodes, 256);
}

void Huffman::construct_tree()
{
    //Create priority queue with custom comparator (sort by occurrences ascending, then by id
    //ascending)
    auto comparator = [](Huffman_Node* a, Huffman_Node* b)
    {
        if (a->occurrences > b->occurrences)
            return true;
        if (a->occurrences == b->occurrences)
            return a->id > b->id;
        return false;
    };
    std::priority_queue<Huffman_Node*, std::vector<Huffman_Node*>, decltype(comparator)> queue(
        comparator);

    //Fill priority queue with data
    for (uint16_t i = 0; i < 256; i++)
    {
        if (!nodes[i]->occurrences)
            break;
        queue.push(nodes[i]);
    }
    leaf_nodes = queue.size();

    //Process while the queue is not empty
    while (queue.size() > 1)
    {
        //Remove two nodes of lowest probability from queue
        Huffman_Node* n1 = queue.top();
        queue.pop();
        Huffman_Node* n2 = queue.top();
        queue.pop();

        //Create new node using acquired two nodes
        Huffman_Node* n = new Huffman_Node(-1, false);
        n->left = n1;
        n->right = n2;
        n->occurrences = n1->occurrences + n2->occurrences;
        n1->parent = n;
        n2->parent = n;

        //Add new node to queue
        queue.push(n);
    }

    //Store constructed tree
    tree = queue.top();
    queue.pop();
}

void Huffman::create_codebook()
{
    //Initialize codebook
    codebook = new Huffman_Codebook(leaf_nodes);

    //For every byte to encode
    for (uint16_t i = 0; i < leaf_nodes; i++)
    {
        //Create codeword
        uint32_t codeword = 0;
        uint8_t codeword_length = 0;
        //Start from MSB
        uint8_t position = 31;

        //Get node
        Huffman_Node* n = nodes[i];

        //Do while node is not root
        do
        {
            //Increment codeword length
            codeword_length++;

            //If node is right leaf of parent, set bit at this position
            if (!n->parent)
                break;
            if (n->parent->right == n)
                codeword |= (1 << position);

            //Move position
            position--;

            //Move to parent
            n = n->parent;
        } while (n->parent != NULL);

        //Store the codeword in the codebook
        codebook->codes[i] = 0;
        for (uint8_t j = 31, k = 0; j > 31 - codeword_length; j--)
            codebook->codes[i] |= (((codeword & (1 << j)) >> j) << k++);
        codebook->codes_length[i] = codeword_length;
        codebook->codes_value[i] = nodes[i]->id;
    }
}

void Huffman::create_canonical_codebook()
{
    //Sort the codebook ascending by codeword length and then by codeword value
    Sort::sort_huffman_codebook_by_length_then_by_value_ascending(codebook);

    //All codeword lengths stay the same
    //Set the first symbol to 0
    codebook->codes[0] = 0;
    for (uint32_t i = 1; i < codebook->size; i++)
    {
        //For each symbol starting from the second one, assign next binary number
        //If the codeword is longer than the previous one, append zeros (left shift)
        codebook->codes[i] = (codebook->codes[i - 1] + 1)
                             << (codebook->codes_length[i] - codebook->codes_length[i - 1]);
    }
}

void Huffman::read_canonical_codebook(std::string file)
{
    //Prepare input stream
    std::ifstream fs(file, std::ifstream::binary);
    char* buffer = new char[BUFFER_SIZE];

    if (!fs.good())
    {
        std::cout << "Input file error"
                  << "\n";
        return;
    }

    //Read file header
    fs.read(buffer, 8);
    compressed_data_starts_at += 8;

    uint16_t dictionary_size = ((uint16_t)buffer[0] & 0xFF) + 1;

    original_file_size = 0;
    original_file_size |= (((uint64_t)buffer[1] & 0xFF) << 48);
    original_file_size |= (((uint64_t)buffer[2] & 0xFF) << 40);
    original_file_size |= (((uint64_t)buffer[3] & 0xFF) << 32);
    original_file_size |= (((uint64_t)buffer[4] & 0xFF) << 24);
    original_file_size |= (((uint64_t)buffer[5] & 0xFF) << 16);
    original_file_size |= (((uint64_t)buffer[6] & 0xFF) << 8);
    original_file_size |= (((uint64_t)buffer[7] & 0xFF) << 0);

    //Recreate codebook
    codebook = new Huffman_Codebook(dictionary_size);

    //Read number of symbols with codeword length of 1...n
    uint16_t dictionary_size_counter = 0;
    uint16_t* number_of_symbols_with_codeword_length = new uint16_t[32];
    for (uint8_t i = 0; i < 32; i++)
        number_of_symbols_with_codeword_length[i] = 0;
    for (uint8_t i = 0; i < 32; i++)
    {
        fs.read(buffer, 1);
        compressed_data_starts_at++;
        number_of_symbols_with_codeword_length[i] = (buffer[0] & 0xFF);
        dictionary_size_counter += (buffer[0] & 0xFF);
        if (dictionary_size_counter == dictionary_size)
        {
            dictionary_size_counter = i;
            break;
        }
    }
    if (!dictionary_size_counter)
        number_of_symbols_with_codeword_length[7] = 256;

    //Set codeword lengths
    for (uint16_t i = 0; i < dictionary_size; i++)
    {
        for (uint8_t j = 0; j < 32; j++)
        {
            if (number_of_symbols_with_codeword_length[j])
            {
                number_of_symbols_with_codeword_length[j]--;
                codebook->codes_length[i] = j + 1;
                break;
            }
        }
    }

    //Set the first symbol to 0
    codebook->codes[0] = 0;
    for (uint32_t i = 1; i < codebook->size; i++)
    {
        //For each symbol starting from the second one, assign next binary number
        //If the codeword is longer than the previous one, append zeros (left shift)
        codebook->codes[i] = (codebook->codes[i - 1] + 1)
                             << (codebook->codes_length[i] - codebook->codes_length[i - 1]);
    }

    //Read codeword values
    fs.read(buffer, dictionary_size);
    compressed_data_starts_at += dictionary_size;
    for (uint16_t i = 0; i < dictionary_size; i++)
        codebook->codes_value[i] = buffer[i];

    delete[] buffer;
    delete[] number_of_symbols_with_codeword_length;
}

void Huffman::compress(std::string file_in, std::string file_out) {
    // Prepare input and output streams
    std::ifstream fs_in(file_in, std::ifstream::binary);
    std::ofstream fs_out(file_out, std::ofstream::binary);

    // Check if files are opened correctly
    if (!fs_in.is_open() || !fs_out.is_open()) {
        std::cerr << "Erro ao abrir arquivos." << std::endl;
        return;
    }

    // Get the size of the file
    fs_in.seekg(0, std::ios::end);
    size_t fileSize = fs_in.tellg();
    fs_in.seekg(0, std::ios::beg);

    // Allocate memory for the entire file
    char* buffer = (char*)malloc(fileSize);
    if (buffer == NULL) {
        std::cerr << "Falha na alocação de memória." << std::endl;
        return;
    }

    // Read the entire file into memory
    fs_in.read(buffer, fileSize);

    //Prepare file header
    uint8_t dictionary_size = codebook->size & 0xFF;

    original_file_size = 0;
    #pragma omp parallel for reduction(+:original_file_size)
    for (uint16_t i = 0; i < 256; i++)
    {
        if (i == 0)
        {
            int num_threads = omp_get_num_threads();
            std::cout << "Number of threads = " << num_threads << std::endl;
        }

        if (nodes[i]->occurrences)
            original_file_size += nodes[i]->occurrences;
    }

    // Count number of symbols with codeword length
    uint8_t* number_of_symbols_with_codeword_length = new uint8_t[32];
    for (uint8_t i = 0; i < 32; i++)
        number_of_symbols_with_codeword_length[i] = 0;
    #pragma omp parallel for // openmp
    for (uint16_t i = 0; i < codebook->size; i++)
        number_of_symbols_with_codeword_length[codebook->codes_length[i] - 1]++;


    uint8_t number_of_symbols_with_codeword_length_highest = 31;
    for (int8_t i = 31; i >= 0; i--)
    {
        if (number_of_symbols_with_codeword_length[i])
        {
            number_of_symbols_with_codeword_length_highest = i;
            break;
        }
    }

    //Write file header
    fs_out << (uint8_t)(dictionary_size - 1);

    fs_out << (uint8_t)((original_file_size >> 48) & 0xFF);
    fs_out << (uint8_t)((original_file_size >> 40) & 0xFF);
    fs_out << (uint8_t)((original_file_size >> 32) & 0xFF);
    fs_out << (uint8_t)((original_file_size >> 24) & 0xFF);
    fs_out << (uint8_t)((original_file_size >> 16) & 0xFF);
    fs_out << (uint8_t)((original_file_size >> 8) & 0xFF);
    fs_out << (uint8_t)((original_file_size >> 0) & 0xFF);

    for (uint8_t i = 0; i <= number_of_symbols_with_codeword_length_highest; i++)
        fs_out << (number_of_symbols_with_codeword_length[i]);

    for (uint16_t i = 0; i < codebook->size; i++)
        fs_out << codebook->codes_value[i];

    printf("Tentando alocar o outputbuffer\n");

    // Allocate output buffer
    uint8_t* outputBuffer = (uint8_t*)malloc(fileSize);
    if (outputBuffer == NULL) {
        std::cerr << "Falha na alocação de memória para outputBuffer." << std::endl;
        free(buffer);
        return;
    }

    // Initialize outputBuffer to 0
    memset(outputBuffer, 0, fileSize);

    printf("output buffer inicializado com zeros \n");

    printf("file_size: %ld\n", fileSize);
    printf("omp max threads: %d\n", omp_get_max_threads());
    // Process the file in parallel
    #pragma omp parallel
    {
        int threadId = omp_get_thread_num();
        size_t segmentSize = fileSize / omp_get_max_threads();
        size_t start = threadId * segmentSize;
        size_t end = (threadId == omp_get_max_threads() - 1) ? fileSize : start + segmentSize;

        for (size_t i = start; i < end; i++) {
            uint8_t data_out = 0;
            int8_t data_out_position = 7;

            // For each byte in buffer, run through codebook
            for (uint16_t j = 0; j < codebook->size; j++) {
                // If current byte from buffer matches this codebook record, write codeword
                if (codebook->codes_value[j] == (uint8_t)buffer[i]) {
                    for (int8_t k = codebook->codes_length[j] - 1; k >= 0; k--) {
                        data_out |= (((codebook->codes[j] >> k) & 0x01) << data_out_position--);
                        if (data_out_position < 0) {
                            outputBuffer[i] = data_out; // Escreva no buffer de saída ao invés de fs_out
                            data_out = 0;
                            data_out_position = 7;
                        }
                    }
                    break;
                }
            }

        }
    }

    // Write outputBuffer to the output file
    for (size_t i = 0; i < fileSize; i++) {
        if (outputBuffer[i] != 0) {
            fs_out.write(reinterpret_cast<const char*>(&outputBuffer[i]), sizeof(uint8_t));
        }
    }

    // Clean up
    free(buffer);
    free(outputBuffer);
}

void Huffman::decompress(std::string file_in, std::string file_out)
{
    //Prepare input and output streams
    std::ifstream fs_in(file_in, std::ifstream::binary);
    char* buffer = new char[BUFFER_SIZE];
    std::ofstream fs_out(file_out, std::ofstream::binary);

    //Skip to data
    fs_in.read(buffer, compressed_data_starts_at);

    //Decompress the input file
    uint32_t codeword = 0;
    uint8_t codeword_length = 0;
    uint64_t bytes_decompressed = 0;
    while (fs_in.good())
    {
        fs_in.read(buffer, BUFFER_SIZE);
        int32_t bytes_read = fs_in.gcount();

        for (int32_t i = 0; i < bytes_read; i++)
        {
            for (int8_t j = 7; j >= 0; j--)
            {
                codeword <<= 1;
                codeword |= (((uint32_t)buffer[i] >> j) & 0x01);
                codeword_length++;
                for (uint16_t k = 0; k < codebook->size; k++)
                {
                    if (codebook->codes[k] == codeword &&
                        codebook->codes_length[k] == codeword_length)
                    {
                        fs_out << codebook->codes_value[k];
                        codeword = 0;
                        codeword_length = 0;
                        bytes_decompressed++;
                        if (bytes_decompressed == original_file_size)
                        {
                            k = codebook->size;
                            j = -1;
                            i = bytes_read;
                        }
                    }
                }
            }
        }
    }

    delete[] buffer;
}

void Huffman::compress_file(std::string filename_in, std::string filename_out)
{
    char* measure_time = getenv("MEASURE");
    double start(0), end(0);
    if (measure_time != nullptr && strcmp(measure_time, "1") == 0) {
        start = omp_get_wtime();
    }

    count_occurrences(filename_in);
    sort_nodes();
    construct_tree();
    create_codebook();
    create_canonical_codebook();
    compress(filename_in, filename_out);

    if (measure_time != nullptr && strcmp(measure_time, "1") == 0) {
        end = omp_get_wtime();
        printf("Tempo gasto: %f segundos\n", end - start);
    }
}

void Huffman::decompress_file(std::string filename_in, std::string filename_out)
{
    read_canonical_codebook(filename_in);
    decompress(filename_in, filename_out);
}

Huffman::Huffman()
{
    this->nodes = new Huffman_Node*[256];
    for (uint16_t i = 0; i < 256; i++)
        this->nodes[i] = new Huffman_Node(i, true);
    this->leaf_nodes = 0;
    this->compressed_data_starts_at = 0;
    this->original_file_size = 0;
    this->tree = NULL;
    this->codebook = NULL;
}

Huffman::~Huffman()
{
    for (uint16_t i = 0; i < 256; i++)
        if (!nodes[i]->occurrences)
            delete nodes[i];
    delete tree;
    delete[] nodes;
    delete codebook;
}
