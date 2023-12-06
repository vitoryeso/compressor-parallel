#include "LZ78.h"

#include <fstream>
#include <iostream>

void LZ78::compress_file(std::string file_in, std::string file_out)
{
    std::ifstream fs_in(file_in, std::ifstream::binary);
    std::ofstream fs_out(file_out, std::ofstream::binary);

    //Create dictionary root entry (empty string)
    LZ78_Node* dictionary = new LZ78_Node(0, 0);

    bool last_tuple_denormalized = false;
    bool input_file_end_reached = false;

    uint8_t data_out = 0;
    int8_t data_out_position = 7;

    //Read the first chunk of file
    if (read_input_file(fs_in, BUFFER_SIZE - 1))
        input_file_end_reached = true;

    while (buffer_bytes_available() > 0)
    {
        LZ78_Tuple* tuple = build_tuple(dictionary, &last_tuple_denormalized);

        //Read next data chunk
        if (!input_file_end_reached)
            if (read_input_file(fs_in, BUFFER_SIZE - buffer_bytes_available() - 1))
                input_file_end_reached = true;

        for (int8_t i = bits_per_dictionary_id() - 1; i >= 0; i--)
        {
            data_out |= (((tuple->id >> i) & 0x01) << (data_out_position--));

            if (data_out_position < 0)
            {
                fs_out << data_out;
                data_out = 0;
                data_out_position = 7;
            }
        }

        if (!last_tuple_denormalized)
        {
            for (int8_t i = 7; i >= 0; i--)
            {
                data_out |= (((tuple->token >> i) & 0x01) << (data_out_position--));

                if (data_out_position < 0)
                {
                    fs_out << data_out;
                    data_out = 0;
                    data_out_position = 7;
                }
            }
        }

        delete tuple;
    }

    //If there is any leftover data, write it to the output file
    if (data_out_position != 7)
        fs_out << data_out;

    //Cleanup
    delete dictionary;
}

LZ78_Tuple* LZ78::build_tuple(LZ78_Node* current_node, bool* denormalized)
{
    //Denormalized tuple
    if (buffer_bytes_available() == 0)
    {
        *denormalized = true;
        dictionary_index++;
        return new LZ78_Tuple(current_node->id, 0);
    }

    uint8_t buf = read_buffer();

    LZ78_Node* found_node = LZ78_Node::find_node_with_token(current_node, buf);

    if (found_node == NULL)
    {
        //Create node
        LZ78_Node* node = new LZ78_Node(++dictionary_index, buf);
        current_node->leaves.push_back(node);
        return new LZ78_Tuple(current_node->id, node->token);
    }
    else
    {
        //Found node, try to dig deeper
        return build_tuple(found_node, denormalized);
    }
}

void LZ78::decompress_file(std::string file_in, std::string file_out)
{
    std::ifstream fs_in(file_in, std::ifstream::binary);
    std::ofstream fs_out(file_out, std::ofstream::binary);

    //Create dictionary root entry (empty string)
    LZ78_Node* dictionary = new LZ78_Node(0, 0);

    bool last_tuple_denormalized = false;
    bool input_file_end_reached = false;

    //Read the first chunk of file
    if (read_input_file(fs_in, BUFFER_SIZE - 1))
        input_file_end_reached = true;

    uint8_t buf = read_buffer();
    uint8_t bits_read = 0;

    dictionary_index = 1;

    while (buffer_bytes_available() > 0 || bits_read < 8)
    {
        LZ78_Tuple* tuple = new LZ78_Tuple(0, 0);

        for (int8_t i = bits_per_dictionary_id() - 1; i >= 0; i--)
        {
            tuple->id <<= 1;
            tuple->id |= ((buf >> (7 - bits_read)) & 0x01);
            bits_read++;

            if (bits_read == 8)
            {
                if (buffer_bytes_available() == 0)
                {
                    last_tuple_denormalized = true;
                    break;
                }
                buf = read_buffer();
                bits_read = 0;
            }
        }
        for (int8_t i = 7; i >= 0; i--)
        {
            tuple->token <<= 1;
            tuple->token |= ((buf >> (7 - bits_read)) & 0x01);
            bits_read++;

            if (bits_read == 8)
            {
                if (buffer_bytes_available() == 0)
                {
                    if (i > 0)
                        last_tuple_denormalized = true;
                    break;
                }
                buf = read_buffer();
                bits_read = 0;
            }
        }

        //Traverse dictionary depth-first, saving the path; append new node to found entry
        std::vector<uint8_t> path;
        find_dictionary_entry(tuple->id, dictionary, &path)
            ->leaves.push_back(new LZ78_Node(dictionary_index++, tuple->token));

        //Append path and token to output
        for (uint64_t i = 0; i < path.size(); i++)
            fs_out << path[i];
        if (!last_tuple_denormalized)
            fs_out << tuple->token;

        //Read next data chunk
        if (!input_file_end_reached)
            if (read_input_file(fs_in, BUFFER_SIZE - buffer_bytes_available() - 1))
                input_file_end_reached = true;

        delete tuple;
    }

    //Cleanup
    delete dictionary;
}

int32_t LZ78::read_input_file(std::ifstream& fs_in, uint32_t length)
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

char LZ78::read_buffer()
{
    char buf = buffer[read_pointer++];
    read_pointer %= BUFFER_SIZE;
    return buf;
}

uint32_t LZ78::buffer_bytes_available()
{
    //Safe if (BUFFER_SIZE + write_pointer - read_pointer) < 2 * BUFFER_SIZE
    return (BUFFER_SIZE + write_pointer - read_pointer) -
           ((BUFFER_SIZE + write_pointer - read_pointer) >= BUFFER_SIZE) * BUFFER_SIZE;
}

LZ78_Node* LZ78::find_dictionary_entry(uint32_t id, LZ78_Node* dictionary,
                                       std::vector<uint8_t>* path)
{
    //Traverse dictionary depth-first, saving the path; return when id is found
    if (dictionary->id == id)
    {
        if (dictionary->id != 0)
            path->push_back(dictionary->token);
        return dictionary;
    }

    for (uint16_t i = 0; i < dictionary->leaves.size(); i++)
    {
        if (dictionary->id != 0)
            path->push_back(dictionary->token);
        LZ78_Node* entry = find_dictionary_entry(id, dictionary->leaves[i], path);
        if (entry != NULL)
            return entry;
        else if (dictionary->id != 0)
            path->pop_back();
    }

    return NULL;
}

uint8_t LZ78::bits_per_dictionary_id()
{
    uint32_t index = dictionary_index;
    uint32_t result = 1;

    while (index >>= 1)
        result++;

    return result;
}

LZ78::LZ78()
{
    buffer = new char[BUFFER_SIZE];
    write_pointer = 0;
    read_pointer = 0;
    dictionary_index = 0;
}

LZ78::~LZ78()
{
    delete[] buffer;
}
