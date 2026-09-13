#include <windows.h>
#include <mmsystem.h>
#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <time.h>
#include <errno.h>

#include "defs.h"
#include "shandalar.h"
#include "shandalar_internal.h"
#include "magic/src/global_duel_ui_ids.h"
#include "magic/src/global_state.h"
#include "magic/src/global_strings.h"
#include "magic/src/global_other.h"
#include "magic/src/game_support.h"
#include "magic/src/duel_engine.h"
#include "magic/src/shared_startup.h"
#include "shandalar_global_strings.h"
#include "cardartlib/src/assert.h"
#include "cardartlib/src/palette.h"
#include "deckdll/src/card_db.h"
#include "deckdll/src/magsnd.h"
#include "deckdll/src/shared_resources.h"
#include "facemaker/src/facemaker_types.h"
#include "drawcardlib/src/pic.h"

// FUNCTION: SHANDALAR 0x004be4c4
char *BuildResolutionSpritePath(char *sprite_filename)
{
  switch (global_screen_width)
  {
  case 0x280:
    strcpy(g_ui_message_buffer, "spr\\");
    break;
  case 800:
    strcpy(g_ui_message_buffer, "spr800\\");
    break;
  case 0x400:
    strcpy(g_ui_message_buffer, "spr1024\\");
    break;
  }
  strcat(g_ui_message_buffer, sprite_filename);
  return g_ui_message_buffer;
}

// FUNCTION: SHANDALAR 0x004bbc50
void LoadOpeningMenuSpriteResources(void)
{
  // GLOBAL: SHANDALAR 0x005892b0
  static int gender_map[] = {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0};

  struct
  {
    int iconb_entry_index;
    EncodedImage *iconb_entries[200];
    int dbox_entry_index;
    EncodedImage *dbox_entries[200];
    EncodedImage *sego_sprite_header_ptr; // ebp - 0x988
    EncodedImage *ego_sprite_header_ptr;  // ebp - 0x984
    int tsprite2_entry_index;             // ebp - 0x980
    EncodedImage *tsprite2_entries[200];  // ebp - 0x97c
    int ttsprite_entry_index;             // ebp - 0x65c
    int ttsprite_row_index;               // ebp - 0x658
    int ttsprite_col_index;               // ebp - 0x654
    EncodedImage *ttsprite_entries[200];  // ebp - 0x650
    int worlds_entry_index;               // ebp - 0x330
    EncodedImage *worlds_entries[200];    // ebp - 0x32c
    int location_block_start_index;       // ebp - 0xc
    int inner_index;                      // ebp - 0x8
    int entry_index;                      // ebp - 0x4
  } s;

  BeginSpriteEncodeSession();
  LoadPcxIntoPage(1, "endtop.pic");
  g_endtop_banner_sprite = EncodeSpriteFromPage(1, 0, 0, 0x95, 0x13);
  FinalizeSpriteEncodeSession();
  ReadSpriteEntryPointers(g_gsprite_sprite_entries, "gsprite.spr");
  ReadSpriteEntryPointers(g_questnew_sprite_entries, "questnew.spr");
  ReadSpriteEntryPointers(g_compnew_sprite_entries, "compnew.spr");
  s.worlds_entry_index = 0;
  ReadSpriteEntryPointers(s.worlds_entries, "worlds.spr");
  for (s.inner_index = 0; s.inner_index < 4; s.inner_index = s.inner_index + 1)
  {
    for (s.entry_index = 0; s.entry_index < 0xc; s.entry_index = s.entry_index + 1)
    {
      g_world_magic_choice_button_sprite_bank.by_group[s.inner_index][s.entry_index] = s.worlds_entries[s.worlds_entry_index];
      s.worlds_entry_index = s.worlds_entry_index + 1;
    }
  }
  for (s.entry_index = 0; s.entry_index < 4; s.entry_index = s.entry_index + 1)
  {
    g_main_menu_button_sprites_normal[s.entry_index] = s.worlds_entries[s.worlds_entry_index];
    s.worlds_entry_index = s.worlds_entry_index + 1;
    g_main_menu_button_sprites_highlight[s.entry_index] = s.worlds_entries[s.worlds_entry_index];
    s.worlds_entry_index = s.worlds_entry_index + 1;
  }
  for (s.entry_index = 0; s.entry_index < 4; s.entry_index = s.entry_index + 1)
  {
    g_worlds_extra_sprite_entries[s.entry_index] = s.worlds_entries[s.worlds_entry_index];
    s.worlds_entry_index = s.worlds_entry_index + 1;
  }
  ReadSpriteEntryPointers(g_asprite_sprite_entries, "asprite.spr");
  s.ttsprite_entry_index = 0;
  ReadSpriteEntryPointers(s.ttsprite_entries, "ttsprite.spr");
  for (s.ttsprite_row_index = 0; s.ttsprite_row_index < 3; s.ttsprite_row_index = s.ttsprite_row_index + 1)
  {
    for (s.ttsprite_col_index = 0; s.ttsprite_col_index < 0x10; s.ttsprite_col_index = s.ttsprite_col_index + 1)
    {
      g_ttsprite_grid_sprite_entries[s.ttsprite_row_index * 0x10 + s.ttsprite_col_index] = s.ttsprite_entries[s.ttsprite_entry_index];
      s.ttsprite_entry_index = s.ttsprite_entry_index + 1;
    }
  }
  for (s.ttsprite_col_index = 0; s.ttsprite_col_index < 6; s.ttsprite_col_index = s.ttsprite_col_index + 1)
  {
    g_ttsprite_aux_sprite_entries[s.ttsprite_col_index] = s.ttsprite_entries[s.ttsprite_entry_index];
    s.ttsprite_entry_index = s.ttsprite_entry_index + 1;
  }
  for (s.ttsprite_col_index = 0; s.ttsprite_col_index < 8; s.ttsprite_col_index = s.ttsprite_col_index + 1)
  {
    g_ttsprite_alt_sprite_entries[s.ttsprite_col_index] = s.ttsprite_entries[s.ttsprite_entry_index];
    s.ttsprite_entry_index = s.ttsprite_entry_index + 1;
  }
  for (s.ttsprite_col_index = 0; s.ttsprite_col_index < 10; s.ttsprite_col_index = s.ttsprite_col_index + 1)
  {
    g_ttsprite_extra_sprite_entries[s.ttsprite_col_index] = s.ttsprite_entries[s.ttsprite_entry_index];
    s.ttsprite_entry_index = s.ttsprite_entry_index + 1;
  }
  g_ttsprite_special_sprite_a = s.ttsprite_entries[s.ttsprite_entry_index];
  s.ttsprite_entry_index = s.ttsprite_entry_index + 1;
  g_ttsprite_special_sprite_b = s.ttsprite_entries[s.ttsprite_entry_index];
  s.ttsprite_entry_index = s.ttsprite_entry_index + 1;
  g_ttsprite_special_sprite_c = s.ttsprite_entries[s.ttsprite_entry_index];
  s.ttsprite_entry_index = s.ttsprite_entry_index + 1;
  ReadSpriteEntryPointers(g_world_magic_avatar_sprites, "amsprite.spr");

  s.entry_index = ReadSpriteEntryPointersWithLimit(&g_cstline1_sprite_entries[0][0], BuildResolutionSpritePath("cstline1.spr"), 0x54);
  s.entry_index = ReadSpriteEntryPointersWithLimit(g_land_tile_sprite_entries, BuildResolutionSpritePath("landtile.spr"), 0x10);
  s.entry_index = ReadSpriteEntryPointersWithLimit(&g_land_sprite_entries[0][0], BuildResolutionSpritePath("land.spr"), 55);
  s.entry_index = ReadSpriteEntryPointersWithLimit(&g_sland_sprite_entries[0][0], BuildResolutionSpritePath("sland.spr"), 55);
  s.entry_index = ReadSpriteEntryPointersWithLimit(&g_land_sprite_entries[11][0], BuildResolutionSpritePath("land2.spr"), 55);
  s.entry_index = ReadSpriteEntryPointersWithLimit(&g_sland_sprite_entries[11][0], BuildResolutionSpritePath("sland2.spr"), 55);
  s.entry_index = ReadSpriteEntryPointersWithLimit(g_road_sprite_entries, BuildResolutionSpritePath("roads.spr"), 0xc);

  s.entry_index = ReadSpriteEntryPointers(&g_location_marker_sprite_entries[0], BuildResolutionSpritePath("locatn01.spr"));
  s.entry_index += ReadSpriteEntryPointers(&g_location_marker_sprite_entries[s.entry_index], BuildResolutionSpritePath("locatn02.spr"));
  s.entry_index += ReadSpriteEntryPointers(&g_location_marker_sprite_entries[s.entry_index], BuildResolutionSpritePath("locatn03.spr"));
  s.location_block_start_index = s.entry_index;
  s.entry_index += ReadSpriteEntryPointers(&g_location_marker_sprite_entries[s.entry_index], BuildResolutionSpritePath("locatn04.spr"));

  g_wizard_controlled_town_sprite_entries[1] = g_location_marker_sprite_entries[s.location_block_start_index + 2];
  g_wizard_controlled_town_sprite_entries[4] = g_location_marker_sprite_entries[s.location_block_start_index + 3];
  g_wizard_controlled_town_sprite_entries[2] = g_location_marker_sprite_entries[s.location_block_start_index + 8];
  g_wizard_controlled_town_sprite_entries[0] = g_location_marker_sprite_entries[s.location_block_start_index + 10];
  g_current_location_marker_sprite = g_location_marker_sprite_entries[s.location_block_start_index + 6];
  s.entry_index += ReadSpriteEntryPointers(&g_location_marker_sprite_entries[s.entry_index], BuildResolutionSpritePath("locatn05.spr"));
  s.location_block_start_index = s.entry_index;
  s.entry_index += ReadSpriteEntryPointers(&g_location_marker_sprite_entries[s.entry_index], BuildResolutionSpritePath("locatn06.spr"));

  g_wizard_controlled_town_sprite_entries[3] = g_location_marker_sprite_entries[s.location_block_start_index];

  s.tsprite2_entry_index = 0;
  ReadSpriteEntryPointers(s.tsprite2_entries, "tsprite2.spr");
  for (s.entry_index = 0; s.entry_index < 6; s.entry_index = s.entry_index + 1)
  {
    for (s.inner_index = 0; s.inner_index < 2; s.inner_index = s.inner_index + 1)
    {
      g_tsprite2_grid_sprite_entries[s.entry_index * 2 + s.inner_index] = s.tsprite2_entries[s.tsprite2_entry_index];
      s.tsprite2_entry_index = s.tsprite2_entry_index + 1;
    }
  }
  memcpy(&g_tsprite2_extra_sprite_entries, &s.tsprite2_entries[s.tsprite2_entry_index], 0x34);
  memcpy(&g_tsprite2_overlay_sprite_entries, &s.tsprite2_entries[s.tsprite2_entry_index + 13], 0x18);

  for (s.entry_index = 0; s.entry_index < 0x20; s.entry_index = s.entry_index + 1)
  {
    g_opening_menu_sprite_work_buffer[s.entry_index].sprites[0] = (EncodedImage *)0;
  }

  s.entry_index = ReadSpriteEntryPointers(&g_opening_menu_sprite_work_buffer[0x10].sprites[0], BuildResolutionSpritePath((gender_map[g_player_is_male] != 0) ? "ego_f.spr" : "ego_m.spr"));
  s.ego_sprite_header_ptr = g_opening_menu_sprite_work_buffer[0x10].sprites[0];
  g_ego_sprite_width = s.ego_sprite_header_ptr->width;
  g_ego_sprite_height = s.ego_sprite_header_ptr->height;
  g_ego_sprite_draw_height = s.ego_sprite_header_ptr->top_clip;
  if (g_ego_sprite_height < g_ego_sprite_draw_height)
  {
    g_ego_sprite_draw_height = (g_ego_sprite_height * 2) / 3;
  }

  s.entry_index = ReadSpriteEntryPointers(&g_opening_menu_sprite_work_buffer[0x11].sprites[0], BuildResolutionSpritePath("sego_f.spr"));
  s.sego_sprite_header_ptr = g_opening_menu_sprite_work_buffer[0x11].sprites[0];
  g_sego_sprite_width = s.sego_sprite_header_ptr->width;
  g_sego_sprite_height = s.sego_sprite_header_ptr->height;
  g_sego_sprite_draw_height = s.sego_sprite_header_ptr->top_clip;
  if (g_sego_sprite_height < g_sego_sprite_draw_height)
  {
    g_sego_sprite_draw_height = (g_sego_sprite_height * 2) / 3;
  }

  s.entry_index = ReadSpriteEntryPointers(g_castles_sprite_entries, BuildResolutionSpritePath("castles1.spr"));
  s.entry_index = ReadSpriteEntryPointersWithLimit(&g_castles_sprite_entries[12], BuildResolutionSpritePath("castles2.spr"), 8);
  s.entry_index = ReadSpriteEntryPointersWithLimit(g_location07_sprite_entries, BuildResolutionSpritePath("locatn07.spr"), 12);

  s.dbox_entry_index = 0;
  ReadSpriteEntryPointers(s.dbox_entries, "dbox.spr");
  for (s.entry_index = 0; s.entry_index < 4; s.entry_index = s.entry_index + 1)
  {
    for (s.inner_index = 0; s.inner_index < 9; s.inner_index = s.inner_index + 1)
    {
      g_dialog_box_sprite_bank.frame[s.entry_index][s.inner_index] = s.dbox_entries[s.dbox_entry_index];
      s.dbox_entry_index = s.dbox_entry_index + 1;
    }
  }
  ReadSpriteEntryPointers(g_icons_sprite_entries, "icons.spr");

  s.iconb_entry_index = 0;
  ReadSpriteEntryPointers(s.iconb_entries, "iconb.spr");
  for (s.entry_index = 0; s.entry_index < 4; s.entry_index = s.entry_index + 1)
  {
    g_dialog_box_sprite_bank.icon_rows[s.entry_index][0] = s.iconb_entries[s.iconb_entry_index++];
    g_dialog_box_sprite_bank.icon_rows[s.entry_index][1] = s.iconb_entries[s.iconb_entry_index++];

    for (s.inner_index = 0; s.inner_index < 2; s.inner_index = s.inner_index + 1)
    {
      g_dialog_box_sprite_bank.icon_rows[s.entry_index][s.inner_index + 2] = s.iconb_entries[s.iconb_entry_index++];
    }
  }
  if (g_skip_world_sfx_preload == 0)
  {
    ReadSpriteEntryPointers(g_clocknew_sprite_entries, "clocknew.spr");
    ReadSpriteEntryPointers(g_daysnew_sprite_entries, "daysnew.spr");
    ReadSpriteEntryPointers(g_sunmoon_sprite_entries, "Sunmoon.spr");
  }

  LoadPcxIntoPage(1, "tips.pic");
  BeginSpriteEncodeSession();
  switch (global_screen_width)
  {
  case 640:
    g_tips_frame_sprite = EncodeSpriteFromPage(1, 1, 1, 5, 0x10);
    g_tips_icon_sprite = EncodeSpriteFromPage(1, 10, 1, 3, 2);
    break;
  case 800:
    g_tips_frame_sprite = EncodeSpriteFromPage(1, 1, 0x1d, 6, 0x14);
    g_tips_icon_sprite = EncodeSpriteFromPage(1, 10, 0x1d, 5, 3);
    break;
  case 1024:
    g_tips_frame_sprite = EncodeSpriteFromPage(1, 1, 0x39, 8, 0x1b);
    g_tips_icon_sprite = EncodeSpriteFromPage(1, 10, 0x39, 6, 9);
    break;
  }
  FinalizeSpriteEncodeSession();
}

// FUNCTION: SHANDALAR 0x004ffcb4
int RenderAdvMenuControlDisabled(AdvMenuControl *control)
{
  int previous_state;

  previous_state = control->state;
  g_menu_render_guard = 1;
  control->state = 0;
  control->on_render(control, 3);
  g_menu_render_guard = 0;
  control->state = 3;
  return previous_state;
}

// FUNCTION: SHANDALAR 0x004ff456
int HandleMainMenuButtonControlEvent(AdvMenuControl *control, int event_type)
{
  int preview_panel_y_offset;
  int avatar_sprite_index;
  int mouse_inside_control;

  if (g_menu_render_guard == 0)
  {
    if ((g_mouse_x < control->x) || (control->x + control->width < g_mouse_x))
    {
      mouse_inside_control = 0;
    }
    else if ((g_mouse_y < control->y) || (control->y + control->height < g_mouse_y))
    {
      mouse_inside_control = 0;
    }
    else
    {
      mouse_inside_control = 1;
    }
    if (mouse_inside_control == 0)
    {
      return 0;
    }
  }
  if (control->state == 3)
  {
    return 0;
  }
  if (event_type != 2)
  {
    DrawEncodedImageResampled(g_menu_control_draw_target_page, control->x, control->y, control->width, control->height, control->mode_data[event_type]);
  }
  else
  {
    preview_panel_y_offset = ScaleUiCoordinate(4);
    avatar_sprite_index = ScaleUiCoordinate(8);
    BlitGraphicsRect(g_page5_window_bounds, (unsigned int)control->x, control->y, (unsigned int)control->width, (DWORD)control->height, g_page1_window_bounds,
                     control->x, control->y);
    DrawEncodedImageResampled(g_page1_window_bounds, control->x + preview_panel_y_offset, control->y + preview_panel_y_offset, control->width - avatar_sprite_index, control->height - avatar_sprite_index,
                              control->mode_data[event_type]);
    BlitGraphicsRect(g_page1_window_bounds, (unsigned int)control->x, control->y, (unsigned int)control->width, (DWORD)control->height, g_page0_window_bounds,
                     control->x, control->y);
  }
  if (event_type == 2 && control->on_activate != (AdvMenuActivateCallback)0)
  {
    control->on_activate(control);
  }
  return 1;
}

// FUNCTION: SHANDALAR 0x004ff652
int HandlePortraitMainMenuControlEvent(AdvMenuControl *control, int event_type)
{
  struct
  {
    int mouse_inside_control;
    int clamped_required_wins;
    int avatar_sprite_index;
    int draw_y;
    DWORD draw_height;
    int draw_x;
    unsigned int draw_width;
    EncodedImage *sprite;
  } s;

  if (event_type == 0)
  {
    s.sprite = g_face_preview_sprite_selected;
  }
  else
  {
    s.sprite = g_face_preview_sprite_group[0];
  }
  if (g_menu_render_guard == 0)
  {
    if ((g_mouse_x < control->x) || (control->x + control->width < g_mouse_x))
    {
      s.mouse_inside_control = 0;
    }
    else if ((g_mouse_y < control->y) || (control->y + control->height < g_mouse_y))
    {
      s.mouse_inside_control = 0;
    }
    else
    {
      s.mouse_inside_control = 1;
    }
    if (s.mouse_inside_control == 0)
    {
      return 0;
    }
  }
  if (control->state == 3)
  {
    return 0;
  }
  s.draw_height = (DWORD)ScaleUiCoordinate(0x30);
  s.draw_width = (unsigned int)ScaleUiCoordinate((s.sprite->width * 0x30) / (int)s.sprite->height);
  s.draw_x = ScaleUiCoordinate(0x20) - (int)s.draw_width / 2;
  s.draw_y = ScaleUiCoordinate(0x106) - (int)s.draw_height / 2;
  if (event_type != 2)
  {
    DrawEncodedImageResampled(g_menu_control_draw_target_page, s.draw_x, s.draw_y, s.draw_width, (int)s.draw_height, s.sprite);
  }
  else
  {
    s.avatar_sprite_index = ScaleUiCoordinate(4);
    s.clamped_required_wins = ScaleUiCoordinate(8);
    BlitGraphicsRect(g_page5_window_bounds, s.draw_x, s.draw_y, s.draw_width, s.draw_height, g_page1_window_bounds, s.draw_x, s.draw_y);
    DrawEncodedImageResampled(g_page1_window_bounds, s.avatar_sprite_index + s.draw_x, s.avatar_sprite_index + s.draw_y, s.draw_width - s.clamped_required_wins, (int)s.draw_height - s.clamped_required_wins, s.sprite);
    BlitGraphicsRect(g_page1_window_bounds, s.draw_x, s.draw_y, s.draw_width, s.draw_height, g_page0_window_bounds, s.draw_x, s.draw_y);
  }
  if (event_type == 2 && control->on_activate != (AdvMenuActivateCallback)0)
  {
    control->on_activate(control);
  }
  return 1;
}

// FUNCTION: SHANDALAR 0x004ff888
int HandleWorldMagicChoiceControlEvent(AdvMenuControl *control_ptr, int event_type)
{
  struct
  {
    int mouse_inside_control;
    int avatar_sprite_draw_order[6];
    int avatar_draw_y;
    int avatar_x_positions[5];
    EncodedImage *avatar_sprite;
    int icon_y_scaled;
    unsigned int icon_x_scaled;
    DWORD icon_height_scaled;
    unsigned int icon_width_scaled;
    EncodedImage *selected_state_sprite;
    int icon_border_padding;
    int world_magic_unlock_score;
    int world_magic_slot_index;
  } s;

  s.world_magic_slot_index = (control_ptr->selection_value - 0x31) * 2 + 2;
  if (g_menu_render_guard == 0)
  {
    if ((g_mouse_x < control_ptr->x) || (control_ptr->width + control_ptr->x < g_mouse_x))
    {
      s.mouse_inside_control = 0;
    }
    else if ((g_mouse_y < control_ptr->y) || (control_ptr->y + control_ptr->height < g_mouse_y))
    {
      s.mouse_inside_control = 0;
    }
    else
    {
      s.mouse_inside_control = 1;
    }
    if (s.mouse_inside_control == 0)
    {
      return 0;
    }
  }
  if (control_ptr->state == 3)
  {
    return 0;
  }
  if ((g_world_magic_bitmap & (1 << (unsigned char)s.world_magic_slot_index)) != 0)
  {
    s.world_magic_unlock_score = FindWorldMagicCardIndex(s.world_magic_slot_index);
    if (g_amulet_inventory[s.world_magic_slot_index / 2 - 1] != 0)
    {
      if (event_type != 2)
      {
        DrawEncodedImageUiScaled(g_page0_window_bounds, g_world_magic_icon_rects[s.world_magic_slot_index].x, g_world_magic_icon_rects[s.world_magic_slot_index].y,
                                 g_world_magic_choice_controls[control_ptr->selection_value - 0x31].mode_data[event_type],
                                 g_world_magic_icon_rects[s.world_magic_slot_index].width, g_world_magic_icon_rects[s.world_magic_slot_index].height);
      }
      else
      {
        s.selected_state_sprite = g_world_magic_choice_controls[control_ptr->selection_value - 0x31].mode_data[event_type];
        s.icon_x_scaled = (unsigned int)ScaleUiCoordinate(g_world_magic_icon_rects[s.world_magic_slot_index].x);
        s.icon_y_scaled = ScaleUiCoordinate(g_world_magic_icon_rects[s.world_magic_slot_index].y);
        s.icon_width_scaled = (unsigned int)ScaleUiCoordinate(g_world_magic_icon_rects[s.world_magic_slot_index].width);
        s.icon_height_scaled = (DWORD)ScaleUiCoordinate(g_world_magic_icon_rects[s.world_magic_slot_index].height);
        s.icon_border_padding = ScaleUiCoordinate(4);
        BlitGraphicsRect(g_page3_window_bounds, s.icon_x_scaled, s.icon_y_scaled - ScaleUiCoordinate(0x148), s.icon_width_scaled, s.icon_height_scaled, g_page1_window_bounds, s.icon_x_scaled, s.icon_y_scaled);
        DrawEncodedImageUiScaled(g_page1_window_bounds, g_world_magic_icon_rects[s.world_magic_slot_index].x, g_world_magic_icon_rects[s.world_magic_slot_index].y, (EncodedImage *)g_world_magic_choice_button_sprite_bank.named.icon[s.world_magic_slot_index],
                                 g_world_magic_icon_rects[s.world_magic_slot_index].width, g_world_magic_icon_rects[s.world_magic_slot_index].height);
        DrawEncodedImageResampled(g_page1_window_bounds,
                                  s.icon_border_padding + (int)s.icon_x_scaled,
                                  s.icon_border_padding + s.icon_y_scaled,
                                  s.icon_width_scaled - s.icon_border_padding * 2,
                                  (int)s.icon_height_scaled - s.icon_border_padding * 2,
                                  s.selected_state_sprite);

        s.avatar_draw_y = 400;
        s.avatar_sprite = g_world_magic_avatar_sprites[0];
        s.avatar_x_positions[0] = 0x6c;
        s.avatar_x_positions[1] = 0xbf;
        s.avatar_x_positions[2] = 0x10e;
        s.avatar_x_positions[3] = 0x15e;
        s.avatar_x_positions[4] = 0x1b1;
        s.avatar_sprite_draw_order[0] = 2;
        s.avatar_sprite_draw_order[1] = 1;
        s.avatar_sprite_draw_order[2] = 4;
        s.avatar_sprite_draw_order[3] = 3;
        s.avatar_sprite_draw_order[4] = 0;
        s.avatar_sprite_draw_order[5] = s.world_magic_slot_index / 2 - 1;
        g_page0_window_bounds->font_slot = 4;
        s.avatar_draw_y -= (int)s.avatar_sprite->height / 2;
        DrawEncodedImageUiScaled(g_page1_window_bounds, s.avatar_x_positions[s.avatar_sprite_draw_order[5]] - 0x1e, s.avatar_draw_y, g_world_magic_avatar_sprites[s.avatar_sprite_draw_order[s.avatar_sprite_draw_order[5]]], (int)s.avatar_sprite->width,
                                 (int)s.avatar_sprite->height);
        BlitGraphicsRect(g_page1_window_bounds, s.icon_x_scaled, s.icon_y_scaled, s.icon_width_scaled, s.icon_height_scaled, g_page0_window_bounds, s.icon_x_scaled, s.icon_y_scaled);
      }
      if ((event_type == 2) && (control_ptr->on_activate != (AdvMenuActivateCallback)0))
      {
        control_ptr->on_activate(control_ptr);
      }
    }
  }
  return 1;
}

// FUNCTION: SHANDALAR 0x004ffc5d
int ActivateWorldMagicChoiceControl(AdvMenuControl *control)
{
  ClearInputAndWaitForMouseRelease();
  if (g_pending_ui_action_code == 0)
  {
    g_pending_ui_action_code = control->selection_value;
  }
  g_mouse_button_down_mask = 0;
  PlaySoundEffectOnChannel(g_world_magic_sound_paths[control->selection_value - 0x31], 0xf, 100, 100, 0);
  return 0;
}

// FUNCTION: SHANDALAR 0x004ffd56
int ActivateMainMenuControl(AdvMenuControl *control)
{
  ClearInputAndWaitForMouseRelease();
  if (g_pending_ui_action_code == 0)
  {
    g_pending_ui_action_code = control->selection_value;
  }
  g_mouse_button_down_mask = 0;
  PlaySoundEffectOnChannel("x:sound\\button2.wav", 0xf, 100, 100, 0);
  return 0;
}

// FUNCTION: SHANDALAR 0x004ffda4
int InitializeMainMenuAndWorldMagicChoiceControls(void)
{
  int i;
  int sprite_index;
  int scale_control_index;

  g_menu_saved_window_bounds = *g_page0_window_bounds;

  for (i = 0; i < 4; i = i + 1)
  {
    g_main_menu_controls[i].mode_data[0] = g_main_menu_button_sprites_normal[i];
    g_main_menu_controls[i].mode_data[1] = g_main_menu_button_sprites_highlight[i];
    g_main_menu_controls[i].mode_data[2] = g_main_menu_button_sprites_highlight[i];
    g_main_menu_controls[i].mode_data[3] = g_main_menu_button_sprites_normal[i];
  }

  for (i = 0; i < 5; i = i + 1)
  {
    sprite_index = i * 2 + 2;
    g_world_magic_choice_controls[i].mode_data[0] = g_world_magic_choice_button_sprite_bank.named.normal[sprite_index];
    g_world_magic_choice_controls[i].mode_data[1] = g_world_magic_choice_button_sprite_bank.named.highlight[sprite_index];
    g_world_magic_choice_controls[i].mode_data[2] = g_world_magic_choice_button_sprite_bank.named.highlight[sprite_index];
    g_world_magic_choice_controls[i].mode_data[3] = g_world_magic_choice_button_sprite_bank.named.pressed[sprite_index];
  }

  if (global_screen_width != 0x280)
  {
    for (scale_control_index = 0; scale_control_index < 4; scale_control_index = scale_control_index + 1)
    {
      g_main_menu_controls[scale_control_index].x = (g_main_menu_controls[scale_control_index].x * global_screen_width) / 0x280;
      g_main_menu_controls[scale_control_index].y = (g_main_menu_controls[scale_control_index].y * global_screen_width) / 0x280;
      g_main_menu_controls[scale_control_index].width = (g_main_menu_controls[scale_control_index].width * global_screen_width) / 0x280;
      g_main_menu_controls[scale_control_index].height = (g_main_menu_controls[scale_control_index].height * global_screen_width) / 0x280;
    }
    for (scale_control_index = 0; scale_control_index < 5; scale_control_index = scale_control_index + 1)
    {
      g_world_magic_choice_controls[scale_control_index].x = (g_world_magic_choice_controls[scale_control_index].x * global_screen_width) / 0x280;
      g_world_magic_choice_controls[scale_control_index].y = (g_world_magic_choice_controls[scale_control_index].y * global_screen_width) / 0x280;
      g_world_magic_choice_controls[scale_control_index].width = (g_world_magic_choice_controls[scale_control_index].width * global_screen_width) / 0x280;
      g_world_magic_choice_controls[scale_control_index].height = (g_world_magic_choice_controls[scale_control_index].height * global_screen_width) / 0x280;
    }
  }

  AddMenuControlsToContext(g_main_menu_controls, 4, 0);
  AddMenuControlsToContext(g_world_magic_choice_controls, 5, 0);
  return 1;
}

// FUNCTION: SHANDALAR 0x005631ca
void UpdateWorldMagicUnlockProgress(void)
{
  struct
  {
    int required_duel_wins;
    int mapped_world_magic_index;
    int world_magic_duel_win_count;
    int scan_index;
    int wizard_color_index;
    int original_required_duel_wins;
    int world_magic_progress_values[5];
    int world_magic_victory_counts[5];
    unsigned char world_magic_town_counts[5];
    unsigned char unused_flag_a;
    unsigned char unused_flag_b;
    unsigned char unused_flag_c;
    int unused_padding;
    int town_count_for_wizard_color;
  } s;

  for (s.wizard_color_index = 0; s.wizard_color_index < 5; s.wizard_color_index = s.wizard_color_index + 1)
  {
    s.world_magic_duel_win_count = 0;
    s.town_count_for_wizard_color = 0;
    s.mapped_world_magic_index = MapWizardColorToDisplayIndex(s.wizard_color_index + 1);
    for (s.scan_index = 0; s.scan_index < 0x80; s.scan_index = s.scan_index + 1)
    {
      if ((g_town_slots[s.scan_index].status_and_ruling_wizard & 0xff00) &&
          (((int)g_town_slots[s.scan_index].status_and_ruling_wizard >> 8) - 1 == s.wizard_color_index))
      {
        s.town_count_for_wizard_color = s.town_count_for_wizard_color + 1;
      }
    }
    s.world_magic_town_counts[s.mapped_world_magic_index] = (unsigned char)s.town_count_for_wizard_color;
    if (g_world_magic_town_flags[s.wizard_color_index] != 0)
    {
      s.world_magic_progress_values[s.mapped_world_magic_index] = 0;
    }
    else
    {
      s.required_duel_wins = s.town_count_for_wizard_color * g_shandalar_difficulty + g_shandalar_difficulty * 5 + 0x1e;
      s.original_required_duel_wins = s.required_duel_wins;
      for (s.scan_index = 0; s.scan_index < 1000; s.scan_index = s.scan_index + 1)
      {
        if ((char)g_duel_victory_log[s.scan_index] == '\0')
        {
          break;
        }

        if (((int)(char)g_duel_victory_log[s.scan_index] >> 4) == s.wizard_color_index + 1)
        {
          s.world_magic_duel_win_count = s.world_magic_duel_win_count + 1;
        }
      }
      s.world_magic_victory_counts[s.mapped_world_magic_index] = s.world_magic_duel_win_count;
      s.required_duel_wins = MAX(g_shandalar_difficulty * 5 + 20, s.required_duel_wins - s.world_magic_duel_win_count);
      s.world_magic_progress_values[s.mapped_world_magic_index] = 0x1e - (s.original_required_duel_wins - s.required_duel_wins);
    }
  }
  s.unused_flag_a = 0;
  s.unused_flag_b = 0;
  s.unused_flag_c = 0;
  UpdateStatWindowProgress(s.world_magic_progress_values);
}

// FUNCTION: SHANDALAR 0x0054cdbd
void RefreshAdventureInterfaceLayout(void)
{
  g_current_world_location = g_pending_world_location = -1;
  g_adventure_ui_layout_dirty = 1;
  if (g_duel_ai_mode_state == 0)
  {
    LoadPcxIntoPage(1, PTR_s_advinter800_pic_00589de8);
    BlitGraphicsRect(g_page1_window_bounds, 0, 0, global_screen_width, global_screen_height, g_page0_window_bounds, 0, 0);
    RenderMenuControlRange(0, 4);
  }

  g_world_scroll_cache_ready = 0;
  if (g_adventure_ui_edge_pages_initialized == 0)
  {
    BlitGraphicsRect(g_page1_window_bounds, 0, ScaleUiCoordinate(0x148), ScaleUiCoordinate(0x280), ScaleUiCoordinate(0x1e0) - ScaleUiCoordinate(0x148), g_page3_window_bounds, 0, 0);
    BlitGraphicsRect(g_page1_window_bounds, 0, 0, ScaleUiCoordinate(0x40), ScaleUiCoordinate(0x148), g_page5_window_bounds, 0, 0);
    g_adventure_ui_edge_pages_initialized = 1;
  }
}

// FUNCTION: SHANDALAR 0x004bdaad
int FreeOpeningMenuSpriteWorkEntries(int work_entry_index_a, int work_entry_index_b)
{
  if (g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites[0] == (EncodedImage *)0)
  {
    return 0;
  }

  FreeSpriteBlob(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites[0]);
  FreeSpriteBlob(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites[0]);
  g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites[0] = (EncodedImage *)0;
  return 0;
}
