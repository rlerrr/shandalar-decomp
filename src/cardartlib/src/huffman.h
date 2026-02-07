#ifndef HUFFMAN_H
#define HUFFMAN_H

int Huffman13_Init(undefined4 bitstream_start, undefined4 symbol_table, undefined4 node_index_base);
int Huffman13_DecodeDwordsWithZeroRuns(undefined8 *out_dwords, uint *bitstream, undefined4 bitstream_end);

#endif