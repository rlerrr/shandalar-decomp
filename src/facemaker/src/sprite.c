#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <io.h>
#include <string.h>
#include "cardartlib/src/assert.h"
#include "drawcardlib/src/pic.h"
#include "facemaker_types.h"

#ifdef _fileno
#undef _fileno
#endif

int ReadSpriteEntryPointers(EncodedImage **out_entry_ptrs, char *sprite_path);
size_t WriteSpriteBlob(void *sprite_blob, char *output_path);
EncodedImage *EncodeSpriteFromPage(int page_number, int x, int y, unsigned int width, int height);

extern int ReadGraphicsPixel(int page_number, int x, int y);
extern void WriteGraphicsScanline(unsigned int *param_1, int param_2, int param_3, int param_4,
                                 unsigned int param_5);
extern void ReadGraphicsScanline(unsigned int *param_1, int param_2, int param_3, int param_4,
                                 unsigned int param_5);
extern void PutGraphicsPixel(FacemakerWindowBounds *window_bounds, int x, int y, unsigned int color_index);
extern void DrawEncodedImageUnscaled(FacemakerWindowBounds *dst, int x, int y, EncodedImage *encoded_image);
extern void LoadPcxIntoPageNoPalette(int page_number, char *path);
extern void BeginSpriteEncodeSession(void);
extern void FinalizeSpriteEncodeSession(void);
extern int *g_sprite_blob_base;
extern int *g_sprite_blob_cursor;
extern DIBSurface *g_graphics_pages[10];
extern FacemakerWindowBounds *g_face_fullscreen_bounds;
extern char g_file_read_mode[];
#pragma optimize("gy", on)

// FUNCTION: SHANDALAR 0x0057b650
// FUNCTION: FACEMAKER 0x00408900
void BeginSpriteEncodeSession(void)
{
  g_sprite_blob_base = malloc(0x200000);
  g_sprite_blob_cursor = g_sprite_blob_base;
}

// FUNCTION: SHANDALAR 0x0057b670
// FUNCTION: FACEMAKER 0x00408920
void FinalizeSpriteEncodeSession(void)
{
  *g_sprite_blob_cursor = -1;
  g_sprite_blob_base = _expand((void *)g_sprite_blob_base, (int)g_sprite_blob_cursor + (0x10 - (int)g_sprite_blob_base));
}

// FUNCTION: SHANDALAR 0x0057b6a0
// FUNCTION: FACEMAKER 0x00408950
void FreeSpriteBlob(void *memory)
{
  free(memory);
}

// FUNCTION: SHANDALAR 0x0057b6c0
// FUNCTION: FACEMAKER 0x00408970
size_t WriteSpriteBlob(void *sprite_blob, char *output_path)
{
  size_t byte_count;
  FILE *file;

  byte_count = _msize(sprite_blob);
  file = fopen(output_path, "wb");
  fwrite(sprite_blob, 1, byte_count, file);
  fclose(file);
  return byte_count;
}

// FUNCTION: SHANDALAR 0x0057b710
// FUNCTION: FACEMAKER 0x004089c0
int ReadSpriteEntryPointers(EncodedImage **out_entry_ptrs, char *sprite_path)
{
  FILE *file;
  int file_handle;
  int entry_count;
  size_t file_size;
  int *sprite_data;

  entry_count = 0;
  file = fopen(sprite_path, g_file_read_mode);
  assert((int)file, "D:\\NewMagic\\sources\\sidlib\\sprite.c", 0xa3,
         "Could not open Sprite File %s\n", sprite_path);
  file_handle = _fileno(file);
  file_size = _filelength(file_handle);
  sprite_data = malloc(file_size);
  fread(sprite_data, 1, file_size, file);
  fclose(file);

  while (*sprite_data != -1)
  {
    *out_entry_ptrs = (EncodedImage *)sprite_data;
    out_entry_ptrs = out_entry_ptrs + 1;
    entry_count = entry_count + 1;
    sprite_data = (int *)((int)sprite_data + *sprite_data);
  }
  return entry_count;
}

// FUNCTION: SHANDALAR 0x0057b840
// FUNCTION: FACEMAKER 0x00408a50
EncodedImage *EncodeSpriteFromPage(int page_number, int x, int y, unsigned int width, int height)
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
    sampled = ReadGraphicsPixel(page_number, probe_x, probe_y);
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
        sampled_next = ReadGraphicsPixel(page_number, i, probe_y);
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
    sampled = ReadGraphicsPixel(page_number, probe_x, probe_y);
    run_end = height + 2;
    if (0 < run_end)
    {
      do
      {
        sampled_next = ReadGraphicsPixel(page_number, probe_x, probe_y);
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

  sprite = (EncodedImage *)g_sprite_blob_cursor;
  write_cursor = (int *)sprite->spans;
  do
  {
    ReadGraphicsScanline((unsigned int *)line_buffer, page_number, x, y + line_index, width);
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
    ReadGraphicsScanline((unsigned int *)line_buffer, page_number, x, y + line_index + 1, width);
  }

  first_char = *(char *)((int)write_cursor - 1);
  g_sprite_blob_cursor = write_cursor;
  while (first_char == -1)
  {
    sprite->row_count = sprite->row_count - 1;
    first_char = *(char *)((int)g_sprite_blob_cursor - 2);
    g_sprite_blob_cursor = (int *)((int)g_sprite_blob_cursor - 1);
  }
  if (((unsigned int)g_sprite_blob_cursor & 1) != 0)
  {
    g_sprite_blob_cursor = (int *)((int)g_sprite_blob_cursor + 1);
  }
  if (((unsigned int)g_sprite_blob_cursor & 2) != 0)
  {
    g_sprite_blob_cursor = (int *)((int)g_sprite_blob_cursor + 2);
  }
  *g_sprite_blob_cursor = -1;
  sprite->total_size = (int)g_sprite_blob_cursor - (int)sprite;
  return sprite;
}

#pragma optimize("", on)
