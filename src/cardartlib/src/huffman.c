#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include "inttypes.h"
#include "mystdbool.h"
#include "defs.h"
#include "catalog.h"
#include "haar.h"
#include "assert.h"

static uint BitStream_ReadBits(uint bit_count);
static uint BitStream_ReadBit(void);
int Huffman13_BuildDecodeTable(int node_count);

// GLOBAL: CARDARTLIB 0x10021a88
// GLOBAL: DRAWCARDLIB 0x10029fb0
uint *DAT_10021a88;

// GLOBAL: CARDARTLIB 0x10021a8c
// GLOBAL: DRAWCARDLIB 0x10029fb4
int DAT_10021a8c;

// GLOBAL: CARDARTLIB 0x10021a90
// GLOBAL: DRAWCARDLIB 0x10029fb8
int DAT_10021a90;

// GLOBAL: CARDARTLIB 0x1001d1dc
// GLOBAL: DRAWCARDLIB 0x10021ecc
undefined4 DAT_1001d1dc = 0x00000000;

// GLOBAL: CARDARTLIB 0x10020e88
// GLOBAL: DRAWCARDLIB 0x100293b0
unsigned char DAT_10020e88[0x100 * 0xc];

// GLOBAL: CARDARTLIB 0x10020e84
// GLOBAL: DRAWCARDLIB 0x100293ac
undefined4 DAT_10020e84;

// GLOBAL: CARDARTLIB 0x10020e00
// GLOBAL: DRAWCARDLIB 0x10029328
uint DAT_10020e00[0x21];

// GLOBAL: CARDARTLIB 0x10021a94
// GLOBAL: DRAWCARDLIB 0x10029fbc
undefined4 DAT_10021a94;

// GLOBAL: CARDARTLIB 0x10021a98
// GLOBAL: DRAWCARDLIB 0x10029fc0
int DAT_10021a98;

// GLOBAL: CARDARTLIB 0x10021a9c
// GLOBAL: DRAWCARDLIB 0x10029fc4
undefined4 DAT_10021a9c;

// GLOBAL: CARDARTLIB 0x10021aa0
// GLOBAL: DRAWCARDLIB 0x10029fc8
unsigned char DAT_10021aa0[0x100 * 8];

// FUNCTION: CARDARTLIB 0x10001f40
// FUNCTION: DRAWCARDLIB 0x10002ea0
void MemZeroDwords(undefined8 *param_1,uint param_2)
{
  //TODO: I don't think MSVC can generate this??  This gotta be inline asm
  __asm {
    mov edi, dword ptr param_1
    mov ecx, dword ptr param_2
    test edi, 4
    je memzero_aligned
    mov byte ptr [edi], 0
    add edi, 4
    dec ecx
    jle memzero_done
  memzero_aligned:
    push ecx
    shr ecx, 1
    dec ecx
    jl memzero_tail
    fldz
    je memzero_store_last_qword
  memzero_qword_loop:
    fst qword ptr [edi]
    add edi, 8
    dec ecx
    jne memzero_qword_loop
  memzero_store_last_qword:
    fstp qword ptr [edi]
  memzero_tail:
    pop ecx
    and ecx, 1
    je memzero_done
    mov byte ptr [edi], 0
  memzero_done:
  }
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10002457
// FUNCTION: DRAWCARDLIB 0x100033b7
int Huffman13_DecodeDwords(undefined4 *out_dwords,undefined4 bitstream_start,undefined4 bitstream_end)
{
  struct {
    uint bit_count;
    uint new_bits;
    uint bit;
    int local_14;
    int symbol;
    undefined4 *out_base;
    int lookahead;
    int node;
  } s;

  s.local_14 = 0;
  s.out_base = out_dwords;
  DAT_10021a88 = (uint *)bitstream_start;
  DAT_10021a8c = (int)DAT_10021a88;
  DAT_10021a90 = bitstream_end;
  DAT_1001d1dc = 0;
  s.lookahead = (int)BitStream_ReadBits(8);
  while (s.lookahead != -1) {
    if (*(int *)(DAT_10020e88 + s.lookahead * 0xc) < 0x7fffffff) {
      s.bit_count = *(uint *)(DAT_10020e88 + s.lookahead * 0xc + 4);
      *(undefined4 *)out_dwords = *(undefined4 *)(DAT_10020e88 + s.lookahead * 0xc);
      out_dwords = out_dwords + 1;
      s.lookahead = s.lookahead >> (byte)s.bit_count;
      s.new_bits = BitStream_ReadBits(s.bit_count);
      if (s.new_bits != 0xffffffff) {
        s.lookahead = s.lookahead | (int)(s.new_bits << (8 - s.bit_count));
      }
      else {
        s.lookahead = (int)s.new_bits;
      }
    }
    else {
      s.node = *(int *)(DAT_10020e88 + s.lookahead * 0xc + 8);
      while ((s.bit = BitStream_ReadBit()) != 0xffffffff) {
        if (s.bit != 0) {
          s.symbol = *(int *)(DAT_10021aa0 + s.node * 8);
        }
        else {
          s.symbol = *(int *)(DAT_10021aa0 + s.node * 8 + 4);
        }
        s.node = s.symbol - DAT_10021a98;
        if (s.node < 0) {
          *(undefined4 *)out_dwords = *(undefined4 *)(DAT_10021a9c + s.symbol * 4);
          out_dwords = out_dwords + 1;
          break;
        }
      }
      s.lookahead = (int)BitStream_ReadBits(8);
    }
  }
  return (int)out_dwords - (int)s.out_base >> 2;
}

// FUNCTION: CARDARTLIB 0x100025cb
// FUNCTION: DRAWCARDLIB 0x1000352b
int Huffman13_Init(undefined4 bitstream_start,undefined4 symbol_table,undefined4 node_index_base)
{
  struct {
    int result;
    int i;
  } s;

  s.result = 0xd;
  s.i = 0;
  
  for (; s.i < 0x20; s.i++) {
    DAT_10020e00[s.i] = 0xffffffff >> (byte)s.i;
  }

  DAT_10021a88 = (uint *)bitstream_start;
  DAT_10021a8c = (int)DAT_10021a88;
  DAT_10021a90 = 100000;
  DAT_1001d1dc = 0;
  DAT_10020e84 = DAT_1001d1dc;
  DAT_10021a94 = BitStream_ReadBits(0xd);
  for (s.i = 0; (int)DAT_10021a94 > s.i; s.i++) {
    ((uint *)&DAT_10021aa0)[s.i*2] = BitStream_ReadBits(0xd);
    ((uint *)&DAT_10021aa0)[s.i*2 + 1] = BitStream_ReadBits(0xd);
    s.result += 0x1a;
  }
  DAT_10021a98 = node_index_base;
  DAT_10021a9c = symbol_table;
  Huffman13_BuildDecodeTable(DAT_10021a94);

  s.result = (s.result / 8) + ((s.result & 7) != 0);
  return s.result;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x100026e3
// FUNCTION: DRAWCARDLIB 0x10003643
int Huffman13_BuildDecodeTable(int node_count)
{
  struct {
    int prev_depth;   /* -0xfc */
    int bit_i_8;      /* -0xf8 */
    int suffix;       /* -0xf4 */
    int bit_i;        /* -0xf0 */
    int prefix;       /* -0xec */
    int i;            /* -0xe8 */
    int pow2[11];     /* -0xe4..-0xbc */
    int bits[0x20];   /* -0xb8..-0x3c */
    int depth;        /* -0x38 */
    int node_stack[9];/* -0x34..-0x14 */
    int pad_10;       /* -0x10 */
    int pad_0c;       /* -0x0c */
    int sym;          /* -0x08 */
    int node;         /* -0x04 */
  } s;

  s.depth = 0;
  s.pow2[0] = 1;
  s.pow2[1] = 2;
  s.pow2[2] = 4;
  s.pow2[3] = 8;
  s.pow2[4] = 0x10;
  s.pow2[5] = 0x20;
  s.pow2[6] = 0x40;
  s.pow2[7] = 0x80;
  s.pow2[8] = 0x100;
  s.pow2[9] = 0x200;
  s.pow2[10] = 0x400;

  for (s.i = 0; s.i < 0x20; s.i++) {
    s.bits[s.i] = 1;
  }

  s.node = node_count - 1;
  s.node_stack[0] = s.node;

  do {
    s.prev_depth = s.depth;
    s.depth++;

    if (s.bits[s.prev_depth] != 0) {
      s.sym = ((int *)DAT_10021aa0)[s.node * 2];
    } else {
      s.sym = ((int *)DAT_10021aa0)[s.node * 2 + 1];
    }

    s.node = s.sym - DAT_10021a98;
    s.node_stack[s.depth] = s.node;

    if (s.node < 0) {
      s.bit_i = 0;
      s.prefix = 0;
      while (s.depth > s.bit_i) {
        s.prefix |= s.bits[s.bit_i] << (byte)s.bit_i;
        s.bit_i++;
      }

      for (s.i = 0; s.i < s.pow2[8 - s.depth]; s.i++) {
        s.suffix = s.i << (byte)s.depth;
        ((int *)(DAT_10020e88 + 4))[((s.suffix | s.prefix) * 3)] = s.depth;
        ((int *)(DAT_10020e88 + 0))[((s.suffix | s.prefix) * 3)] = ((int *)DAT_10021a9c)[s.sym];
        ((int *)(DAT_10020e88 + 8))[((s.suffix | s.prefix) * 3)] = -1;
      }

      s.bits[s.depth] = 1;
      s.depth--;
      s.bits[s.depth]--;
      s.node = s.node_stack[s.depth];
    } else if (s.depth == 8) {
      s.bit_i_8 = 0;
      s.prefix = 0;
      while (s.depth > s.bit_i_8) {
        s.prefix |= s.bits[s.bit_i_8] << (byte)s.bit_i_8;
        s.bit_i_8++;
      }

      ((int *)(DAT_10020e88 + 0))[(s.prefix * 3)] = 0x7fffffff;
      ((int *)(DAT_10020e88 + 4))[(s.prefix * 3)] = s.depth;
      ((int *)(DAT_10020e88 + 8))[(s.prefix * 3)] = s.node;

      s.bits[s.depth] = 1;
      s.depth--;
      s.bits[s.depth]--;
      s.node = s.node_stack[s.depth];
    } else
      continue;

    for (;s.bits[0] >= 0 && s.bits[s.depth] < 0;) {
      s.bits[s.depth] = 1;
      s.depth--;
      s.bits[s.depth]--;
      s.node = s.node_stack[s.depth];
    }
  } while (s.bits[0] >= 0);

  return 0;
}

// FUNCTION: CARDARTLIB 0x10002cde
// FUNCTION: DRAWCARDLIB 0x10003c3e
int Huffman13_DecodeDwordsWithZeroRuns(undefined8 *out_dwords,uint *bitstream,undefined4 bitstream_end)
{
  struct {
    int run_len10;        /* -0x2c */
    int run_bits;         /* -0x28 */
    int bit_count;        /* -0x24 */
    int bits8;            /* -0x20 */
    int align_bytes;      /* -0x1c */
    int bit;              /* -0x18 */
    int zero;             /* -0x14 */
    int leaf_sym;         /* -0x10 */
    undefined8 *out_base; /* -0x0c */
    int lookahead;        /* -0x08 */
    int tree_node;        /* -0x04 */
  } s;

  s.zero = 0;
  s.out_base = out_dwords;

  DAT_10021a88 = bitstream;
  DAT_10021a8c = (int)DAT_10021a88;
  DAT_10021a90 = bitstream_end;

  if (((uint)bitstream & 3) == 0) {
    DAT_1001d1dc = 0;
    DAT_10020e84 = DAT_1001d1dc;
  } else {
    s.align_bytes = 4 - ((uint)bitstream & 3);
    DAT_1001d1dc = s.align_bytes * 8;
    DAT_10020e84 = 0xffffffffU >> (0x20U - DAT_1001d1dc) & *bitstream;
    *(int *)&DAT_10021a88 += s.align_bytes;
  }

  s.lookahead = (int)BitStream_ReadBits(8);

  while (s.lookahead != -1) {
    if (((int *)DAT_10020e88)[s.lookahead * 3] < 0x7fffffff) {
      s.bit_count = ((int *)(DAT_10020e88 + 4))[s.lookahead * 3];

      if (((int *)DAT_10020e88)[s.lookahead * 3] == (int)0x80000000) {
        s.run_bits = (int)BitStream_ReadBits((uint)(s.bit_count + 2));
        if (s.run_bits < 0) {
          break;
        }

        s.run_bits = (s.lookahead >> (byte)s.bit_count) | (s.run_bits << (8 - s.bit_count));

        MemZeroDwords(out_dwords, (uint)s.run_bits);
        *(int *)&out_dwords += (s.run_bits << 2);
        s.lookahead = (int)BitStream_ReadBits(8);
      }
      else {
        *(int *)out_dwords = ((int *)DAT_10020e88)[s.lookahead * 3];
        out_dwords = (undefined8 *)((int)out_dwords + 4);

        s.lookahead = s.lookahead >> (byte)s.bit_count;
        s.bits8 = (int)BitStream_ReadBits((uint)s.bit_count);
        if (s.bits8 != -1) {        
          s.lookahead |= (s.bits8 << (8 - s.bit_count));
        }
        else
          break;
      }
    }
    else {
      s.tree_node = ((int *)(DAT_10020e88 + 8))[s.lookahead * 3];

      while ((s.bit = (int)BitStream_ReadBit()) != -1) {
        if (s.bit != 0) {
          s.leaf_sym = ((int *)DAT_10021aa0)[s.tree_node * 2];
        } else {
          s.leaf_sym = ((int *)DAT_10021aa0)[s.tree_node * 2 + 1];
        }

        s.tree_node = s.leaf_sym - DAT_10021a98;

        if (s.tree_node < 0) {
          if (s.leaf_sym == 0) {
            s.run_len10 = (int)BitStream_ReadBits(10);
            if (s.run_len10 < 0) {
              break;
            }

            MemZeroDwords(out_dwords, (uint)s.run_len10);
            *(int *)&out_dwords += (s.run_len10 << 2);
          }
          else {
            *(int *)out_dwords = *(int *)(DAT_10021a9c + s.leaf_sym * 4);
            out_dwords = (undefined8 *)((int)out_dwords + 4);
          }
          break;
        }
      }
    
      s.lookahead = (int)BitStream_ReadBits(8);
    }
  }

  (void)s.zero;
  return ((int)out_dwords - (int)s.out_base) >> 2;
}

// FUNCTION: CARDARTLIB 0x10002f70
// FUNCTION: DRAWCARDLIB 0x10003ed0
uint BitStream_ReadBits(uint bit_count)
{
  uint bits_in_buf;
  uint result;

  if (DAT_1001d1dc >= bit_count) {
    result = DAT_10020e00[0x20 - bit_count] & DAT_10020e84;
    DAT_10020e84 = DAT_10020e84 >> (byte)bit_count;
    DAT_1001d1dc = DAT_1001d1dc - bit_count;
    return result;
  } else {
    bits_in_buf = DAT_1001d1dc;
    result = DAT_10020e84;
    bit_count = (int) bit_count - (int)DAT_1001d1dc;

    if ((int)DAT_10021a88 - DAT_10021a8c + 4 >= DAT_10021a90) {
      return 0xffffffff;
    }

    DAT_10020e84 = *DAT_10021a88;
    DAT_10021a88 = DAT_10021a88 + 1;
    result = result | (DAT_10020e00[0x20 - bit_count] & DAT_10020e84) << (byte)bits_in_buf;
    DAT_10020e84 = DAT_10020e84 >> (byte)bit_count;
    DAT_1001d1dc = 0x20 - bit_count;
  }
  return result;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10003060
// FUNCTION: DRAWCARDLIB 0x10003fc0
static uint BitStream_ReadBit(void)
{
  uint result = 0;
  
  if (DAT_1001d1dc == 0) {
    if (DAT_10021a90 <= (int)DAT_10021a88 - DAT_10021a8c) {
      return 0xffffffff;
    }
    DAT_10020e84 = *DAT_10021a88;
    DAT_10021a88 = DAT_10021a88 + 1;
    DAT_1001d1dc = 0x20;
  }
  if ((DAT_10020e84 & 1) != 0)
    result = 1;

  DAT_10020e84 = DAT_10020e84 >> 1;
  DAT_1001d1dc = DAT_1001d1dc + -1;
  return result;
}
