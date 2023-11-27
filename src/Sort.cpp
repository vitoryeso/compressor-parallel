#include "Sort.h"

#include "Huffman_Codebook.h"

#include <algorithm>

void Sort::sort_nodes_by_occurrences(Huffman_Node** a, uint16_t size)
{
    for (uint16_t i = 0; i < size - 1; i++)
        for (uint16_t j = 0; j < size - i - 1; j++)
            if (a[j]->occurrences < a[j + 1]->occurrences)
                std::swap(a[j], a[j + 1]);
}

void Sort::sort_huffman_codebook_by_length_then_by_value_ascending(Huffman_Codebook* hc)
{
    for (uint16_t i = 0; i < hc->size - 1; i++)
        for (uint16_t j = 0; j < hc->size - i - 1; j++)
            if (hc->codes_length[j] > hc->codes_length[j + 1])
            {
                std::swap(hc->codes[j], hc->codes[j + 1]);
                std::swap(hc->codes_length[j], hc->codes_length[j + 1]);
                std::swap(hc->codes_value[j], hc->codes_value[j + 1]);
            }
            else if (hc->codes_length[j] == hc->codes_length[j + 1])
                if (hc->codes_value[j] > hc->codes_value[j + 1])
                {
                    std::swap(hc->codes[j], hc->codes[j + 1]);
                    std::swap(hc->codes_length[j], hc->codes_length[j + 1]);
                    std::swap(hc->codes_value[j], hc->codes_value[j + 1]);
                }
}
