#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <string.h>
#include "cardartlib/src/assert.h"
#include "drawcardlib/src/pic.h"
#include "drawcardlib/src/pcxw.h"

extern void FUN_004075d0(unsigned int *param_1, int param_2, int param_3, int param_4,
                                 unsigned int param_5);
extern void FUN_004076c0(unsigned int *param_1, int param_2, int param_3, int param_4,
                         unsigned int param_5);
extern unsigned char gPcxScanlineBuffer[0x1000];
extern void RpBits_Setup(int fileDescriptor);
extern void RpBits_ReadTables(unsigned short *param_3);
extern void RpBits_DecodeImage(void *dst, int count);
extern void RpBits_ApplyPalette(short *palette_data_words);
extern char DAT_0040d224[];
extern PALETTEENTRY DAT_00426140[256];
extern DIBSurface *DAT_00426540[10];

void FUN_00409eb0(int page_number, int x, int y, char *path, void *opaque);

typedef void(__cdecl *FUN_004094b0_Callback)(unsigned int *param_1, int param_2, int param_3, int param_4,
                                             unsigned int param_5);

#pragma intrinsic(memset)
#pragma optimize("gy", on)

// GLOBAL: FACEMAKER 0x0040d29c
char s_D__NewMagic__sources__sidlib__Fileio_c_0040d29c[] = "D:\\NewMagic\\sources\\sidlib\\Fileio.c";

// GLOBAL: FACEMAKER 0x0040d2c0
char s_Error_Opening_File__s_0040d2c0[] = "Error Opening File %s\r\n";

// GLOBAL: FACEMAKER 0x0040d2d8
char DAT_0040d2d8[] = ".pcx";

// GLOBAL: FACEMAKER 0x0040d1b8
unsigned char DAT_0040d1b8[5] = {'M', '1', 2, 3, 0};

// GLOBAL: FACEMAKER 0x0041aa08
int DAT_0041aa08;

typedef struct RpBitsOutputHeader
{
  unsigned short signature;
  unsigned short payload_size;
  unsigned short width;
  unsigned short height;
} RpBitsOutputHeader;

// GLOBAL: FACEMAKER 0x0041a9f0
int DAT_0041a9f0;

// GLOBAL: FACEMAKER 0x0041a9f4
int DAT_0041a9f4;

// GLOBAL: FACEMAKER 0x0041a9f8
int DAT_0041a9f8;

// GLOBAL: FACEMAKER 0x0041aa00
RpBitsOutputHeader DAT_0041aa00;

// GLOBAL: FACEMAKER 0x0041aa0c
int DAT_0041aa0c;

// GLOBAL: FACEMAKER 0x0041aa10
int DAT_0041aa10;

// GLOBAL: FACEMAKER 0x0041aa14
int DAT_0041aa14;

// GLOBAL: FACEMAKER 0x0041aa18
int DAT_0041aa18;

// GLOBAL: FACEMAKER 0x0041aa1c
int DAT_0041aa1c;

// GLOBAL: FACEMAKER 0x0041aa20
int DAT_0041aa20;

// GLOBAL: FACEMAKER 0x0041aa24
int DAT_0041aa24;

// GLOBAL: FACEMAKER 0x0041aa28
unsigned char DAT_0041aa28[0x718];

// GLOBAL: FACEMAKER 0x0041b140
unsigned char DAT_0041b140[0x1000];

// GLOBAL: FACEMAKER 0x00422120
unsigned char DAT_00422120[0x200];

// GLOBAL: FACEMAKER 0x00423330
int *DAT_00423330;

void FUN_00409a20(int param_1);
void FUN_00409cc0(void);
unsigned int FUN_00409d30(int param_1);
void FUN_00409e00(int param_1, unsigned int param_2);

// FUNCTION: FACEMAKER 0x00407a00
void FUN_00407a00(int file_handle)
{
  unsigned char *palette_byte_ptr;
  unsigned int *palette_entry_word_ptr;
  unsigned int *next_palette_entry_word_ptr;
  struct
  {
    unsigned int palette_header_magic;
    unsigned char palette_header_flags;
    unsigned char palette_bytes[0x31b];
  } palette_file_data;

  palette_file_data.palette_header_magic = *(unsigned int *)DAT_0040d1b8;
  palette_file_data.palette_header_flags = DAT_0040d1b8[4];
  memset(palette_file_data.palette_bytes, 0, sizeof(palette_file_data.palette_bytes));
  palette_file_data.palette_header_flags = 0;
  palette_file_data.palette_bytes[0] = 0xff;

  palette_byte_ptr = palette_file_data.palette_bytes + 1;
  palette_entry_word_ptr = (unsigned int *)&DAT_00426140;
  do
  {
    *palette_byte_ptr = *(unsigned char *)palette_entry_word_ptr;
    palette_byte_ptr = palette_byte_ptr + 2;
    next_palette_entry_word_ptr = palette_entry_word_ptr + 1;
    *(palette_byte_ptr - 1) = *(unsigned char *)((int)next_palette_entry_word_ptr - 3);
    *palette_byte_ptr = *(unsigned char *)((int)next_palette_entry_word_ptr - 2);
    palette_byte_ptr = palette_byte_ptr + 1;
    palette_entry_word_ptr = next_palette_entry_word_ptr;
  } while (next_palette_entry_word_ptr < (unsigned int *)&DAT_00426540);

  _write(file_handle, &palette_file_data, 0x306);
}

// FUNCTION: FACEMAKER 0x004094b0
int FUN_004094b0(int param_1, FUN_004094b0_Callback param_2, int param_3, int param_4, int param_5,
                 int param_6, int param_7)
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

  DAT_0041aa0c = param_1;
  DAT_0041aa1c = (int)malloc(0xbfd0);
  DAT_0041aa20 = -1;
  DAT_0041aa00.payload_size = 0;
  DAT_0041a9f8 = 0;
  DAT_0041a9f0 = 0;
  DAT_0041aa00.width = (unsigned short)param_6;
  DAT_0041aa00.height = (unsigned short)param_7;
  DAT_0041aa00.signature = 0x3058;
  DAT_0041aa18 = 0;
  max_match_len = 7;
  scanline_ptr = (unsigned char *)&DAT_0041aa00;
  do
  {
    FUN_00409e00(8, (unsigned int)*scanline_ptr);
    max_match_len = max_match_len - 1;
    scanline_ptr = scanline_ptr + 1;
  } while (max_match_len != -1);

  run_length = 1;
  scanline_ptr = DAT_0041aa28 + param_4;
  current_value = 0xffffffff;
  cursor_ptr = scanline_ptr + param_6;
  do
  {
    run_value = current_value;
    if (scanline_ptr + param_6 <= cursor_ptr)
    {
      reached_last_row = (param_7 == 0);
      param_7 = param_7 - 1;
      if (reached_last_row != 0)
      {
        max_match_len = DAT_0041aa20;
        if ((short)current_value == 0x90)
        {
          while (run_length != 0)
          {
            run_length = run_length - 1;
            if (max_match_len == -1)
            {
              DAT_0041aa20 = 0;
              FUN_00409e00(8, 0xb);
              FUN_00409cc0();
              DAT_0041aa14 = 0x90;
              DAT_0041b140[0] = 0x90;
              DAT_0041aa10 = 1;
              current_value = DAT_0041aa14;
            }
            else
            {
              next_row_index = DAT_0041aa10 + 1;
              dictionary_ptr = DAT_0041b140 + DAT_0041aa10;
              DAT_0041aa10 = next_row_index;
              *dictionary_ptr = 0x90;
              if (max_match_len < next_row_index)
              {
                DAT_0041aa20 = next_row_index;
              }
              current_value = FUN_00409d30(0x90);
              if (current_value == 0xffffffff)
              {
                *DAT_00423330 = 0x90;
                DAT_00423330[1] = DAT_0041aa14;
                DAT_00423330[2] = DAT_0041aa24;
                DAT_0041aa24 = DAT_0041aa24 + 1;
                FUN_00409e00(DAT_0041a9f4, *(unsigned int *)(DAT_0041aa1c + 8 + DAT_0041aa14 * 0xc));
                DAT_0041aa10 = 1;
                DAT_0041aa14 = 0x90;
                DAT_0041b140[0] = 0x90;
                current_value = DAT_0041aa14;
                if ((1 << ((unsigned char)DAT_0041a9f4 & 0x1f)) < DAT_0041aa24)
                {
                  DAT_0041a9f4 = DAT_0041a9f4 + 1;
                  if (0xb < DAT_0041a9f4)
                  {
                    FUN_00409cc0();
                    current_value = DAT_0041aa14;
                  }
                }
              }
            }
            DAT_0041aa14 = current_value;
            FUN_00409a20(0);
            max_match_len = DAT_0041aa20;
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
              FUN_00409a20(current_value & 0xffff);
              FUN_00409a20(0x90);
              FUN_00409a20(run_chunk_len);
            } while (3 < run_length);
          }
          while (run_length != 0)
          {
            run_length = run_length - 1;
            FUN_00409a20(current_value & 0xffff);
          }
        }

        FUN_00409e00(DAT_0041a9f4, *(unsigned int *)(DAT_0041aa1c + 8 + DAT_0041aa14 * 0xc));
        FUN_00409e00(8, 0);
        _write(DAT_0041aa0c, DAT_00422120, DAT_0041aa18);
        DAT_0041aa18 = 0;
        seek_offset = _tell(DAT_0041aa0c);
        DAT_0041aa00.payload_size = (unsigned short)seek_offset - 4;
        if (DAT_0041aa08 == 0)
        {
          seek_offset = 0;
        }
        else
        {
          seek_offset = 0x306;
        }
        _lseek(DAT_0041aa0c, seek_offset, 0);
        _write(DAT_0041aa0c, &DAT_0041aa00, 8);
        free((void *)DAT_0041aa1c);
        return _close(DAT_0041aa0c);
      }

      next_row_index = param_5 + 1;
      param_2((unsigned int *)scanline_ptr, param_3, param_4, param_5, param_6);
      param_5 = next_row_index;
      cursor_ptr = scanline_ptr;
      if (current_value == 0xffffffff)
      {
        cursor_ptr = DAT_0041aa28 + param_4 + 1;
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
      max_match_len = DAT_0041aa20;
      if ((short)run_value == 0x90)
      {
        while (run_length != 0)
        {
          run_length = run_length - 1;
          if (max_match_len == -1)
          {
            DAT_0041aa20 = 0;
            FUN_00409e00(8, 0xb);
            FUN_00409cc0();
            DAT_0041aa14 = 0x90;
            DAT_0041b140[0] = 0x90;
            DAT_0041aa10 = 1;
            run_value = DAT_0041aa14;
          }
          else
          {
            next_row_index = DAT_0041aa10 + 1;
            dictionary_ptr = DAT_0041b140 + DAT_0041aa10;
            DAT_0041aa10 = next_row_index;
            *dictionary_ptr = 0x90;
            if (max_match_len < next_row_index)
            {
              DAT_0041aa20 = next_row_index;
            }
            run_value = FUN_00409d30(0x90);
            if (run_value == 0xffffffff)
            {
              *DAT_00423330 = 0x90;
              DAT_00423330[1] = DAT_0041aa14;
              DAT_00423330[2] = DAT_0041aa24;
              DAT_0041aa24 = DAT_0041aa24 + 1;
              FUN_00409e00(DAT_0041a9f4, *(unsigned int *)(DAT_0041aa1c + 8 + DAT_0041aa14 * 0xc));
              DAT_0041aa14 = 0x90;
              DAT_0041b140[0] = 0x90;
              DAT_0041aa10 = 1;
              run_value = DAT_0041aa14;
              if ((1 << ((unsigned char)DAT_0041a9f4 & 0x1f)) < DAT_0041aa24)
              {
                DAT_0041a9f4 = DAT_0041a9f4 + 1;
                if (0xb < DAT_0041a9f4)
                {
                  FUN_00409cc0();
                  run_value = DAT_0041aa14;
                }
              }
            }
          }
          DAT_0041aa14 = run_value;
          FUN_00409a20(0);
          max_match_len = DAT_0041aa20;
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
            FUN_00409a20(run_value & 0xffff);
            FUN_00409a20(0x90);
            FUN_00409a20(run_chunk_len);
          } while (3 < run_length);
        }
        while (run_length != 0)
        {
          run_length = run_length - 1;
          FUN_00409a20(run_value & 0xffff);
        }
      }
      run_length = 1;
      cursor_ptr = cursor_ptr + 1;
    }
  } while (1);

  return 0;
}

// FUNCTION: FACEMAKER 0x00409a20
void FUN_00409a20(int param_1)
{
  unsigned char *dictionary_ptr;
  int next_code_length;
  int table_index;
  unsigned int hash_key;
  unsigned int hash_slot;
  unsigned char code_byte;
  int probe_stride;

  probe_stride = DAT_0041aa20;
  code_byte = (unsigned char)param_1;
  if (DAT_0041aa20 == -1)
  {
    probe_stride = 0;
    DAT_0041aa20 = 0;
    FUN_00409e00(8, 0xb);
    table_index = 0;
    do
    {
      table_index = table_index + 0xc;
      *(int *)(DAT_0041aa1c + -4 + table_index) = probe_stride;
      *(int *)(DAT_0041aa1c + -0xc + table_index) = probe_stride;
      probe_stride = probe_stride + 1;
      *(int *)(DAT_0041aa1c + -8 + table_index) = -1;
    } while (table_index < 0xc00);
    if (probe_stride < 0xffb)
    {
      probe_stride = probe_stride * 0xc;
      do
      {
        probe_stride = probe_stride + 0xc;
        *(int *)(DAT_0041aa1c + -0xc + probe_stride) = -1;
      } while (probe_stride < 0xbfc4);
    }
    DAT_0041a9f4 = 9;
    DAT_0041aa24 = 0x101;
    DAT_0041aa10 = 1;
    DAT_0041aa14 = param_1;
    DAT_0041b140[0] = code_byte;
    return;
  }

  next_code_length = DAT_0041aa10 + 1;
  dictionary_ptr = DAT_0041b140 + DAT_0041aa10;
  DAT_0041aa10 = next_code_length;
  *dictionary_ptr = code_byte;
  table_index = DAT_0041aa10;
  if (probe_stride < next_code_length)
  {
    DAT_0041aa20 = next_code_length;
  }
  hash_key = 0;
  DAT_0041b140[DAT_0041aa10] = 0;
  if (0 < table_index)
  {
    probe_stride = 0;
    do
    {
      hash_key = hash_key + (int)*(short *)(DAT_0041b140 + probe_stride);
      probe_stride = probe_stride + 2;
    } while (probe_stride < DAT_0041aa10);
  }
  hash_key = hash_key & 0x7fff;
  if (hash_key == 0)
  {
    hash_key = DAT_0041aa10 * 0x25 & 0x7fff;
  }
  hash_slot = hash_key % 0xffb;
  probe_stride = -1;
  while (1)
  {
    DAT_00423330 = (int *)(DAT_0041aa1c + hash_slot * 0xc);
    if (*DAT_00423330 == -1)
    {
      break;
    }
    if ((param_1 == *DAT_00423330) && (DAT_00423330[1] == DAT_0041aa14))
    {
      goto MATCH_FOUND;
    }
    if (probe_stride == -1)
    {
      probe_stride = 0xff9 - hash_key % 0xff9;
    }
    if (probe_stride == 0)
    {
      probe_stride = DAT_0041aa10 * 0x89;
    }
    hash_slot = (unsigned int)(hash_slot + probe_stride) % 0xffb;
  }
  hash_slot = 0xffffffff;

MATCH_FOUND:
  if (hash_slot != 0xffffffff)
  {
    DAT_0041aa14 = hash_slot;
    return;
  }

  *DAT_00423330 = param_1;
  DAT_00423330[1] = DAT_0041aa14;
  DAT_00423330[2] = DAT_0041aa24;
  DAT_0041aa24 = DAT_0041aa24 + 1;
  FUN_00409e00(DAT_0041a9f4, *(unsigned int *)(DAT_0041aa1c + 8 + DAT_0041aa14 * 0xc));
  DAT_0041aa14 = param_1;
  DAT_0041aa10 = 1;
  DAT_0041b140[0] = code_byte;
  if ((1 << ((unsigned char)DAT_0041a9f4 & 0x1f)) < DAT_0041aa24)
  {
    DAT_0041a9f4 = DAT_0041a9f4 + 1;
    if (0xb < DAT_0041a9f4)
    {
      FUN_00409cc0();
    }
  }
}

// FUNCTION: FACEMAKER 0x00409cc0
void FUN_00409cc0(void)
{
  int code_index;
  int table_offset;

  code_index = 0;
  table_offset = 0;
  do
  {
    table_offset = table_offset + 0xc;
    *(int *)(DAT_0041aa1c + -4 + table_offset) = code_index;
    *(int *)(DAT_0041aa1c + -0xc + table_offset) = code_index;
    code_index = code_index + 1;
    *(int *)(DAT_0041aa1c + -8 + table_offset) = -1;
  } while (table_offset < 0xc00);
  if (code_index < 0xffb)
  {
    code_index = code_index * 0xc;
    do
    {
      code_index = code_index + 0xc;
      *(int *)(DAT_0041aa1c + -0xc + code_index) = -1;
    } while (code_index < 0xbfc4);
  }
  DAT_0041a9f4 = 9;
  DAT_0041aa24 = 0x101;
}

// FUNCTION: FACEMAKER 0x00409d30
unsigned int FUN_00409d30(int param_1)
{
  int scan_offset;
  unsigned int hash_sum;
  unsigned int hash_slot;
  int probe_stride;

  scan_offset = DAT_0041aa10;
  hash_sum = 0;
  DAT_0041b140[DAT_0041aa10] = 0;
  if (0 < scan_offset)
  {
    scan_offset = 0;
    do
    {
      hash_sum = hash_sum + (int)*(short *)(DAT_0041b140 + scan_offset);
      scan_offset = scan_offset + 2;
    } while (scan_offset < DAT_0041aa10);
  }
  hash_sum = hash_sum & 0x7fff;
  if (hash_sum == 0)
  {
    hash_sum = DAT_0041aa10 * 0x25 & 0x7fff;
  }

  probe_stride = -1;
  hash_slot = hash_sum % 0xffb;
  while (1)
  {
    DAT_00423330 = (int *)(DAT_0041aa1c + hash_slot * 0xc);
    if (*DAT_00423330 == -1)
    {
      return 0xffffffff;
    }
    if ((param_1 == *DAT_00423330) && (DAT_00423330[1] == DAT_0041aa14))
    {
      break;
    }
    if (probe_stride == -1)
    {
      probe_stride = 0xff9 - hash_sum % 0xff9;
    }
    if (probe_stride == 0)
    {
      probe_stride = DAT_0041aa10 * 0x89;
    }
    hash_slot = (unsigned int)(probe_stride + hash_slot) % 0xffb;
  }
  return hash_slot;
}

// FUNCTION: FACEMAKER 0x00409e00
void FUN_00409e00(int param_1, unsigned int param_2)
{
  int file_handle;
  unsigned int bit_accumulator;

  file_handle = DAT_0041aa0c;
  while (param_1 != 0)
  {
    param_1 = param_1 - 1;
    bit_accumulator = DAT_0041a9f0;
    if (7 < DAT_0041a9f8)
    {
      DAT_00422120[DAT_0041aa18] = (unsigned char)DAT_0041a9f0;
      DAT_0041aa18 = DAT_0041aa18 + 1;
      DAT_0041a9f0 = 0;
      DAT_0041a9f8 = 0;
      bit_accumulator = 0;
      if (0x1ff < DAT_0041aa18)
      {
        _write(file_handle, DAT_00422120, DAT_0041aa18);
        DAT_0041aa18 = 0;
        file_handle = DAT_0041aa0c;
        bit_accumulator = DAT_0041a9f0;
      }
    }
    DAT_0041a9f0 = (int)bit_accumulator >> 1;
    if ((param_2 & 1) != 0)
    {
      DAT_0041a9f0 = DAT_0041a9f0 | 0x80;
    }
    param_2 = (unsigned int)((int)param_2 >> 1);
    DAT_0041a9f8 = DAT_0041a9f8 + 1;
  }
}

// FUNCTION: FACEMAKER 0x00409420
int FUN_00409420(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, char *param_7)
{
  int file_handle;
  int result;

  file_handle = _open(param_7, 0x8302, 0x80);
  result = file_handle;
  if (file_handle != -1)
  {
    if (param_6 != 0)
    {
      FUN_00407a00(file_handle);
      DAT_0041aa08 = 1;
    }
    else
    {
      DAT_0041aa08 = 0;
    }
    result = FUN_004094b0(file_handle, FUN_004076c0, param_1, param_2, param_3, param_4, param_5);
    _close(file_handle);
  }
  return result;
}

// FUNCTION: FACEMAKER 0x0040a110
void FUN_0040a110(int page_number, char *path)
{
  FUN_00409eb0(page_number, 0, 0, path, (void *)1);
}

// FUNCTION: FACEMAKER 0x0040a130
void FUN_0040a130(int page_number, char *path)
{
  FUN_00409eb0(page_number, 0, 0, path, (void *)0);
}

// FUNCTION: FACEMAKER 0x0040a150
void FUN_0040a150(char *path, unsigned short *palette)
{
  FUN_00409eb0(-1, 0, 0, path, palette);
}

// FUNCTION: FACEMAKER 0x00409eb0
void FUN_00409eb0(int page_number, int x, int y, char *path, void *opaque)
{
  char *ext;
  int file_handle;
  int line;
  unsigned short *palette;
  unsigned short local_palette[0x200];

  ext = strchr(path, '.');
  if (_stricmp(DAT_0040d2d8, ext) != 0)
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
      FUN_004075d0((unsigned int *)gPcxScanlineBuffer, page_number, x, y + line, global_pcxw_image_width);
      line = line + 1;
    }

    assert((unsigned int)(_close(file_handle) == 0), s_D__NewMagic__sources__sidlib__Fileio_c_0040d29c, 0xa8,
           (char *)0);
    return;
  }

  gPcxInFile = fopen(path, DAT_0040d224);
  assert((unsigned int)(gPcxInFile != (FILE *)0), s_D__NewMagic__sources__sidlib__Fileio_c_0040d29c, 0xf7,
         s_Error_Opening_File__s_0040d2c0, path);
  gPcxPath = path;
  palette = (unsigned short *)opaque;
  if (palette == (unsigned short *)1)
  {
    palette = local_palette;
  }
  if (palette == (unsigned short *)0)
  {
    PcxReadHeaderAndPalette((void *)0);
  }
  else
  {
    PcxReadHeaderAndPalette(palette + 3);
    *(char *)palette = 'M';
    *((char *)palette + 1) = '1';
    *(unsigned short *)((char *)palette + 2) = 0x300;
    *((char *)palette + 4) = '\0';
    *((char *)palette + 5) = '\xff';
    RpBits_ApplyPalette((short *)palette);
  }

  if (page_number < 0)
  {
    global_pcxw_image_height = 0;
  }
  line = 0;
  while (line < global_pcxw_image_height)
  {
    PcxReadScanlineRle((char *)gPcxScanlineBuffer);
    FUN_004075d0((unsigned int *)gPcxScanlineBuffer, page_number, x, y + line, global_pcxw_image_width);
    line = line + 1;
  }
  fclose(gPcxInFile);
}
