#include "LZW.h"

#include "LZW_Node.h"

#include <fstream>
#include <unordered_map>
#include <vector>

void LZW::compress_file(std::string file_in, std::string file_out)
{
    std::ifstream fs_in(file_in, std::ifstream::binary);
    std::ofstream fs_out(file_out, std::ofstream::binary);

    bool input_file_end_reached = false;
    bool last_index_to_write = false;

    std::unordered_map<std::vector<uint8_t>, uint32_t, HashAlgorithm> dictionary;

    //Initialize dictionary
    for (dictionary_index = 0; dictionary_index < 256; dictionary_index++)
        dictionary.insert(std::pair<std::vector<uint8_t>, uint32_t>({(uint8_t)dictionary_index},
                                                                    dictionary_index));

    uint8_t data_out = 0;
    int8_t data_out_position = 7;

    //Read the first chunk of file
    if (read_input_file(fs_in, BUFFER_SIZE - 1))
        input_file_end_reached = true;

    while (buffer_bytes_available() > 0)
    {
        //Find the entry that is not yet present in dictionary
        std::vector<uint8_t> current_data;
        int32_t index;
        int32_t last_found_index = -1;

        do
        {
            index = -1;

            if (buffer_bytes_available() == 0)
            {
                last_index_to_write = true;
                break;
            }

            current_data.push_back(read_buffer());

            std::unordered_map<std::vector<uint8_t>, uint32_t, HashAlgorithm>::iterator it =
                dictionary.find(current_data);
            if (it != dictionary.end())
                index = last_found_index = it->second;
        } while (index != -1);

        //Last byte of new entry is not encoded by last_found_index
        if (buffer_bytes_available() > 0 && !last_index_to_write)
            unread_buffer();
        else
        {
            if (!last_index_to_write)
                unread_buffer();
            last_index_to_write = true;
        }

        //Write output
        for (int8_t i = bits_per_dictionary_id() - 1; i >= 0; i--)
        {
            data_out |= (((last_found_index >> i) & 0x01) << (data_out_position--));

            if (data_out_position < 0)
            {
                fs_out << data_out;
                data_out = 0;
                data_out_position = 7;
            }
        }

        //Add new entry to dictionary
        dictionary.insert(
            std::pair<std::vector<uint8_t>, uint32_t>(current_data, dictionary_index++));

        //Read next data chunk
        if (!input_file_end_reached)
            if (read_input_file(fs_in, BUFFER_SIZE - buffer_bytes_available() - 1))
                input_file_end_reached = true;
    }

    //If there is any leftover data, write it to the output file
    if (data_out_position != 7)
        fs_out << data_out;
}

void LZW::decompress_file(std::string file_in, std::string file_out)
{
    std::ifstream fs_in(file_in, std::ifstream::binary);
    std::ofstream fs_out(file_out, std::ofstream::binary);

    std::vector<LZW_Node*> dictionary;

    //Initialize dictionary
    for (dictionary_index = 0; dictionary_index < 256; dictionary_index++)
    {
        LZW_Node* node = new LZW_Node(dictionary_index);
        node->token.push_back(dictionary_index);
        dictionary.push_back(node);
    }

    bool input_file_end_reached = false;
    bool is_data_valid = true;

    //Read the first chunk of file
    if (read_input_file(fs_in, BUFFER_SIZE - 1))
        input_file_end_reached = true;

    uint8_t buf = read_buffer();
    uint8_t bits_read = 0;
    uint32_t data_read;

    while (buffer_bytes_available() > 0 || bits_read < 8)
    {
        data_read = 0;

        for (int8_t i = bits_per_dictionary_id() - 1; i >= 0; i--)
        {
            data_read <<= 1;
            data_read |= ((buf >> (7 - bits_read)) & 0x01);
            bits_read++;

            if (bits_read == 8)
            {
                if (buffer_bytes_available() == 0)
                {
                    if (i > 0)
                        is_data_valid = false;
                    break;
                }
                buf = read_buffer();
                bits_read = 0;
            }
        }

        if (!is_data_valid)
            break;

        //Skip on first pass of the loop
        if (dictionary_index > 256)
            //Complete previous dictionary entry
            dictionary[dictionary_index - 1]->token.push_back(dictionary[data_read]->token[0]);

        //Create new incomplete dictionary entry
        LZW_Node* entry = new LZW_Node(dictionary_index++);
        entry->token = dictionary[data_read]->token;
        dictionary.push_back(entry);

        //Write data encoded by index
        for (uint32_t i = 0; i < dictionary[data_read]->token.size(); i++)
            fs_out << dictionary[data_read]->token[i];

        //Read next data chunk
        if (!input_file_end_reached)
            if (read_input_file(fs_in, BUFFER_SIZE - buffer_bytes_available() - 1))
                input_file_end_reached = true;
    }

    for (uint32_t i = 0; i < dictionary.size(); i++)
        delete dictionary[i];
}

int32_t LZW::read_input_file(std::ifstream& fs_in, uint32_t length)
{
    //Case: reading to the middle of the buffer
    if (write_pointer + length < BUFFER_SIZE)
    {
        fs_in.read(buffer + write_pointer, length);

        int32_t bytes_read = fs_in.gcount();
        write_pointer += bytes_read;

        return length - bytes_read;
    }

    //Case: reading to the end of the buffer
    if (write_pointer + length == BUFFER_SIZE)
    {
        fs_in.read(buffer + write_pointer, length);

        int32_t bytes_read = fs_in.gcount();
        write_pointer += bytes_read;
        write_pointer -= (write_pointer >= BUFFER_SIZE) * BUFFER_SIZE;

        return length - bytes_read;
    }

    //Case: reading with crossing the end of the buffer
    {
        fs_in.read(buffer + write_pointer, BUFFER_SIZE - write_pointer);

        int32_t bytes_read = fs_in.gcount();
        write_pointer += bytes_read;
        write_pointer -= (write_pointer >= BUFFER_SIZE) * BUFFER_SIZE;

        if (write_pointer != 0)
            return bytes_read;

        fs_in.read(buffer, length - bytes_read);

        int32_t bytes_read_2 = fs_in.gcount();
        write_pointer += bytes_read_2;
        write_pointer -= (write_pointer >= BUFFER_SIZE) * BUFFER_SIZE;

        return length - (bytes_read + bytes_read_2);
    }
}

char LZW::read_buffer()
{
    char buf = buffer[read_pointer++];
    read_pointer %= BUFFER_SIZE;
    return buf;
}

void LZW::unread_buffer()
{
    read_pointer += BUFFER_SIZE - 1;
    read_pointer %= BUFFER_SIZE;
}

uint32_t LZW::buffer_bytes_available()
{
    //Safe if (BUFFER_SIZE + write_pointer - read_pointer) < 2 * BUFFER_SIZE
    return (BUFFER_SIZE + write_pointer - read_pointer) -
           ((BUFFER_SIZE + write_pointer - read_pointer) >= BUFFER_SIZE) * BUFFER_SIZE;
}

uint8_t LZW::bits_per_dictionary_id()
{
    uint32_t index = dictionary_index;
    uint32_t result = 1;

    while (index >>= 1)
        result++;

    return result;
}

LZW::LZW()
{
    buffer = new char[BUFFER_SIZE];
    write_pointer = 0;
    read_pointer = 0;
}

LZW::~LZW()
{
    delete[] buffer;
}
