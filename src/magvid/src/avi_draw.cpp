#include "avi_draw.h"

#include <string.h>

// GLOBAL: MAGVID 0x10015A10
static int g_use_draw_dib;

LRESULT __stdcall codec_draw_recommend(HIC codec, int p2, int p3, int p4,
                                       int p5, int p6, int p7, int p8,
                                       int p9, int p10, int p11, int p12,
                                       int p13, int p14);
LRESULT __stdcall codec_draw_begin(HIC codec, int p2, int p3, int p4,
                                   int p5, int p6, int p7, int p8, int p9,
                                   int p10, int p11, int p12, int p13,
                                    int p14);
LRESULT __stdcall codec_draw_frame(HIC codec, int p2, int p3, int p4,
                                   int p5, int p6, int p7, int p8, int p9,
                                   int p10, int p11, int p12, int p13,
                                   int p14);

// FUNCTION: MAGVID 0x10004040
AviDraw::AviDraw(void)
{
  memset(this, 0, sizeof(AviDraw));
  status = 0;
  source_frame = NULL;
  decoded_frame = NULL;
  external_frame_buffer = NULL;
  allocated_frame_buffer = NULL;
  overlay_enabled = 0;
  offscreen_surface = NULL;
  overlay_target = NULL;
  foreground = NULL;
  codec = NULL;
}

// FUNCTION: MAGVID 0x100040E1
AviDraw::~AviDraw(void)
{
  release_resources();
}

// FUNCTION: MAGVID 0x100040FF
int AviDraw::open_codec(DWORD handler, BITMAPINFOHEADER *format)
{
  int row_bits;

  status = 0;
  if (codec != NULL)
  {
    release_resources();
  }
  draw_dib = DrawDibOpen();
  output_bitmap = NULL;
  if (handler == 0)
  {
    stream_handler = format->biCompression;
  }
  else
  {
    stream_handler = handler;
  }
  if (stream_handler == mmioFOURCC('I', 'V', '4', '1'))
  {
    stream_handler = mmioFOURCC('i', 'v', '4', '1');
  }
  codec = ICLocate(ICTYPE_VIDEO, handler, format, NULL,
                   ICMODE_DECOMPRESS);
  if (codec == NULL)
  {
    return 0x80044071;
  }
  ICSendMessage(codec, ICM_DRAW_QUERY, 0, 0);
  source_frame = (unsigned char *)operator new(0x428);
  decoded_frame = (unsigned char *)operator new(0x428);
  if ((source_frame == NULL) || (decoded_frame == NULL))
  {
    return 3;
  }
  memset(decoded_frame, 0, 0x428);
  memset(source_frame, 0, 0x428);
  memcpy(source_frame, format, sizeof(BITMAPINFOHEADER));
  memcpy(decoded_frame, source_frame, sizeof(BITMAPINFOHEADER));
  ((BITMAPINFOHEADER *)decoded_frame)->biBitCount = 24;
  ((BITMAPINFOHEADER *)decoded_frame)->biCompression = BI_RGB;
  ((BITMAPINFOHEADER *)decoded_frame)->biSizeImage = 0;
  source_rect.x = 0;
  source_rect.y = 0;
  source_rect.width = format->biWidth;
  source_rect.height = format->biHeight;
  destination_rect.x = 0;
  destination_rect.y = 0;
  destination_rect.width = format->biWidth;
  destination_rect.height = format->biHeight;
  draw_rect.x = 0;
  draw_rect.y = 0;
  draw_rect.width = format->biWidth;
  draw_rect.height = format->biHeight;
  present_rect.x = 0;
  present_rect.y = 0;
  present_rect.width = format->biWidth;
  present_rect.height = format->biHeight;
  row_bits = ((format->biWidth + 3) / 4) * format->biBitCount *
             format->biHeight * 4;
  frame_buffer_size = (row_bits + 7) / 8;
  allocated_frame_buffer = malloc(frame_buffer_size);
  if (allocated_frame_buffer == NULL)
  {
    return 3;
  }
  return 0;
}

// FUNCTION: MAGVID 0x1000454C
int AviDraw::release_resources(void)
{
  if (source_frame != NULL)
  {
    delete source_frame;
  }
  if (decoded_frame != NULL)
  {
    delete decoded_frame;
  }
  decoded_frame = NULL;
  source_frame = decoded_frame;
  if (allocated_frame_buffer != NULL)
  {
    free(allocated_frame_buffer);
    allocated_frame_buffer = NULL;
  }
  if (draw_dib != NULL)
  {
    DrawDibClose(draw_dib);
    draw_dib = NULL;
  }
  if (codec != NULL)
  {
    ICClose(codec);
    codec = NULL;
  }
  if (offscreen_surface != NULL)
  {
    offscreen_surface = NULL;
  }
  if (overlay_target != NULL)
  {
    overlay_target = NULL;
  }
  codec = NULL;
  stream_handler = 0;
  return 0;
}

// FUNCTION: MAGVID 0x1000468A
int AviDraw::begin_draw(HDC device_context, float frame_period)
{
  int result;
  int draw_dib_result;
  int draw_rate;

  if ((codec == NULL) || (device_context == NULL))
  {
    return -1;
  }
  target_dc = device_context;
  offscreen_dc = CreateCompatibleDC(target_dc);
  result = codec_draw_begin(codec, 0, (int)source_frame, 0,
                            source_rect.x, source_rect.y,
                            source_rect.width, source_rect.height,
                            (int)decoded_frame, 0,
                            destination_rect.x, destination_rect.y,
                            destination_rect.width, destination_rect.height);
  if (result != 0)
  {
    return result;
  }
  g_use_draw_dib = 1;
  result = codec_draw_recommend(codec, 0, (int)source_frame, 0,
                                source_rect.x, source_rect.y,
                                source_rect.width, source_rect.height,
                                (int)decoded_frame, 0,
                                destination_rect.x, destination_rect.y,
                                destination_rect.width,
                                destination_rect.height);
  if (overlay_target != NULL)
  {
    offscreen_surface = new MagDib;
    if ((offscreen_surface != NULL) && (g_use_draw_dib != 0))
    {
      offscreen_surface->attach_external_bits(
          (BITMAPINFOHEADER *)decoded_frame, allocated_frame_buffer);
    }
    else if (offscreen_surface != NULL)
    {
      offscreen_surface->attach_external_bits(
          (BITMAPINFOHEADER *)decoded_frame, external_frame_buffer);
    }
    overlay_target->blit_to(offscreen_surface, 0, 0,
                            destination_rect.width,
                            destination_rect.height,
                            draw_rect.x, draw_rect.y);
  }
  if (g_use_draw_dib != 0)
  {
    draw_dib_result = DrawDibBegin(draw_dib, NULL,
                                   draw_rect.width, draw_rect.height,
                                   (BITMAPINFOHEADER *)decoded_frame,
                                   destination_rect.width,
                                   destination_rect.height, 0);
    draw_rate = (int)((1.0 / frame_period) * 1000000.0);
    draw_dib_result = DrawDibStart(draw_dib, draw_rate);
  }
  SetStretchBltMode(target_dc, 3);
  status = 1;
  return 0;
}

// FUNCTION: MAGVID 0x10004CCB
DWORD AviDraw::draw_frame(void *compressed_frame, DWORD flags)
{
  struct
  {
    void *frame_buffer;
    DWORD result;
    int destination_y;
    int destination_x;
  } local;

  if (status == 0)
  {
    return 0;
  }
  if (g_use_draw_dib != 0)
  {
    local.frame_buffer = allocated_frame_buffer;
  }
  else
  {
    local.frame_buffer = external_frame_buffer;
  }
  if ((overlay_target != NULL) && (overlay_enabled != 0))
  {
    overlay_target->blit_to(
        offscreen_surface, decoded_rect.x, decoded_rect.y,
        decoded_rect.width, decoded_rect.height,
        decoded_rect.x + draw_rect.x, decoded_rect.y + draw_rect.y);
  }
  local.destination_x = destination_rect.x;
  local.destination_y = destination_rect.y;
  local.result = codec_draw_frame(codec, flags, (int)source_frame,
                                  (int)compressed_frame,
                                  source_rect.x, source_rect.y,
                                  source_rect.width, source_rect.height,
                                  (int)decoded_frame,
                                  (int)local.frame_buffer, 0, 0,
                                  destination_rect.width,
                                  destination_rect.height);
  if (local.result != 0)
  {
    return local.result;
  }
  if ((overlay_target != NULL) && (overlay_enabled != 0))
  {
    memcpy(&decoded_rect, &previous_decoded_rect, sizeof(StoredRect));
  }
  if (foreground != NULL)
  {
    foreground->blit_to(offscreen_surface, 0, 0,
                        offscreen_surface->get_width(),
                        offscreen_surface->get_height(), 0, 0);
  }
  if (g_use_draw_dib == 0)
  {
    if ((draw_rect.width == destination_rect.width) &&
        (draw_rect.height == destination_rect.height))
    {
      local.result = BitBlt(target_dc, present_rect.x, present_rect.y,
                            present_rect.width, present_rect.height,
                            offscreen_dc, 0, 0, SRCCOPY);
      if (local.result == 0)
      {
        local.result = GetLastError();
        return local.result;
      }
    }
    else
    {
      local.result = StretchBlt(target_dc, draw_rect.x, draw_rect.y,
                                draw_rect.width, draw_rect.height,
                                offscreen_dc, destination_rect.x,
                                destination_rect.y,
                                destination_rect.width,
                                destination_rect.height, SRCCOPY);
      if (local.result == 0)
      {
        local.result = GetLastError();
        return local.result;
      }
    }
    GdiFlush();
  }
  else
  {
    local.result = DrawDibDraw(draw_dib, target_dc, draw_rect.x,
                               draw_rect.y, draw_rect.width,
                               draw_rect.height,
                               (BITMAPINFOHEADER *)decoded_frame,
                               local.frame_buffer, 0, 0,
                               destination_rect.width,
                               destination_rect.height, 0);
    if (local.result == 0)
    {
      return local.result;
    }
  }
  return 0;
}

// FUNCTION: MAGVID 0x100050B2
int AviDraw::catch_up_frame(void *compressed_frame, DWORD flags)
{
  void *frame_buffer;
  int result;

  if (status == 0)
  {
    return 0;
  }
  if (g_use_draw_dib != 0)
  {
    frame_buffer = external_frame_buffer;
  }
  else
  {
    frame_buffer = allocated_frame_buffer;
  }
  result = codec_draw_frame(codec, flags, (int)source_frame,
                            (int)compressed_frame, 0, 0,
                            source_rect.width, source_rect.height,
                            (int)decoded_frame, (int)frame_buffer, 0, 0,
                            source_rect.width, source_rect.height);
  if (result != 0)
  {
    return result;
  }
  return 0;
}

// FUNCTION: MAGVID 0x10004BEC
int AviDraw::start_codec_playback(float frame_period)
{
  ICDrawBegin(codec, 0, NULL, NULL, NULL, 0, 0, 0, 0, NULL, 0, 0, 0, 0,
              (DWORD)((1.0 / frame_period) * 1000000.0), 1000000);
  ICSendMessage(codec, ICM_DRAW_START, 0, 0);
  return 0;
}

// FUNCTION: MAGVID 0x10004C5C
int AviDraw::stop_codec_playback(void)
{
  ICSendMessage(codec, ICM_DRAW_STOP, 0, 0);
  ICSendMessage(codec, ICM_DRAW_END, 0, 0);
  return 0;
}

// FUNCTION: MAGVID 0x10004A66
int AviDraw::end_draw(void)
{
  int result;

  status = 0;
  if (target_dc == NULL)
  {
    return -1;
  }
  result = ICSendMessage(codec, 0x403f, 0, 0);
  if (offscreen_surface != NULL)
  {
    delete offscreen_surface;
  }
  if (g_use_draw_dib != 0)
  {
    DrawDibStop(draw_dib);
    DrawDibEnd(draw_dib);
  }
  if (old_target_palette != NULL)
  {
    SelectPalette(target_dc, old_target_palette, FALSE);
  }
  if (old_offscreen_palette != NULL)
  {
    SelectPalette(offscreen_dc, old_offscreen_palette, FALSE);
  }
  if (output_bitmap != NULL)
  {
    DeleteObject(output_bitmap);
    output_bitmap = NULL;
  }
  if (old_bitmap != NULL)
  {
    SelectObject(offscreen_dc, old_bitmap);
  }
  if (offscreen_dc != NULL)
  {
    DeleteDC(offscreen_dc);
  }
  target_dc = NULL;
  return 0;
}

// FUNCTION: MAGVID 0x10004978
LRESULT __stdcall codec_draw_recommend(HIC codec, int p2, int p3, int p4,
                                       int p5, int p6, int p7, int p8,
                                       int p9, int p10, int p11, int p12,
                                       int p13, int p14)
{
  struct
  {
    int p2;
    int p3;
    int p4;
    int p9;
    int p10;
    int p11;
    int p12;
    int p13;
    int p14;
    int p5;
    int p6;
    int p7;
    int p8;
  } local;

  local.p2 = p2;
  local.p3 = p3;
  local.p4 = p4;
  local.p5 = p5;
  local.p6 = p6;
  local.p7 = p7;
  local.p8 = p8;
  local.p9 = p9;
  local.p10 = p10;
  local.p11 = p11;
  local.p12 = p12;
  local.p13 = p13;
  local.p14 = p14;
  return ICSendMessage(codec, 0x403c, (DWORD)&local, 0x34);
}

// FUNCTION: MAGVID 0x100049EF
LRESULT __stdcall codec_draw_begin(HIC codec, int p2, int p3, int p4,
                                   int p5, int p6, int p7, int p8, int p9,
                                   int p10, int p11, int p12, int p13,
                                   int p14)
{
  struct
  {
    int p2;
    int p3;
    int p4;
    int p9;
    int p10;
    int p11;
    int p12;
    int p13;
    int p14;
    int p5;
    int p6;
    int p7;
    int p8;
  } local;

  local.p2 = p2;
  local.p3 = p3;
  local.p4 = p4;
  local.p5 = p5;
  local.p6 = p6;
  local.p7 = p7;
  local.p8 = p8;
  local.p9 = p9;
  local.p10 = p10;
  local.p11 = p11;
  local.p12 = p12;
  local.p13 = p13;
  local.p14 = p14;
  return ICSendMessage(codec, 0x403d, (DWORD)&local, 0x34);
}

// FUNCTION: MAGVID 0x1000503B
LRESULT __stdcall codec_draw_frame(HIC codec, int p2, int p3, int p4,
                                   int p5, int p6, int p7, int p8, int p9,
                                   int p10, int p11, int p12, int p13,
                                   int p14)
{
  struct
  {
    int p2;
    int p3;
    int p4;
    int p9;
    int p10;
    int p11;
    int p12;
    int p13;
    int p14;
    int p5;
    int p6;
    int p7;
    int p8;
  } local;

  local.p2 = p2;
  local.p3 = p3;
  local.p4 = p4;
  local.p5 = p5;
  local.p6 = p6;
  local.p7 = p7;
  local.p8 = p8;
  local.p9 = p9;
  local.p10 = p10;
  local.p11 = p11;
  local.p12 = p12;
  local.p13 = p13;
  local.p14 = p14;
  return ICSendMessage(codec, 0x403e, (DWORD)&local, 0x34);
}

// FUNCTION: MAGVID 0x10005168
void AviDraw::get_source_rect(RECT *rect)
{
  rect->left = source_rect.x;
  rect->top = source_rect.y;
  rect->right = source_rect.x + source_rect.width;
  rect->bottom = source_rect.y + source_rect.height;
}

// FUNCTION: MAGVID 0x100051BB
void AviDraw::set_source_rect(const RECT *rect)
{
  source_rect.x = rect->left;
  source_rect.y = rect->top;
  source_rect.width = rect->right - rect->left;
  source_rect.height = rect->bottom - rect->top;
}

// FUNCTION: MAGVID 0x1000520D
void AviDraw::get_destination_rect(RECT *rect)
{
  rect->left = destination_rect.x;
  rect->top = destination_rect.y;
  rect->right = destination_rect.x + destination_rect.width;
  rect->bottom = destination_rect.y + destination_rect.height;
}

// FUNCTION: MAGVID 0x10005260
void AviDraw::set_destination_rect(const RECT *rect)
{
  destination_rect.x = rect->left;
  destination_rect.y = rect->top;
  destination_rect.width = rect->right - rect->left;
  destination_rect.height = rect->bottom - rect->top;
}

// FUNCTION: MAGVID 0x100052B2
void AviDraw::get_draw_rect(RECT *rect)
{
  rect->left = draw_rect.x;
  rect->top = draw_rect.y;
  rect->right = draw_rect.x + draw_rect.width;
  rect->bottom = draw_rect.y + draw_rect.height;
}

// FUNCTION: MAGVID 0x10005305
void AviDraw::set_draw_rect(const RECT *rect)
{
  draw_rect.x = rect->left;
  draw_rect.y = rect->top;
  draw_rect.width = rect->right - rect->left;
  draw_rect.height = rect->bottom - rect->top;
  if (overlay_target == NULL)
  {
    present_rect.x = draw_rect.x;
    present_rect.y = draw_rect.y;
    present_rect.width = draw_rect.width;
    present_rect.height = draw_rect.height;
  }
}

// FUNCTION: MAGVID 0x10005397
void AviDraw::get_present_rect(RECT *rect)
{
  rect->left = present_rect.x;
  rect->top = present_rect.y;
  rect->right = present_rect.x + present_rect.width;
  rect->bottom = present_rect.y + present_rect.height;
}

// FUNCTION: MAGVID 0x100053EA
void AviDraw::set_present_rect(const RECT *rect)
{
  present_rect.x = rect->left;
  present_rect.y = rect->top;
  present_rect.width = rect->right - rect->left;
  present_rect.height = rect->bottom - rect->top;
}

// FUNCTION: MAGVID 0x1000543C
int AviDraw::get_decoded_rect(RECT *rect)
{
  if (stream_handler != mmioFOURCC('i', 'v', '4', '1'))
  {
    return -1;
  }
  rect->left = decoded_rect.x;
  rect->top = decoded_rect.y;
  rect->right = decoded_rect.width + rect->left;
  rect->bottom = decoded_rect.height + rect->top;
  return 0;
}

// FUNCTION: MAGVID 0x100054B6
void AviDraw::get_source_format(void *format)
{
  memcpy(format, source_frame, sizeof(BITMAPINFOHEADER));
}

// FUNCTION: MAGVID 0x100054E3
void AviDraw::set_source_format(const void *format)
{
  memcpy(source_frame, format, sizeof(BITMAPINFOHEADER));
}

// FUNCTION: MAGVID 0x10005510
void AviDraw::get_decoded_format(void *format)
{
  memcpy(format, decoded_frame, sizeof(BITMAPINFOHEADER));
}

// FUNCTION: MAGVID 0x10005756
int AviDraw::set_palette(LOGPALETTE *palette)
{
  int result;
  unsigned int entry_count;
  int entry;

  if (palette == NULL)
  {
    result = ICSendMessage(codec, ICM_DRAW_QUERY, 0, 0);
  }
  else
  {
    entry_count = palette->palNumEntries;
    if (entry_count > 0xec)
    {
      entry_count = 0xec;
    }
    for (entry = 0; entry < (int)entry_count; entry++)
    {
      ((unsigned char *)decoded_frame)[0x52 + entry * 4] =
          palette->palPalEntry[entry].peRed;
      ((unsigned char *)decoded_frame)[0x51 + entry * 4] =
          palette->palPalEntry[entry].peGreen;
      ((unsigned char *)decoded_frame)[0x50 + entry * 4] =
          palette->palPalEntry[entry].peBlue;
    }
    ((BITMAPINFOHEADER *)decoded_frame)->biClrUsed = 0x100;
    result = ICSendMessage(codec, ICM_DRAW_QUERY,
                           (DWORD)decoded_frame, 0);
    if (result != 0)
    {
      ICSendMessage(codec, ICM_DRAW_QUERY, 0, 0);
    }
  }
  return result == 0;
}

// FUNCTION: MAGVID 0x100059A3
int AviDraw::set_transparency(int enabled)
{
  if (overlay_target == NULL)
  {
    return 1;
  }
  if (enabled == 1)
  {
    codec_control.transparency = 0;
    overlay_enabled = 1;
  }
  else
  {
    codec_control.transparency = 0;
    overlay_enabled = 0;
  }
  codec_control.flags = 0x80000004;
  ICSendMessage(codec, 0x5001, (DWORD)&codec_control, sizeof(codec_control));
  return 0;
}

// FUNCTION: MAGVID 0x1000587E
int AviDraw::set_overlay_target(MagDib *target)
{
  if (stream_handler != mmioFOURCC('i', 'v', '4', '1'))
  {
    return 0;
  }
  if ((target->get_width() < draw_rect.width) ||
      (target->get_height() < draw_rect.height))
  {
    return 0;
  }
  if (overlay_target != NULL)
  {
    delete overlay_target;
  }
  if (target == NULL)
  {
    return 0;
  }
  overlay_target = target;
  present_rect.y = 0;
  present_rect.x = present_rect.y;
  present_rect.width = overlay_target->get_width();
  present_rect.height = overlay_target->get_height();
  destination_rect.x = 0;
  destination_rect.y = 0;
  return 1;
}

// FUNCTION: MAGVID 0x10005A3D
int AviDraw::set_foreground(MagDib *new_foreground)
{
  if (stream_handler != mmioFOURCC('i', 'v', '4', '1'))
  {
    return 0;
  }
  if ((new_foreground->get_width() < draw_rect.width) ||
      (new_foreground->get_height() < draw_rect.height))
  {
    return 0;
  }
  if (foreground != NULL)
  {
    delete overlay_target;
  }
  if (new_foreground == NULL)
  {
    return 0;
  }
  foreground = new_foreground;
  return 1;
}
