#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "shandalar.h"
#include "magic/src/global_state.h"
#include "shandalar_global_strings.h"
#include "facemaker/src/facemaker_types.h"
#include "drawcardlib/src/pic.h"


// GLOBAL: SHANDALAR 0x0059076c
int g_world_ui_top_offset;
// GLOBAL: SHANDALAR 0x00590798
signed char g_lair_sprite_index_by_color[8] = {0, 4, 0, 3, 2, 1, 0, 0};

// GLOBAL: SHANDALAR 0x006506e4
int g_world_camera_initialized;
// GLOBAL: SHANDALAR 0x006506ec
int g_world_camera_viewport_top;
// GLOBAL: SHANDALAR 0x006506f0
int g_world_draw_entry_count;
// GLOBAL: SHANDALAR 0x006506f4
int g_world_draw_head_index;
// GLOBAL: SHANDALAR 0x006506f8
int g_world_camera_viewport_left;
// GLOBAL: SHANDALAR 0x006506fc
int g_world_camera_anchor_world_x;
// GLOBAL: SHANDALAR 0x00650700
int g_world_draw_depth_sort_keys[0x200];
// GLOBAL: SHANDALAR 0x00650f00
int g_world_camera_scroll_limit_left;
// GLOBAL: SHANDALAR 0x00650f08
int g_world_camera_viewport_width_pixels;
// GLOBAL: SHANDALAR 0x00650f0c
int g_world_camera_minimap_panel_x;
// GLOBAL: SHANDALAR 0x00650f10
int g_world_camera_origin_screen_x;
// GLOBAL: SHANDALAR 0x00650f14
int g_world_player_tile_x;
// GLOBAL: SHANDALAR 0x00650f18
int g_world_player_tile_y;
// GLOBAL: SHANDALAR 0x00650f24
int g_world_camera_viewport_height_scaled;
// GLOBAL: SHANDALAR 0x00650f2c
int g_world_camera_scroll_limit_bottom;
// GLOBAL: SHANDALAR 0x00650f30
int g_world_camera_viewport_height_pixels;
// GLOBAL: SHANDALAR 0x00650f34
int g_world_camera_minimap_panel_y;
// GLOBAL: SHANDALAR 0x00650f38
int g_world_camera_scroll_limit_top;
// GLOBAL: SHANDALAR 0x00650f40
EncodedImage *g_world_draw_sprites[0x200];
// GLOBAL: SHANDALAR 0x00651740
int g_world_draw_screen_x[0x200];
// GLOBAL: SHANDALAR 0x00651f40
int g_world_draw_screen_y[0x200];
// GLOBAL: SHANDALAR 0x006527a8
int g_world_camera_viewport_width_scaled;
// GLOBAL: SHANDALAR 0x006527ac
int g_world_camera_anchor_world_y;
// GLOBAL: SHANDALAR 0x006527b4
int g_world_camera_origin_screen_y;
// GLOBAL: SHANDALAR 0x006527b8
int g_world_camera_scroll_limit_right;
// GLOBAL: SHANDALAR 0x006527c0
int g_world_draw_next_index[0x200];
// GLOBAL: SHANDALAR 0x0073ea98
int g_world_camera_last_anchor_x;
// GLOBAL: SHANDALAR 0x0073ea9c
int g_world_camera_last_anchor_y;
// GLOBAL: SHANDALAR 0x0073eaf0
int g_world_tile_screen_x_cache[0x1000];
// GLOBAL: SHANDALAR 0x00742af0
int g_world_tile_screen_y_cache[0x1000];
// GLOBAL: SHANDALAR 0x00746af0
int g_world_scroll_x;
// GLOBAL: SHANDALAR 0x00746af4
int g_world_scroll_y;
// GLOBAL: SHANDALAR 0x00746af8
int g_world_player_world_x_cached;
// GLOBAL: SHANDALAR 0x00746afc
int g_world_player_world_y_cached;

typedef union
{
  struct
  {
    EncodedImage *normal[12];
    EncodedImage *highlight[12];
    EncodedImage *pressed[12];
    EncodedImage *icon[12];
  } named;
  EncodedImage *by_group[4][12];
} WorldMagicChoiceButtonSpriteBank;

extern int global_screen_width;
extern int global_screen_height;
extern FacemakerWindowBounds *PTR_DAT_005832b4;
extern FacemakerWindowBounds *PTR_DAT_005832dc;
extern FacemakerWindowBounds *PTR_DAT_0058332c;
extern FacemakerWindowBounds *PTR_DAT_00583304;
extern int DAT_00590768;
extern int DAT_0073ea70[8];
extern int DAT_0073e9dc;
extern int DAT_0073eaa0;
extern int g_monster_timer;
extern int g_skip_world_sfx_preload;
extern char g_ui_message_buffer[0x1000];
extern int g_ego_sprite_width;
extern int g_ego_sprite_draw_height;
extern int g_sego_sprite_width;
extern int g_sego_sprite_draw_height;
extern int g_neighbor_dx[9];
extern int g_neighbor_dy[9];
extern EncodedImage *g_location07_sprite_entries[0xc];
extern EncodedImage *g_road_sprite_entries[0xc];
extern EncodedImage *g_land_sprite_entries[0x37];
extern EncodedImage *g_sland_sprite_entries[0x37];
extern EncodedImage *g_land_tile_sprite_entries[0x10];
extern EncodedImage *g_cstline1_sprite_entries[0x54];
extern EncodedImage *g_ttsprite_aux_sprite_entries[0x18];
extern EncodedImage *g_questnew_sprite_entries[4];
extern EncodedImage *g_clocknew_sprite_entries[9];
extern EncodedImage *g_sunmoon_sprite_entries[0x14];
extern EncodedImage *g_daysnew_sprite_entries[0xc];
extern EncodedImage *g_tips_frame_sprite;
extern EncodedImage *g_tips_icon_sprite;
extern EncodedImage *g_icons_sprite_entries[0x20];
extern DIBSurface *g_graphics_pages[10];
extern char g_opening_menu_sprite_work_buffer[0x1680];
extern EncodedImage *g_world_magic_avatar_sprites[5];
extern EncodedImage *g_worlds_extra_sprite_entries[4];
extern AdvMenuRect g_world_magic_icon_rects[0xc];
extern WorldMagicChoiceButtonSpriteBank g_world_magic_choice_button_sprite_bank;
extern int g_amulet_inventory[5];
extern int g_next_duel_life_delta;
extern int g_next_duel_card_id;
extern int Gold;
extern int g_food;
extern int DAT_0078990c[10];
extern int DAT_00789938;
extern int DAT_0078df68;

void FUN_0046ed03(void);
void FUN_0046ed33(void);
int ScaleUiCoordinate(int value);
int *PushGraphicsClipRect(int *saved_clip_rect, FacemakerWindowBounds *page, int x, int y, int width, int height);
void BlitGraphicsRect(FacemakerWindowBounds *dst, unsigned int dst_x, int dst_y, unsigned int width, DWORD height,
                      FacemakerWindowBounds *src, int src_x, int src_y);
void DrawEncodedImageResampled(FacemakerWindowBounds *dst, int x, int y, int width, int height, EncodedImage *encoded_image);
void DrawEncodedImageUnscaled(FacemakerWindowBounds *dst, int x, int y, EncodedImage *encoded_image);
void DrawEncodedImageUnscaledClipped(FacemakerWindowBounds *dst, int x, int y, EncodedImage *encoded_image);
void DrawEncodedImageUiScaled(FacemakerWindowBounds *dst, int x_320, int y_200, EncodedImage *sprite, int width_320, int height_200);
void DrawTextAt(FacemakerWindowBounds *window, int color, int x, int y, char *text);
void FUN_0057b560(FacemakerWindowBounds *window, int color_index, int x, int y, char *format, ...);
void FUN_004ce992(int param_1);
int FUN_004ecf30(int x, int y);
int FUN_004bb458(int param_1);
unsigned int FUN_0043146b(int x, int y);
void FUN_00431526(unsigned int mask, int x, int y);
void MarkPathConnection(int x, int y, int direction_index);
unsigned int FUN_005611c8(unsigned int tile_mask);
unsigned int FUN_005795f0(FacemakerWindowBounds *window, int x, int y);
void FUN_0054a89f(FacemakerWindowBounds *window, int draw_x, int draw_y, int depth_y, EncodedImage *sprite);
void FUN_0054d619(unsigned int world_x, unsigned int world_y, int tile_x_start, int tile_x_end, int tile_y_start, int tile_y_end,
                  int edge_mode, int draw_mode);
void FUN_0054ab73(int screen_x, int screen_y, int *out_world_x, int *out_world_y);
int FUN_004318c7(int x, int y);
int FUN_00508b89(int world_x, int world_y);
int FUN_004bb040(int world_x, int world_y);
void FUN_0054aae9(int world_x, int world_y, int *out_screen_x, int *out_screen_y);
void RefreshAdventureInterfaceLayout(void);

// FUNCTION: SHANDALAR 0x005501dc
int FUN_005501dc(int value)
{
  return (value * global_screen_width) / 0x140;
}

// FUNCTION: SHANDALAR 0x00561441
char *FUN_00561441(int creature_type)
{
  return gs_creature_names_00591a08[creature_type].name;
}

// FUNCTION: SHANDALAR 0x0057d330
unsigned int FUN_0057d330(HDC dst_hdc, int dst_x, int dst_y, int width, int height, int block_w, int block_h,
                          HDC src_hdc, int src_x, int src_y)
{
  int tiles_x;
  int tiles_y;
  unsigned int bit;
  int bit_count;
  int tile_x;
  int tile_y;
  COLORREF pixel;
  int draw_x;
  int draw_y;
  int copy_w;
  int copy_h;
  unsigned int tile_count;
  unsigned int lfsr_mask;
  unsigned int lfsr;
  unsigned int lfsr_limit;
  unsigned int result;

  tiles_x = width / block_w + (width % block_w != 0);
  tiles_y = height / block_h + (height % block_h != 0);
  bit_count = 0;
  lfsr_limit = 0;
  tile_count = (unsigned int)((tiles_y + 1) * (tiles_x + 1));
  bit = 0x40000000;
  do
  {
    if ((tile_count & bit) != 0)
    {
      if (lfsr_limit == 0)
      {
        lfsr_limit = bit;
      }
      bit_count = bit_count + 1;
    }
    bit = (unsigned int)((int)bit >> 1);
  } while (bit != 0);
  if (bit_count != 1)
  {
    lfsr_limit = lfsr_limit * 2;
  }

  lfsr_mask = 0;
  bit = 1;
  while (bit < lfsr_limit)
  {
    lfsr_mask = lfsr_mask | bit;
    bit = bit * 2;
  }

  lfsr = (unsigned int)rand() % lfsr_limit;
  result = (unsigned int)rand() / lfsr_limit;
  while (tile_count != 0)
  {
    lfsr = (lfsr * 0x21 + 1) & lfsr_mask;
    result = lfsr;
    if ((int)lfsr <= tiles_y * tiles_x)
    {
      tile_y = (int)lfsr / tiles_x;
      tile_x = (int)lfsr % tiles_x;
      draw_x = dst_x + tile_x * block_w;
      draw_y = dst_y + tile_y * block_h;

      copy_w = block_w;
      if (dst_x + width <= draw_x + block_w)
      {
        copy_w = dst_x + width - draw_x;
      }

      copy_h = block_h;
      if (dst_y + height <= draw_y + block_h)
      {
        copy_h = dst_y + height - draw_y;
      }

      if ((block_w == 1) && (block_h == 1))
      {
        pixel = GetPixel(src_hdc, tile_x, tile_y);
        result = SetPixelV(dst_hdc, tile_x, tile_y, pixel);
      }
      else
      {
        result = BitBlt(dst_hdc, draw_x, draw_y, copy_w, copy_h, src_hdc,
                        src_x + tile_x * block_w, src_y + tile_y * block_h, SRCCOPY);
      }
      tile_count = tile_count - 1;
    }
  }

  return result;
}

// FUNCTION: SHANDALAR 0x0054cee2
void FUN_0054cee2(unsigned int world_x, unsigned int world_y)
{
  struct
  {
    int saved_page_number;
    int snapped_world_x;
    int snapped_world_y;
    int snapped_screen_x;
    int snapped_screen_y;
    int world_screen_x;
    int world_screen_y;
    int origin_screen_x;
    int origin_screen_y;
    int scroll_delta_x;
    int scroll_delta_y;
    int tile_x;
    int tile_y;
    int clip_dst_x;
    int clip_src_x;
    int clip_width;
    int clip_mode_x;
    int clip_dst_y;
    int clip_src_y;
    int clip_height;
    int clip_mode_y;
    int redraw_x_start;
    int redraw_x_end;
    int redraw_y_start;
    int redraw_y_end;
    int need_left_edge;
    int need_right_edge;
    int need_top_edge;
    int need_bottom_edge;
  } s;

  s.saved_page_number = PTR_DAT_005832b4->page_number;
  PTR_DAT_005832b4->page_number = 2;

  g_world_player_world_x_cached = world_x;
  g_world_player_world_y_cached = world_y;

  s.snapped_world_x = (int)world_x - ((int)world_x & 0x1f);
  s.snapped_world_y = (int)world_y - ((int)world_y & 0x1f);
  FUN_0054aae9(s.snapped_world_x, s.snapped_world_y, &s.snapped_screen_x, &s.snapped_screen_y);
  FUN_0054aae9((int)world_x, (int)world_y, &s.world_screen_x, &s.world_screen_y);
  FUN_0054aae9(0, 0, &s.origin_screen_x, &s.origin_screen_y);

  if ((g_world_camera_initialized != 0) && (DAT_0073eaa0 != 0))
  {
    s.need_left_edge = 0;
    s.need_right_edge = 0;
    s.need_top_edge = 0;
    s.need_bottom_edge = 0;

    s.tile_x = (int)((world_x + (((int)world_x >> 0x1f) & 0x1fU)) >> 5);
    s.tile_y = (int)((world_y + (((int)world_y >> 0x1f) & 0x1fU)) >> 5);
    g_world_scroll_x = g_world_tile_screen_x_cache[s.tile_x + s.tile_y * 0x100] - s.snapped_screen_x;
    s.scroll_delta_y = g_world_tile_screen_y_cache[s.tile_x + s.tile_y * 0x100] - s.snapped_screen_y;

    if ((g_world_scroll_x != 0) || (s.scroll_delta_y != 0))
    {
      if ((g_world_scroll_x < 0) && (g_world_scroll_x <= g_world_camera_scroll_limit_left))
      {
        s.need_left_edge = 1;
      }
      if ((0 < g_world_scroll_x) && (g_world_camera_scroll_limit_right <= g_world_scroll_x))
      {
        s.need_right_edge = 1;
      }
      if ((0 < s.scroll_delta_y) && (g_world_camera_scroll_limit_bottom <= s.scroll_delta_y))
      {
        s.need_bottom_edge = 1;
      }
      if ((s.scroll_delta_y < 0) && (s.scroll_delta_y <= g_world_camera_scroll_limit_top))
      {
        s.need_top_edge = 1;
      }
    }

    g_world_scroll_y = s.scroll_delta_y;

    if ((s.need_left_edge | s.need_right_edge | s.need_top_edge | s.need_bottom_edge) == 0)
    {
      BlitGraphicsRect(PTR_DAT_005832b4, g_world_camera_viewport_left + g_world_scroll_x, g_world_camera_viewport_top + s.scroll_delta_y,
                       g_world_camera_viewport_width_scaled, g_world_camera_viewport_height_scaled,
                       PTR_DAT_005832dc, 0x40, g_world_camera_minimap_panel_y * 2 + g_world_ui_top_offset + 0x10);

      PTR_DAT_005832b4->page_number = s.saved_page_number;
      FUN_0054d619(g_world_camera_anchor_world_x, g_world_camera_anchor_world_y, -5, 4, -7, 9, 0, 1);
      g_world_player_world_x_cached = world_x;
      g_world_player_world_y_cached = world_y;
      return;
    }

    FUN_0054d619(world_x, world_y, -5, 4, -7, 9, 0, 1);

    if (g_world_scroll_x < 1)
    {
      if (g_world_scroll_x < 0)
      {
        s.clip_dst_x = 0;
        s.clip_src_x = -g_world_scroll_x;
        s.clip_width = PTR_DAT_005832b4->max_x + g_world_scroll_x;
        s.clip_mode_x = 1;
      }
      else
      {
        s.clip_dst_x = 0;
        s.clip_src_x = 0;
        s.clip_width = PTR_DAT_005832b4->max_x;
        s.clip_mode_x = -1;
      }
    }
    else
    {
      s.clip_dst_x = g_world_scroll_x;
      s.clip_src_x = 0;
      s.clip_width = PTR_DAT_005832b4->max_x - g_world_scroll_x;
      s.clip_mode_x = 2;
    }

    if (s.scroll_delta_y < 1)
    {
      if (s.scroll_delta_y < 0)
      {
        s.clip_dst_y = 0x80;
        s.clip_src_y = 0x80 - s.scroll_delta_y;
        s.clip_height = (PTR_DAT_005832b4->max_y + s.scroll_delta_y) - 0x80;
        s.clip_mode_y = 1;
      }
      else
      {
        s.clip_dst_y = 0x80;
        s.clip_src_y = 0x80;
        s.clip_height = PTR_DAT_005832b4->max_y - 0x80;
        s.clip_mode_y = -1;
      }
    }
    else
    {
      s.clip_dst_y = s.scroll_delta_y + 0x80;
      s.clip_src_y = 0x80;
      s.clip_height = (PTR_DAT_005832b4->max_y - s.scroll_delta_y) - 0x80;
      s.clip_mode_y = 2;
    }

    BlitGraphicsRect(PTR_DAT_005832b4, s.clip_dst_x, s.clip_dst_y, s.clip_width, (DWORD)s.clip_height,
                     PTR_DAT_005832b4, s.clip_src_x, s.clip_src_y);

    if (s.clip_mode_x == 1)
    {
      FUN_0054d619(world_x, world_y, -3, -1, -7, 7, 1, 0);
      if (s.clip_mode_y == 1)
      {
        FUN_0054d619(world_x, world_y, -1, 4, -7, -3, 1, 0);
      }
      else if (s.clip_mode_y == 2)
      {
        FUN_0054d619(world_x, world_y, -1, 4, 3, 7, 1, 0);
      }
    }
    else if (s.clip_mode_x == 2)
    {
      FUN_0054d619(world_x, world_y, 2, 5, -7, 7, 1, 0);
      if (s.clip_mode_y == 1)
      {
        FUN_0054d619(world_x, world_y, -3, 2, -7, -3, 1, 0);
      }
      else if (s.clip_mode_y == 2)
      {
        FUN_0054d619(world_x, world_y, -3, 2, 3, 7, 1, 0);
      }
    }
    else if (s.clip_mode_y == 1)
    {
      FUN_0054d619(world_x, world_y, -3, 5, -7, -3, 1, 0);
    }
    else if (s.clip_mode_y == 2)
    {
      FUN_0054d619(world_x, world_y, -3, 5, 3, 7, 1, 0);
    }

    FUN_0054d619(world_x, world_y, -4, 6, -9, 0xb, 2, 0);

    g_world_camera_origin_screen_x = s.origin_screen_x;
    g_world_camera_origin_screen_y = s.origin_screen_y;
    g_world_camera_anchor_world_x = (int)world_x;
    g_world_camera_anchor_world_y = (int)world_y;
    g_world_scroll_x = 0;
    g_world_scroll_y = 0;
    g_world_camera_last_anchor_x = (int)world_x;
    g_world_camera_last_anchor_y = (int)world_y;
  }
  else
  {
    s.redraw_y_start = -9;
    s.redraw_x_end = 9;
    s.redraw_x_start = -4;
    s.redraw_y_end = 6;

    g_world_camera_origin_screen_x = s.origin_screen_x;
    g_world_camera_origin_screen_y = s.origin_screen_y;
    g_world_camera_anchor_world_x = (int)world_x;
    g_world_camera_anchor_world_y = (int)world_y;
    g_world_scroll_x = 0;
    g_world_scroll_y = 0;
    g_world_camera_last_anchor_x = (int)world_x;
    g_world_camera_last_anchor_y = (int)world_y;
    g_world_camera_viewport_left = 0x40;
    g_world_camera_viewport_top = g_world_camera_minimap_panel_y * 2 + g_world_ui_top_offset + 0x10;
    g_world_camera_viewport_width_scaled = FUN_005501dc(0x100);
    g_world_camera_viewport_height_scaled = FUN_005501dc(0x8c);
    g_world_camera_scroll_limit_left = -0x40;
    g_world_camera_scroll_limit_right = 0x40;
    g_world_camera_scroll_limit_top = 0x80 - g_world_camera_viewport_top;
    g_world_camera_scroll_limit_bottom = PTR_DAT_005832b4->max_y - (g_world_camera_viewport_top + g_world_camera_viewport_height_scaled);
    g_world_camera_initialized = 1;
    DAT_0073eaa0 = 1;

    FUN_0054d619(world_x, world_y, s.redraw_x_start, s.redraw_y_end, s.redraw_y_start, s.redraw_x_end, 1, 0);
    FUN_0054d619(world_x, world_y, s.redraw_x_start - 2, s.redraw_y_end, s.redraw_y_start, s.redraw_x_end + 2, 2, 1);
  }

  g_world_player_world_x_cached = world_x;
  g_world_player_world_y_cached = world_y;
  BlitGraphicsRect(PTR_DAT_005832b4, g_world_camera_viewport_left, g_world_camera_viewport_top,
                   g_world_camera_viewport_width_scaled, g_world_camera_viewport_height_scaled,
                   PTR_DAT_005832dc, 0x40, g_world_camera_minimap_panel_y * 2 + g_world_ui_top_offset + 0x10);
  PTR_DAT_005832b4->page_number = s.saved_page_number;
}

// FUNCTION: SHANDALAR 0x0054d619
void FUN_0054d619(unsigned int world_x, unsigned int world_y, int tile_x_start, int tile_x_end, int tile_y_start, int tile_y_end,
                  int edge_mode, int draw_mode)
{
  struct
  {
    int saved_page_number;
    int snapped_world_x;
    int snapped_world_y;
    int snapped_screen_x;
    int snapped_screen_y;
    int tile_x;
    int tile_y;
    int tile_screen_x;
    int tile_screen_y;
    int world_coord_x;
    int world_coord_y;
    int world_tile_x;
    int world_tile_y;
    unsigned int tile_mask;
    int tile_type;
    unsigned int tile_class;
    int i;
    int coastline_bits;
    int coast_bits_expanded;
    unsigned int road_bits;
    unsigned int road_mask;
    unsigned int neighbor_tile_type;
    unsigned int neighbor_tile_class;
    int terrain_row;
    int terrain_column;
    int terrain_pattern;
    int terrain_offset_x;
    int terrain_offset_y;
    int terrain_height;
    int second_terrain_row;
    int second_terrain_index;
  } s;

  s.saved_page_number = PTR_DAT_005832b4->page_number;
  PTR_DAT_005832b4->page_number = 2;

  g_world_player_world_x_cached = world_x;
  g_world_player_world_y_cached = world_y;
  s.snapped_world_x = (int)world_x - ((int)world_x & 0x1f);
  s.snapped_world_y = (int)world_y - ((int)world_y & 0x1f);
  FUN_0054aae9(s.snapped_world_x, s.snapped_world_y, &s.snapped_screen_x, &s.snapped_screen_y);
  for (s.tile_y = tile_y_start; s.tile_y < tile_y_end; s.tile_y++)
  {
    for (s.tile_x = tile_x_start; s.tile_x < tile_x_end; s.tile_x++)
    {
      s.tile_screen_x = s.tile_x * g_world_camera_minimap_panel_x + s.snapped_screen_x;
      s.tile_screen_y = s.tile_y * g_world_camera_minimap_panel_y + s.snapped_screen_y;
      if ((s.tile_y & 1) != 0)
      {
        s.tile_screen_x += g_world_camera_minimap_panel_x / 2;
      }

      FUN_0054ab73(s.tile_screen_x + 0x10, s.tile_screen_y, &s.world_coord_x, &s.world_coord_y);
      s.world_tile_x = (s.world_coord_x + ((s.world_coord_x >> 0x1f) & 0x1fU)) >> 5;
      s.world_tile_y = (s.world_coord_y + ((s.world_coord_y >> 0x1f) & 0x1fU)) >> 5;

      if ((abs(s.tile_y) < 8) && (abs(s.tile_x) < 4))
      {
        FUN_00431526(0x80, s.world_tile_x, s.world_tile_y);
      }

      MarkPathConnection(s.world_tile_x, s.world_tile_y, 0);
      s.tile_mask = FUN_0043146b(s.world_tile_x, s.world_tile_y);
      s.tile_type = s.tile_mask & 0xf;

      if ((draw_mode != 0) &&
          (s.world_tile_x >= 0) && (s.world_tile_x < 0x40) &&
          (s.world_tile_y >= 0) && (s.world_tile_y < 0x40))
      {
        g_world_tile_screen_x_cache[s.world_tile_x + s.world_tile_y * 0x100] = s.tile_screen_x;
        g_world_tile_screen_y_cache[s.world_tile_x + s.world_tile_y * 0x100] = s.tile_screen_y;
      }

      s.tile_screen_y -= g_world_camera_minimap_panel_y;
      s.tile_class = FUN_005611c8((unsigned int)s.tile_type);

      if ((edge_mode != 2) && ((draw_mode == 0) || (edge_mode != 0)))
      {
        if (((s.tile_type == 0) || (s.tile_type == 8)) && (edge_mode != 0))
        {
          DrawEncodedImageUnscaledClipped(PTR_DAT_005832b4, s.tile_screen_x, s.tile_screen_y, g_land_tile_sprite_entries[1]);
        }
        else if (edge_mode != 0)
        {
          if (s.tile_type == 1)
          {
            DrawEncodedImageUnscaledClipped(PTR_DAT_005832b4, s.tile_screen_x, s.tile_screen_y, g_land_tile_sprite_entries[2]);
          }
          else
          {
            if (s.tile_class == 2)
            {
              DrawEncodedImageUnscaledClipped(PTR_DAT_005832b4, s.tile_screen_x, s.tile_screen_y, g_land_tile_sprite_entries[4]);
            }
            else
            {
              DrawEncodedImageUnscaledClipped(PTR_DAT_005832b4, s.tile_screen_x, s.tile_screen_y, g_land_tile_sprite_entries[0]);
            }
          }

          s.coastline_bits = 0;
          s.road_mask = 0;
          for (s.i = 1; s.i < 9; s.i++)
          {
            s.coastline_bits >>= 1;
            s.road_mask >>= 1;
            s.neighbor_tile_type = FUN_0043146b(g_neighbor_dx[s.i] + s.world_tile_x, g_neighbor_dy[s.i] + s.world_tile_y);
            s.neighbor_tile_class = FUN_005611c8(s.neighbor_tile_type);

            if ((s.neighbor_tile_type == 0) || (s.neighbor_tile_type == 8) ||
                ((s.neighbor_tile_type != (unsigned int)s.tile_type) && (((s.tile_class | s.neighbor_tile_class) & 4) != 0)))
            {
              s.coastline_bits |= 0x80;
            }

            if ((s.tile_class != 2) && (s.neighbor_tile_class == 2))
            {
              s.road_mask |= 0x80;
            }
          }

          s.road_mask |= s.road_mask << 8;
          if (s.road_mask != 0)
          {
            for (s.i = 0; s.i < 4; s.i++)
            {
              switch (s.i)
              {
              case 0:
                s.road_bits = s.road_mask & 7;
                break;
              case 1:
                s.road_bits = (s.road_mask >> 4) & 7;
                break;
              case 2:
                s.road_bits = (s.road_mask >> 6) & 7;
                break;
              default:
                s.road_bits = (s.road_mask >> 2) & 7;
                break;
              }

              if (s.road_bits != 0)
              {
                if (s.i < 2)
                {
                  DrawEncodedImageUnscaledClipped(PTR_DAT_005832b4,
                                                  s.tile_screen_x + g_world_camera_minimap_panel_y,
                                                  (s.i & 1) * g_world_camera_minimap_panel_y + s.tile_screen_y,
                                                  g_cstline1_sprite_entries[(s.road_bits - 1) + (s.i + 4) * 7]);
                }
                else
                {
                  DrawEncodedImageUnscaledClipped(PTR_DAT_005832b4,
                                                  (s.i & 1) * g_world_camera_minimap_panel_y * 2 + s.tile_screen_x,
                                                  s.tile_screen_y + g_world_camera_minimap_panel_y / 2,
                                                  g_cstline1_sprite_entries[(s.road_bits - 1) + (s.i + 4) * 7]);
                }
              }
            }
          }

          s.coast_bits_expanded = s.coastline_bits | (s.coastline_bits << 8);
          if (s.coast_bits_expanded != 0)
          {
            for (s.i = 0; s.i < 4; s.i++)
            {
              switch (s.i)
              {
              case 0:
                s.road_bits = s.coast_bits_expanded & 7;
                break;
              case 1:
                s.road_bits = (s.coast_bits_expanded >> 4) & 7;
                break;
              case 2:
                s.road_bits = (s.coast_bits_expanded >> 6) & 7;
                break;
              default:
                s.road_bits = (s.coast_bits_expanded >> 2) & 7;
                break;
              }

              if (s.road_bits != 0)
              {
                if (s.i < 2)
                {
                  DrawEncodedImageUnscaledClipped(PTR_DAT_005832b4,
                                                  s.tile_screen_x + g_world_camera_minimap_panel_y,
                                                  (s.i & 1) * g_world_camera_minimap_panel_y + s.tile_screen_y,
                                                  g_cstline1_sprite_entries[s.i * 7 + (s.road_bits - 1)]);
                }
                else
                {
                  DrawEncodedImageUnscaledClipped(PTR_DAT_005832b4,
                                                  (s.i & 1) * g_world_camera_minimap_panel_y * 2 + s.tile_screen_x,
                                                  s.tile_screen_y + g_world_camera_minimap_panel_y / 2,
                                                  g_cstline1_sprite_entries[s.i * 7 + (s.road_bits - 1)]);
                }
              }
            }
          }
        }

        if (FUN_004318c7(s.world_tile_x, s.world_tile_y) != 0)
        {
          s.road_mask = FUN_005795f0(PTR_DAT_00583304, s.world_tile_x, s.world_tile_y + 0x40);
        }
        else
        {
          s.road_mask = 0;
        }

        if ((s.road_mask != 0) && (edge_mode != 0))
        {
          for (s.i = 0; s.i < 8; s.i++)
          {
            if ((s.road_mask & (1 << ((unsigned char)s.i & 0x1f))) != 0)
            {
              DrawEncodedImageUnscaledClipped(PTR_DAT_005832b4, s.tile_screen_x, s.tile_screen_y, g_road_sprite_entries[(s.i - 2 & 7) + 1]);
            }
          }
        }
      }

      if (edge_mode != 1)
      {
        s.tile_screen_y += g_world_camera_minimap_panel_y;
        if ((s.tile_mask & 0x10) == 0)
        {
          switch (s.tile_type)
          {
          case 1:
            s.terrain_column = 1;
            break;
          case 2:
            s.terrain_column = 2;
            break;
          case 3:
            s.terrain_column = 0;
            break;
          case 5:
            s.terrain_column = 3;
            break;
          case 6:
            s.terrain_column = 4;
            break;
          case 8:
            s.terrain_column = 5;
            break;
          case 10:
            s.terrain_column = 7;
            break;
          case 0xd:
            s.terrain_column = 6;
            break;
          case 0xf:
            s.terrain_column = 8;
            break;
          default:
            s.terrain_column = -1;
            break;
          }

          if (s.terrain_column >= 0)
          {
            s.terrain_pattern = abs(s.world_tile_y) * 3 + abs(s.world_tile_x) * 7;
            s.terrain_offset_x = g_world_camera_minimap_panel_x / ((s.terrain_pattern % 5) * 2 + 4);
            s.terrain_offset_y = ScaleUiCoordinate((abs(s.terrain_pattern) % 4) * 2 + 5);
            if (s.tile_type == 1)
            {
              s.terrain_offset_x = 0;
              s.terrain_offset_y = 0;
            }

            s.terrain_row = s.terrain_pattern % 0xb;
            if (s.terrain_column > 4)
            {
              s.terrain_column %= 5;
              s.terrain_row += 0xb;
            }
            s.second_terrain_index = s.terrain_row * 5 + s.terrain_column;
            s.terrain_height = (int)*(short *)((int)g_land_sprite_entries[s.second_terrain_index] + 10);
            if (s.terrain_height == -1)
            {
              s.terrain_height = 0;
            }

            if (draw_mode != 0)
            {
              FUN_0054a89f(PTR_DAT_005832b4,
                           s.tile_screen_x + s.terrain_offset_x,
                           (s.tile_screen_y - s.terrain_height) - s.terrain_offset_y,
                           s.tile_screen_y - s.terrain_offset_y,
                           g_land_sprite_entries[s.second_terrain_index]);
            }
            if (edge_mode != 0)
            {
              DrawEncodedImageUnscaledClipped(PTR_DAT_005832b4,
                                              s.tile_screen_x + s.terrain_offset_x,
                                              (s.tile_screen_y - s.terrain_height) - s.terrain_offset_y,
                                              g_sland_sprite_entries[s.second_terrain_index]);
            }

            s.second_terrain_row = (((s.terrain_row < 0xb) - 1) & 0xb) + (s.terrain_pattern + s.world_tile_x) % 0xb;
            s.second_terrain_index = s.second_terrain_row * 5 + s.terrain_column;
            s.terrain_height = (int)*(short *)((int)g_land_sprite_entries[s.second_terrain_index] + 10);
            if (s.terrain_height == -1)
            {
              s.terrain_height = 0;
            }

            if (draw_mode != 0)
            {
              FUN_0054a89f(PTR_DAT_005832b4,
                           s.tile_screen_x - s.terrain_offset_x,
                           (s.tile_screen_y - s.terrain_height) + s.terrain_offset_y,
                           s.tile_screen_y + s.terrain_offset_y,
                           g_land_sprite_entries[s.second_terrain_index]);
            }
            if (edge_mode != 0)
            {
              DrawEncodedImageUnscaledClipped(PTR_DAT_005832b4,
                                              s.tile_screen_x - s.terrain_offset_x,
                                              (s.tile_screen_y - s.terrain_height) + s.terrain_offset_y,
                                              g_sland_sprite_entries[s.second_terrain_index]);
            }
          }
        }
      }
    }
  }

  FUN_0054aae9(g_world_player_world_x_cached, g_world_player_world_y_cached, &s.tile_screen_x, &s.tile_screen_y);

  PTR_DAT_005832b4->page_number = s.saved_page_number;
}

// FUNCTION: SHANDALAR 0x0054baaa
int FUN_0054baaa(int param_1)
{
  struct
  {
    int saved_page_number;
    int questnew_sprite_entry;
    int clocknew_sprite_entry;
    int quest_deadline_delta;
    int scaled_strip_x;
    int scaled_strip_y;
    int scaled_strip_height;
    int avatar_draw_y;
  } s;

  s.saved_page_number = PTR_DAT_005832dc->page_number;
  PTR_DAT_005832dc->page_number = 1;
  s.quest_deadline_delta = g_current_quest_deadline - g_quest_restock_timer;

  if ((param_1 == 0) ||
      ((g_current_quest_destination == -1) && (g_lair_or_monster_slots[7].entry_type == -1) &&
       (g_next_duel_life_delta == 0) && (g_next_duel_card_id == -1)))
  {
    if ((param_1 != 0) && (g_questnew_sprite_entries[0] != (EncodedImage *)0))
    {
      DrawEncodedImageUiScaled(PTR_DAT_005832b4, 0x181, 0x135, g_questnew_sprite_entries[0], 0xed, 0x74);
    }

    PTR_DAT_005832dc->page_number = s.saved_page_number;
    return 0;
  }

  s.questnew_sprite_entry = (int)g_questnew_sprite_entries[1];
  s.clocknew_sprite_entry = (int)g_clocknew_sprite_entries[0];
  s.scaled_strip_height = ScaleUiCoordinate(*(short *)(s.questnew_sprite_entry + 6)) - ScaleUiCoordinate(0x13);

  if (g_skip_world_sfx_preload == 0)
  {
    s.scaled_strip_x = ScaleUiCoordinate(0x17c);
    s.scaled_strip_y = ScaleUiCoordinate(0x148) - ScaleUiCoordinate(0x135);
    BlitGraphicsRect(PTR_DAT_0058332c, (unsigned int)s.scaled_strip_x, 0,
                     (unsigned int)(ScaleUiCoordinate(0x280) - s.scaled_strip_x),
                     (DWORD)PTR_DAT_0058332c->max_y,
                     PTR_DAT_005832dc, s.scaled_strip_x, s.scaled_strip_y);

    DrawEncodedImageResampled(PTR_DAT_005832dc,
                              ScaleUiCoordinate(0x181),
                              0,
                              ScaleUiCoordinate(*(short *)(s.questnew_sprite_entry + 4)),
                              ScaleUiCoordinate(*(short *)(s.questnew_sprite_entry + 6)),
                              g_questnew_sprite_entries[1]);

    if (g_current_quest_destination != -1)
    {
      DrawEncodedImageResampled(PTR_DAT_005832dc,
                                ScaleUiCoordinate(0x23a),
                                ScaleUiCoordinate(0x15),
                                ScaleUiCoordinate(*(short *)(s.clocknew_sprite_entry + 4)),
                                ScaleUiCoordinate(*(short *)(s.clocknew_sprite_entry + 6)),
                                g_clocknew_sprite_entries[g_monster_timer & 7]);

      DrawEncodedImageResampled(PTR_DAT_005832dc,
                                ScaleUiCoordinate(0x23a),
                                ScaleUiCoordinate(0x15),
                                ScaleUiCoordinate(*(short *)(s.clocknew_sprite_entry + 4)),
                                ScaleUiCoordinate(*(short *)(s.clocknew_sprite_entry + 6)),
                                g_sunmoon_sprite_entries[s.quest_deadline_delta % 0xe]);

      DrawEncodedImageResampled(PTR_DAT_005832dc,
                                ScaleUiCoordinate(0x23a),
                                ScaleUiCoordinate(0x15),
                                ScaleUiCoordinate(*(short *)(s.clocknew_sprite_entry + 4)),
                                ScaleUiCoordinate(*(short *)(s.clocknew_sprite_entry + 6)),
                                g_daysnew_sprite_entries[((s.quest_deadline_delta + ((s.quest_deadline_delta >> 0x1f) & 0xfU)) >> 4) + 1]);
    }
  }

  PTR_DAT_005832dc->unk_1c = 4;
  if (g_world_magic_avatar_sprites[0] != (EncodedImage *)0)
  {
    s.avatar_draw_y = 0x5b - (int)g_world_magic_avatar_sprites[0]->height / 2;
    DrawEncodedImageUiScaled(PTR_DAT_005832dc,
                             0x193,
                             s.avatar_draw_y,
                             g_world_magic_avatar_sprites[0],
                             (int)g_world_magic_avatar_sprites[0]->width,
                             (int)g_world_magic_avatar_sprites[0]->height);
    FUN_0057b560(PTR_DAT_005832dc, 0xff, 0x1a9, s.avatar_draw_y + (int)g_world_magic_avatar_sprites[0]->height / 2, "%d", g_amulet_inventory[4]);
  }

  PTR_DAT_005832dc->unk_1c = 2;
  strcpy(g_ui_message_buffer, gs_queststatus_0077e0a0[0]);
  if (g_current_quest_destination == -1)
  {
    strcat(g_ui_message_buffer, gs_queststatus_0077e0a0[10]);
  }
  else
  {
    strcat(g_ui_message_buffer, gs_queststatus_0077e0a0[1]);
  }
  DrawTextAt(PTR_DAT_005832dc, 0xff, 0x200, 0x41, g_ui_message_buffer);

  s.scaled_strip_x = ScaleUiCoordinate(0x17c);
  BlitGraphicsRect(PTR_DAT_005832dc, (unsigned int)s.scaled_strip_x,
                   ScaleUiCoordinate(0x148) - ScaleUiCoordinate(0x135),
                   (unsigned int)(ScaleUiCoordinate(0x280) - s.scaled_strip_x),
                   (DWORD)s.scaled_strip_height,
                   PTR_DAT_005832b4, s.scaled_strip_x, ScaleUiCoordinate(0x148));

  if ((g_current_quest_destination != -1) && (g_current_quest_deadline <= g_quest_restock_timer))
  {
    g_current_quest_destination = -1;
    RefreshAdventureInterfaceLayout();
    FUN_0054baaa(1);
  }

  PTR_DAT_005832dc->page_number = s.saved_page_number;
  return 0;
}

// FUNCTION: SHANDALAR 0x0054b80d
int FUN_0054b80d(void)
{
  struct
  {
    int scaled_strip_height;             // ebp - 0x10
    EncodedImage *clocknew_sprite_entry; // ebp - 0xc
    EncodedImage *questnew_sprite_entry; // ebp - 0x8
    int day_sprite_index;                // ebp - 0x4
  } s;

  if (g_current_quest_destination != -1)
  {
    s.questnew_sprite_entry = &g_questnew_sprite_entries[1];
    s.clocknew_sprite_entry = &g_clocknew_sprite_entries[0];
    s.day_sprite_index = ((g_current_quest_deadline - g_quest_restock_timer) / 16) + 1;

    if (g_skip_world_sfx_preload == 0)
    {
      s.scaled_strip_height = ScaleUiCoordinate(0x23a);
      BlitGraphicsRect(PTR_DAT_0058332c, (unsigned int)s.scaled_strip_height, 0,
                       (unsigned int)(ScaleUiCoordinate(0x280) - s.scaled_strip_height),
                       (DWORD)PTR_DAT_0058332c->max_y,
                       PTR_DAT_005832dc, s.scaled_strip_height, ScaleUiCoordinate(0x148) - ScaleUiCoordinate(0x135));

      DrawEncodedImageResampled(PTR_DAT_005832dc,
                                ScaleUiCoordinate(0x181),
                                ScaleUiCoordinate(0),
                                ScaleUiCoordinate(s.questnew_sprite_entry->width),
                                ScaleUiCoordinate(s.questnew_sprite_entry->height),
                                g_questnew_sprite_entries[1]);

      DrawEncodedImageResampled(PTR_DAT_005832dc,
                                ScaleUiCoordinate(0x23a),
                                ScaleUiCoordinate(0x15),
                                ScaleUiCoordinate(s.clocknew_sprite_entry->width),
                                ScaleUiCoordinate(s.clocknew_sprite_entry->height),
                                g_clocknew_sprite_entries[g_monster_timer & 7]);

      DrawEncodedImageResampled(PTR_DAT_005832dc,
                                ScaleUiCoordinate(0x23a),
                                ScaleUiCoordinate(0x15),
                                ScaleUiCoordinate(s.clocknew_sprite_entry->width),
                                ScaleUiCoordinate(s.clocknew_sprite_entry->height),
                                g_sunmoon_sprite_entries[(g_current_quest_deadline - g_quest_restock_timer) % 0xe]);

      DrawEncodedImageResampled(PTR_DAT_005832dc,
                                ScaleUiCoordinate(0x23a),
                                ScaleUiCoordinate(0x15),
                                ScaleUiCoordinate(s.clocknew_sprite_entry->width),
                                ScaleUiCoordinate(s.clocknew_sprite_entry->height),
                                g_daysnew_sprite_entries[s.day_sprite_index]);

      BlitGraphicsRect(PTR_DAT_005832dc, (unsigned int)ScaleUiCoordinate(0x23a),
                       ScaleUiCoordinate(0x148) - ScaleUiCoordinate(0x135),
                       (unsigned int)ScaleUiCoordinate(s.clocknew_sprite_entry->width),
                       ScaleUiCoordinate(s.clocknew_sprite_entry->height),
                       PTR_DAT_005832b4, ScaleUiCoordinate(0x23a), ScaleUiCoordinate(0x148));
    }
  }
  return 0;
}

// FUNCTION: SHANDALAR 0x0055060c
void *FUN_0055060c(int param_1)
{
  struct
  {
    int scaled_panel_src_y;
    int scaled_panel_dst_x;
    int scaled_panel_height;
    int avatar_draw_y;
    int stats_color;
    int stat_unknown;
    unsigned int world_magic_slot_index;
  } s;

  PTR_DAT_005832b4->unk_1c = 4;
  PTR_DAT_005832dc->unk_1c = 4;

  s.scaled_panel_src_y = ScaleUiCoordinate(0x15b);
  s.scaled_panel_height = s.scaled_panel_src_y - ScaleUiCoordinate(0x148);
  s.scaled_panel_dst_x = ScaleUiCoordinate(0x58);
  BlitGraphicsRect(PTR_DAT_0058332c, (unsigned int)s.scaled_panel_dst_x, s.scaled_panel_height,
                   (unsigned int)ScaleUiCoordinate(0x126), (DWORD)s.scaled_panel_height,
                   PTR_DAT_005832dc, 0, 0);

  s.stats_color = 0xff;
  FUN_0057b560(PTR_DAT_005832dc, s.stats_color, ScaleUiCoordinate(0x12), ScaleUiCoordinate(10), "%d", Gold);
  FUN_0057b560(PTR_DAT_005832dc, s.stats_color, ScaleUiCoordinate(0x60), ScaleUiCoordinate(10), "%d", g_food);
  s.stat_unknown = 0;
  FUN_0057b560(PTR_DAT_005832dc, s.stats_color, ScaleUiCoordinate(0xab), ScaleUiCoordinate(10), "%d", s.stat_unknown);
  FUN_0057b560(PTR_DAT_005832dc, s.stats_color, ScaleUiCoordinate(0x10c), ScaleUiCoordinate(10), "%d/%d", DAT_0078df68, DAT_00789938);

  BlitGraphicsRect(PTR_DAT_005832dc, 0, 0, (unsigned int)ScaleUiCoordinate(0x126), (DWORD)s.scaled_panel_height,
                   PTR_DAT_005832b4, ScaleUiCoordinate(0x58), ScaleUiCoordinate(0x15b));

  PTR_DAT_005832b4->unk_1c = 2;
  FUN_0054baaa(1);

  if ((DAT_00590768 == 0) && (param_1 == 0))
  {
    FUN_0046ed03();
    return (void *)0;
  }

  PTR_DAT_005832b4->unk_1c = 4;
  if (g_world_magic_avatar_sprites[0] != (EncodedImage *)0)
  {
    s.avatar_draw_y = 400 - (int)g_world_magic_avatar_sprites[0]->height / 2;

    DrawEncodedImageUiScaled(PTR_DAT_005832b4, 0x4e, s.avatar_draw_y, g_world_magic_avatar_sprites[2],
                             (int)g_world_magic_avatar_sprites[0]->width,
                             (int)g_world_magic_avatar_sprites[0]->height);
    FUN_0057b560(PTR_DAT_005832b4, s.stats_color, 100, s.avatar_draw_y + (int)g_world_magic_avatar_sprites[0]->height / 2, "%d", g_amulet_inventory[0]);

    DrawEncodedImageUiScaled(PTR_DAT_005832b4, 0xa1, s.avatar_draw_y, g_world_magic_avatar_sprites[1],
                             (int)g_world_magic_avatar_sprites[0]->width,
                             (int)g_world_magic_avatar_sprites[0]->height);
    FUN_0057b560(PTR_DAT_005832b4, s.stats_color, 0xb7, s.avatar_draw_y + (int)g_world_magic_avatar_sprites[0]->height / 2, "%d", g_amulet_inventory[1]);

    DrawEncodedImageUiScaled(PTR_DAT_005832b4, 0xf0, s.avatar_draw_y, g_world_magic_avatar_sprites[4],
                             (int)g_world_magic_avatar_sprites[0]->width,
                             (int)g_world_magic_avatar_sprites[0]->height);
    FUN_0057b560(PTR_DAT_005832b4, s.stats_color, 0x106, s.avatar_draw_y + (int)g_world_magic_avatar_sprites[0]->height / 2, "%d", g_amulet_inventory[2]);

    DrawEncodedImageUiScaled(PTR_DAT_005832b4, 0x140, s.avatar_draw_y, g_world_magic_avatar_sprites[3],
                             (int)g_world_magic_avatar_sprites[0]->width,
                             (int)g_world_magic_avatar_sprites[0]->height);
    FUN_0057b560(PTR_DAT_005832b4, s.stats_color, 0x156, s.avatar_draw_y + (int)g_world_magic_avatar_sprites[0]->height / 2, "%d", g_amulet_inventory[3]);

    DrawEncodedImageUiScaled(PTR_DAT_005832b4, 0x193, s.avatar_draw_y, g_world_magic_avatar_sprites[0],
                             (int)g_world_magic_avatar_sprites[0]->width,
                             (int)g_world_magic_avatar_sprites[0]->height);
    FUN_0057b560(PTR_DAT_005832b4, s.stats_color, 0x1a9, s.avatar_draw_y + (int)g_world_magic_avatar_sprites[0]->height / 2, "%d", g_amulet_inventory[4]);
  }

  for (s.world_magic_slot_index = 0; (int)s.world_magic_slot_index < 0xc; s.world_magic_slot_index++)
  {
    if ((g_world_magic_bitmap & (1 << ((unsigned char)s.world_magic_slot_index & 0x1f))) == 0)
    {
      continue;
    }

    FUN_004bb458((int)s.world_magic_slot_index);
    if (((int)s.world_magic_slot_index < 2) || ((s.world_magic_slot_index & 1) != 0))
    {
      DrawEncodedImageUiScaled(PTR_DAT_005832b4,
                               g_world_magic_icon_rects[s.world_magic_slot_index].x,
                               g_world_magic_icon_rects[s.world_magic_slot_index].y,
                               g_world_magic_choice_button_sprite_bank.named.normal[s.world_magic_slot_index],
                               g_world_magic_icon_rects[s.world_magic_slot_index].width,
                               g_world_magic_icon_rects[s.world_magic_slot_index].height);
    }
    else if (DAT_0078990c[s.world_magic_slot_index / 2] == 0)
    {
      DrawEncodedImageUiScaled(PTR_DAT_005832b4,
                               g_world_magic_icon_rects[s.world_magic_slot_index].x,
                               g_world_magic_icon_rects[s.world_magic_slot_index].y,
                               g_world_magic_choice_button_sprite_bank.named.pressed[s.world_magic_slot_index],
                               g_world_magic_icon_rects[s.world_magic_slot_index].width,
                               g_world_magic_icon_rects[s.world_magic_slot_index].height);
    }
    else
    {
      DrawEncodedImageUiScaled(PTR_DAT_005832b4,
                               g_world_magic_icon_rects[s.world_magic_slot_index].x,
                               g_world_magic_icon_rects[s.world_magic_slot_index].y,
                               g_world_magic_choice_button_sprite_bank.named.normal[s.world_magic_slot_index],
                               g_world_magic_icon_rects[s.world_magic_slot_index].width,
                               g_world_magic_icon_rects[s.world_magic_slot_index].height);
      DrawEncodedImageUiScaled(PTR_DAT_005832b4,
                               g_world_magic_icon_rects[s.world_magic_slot_index].x,
                               g_world_magic_icon_rects[s.world_magic_slot_index].y,
                               g_world_magic_choice_button_sprite_bank.named.icon[s.world_magic_slot_index],
                               g_world_magic_icon_rects[s.world_magic_slot_index].width,
                               g_world_magic_icon_rects[s.world_magic_slot_index].height);
    }
  }

  PTR_DAT_005832b4->unk_1c = 1;
  return PTR_DAT_005832b4;
}

// FUNCTION: SHANDALAR 0x0054a880
void FUN_0054a880(void)
{
  g_world_draw_entry_count = 0;
  g_world_draw_head_index = 0;
}

// FUNCTION: SHANDALAR 0x0054a89f
void FUN_0054a89f(FacemakerWindowBounds *window, int draw_x, int draw_y, int depth_y, EncodedImage *sprite)
{
  struct
  {
    int current_index;
    int previous_index;
  } s;

  (void)window;

  g_world_draw_screen_x[g_world_draw_entry_count] = draw_x;
  g_world_draw_screen_y[g_world_draw_entry_count] = draw_y;
  g_world_draw_depth_sort_keys[g_world_draw_entry_count] = depth_y;
  g_world_draw_sprites[g_world_draw_entry_count] = sprite;
  if (g_world_draw_entry_count == 0)
  {
    g_world_draw_next_index[g_world_draw_entry_count] = -1;
    g_world_draw_entry_count++;
  }
  else
  {
    s.current_index = g_world_draw_head_index;
    s.previous_index = -1;
    while ((g_world_draw_depth_sort_keys[s.current_index] < depth_y) && (s.current_index != -1))
    {
      s.previous_index = s.current_index;
      s.current_index = g_world_draw_next_index[s.current_index];
    }
    g_world_draw_next_index[g_world_draw_entry_count] = s.current_index;
    if (s.previous_index != -1)
    {
      g_world_draw_next_index[s.previous_index] = g_world_draw_entry_count;
    }
    else
    {
      g_world_draw_head_index = g_world_draw_entry_count;
    }
    g_world_draw_entry_count++;
  }
}

// FUNCTION: SHANDALAR 0x0054a997
void FUN_0054a997(void)
{
  int draw_index;

  draw_index = g_world_draw_head_index;
  while (draw_index != -1)
  {
    DrawEncodedImageUnscaledClipped(PTR_DAT_005832b4,
                                    g_world_draw_screen_x[draw_index] - g_world_scroll_x,
                                    g_world_draw_screen_y[draw_index] - g_world_scroll_y,
                                    g_world_draw_sprites[draw_index]);
    draw_index = g_world_draw_next_index[draw_index];
  }
}

// FUNCTION: SHANDALAR 0x0054aa04
void FUN_0054aa04(int world_subcell_x, int world_subcell_y, int *out_screen_x, int *out_screen_y)
{
  int height_delta;

  height_delta = (world_subcell_y - world_subcell_x) * global_screen_height;
  *out_screen_x = ((world_subcell_y + world_subcell_x) * global_screen_width * 2) / 0x280;
  *out_screen_y = height_delta / 0x1e0;
}

// FUNCTION: SHANDALAR 0x0054aa60
void FUN_0054aa60(int world_tile_x, int world_tile_y, int *out_screen_x, int *out_screen_y)
{
  struct
  {
    int delta_y;
    int screen_x;
    int delta_x;
    int screen_y;
  } s;

  s.delta_x = world_tile_x - g_world_camera_last_anchor_x;
  s.delta_y = world_tile_y - g_world_camera_last_anchor_y;
  s.screen_x = ((s.delta_y + s.delta_x) * global_screen_width * 2) / 0x280 + g_world_camera_viewport_width_pixels / 2;
  s.screen_y = ((s.delta_y - s.delta_x) * global_screen_height) / 0x1e0 + g_world_camera_viewport_height_pixels / 2;
  *out_screen_x = s.screen_x;
  *out_screen_y = g_world_ui_top_offset + s.screen_y;
}

// FUNCTION: SHANDALAR 0x0054aae9
void FUN_0054aae9(int world_x, int world_y, int *out_screen_x, int *out_screen_y)
{
  struct
  {
    int delta_y;
    int screen_x;
    int delta_x;
    int screen_y;
  } s;

  s.delta_x = world_x - g_world_player_world_x_cached;
  s.delta_y = world_y - g_world_player_world_y_cached;
  s.screen_x = ((s.delta_y + s.delta_x) * global_screen_width * 2) / 0x280 + g_world_camera_viewport_width_pixels / 2;
  s.screen_y = ((s.delta_y - s.delta_x) * global_screen_height) / 0x1e0 + g_world_camera_viewport_height_pixels / 2;
  *out_screen_x = s.screen_x;
  *out_screen_y = s.screen_y + g_world_ui_top_offset;
}

// FUNCTION: SHANDALAR 0x0054ab73
void FUN_0054ab73(int screen_x, int screen_y, int *out_world_x, int *out_world_y)
{
  int world_delta_y;
  int world_delta_x;

  world_delta_y = (((screen_y - g_world_ui_top_offset) - g_world_camera_viewport_height_pixels / 2) * 0x1e0) / global_screen_height;
  world_delta_x = (((screen_x - g_world_camera_viewport_width_pixels / 2) * 0x280) / global_screen_width) >> 1;
  *out_world_x = g_world_player_world_x_cached + (world_delta_x - world_delta_y) / 2;
  *out_world_y = g_world_player_world_y_cached + (world_delta_x + world_delta_y) / 2;
}

// FUNCTION: SHANDALAR 0x004318c7
int FUN_004318c7(int x, int y)
{
  if ((x < 0) || (x >= 0x40))
  {
    return 0;
  }
  if ((y < 0) || (y >= 0x40))
  {
    return 0;
  }
  return 1;
}

// FUNCTION: SHANDALAR 0x00508b89
int FUN_00508b89(int world_x, int world_y)
{
  int entry_index;

  for (entry_index = 0; entry_index < 0xf; entry_index++)
  {
    if ((g_castle_dungeon_slots[entry_index].world_x == world_x) &&
        (g_castle_dungeon_slots[entry_index].world_y == world_y))
    {
      return entry_index;
    }
  }

  return -1;
}

// FUNCTION: SHANDALAR 0x004bb040
int FUN_004bb040(int world_x, int world_y)
{
  int town_index;

  for (town_index = 0; town_index < 0x80; town_index++)
  {
    if ((g_town_slots[town_index].location_type != -1) &&
        (g_town_slots[town_index].world_x == world_x) &&
        (g_town_slots[town_index].world_y == world_y))
    {
      return town_index;
    }
  }

  return -1;
}

static __inline void ConfigureAdventureWorldViewport(void)
{
  switch (global_screen_width)
  {
  case 640:
    g_world_camera_viewport_width_pixels = 0x280;
    g_world_camera_viewport_height_pixels = 0x1e0;
    g_world_camera_minimap_panel_x = 0x80;
    g_world_camera_minimap_panel_y = 0x20;
    break;
  case 800:
    g_world_camera_viewport_width_pixels = 800;
    g_world_camera_viewport_height_pixels = 600;
    g_world_camera_minimap_panel_x = 0xa0;
    g_world_camera_minimap_panel_y = 0x28;
    break;
  case 1024:
    g_world_camera_viewport_width_pixels = 0x400;
    g_world_camera_viewport_height_pixels = 0x300;
    g_world_camera_minimap_panel_x = 0xcc;
    g_world_camera_minimap_panel_y = 0x33;
    break;
  default:
    break;
  }
}

static __inline EncodedImage *GetWorldEntitySpriteFromWorkBuffer(int entity_slot, int animation_state)
{
  int sprite_offset;

  sprite_offset = (((char)animation_state + 2U) & 7) * 0x14 + (((unsigned int)animation_state >> 8) & 0xff) * 4;
  return *(EncodedImage **)(g_opening_menu_sprite_work_buffer + entity_slot * 0xb4 + sprite_offset);
}

static __inline void DrawWorldStatusTipSprites(void)
{
  int status_layout_x[6];
  int status_layout_y[3];
  int status_frame_y[3];
  int resolution_index;
  int panel_top_y;
  int left_icon_x;
  int right_icon_x;
  int frame_y;
  EncodedImage *status_sprite;

  if (g_questnew_sprite_entries[0] == (EncodedImage *)0)
  {
    return;
  }

  status_layout_x[0] = 0xcb;
  status_layout_x[1] = 0xfd;
  status_layout_x[2] = 0x143;
  status_layout_x[3] = 0x1af;
  status_layout_x[4] = 0x21a;
  status_layout_x[5] = 0x2b0;

  status_layout_y[0] = 0x40;
  status_layout_y[1] = 0x50;
  status_layout_y[2] = 0x66;

  status_frame_y[0] = 0x48;
  status_frame_y[1] = 0x5a;
  status_frame_y[2] = 0x72;

  if (global_screen_width == 0x280)
  {
    resolution_index = 0;
  }
  else if (global_screen_width == 800)
  {
    resolution_index = 1;
  }
  else
  {
    resolution_index = 2;
  }

  status_sprite = g_questnew_sprite_entries[0];
  panel_top_y = ScaleUiCoordinate(0x105) + g_world_camera_minimap_panel_y * 2 + g_world_ui_top_offset + 0x10;
  DrawEncodedImageResampled(PTR_DAT_005832dc,
                            ScaleUiCoordinate(0x141) + 0x40,
                            panel_top_y,
                            ScaleUiCoordinate((int)status_sprite->width),
                            ScaleUiCoordinate((int)status_sprite->height),
                            status_sprite);

  if (g_tips_frame_sprite != (EncodedImage *)0)
  {
    frame_y = status_frame_y[resolution_index] + g_world_camera_minimap_panel_y * 2 + g_world_ui_top_offset + 0x10;
    DrawEncodedImageUnscaled(PTR_DAT_005832dc, 0x40, frame_y - ScaleUiCoordinate(0x30), g_tips_frame_sprite);
  }

  if (g_tips_icon_sprite != (EncodedImage *)0)
  {
    panel_top_y = status_layout_y[resolution_index] + g_world_camera_minimap_panel_y * 2 + g_world_ui_top_offset + 0x10;
    left_icon_x = status_layout_x[resolution_index];
    right_icon_x = status_layout_x[resolution_index + 3];
    DrawEncodedImageUnscaled(PTR_DAT_005832dc, (left_icon_x + 0x40) - ScaleUiCoordinate(0x40), panel_top_y, g_tips_icon_sprite);
    DrawEncodedImageUnscaled(PTR_DAT_005832dc, (right_icon_x + 0x40) - ScaleUiCoordinate(0x40), panel_top_y, g_tips_icon_sprite);
  }
}

// FUNCTION: SHANDALAR 0x0054ac08
void RenderAdventureWorldScene(int world_x, int world_y, int world_state)
{
  int saved_clip_rect_world[4];
  int saved_clip_rect_ui[4];
  int saved_minimap_clip_rect[4];
  int previous_page_number;
  int player_screen_x;
  int player_screen_y;
  int tile_x;
  int tile_y;
  int player_tile_x;
  int player_tile_y;
  int slot_index;
  int sprite_offset;
  int monster_tile_screen_x;
  int monster_tile_screen_y;
  int monster_subcell_screen_x;
  int monster_subcell_screen_y;
  int monster_screen_x;
  int monster_screen_y;
  int lair_sprite_index;
  int current_entry_type;
  int heading_sprite_width;
  int heading_sprite_height;
  int panel_top_y;
  int scaled_0x18;
  int scaled_0x20;
  int scaled_0x8c;
  int scaled_0x100;
  EncodedImage *sprite;
  ShandalarCardSlot *slot;

  previous_page_number = PTR_DAT_005832dc->page_number;
  FUN_0046ed33();

  ConfigureAdventureWorldViewport();

  g_world_player_world_x_cached = world_x;
  g_world_player_world_y_cached = world_y;
  g_world_ui_top_offset = 0x50;

  player_tile_x = (int)(world_x + ((world_x >> 0x1f) & 0x1fU)) >> 5;
  player_tile_y = (int)(world_y + ((world_y >> 0x1f) & 0x1fU)) >> 5;
  g_world_player_tile_x = player_tile_x;
  g_world_player_tile_y = player_tile_y;

  PTR_DAT_005832b4->page_number = PTR_DAT_005832dc->page_number;
  FUN_0054a880();
  PushGraphicsClipRect(saved_clip_rect_world, PTR_DAT_005832dc, 0, 0x80, PTR_DAT_005832b4->max_x, PTR_DAT_005832b4->max_y - 0x80);
  PushGraphicsClipRect(saved_clip_rect_ui, PTR_DAT_005832b4, 0, 0x80, PTR_DAT_005832b4->max_x, PTR_DAT_005832b4->max_y - 0x80);
  FUN_0054cee2((unsigned int)world_x, (unsigned int)world_y);

  FUN_0054aae9(world_x, world_y, &player_screen_x, &player_screen_y);
  PushGraphicsClipRect(saved_minimap_clip_rect, PTR_DAT_005832b4, 0x40,
                       g_world_camera_minimap_panel_y * 2 + g_world_ui_top_offset + 0x10,
                       FUN_005501dc(0x100), FUN_005501dc(0x8c));

  sprite_offset = DAT_0073ea70[7] * 4 + ((DAT_0073ea70[5] + 2U) & 7) * 0x14;
  sprite = *(EncodedImage **)(g_opening_menu_sprite_work_buffer + sprite_offset + 0xb40);
  FUN_0054a89f(PTR_DAT_005832b4,
               (player_screen_x - g_ego_sprite_width / 2) + g_world_scroll_x,
               (player_screen_y - g_ego_sprite_draw_height) + g_world_scroll_y,
               player_screen_y + g_world_scroll_y,
               sprite);

  sprite = *(EncodedImage **)(g_opening_menu_sprite_work_buffer + sprite_offset + 0xbf4);
  DrawEncodedImageUnscaledClipped(PTR_DAT_005832b4,
                                  player_screen_x - g_sego_sprite_width / 2,
                                  player_screen_y - g_sego_sprite_draw_height,
                                  sprite);

  for (slot_index = 0; slot_index < 8; slot_index = slot_index + 1)
  {
    slot = &g_lair_or_monster_slots[slot_index];
    if (slot->entry_type == -1)
    {
      continue;
    }

    tile_x = (int)(slot->world_x + ((slot->world_x >> 0x1f) & 0x1fU)) >> 5;
    tile_y = (int)(slot->world_y + ((slot->world_y >> 0x1f) & 0x1fU)) >> 5;
    if ((abs(tile_x - player_tile_x) >= 2) || (abs(tile_y - player_tile_y) >= 2))
    {
      continue;
    }

    FUN_0054aa60(slot->world_x & 0xffffffe0, slot->world_y & 0xffffffe0, &monster_tile_screen_x, &monster_tile_screen_y);
    FUN_0054aa04(slot->world_x & 0x1f, slot->world_y & 0x1f, &monster_subcell_screen_x, &monster_subcell_screen_y);
    monster_screen_x = monster_tile_screen_x + monster_subcell_screen_x;
    monster_screen_y = monster_tile_screen_y + monster_subcell_screen_y;
    current_entry_type = slot->entry_type;
    if ((gs_creature_names_00591a08[current_entry_type].metadata[0xa] & 2U) != 0)
    {
      current_entry_type = current_entry_type - (((unsigned int)g_monster_timer >> 5) & 3);
    }
    if ((gs_creature_names_00591a08[current_entry_type].metadata[0xb] & 1U) != 0)
    {
      if (FUN_004ecf30(monster_screen_x - 0x140, monster_screen_y - 0xf0) > FUN_005501dc(0x80))
      {
        continue;
      }
    }

    if (slot->entry_type == 0)
    {
      lair_sprite_index = (int)g_lair_sprite_index_by_color[slot->color];
      sprite = g_location07_sprite_entries[lair_sprite_index];
      FUN_0054a89f(PTR_DAT_005832b4,
                   monster_screen_x - FUN_005501dc(0x20),
                   monster_screen_y - FUN_005501dc(0x30),
                   monster_screen_y,
                   sprite);

      sprite = g_location07_sprite_entries[lair_sprite_index + 6];
      DrawEncodedImageUnscaledClipped(PTR_DAT_005832b4,
                                      (monster_screen_x - FUN_005501dc(0x20)) - g_world_scroll_x,
                                      (monster_screen_y - FUN_005501dc(0x30)) - g_world_scroll_y,
                                      sprite);
    }
    else
    {
      sprite = GetWorldEntitySpriteFromWorkBuffer(slot_index, slot->movement_heading);
      heading_sprite_width = (int)g_icons_sprite_entries[slot_index + 0x18];
      heading_sprite_height = (int)g_ttsprite_aux_sprite_entries[slot_index + 8];
      FUN_0054a89f(PTR_DAT_005832b4,
                   monster_screen_x - heading_sprite_width / 2,
                   monster_screen_y - heading_sprite_height,
                   monster_screen_y,
                   sprite);

      sprite = GetWorldEntitySpriteFromWorkBuffer(slot_index + 8, slot->movement_heading);
      heading_sprite_width = (int)g_icons_sprite_entries[slot_index + 0x20];
      heading_sprite_height = (int)g_ttsprite_aux_sprite_entries[slot_index + 0x10];
      DrawEncodedImageUnscaledClipped(PTR_DAT_005832b4,
                                      (monster_screen_x - heading_sprite_width / 2) - g_world_scroll_x,
                                      (monster_screen_y - heading_sprite_height) - g_world_scroll_y,
                                      sprite);
    }

    if ((((g_monster_timer >> 3) ^ (slot_index & 3)) & 3) == 0)
    {
      PTR_DAT_005832b4->unk_1c = 3;
      if ((current_entry_type >= 0) && ((unsigned int)current_entry_type < gs_creature_name_count_00593934))
      {
        strcpy(g_ui_message_buffer, FUN_00561441(current_entry_type));
      }
    }
  }

  FUN_0054a997();
  if (g_questnew_sprite_entries[0] != (EncodedImage *)0)
  {
    DrawWorldStatusTipSprites();
    panel_top_y = g_world_camera_minimap_panel_y * 2 + g_world_ui_top_offset + 0x10;
    scaled_0x18 = FUN_005501dc(0x18);
    scaled_0x20 = FUN_005501dc(0x20);
    scaled_0x8c = FUN_005501dc(0x8c);
    scaled_0x100 = FUN_005501dc(0x100);

    if (DAT_0073e9dc == 0)
    {
      if (global_screen_width == 0x400)
      {
        BlitGraphicsRect(PTR_DAT_005832dc, 0x40, panel_top_y, scaled_0x100 - 2, scaled_0x8c, PTR_DAT_005832b4,
                         (scaled_0x20 + 4) & 0xfffffffc, scaled_0x18);
      }
      else
      {
        BlitGraphicsRect(PTR_DAT_005832dc, 0x40, panel_top_y, scaled_0x100, scaled_0x8c, PTR_DAT_005832b4,
                         scaled_0x20 & 0xfffffffc, scaled_0x18);
      }
    }
    else
    {
      if (global_screen_width == 0x400)
      {
        FUN_0057d330(g_graphics_pages[PTR_DAT_005832b4->page_number]->hTempDC,
                     (scaled_0x20 + 4) & 0xfffffffc, scaled_0x18,
                     scaled_0x100 - 2, scaled_0x8c, 8, 8,
                     g_graphics_pages[PTR_DAT_005832dc->page_number]->hTempDC,
                     0x40, panel_top_y);
      }
      else
      {
        FUN_0057d330(g_graphics_pages[PTR_DAT_005832b4->page_number]->hTempDC,
                     scaled_0x20 & 0xfffffffc, scaled_0x18,
                     scaled_0x100, scaled_0x8c, 6, 6,
                     g_graphics_pages[PTR_DAT_005832dc->page_number]->hTempDC,
                     0x40, panel_top_y);
      }
      FUN_004ce992(0x2d);
      DAT_0073e9dc = 0;
    }
  }

  PTR_DAT_005832dc->clip_left = saved_clip_rect_world[0];
  PTR_DAT_005832dc->clip_top = saved_clip_rect_world[1];
  PTR_DAT_005832dc->max_x = saved_clip_rect_world[2];
  PTR_DAT_005832dc->max_y = saved_clip_rect_world[3];
  PTR_DAT_005832b4->clip_left = saved_clip_rect_ui[0];
  PTR_DAT_005832b4->clip_top = saved_clip_rect_ui[1];
  PTR_DAT_005832b4->max_x = saved_clip_rect_ui[2];
  PTR_DAT_005832b4->max_y = saved_clip_rect_ui[3];
  PTR_DAT_005832dc->page_number = previous_page_number;

  if ((world_state == 0) && (DAT_00590768 == 0))
  {
    PTR_DAT_005832b4->unk_1c = 1;
    FUN_0046ed03();
    FUN_0054baaa(0);
    FUN_0054b80d();
  }
  else
  {
    FUN_0055060c(0);
    DAT_00590768 = 0;
    FUN_0046ed03();
  }
}
