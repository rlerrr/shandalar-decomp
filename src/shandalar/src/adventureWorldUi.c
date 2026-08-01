#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "defs.h"
#include "shandalar.h"
#include "magic/src/global_state.h"
#include "shandalar_global_strings.h"
#include "facemaker/src/facemaker_types.h"
#include "drawcardlib/src/pic.h"

// GLOBAL: SHANDALAR 0x0059076c
int g_world_ui_top_offset = 128;
// GLOBAL: SHANDALAR 0x00590760
int g_world_ui_stats_color_index = 0xca;
// GLOBAL: SHANDALAR 0x00590770
unsigned char g_location_marker_sprite_lookup_by_town[0x24] = {
    0x01,
    0x07,
    0x05,
    0x0b,
    0x1c,
    0x20,
    0x19,
    0x1d,
    0x00,
    0x06,
    0x23,
    0x24,
    0x0d,
    0x13,
    0x21,
    0x22,
    0x02,
    0x08,
    0x0c,
    0x12,
    0x0e,
    0x14,
    0x1a,
    0x1e,
    0x1b,
    0x1f,
    0x26,
    0x2c,
    0x27,
    0x2d,
    0x34,
    0x3a,
    0x03,
    0x09,
    0x04,
    0x0a,
};
// GLOBAL: SHANDALAR 0x005907a8
int g_dungeon_marker_sprite_lookup[24] = {
    0x2a,
    0x30,
    0x3f,
    0x43,
    0x36,
    0x3c,
    0x35,
    0x3b,
    0x32,
    0x38,
    0x33,
    0x39,
    0x4e,
    0x54,
    0x50,
    0x56,
    0x4f,
    0x55,
    0x37,
    0x3d,
    0x4d,
    0x53,
    0x4c,
    0x52,
};
// GLOBAL: SHANDALAR 0x005907a0
int g_world_ui_cached_next_duel_life_delta = 0;
// GLOBAL: SHANDALAR 0x005907a4
int g_world_ui_cached_next_duel_card_id = -1;
// GLOBAL: SHANDALAR 0x0078cee0
int g_current_quest_giver_town_index;
// GLOBAL: SHANDALAR 0x00590808
unsigned char g_castle_sprite_lookup_by_tile_class[0x40] = {
    0x08,
    0x09,
    0x0a,
    0x0b,
    0x02,
    0x06,
    0x03,
    0x07,
    0x0c,
    0x10,
    0x0d,
    0x11,
    0x0e,
    0x12,
    0x0f,
    0x13,
    0x00,
    0x04,
    0x01,
    0x05,
    0x00,
    0x00,
    0x00,
    0x00,
    0x08,
    0x09,
    0x0a,
    0x0b,
    0x02,
    0x06,
    0x03,
    0x07,
    0x0c,
    0x10,
    0x0d,
    0x11,
    0x0e,
    0x12,
    0x0f,
    0x13,
    0x00,
    0x04,
    0x01,
    0x05,
    0x00,
    0x00,
    0x00,
    0x00,
    0xd2,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0xf4,
    0x00,
    0x00,
    0x00,
    0xce,
    0x00,
    0x00,
    0x00,
};
// GLOBAL: SHANDALAR 0x00652740
char g_world_ui_text_scratch_buffer[0x1000];
// GLOBAL: SHANDALAR 0x006696c0
char g_creature_name_with_article_buffer[0x80];
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
// GLOBAL: SHANDALAR 0x00650f04
ShandalarEntryType g_world_ui_cached_siege_entry_type;
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
// GLOBAL: SHANDALAR 0x0073e9d0
int g_world_player_tile_x;
// GLOBAL: SHANDALAR 0x0073e9d4
int g_world_player_tile_y;

// GLOBAL: SHANDALAR 0x00590a48
AdvMenuRect g_adventure_interface_world_magic_icon_rects[0xc] = {
    {22, 427, 48, 48},
    {113, 427, 48, 48},
    {28, 378, 49, 49},
    {205, 427, 48, 48},
    {112, 378, 49, 49},
    {297, 427, 48, 48},
    {195, 378, 49, 49},
    {385, 427, 48, 48},
    {274, 378, 49, 49},
    {474, 427, 48, 48},
    {354, 378, 49, 49},
    {567, 427, 48, 48},
};

// TODO: why does g_world_tile_screen_x_cache need to be oversize? it isn't resolution dependant
#ifdef MODERN_FIXES
int g_world_tile_screen_x_cache[0x10000];
int g_world_tile_screen_y_cache[0x10000];
#else
// GLOBAL: SHANDALAR 0x0073eaf0
int g_world_tile_screen_x_cache[0x1000];
// GLOBAL: SHANDALAR 0x00742af0
int g_world_tile_screen_y_cache[0x1000];
#endif

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
extern int g_adventure_ui_layout_dirty;
extern int g_world_player_animation_direction;
extern int g_world_player_animation_frame;
extern int g_world_scene_reveal_effect_pending;
extern int g_world_scroll_cache_ready;
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
extern EncodedImage *g_land_sprite_entries[22][5];
extern EncodedImage *g_sland_sprite_entries[22][5];
extern EncodedImage *g_land_tile_sprite_entries[0x10];
extern EncodedImage *g_cstline1_sprite_entries[12][7];
extern EncodedImage *g_location_marker_sprite_entries[0x9e];
extern int g_world_lair_monster_sprite_top_clips[0x10];
extern EncodedImage *g_questnew_sprite_entries[4];
extern EncodedImage *g_clocknew_sprite_entries[9];
extern EncodedImage *g_sunmoon_sprite_entries[0x14];
extern EncodedImage *g_daysnew_sprite_entries[0xc];
extern EncodedImage *g_tips_frame_sprite;
extern EncodedImage *g_tips_icon_sprite;
extern int g_world_lair_monster_sprite_widths[0x10];
extern DIBSurface *g_graphics_pages[10];
extern OpeningMenuSpriteWorkEntry g_opening_menu_sprite_work_buffer[0x20];
extern EncodedImage *g_world_magic_avatar_sprites[5];
extern EncodedImage *g_worlds_extra_sprite_entries[4];
extern EncodedImage *g_castles_sprite_entries[20];
extern EncodedImage *g_wizard_controlled_town_sprite_entries[5];
extern EncodedImage *DAT_00749418;
extern WorldMagicChoiceButtonSpriteBank g_world_magic_choice_button_sprite_bank;

extern int g_next_duel_life_delta;
extern int g_next_duel_card_id;
extern int Gold;
extern int g_food;

extern int g_deck_total_card_count;
extern int g_deck_active_card_count;
extern int DAT_00586498;
extern card_data_t global_cards_data[];
extern char g_ini_string_scratch[0x28];
extern char g_itoa_buffer[0x20];

void FUN_0046ed03(void);
void FUN_0046ed33(void);
int ScaleUiCoordinateFrom320(int value);
AdvMenuRect *PushGraphicsClipRect(AdvMenuRect *saved_clip_rect, FacemakerWindowBounds *page, int x, int y, int width, int height);
void BlitGraphicsRect(FacemakerWindowBounds *dst, unsigned int dst_x, int dst_y, unsigned int width, DWORD height,
                      FacemakerWindowBounds *src, int src_x, int src_y);
void DrawEncodedImageResampled(FacemakerWindowBounds *dst, int x, int y, int width, int height, EncodedImage *encoded_image);
void DrawEncodedImageUnscaled(FacemakerWindowBounds *dst, int x, int y, EncodedImage *encoded_image);
void DrawEncodedImageUnscaledClipped(FacemakerWindowBounds *dst, int x, int y, EncodedImage *encoded_image);
void DrawEncodedImageUiScaled(FacemakerWindowBounds *dst, int x_320, int y_200, EncodedImage *sprite, int width_320, int height_200);
int DrawTextFormatted(FacemakerWindowBounds *dst, int text_color, int parse_format, int centered, int draw_shadow, int multiline, int x, int y, int *arg_ptr);
void DrawTextAt(FacemakerWindowBounds *window, int color, int x, int y, char *text, ...);
void DrawWorldUiFormattedText(FacemakerWindowBounds *window, int color_index, int x, int y, char *format, ...);
unsigned int BlitRectByRandomTileOrder(HDC dst_hdc, int dst_x, int dst_y, int width, int height, int block_w, int block_h,
                                       HDC src_hdc, int src_x, int src_y);
void DrawFormattedTextShadowedCentered(FacemakerWindowBounds *window, int color_index, int x, int y, char *format, ...);
void DelayUiTicks(int param_1);
int ApproximateDistance(int x, int y);
int FUN_004bb458(int param_1);
int single_color_test_bit_to_color_t(int param_1);
unsigned int GetWorldTileType(int x, int y);
unsigned int FUN_004314ca(int x, int y);
void SetWorldMapPixelFlags(unsigned int mask, int x, int y);
unsigned int GetWorldTileMagicMask(unsigned int tile_mask);
unsigned int GetGraphicsPixelColorRef(FacemakerWindowBounds *window, int x, int y);
void QueueWorldSpriteForDraw(FacemakerWindowBounds *window, int draw_x, int draw_y, int depth_y, EncodedImage *sprite);
void DrawWorldTileRange(unsigned int world_x, unsigned int world_y, int tile_x_start, int tile_x_end, int tile_y_start, int tile_y_end,
                        int edge_mode, int draw_mode);
void ScreenToWorldPoint(int screen_x, int screen_y, int *out_world_x, int *out_world_y);
int FUN_004318c7(int x, int y);
int FUN_00508b89(int world_x, int world_y);
int FUN_004bb040(int world_x, int world_y);
char *BuildQuestLocationName(int town_index, int quest_destination, int mana_castle_index);
void WorldPointToScreen(int world_x, int world_y, int *out_screen_x, int *out_screen_y);
void RefreshAdventureInterfaceLayout(void);
int RunTextMenuAtScaled(char *menu_text, int x_320_scale, unsigned int y_200_scale);
int CountDuelPoolEligibleTowns(void);
void AddJournalEntry(int entry_type, int entry_arg);
int GetRelativeWorldQuadrant(int world_x, int world_y);
int FindDeckSlotForQuestColorAndType(unsigned char quest_color, int quest_bitmap_mask);
char *FUN_004f2e17(int town_index);
char *GetQuestCardClassName(int quest_bitmap_mask);
int FindNearestTownIndex(int world_x, int world_y);
char *BuildCreatureNameWithArticle(int creature_type);
char *BuildTownDisplayName(int town_index);
DWORD FormatMessageFromStringStripCarriageReturns(char *dst, DWORD max_length, LPCVOID format, ...);

// FUNCTION: SHANDALAR 0x005501dc
int ScaleUiCoordinateFrom320(int value)
{
  return (value * global_screen_width) / 0x140;
}

// FUNCTION: SHANDALAR 0x004bb0ca
int FindNearestTownIndex(int world_x, int world_y)
{
  struct
  {
    int nearest_town_distance;
    int nearest_town_index;
    int town_index;
    int town_distance;
  } s;

  s.nearest_town_index = -1;
  s.nearest_town_distance = 0x7fff;
  for (s.town_index = 0; s.town_index < 0x80; s.town_index = s.town_index + 1)
  {
    if (g_town_slots[s.town_index].location_type == -1)
    {
      continue;
    }
    s.town_distance = ApproximateDistance(g_town_slots[s.town_index].world_x - world_x, g_town_slots[s.town_index].world_y - world_y);
    if (s.town_distance < s.nearest_town_distance)
    {
      s.nearest_town_distance = s.town_distance;
      s.nearest_town_index = s.town_index;
    }
  }
  return s.nearest_town_index;
}

// FUNCTION: SHANDALAR 0x00550220
char *BuildTownDisplayName(int town_index)
{
  int second_half_index;
  int first_half_index;

  second_half_index = town_index / 8 % 0x10;
  if (second_half_index < 0)
  {
    second_half_index = 0;
  }
  if (second_half_index > 0xf)
  {
    second_half_index = 0xf;
  }

  first_half_index = town_index % 0x10;
  if (first_half_index < 0)
  {
    first_half_index = 0;
  }
  if (first_half_index > 0xf)
  {
    first_half_index = 0xf;
  }

  if (g_town_slots[town_index].location_type != 1)
  {
    FormatMessageFromStringStripCarriageReturns(g_world_ui_text_scratch_buffer, 0x1000, gs_cityname_format_left_0074c950, gs_citynames_firsthalf_0077e060[second_half_index],
                                                gs_citynames_secondhalf_007653e0[first_half_index]);
  }
  else
  {
    FormatMessageFromStringStripCarriageReturns(g_world_ui_text_scratch_buffer, 0x1000, gs_cityname_format_right_0077f190, gs_citynames_firsthalf_0077e060[second_half_index]);
  }
  return g_world_ui_text_scratch_buffer;
}

// FUNCTION: SHANDALAR 0x004f2e17
char *FUN_004f2e17(int town_index)
{
  int card_color_index;
  int city_card_class;
  char *city_card_text;

  card_color_index = single_color_test_bit_to_color_t(g_town_slots[town_index].trade_color_and_type & 0xff);
  city_card_class = g_town_slots[town_index].trade_color_and_type >> 8;
  if (city_card_class == 7)
  {
    city_card_class = 6;
  }
  if ((g_town_slots[town_index].trade_color_and_type & 0xff))
  {
    city_card_text = gs_citycardtext_0074ccf0[card_color_index * 7 + city_card_class];
  }
  else
  {
    city_card_text = gs_citycardtext_0074ccf0[city_card_class + 0x2a];
  }
  return city_card_text;
}

// FUNCTION: SHANDALAR 0x004f21bb
int GetRelativeWorldQuadrant(int world_x, int world_y)
{
  struct
  {
    int delta_y;
    int delta_x;
    int quadrant_index;
  } s;

  s.delta_x = world_x - g_world_player_tile_x;
  s.delta_y = world_y - g_world_player_tile_y;
  if (s.delta_y > 0)
  {
    if (0 < s.delta_x)
    {
      s.quadrant_index = 1;
    }
    else
    {
      s.quadrant_index = 2;
    }
  }
  else
  {
    if (0 < s.delta_x)
    {
      s.quadrant_index = 0;
    }
    else
    {
      s.quadrant_index = 3;
    }
  }
  return s.quadrant_index;
}

// FUNCTION: SHANDALAR 0x004f2322
int FindDeckSlotForQuestColorAndType(unsigned char quest_color, int quest_bitmap_mask)
{
  struct
  {
    int deck_slot_index;
    int card_id;
  } s;

  for (s.deck_slot_index = 0; s.deck_slot_index < 500; ++s.deck_slot_index)
  {
    if (deck[s.deck_slot_index] != -1)
    {
      s.card_id = deck[s.deck_slot_index] & 0xfff;
      if (((1 << quest_color) & (int)(char)global_cards_data[s.card_id].color) != 0)
      {
        if ((quest_bitmap_mask & (unsigned char)global_cards_data[s.card_id].type) != 0)
        {
          return s.deck_slot_index + 1;
        }
      }
    }
  }
  return 0;
}

// FUNCTION: SHANDALAR 0x00428615
char *GetQuestCardClassName(int quest_bitmap_mask)
{
  char *result;

  switch (quest_bitmap_mask)
  {
  case 1:
    result = gs_cardclassnames_0077cf70[0];
    break;
  case 2:
    result = gs_cardclassnames_0077cf70[1];
    break;
  case 4:
    result = gs_cardclassnames_0077cf70[2];
    break;
  case 8:
    result = gs_cardclassnames_0077cf70[3];
    break;
  case 0x10:
    result = gs_cardclassnames_0077cf70[4];
    break;
  case 0x20:
    result = gs_cardclassnames_0077cf70[5];
    break;
  case 0x40:
    result = gs_cardclassnames_0077cf70[6];
    break;
  case 0x42:
    result = gs_cardclassnames_0077cf70[7];
    break;
  case 0x80:
    result = gs_cardclassnames_0077cf70[8];
    break;
  default:
    result = "";
    break;
  }
  return result;
}

// FUNCTION: SHANDALAR 0x00561465
char *BuildCreatureNameWithArticle(int creature_type)
{
  strcpy(g_creature_name_with_article_buffer, g_shandalar_monster_definitions[creature_type].article);
  strcat(g_creature_name_with_article_buffer, g_shandalar_monster_definitions[creature_type].name);
  return g_creature_name_with_article_buffer;
}

// FUNCTION: SHANDALAR 0x00564e70
DWORD FormatMessageFromStringStripCarriageReturns(char *dst, DWORD max_length, LPCVOID format, ...)
{
  struct
  {
    DWORD result;
    va_list args;
    char *carriage_return;
  } s;

  s.args = (va_list)(&format + 1);
  s.result = FormatMessageA(0x400, format, 0, 0, dst, max_length, &s.args);
  while ((s.carriage_return = strchr(dst, '\r')) != (char *)0)
  {
    strcpy(s.carriage_return, s.carriage_return + 1);
  }
  s.args = (va_list)0;
  return s.result;
}

// FUNCTION: SHANDALAR 0x00550416
char *BuildQuestLocationName(int town_index, int quest_destination, int mana_castle_index)
{
  int city_name_index = (town_index / 8) % 16;
  int sign_adjust;
  int location_type;
  if (city_name_index < 0)
  {
    city_name_index = 0;
  }
  if (city_name_index > 15)
  {
    city_name_index = 15;
  }
  sign_adjust = town_index % 16;
  if (sign_adjust < 0)
  {
    sign_adjust = 0;
  }
  if (sign_adjust > 15)
  {
    sign_adjust = 15;
  }

  location_type = g_town_slots[town_index].location_type;
  if ((location_type > 1) || (town_index == quest_destination))
  {
    if (location_type == 4)
    {
      sprintf(g_world_ui_text_scratch_buffer, (char *)gs_cityname_manacastle_0077de00 + mana_castle_index * 100);
    }
    else if (location_type > 1)
    {
      FormatMessageFromStringStripCarriageReturns(g_world_ui_text_scratch_buffer, 0x1000, gs_cityname_format_left_0074c950, gs_citynames_firsthalf_0077e060[city_name_index], gs_citynames_secondhalf_007653e0[sign_adjust]);
    }
    else if (location_type == 1)
    {
      FormatMessageFromStringStripCarriageReturns(g_world_ui_text_scratch_buffer, 0x1000, gs_cityname_format_right_0077f190, gs_citynames_firsthalf_0077e060[city_name_index]);
    }
    else
    {
      strcpy(g_world_ui_text_scratch_buffer, gs_citynames_firsthalf_0077e060[city_name_index]);
    }
  }
  else
  {
    strcpy(g_world_ui_text_scratch_buffer, gs_cityname_village_0077cfd0);
  }
  return g_world_ui_text_scratch_buffer;
}

// FUNCTION: SHANDALAR 0x0054cee2
void UpdateWorldViewportBuffer(int world_x, int world_y)
{
  struct
  {
    int clip_dst_y;
    int clip_dst_x;
    int clip_mode_x;
    int clip_height;
    int clip_width;
    int clip_src_y;
    int clip_src_x;
    int clip_mode_y;
    int any_edge_needed;
    int need_top_edge;
    int need_left_edge;
    int need_right_edge;
    int need_bottom_edge;
    int tile_x;
    int scroll_delta_y;
    int origin_screen_x;
    int scroll_delta_x;
    int redraw_x_end;
    int snapped_screen_y;
    int redraw_y_end;
    int snapped_world_y;
    int snapped_screen_x;
    int redraw_y_start;
    int redraw_x_start;
    int snapped_world_x;
    int origin_screen_y;
    int saved_page_number;
    int world_screen_y;
    int world_screen_x;
    int tile_y;
  } s;

  s.saved_page_number = PTR_DAT_005832b4->page_number;
  PTR_DAT_005832b4->page_number = 2;

  g_world_player_world_x_cached = world_x;
  g_world_player_world_y_cached = world_y;

  s.snapped_world_x = world_x - (world_x & 0x1f);
  s.snapped_world_y = world_y - (world_y & 0x1f);
  WorldPointToScreen(s.snapped_world_x, s.snapped_world_y, &s.snapped_screen_x, &s.snapped_screen_y);
  WorldPointToScreen(world_x, world_y, &s.world_screen_x, &s.world_screen_y);
  WorldPointToScreen(0, 0, &s.origin_screen_x, &s.origin_screen_y);

  if ((g_world_camera_initialized != 0) && (g_world_scroll_cache_ready != 0))
  {
    s.need_left_edge = 0;
    s.need_right_edge = 0;
    s.need_top_edge = 0;
    s.need_bottom_edge = 0;

    s.tile_x = world_x / 32;
    s.tile_y = world_y / 32;
    s.scroll_delta_x = g_world_tile_screen_x_cache[(s.tile_y << 8) + s.tile_x] - s.snapped_screen_x;
    g_world_scroll_x = s.scroll_delta_x;
    s.scroll_delta_y = g_world_tile_screen_y_cache[(s.tile_y << 8) + s.tile_x] - s.snapped_screen_y;
    g_world_scroll_y = s.scroll_delta_y;

    if ((s.scroll_delta_x != 0) || (s.scroll_delta_y != 0))
    {
      if (s.scroll_delta_x < 0 && g_world_camera_scroll_limit_left >= s.scroll_delta_x)
      {
        s.need_left_edge = 1;
      }
      if (s.scroll_delta_x > 0 && g_world_camera_scroll_limit_right <= s.scroll_delta_x)
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

    s.any_edge_needed = s.need_bottom_edge | s.need_top_edge | s.need_right_edge | s.need_left_edge;
    if (s.any_edge_needed == 0)
    {
      BlitGraphicsRect(PTR_DAT_005832b4, g_world_camera_viewport_left + s.scroll_delta_x, g_world_camera_viewport_top + s.scroll_delta_y,
                       g_world_camera_viewport_width_scaled, g_world_camera_viewport_height_scaled,
                       PTR_DAT_005832dc, 0x40, g_world_camera_minimap_panel_y * 2 + g_world_ui_top_offset + 0x10);

      PTR_DAT_005832b4->page_number = s.saved_page_number;
      DrawWorldTileRange(g_world_camera_anchor_world_x, g_world_camera_anchor_world_y, -5, 4, -7, 9, 0, 1);
      g_world_player_world_x_cached = world_x;
      g_world_player_world_y_cached = world_y;
      return;
    }
    else
    {
      s.clip_width = PTR_DAT_005832b4->max_x;
      s.clip_height = PTR_DAT_005832b4->max_y;
      s.clip_mode_x = 0;
      s.clip_mode_y = 0;

      DrawWorldTileRange(world_x, world_y, -5, 4, -7, 9, 0, 1);

      if (s.scroll_delta_x > 0)
      {
        s.clip_dst_x = s.scroll_delta_x;
        s.clip_src_x = 0;
        s.clip_width = PTR_DAT_005832b4->max_x - s.scroll_delta_x;
        s.clip_mode_x = 2;
      }
      else if (s.scroll_delta_x < 0)
      {
        s.clip_dst_x = 0;
        s.clip_src_x = -s.scroll_delta_x;
        s.clip_width = PTR_DAT_005832b4->max_x - -s.scroll_delta_x;
        s.clip_mode_x = 1;
      }
      else
      {
        s.clip_dst_x = 0;
        s.clip_src_x = 0;
        s.clip_width = PTR_DAT_005832b4->max_x;
        s.clip_mode_x = -1;
      }

      if (s.scroll_delta_y > 0)
      {
        s.clip_dst_y = s.scroll_delta_y + 0x80;
        s.clip_src_y = 0x80;
        s.clip_height = (PTR_DAT_005832b4->max_y - s.scroll_delta_y) - 0x80;
        s.clip_mode_y = 2;
      }
      else if (s.scroll_delta_y < 0)
      {
        s.clip_dst_y = 0x80;
        s.clip_src_y = 0x80 - s.scroll_delta_y;
        s.clip_height = (PTR_DAT_005832b4->max_y - -s.scroll_delta_y) - 0x80;
        s.clip_mode_y = 1;
      }
      else
      {
        s.clip_dst_y = 0x80;
        s.clip_src_y = 0x80;
        s.clip_height = PTR_DAT_005832b4->max_y - 0x80;
        s.clip_mode_y = -1;
      }

      BlitGraphicsRect(PTR_DAT_005832b4, s.clip_dst_x, s.clip_dst_y, s.clip_width, (DWORD)s.clip_height,
                       PTR_DAT_005832b4, s.clip_src_x, s.clip_src_y);

      if (s.clip_mode_x == 1)
      {
        DrawWorldTileRange(world_x, world_y, -3, -1, -7, 7, 1, 0);
        if (s.clip_mode_y == 1)
        {
          DrawWorldTileRange(world_x, world_y, -1, 4, -7, -3, 1, 0);
        }
        else if (s.clip_mode_y == 2)
        {
          DrawWorldTileRange(world_x, world_y, -1, 4, 3, 7, 1, 0);
        }
      }
      else if (s.clip_mode_x == 2)
      {
        DrawWorldTileRange(world_x, world_y, 2, 5, -7, 7, 1, 0);
        if (s.clip_mode_y == 1)
        {
          DrawWorldTileRange(world_x, world_y, -3, 2, -7, -3, 1, 0);
        }
        else if (s.clip_mode_y == 2)
        {
          DrawWorldTileRange(world_x, world_y, -3, 2, 3, 7, 1, 0);
        }
      }
      else if (s.clip_mode_y == 1)
      {
        DrawWorldTileRange(world_x, world_y, -3, 5, -7, -3, 1, 0);
      }
      else if (s.clip_mode_y == 2)
      {
        DrawWorldTileRange(world_x, world_y, -3, 5, 3, 7, 1, 0);
      }

      DrawWorldTileRange(world_x, world_y, -4, 6, -9, 0xb, 2, 0);

      g_world_camera_origin_screen_x = s.origin_screen_x;
      g_world_camera_origin_screen_y = s.origin_screen_y;
      g_world_camera_anchor_world_x = world_x;
      g_world_camera_anchor_world_y = world_y;
      g_world_scroll_x = 0;
      g_world_scroll_y = 0;
      g_world_camera_last_anchor_x = world_x;
      g_world_camera_last_anchor_y = world_y;
    }
  }
  else
  {
    s.redraw_y_start = -9;
    s.redraw_x_end = 9;
    s.redraw_x_start = -4;
    s.redraw_y_end = 6;

    g_world_camera_origin_screen_x = s.origin_screen_x;
    g_world_camera_origin_screen_y = s.origin_screen_y;
    g_world_camera_anchor_world_x = world_x;
    g_world_camera_anchor_world_y = world_y;
    g_world_scroll_x = 0;
    g_world_scroll_y = 0;
    g_world_camera_last_anchor_x = world_x;
    g_world_camera_last_anchor_y = world_y;
    g_world_camera_viewport_left = 0x40;
    g_world_camera_viewport_top = g_world_camera_minimap_panel_y * 2 + g_world_ui_top_offset + 0x10;
    g_world_camera_viewport_width_scaled = ScaleUiCoordinateFrom320(0x100);
    g_world_camera_viewport_height_scaled = ScaleUiCoordinateFrom320(0x8c);
    g_world_camera_scroll_limit_left = -0x40;
    g_world_camera_scroll_limit_right = 0x40;
    g_world_camera_scroll_limit_top = 0x80 - g_world_camera_viewport_top;
    g_world_camera_scroll_limit_bottom = PTR_DAT_005832b4->max_y - (g_world_camera_viewport_top + g_world_camera_viewport_height_scaled);
    g_world_camera_initialized = 1;
    g_world_scroll_cache_ready = 1;

    DrawWorldTileRange(world_x, world_y, s.redraw_x_start, s.redraw_y_end, s.redraw_y_start, s.redraw_x_end, 1, 0);
    DrawWorldTileRange(world_x, world_y, s.redraw_x_start - 2, s.redraw_y_end, s.redraw_y_start, s.redraw_x_end + 2, 2, 1);
  }

  g_world_player_world_x_cached = world_x;
  g_world_player_world_y_cached = world_y;
  BlitGraphicsRect(PTR_DAT_005832b4, g_world_camera_viewport_left, g_world_camera_viewport_top,
                   g_world_camera_viewport_width_scaled, g_world_camera_viewport_height_scaled,
                   PTR_DAT_005832dc, 0x40, g_world_camera_minimap_panel_y * 2 + g_world_ui_top_offset + 0x10);
  PTR_DAT_005832b4->page_number = s.saved_page_number;
}

// FUNCTION: SHANDALAR 0x0054d619
void DrawWorldTileRange(unsigned int world_x, unsigned int world_y, int tile_x_start, int tile_x_end, int tile_y_start, int tile_y_end,
                        int edge_mode, int draw_mode)
{
  struct
  {
    int terrain_edge_x_adjust;           /* ebp-0xb4 */
    int terrain_edge_y_adjust;           /* ebp-0xb0 */
    int terrain_draw_x_adjust;           /* ebp-0xac */
    int terrain_draw_y_adjust;           /* ebp-0xa8 */
    int terrain_draw_base_y_adjust;      /* ebp-0xa4 */
    int coastline_bits_before_dec;       /* ebp-0xa0 */
    int road_bits_before_dec;            /* ebp-0x9c */
    EncodedImage *marker_sprite;         /* ebp-0x98 */
    EncodedImage *town_marker_sprite;    /* ebp-0x94 */
    int town_marker_height;              /* ebp-0x90 */
    int wizard_index;                    /* ebp-0x8c */
    EncodedImage *dungeon_marker_sprite; /* ebp-0x88 */
    EncodedImage *second_terrain_sprite; /* ebp-0x84 */
    EncodedImage *first_terrain_sprite;  /* ebp-0x80 */
    int tile_x;                          /* ebp-0x7c */
    int terrain_offset_y;                /* ebp-0x78 */
    int tile_screen_x;                   /* ebp-0x74 */
    int tile_base_screen_y;              /* ebp-0x70 */
    int terrain_flip_side;               /* ebp-0x6c */
    int terrain_pattern;                 /* ebp-0x68 */
    unsigned int neighbor_tile_class;    /* ebp-0x64 */
    int terrain_offset_x;                /* ebp-0x60 */
    unsigned int neighbor_tile_type;     /* ebp-0x5c */
    int terrain_column;                  /* ebp-0x58 */
    unsigned int tile_mask;              /* ebp-0x54 */
    int snapped_screen_y;                /* ebp-0x50 */
    unsigned int tile_class;             /* ebp-0x4c */
    int snapped_world_y;                 /* ebp-0x48 */
    int snapped_screen_x;                /* ebp-0x44 */
    unsigned int road_mask;              /* ebp-0x40 */
    int scratch_index;                   /* ebp-0x3c */
    int i;                               /* ebp-0x38 */
    int snapped_world_x;                 /* ebp-0x34 */
    int coastline_bits;                  /* ebp-0x30 */
    int terrain_row;                     /* ebp-0x2c */
    unsigned int tile_type;              /* ebp-0x28 */
    unsigned int road_bits;              /* ebp-0x24 */
    int tile_screen_y;                   /* ebp-0x20 */
    int saved_page_number;               /* ebp-0x1c */
    int world_tile_y;                    /* ebp-0x18 */
    int town_index;                      /* ebp-0x14 */
    int world_tile_x;                    /* ebp-0x10 */
    int road_connection_bits;            /* ebp-0x0c */
    int terrain_height;                  /* ebp-0x08 */
    int tile_y;                          /* ebp-0x04 */
  } s;

  s.saved_page_number = PTR_DAT_005832b4->page_number;
  PTR_DAT_005832b4->page_number = 2;

  g_world_player_world_x_cached = world_x;
  g_world_player_world_y_cached = world_y;
  s.snapped_world_x = (int)world_x - ((int)world_x & 0x1f);
  s.snapped_world_y = (int)world_y - ((int)world_y & 0x1f);
  WorldPointToScreen(s.snapped_world_x, s.snapped_world_y, &s.snapped_screen_x, &s.snapped_screen_y);
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

      ScreenToWorldPoint(s.tile_screen_x + 0x10, s.tile_screen_y, &s.world_tile_x, &s.world_tile_y);
      s.world_tile_x = s.world_tile_x / 32;
      s.world_tile_y = s.world_tile_y / 32;

      if ((abs(s.tile_y) <= 7) && (abs(s.tile_x) <= 3))
      {
        SetWorldMapPixelFlags(0x80, s.world_tile_x, s.world_tile_y);
      }

      /* Tile flags/type for rendering decisions. */
      s.tile_mask = FUN_004314ca(s.world_tile_x, s.world_tile_y);
      s.tile_type = s.tile_mask & 0xf;

      if ((draw_mode != 0) &&
          (s.world_tile_y >= 0) && (s.world_tile_x >= 0) &&
          (s.world_tile_y <= 0x3f) && (s.world_tile_x <= 0x3f))
      {
        g_world_tile_screen_x_cache[s.world_tile_y * 0x100 + s.world_tile_x] = s.tile_screen_x;
        g_world_tile_screen_y_cache[s.world_tile_y * 0x100 + s.world_tile_x] = s.tile_screen_y;
      }

      s.tile_screen_y -= g_world_camera_minimap_panel_y;
      s.tile_class = GetWorldTileMagicMask((unsigned int)s.tile_type);

      if ((edge_mode == 2) || ((draw_mode != 0) && (edge_mode == 0)))
      {
      }
      else
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
              s.marker_sprite = g_land_tile_sprite_entries[4];
            }
            else
            {
              s.marker_sprite = g_land_tile_sprite_entries[0];
            }
            DrawEncodedImageUnscaledClipped(PTR_DAT_005832b4, s.tile_screen_x, s.tile_screen_y, s.marker_sprite);
          }

          s.coastline_bits = s.road_connection_bits = 0;
          for (s.i = 1; s.i <= 8; s.i++)
          {
            s.coastline_bits >>= 1;
            s.road_connection_bits >>= 1;
            s.neighbor_tile_type = GetWorldTileType(g_neighbor_dx[s.i] + s.world_tile_x, g_neighbor_dy[s.i] + s.world_tile_y);
            s.neighbor_tile_class = GetWorldTileMagicMask(s.neighbor_tile_type);

            if ((s.neighbor_tile_type == 0) || (s.neighbor_tile_type == 8) ||
                ((s.neighbor_tile_type != (unsigned int)s.tile_type) && (((s.tile_class | s.neighbor_tile_class) & 4) != 0)))
            {
              s.coastline_bits |= 0x80;
            }

            if ((s.tile_class != 2) && (s.neighbor_tile_class == 2))
            {
              s.road_connection_bits |= 0x80;
            }
          }

          s.scratch_index = s.road_connection_bits;
          s.road_connection_bits |= s.road_connection_bits << 8;
          if ((s.road_connection_bits != 0) && (DAT_00586498 == 0))
          {
            for (s.i = 0; s.i < 4; s.i++)
            {
              switch (s.i)
              {
              case 0:
                s.road_bits = s.road_connection_bits & 7;
                break;
              case 3:
                s.road_bits = (s.road_connection_bits >> 2) & 7;
                break;
              case 1:
                s.road_bits = (s.road_connection_bits >> 4) & 7;
                break;
              case 2:
                s.road_bits = (s.road_connection_bits >> 6) & 7;
                break;
              }

              s.road_bits_before_dec = s.road_bits;
              s.road_bits--;
              if (s.road_bits_before_dec != 0)
              {
                if (s.i < 2)
                {
                  DrawEncodedImageUnscaledClipped(PTR_DAT_005832b4,
                                                  s.tile_screen_x + g_world_camera_minimap_panel_y,
                                                  (s.i & 1) * g_world_camera_minimap_panel_y + s.tile_screen_y,
                                                  g_cstline1_sprite_entries[s.i + 4][s.road_bits]);
                }
                else
                {
                  DrawEncodedImageUnscaledClipped(PTR_DAT_005832b4,
                                                  (s.i & 1) * g_world_camera_minimap_panel_y * 2 + s.tile_screen_x,
                                                  s.tile_screen_y + g_world_camera_minimap_panel_y / 2,
                                                  g_cstline1_sprite_entries[s.i + 4][s.road_bits]);
                }
              }
            }
          }

          s.scratch_index = s.coastline_bits;
          s.coastline_bits |= s.coastline_bits << 8;
          if ((s.coastline_bits != 0) && (DAT_00586498 == 0))
          {
            for (s.i = 0; s.i < 4; s.i++)
            {
              switch (s.i)
              {
              case 0:
                s.road_bits = s.coastline_bits & 7;
                break;
              case 3:
                s.road_bits = (s.coastline_bits >> 2) & 7;
                break;
              case 1:
                s.road_bits = (s.coastline_bits >> 4) & 7;
                break;
              case 2:
                s.road_bits = (s.coastline_bits >> 6) & 7;
                break;
              }

              s.coastline_bits_before_dec = s.road_bits;
              s.road_bits--;
              if (s.coastline_bits_before_dec != 0)
              {
                if (s.i < 2)
                {
                  DrawEncodedImageUnscaledClipped(PTR_DAT_005832b4,
                                                  s.tile_screen_x + g_world_camera_minimap_panel_y,
                                                  (s.i & 1) * g_world_camera_minimap_panel_y + s.tile_screen_y,
                                                  g_cstline1_sprite_entries[s.i][s.road_bits]);
                }
                else
                {
                  DrawEncodedImageUnscaledClipped(PTR_DAT_005832b4,
                                                  (s.i & 1) * g_world_camera_minimap_panel_y * 2 + s.tile_screen_x,
                                                  s.tile_screen_y + g_world_camera_minimap_panel_y / 2,
                                                  g_cstline1_sprite_entries[s.i][s.road_bits]);
                }
              }
            }
          }
        }

        if (FUN_004318c7(s.world_tile_x, s.world_tile_y) != 0)
        {
          s.road_mask = GetGraphicsPixelColorRef(PTR_DAT_00583304, s.world_tile_x, s.world_tile_y + 0x40);
        }
        else
        {
          s.road_mask = 0;
        }

        if (s.road_mask != 0)
        {
          for (s.i = 0; s.i < 8; s.i++)
          {
            if (((s.road_mask & (1 << (unsigned char)s.i)) != 0) && (edge_mode != 0))
            {
              DrawEncodedImageUnscaledClipped(PTR_DAT_005832b4, s.tile_screen_x, s.tile_screen_y, g_road_sprite_entries[(s.i - 2 & 7) + 1]);
            }
          }
        }
      }

      if (edge_mode == 1)
      {
        continue;
      }

      s.tile_base_screen_y = s.tile_screen_y + g_world_camera_minimap_panel_y;
      if (s.tile_mask & 0x10)
      {
      }
      else
      {
        s.terrain_flip_side = ((-(s.world_tile_y * 2 - s.world_tile_y) - (s.world_tile_x * 2 - s.world_tile_x) & 2) != 0);
        s.terrain_pattern = abs(s.world_tile_y) * 3 + abs(s.world_tile_x) * 7;
        switch (s.tile_type)
        {
        case 0:
          s.terrain_column = 0;
          break;
        case 1:
          s.terrain_column = 1;
          break;
        case 2:
          s.terrain_column = 2;
          break;
        case 3:
          s.terrain_column = 3;
          break;
        case 5:
          s.terrain_column = 4;
          break;
        case 6:
          s.terrain_column = 5;
          break;
        case 8:
          s.terrain_column = 6;
          break;
        case 10:
          s.terrain_column = 7;
          break;
        case 0xd:
          s.terrain_column = 8;
          break;
        case 0xf:
          s.terrain_column = -1;
          break;
        default:
          s.terrain_column = -1;
          break;
        }

        s.scratch_index = 0;
        s.terrain_offset_x = g_world_camera_minimap_panel_x / ((s.terrain_pattern % 5) * 2 + 4);
        s.terrain_offset_y = ScaleUiCoordinate((s.terrain_pattern % 4) * 2 + 5);
        if (s.tile_type == 1)
        {
          s.terrain_offset_y = 0;
          s.terrain_offset_x = s.terrain_offset_y;
        }

        if (s.terrain_column >= 0)
        {
          s.terrain_row = s.terrain_pattern % 0xb;
          if (5 <= s.terrain_column)
          {
            s.terrain_column %= 5;
            s.terrain_row += 0xb;
          }
          s.first_terrain_sprite = g_land_sprite_entries[s.terrain_row][s.terrain_column];
          if (s.first_terrain_sprite->top_clip != 0xffff)
          {
            s.terrain_height = s.first_terrain_sprite->top_clip;
          }
          else
          {
            s.terrain_height = 0;
          }

          if (draw_mode != 0)
          {
            if (s.scratch_index != 0)
            {
              s.terrain_draw_base_y_adjust = s.terrain_offset_y;
            }
            else
            {
              s.terrain_draw_base_y_adjust = -s.terrain_offset_y;
            }

            if (s.scratch_index != 0)
            {
              s.terrain_draw_y_adjust = s.terrain_offset_y;
            }
            else
            {
              s.terrain_draw_y_adjust = -s.terrain_offset_y;
            }

            if (s.scratch_index != 0)
            {
              s.terrain_draw_x_adjust = -s.terrain_offset_x;
            }
            else
            {
              s.terrain_draw_x_adjust = s.terrain_offset_x;
            }

            QueueWorldSpriteForDraw(PTR_DAT_005832b4,
                                    s.tile_screen_x + s.terrain_draw_x_adjust,
                                    (s.tile_base_screen_y - s.terrain_height) + s.terrain_draw_y_adjust,
                                    s.terrain_draw_base_y_adjust + s.tile_base_screen_y,
                                    g_land_sprite_entries[s.terrain_row][s.terrain_column]);
          }
          if (edge_mode != 0)
          {
            if (s.scratch_index != 0)
            {
              s.terrain_edge_y_adjust = s.terrain_offset_y;
            }
            else
            {
              s.terrain_edge_y_adjust = -s.terrain_offset_y;
            }

            if (s.scratch_index != 0)
            {
              s.terrain_edge_x_adjust = -s.terrain_offset_x;
            }
            else
            {
              s.terrain_edge_x_adjust = s.terrain_offset_x;
            }
            DrawEncodedImageUnscaledClipped(PTR_DAT_005832b4,
                                            s.tile_screen_x + s.terrain_edge_x_adjust,
                                            (s.tile_base_screen_y - s.terrain_height) + s.terrain_edge_y_adjust,
                                            g_sland_sprite_entries[s.terrain_row][s.terrain_column]);
          }

          s.scratch_index = (s.scratch_index == 0);
          s.terrain_row = (s.terrain_pattern + s.world_tile_x) % 0xb + (((s.terrain_row < 0xb) - 1) & 0xb);
          s.second_terrain_sprite = g_land_sprite_entries[s.terrain_row][s.terrain_column];
          if (s.second_terrain_sprite->top_clip != 0xffff)
          {
            s.terrain_height = s.second_terrain_sprite->top_clip;
          }
          else
          {
            s.terrain_height = 0;
          }

          if (draw_mode != 0)
          {

            QueueWorldSpriteForDraw(PTR_DAT_005832b4,
                                    s.tile_screen_x - s.terrain_offset_x,
                                    (s.tile_base_screen_y - s.terrain_height) + s.terrain_offset_y,
                                    s.terrain_offset_y + s.tile_base_screen_y,
                                    g_land_sprite_entries[s.terrain_row][s.terrain_column]);
          }
          if (edge_mode != 0)
          {
            DrawEncodedImageUnscaledClipped(PTR_DAT_005832b4,
                                            s.tile_screen_x - s.terrain_offset_x,
                                            (s.tile_base_screen_y - s.terrain_height) + s.terrain_offset_y,
                                            g_sland_sprite_entries[s.terrain_row][s.terrain_column]);
          }
        }
      }

      if (s.tile_mask & 0x40)
      {
        s.i = FUN_00508b89(s.world_tile_x, s.world_tile_y);

        if ((s.i >= 0) &&
            (g_castle_dungeon_slots[s.i].clues_bitmap != 0) &&
            (g_castle_dungeon_slots[s.i].card_slot_1 != -1))
        {
          s.scratch_index = (s.i - 5) * 2;
          s.dungeon_marker_sprite = g_location_marker_sprite_entries[g_dungeon_marker_sprite_lookup[s.scratch_index]];

          if (draw_mode != 0)
          {
            QueueWorldSpriteForDraw(PTR_DAT_005832b4,
                                    s.tile_screen_x,
                                    (s.tile_base_screen_y - (int)s.dungeon_marker_sprite->height) + g_world_camera_minimap_panel_y,
                                    s.tile_base_screen_y,
                                    g_location_marker_sprite_entries[g_dungeon_marker_sprite_lookup[s.scratch_index]]);
          }
          if (edge_mode != 0)
          {
            DrawEncodedImageUnscaledClipped(PTR_DAT_005832b4,
                                            s.tile_screen_x,
                                            (s.tile_base_screen_y - (int)s.dungeon_marker_sprite->height) + g_world_camera_minimap_panel_y,
                                            g_location_marker_sprite_entries[g_dungeon_marker_sprite_lookup[s.scratch_index + 1]]);
          }
        }
      }

      if ((s.tile_mask & 0x10) != 0)
      {
        s.town_index = FUN_004bb040(s.world_tile_x, s.world_tile_y);
        if (g_town_slots[s.town_index].location_type == 4)
        {
          s.tile_class = single_color_test_bit_to_color_t((int)s.tile_class) - 1;
          if (draw_mode != 0)
          {
            QueueWorldSpriteForDraw(PTR_DAT_005832b4,
                                    s.tile_screen_x,
                                    s.tile_base_screen_y - ScaleUiCoordinate(0xa0),
                                    s.tile_base_screen_y,
                                    g_castles_sprite_entries[(char)g_castle_sprite_lookup_by_tile_class[s.tile_class * 4]]);
          }
          if (edge_mode != 0)
          {
            DrawEncodedImageUnscaledClipped(PTR_DAT_005832b4,
                                            s.tile_screen_x,
                                            s.tile_base_screen_y - ScaleUiCoordinate(0xa0),
                                            g_castles_sprite_entries[(char)g_castle_sprite_lookup_by_tile_class[s.tile_class * 4 + 1]]);
          }
        }
        else if (g_town_slots[s.town_index].location_type == 5)
        {
          s.tile_class = single_color_test_bit_to_color_t((int)s.tile_class) - 1;
          if (draw_mode != 0)
          {
            QueueWorldSpriteForDraw(PTR_DAT_005832b4,
                                    s.tile_screen_x,
                                    s.tile_base_screen_y - ScaleUiCoordinate(0xa0),
                                    s.tile_base_screen_y,
                                    g_castles_sprite_entries[(char)g_castle_sprite_lookup_by_tile_class[s.tile_class * 4 + 0x1a]]);
          }
          if (edge_mode != 0)
          {
            DrawEncodedImageUnscaledClipped(PTR_DAT_005832b4,
                                            s.tile_screen_x,
                                            s.tile_base_screen_y - ScaleUiCoordinate(0xa0),
                                            g_castles_sprite_entries[(char)g_castle_sprite_lookup_by_tile_class[s.tile_class * 4 + 0x1b]]);
          }
        }
        else if (g_town_slots[s.town_index].status_and_ruling_wizard & 0xff00)
        {
          s.wizard_index = (g_town_slots[s.town_index].status_and_ruling_wizard >> 8) - 1;
          if (draw_mode != 0)
          {
            QueueWorldSpriteForDraw(PTR_DAT_005832b4,
                                    s.tile_screen_x,
                                    s.tile_base_screen_y - ScaleUiCoordinate(0xa6),
                                    s.tile_base_screen_y,
                                    g_wizard_controlled_town_sprite_entries[s.wizard_index]);
          }
          if (edge_mode != 0)
          {
            DrawEncodedImageUnscaledClipped(PTR_DAT_005832b4,
                                            s.tile_screen_x,
                                            s.tile_base_screen_y - ScaleUiCoordinate(0xa6),
                                            DAT_00749418);
          }
        }
        else
        {
          if (g_town_slots[s.town_index].location_type == 1)
          {
            s.scratch_index = ((s.world_tile_x & 1) + 0x10) * 2;
          }
          else
          {
            s.scratch_index = (s.town_index % 0x10) * 2;
          }

          s.town_marker_sprite = g_location_marker_sprite_entries[(char)g_location_marker_sprite_lookup_by_town[s.scratch_index]];
          s.town_marker_height = s.town_marker_sprite->height;
          if (draw_mode != 0)
          {
            QueueWorldSpriteForDraw(PTR_DAT_005832b4,
                                    s.tile_screen_x,
                                    ScaleUiCoordinate(0x18) + (s.tile_base_screen_y - s.town_marker_height),
                                    s.tile_base_screen_y,
                                    g_location_marker_sprite_entries[(char)g_location_marker_sprite_lookup_by_town[s.scratch_index]]);
          }
          if (edge_mode != 0)
          {
            DrawEncodedImageUnscaledClipped(PTR_DAT_005832b4,
                                            s.tile_screen_x,
                                            ScaleUiCoordinate(0x18) + (s.tile_base_screen_y - s.town_marker_height),
                                            g_location_marker_sprite_entries[(char)g_location_marker_sprite_lookup_by_town[s.scratch_index + 1]]);
          }
        }

        if ((s.town_index == g_current_quest_destination) && (edge_mode != 0))
        {
          PTR_DAT_005832b4->font_slot = 1;
          if (g_town_slots[s.town_index].location_type > 1 || s.town_index == g_current_quest_destination)
          {
            if (g_town_slots[s.town_index].location_type == 4)
            {
              s.scratch_index = single_color_test_bit_to_color_t((int)s.tile_class);
              strcpy(g_ui_message_buffer, BuildQuestLocationName(s.town_index, g_current_quest_destination, s.scratch_index));
              g_castle_dungeon_slots[s.scratch_index - 1].clues_bitmap |= 1;
            }
            else
            {
              strcpy(g_ui_message_buffer, BuildQuestLocationName(s.town_index, g_current_quest_destination, 0));
            }

            DrawWorldUiFormattedText(PTR_DAT_005832b4, 0xff, s.tile_screen_x + g_world_camera_minimap_panel_x / 2,
                                     s.tile_base_screen_y + g_world_camera_minimap_panel_y / 2, g_ui_message_buffer);
          }
          else
          {
            strcpy(g_ui_message_buffer, BuildQuestLocationName(s.town_index, g_current_quest_destination, 0));
            DrawWorldUiFormattedText(PTR_DAT_005832b4, 0xff, s.tile_screen_x + g_world_camera_minimap_panel_x / 2,
                                     s.tile_base_screen_y + g_world_camera_minimap_panel_y / 2, g_ui_message_buffer);
          }
        }
      }
    }
  }

  WorldPointToScreen(g_world_player_world_x_cached, g_world_player_world_y_cached, &s.tile_screen_x, &s.tile_screen_y);

  PTR_DAT_005832b4->page_number = s.saved_page_number;
}

// FUNCTION: SHANDALAR 0x0054baaa
int DrawAdventureQuestStatusPanel(int force_redraw)
{
  struct
  {
    int siege_slot_index;
    int avatar_draw_y;
    EncodedImage *avatar_sprite_entry;
    int scaled_strip_x;
    int scaled_strip_height;
    int text_x;
    int day_sprite_index;
    int text_tail_length;
    int saved_page_number;
    EncodedImage *clocknew_sprite_entry;
    EncodedImage *questnew_sprite_entry;
  } s;

  s.questnew_sprite_entry = g_questnew_sprite_entries[1];
  s.clocknew_sprite_entry = g_clocknew_sprite_entries[0];
  s.saved_page_number = PTR_DAT_005832dc->page_number;
  PTR_DAT_005832dc->page_number = 1;
  s.day_sprite_index = (g_current_quest_deadline - g_quest_restock_timer) / 0x10 + 1;

  if (force_redraw != 0 &&
      ((g_current_quest_destination != -1) || (g_lair_or_monster_slots[7].entry_type != SHANDALAR_ENTRY_NONE) ||
       (g_next_duel_life_delta != 0) || (g_next_duel_card_id != -1)))
  {
    g_world_ui_cached_next_duel_card_id = g_next_duel_card_id;
    g_world_ui_cached_next_duel_life_delta = g_next_duel_life_delta;
    g_world_ui_cached_siege_entry_type = g_lair_or_monster_slots[7].entry_type;
    s.scaled_strip_height = ScaleUiCoordinate((int)s.questnew_sprite_entry->height) - ScaleUiCoordinate(0x13);

    if (g_skip_world_sfx_preload == 0)
    {
      s.scaled_strip_x = ScaleUiCoordinate(0x17c);
      BlitGraphicsRect(PTR_DAT_0058332c, (unsigned int)s.scaled_strip_x, 0,
                       (unsigned int)(ScaleUiCoordinate(0x280) - s.scaled_strip_x),
                       (DWORD)PTR_DAT_0058332c->max_y,
                       PTR_DAT_005832dc, s.scaled_strip_x, ScaleUiCoordinate(0x148) - ScaleUiCoordinate(0x135));

      s.scaled_strip_x = ScaleUiCoordinate(0x181);
      DrawEncodedImageResampled(PTR_DAT_005832dc,
                                s.scaled_strip_x,
                                0,
                                ScaleUiCoordinate((int)s.questnew_sprite_entry->width),
                                ScaleUiCoordinate((int)s.questnew_sprite_entry->height),
                                g_questnew_sprite_entries[1]);

      if (g_current_quest_destination != -1)
      {
        DrawEncodedImageResampled(PTR_DAT_005832dc,
                                  ScaleUiCoordinate(0x23a),
                                  ScaleUiCoordinate(0x15),
                                  ScaleUiCoordinate((int)s.clocknew_sprite_entry->width),
                                  ScaleUiCoordinate((int)s.clocknew_sprite_entry->height),
                                  g_clocknew_sprite_entries[g_monster_timer & 7]);

        DrawEncodedImageResampled(PTR_DAT_005832dc,
                                  ScaleUiCoordinate(0x23a),
                                  ScaleUiCoordinate(0x15),
                                  ScaleUiCoordinate((int)s.clocknew_sprite_entry->width),
                                  ScaleUiCoordinate((int)s.clocknew_sprite_entry->height),
                                  g_sunmoon_sprite_entries[(g_current_quest_deadline - g_quest_restock_timer) % 0xe]);

        DrawEncodedImageResampled(PTR_DAT_005832dc,
                                  ScaleUiCoordinate(0x23a),
                                  ScaleUiCoordinate(0x15),
                                  ScaleUiCoordinate((int)s.clocknew_sprite_entry->width),
                                  ScaleUiCoordinate((int)s.clocknew_sprite_entry->height),
                                  g_daysnew_sprite_entries[s.day_sprite_index]);
      }
    }

    s.avatar_draw_y = 0x5b;
    s.avatar_sprite_entry = g_world_magic_avatar_sprites[0];
    PTR_DAT_005832dc->font_slot = 4;

    s.avatar_draw_y -= (int)s.avatar_sprite_entry->height / 2;
    s.avatar_sprite_entry = g_world_magic_avatar_sprites[0];
    DrawEncodedImageUiScaled(PTR_DAT_005832dc,
                             0x193,
                             s.avatar_draw_y,
                             g_world_magic_avatar_sprites[0],
                             (int)s.avatar_sprite_entry->width,
                             (int)s.avatar_sprite_entry->height);
    DrawTextAt(PTR_DAT_005832dc, g_world_ui_stats_color_index, 0x1a9, s.avatar_draw_y + (int)s.avatar_sprite_entry->height / 2, "%d",
               g_amulet_inventory[4]);

    s.scaled_strip_x = ScaleUiCoordinate(0x17c);
    s.text_x = 0x200;
    s.text_tail_length = 0x33;
    PTR_DAT_005832dc->font_slot = 2;
    strcpy(g_ui_message_buffer, "");
    if (g_lair_or_monster_slots[7].entry_type != SHANDALAR_ENTRY_NONE)
    {
      s.siege_slot_index = 7;
      FormatMessageFromStringStripCarriageReturns(
          g_ui_message_buffer,
          0x1000,
          gs_queststatus_0077e0a0[0],
          GetCreatureName(g_lair_or_monster_slots[s.siege_slot_index].entry_type),
          BuildTownDisplayName(FindNearestTownIndex(
              g_lair_or_monster_slots[s.siege_slot_index].world_x / 0x20,
              g_lair_or_monster_slots[s.siege_slot_index].world_y / 0x20)));
    }
    else
    {
      g_siege_indicator = 0;
    }

    if ((g_next_duel_life_delta != 0) || (g_next_duel_card_id != -1))
    {
      strcat(g_ui_message_buffer, gs_queststatus_0077e0a0[1]);
      if (g_next_duel_life_delta != 0)
      {
        if (g_next_duel_life_delta >= 0)
        {
          sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), gs_queststatus_0077e0a0[2], g_next_duel_life_delta);
        }
        else
        {
          sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), gs_queststatus_0077e0a0[3], abs(g_next_duel_life_delta));
        }
        DrawEncodedImageResampled(PTR_DAT_005832b4,
                                  ScaleUiCoordinate(0x254),
                                  ScaleUiCoordinate(200),
                                  ScaleUiCoordinate(0x1c),
                                  ScaleUiCoordinate(0x25),
                                  g_worlds_extra_sprite_entries[3]);
      }

      if (g_next_duel_card_id == 0)
      {
        strcat(g_ui_message_buffer, gs_queststatus_0077e0a0[4]);
        DrawEncodedImageResampled(PTR_DAT_005832b4,
                                  ScaleUiCoordinate(0x254),
                                  ScaleUiCoordinate(200),
                                  ScaleUiCoordinate(0x1c),
                                  ScaleUiCoordinate(0x25),
                                  g_worlds_extra_sprite_entries[1]);
      }
      else if (g_next_duel_card_id > 0 && g_next_duel_card_id <= 5)
      {
        sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), gs_queststatus_0077e0a0[2], g_next_duel_card_id);
        DrawEncodedImageResampled(PTR_DAT_005832b4,
                                  ScaleUiCoordinate(0x254),
                                  ScaleUiCoordinate(200),
                                  ScaleUiCoordinate(0x1c),
                                  ScaleUiCoordinate(0x25),
                                  g_worlds_extra_sprite_entries[2]);
      }
      else if (g_next_duel_card_id > 5)
      {
        strcat(g_ui_message_buffer, global_cards_data[g_next_duel_card_id].name);
        strcat(g_ui_message_buffer, " ");
        DrawEncodedImageResampled(PTR_DAT_005832b4,
                                  ScaleUiCoordinate(0x254),
                                  ScaleUiCoordinate(0x90),
                                  ScaleUiCoordinate(0x1c),
                                  ScaleUiCoordinate(0x25),
                                  g_worlds_extra_sprite_entries[0]);
      }
      strcat(g_ui_message_buffer, "\n\n");
    }

    if (g_current_quest_destination != -1)
    {
      if (g_current_quest_type != 0)
      {
        if (g_town_slots[g_current_quest_destination].location_type == 1)
        {
          strcat(g_ui_message_buffer, gs_queststatus_0077e0a0[g_current_quest_color + 4]);
        }
        else
        {
          strcat(g_ui_message_buffer, FUN_004f2e17(g_current_quest_destination));
        }
        strcat(g_ui_message_buffer, "\n");
      }
      else
      {
        strcat(g_ui_message_buffer, gs_queststatus_0077e0a0[10]);
      }

      if ((((g_current_quest_type == 0) || (g_current_quest_type == 2)) ||
           ((g_current_quest_type == 1) &&
            (FindDeckSlotForQuestColorAndType(g_current_quest_color, 1 << (g_current_quest_destination & 3)) != 0))) ||
          (g_current_quest_type < -100))
      {
        s.day_sprite_index = GetRelativeWorldQuadrant(g_town_slots[g_current_quest_destination].world_x,
                                                      g_town_slots[g_current_quest_destination].world_y);
        if (g_current_quest_data != s.day_sprite_index)
        {
          g_current_quest_data = -1;
        }

        switch (g_current_quest_data)
        {
        case -1:
          if (g_current_quest_type < 0)
          {
            sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), gs_queststatus_0077e0a0[0xb], BuildTownDisplayName(g_current_quest_destination));
          }
          else if ((g_current_quest_type == 0) || (g_current_quest_type == 2))
          {
            sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), gs_queststatus_0077e0a0[0xc], BuildTownDisplayName(g_current_quest_destination));
          }
          else
          {
            sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), gs_queststatus_0077e0a0[0xd], BuildTownDisplayName(g_current_quest_destination));
          }
          break;
        case 0:
          sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), gs_queststatus_0077e0a0[0xe], BuildTownDisplayName(g_current_quest_destination));
          break;
        case 1:
          sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), gs_queststatus_0077e0a0[0xf], BuildTownDisplayName(g_current_quest_destination));
          break;
        case 2:
          sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), gs_queststatus_0077e0a0[0x10], BuildTownDisplayName(g_current_quest_destination));
          break;
        case 3:
          sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), gs_queststatus_0077e0a0[0x11], BuildTownDisplayName(g_current_quest_destination));
          break;
        }

        DrawTextAt(PTR_DAT_005832dc, 0xff, s.text_x, 0x41, g_ui_message_buffer);

        BlitGraphicsRect(PTR_DAT_005832dc, (unsigned int)s.scaled_strip_x,
                         ScaleUiCoordinate(0x148) - ScaleUiCoordinate(0x135),
                         (unsigned int)(ScaleUiCoordinate(0x280) - s.scaled_strip_x),
                         (DWORD)s.scaled_strip_height,
                         PTR_DAT_005832b4, s.scaled_strip_x, ScaleUiCoordinate(0x148));

        goto check_quest_deadline;
      }
      if (g_current_quest_type == 1)
      {
        FormatMessageFromStringStripCarriageReturns(g_ui_message_buffer, 0x1000, gs_queststatus_0077e0a0[0x12],
                                                    gs_queststatus_0077e0a0[g_current_quest_color + 0x12],
                                                    GetQuestCardClassName(1 << (g_current_quest_destination & 3)));
      }
      if (g_current_quest_type < 0)
      {
        sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), gs_queststatus_0077e0a0[0x18], BuildCreatureNameWithArticle(-g_current_quest_type));
      }
    }

    DrawTextAt(PTR_DAT_005832dc, 0xff, s.text_x, 0x41, g_ui_message_buffer);

    BlitGraphicsRect(PTR_DAT_005832dc, (unsigned int)s.scaled_strip_x,
                     ScaleUiCoordinate(0x148) - ScaleUiCoordinate(0x135),
                     (unsigned int)(ScaleUiCoordinate(0x280) - s.scaled_strip_x),
                     (DWORD)s.scaled_strip_height,
                     PTR_DAT_005832b4, s.scaled_strip_x, ScaleUiCoordinate(0x148));

  check_quest_deadline:
    if ((g_current_quest_destination != -1) && (g_current_quest_deadline <= g_quest_restock_timer))
    {
      AddJournalEntry(JOURNAL_ENTRY_QUEST_FAILED, g_current_quest_type);
      sprintf(g_ui_message_buffer, gs_queststatus_0077e0a0[0x19], BuildTownDisplayName(g_current_quest_giver_town_index));
      PTR_DAT_005832b4->font_slot = 4;
      RunTextMenuAtScaled(g_ui_message_buffer, 0x5a, 0x50);
      g_town_slots[g_current_quest_giver_town_index].status_and_ruling_wizard |= 4;
      g_current_quest_destination = -1;
      RefreshAdventureInterfaceLayout();
      DrawAdventureQuestStatusPanel(1);
    }
  }
  else if (force_redraw != 0)
  {
    DrawEncodedImageUiScaled(PTR_DAT_005832b4, 0x181, 0x135, g_questnew_sprite_entries[0], 0xed, 0x74);
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
    s.questnew_sprite_entry = g_questnew_sprite_entries[1];
    s.clocknew_sprite_entry = g_clocknew_sprite_entries[0];
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
void *DrawAdventureInterfaceLayout(int force_redraw)
{
  struct
  {
    int avatar_draw_y;
    EncodedImage *avatar_sprite_entry;
    int world_magic_score;
    int world_magic_icon_y;
    int world_magic_slot_index;
    int avatar_x;
  } s;

  PTR_DAT_005832b4->font_slot = 4;
  PTR_DAT_005832dc->font_slot = 4;

  BlitGraphicsRect(PTR_DAT_0058332c, ScaleUiCoordinate(0x58), ScaleUiCoordinate(0x15b) - ScaleUiCoordinate(0x148),
                   (unsigned int)ScaleUiCoordinate(0x126), ScaleUiCoordinate(0x15b) - ScaleUiCoordinate(0x148),
                   PTR_DAT_005832dc, 0, 0);

  DrawFormattedTextShadowedCentered(PTR_DAT_005832dc, g_world_ui_stats_color_index, ScaleUiCoordinate(0x12), ScaleUiCoordinate(10), "%d", Gold);
  DrawFormattedTextShadowedCentered(PTR_DAT_005832dc, g_world_ui_stats_color_index, ScaleUiCoordinate(0x60), ScaleUiCoordinate(10), "%d", g_food);
  DrawFormattedTextShadowedCentered(PTR_DAT_005832dc, g_world_ui_stats_color_index, ScaleUiCoordinate(0xab), ScaleUiCoordinate(10), "%d", CountDuelPoolEligibleTowns());
  DrawFormattedTextShadowedCentered(PTR_DAT_005832dc, g_world_ui_stats_color_index, ScaleUiCoordinate(0x10c), ScaleUiCoordinate(10), "%d/%d", g_deck_active_card_count, g_deck_total_card_count);

  BlitGraphicsRect(PTR_DAT_005832dc, 0, 0, (unsigned int)ScaleUiCoordinate(0x126), ScaleUiCoordinate(0x15b) - ScaleUiCoordinate(0x148),
                   PTR_DAT_005832b4, ScaleUiCoordinate(0x58), ScaleUiCoordinate(0x15b));

  PTR_DAT_005832b4->font_slot = 2;
  DrawAdventureQuestStatusPanel(1);

  if ((g_adventure_ui_layout_dirty == 0) && (force_redraw == 0))
  {
    FUN_0046ed03();
    return (void *)0;
  }

  s.avatar_x = 0x48;
  s.avatar_draw_y = 400;
  s.avatar_sprite_entry = g_world_magic_avatar_sprites[0];
  PTR_DAT_005832b4->font_slot = 4;
  s.avatar_draw_y -= (int)s.avatar_sprite_entry->height / 2;

  DrawEncodedImageUiScaled(PTR_DAT_005832b4, 0x4e, s.avatar_draw_y, g_world_magic_avatar_sprites[2],
                           (int)s.avatar_sprite_entry->width,
                           (int)s.avatar_sprite_entry->height);
  DrawTextAt(PTR_DAT_005832b4, g_world_ui_stats_color_index, 100, s.avatar_draw_y + (int)s.avatar_sprite_entry->height / 2, "%d",
             g_amulet_inventory[0]);

  DrawEncodedImageUiScaled(PTR_DAT_005832b4, 0xa1, s.avatar_draw_y, g_world_magic_avatar_sprites[1],
                           (int)s.avatar_sprite_entry->width,
                           (int)s.avatar_sprite_entry->height);
  DrawTextAt(PTR_DAT_005832b4, g_world_ui_stats_color_index, 0xb7, s.avatar_draw_y + (int)s.avatar_sprite_entry->height / 2, "%d",
             g_amulet_inventory[1]);

  DrawEncodedImageUiScaled(PTR_DAT_005832b4, 0xf0, s.avatar_draw_y, g_world_magic_avatar_sprites[4],
                           (int)s.avatar_sprite_entry->width,
                           (int)s.avatar_sprite_entry->height);
  DrawTextAt(PTR_DAT_005832b4, g_world_ui_stats_color_index, 0x106, s.avatar_draw_y + (int)s.avatar_sprite_entry->height / 2, "%d",
             g_amulet_inventory[2]);

  DrawEncodedImageUiScaled(PTR_DAT_005832b4, 0x140, s.avatar_draw_y, g_world_magic_avatar_sprites[3],
                           (int)s.avatar_sprite_entry->width,
                           (int)s.avatar_sprite_entry->height);
  DrawTextAt(PTR_DAT_005832b4, g_world_ui_stats_color_index, 0x156, s.avatar_draw_y + (int)s.avatar_sprite_entry->height / 2, "%d",
             g_amulet_inventory[3]);

  DrawEncodedImageUiScaled(PTR_DAT_005832b4, 0x193, s.avatar_draw_y, g_world_magic_avatar_sprites[0],
                           (int)s.avatar_sprite_entry->width,
                           (int)s.avatar_sprite_entry->height);
  DrawTextAt(PTR_DAT_005832b4, g_world_ui_stats_color_index, 0x1a9, s.avatar_draw_y + (int)s.avatar_sprite_entry->height / 2, "%d",
             g_amulet_inventory[4]);

  s.avatar_x = 0x10;
  s.world_magic_icon_y = 0x96;
  for (s.world_magic_slot_index = 0; s.world_magic_slot_index < 0xc; s.world_magic_slot_index++)
  {
    if ((g_world_magic_bitmap & (1 << (unsigned char)s.world_magic_slot_index)) != 0)
    {
      s.world_magic_score = FUN_004bb458(s.world_magic_slot_index);
      if ((s.world_magic_slot_index >= 2) && ((s.world_magic_slot_index & 1) == 0))
      {
        if (g_amulet_inventory[s.world_magic_slot_index / 2 - 1] != 0)
        {
          DrawEncodedImageUiScaled(PTR_DAT_005832b4,
                                   g_adventure_interface_world_magic_icon_rects[s.world_magic_slot_index].x,
                                   g_adventure_interface_world_magic_icon_rects[s.world_magic_slot_index].y,
                                   g_world_magic_choice_button_sprite_bank.named.normal[s.world_magic_slot_index],
                                   g_adventure_interface_world_magic_icon_rects[s.world_magic_slot_index].width,
                                   g_adventure_interface_world_magic_icon_rects[s.world_magic_slot_index].height);
          DrawEncodedImageUiScaled(PTR_DAT_005832b4,
                                   g_adventure_interface_world_magic_icon_rects[s.world_magic_slot_index].x,
                                   g_adventure_interface_world_magic_icon_rects[s.world_magic_slot_index].y,
                                   g_world_magic_choice_button_sprite_bank.named.icon[s.world_magic_slot_index],
                                   g_adventure_interface_world_magic_icon_rects[s.world_magic_slot_index].width,
                                   g_adventure_interface_world_magic_icon_rects[s.world_magic_slot_index].height);
        }
        else
        {
          DrawEncodedImageUiScaled(PTR_DAT_005832b4,
                                   g_adventure_interface_world_magic_icon_rects[s.world_magic_slot_index].x,
                                   g_adventure_interface_world_magic_icon_rects[s.world_magic_slot_index].y,
                                   g_world_magic_choice_button_sprite_bank.named.pressed[s.world_magic_slot_index],
                                   g_adventure_interface_world_magic_icon_rects[s.world_magic_slot_index].width,
                                   g_adventure_interface_world_magic_icon_rects[s.world_magic_slot_index].height);
        }
      }
      else
      {
        DrawEncodedImageUiScaled(PTR_DAT_005832b4,
                                 g_adventure_interface_world_magic_icon_rects[s.world_magic_slot_index].x,
                                 g_adventure_interface_world_magic_icon_rects[s.world_magic_slot_index].y,
                                 g_world_magic_choice_button_sprite_bank.named.normal[s.world_magic_slot_index],
                                 g_adventure_interface_world_magic_icon_rects[s.world_magic_slot_index].width,
                                 g_adventure_interface_world_magic_icon_rects[s.world_magic_slot_index].height);
      }
    }
  }

  PTR_DAT_005832b4->font_slot = 1;
}

// FUNCTION: SHANDALAR 0x0054a880
void ResetWorldDrawQueue(void)
{
  g_world_draw_entry_count = 0;
  g_world_draw_head_index = 0;
}

// FUNCTION: SHANDALAR 0x0054a89f
void QueueWorldSpriteForDraw(FacemakerWindowBounds *window, int draw_x, int draw_y, int depth_y, EncodedImage *sprite)
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
void DrawQueuedWorldSprites(void)
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
void WorldSubtileOffsetToScreen(int world_subcell_x, int world_subcell_y, int *out_screen_x, int *out_screen_y)
{
  struct
  {
    int world_subcell_y;
    int screen_x;
    int world_subcell_x;
    int screen_y;
  } s;

  s.world_subcell_x = world_subcell_x;
  s.world_subcell_y = world_subcell_y;
  s.screen_x = ((s.world_subcell_y + s.world_subcell_x) * global_screen_width * 2) / 0x280;
  s.screen_y = ((s.world_subcell_y - s.world_subcell_x) * global_screen_height) / 0x1e0;
  *out_screen_x = s.screen_x;
  *out_screen_y = s.screen_y;
}

// FUNCTION: SHANDALAR 0x0054aa60
void WorldTileToScreenFromCameraAnchor(int world_tile_x, int world_tile_y, int *out_screen_x, int *out_screen_y)
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
void WorldPointToScreen(int world_x, int world_y, int *out_screen_x, int *out_screen_y)
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
void ScreenToWorldPoint(int screen_x, int screen_y, int *out_world_x, int *out_world_y)
{
  struct
  {
    int world_delta_y;
    int world_delta_x;
  } s;

  s.world_delta_x = screen_x - g_world_camera_viewport_width_pixels / 2;
  s.world_delta_y = (screen_y - g_world_ui_top_offset) - g_world_camera_viewport_height_pixels / 2;
  s.world_delta_x = (s.world_delta_x * 0x280) / global_screen_width;
  s.world_delta_y = (s.world_delta_y * 0x1e0) / global_screen_height;
  *out_world_x = g_world_player_world_x_cached + ((s.world_delta_x >> 1) - s.world_delta_y) / 2;
  *out_world_y = g_world_player_world_y_cached + ((s.world_delta_x >> 1) + s.world_delta_y) / 2;
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
  struct
  {
    int scratch_padding[4];
    AdvMenuRect restore_clip_rect_scratch;
    AdvMenuRect minimap_clip_rect_scratch;
    AdvMenuRect ui_clip_rect_scratch;
    AdvMenuRect world_clip_rect_scratch;
    int scaled_0x20;
    int scaled_0x8c;
    int status_layout_x[6];
    int status_layout_y[3];
    int resolution_index;
    int status_layout_panel_y[3];
    int status_frame_y[3];
    int scaled_0x100;
    int monster_tile_screen_x;
    int monster_subcell_screen_y;
    int monster_subcell_screen_x;
    int monster_tile_screen_y;
    int player_screen_y;
    int player_screen_x;
    int heading_sprite_index;
    int slot_index;
    int tile_y;
    ShandalarEntryType current_entry_type;
    int tile_x_text;
    int previous_page_number;
    AdvMenuRect saved_clip_rect_world;
    AdvMenuRect saved_clip_rect_ui;
  } s;

  s.previous_page_number = PTR_DAT_005832dc->page_number;
  FUN_0046ed33();

  ConfigureAdventureWorldViewport();

  g_world_player_world_x_cached = world_x;
  g_world_player_world_y_cached = world_y;

  g_world_player_tile_x = world_x / 32;
  g_world_player_tile_y = world_y / 32;

  PTR_DAT_005832b4->page_number = PTR_DAT_005832dc->page_number;
  ResetWorldDrawQueue();
  g_world_ui_top_offset = 0x50;
  s.saved_clip_rect_world =
      *PushGraphicsClipRect(&s.world_clip_rect_scratch, PTR_DAT_005832dc, 0, 0x80, PTR_DAT_005832b4->max_x, PTR_DAT_005832b4->max_y - 0x80);
  s.saved_clip_rect_ui =
      *PushGraphicsClipRect(&s.ui_clip_rect_scratch, PTR_DAT_005832b4, 0, 0x80, PTR_DAT_005832b4->max_x, PTR_DAT_005832b4->max_y - 0x80);
  UpdateWorldViewportBuffer(world_x, world_y);

  WorldPointToScreen(world_x, world_y, &s.player_screen_x, &s.player_screen_y);
  PushGraphicsClipRect(&s.minimap_clip_rect_scratch, PTR_DAT_005832b4, 0x40,
                       g_world_camera_minimap_panel_y * 2 + g_world_ui_top_offset + 0x10,
                       ScaleUiCoordinateFrom320(0x100), ScaleUiCoordinateFrom320(0x8c));

  QueueWorldSpriteForDraw(PTR_DAT_005832b4,
                          (s.player_screen_x - g_ego_sprite_width / 2) + g_world_scroll_x,
                          (s.player_screen_y - g_ego_sprite_draw_height) + g_world_scroll_y,
                          s.player_screen_y + g_world_scroll_y,
                          (&g_opening_menu_sprite_work_buffer[0x10].sprites[((g_world_player_animation_direction + 2) & 7) * 5])
                              [g_world_player_animation_frame]);

  DrawEncodedImageUnscaledClipped(PTR_DAT_005832b4,
                                  s.player_screen_x - g_sego_sprite_width / 2,
                                  s.player_screen_y - g_sego_sprite_draw_height,
                                  (&g_opening_menu_sprite_work_buffer[0x11].sprites[((g_world_player_animation_direction + 2) & 7) * 5])
                                      [g_world_player_animation_frame]);
  strcpy(g_ui_message_buffer, "");
  strcat(g_ui_message_buffer, _itoa(s.tile_x_text, g_itoa_buffer, 10));
  strcat(g_ui_message_buffer, " ");
  strcat(g_ui_message_buffer, _itoa(s.tile_y, g_itoa_buffer, 10));

  for (s.slot_index = 0; s.slot_index < 8; s.slot_index = s.slot_index + 1)
  {
    WorldTileToScreenFromCameraAnchor(g_lair_or_monster_slots[s.slot_index].world_x & 0xffffffe0, g_lair_or_monster_slots[s.slot_index].world_y & 0xffffffe0,
                                      &s.monster_tile_screen_x, &s.monster_tile_screen_y);
    WorldSubtileOffsetToScreen(g_lair_or_monster_slots[s.slot_index].world_x & 0x1f, g_lair_or_monster_slots[s.slot_index].world_y & 0x1f,
                               &s.monster_subcell_screen_x, &s.monster_subcell_screen_y);
    s.player_screen_x = s.monster_subcell_screen_x + s.monster_tile_screen_x;
    s.player_screen_y = s.monster_tile_screen_y + s.monster_subcell_screen_y;
    s.monster_tile_screen_x = g_lair_or_monster_slots[s.slot_index].world_x / 32;
    s.monster_tile_screen_y = g_lair_or_monster_slots[s.slot_index].world_y / 32;
    if ((g_lair_or_monster_slots[s.slot_index].entry_type != SHANDALAR_ENTRY_NONE))
    {
      if ((abs(s.monster_tile_screen_x - g_world_player_tile_x) > 1) ||
          (abs(s.monster_tile_screen_y - g_world_player_tile_y) > 1))
      {
        continue;
      }

      s.current_entry_type = g_lair_or_monster_slots[s.slot_index].entry_type;
      if ((g_shandalar_monster_definitions[s.current_entry_type].flags_0a & 2U) != 0)
      {
        s.current_entry_type -= (ShandalarEntryType)(((unsigned int)g_monster_timer >> 5) & 3);
      }
      if (((g_shandalar_monster_definitions[s.current_entry_type].flags_0a & 0x100U) != 0) &&
          (ApproximateDistance(s.player_screen_x - 0x140, s.player_screen_y - 0xf0) > ScaleUiCoordinateFrom320(0x80)))
      {
        continue;
      }

      if (g_lair_or_monster_slots[s.slot_index].entry_type == SHANDALAR_ENTRY_LAIR)
      {
        QueueWorldSpriteForDraw(PTR_DAT_005832b4,
                                s.player_screen_x - ScaleUiCoordinateFrom320(0x20),
                                s.player_screen_y - ScaleUiCoordinateFrom320(0x30),
                                s.player_screen_y,
                                g_location07_sprite_entries[g_lair_sprite_index_by_color[g_lair_or_monster_slots[s.slot_index].color]]);

        DrawEncodedImageUnscaledClipped(PTR_DAT_005832b4,
                                        (s.player_screen_x - ScaleUiCoordinateFrom320(0x20)) - g_world_scroll_x,
                                        (s.player_screen_y - ScaleUiCoordinateFrom320(0x30)) - g_world_scroll_y,
                                        g_location07_sprite_entries[g_lair_sprite_index_by_color[g_lair_or_monster_slots[s.slot_index].color] + 6]);
      }
      else
      {
        s.heading_sprite_index = (signed char)g_shandalar_monster_definitions[s.current_entry_type].encounter_type;
        if (g_skip_world_sfx_preload != 0)
        {
          s.heading_sprite_index = 1;
        }
        s.heading_sprite_index = s.slot_index;
        QueueWorldSpriteForDraw(PTR_DAT_005832b4,
                                s.player_screen_x - g_world_lair_monster_sprite_widths[s.heading_sprite_index] / 2,
                                s.player_screen_y - g_world_lair_monster_sprite_top_clips[s.heading_sprite_index],
                                s.player_screen_y,
                                (&g_opening_menu_sprite_work_buffer[s.heading_sprite_index]
                                      .sprites[((g_lair_or_monster_slots[s.slot_index].movement_heading + 2) & 7) * 5])
                                    [g_lair_or_monster_slots[s.slot_index].movement_anim_frame]);

        s.heading_sprite_index = s.slot_index + 8;
        DrawEncodedImageUnscaledClipped(PTR_DAT_005832b4,
                                        (s.player_screen_x - g_world_lair_monster_sprite_widths[s.heading_sprite_index] / 2) - g_world_scroll_x,
                                        (s.player_screen_y - g_world_lair_monster_sprite_top_clips[s.heading_sprite_index]) - g_world_scroll_y,
                                        (&g_opening_menu_sprite_work_buffer[s.heading_sprite_index]
                                              .sprites[((g_lair_or_monster_slots[s.slot_index].movement_heading + 2) & 7) * 5])
                                            [g_lair_or_monster_slots[s.slot_index].movement_anim_frame]);
      }

      if ((((g_monster_timer >> 3) & 3) == (s.slot_index & 3)))
      {
        PTR_DAT_005832b4->font_slot = 3;
        strcpy(g_ui_message_buffer, GetCreatureName(s.current_entry_type));
      }
    }
  }

  DrawQueuedWorldSprites();
  PTR_DAT_005832b4->page_number = 0;
  s.scaled_0x100 = (int)g_questnew_sprite_entries[0];
  s.status_layout_y[0] = 0x40;
  s.status_layout_y[1] = 0x50;
  s.status_layout_y[2] = 0x66;
  s.status_frame_y[0] = 0x48;
  s.status_frame_y[1] = 0x5a;
  s.status_frame_y[2] = 0x72;
  s.status_layout_x[0] = 0xcb;
  s.status_layout_x[1] = 0xfd;
  s.status_layout_x[2] = 0x143;
  s.status_layout_x[3] = 0x1af;
  s.status_layout_x[4] = 0x21a;
  s.status_layout_x[5] = 0x2b0;
  s.status_layout_panel_y[0] = 0x2f;
  s.status_layout_panel_y[1] = 0x3c;
  s.status_layout_panel_y[2] = 0x4d;
  DrawEncodedImageResampled(PTR_DAT_005832dc,
                            ScaleUiCoordinate(0x141) + 0x40,
                            ScaleUiCoordinate(0x105) + g_world_camera_minimap_panel_y * 2 + g_world_ui_top_offset + 0x10,
                            ScaleUiCoordinate((int)((EncodedImage *)s.scaled_0x100)->width),
                            ScaleUiCoordinate((int)((EncodedImage *)s.scaled_0x100)->height),
                            g_questnew_sprite_entries[0]);
  switch (global_screen_width)
  {
  case 0x280:
    s.resolution_index = 0;
    break;

  case 0x320:
    s.resolution_index = 1;
    break;

  case 0x400:
    s.resolution_index = 2;
    break;
  }
  DrawEncodedImageUnscaled(PTR_DAT_005832dc, 0x40,
                           (s.status_frame_y[s.resolution_index] + g_world_camera_minimap_panel_y * 2 + g_world_ui_top_offset + 0x10) -
                               ScaleUiCoordinate(0x30),
                           g_tips_frame_sprite);
  DrawEncodedImageUnscaled(PTR_DAT_005832dc,
                           (s.status_layout_x[s.resolution_index] + 0x40) - ScaleUiCoordinate(0x40),
                           g_world_camera_minimap_panel_y * 2 + g_world_ui_top_offset + 0x10,
                           g_tips_icon_sprite);
  DrawEncodedImageUnscaled(PTR_DAT_005832dc,
                           (s.status_layout_x[s.resolution_index + 3] + 0x40) - ScaleUiCoordinate(0x40),
                           g_world_camera_minimap_panel_y * 2 + g_world_ui_top_offset + 0x10,
                           g_tips_icon_sprite);
  if (g_world_scene_reveal_effect_pending == 0)
  {
    if (global_screen_width != 0x400)
    {
      BlitGraphicsRect(PTR_DAT_005832dc, 0x40, g_world_camera_minimap_panel_y * 2 + g_world_ui_top_offset + 0x10,
                       ScaleUiCoordinateFrom320(0x100), ScaleUiCoordinateFrom320(0x8c), PTR_DAT_005832b4,
                       ScaleUiCoordinateFrom320(0x20) & 0xfffffffc, ScaleUiCoordinateFrom320(0x18));
    }
    else
    {
      BlitGraphicsRect(PTR_DAT_005832dc, 0x40, g_world_camera_minimap_panel_y * 2 + g_world_ui_top_offset + 0x10,
                       ScaleUiCoordinateFrom320(0x100) - 2, ScaleUiCoordinateFrom320(0x8c), PTR_DAT_005832b4,
                       (ScaleUiCoordinateFrom320(0x20) + 4U) & 0xfffffffc, ScaleUiCoordinateFrom320(0x18));
    }
  }
  else
  {
    s.scaled_0x8c = (int)g_graphics_pages[PTR_DAT_005832dc->page_number];
    s.scaled_0x20 = (int)g_graphics_pages[PTR_DAT_005832b4->page_number];
    if (global_screen_width != 0x400)
    {
      BlitRectByRandomTileOrder(((DIBSurface *)s.scaled_0x20)->hTempDC,
                                ScaleUiCoordinateFrom320(0x20) & 0xfffffffc, ScaleUiCoordinateFrom320(0x18),
                                ScaleUiCoordinateFrom320(0x100), ScaleUiCoordinateFrom320(0x8c), 6, 6,
                                ((DIBSurface *)s.scaled_0x8c)->hTempDC,
                                0x40, g_world_camera_minimap_panel_y * 2 + g_world_ui_top_offset + 0x10);
      DelayUiTicks(0x2d);
    }
    else
    {
      BlitRectByRandomTileOrder(((DIBSurface *)s.scaled_0x20)->hTempDC,
                                (ScaleUiCoordinateFrom320(0x20) + 4U) & 0xfffffffc, ScaleUiCoordinateFrom320(0x18),
                                ScaleUiCoordinateFrom320(0x100) - 2, ScaleUiCoordinateFrom320(0x8c), 8, 8,
                                ((DIBSurface *)s.scaled_0x8c)->hTempDC,
                                0x40, g_world_camera_minimap_panel_y * 2 + g_world_ui_top_offset + 0x10);
      DelayUiTicks(0x2d);
    }
    g_world_scene_reveal_effect_pending = 0;
  }

  PushGraphicsClipRect(&s.restore_clip_rect_scratch, PTR_DAT_005832dc, s.saved_clip_rect_world.x, s.saved_clip_rect_world.y,
                       s.saved_clip_rect_world.width, s.saved_clip_rect_world.height);
  PushGraphicsClipRect((AdvMenuRect *)s.scratch_padding, PTR_DAT_005832b4, s.saved_clip_rect_ui.x, s.saved_clip_rect_ui.y, s.saved_clip_rect_ui.width,
                       s.saved_clip_rect_ui.height);
  PTR_DAT_005832dc->page_number = s.previous_page_number;

  if ((world_state == 0) && (g_adventure_ui_layout_dirty == 0))
  {
    PTR_DAT_005832b4->font_slot = 1;
    FUN_0046ed03();
    DrawAdventureQuestStatusPanel(0);
    FUN_0054b80d();
    return;
  }
  else
  {
    DrawAdventureInterfaceLayout(0);
    g_adventure_ui_layout_dirty = 0;
    FUN_0046ed03();
  }
}
