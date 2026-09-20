#ifndef MAGVID_MAG_DIB_H
#define MAGVID_MAG_DIB_H

#include <windows.h>

// Polymorphic bitmap surface used by MAGVID foreground/background exports.
// This is the same 0x18-byte DIB interface used by the client DLLs.
// VTABLE: MAGVID 0x1000B020
class MagDib
{
public:
  BITMAPINFOHEADER *header;       // 0x04
  void *bits;                    // 0x08
  int owns_bits;                 // 0x0c
  void *transparent_color;       // 0x10
  int reserved_14;               // 0x14

  MagDib();
  // SYNTHETIC: MAGVID 0x10002C80
  // MagDib::`scalar deleting destructor'
  ~MagDib();

  virtual int load_bitmap(HWND window, char *path, unsigned int bit_count);
  virtual int save_bitmap(char *path);
  virtual int get_width(void);
  virtual int get_height(void);
  virtual int map_to_palette(HPALETTE palette);
  virtual void get_bounds_rect(RECT *rect);
  virtual void blit_to(MagDib *destination, int x, int y, int width,
                       int height, int source_x, int source_y);

  int bitmap_width(void);
  int bitmap_height(void);
  void *get_pixel_address(int x, int y);
  void set_transparent_color(int value);
  BITMAPINFOHEADER *get_header(void);
  void *get_bits(void);
  int attach_external_bits(BITMAPINFOHEADER *source_header,
                           void *source_bits);
};

extern "C" int __fastcall mag_dib_get_color_count(MagDib *dib);
extern "C" void *__fastcall mag_dib_get_palette(MagDib *dib);
extern "C" int __fastcall mag_dib_get_bit_count(MagDib *dib);
extern "C" void __fastcall mag_dib_release_transparent_color(MagDib *dib);
extern "C" unsigned int __fastcall mag_dib_get_stride(MagDib *dib);

#endif
