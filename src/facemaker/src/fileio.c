#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <string.h>
#include "cardartlib/src/assert.h"
#include "drawcardlib/src/pic.h"
#include "drawcardlib/src/pcxw.h"

extern void WriteGraphicsScanline(unsigned int *scanline_data, int page_number, int dst_x, int dst_y,
                                  unsigned int byte_count);
extern void ReadGraphicsScanline(unsigned int *out_scanline, int page_number, int src_x, int src_y,
                                 unsigned int byte_count);
extern unsigned char gPcxScanlineBuffer[0x1000];
extern void RpBits_Setup(int fileDescriptor);
extern void RpBits_ReadTables(unsigned short *palette);
extern void RpBits_DecodeImage(void *dst, int count);
extern char g_file_read_mode[];
extern PALETTEENTRY g_palette_entries[256];
extern DIBSurface *g_graphics_pages[10];

void LoadPcxResource(int page_number, int x, int y, char *path, void *opaque);

typedef void(__cdecl *EncodeRpBitsImage_Callback)(unsigned int *scanline, int page_number, int x, int y,
                                                  unsigned int width);

#pragma intrinsic(memset)
#pragma intrinsic(memcpy)
#pragma optimize("gy", on)

// GLOBAL: FACEMAKER 0x0040d29c
char s_D__NewMagic__sources__sidlib__Fileio_c_0040d29c[] = "D:\\NewMagic\\sources\\sidlib\\Fileio.c";

// GLOBAL: FACEMAKER 0x0040d2c0
char s_Error_Opening_File__s_0040d2c0[] = "Error Opening File %s\r\n";

// GLOBAL: FACEMAKER 0x0040d1b8
// GLOBAL: SHANDALAR 0x005a1738
unsigned char g_rpbits_palette_magic[5] = {'M', '1', 2, 3, 0};

// GLOBAL: FACEMAKER 0x0041aa08
// GLOBAL: SHANDALAR 0x0073b880
int g_export_write_palette;

typedef struct RpBitsOutputHeader
{
  unsigned short signature;
  unsigned short payload_size;
  unsigned short width;
  unsigned short height;
} RpBitsOutputHeader;

// GLOBAL: FACEMAKER 0x0041a9f0
// GLOBAL: SHANDALAR 0x0073b868
int g_rpbits_bit_accumulator;

// GLOBAL: FACEMAKER 0x0041a9f4
// GLOBAL: SHANDALAR 0x0073b86c
int g_rpbits_code_bits;

// GLOBAL: FACEMAKER 0x0041a9f8
// GLOBAL: SHANDALAR 0x0073b870
int g_rpbits_bit_count;

// GLOBAL: FACEMAKER 0x0041aa00
// GLOBAL: SHANDALAR 0x0073b878
RpBitsOutputHeader g_rpbits_header;

// GLOBAL: FACEMAKER 0x0041aa0c
// GLOBAL: SHANDALAR 0x0073b884
int g_rpbits_output_fd;

// GLOBAL: FACEMAKER 0x0041aa10
// GLOBAL: SHANDALAR 0x0073b888
int g_rpbits_symbol_length;

// GLOBAL: FACEMAKER 0x0041aa14
int g_rpbits_prev_code;

// GLOBAL: FACEMAKER 0x0041aa18
// GLOBAL: SHANDALAR 0x0073b890
int g_rpbits_output_bytes;

// GLOBAL: FACEMAKER 0x0041aa1c
// GLOBAL: SHANDALAR 0x0073b894
int g_rpbits_dict_table;

// GLOBAL: FACEMAKER 0x0041aa20
// GLOBAL: SHANDALAR 0x0073b898
int g_rpbits_max_symbol_len;

// GLOBAL: FACEMAKER 0x0041aa24
// GLOBAL: SHANDALAR 0x0073b89c
int g_rpbits_next_code;

// GLOBAL: FACEMAKER 0x0041aa28
unsigned char g_rpbits_scanline_buffer[0x718];

// GLOBAL: FACEMAKER 0x0041b140
unsigned char g_rpbits_symbol_buffer[0x1000];

// GLOBAL: FACEMAKER 0x00422120
unsigned char g_rpbits_bitstream_buffer[0x200];

// GLOBAL: FACEMAKER 0x00423330
// GLOBAL: SHANDALAR 0x0097f990
int *g_rpbits_dict_entry_ptr;

void RpBitsEncodeSymbol(int symbol);
void RpBitsResetDictionary(void);
unsigned int RpBitsLookupSymbol(int symbol);
void RpBitsWriteBits(int bit_count, unsigned int bits);

// FUNCTION: SHANDALAR 0x0057a340
// FUNCTION: FACEMAKER 0x00407a00
void WriteRpBitsPalette(int file_handle)
{
  unsigned char palette_bytes[0x320];
  unsigned char *palette_byte_ptr;
  unsigned int *palette_entry_word_ptr;
  unsigned int *next_palette_entry_word_ptr;

  memcpy(palette_bytes, g_rpbits_palette_magic, 5);
  memset(&palette_bytes[13], 0, 0x31b);
  palette_bytes[12] = 0;
  palette_bytes[13] = 0xff;

  palette_byte_ptr = &palette_bytes[14];
  palette_entry_word_ptr = (unsigned int *)&g_palette_entries;
  do
  {
    *palette_byte_ptr = *(unsigned char *)palette_entry_word_ptr;
    palette_byte_ptr++;
    palette_byte_ptr++;
    next_palette_entry_word_ptr = palette_entry_word_ptr + 1;
    *(palette_byte_ptr - 1) = *(unsigned char *)((int)next_palette_entry_word_ptr - 3);
    *palette_byte_ptr = *(unsigned char *)((int)next_palette_entry_word_ptr - 2);
    palette_byte_ptr = palette_byte_ptr + 1;
    palette_entry_word_ptr = next_palette_entry_word_ptr;
  } while (next_palette_entry_word_ptr < (unsigned int *)&g_graphics_pages);

  _write(file_handle, &palette_bytes, 0x306);
}

// FUNCTION: SHANDALAR 0x0057de20
// FUNCTION: FACEMAKER 0x004094b0
int EncodeRpBitsImage(int file_handle, EncodeRpBitsImage_Callback read_scanline, int page_number, int x, int y,
                      int width, int height)
{
  unsigned char *dictionary_ptr;
  unsigned char *scanline_ptr;
  int next_row_index;
  unsigned int run_value;
  unsigned int current_value;
  long seek_offset;
  unsigned short run_length;
  int max_match_len;
  unsigned int run_chunk_len;
  int reached_last_row;
  unsigned char *cursor_ptr;

  g_rpbits_output_fd = file_handle;
  g_rpbits_dict_table = (int)malloc(0xbfd0);
  g_rpbits_max_symbol_len = -1;
  g_rpbits_header.payload_size = 0;
  g_rpbits_bit_count = 0;
  g_rpbits_bit_accumulator = 0;
  g_rpbits_header.width = (unsigned short)width;
  g_rpbits_header.height = (unsigned short)height;
  g_rpbits_header.signature = 0x3058;
  g_rpbits_output_bytes = 0;
  max_match_len = 7;
  scanline_ptr = (unsigned char *)&g_rpbits_header;
  do
  {
    RpBitsWriteBits(8, (unsigned int)*scanline_ptr);
    max_match_len = max_match_len - 1;
    scanline_ptr = scanline_ptr + 1;
  } while (max_match_len != -1);

  run_length = 1;
  scanline_ptr = g_rpbits_scanline_buffer + x;
  current_value = 0xffffffff;
  cursor_ptr = scanline_ptr + width;
  do
  {
    run_value = current_value;
    if (scanline_ptr + width <= cursor_ptr)
    {
      reached_last_row = (height == 0);
      height = height - 1;
      if (reached_last_row != 0)
      {
        max_match_len = g_rpbits_max_symbol_len;
        if ((short)current_value == 0x90)
        {
          while (run_length != 0)
          {
            run_length = run_length - 1;
            if (max_match_len == -1)
            {
              g_rpbits_max_symbol_len = 0;
              RpBitsWriteBits(8, 0xb);
              RpBitsResetDictionary();
              g_rpbits_prev_code = 0x90;
              g_rpbits_symbol_buffer[0] = 0x90;
              g_rpbits_symbol_length = 1;
              current_value = g_rpbits_prev_code;
            }
            else
            {
              next_row_index = g_rpbits_symbol_length + 1;
              dictionary_ptr = g_rpbits_symbol_buffer + g_rpbits_symbol_length;
              g_rpbits_symbol_length = next_row_index;
              *dictionary_ptr = 0x90;
              if (max_match_len < next_row_index)
              {
                g_rpbits_max_symbol_len = next_row_index;
              }
              current_value = RpBitsLookupSymbol(0x90);
              if (current_value == 0xffffffff)
              {
                *g_rpbits_dict_entry_ptr = 0x90;
                g_rpbits_dict_entry_ptr[1] = g_rpbits_prev_code;
                g_rpbits_dict_entry_ptr[2] = g_rpbits_next_code;
                g_rpbits_next_code = g_rpbits_next_code + 1;
                RpBitsWriteBits(g_rpbits_code_bits, *(unsigned int *)(g_rpbits_dict_table + 8 + g_rpbits_prev_code * 0xc));
                g_rpbits_symbol_length = 1;
                g_rpbits_prev_code = 0x90;
                g_rpbits_symbol_buffer[0] = 0x90;
                current_value = g_rpbits_prev_code;
                if ((1 << ((unsigned char)g_rpbits_code_bits & 0x1f)) < g_rpbits_next_code)
                {
                  g_rpbits_code_bits = g_rpbits_code_bits + 1;
                  if (0xb < g_rpbits_code_bits)
                  {
                    RpBitsResetDictionary();
                    current_value = g_rpbits_prev_code;
                  }
                }
              }
            }
            g_rpbits_prev_code = current_value;
            RpBitsEncodeSymbol(0);
            max_match_len = g_rpbits_max_symbol_len;
          }
        }
        else
        {
          if (3 < run_length)
          {
            do
            {
              run_chunk_len = (unsigned int)run_length;
              if (0xfe < run_length)
              {
                run_chunk_len = 0xff;
              }
              run_length = run_length - (unsigned short)run_chunk_len;
              RpBitsEncodeSymbol(current_value & 0xffff);
              RpBitsEncodeSymbol(0x90);
              RpBitsEncodeSymbol(run_chunk_len);
            } while (3 < run_length);
          }
          while (run_length != 0)
          {
            run_length = run_length - 1;
            RpBitsEncodeSymbol(current_value & 0xffff);
          }
        }

        RpBitsWriteBits(g_rpbits_code_bits, *(unsigned int *)(g_rpbits_dict_table + 8 + g_rpbits_prev_code * 0xc));
        RpBitsWriteBits(8, 0);
        _write(g_rpbits_output_fd, g_rpbits_bitstream_buffer, g_rpbits_output_bytes);
        g_rpbits_output_bytes = 0;
        seek_offset = _tell(g_rpbits_output_fd);
        g_rpbits_header.payload_size = (unsigned short)seek_offset - 4;
        if (g_export_write_palette == 0)
        {
          seek_offset = 0;
        }
        else
        {
          seek_offset = 0x306;
        }
        _lseek(g_rpbits_output_fd, seek_offset, 0);
        _write(g_rpbits_output_fd, &g_rpbits_header, 8);
        free((void *)g_rpbits_dict_table);
        return _close(g_rpbits_output_fd);
      }

      next_row_index = y + 1;
      read_scanline((unsigned int *)scanline_ptr, page_number, x, y, width);
      y = next_row_index;
      cursor_ptr = scanline_ptr;
      if (current_value == 0xffffffff)
      {
        cursor_ptr = g_rpbits_scanline_buffer + x + 1;
        run_value = (unsigned int)*scanline_ptr;
      }
    }

    current_value = (unsigned int)*cursor_ptr;
    if (run_value == current_value)
    {
      run_length = run_length + 1;
      current_value = run_value;
      cursor_ptr = cursor_ptr + 1;
    }
    else
    {
      max_match_len = g_rpbits_max_symbol_len;
      if ((short)run_value == 0x90)
      {
        while (run_length != 0)
        {
          run_length = run_length - 1;
          if (max_match_len == -1)
          {
            g_rpbits_max_symbol_len = 0;
            RpBitsWriteBits(8, 0xb);
            RpBitsResetDictionary();
            g_rpbits_prev_code = 0x90;
            g_rpbits_symbol_buffer[0] = 0x90;
            g_rpbits_symbol_length = 1;
            run_value = g_rpbits_prev_code;
          }
          else
          {
            next_row_index = g_rpbits_symbol_length + 1;
            dictionary_ptr = g_rpbits_symbol_buffer + g_rpbits_symbol_length;
            g_rpbits_symbol_length = next_row_index;
            *dictionary_ptr = 0x90;
            if (max_match_len < next_row_index)
            {
              g_rpbits_max_symbol_len = next_row_index;
            }
            run_value = RpBitsLookupSymbol(0x90);
            if (run_value == 0xffffffff)
            {
              *g_rpbits_dict_entry_ptr = 0x90;
              g_rpbits_dict_entry_ptr[1] = g_rpbits_prev_code;
              g_rpbits_dict_entry_ptr[2] = g_rpbits_next_code;
              g_rpbits_next_code = g_rpbits_next_code + 1;
              RpBitsWriteBits(g_rpbits_code_bits, *(unsigned int *)(g_rpbits_dict_table + 8 + g_rpbits_prev_code * 0xc));
              g_rpbits_prev_code = 0x90;
              g_rpbits_symbol_buffer[0] = 0x90;
              g_rpbits_symbol_length = 1;
              run_value = g_rpbits_prev_code;
              if ((1 << ((unsigned char)g_rpbits_code_bits & 0x1f)) < g_rpbits_next_code)
              {
                g_rpbits_code_bits = g_rpbits_code_bits + 1;
                if (0xb < g_rpbits_code_bits)
                {
                  RpBitsResetDictionary();
                  run_value = g_rpbits_prev_code;
                }
              }
            }
          }
          g_rpbits_prev_code = run_value;
          RpBitsEncodeSymbol(0);
          max_match_len = g_rpbits_max_symbol_len;
        }
      }
      else
      {
        if (3 < run_length)
        {
          do
          {
            run_chunk_len = (unsigned int)run_length;
            if (0xfe < run_length)
            {
              run_chunk_len = 0xff;
            }
            run_length = run_length - (unsigned short)run_chunk_len;
            RpBitsEncodeSymbol(run_value & 0xffff);
            RpBitsEncodeSymbol(0x90);
            RpBitsEncodeSymbol(run_chunk_len);
          } while (3 < run_length);
        }
        while (run_length != 0)
        {
          run_length = run_length - 1;
          RpBitsEncodeSymbol(run_value & 0xffff);
        }
      }
      run_length = 1;
      cursor_ptr = cursor_ptr + 1;
    }
  } while (1);

  return 0;
}

// FUNCTION: SHANDALAR 0x0057e390
// FUNCTION: FACEMAKER 0x00409a20
void RpBitsEncodeSymbol(int symbol)
{
  unsigned char *dictionary_ptr;
  int next_code_length;
  int table_index;
  unsigned int hash_key;
  unsigned int hash_slot;
  unsigned char code_byte;
  int probe_stride;

  probe_stride = g_rpbits_max_symbol_len;
  code_byte = (unsigned char)symbol;
  if (g_rpbits_max_symbol_len == -1)
  {
    probe_stride = 0;
    g_rpbits_max_symbol_len = 0;
    RpBitsWriteBits(8, 0xb);
    table_index = 0;
    do
    {
      table_index = table_index + 0xc;
      *(int *)(g_rpbits_dict_table + -4 + table_index) = probe_stride;
      *(int *)(g_rpbits_dict_table + -0xc + table_index) = probe_stride;
      probe_stride = probe_stride + 1;
      *(int *)(g_rpbits_dict_table + -8 + table_index) = -1;
    } while (table_index < 0xc00);
    if (probe_stride < 0xffb)
    {
      probe_stride = probe_stride * 0xc;
      do
      {
        probe_stride = probe_stride + 0xc;
        *(int *)(g_rpbits_dict_table + -0xc + probe_stride) = -1;
      } while (probe_stride < 0xbfc4);
    }
    g_rpbits_code_bits = 9;
    g_rpbits_next_code = 0x101;
    g_rpbits_symbol_length = 1;
    g_rpbits_prev_code = symbol;
    g_rpbits_symbol_buffer[0] = code_byte;
    return;
  }

  next_code_length = g_rpbits_symbol_length + 1;
  dictionary_ptr = g_rpbits_symbol_buffer + g_rpbits_symbol_length;
  g_rpbits_symbol_length = next_code_length;
  *dictionary_ptr = code_byte;
  table_index = g_rpbits_symbol_length;
  if (probe_stride < next_code_length)
  {
    g_rpbits_max_symbol_len = next_code_length;
  }
  hash_key = 0;
  g_rpbits_symbol_buffer[g_rpbits_symbol_length] = 0;
  if (0 < table_index)
  {
    probe_stride = 0;
    do
    {
      hash_key = hash_key + (int)*(short *)(g_rpbits_symbol_buffer + probe_stride);
      probe_stride = probe_stride + 2;
    } while (probe_stride < g_rpbits_symbol_length);
  }
  hash_key = hash_key & 0x7fff;
  if (hash_key == 0)
  {
    hash_key = g_rpbits_symbol_length * 0x25 & 0x7fff;
  }
  hash_slot = hash_key % 0xffb;
  probe_stride = -1;
  while (1)
  {
    g_rpbits_dict_entry_ptr = (int *)(g_rpbits_dict_table + hash_slot * 0xc);
    if (*g_rpbits_dict_entry_ptr == -1)
    {
      break;
    }
    if ((symbol == *g_rpbits_dict_entry_ptr) && (g_rpbits_dict_entry_ptr[1] == g_rpbits_prev_code))
    {
      goto MATCH_FOUND;
    }
    if (probe_stride == -1)
    {
      probe_stride = 0xff9 - hash_key % 0xff9;
    }
    if (probe_stride == 0)
    {
      probe_stride = g_rpbits_symbol_length * 0x89;
    }
    hash_slot = (unsigned int)(hash_slot + probe_stride) % 0xffb;
  }
  hash_slot = 0xffffffff;

MATCH_FOUND:
  if (hash_slot != 0xffffffff)
  {
    g_rpbits_prev_code = hash_slot;
    return;
  }

  *g_rpbits_dict_entry_ptr = symbol;
  g_rpbits_dict_entry_ptr[1] = g_rpbits_prev_code;
  g_rpbits_dict_entry_ptr[2] = g_rpbits_next_code;
  g_rpbits_next_code = g_rpbits_next_code + 1;
  RpBitsWriteBits(g_rpbits_code_bits, *(unsigned int *)(g_rpbits_dict_table + 8 + g_rpbits_prev_code * 0xc));
  g_rpbits_prev_code = symbol;
  g_rpbits_symbol_length = 1;
  g_rpbits_symbol_buffer[0] = code_byte;
  if ((1 << ((unsigned char)g_rpbits_code_bits & 0x1f)) < g_rpbits_next_code)
  {
    g_rpbits_code_bits = g_rpbits_code_bits + 1;
    if (0xb < g_rpbits_code_bits)
    {
      RpBitsResetDictionary();
    }
  }
}

// FUNCTION: SHANDALAR 0x0057e630
// FUNCTION: FACEMAKER 0x00409cc0
void RpBitsResetDictionary(void)
{
  int code_index;
  int table_offset;

  code_index = 0;
  table_offset = 0;
  do
  {
    table_offset = table_offset + 0xc;
    *(int *)(g_rpbits_dict_table + -4 + table_offset) = code_index;
    *(int *)(g_rpbits_dict_table + -0xc + table_offset) = code_index;
    code_index = code_index + 1;
    *(int *)(g_rpbits_dict_table + -8 + table_offset) = -1;
  } while (table_offset < 0xc00);
  if (code_index < 0xffb)
  {
    code_index = code_index * 0xc;
    do
    {
      code_index = code_index + 0xc;
      *(int *)(g_rpbits_dict_table + -0xc + code_index) = -1;
    } while (code_index < 0xbfc4);
  }
  g_rpbits_code_bits = 9;
  g_rpbits_next_code = 0x101;
}

// FUNCTION: SHANDALAR 0x0057e6a0
// FUNCTION: FACEMAKER 0x00409d30
unsigned int RpBitsLookupSymbol(int symbol)
{
  int scan_offset;
  unsigned int hash_sum;
  unsigned int hash_slot;
  int probe_stride;

  scan_offset = g_rpbits_symbol_length;
  hash_sum = 0;
  g_rpbits_symbol_buffer[g_rpbits_symbol_length] = 0;
  if (0 < scan_offset)
  {
    scan_offset = 0;
    do
    {
      hash_sum = hash_sum + (int)*(short *)(g_rpbits_symbol_buffer + scan_offset);
      scan_offset = scan_offset + 2;
    } while (scan_offset < g_rpbits_symbol_length);
  }
  hash_sum = hash_sum & 0x7fff;
  if (hash_sum == 0)
  {
    hash_sum = g_rpbits_symbol_length * 0x25 & 0x7fff;
  }

  probe_stride = -1;
  hash_slot = hash_sum % 0xffb;
  while (1)
  {
    g_rpbits_dict_entry_ptr = (int *)(g_rpbits_dict_table + hash_slot * 0xc);
    if (*g_rpbits_dict_entry_ptr == -1)
    {
      return 0xffffffff;
    }
    if ((symbol == *g_rpbits_dict_entry_ptr) && (g_rpbits_dict_entry_ptr[1] == g_rpbits_prev_code))
    {
      break;
    }
    if (probe_stride == -1)
    {
      probe_stride = 0xff9 - hash_sum % 0xff9;
    }
    if (probe_stride == 0)
    {
      probe_stride = g_rpbits_symbol_length * 0x89;
    }
    hash_slot = (unsigned int)(probe_stride + hash_slot) % 0xffb;
  }
  return hash_slot;
}

// FUNCTION: SHANDALAR 0x0057e770
// FUNCTION: FACEMAKER 0x00409e00
void RpBitsWriteBits(int bit_count, unsigned int bits)
{
  int file_handle;
  unsigned int bit_accumulator;

  file_handle = g_rpbits_output_fd;
  while (bit_count != 0)
  {
    bit_count = bit_count - 1;
    bit_accumulator = g_rpbits_bit_accumulator;
    if (7 < g_rpbits_bit_count)
    {
      g_rpbits_bitstream_buffer[g_rpbits_output_bytes] = (unsigned char)g_rpbits_bit_accumulator;
      g_rpbits_output_bytes = g_rpbits_output_bytes + 1;
      g_rpbits_bit_accumulator = 0;
      g_rpbits_bit_count = 0;
      bit_accumulator = 0;
      if (0x1ff < g_rpbits_output_bytes)
      {
        _write(file_handle, g_rpbits_bitstream_buffer, g_rpbits_output_bytes);
        g_rpbits_output_bytes = 0;
        file_handle = g_rpbits_output_fd;
        bit_accumulator = g_rpbits_bit_accumulator;
      }
    }
    g_rpbits_bit_accumulator = (int)bit_accumulator >> 1;
    if ((bits & 1) != 0)
    {
      g_rpbits_bit_accumulator = g_rpbits_bit_accumulator | 0x80;
    }
    bits = (unsigned int)((int)bits >> 1);
    g_rpbits_bit_count = g_rpbits_bit_count + 1;
  }
}

// FUNCTION: SHANDALAR 0x0057dd90
// FUNCTION: FACEMAKER 0x00409420
int ExportEncodedImage(int page_number, int x, int y, int width, int height, int write_palette, char *path)
{
  int result;
  int file_handle;

  if ((file_handle = _open(path, 0x8302, 0x80)) != -1)
  {
    if (write_palette)
    {
      WriteRpBitsPalette(file_handle);
      g_export_write_palette = 1;
    }
    else
    {
      g_export_write_palette = 0;
    }
    result = EncodeRpBitsImage(file_handle, ReadGraphicsScanline, page_number, x, y, width, height);
#ifndef _DEBUG
    _close(file_handle);
#endif
    return result;
  }
}

void LoadPcxResource(int page_number, int x, int y, char *path, void *opaque);

// FUNCTION: FACEMAKER 0x0040a110
// FUNCTION: SHANDALAR 0x0057c800
void LoadPcxIntoPage(int page_number, char *path)
{
#ifdef SHANDALAR
  LoadPcxResource(page_number, 0, 0, path, (void *)0);
#else
  LoadPcxResource(page_number, 0, 0, path, (void *)1);
#endif
}

// FUNCTION: FACEMAKER 0x0040a130
// FUNCTION: SHANDALAR 0x0057c820
void LoadPcxIntoPageNoPalette(
#ifndef SHANDALAR
    int page_number,
#endif
    char *path)
{
#ifdef SHANDALAR
  LoadPcxResource(-1, 0, 0, path, (void *)1);
#else
  LoadPcxResource(page_number, 0, 0, path, (void *)0);
#endif
}

// FUNCTION: SHANDALAR 0x0057c840
// FUNCTION: FACEMAKER 0x0040a150
void LoadPcxPalette(char *path, unsigned short *palette)
{
  LoadPcxResource(-1, 0, 0, path, palette);
}

// FUNCTION: SHANDALAR 0x0057c580
// FUNCTION: FACEMAKER 0x00409eb0
void LoadPcxResource(int page_number, int x, int y, char *path, void *opaque)
{
  char *ext;
  int file_handle;
  int line;
  RpBitsPalettePacket *palette;
  unsigned short palette_buffer[0x200];

  ext = strchr(path, '.');
  if (_stricmp(".pcx", ext) != 0)
  {
    file_handle = _open(path, 0x8000);
    assert((unsigned int)(file_handle != -1), s_D__NewMagic__sources__sidlib__Fileio_c_0040d29c, 0x88,
           s_Error_Opening_File__s_0040d2c0, path);

    RpBits_Setup(file_handle);
    RpBits_ReadTables((unsigned short *)opaque);
    if (page_number < 0)
    {
      global_pcxw_image_height = 0;
    }
    line = 0;
    while (line < global_pcxw_image_height)
    {
      RpBits_DecodeImage(gPcxScanlineBuffer, global_pcxw_image_width);
      WriteGraphicsScanline((unsigned int *)gPcxScanlineBuffer, page_number, x, y + line, global_pcxw_image_width);
      line = line + 1;
    }

    assert((unsigned int)(_close(file_handle) == 0), s_D__NewMagic__sources__sidlib__Fileio_c_0040d29c, 0xa8,
           (char *)0);
    return;
  }

  gPcxInFile = fopen(path, g_file_read_mode);
  assert((unsigned int)(gPcxInFile != (FILE *)0), s_D__NewMagic__sources__sidlib__Fileio_c_0040d29c, 0xf7,
         s_Error_Opening_File__s_0040d2c0, path);
  gPcxPath = path;
  palette = (RpBitsPalettePacket *)opaque;
  if (palette == (RpBitsPalettePacket *)1)
  {
    palette = (RpBitsPalettePacket *)palette_buffer;
  }
  if (palette == (RpBitsPalettePacket *)0)
  {
    PcxReadHeaderAndPalette((void *)0);
  }
  else
  {
    PcxReadHeaderAndPalette((unsigned short *)palette->entry_data);
    palette->signature = (unsigned short)('M' | ('1' << 8));
    palette->block_size = 0x300;
    palette->first_index = '\0';
    palette->last_index = 0xFF;
    RpBits_ApplyPalette(palette);
  }

  if (page_number < 0)
  {
    global_pcxw_image_height = 0;
  }
  line = 0;
  while (line < global_pcxw_image_height)
  {
    PcxReadScanlineRle((char *)gPcxScanlineBuffer);
    WriteGraphicsScanline((unsigned int *)gPcxScanlineBuffer, page_number, x, y + line, global_pcxw_image_width);
    line = line + 1;
  }
  fclose(gPcxInFile);
}
