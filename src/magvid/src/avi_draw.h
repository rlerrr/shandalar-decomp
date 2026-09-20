#ifndef MAGVID_AVI_DRAW_H
#define MAGVID_AVI_DRAW_H

#include <windows.h>
#include <vfw.h>

#include "mag_dib.h"

struct MagVid;

// Video-for-Windows decompression and DrawDib state.  The original object is
// 0x1b0 bytes; additional reserved regions will be named as its methods are
// reconstructed.
class AviDraw
{
public:
  AviDraw(void);
  // SYNTHETIC: MAGVID 0x10007E70
  // AviDraw::`scalar deleting destructor'
  ~AviDraw(void);
  int release_resources(void);
  int open_codec(DWORD handler, BITMAPINFOHEADER *format);
  int begin_draw(HDC device_context, float frame_period);
  DWORD draw_frame(void *compressed_frame, DWORD flags);
  int catch_up_frame(void *compressed_frame, DWORD flags);
  int start_codec_playback(float frame_period);
  int stop_codec_playback(void);
  int end_draw(void);
  void get_source_rect(RECT *rect);
  void set_source_rect(const RECT *rect);
  void get_destination_rect(RECT *rect);
  void set_destination_rect(const RECT *rect);
  void get_draw_rect(RECT *rect);
  void set_draw_rect(const RECT *rect);
  void get_present_rect(RECT *rect);
  void set_present_rect(const RECT *rect);
  int get_decoded_rect(RECT *rect);
  void get_source_format(void *format);
  void set_source_format(const void *format);
  void get_decoded_format(void *format);
  int set_palette(LOGPALETTE *palette);
  int set_transparency(int enabled);
  int set_overlay_target(MagDib *target);
  int set_foreground(MagDib *foreground);

private:
  struct StoredRect
  {
    int x;
    int y;
    int width;
    int height;
  };

  HIC codec;                    // 0x000
  HDC target_dc;               // 0x004
  HDRAWDIB draw_dib;            // 0x008
  DWORD stream_handler;        // 0x00c
  int status;                   // 0x010
  unsigned char *source_frame;  // 0x014
  unsigned char *decoded_frame; // 0x018
  StoredRect source_rect;       // 0x01c
  StoredRect destination_rect;  // 0x02c
  StoredRect draw_rect;         // 0x03c
  StoredRect present_rect;      // 0x04c
  struct CodecControl
  {
    unsigned char reserved_00[0x14];
    int flags;                  // +0x14
    unsigned char reserved_18[0x0c];
    int transparency;           // +0x24
    int reserved_28;
  } codec_control;              // 0x05c
  unsigned char reserved_088[0xd4]; // 0x088
  StoredRect previous_decoded_rect; // 0x15c
  StoredRect decoded_rect;      // 0x16c
  MagDib *offscreen_surface;    // 0x17c
  MagDib *overlay_target;       // 0x180
  MagDib *foreground;           // 0x184
  int overlay_enabled;          // 0x188
  HBITMAP output_bitmap;        // 0x18c
  void *external_frame_buffer;  // 0x190
  int reserved_194;             // 0x194
  int frame_buffer_size;        // 0x198
  void *allocated_frame_buffer; // 0x19c
  HDC offscreen_dc;             // 0x1a0
  HGDIOBJ old_bitmap;           // 0x1a4
  HPALETTE old_target_palette;  // 0x1a8
  HPALETTE old_offscreen_palette; // 0x1ac

  friend void __cdecl realize_video_palette(MagVid *state, HWND excluded);
};

#endif
