#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <io.h>
#include <string.h>
#include "cardartlib/src/assert.h"
#include "facemaker_types.h"

typedef struct FacemakerWindowBounds
{
  char pad_0[12];
  int max_x;
  int max_y;
} FacemakerWindowBounds;

int FUN_004089c0(int *param_1, char *param_2);
size_t FUN_00408970(void *param_1, char *param_2);
int FUN_00408a50(int page_number, int x, int y, unsigned int width, int height);

extern int FUN_00406da0(int page_number, int x, int y);
extern void FUN_004075d0(unsigned int *param_1, int param_2, int param_3, int param_4,
                                 unsigned int param_5);
extern void FUN_004076c0(unsigned int *param_1, int param_2, int param_3, int param_4,
                                 unsigned int param_5);
extern void FUN_0040a130(int page_number, char *path);
extern void FUN_00408900(void);
extern void FUN_00408920(void);
extern int *DAT_004199c8;
extern int *DAT_004199cc;
extern FacemakerWindowBounds *PTR_DAT_0040c0ac;
extern char DAT_0040d224[];

#pragma optimize("gy", on)

// GLOBAL: FACEMAKER 0x0040c9c4
char DAT_0040c9c4[] = "1";

// GLOBAL: FACEMAKER 0x0040c9c8
char s___pcx_0040c9c8[] = "*.pcx";

// GLOBAL: FACEMAKER 0x0040c9d0
char DAT_0040c9d0[] = ".pcx";

// GLOBAL: FACEMAKER 0x0040c9d8
char DAT_0040c9d8[] = ".pcx";

// GLOBAL: FACEMAKER 0x0040d24c
char DAT_0040d24c[] = "wb";

// GLOBAL: FACEMAKER 0x0040d250
char s_D__NewMagic__sources__sidlib__sprite_c_0040d250[] = "D:\\NewMagic\\sources\\sidlib\\sprite.c";

// GLOBAL: FACEMAKER 0x0040d274
char s_Could_not_open_Sprite_File__s_0040d274[] = "Could not open Sprite File %s\n";

#pragma optimize("", off)

// FUNCTION: FACEMAKER 0x00405261
int FUN_00405261(int page_number, int x, int y, unsigned int width, int height)
{
  int pixel_index;
  unsigned char local_404[1024];

  (void)page_number;
  while (1)
  {
    if (height == 0)
    {
      return 1;
    }
    height = height - 1;
    FUN_004076c0((unsigned int *)local_404, 2, x, y, width);
    y = y + 1;
    for (pixel_index = 0; pixel_index < (int)width; pixel_index = pixel_index + 1)
    {
      if (local_404[pixel_index] != '\0')
      {
        return 0;
      }
    }
  }
}

// FUNCTION: FACEMAKER 0x0040530d
int FUN_0040530d(char *param_1, int param_2, int param_3, int *param_4)
{
  int table_count;
  int table_index;
  int entry_count;
  int current_sprite;
  int current_entry;
  int sprite_table[1000];

  entry_count = 0;
  table_count = 0;
  table_index = FUN_004089c0(sprite_table, param_1);
  *param_4 = sprite_table[0];

  current_sprite = 1;
  while (1)
  {
    if (table_index <= current_sprite)
    {
      return entry_count;
    }

    current_entry = sprite_table[current_sprite];
    *(int *)(entry_count * 0x50 + table_count * 4 + param_3) = current_entry;
    table_count = table_count + 1;
    if (*(short *)(current_entry + 8) == 0 && *(short *)(current_entry + 10) == 0)
    {
      *(int *)(param_2 + entry_count * 4) = table_count;
      entry_count = entry_count + 1;
      break;
    }
    if (*(short *)(current_entry + 8) == 0)
    {
      *(int *)(param_2 + entry_count * 4) = table_count;
      entry_count = entry_count + 1;
      table_count = 0;
    }
    current_sprite = current_sprite + 1;
  }
  return entry_count;
}

// FUNCTION: FACEMAKER 0x0040542d
int FUN_0040542d(int *param_1, int param_2, int param_3, unsigned int param_4, int param_5,
                         unsigned int param_6, unsigned char param_7)
{
  int row_index;
  int pixel_index;
  unsigned char local_804[2048];

  for (row_index = 0; row_index < param_5; row_index = row_index + 1)
  {
    FUN_004076c0((unsigned int *)local_804, *param_1, param_2, row_index + param_3, param_4);
    for (pixel_index = 0; pixel_index < (int)param_4; pixel_index = pixel_index + 1)
    {
      if (local_804[pixel_index] == param_6)
      {
        local_804[pixel_index] = param_7;
      }
    }
    FUN_004075d0((unsigned int *)local_804, *param_1, param_2, row_index + param_3, param_4);
  }
  return param_5;
}

// FUNCTION: FACEMAKER 0x00404d70
int FUN_00404d70(char *path, int counts_out, void *entries_out, int *first_entry_out)
{
  int current_sprite;
  int tile_x;
  int tile_y;
  int frames_in_group;
  int group_count;
  int saw_separator;
  int use_pcx_tiles;
  int first_tile;
  unsigned int tile_index;
  unsigned int sign_bits;
  char *group_suffix;
  char local_350[260];
  struct _finddata_t find_data;
  struct _finddata_t pcx_find_data;
  long find_handle;
  long pcx_find_handle;

  group_count = 0;
  frames_in_group = 0;
  saw_separator = 0;
  group_suffix = DAT_0040c9c4;
  first_tile = 1;
  use_pcx_tiles = 0;
  memset(entries_out, 0, 4);
  *group_suffix = 'a';

  strcpy(local_350, path);
  find_handle = _findfirst(local_350, &find_data);

  strcpy(local_350, path);
  strcat(local_350, s___pcx_0040c9c8);
  pcx_find_handle = _findfirst(local_350, &pcx_find_data);
  if (pcx_find_handle == -1 && find_handle == -1)
  {
    return 0;
  }

  if (find_handle == -1)
  {
    use_pcx_tiles = 1;
  }
  else if (pcx_find_handle != -1)
  {
    while (_findnext(pcx_find_handle, &pcx_find_data) == 0)
    {
      if (find_data.size < pcx_find_data.size)
      {
        use_pcx_tiles = 1;
      }
    }
  }

  if (pcx_find_handle != -1)
  {
    _findclose(pcx_find_handle);
  }
  if (find_handle != -1)
  {
    _findclose(find_handle);
  }

  strcpy(local_350, path);
  strcat(local_350, DAT_0040c9d0);
  if (use_pcx_tiles == 0)
  {
    return FUN_0040530d(path, counts_out, (int)entries_out, first_entry_out);
  }

  FUN_0040a130(2, local_350);
  FUN_0040542d((int *)PTR_DAT_0040c0ac, 0, 0, 0x22c, 0x158, 0x6d, 0);
  FUN_00408900();
  *first_entry_out = FUN_00408a50(2, 0, 0, 0x89, 0xa9);
  current_sprite = *first_entry_out;
  tile_index = 1;
  while (1)
  {
    sign_bits = (unsigned int)((int)tile_index >> 0x1f);
    tile_x = ((((tile_index ^ sign_bits) - sign_bits) & 3U) ^ sign_bits) - sign_bits;
    tile_x = tile_x * 0x8a;
    tile_y = ((int)(tile_index + (sign_bits & 3U)) >> 2) * 0xaa;
    if (FUN_00405261(2, tile_x, tile_y, 0x8a, 0xaa) == 0)
    {
      *(int *)(group_count * 0x50 + frames_in_group * 4 + (int)entries_out) = FUN_00408a50(2, tile_x, tile_y, 0x89, 0xa9);
      current_sprite = *(int *)(group_count * 0x50 + frames_in_group * 4 + (int)entries_out);
      frames_in_group = frames_in_group + 1;
      *(short *)(current_sprite + 10) = -1;
      *(short *)(current_sprite + 8) = *(short *)(current_sprite + 10);
      saw_separator = 0;
    }
    else
    {
      if (saw_separator != 0)
      {
        *(short *)(current_sprite + 10) = 0;
        break;
      }
      if (first_tile == 0)
      {
        *(short *)(current_sprite + 8) = 0;
        *(int *)(counts_out + group_count * 4) = frames_in_group;
        group_count = group_count + 1;
        frames_in_group = 0;
        saw_separator = 1;
      }
      else
      {
        *(short *)(current_sprite + 8) = 0;
      }
    }

    tile_index = tile_index + 1;
    first_tile = 0;
    if (7 < (int)tile_index)
    {
      tile_index = 0;
      strcpy(local_350, path);
      strcat(local_350, group_suffix);
      strcat(local_350, DAT_0040c9d8);
      *group_suffix = *group_suffix + '\x01';
      pcx_find_handle = _findfirst(local_350, &pcx_find_data);
      if (pcx_find_handle == -1)
      {
        break;
      }
      _findclose(pcx_find_handle);
      FUN_0040a130(2, local_350);
      FUN_0040542d((int *)PTR_DAT_0040c0ac, 0, 0, 0x22c, 0x158, 0x6d, 0);
    }
  }

  FUN_00408920();
  FUN_00408970((void *)*first_entry_out, path);
  return group_count;
}

#pragma optimize("gy", on)

// FUNCTION: FACEMAKER 0x00408900
void FUN_00408900(void)
{
  DAT_004199c8 = malloc(0x200000);
  DAT_004199cc = DAT_004199c8;
}

// FUNCTION: FACEMAKER 0x00408920
void FUN_00408920(void)
{
  *DAT_004199cc = -1;
  DAT_004199c8 = _expand((void *)DAT_004199c8, (int)DAT_004199cc + (0x10 - (int)DAT_004199c8));
}

// FUNCTION: FACEMAKER 0x00408970
size_t FUN_00408970(void *param_1, char *param_2)
{
  size_t byte_count;
  FILE *file;

  byte_count = _msize(param_1);
  file = fopen(param_2, DAT_0040d24c);
  fwrite(param_1, 1, byte_count, file);
  fclose(file);
  return byte_count;
}

// FUNCTION: FACEMAKER 0x004089c0
int FUN_004089c0(int *param_1, char *param_2)
{
  FILE *file;
  int file_handle;
  int entry_count;
  size_t file_size;
  int *sprite_data;

  entry_count = 0;
  file = fopen(param_2, DAT_0040d224);
  assert((int)file, s_D__NewMagic__sources__sidlib__sprite_c_0040d250, 0xa3,
         s_Could_not_open_Sprite_File__s_0040d274, param_2);
  file_handle = _fileno(file);
  file_size = _filelength(file_handle);
  sprite_data = malloc(file_size);
  fread(sprite_data, 1, file_size, file);
  fclose(file);

  while (*sprite_data != -1)
  {
    *param_1 = (int)sprite_data;
    param_1 = param_1 + 1;
    entry_count = entry_count + 1;
    sprite_data = (int *)((int)sprite_data + *sprite_data);
  }
  return entry_count;
}

// FUNCTION: FACEMAKER 0x00408a50
int FUN_00408a50(int page_number, int x, int y, unsigned int width, int height)
{
  char *scan;
  char *write_ptr;
  char first_char;
  EncodedImage *sprite;
  int *write_cursor;
  int border_left;
  int border_top;
  int line_index;
  int probe_x;
  int probe_y;
  int i;
  int run_start;
  int run_end;
  unsigned int sampled;
  unsigned int sampled_next;
  unsigned int run_len;
  unsigned int run_offset;
  unsigned int dword_count;
  unsigned int tail_count;
  unsigned char *run_header;
  void *zero_in_run;
  char line_buffer[0x400];

  line_index = 0;
  probe_y = y + 1 + height;
  probe_x = x - 1;
  if (probe_x < 0 || probe_y < 0)
  {
    border_left = -1;
  }
  else
  {
    border_left = (int)(width != 0xfffffffe);
    sampled = FUN_00406da0(page_number, probe_x, probe_y);
    run_end = width + 2;
    if (width == 0xfffffffe)
    {
      run_end = 0;
    }
    run_start = 0;
    i = probe_x;
    if (0 < run_end)
    {
      do
      {
        sampled_next = FUN_00406da0(page_number, i, probe_y);
        if (sampled_next != sampled)
        {
          break;
        }
        run_start = run_start + 1;
        i = i + border_left;
      } while (run_start < run_end);
    }
    border_left = -1;
    if (run_end != run_start)
    {
      border_left = run_start;
    }
  }

  probe_y = y - 1;
  if (probe_x < 0 || probe_y < 0)
  {
    border_top = -1;
  }
  else
  {
    i = 0;
    sampled = FUN_00406da0(page_number, probe_x, probe_y);
    run_end = height + 2;
    if (0 < run_end)
    {
      do
      {
        sampled_next = FUN_00406da0(page_number, probe_x, probe_y);
        if (sampled_next != sampled)
        {
          break;
        }
        i = i + 1;
        probe_y = probe_y + (unsigned int)(height != -2);
      } while (i < run_end);
    }
    border_top = -1;
    if (run_end != i)
    {
      border_top = i;
    }
  }

  sprite = (EncodedImage *)DAT_004199cc;
  write_cursor = (int *)sprite->spans;
  do
  {
    FUN_004076c0((unsigned int *)line_buffer, page_number, x, y + line_index, width);
    scan = line_buffer;
    run_offset = width;
    first_char = line_buffer[0];
    while (first_char == '\0' && run_offset != 0)
    {
      first_char = scan[1];
      scan = scan + 1;
      run_offset = run_offset - 1;
    }
    if (scan - width != line_buffer)
    {
      break;
    }
    line_index = line_index + 1;
  } while (line_index < height);

  sprite->width = (unsigned short)width;
  sprite->height = (unsigned short)height;
  sprite->left_clip = (short)border_left;
  sprite->top_clip = (short)border_top;
  sprite->first_row = (short)line_index;
  sprite->row_count = (short)height - (short)line_index;
  for (; line_index < height; line_index = line_index + 1)
  {
    scan = line_buffer;
    run_offset = width;
    first_char = line_buffer[0];
    while (first_char == '\0' && run_offset != 0)
    {
      first_char = scan[1];
      scan = scan + 1;
      run_offset = run_offset - 1;
    }
    run_offset = (int)scan - (int)line_buffer;
    if (width == run_offset)
    {
      *(unsigned char *)write_cursor = 0xff;
      write_cursor = (int *)((int)write_cursor + 1);
    }
    else
    {
      *(char *)write_cursor = (char)run_offset;
      run_header = (unsigned char *)((int)write_cursor + 1);
      first_char = *(char *)((int)&border_left + width + 3);
      run_start = (int)&border_left + width + 3;
      run_end = run_start;
      run_len = width;
      while (first_char == '\0' && run_len != 0)
      {
        first_char = *(char *)(run_end + -1);
        run_end = run_end - 1;
        run_len = run_len - 1;
      }
      run_len = run_end + ((width - run_offset) - run_start);
      zero_in_run = memchr(line_buffer + run_offset, 0, run_len);
      if (zero_in_run == (void *)0)
      {
        *run_header = 0xfe;
        run_header = (unsigned char *)((int)write_cursor + 2);
      }
      *run_header = (char)run_len;
      scan = line_buffer + run_offset;
      write_ptr = (char *)(run_header + 1);
      for (dword_count = run_len >> 2; dword_count != 0; dword_count = dword_count - 1)
      {
        *(unsigned int *)write_ptr = *(unsigned int *)scan;
        scan = scan + 4;
        write_ptr = write_ptr + 4;
      }
      write_cursor = (int *)(run_header + 1 + run_len);
      for (tail_count = run_len & 3; tail_count != 0; tail_count = tail_count - 1)
      {
        *write_ptr = *scan;
        scan = scan + 1;
        write_ptr = write_ptr + 1;
      }
    }
    FUN_004076c0((unsigned int *)line_buffer, page_number, x, y + line_index + 1, width);
  }

  first_char = *(char *)((int)write_cursor - 1);
  DAT_004199cc = write_cursor;
  while (first_char == -1)
  {
    sprite->row_count = sprite->row_count - 1;
    first_char = *(char *)((int)DAT_004199cc - 2);
    DAT_004199cc = (int *)((int)DAT_004199cc - 1);
  }
  if (((unsigned int)DAT_004199cc & 1) != 0)
  {
    DAT_004199cc = (int *)((int)DAT_004199cc + 1);
  }
  if (((unsigned int)DAT_004199cc & 2) != 0)
  {
    DAT_004199cc = (int *)((int)DAT_004199cc + 2);
  }
  *DAT_004199cc = -1;
  sprite->total_size = (int)DAT_004199cc - (int)sprite;
  return (int)sprite;
}
