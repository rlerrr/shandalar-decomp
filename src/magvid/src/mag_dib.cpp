#include "mag_dib.h"

#include <stdlib.h>
#include <string.h>

int __cdecl bitmap_header_color_count(BITMAPINFOHEADER *header);
int __cdecl bitmap_header_has_color_table(BITMAPINFOHEADER *header);

#define RGBQUAD_TO_RGB555(rgbq)             \
  ((USHORT)((((rgbq).rgbRed >> 3) << 10) |  \
            (((rgbq).rgbGreen >> 3) << 5) | \
            ((rgbq).rgbBlue >> 3)))

// FUNCTION: MAGVID 0x10008880
MagDib::MagDib()
{
  header = NULL;
  bits = NULL;
  owns_bits = 1;
  transparent_color = NULL;
  reserved_14 = 0;
}

// FUNCTION: MAGVID 0x100088D4
MagDib::~MagDib()
{
  void *color;
  void *delete_ptr;

  if (header != NULL)
  {
    free(header);
  }
  if ((owns_bits != 0) && (bits != NULL))
  {
    free(bits);
  }
  if (transparent_color != NULL)
  {
    color = transparent_color;
    delete_ptr = color;
    operator delete(delete_ptr);
  }
}

// FUNCTION: MAGVID 0x10002CC0
BITMAPINFOHEADER *MagDib::get_header(void)
{
  return header;
}

// FUNCTION: MAGVID 0x10002CE0
void *MagDib::get_bits(void)
{
  return bits;
}

// FUNCTION: MAGVID 0x10007410
extern "C" void *__fastcall mag_dib_get_palette(MagDib *dib)
{
  return (dib->header->biBitCount > 8) ? NULL :
      (char *)dib->header + sizeof(BITMAPINFOHEADER);
}

// FUNCTION: MAGVID 0x10008B48
int MagDib::attach_external_bits(BITMAPINFOHEADER *source_header,
                                 void *source_bits)
{
  if (header != NULL)
  {
    free(header);
  }
  if (source_header->biBitCount == 8)
  {
    header = (BITMAPINFOHEADER *)malloc(0x428);
  }
  else
  {
    header = (BITMAPINFOHEADER *)malloc(0x28);
  }
  if (header == NULL)
  {
    return 0;
  }
  memcpy(header, source_header,
         bitmap_header_color_count(source_header) * 4 + 0x28);
  if ((owns_bits != 0) && (bits != NULL))
  {
    free(bits);
  }
  bits = source_bits;
  owns_bits = 0;
  return 1;
}

// FUNCTION: MAGVID 0x10008C35
int __cdecl bitmap_header_color_count(BITMAPINFOHEADER *header)
{
  struct
  {
    int color_count;
    int max_color_count;
    BITMAPINFOHEADER *header_again;
    int bit_count;
    BITMAPINFOHEADER *header;
  } local;

  local.header = header;
  local.header_again = local.header;
  if (bitmap_header_has_color_table(local.header) != 0)
  {
    local.bit_count = local.header->biBitCount;
  }
  else
  {
    local.bit_count = ((BITMAPCOREHEADER *)local.header_again)->bcBitCount;
  }
  switch (local.bit_count)
  {
  case 1:
    local.color_count = 2;
    break;
  case 4:
    local.color_count = 16;
    break;
  case 8:
    local.color_count = 256;
    break;
  default:
    local.color_count = 0;
    break;
  }
  if ((bitmap_header_has_color_table(local.header) != 0) &&
      (local.header->biClrUsed != 0))
  {
    local.color_count = local.header->biClrUsed;
  }
  local.max_color_count = 0;
  switch (local.bit_count)
  {
  case 1:
    local.max_color_count = 2;
    break;
  case 4:
    local.max_color_count = 16;
    break;
  case 8:
    local.max_color_count = 256;
    break;
  default:
    local.max_color_count = 0;
    break;
  }
  if ((local.max_color_count != 0) &&
      (local.max_color_count < local.color_count))
  {
    local.color_count = local.max_color_count;
  }
  if (local.bit_count > 8)
  {
    local.color_count = 0;
  }
  return local.color_count;
}

// FUNCTION: MAGVID 0x10008DAC
int __cdecl bitmap_header_has_color_table(BITMAPINFOHEADER *header)
{
  if (header->biSize == sizeof(BITMAPCOREHEADER))
  {
    return 0;
  }
  return 1;
}

// FUNCTION: MAGVID 0x10008DD4
int MagDib::load_bitmap(HWND window, char *path, unsigned int bit_count)
{
  struct
  {
    RGBQUAD *colors;
    unsigned char *destination_24;
    int row;
    int column;
    unsigned char *source;
    unsigned char *destination_16;
    DWORD read_error;
    BITMAPINFOHEADER source_header;
    int color_count;
    size_t bits_size;
    size_t header_size;
    HANDLE file;
    DWORD bytes_read;
    BITMAPINFO *new_header;
    unsigned char *bits;
    size_t palette_size;
    BITMAPFILEHEADER file_header;
    unsigned char *original_bits;
  } s;

  s.new_header = NULL;
  s.bits = NULL;
  s.original_bits = NULL;
  s.file = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL,
                       OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL);
  if (s.file == INVALID_HANDLE_VALUE)
  {
    goto cleanup;
  }
  if ((ReadFile(s.file, &s.file_header, sizeof(BITMAPFILEHEADER),
                &s.bytes_read, NULL) == 0) ||
      (s.bytes_read != sizeof(BITMAPFILEHEADER)))
  {
    s.read_error = GetLastError();
    CloseHandle(s.file);
    goto cleanup;
  }

  if (s.file_header.bfType != 0x4d42)
  {
    CloseHandle(s.file);
    goto cleanup;
  }

  if ((ReadFile(s.file, &s.source_header, sizeof(BITMAPINFOHEADER),
                &s.bytes_read, NULL) == 0) ||
      (s.bytes_read != sizeof(BITMAPINFOHEADER)))
  {
    CloseHandle(s.file);
    goto cleanup;
  }

  if (s.source_header.biSize != sizeof(BITMAPINFOHEADER))
  {
    CloseHandle(s.file);
    MessageBoxA(window, "Not a Windows DIB.", "KPlay error",
                MB_ICONEXCLAMATION);
    goto cleanup;
  }

  s.color_count = bitmap_header_color_count(&s.source_header);
  s.palette_size = s.color_count << 2;
  s.header_size = 0x428;
  s.bits_size = s.file_header.bfSize - s.file_header.bfOffBits;
  s.new_header = (BITMAPINFO *)malloc(s.header_size);
  if (s.new_header == NULL)
  {
    CloseHandle(s.file);
    goto cleanup;
  }

  memset(s.new_header, 0, s.header_size);
  memcpy(s.new_header, &s.source_header, sizeof(BITMAPINFOHEADER));
  s.new_header->bmiHeader.biBitCount = (WORD)bit_count;
  if ((s.color_count != 0) &&
      ((ReadFile(s.file, &s.new_header->bmiColors, s.palette_size,
                 &s.bytes_read, NULL) == 0) ||
       (s.palette_size != s.bytes_read)))
  {
    CloseHandle(s.file);
    goto cleanup;
  }

  s.bits = (unsigned char *)malloc(s.bits_size);
  s.original_bits = s.bits;
  if (s.bits == NULL)
  {
    CloseHandle(s.file);
    goto cleanup;
  }

  if ((ReadFile(s.file, s.bits, s.bits_size, &s.bytes_read, NULL) == 0) ||
      (s.bits_size != s.bytes_read))
  {
    goto cleanup;
  }

  if (s.source_header.biBitCount != bit_count)
  {
    s.bits_size = ((((int)(s.source_header.biWidth * bit_count) / 8) + 3) &
                   ~3) * s.source_header.biHeight;
    s.bits = (unsigned char *)malloc(s.bits_size);
    switch (bit_count)
    {
    case 0x18:
      s.source = s.original_bits;
      s.destination_24 = s.bits;
      s.colors = s.new_header->bmiColors;
      for (s.row = 0; s.row < s.source_header.biHeight; s.row++)
      {
        for (s.column = 0; s.column < s.source_header.biWidth; s.column++)
        {
          s.destination_24[2] =
              s.colors[(unsigned int)(unsigned char)*s.source].rgbRed;
          s.destination_24[1] =
              s.colors[(unsigned int)(unsigned char)*s.source].rgbGreen;
          s.destination_24[0] =
              s.colors[(unsigned int)(unsigned char)*s.source].rgbBlue;
          s.source++;
          s.destination_24 += 3;
        }
        s.source += ((s.source_header.biWidth + 3U) & 0xfffffffc) -
                    s.source_header.biWidth;
        s.destination_24 +=
            ((((int)(s.source_header.biWidth * bit_count) / 8) + 3U) &
              0xfffffffc) -
            ((int)(s.source_header.biWidth * bit_count) / 8);
      }
      break;
    case 0x10:
      s.source = s.original_bits;
      s.destination_16 = s.bits;
      s.colors = s.new_header->bmiColors;
      for (s.row = 0; s.row < s.source_header.biHeight; s.row++)
      {
        for (s.column = 0; s.column < s.source_header.biWidth; s.column++)
        {
          *(unsigned short *)s.destination_16 =
              RGBQUAD_TO_RGB555(
                  s.colors[(unsigned int)(unsigned char)*s.source]);
          s.source++;
          s.destination_16 += 2;
        }
        s.source += ((s.source_header.biWidth + 3U) & 0xfffffffc) -
                    s.source_header.biWidth;
        s.destination_16 +=
            ((((int)(s.source_header.biWidth * bit_count) / 8) + 3U) &
              0xfffffffc) -
            ((int)(s.source_header.biWidth * bit_count) / 8);
      }
      break;
    }
    free(s.original_bits);
    s.source_header.biBitCount = (WORD)bit_count;
  }
  if (header != NULL)
  {
    free(header);
  }
  header = &s.new_header->bmiHeader;
  if (bits != NULL)
  {
    free(bits);
  }
  bits = s.bits;
  owns_bits = 1;
  CloseHandle(s.file);
  return 1;

cleanup:
  if (s.new_header != NULL)
  {
    free(s.new_header);
  }
  if (s.bits != NULL)
  {
    free(s.bits);
  }
  return 0;
}

// FUNCTION: MAGVID 0x1000934A
int MagDib::save_bitmap(char *path)
{
  struct
  {
    int bits_size;
    BITMAPINFOHEADER *header;
    void *palette;
    HANDLE file;
    int bit_count;
    DWORD bytes_written;
    void *bits;
    int color_count;
    BITMAPFILEHEADER file_header;
    int unused;
  } s;

  s.unused = 0;
  s.color_count = 0;
  s.file = CreateFileA(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                       FILE_ATTRIBUTE_NORMAL, NULL);
  if (s.file == INVALID_HANDLE_VALUE)
  {
    return 2;
  }

  s.file_header.bfType = 0x4d42;
  s.file_header.bfReserved1 = 0;
  s.file_header.bfReserved2 = 0;
  s.color_count = mag_dib_get_color_count(this);
  s.bit_count = mag_dib_get_bit_count(this);
  s.file_header.bfSize = 0xe;
  s.file_header.bfSize += (s.color_count << 2) + 0x28;
  s.file_header.bfOffBits = s.file_header.bfSize;
  s.header = get_header();
  s.bits_size = s.header->biWidth * s.header->biHeight * (s.bit_count / 8);
  s.file_header.bfSize += s.bits_size;
  WriteFile(s.file, &s.file_header, sizeof(BITMAPFILEHEADER),
            &s.bytes_written, NULL);
  s.header->biSizeImage = s.bits_size;
  WriteFile(s.file, s.header, sizeof(BITMAPINFOHEADER), &s.bytes_written,
            NULL);
  if (s.color_count != 0)
  {
    s.palette = mag_dib_get_palette(this);
    WriteFile(s.file, s.palette, s.color_count << 2, &s.bytes_written, NULL);
  }
  s.bits = get_bits();
  WriteFile(s.file, s.bits, s.bits_size, &s.bytes_written, NULL);
  CloseHandle(s.file);
  return 0;
}

// FUNCTION: MAGVID 0x100094A8
extern "C" int __fastcall mag_dib_get_color_count(MagDib *dib)
{
  return bitmap_header_color_count(dib->header);
}

// FUNCTION: MAGVID 0x100094CD
int MagDib::map_to_palette(HPALETTE palette)
{
  struct
  {
    PALETTEENTRY entries[256];
    unsigned int palette_index;
    int bits_size;
    unsigned char *bits;
    unsigned char index_map[256];
    int remapped_count;
    unsigned char *dib_palette;
  } s;

  if (palette == NULL)
  {
    return 0;
  }
  if (header->biBitCount != 8)
  {
    return 0;
  }
  if (reserved_14 != 0)
  {
    return 1;
  }

  s.dib_palette = (unsigned char *)mag_dib_get_palette(this);
  s.remapped_count = 0;
  for (s.palette_index = 0; s.palette_index < 0x100; s.palette_index++)
  {
    s.index_map[s.palette_index] =
        (unsigned char)GetNearestPaletteIndex(
            palette, RGB(s.dib_palette[2], s.dib_palette[1],
                         s.dib_palette[0]));
    s.dib_palette += 4;
    if (s.index_map[s.palette_index] != s.palette_index)
    {
      s.remapped_count++;
    }
    if ((transparent_color != NULL) &&
        (*(unsigned int *)transparent_color == s.palette_index))
    {
      mag_dib_release_transparent_color(this);
      set_transparent_color(s.index_map[s.palette_index]);
    }
  }

  s.bits = (unsigned char *)get_bits();
  s.bits_size = bitmap_height() * mag_dib_get_stride(this);
  while (s.bits_size-- != 0)
  {
    *s.bits = s.index_map[*s.bits];
    s.bits++;
  }

  GetPaletteEntries(palette, 0, 0x100, s.entries);
  s.dib_palette = (unsigned char *)mag_dib_get_palette(this);
  for (s.palette_index = 0; s.palette_index < 0x100; s.palette_index++)
  {
    s.dib_palette[2] = s.entries[s.palette_index].peRed;
    s.dib_palette[1] = s.entries[s.palette_index].peGreen;
    s.dib_palette[0] = s.entries[s.palette_index].peBlue;
    s.dib_palette += 4;
  }
  reserved_14 = 1;
  return 1;
}

// FUNCTION: MAGVID 0x1000972F
void *MagDib::get_pixel_address(int x, int y)
{
  unsigned int stride;

  if ((x >= bitmap_width()) || (y >= bitmap_height()))
  {
    return NULL;
  }

  stride = mag_dib_get_stride(this);
  return (unsigned char *)bits +
         (bitmap_height() - y - 1) * stride +
         ((header->biBitCount * x / 8));
}

// FUNCTION: MAGVID 0x100097B2
void MagDib::get_bounds_rect(RECT *rect)
{
  rect->top = 0;
  rect->left = 0;
  rect->bottom = bitmap_height();
  rect->right = bitmap_width();
}

// FUNCTION: MAGVID 0x100097F9
void MagDib::blit_to(MagDib *destination, int x, int y, int width,
                     int height, int source_x, int source_y)
{
  struct
  {
    int memmove_width;
    unsigned int color;
    int pixel_offset;
    int color_byte;
    int bytes_per_pixel;
    int pixel_index;
    unsigned int color_mask;
    int row_width_bytes;
    int byte_index;
    void *source_line;
    void *destination_line;
    unsigned int destination_stride;
    unsigned int source_stride;
  } s;

  if (source_x < 0)
  {
    width += source_x;
    x -= source_x;
    source_x = 0;
  }
  else
  {
    if (bitmap_width() - width < source_x)
    {
      if (bitmap_width() > source_x)
      {
        width = bitmap_width() - source_x;
      }
      else
      {
        return;
      }
    }
  }
  if (x < 0)
  {
    width += x;
    source_x -= x;
    x = 0;
  }
  else if ((0 < width) && (destination->bitmap_width() - width < x))
  {
    width = destination->bitmap_width() - x;
  }

  if (source_y < 0)
  {
    height += source_y;
    y -= source_y;
    source_y = 0;
  }
  else
  {
    if (bitmap_height() - height < source_y)
    {
      if (bitmap_height() > source_y)
      {
        height = bitmap_height() - source_y;
      }
      else
      {
        return;
      }
    }
  }
  if (y < 0)
  {
    height += y;
    source_y -= y;
    y = 0;
  }
  else if ((0 < height) &&
           (destination->bitmap_height() - height < y))
  {
    height = destination->bitmap_height() - y;
  }

  if ((width <= 0) || (height <= 0))
  {
    return;
  }

  if (bitmap_height() - source_y <= height)
  {
    height = bitmap_height() - source_y;
  }
  else
  {
  }

  if (destination->bitmap_height() - y <= height)
  {
    height = destination->bitmap_height() - y;
  }
  else
  {
  }

  s.source_line = get_pixel_address(source_x, source_y + height - 1);
  s.destination_line = destination->get_pixel_address(x, y + height - 1);
  s.source_stride = mag_dib_get_stride(this);
  s.destination_stride = mag_dib_get_stride(destination);

  if (bitmap_width() - source_x <= width)
  {
    width = bitmap_width() - source_x;
  }
  else
  {
  }

  if (destination->bitmap_width() - x <= width)
  {
    width = destination->bitmap_width() - x;
  }
  else
  {
  }

  if (width <= 0)
  {
    return;
  }

  if (mag_dib_get_bit_count(this) <= 8)
  {
    s.row_width_bytes = (header->biBitCount * width) / 8;
    while (height--)
    {
      for (s.byte_index = 0; s.byte_index < s.row_width_bytes;
           s.byte_index++)
      {
        if ((transparent_color != NULL) &&
            ((unsigned int)((unsigned char *)s.source_line)[s.byte_index] ==
             *(unsigned int *)transparent_color))
        {
        }
        else
        {
          ((unsigned char *)s.destination_line)[s.byte_index] =
              ((unsigned char *)s.source_line)[s.byte_index];
        }
      }
      s.source_line = (unsigned char *)s.source_line + s.source_stride;
      s.destination_line =
          (unsigned char *)s.destination_line + s.destination_stride;
    }
  }
  else
  {
    s.bytes_per_pixel = header->biBitCount >> 3;
    s.color_mask = 0xffffff;
    if (transparent_color != NULL)
    {
      while (height--)
      {
        for (s.pixel_index = 0, s.pixel_offset = 0;
             width > s.pixel_index;
             s.pixel_index++, s.pixel_offset += s.bytes_per_pixel)
        {
          s.color = *(unsigned int *)((unsigned char *)s.source_line +
                                      s.pixel_offset);
          s.color &= s.color_mask;
          if (*(unsigned int *)transparent_color == s.color)
          {
          }
          else
          {
            for (s.color_byte = 0; s.bytes_per_pixel > s.color_byte;
                 s.color_byte++)
            {
              ((unsigned char *)s.destination_line)
                  [s.pixel_offset + s.color_byte] =
                      ((unsigned char *)&s.color)[s.color_byte];
            }
          }
        }
        s.source_line = (unsigned char *)s.source_line + s.source_stride;
        s.destination_line =
            (unsigned char *)s.destination_line + s.destination_stride;
      }
    }
    else
    {
      s.memmove_width = width * s.bytes_per_pixel;
      while (height--)
      {
        memmove(s.destination_line, s.source_line, s.memmove_width);
        s.source_line = (unsigned char *)s.source_line + s.source_stride;
        s.destination_line =
            (unsigned char *)s.destination_line + s.destination_stride;
      }
    }
  }
}

// FUNCTION: MAGVID 0x10009CC0
int MagDib::get_width(void)
{
  return bitmap_width();
}

// FUNCTION: MAGVID 0x10009CE0
int MagDib::get_height(void)
{
  return bitmap_height();
}

// FUNCTION: MAGVID 0x10009D00
int MagDib::bitmap_width(void)
{
  return header->biWidth;
}

// FUNCTION: MAGVID 0x10009D20
int MagDib::bitmap_height(void)
{
  return header->biHeight;
}

// FUNCTION: MAGVID 0x10009D40
extern "C" int __fastcall mag_dib_get_bit_count(MagDib *dib)
{
  return dib->header->biBitCount;
}

// FUNCTION: MAGVID 0x10009D70
void MagDib::set_transparent_color(int value)
{
  transparent_color = operator new(4);
  if (transparent_color != NULL)
  {
    *(int *)transparent_color = value;
  }
}

// FUNCTION: MAGVID 0x10009DB0
extern "C" void __fastcall mag_dib_release_transparent_color(MagDib *dib)
{
  void *delete_ptr;
  void *transparent_color;

  if (dib->transparent_color != NULL)
  {
    delete_ptr = dib->transparent_color;
    transparent_color = delete_ptr;
    operator delete(transparent_color);
  }
}

// FUNCTION: MAGVID 0x10009DF0
extern "C" unsigned int __fastcall mag_dib_get_stride(MagDib *dib)
{
  return ((dib->header->biBitCount * dib->header->biWidth) / 8 + 3) & ~3;
}
