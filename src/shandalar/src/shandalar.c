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
#include <drawcardlib/src/pic.h>

void LoadPcxResource(int page_number, int x, int y, char *path, void *opaque);

// GLOBAL: SHANDALAR 0x007483f8
HDC global_main_hdc;
// GLOBAL: SHANDALAR 0x005863b8
int global_screen_width = 0x280;
// GLOBAL: SHANDALAR 0x005863bc
int global_screen_height = 0x1e0;
// GLOBAL: SHANDALAR 0x005862d8
int g_neighbor_dx[9] = {0, 0, 1, 1, 1, 0, -1, -1, -1};
// GLOBAL: SHANDALAR 0x00586340
int g_neighbor_dy[9] = {0, -1, -1, 0, 1, 1, 1, 0, -1};
// GLOBAL: SHANDALAR 0x00589de8
char *PTR_s_advinter800_pic_00589de8 = "advinter800.pic";
// GLOBAL: SHANDALAR 0x00583290
FacemakerWindowBounds g_page0_window_bounds_storage = {0, 0, 0, 0x27f, 0x1df, 1, 0x0f, 4, 0};
// GLOBAL: SHANDALAR 0x005832b4
FacemakerWindowBounds *g_page0_window_bounds = &g_page0_window_bounds_storage;
// GLOBAL: SHANDALAR 0x005832b8
FacemakerWindowBounds g_page1_window_bounds_storage = {1, 0, 0, 0x320, 0x258, 1, 0x0f, 4, 0};
// GLOBAL: SHANDALAR 0x005832dc
FacemakerWindowBounds *g_page1_window_bounds = &g_page1_window_bounds_storage;
// GLOBAL: SHANDALAR 0x005832e0
FacemakerWindowBounds g_page2_window_bounds_storage = {2, 0, 0, 0x27f, 0x1df, 1, 0x0f, 4, 0};
// GLOBAL: SHANDALAR 0x00583304
FacemakerWindowBounds *g_page2_window_bounds = &g_page2_window_bounds_storage;
// GLOBAL: SHANDALAR 0x00583308
FacemakerWindowBounds g_page3_window_bounds_storage = {3, 0, 0, 0x27f, 0x1df, 1, 0x0f, 4, 0};
// GLOBAL: SHANDALAR 0x0058332c
FacemakerWindowBounds *g_page3_window_bounds = &g_page3_window_bounds_storage;
// GLOBAL: SHANDALAR 0x00583330
FacemakerWindowBounds g_page5_window_bounds_storage = {5, 0, 0, 0x27f, 0x1df, 1, 0x0f, 4, 0};
// GLOBAL: SHANDALAR 0x00583354
FacemakerWindowBounds *g_page5_window_bounds = &g_page5_window_bounds_storage;
// GLOBAL: SHANDALAR 0x00939160
HINSTANCE g_app_instance;
// GLOBAL: SHANDALAR 0x00591210
int g_skip_world_sfx_preload = 0;
// GLOBAL: SHANDALAR 0x00591224
int g_loadsave_skip_esc = 0;
// GLOBAL: SHANDALAR 0x00748418
UINT g_timer_resolution_ms;
// GLOBAL: SHANDALAR 0x007483fc
UINT g_timer_event_handle;
// GLOBAL: SHANDALAR 0x0078cefc
int g_cursor_visibility_depth;
// GLOBAL: SHANDALAR 0x00589de4
UINT g_timer_period_ms = 0x21;
// GLOBAL: SHANDALAR 0x00589df0
int g_ui_tick_count = 0;
// GLOBAL: SHANDALAR 0x00589dec
int g_default_palette_fade_steps = 0x30;
// GLOBAL: SHANDALAR 0x00748400
int g_sound_loader_busy;
// GLOBAL: SHANDALAR 0x00748404
HANDLE g_main_thread_handle;
// GLOBAL: SHANDALAR 0x00748424
HANDLE g_timer_thread_handle;
// GLOBAL: SHANDALAR 0x00748408
int g_local_sound_missing;
// GLOBAL: SHANDALAR 0x0074840c
int g_local_sound_state;
// GLOBAL: SHANDALAR 0x00748410
HANDLE g_loader_thread_handle;
// GLOBAL: SHANDALAR 0x005b7d90
int g_palette_class_registered;
// GLOBAL: SHANDALAR 0x005b7d94
int g_timer_thread_handle_ready;
// GLOBAL: SHANDALAR 0x005b7d98
HWND g_palette_window_hwnd;
// GLOBAL: SHANDALAR 0x00986d94
int g_mouse_button_down_mask;
// GLOBAL: SHANDALAR 0x00986d98
int g_mouse_y;
// GLOBAL: SHANDALAR 0x00986d9c
int g_mouse_x;
// GLOBAL: SHANDALAR 0x00986da0
int g_mouse_button_released_mask;

// GLOBAL: SHANDALAR 0x00669704
int g_cached_cwd_initialized;
// GLOBAL: SHANDALAR 0x0066970c
int g_sound_drive_initialized;
// GLOBAL: SHANDALAR 0x0073e890
char g_cached_cwd[0x100];
// GLOBAL: SHANDALAR 0x0073e9d8
char g_sound_drive_letter;
// GLOBAL: SHANDALAR 0x0067a3b8
char *g_advblocks_file_buffer;
// GLOBAL: SHANDALAR 0x0078cf08
FILE *g_advbuttons_ini_file;
// GLOBAL: SHANDALAR 0x0078df10
char g_ini_string_scratch[0x28];
// GLOBAL: SHANDALAR 0x0078df38
char *g_done_text_table_entry;
// GLOBAL: SHANDALAR 0x00586494
int g_legacy_mouse_active = 0;
// GLOBAL: SHANDALAR 0x00586498
int g_hide_world_map_overlays = 0;
// GLOBAL: SHANDALAR 0x007486d0
EncodedImage *g_ttsprite_special_sprite_a;
// GLOBAL: SHANDALAR 0x007486e0
EncodedImage *g_world_magic_avatar_sprites[5];
// GLOBAL: SHANDALAR 0x00781728
EncodedImage *g_face_preview_sprite_selected;
// GLOBAL: SHANDALAR 0x0078172c
EncodedImage *g_face_preview_sprite_group[6];
// GLOBAL: SHANDALAR 0x0073e9dc
int g_world_scene_reveal_effect_pending;
// GLOBAL: SHANDALAR 0x00591214
int g_world_location_entry_enabled = 1;
// GLOBAL: SHANDALAR 0x00591218
int g_world_move_dir_index = 0;
// GLOBAL: SHANDALAR 0x0059121c
int g_world_location_music_active = 0;
// GLOBAL: SHANDALAR 0x00591220
int g_world_location_music_track_id = -1;
// GLOBAL: SHANDALAR 0x0059126c
int g_world_location_music_town_index = -1;
// GLOBAL: SHANDALAR 0x0073ea70
int g_world_magic_town_flags[5];
// GLOBAL: SHANDALAR 0x0073ea84
int g_world_player_animation_direction;
// GLOBAL: SHANDALAR 0x0073ea8c
int g_world_player_animation_frame;
// GLOBAL: SHANDALAR 0x006696f4
int g_world_location_entry_latched;
// GLOBAL: SHANDALAR 0x006696f8
int g_advfac64_load_state;
// GLOBAL: SHANDALAR 0x006696fc
int g_adventure_demo_enabled;
// GLOBAL: SHANDALAR 0x00669700
int g_adventure_demo_idle_ticks;
// GLOBAL: SHANDALAR 0x00669710
int g_world_scene_force_redraw;
// GLOBAL: SHANDALAR 0x00789938
int g_deck_total_card_count;
// GLOBAL: SHANDALAR 0x0078df68
int g_deck_active_card_count;
// GLOBAL: SHANDALAR 0x0097db40
long g_hint_text_offsets[0x100];
// GLOBAL: SHANDALAR 0x0097df40
int g_hint_difficulty_masks[0x100];
// GLOBAL: SHANDALAR 0x0097e340
char g_last_parsed_deck_path[0x110];
// GLOBAL: SHANDALAR 0x0097e450
HintPair g_hint_card_pairs[0x100];
// GLOBAL: SHANDALAR 0x007898f4
int g_mouse_x_snapshot;
// GLOBAL: SHANDALAR 0x007898f8
int g_mouse_y_snapshot;
// GLOBAL: SHANDALAR 0x007898f0
int g_mouse_button_mask_snapshot;
// GLOBAL: SHANDALAR 0x007490f0
DialogBoxSpriteBank g_dialog_box_sprite_bank;
// GLOBAL: SHANDALAR 0x0058e050
HANDLE g_statwin_dll_module = 0;
// GLOBAL: SHANDALAR 0x0058c5ec
int g_dungeon_card_effect_by_color[4] = {0, 8, 867, 939};
// GLOBAL: SHANDALAR 0x0058c5fc
int g_dungeon_card_effect_by_monster_flags[9] = {178, 51, 159, 287, 63, 114, 229, 5, -1};
// GLOBAL: SHANDALAR 0x0058c620
int g_mana_castle_card_effect_by_difficulty[5][4] = {
    {246, 100, 208, 661},
    {52, 494, 935, 340},
    {288, 15, 262, 803},
    {123, 439, 192, 155},
    {128, 134, 28, 620}};
// GLOBAL: SHANDALAR 0x00746e00
int g_statwin_exports_by_ordinal[3];
// GLOBAL: SHANDALAR 0x00747ee0
int g_menu_render_guard;
// GLOBAL: SHANDALAR 0x00747ef0
AnimatedNoiseGridPlane g_animated_noise_grid[2];
// GLOBAL: SHANDALAR 0x00746ec0
FacemakerWindowBounds g_menu_saved_window_bounds;
// GLOBAL: SHANDALAR 0x00746ef0
int g_menu_control_count_by_context[50];
// GLOBAL: SHANDALAR 0x00746f40
AdvMenuControl *g_menu_controls_by_context[50][50];
// GLOBAL: SHANDALAR 0x00748d10
EncodedImage *g_main_menu_button_sprites_normal[4];
// GLOBAL: SHANDALAR 0x00748d20
EncodedImage *g_main_menu_button_sprites_highlight[4];
// GLOBAL: SHANDALAR 0x007491c0
WorldMagicChoiceButtonSpriteBank g_world_magic_choice_button_sprite_bank;
// GLOBAL: SHANDALAR 0x00748440
EncodedImage *g_road_sprite_entries[0xc];
// GLOBAL: SHANDALAR 0x00748470
EncodedImage *g_location07_sprite_entries[0xc];
// GLOBAL: SHANDALAR 0x007484a0
EncodedImage *g_land_sprite_entries[22][5];

// GLOBAL: SHANDALAR 0x00748660
EncodedImage *g_tsprite2_grid_sprite_entries[0xc];
// GLOBAL: SHANDALAR 0x00748690
EncodedImage *g_land_tile_sprite_entries[0x10];
// GLOBAL: SHANDALAR 0x00748700
EncodedImage *g_ttsprite_alt_sprite_entries[8];
// GLOBAL: SHANDALAR 0x00748720
EncodedImage *g_cstline1_sprite_entries[12][7];
// GLOBAL: SHANDALAR 0x00748870
EncodedImage *g_ttsprite_aux_sprite_entries[8];
// GLOBAL: SHANDALAR 0x00748890
int g_world_lair_monster_sprite_top_clips[0x10];
// GLOBAL: SHANDALAR 0x007488d0
int g_ego_sprite_draw_height;
// GLOBAL: SHANDALAR 0x007488d4
int g_sego_sprite_draw_height;
// GLOBAL: SHANDALAR 0x00748910
EncodedImage *g_location_marker_sprite_entries[0x9e];
// GLOBAL: SHANDALAR 0x00748d30
EncodedImage *g_questnew_sprite_entries[4];
// GLOBAL: SHANDALAR 0x00748d40
EncodedImage *g_asprite_sprite_entries[0x3c];
// GLOBAL: SHANDALAR 0x00748e30
EncodedImage *g_tsprite2_extra_sprite_entries[0xd];
// GLOBAL: SHANDALAR 0x00748e64
EncodedImage *g_gsprite_sprite_entries[3];
// GLOBAL: SHANDALAR 0x00748e70
EncodedImage *g_sunmoon_sprite_entries[0x14];
// GLOBAL: SHANDALAR 0x00748ec0
EncodedImage *g_worlds_extra_sprite_entries[4];
// GLOBAL: SHANDALAR 0x00748ed0
EncodedImage *g_tips_frame_sprite;
// GLOBAL: SHANDALAR 0x00748ee0
EncodedImage *g_clocknew_sprite_entries[9];
// GLOBAL: SHANDALAR 0x00748f04
EncodedImage *g_tips_icon_sprite;
// GLOBAL: SHANDALAR 0x00748f10
EncodedImage *g_wizard_controlled_town_sprite_entries[5];
// GLOBAL: SHANDALAR 0x00748f30
EncodedImage *g_sland_sprite_entries[22][5];
// GLOBAL: SHANDALAR 0x00749280
EncodedImage *g_daysnew_sprite_entries[0xc];
// GLOBAL: SHANDALAR 0x007492b0
EncodedImage *g_icons_sprite_entries[0x18];
// GLOBAL: SHANDALAR 0x00749310
int g_world_lair_monster_sprite_widths[0x10];
// GLOBAL: SHANDALAR 0x00749350
int g_ego_sprite_width;
// GLOBAL: SHANDALAR 0x00749354
int g_sego_sprite_width;
// GLOBAL: SHANDALAR 0x00749390
int g_world_lair_monster_sprite_heights[0x10];
// GLOBAL: SHANDALAR 0x007493d0
int g_ego_sprite_height;
// GLOBAL: SHANDALAR 0x007493d4
int g_sego_sprite_height;
// GLOBAL: SHANDALAR 0x00749410
EncodedImage *g_ttsprite_special_sprite_b;
// GLOBAL: SHANDALAR 0x00749414
EncodedImage *g_ttsprite_special_sprite_c;
// GLOBAL: SHANDALAR 0x00749418
EncodedImage *g_current_location_marker_sprite;
// GLOBAL: SHANDALAR 0x00749420
EncodedImage *g_compnew_sprite_entries[5];
// GLOBAL: SHANDALAR 0x00749434
EncodedImage *g_endtop_banner_sprite;
// GLOBAL: SHANDALAR 0x00749440
EncodedImage *g_tsprite2_overlay_sprite_entries[8];
// GLOBAL: SHANDALAR 0x00749460
EncodedImage *g_ttsprite_grid_sprite_entries[0x40];
// GLOBAL: SHANDALAR 0x00749560
EncodedImage *g_castles_sprite_entries[20];
// GLOBAL: SHANDALAR 0x007496a0
OpeningMenuSpriteWorkEntry g_opening_menu_sprite_work_buffer[0x20];
// GLOBAL: SHANDALAR 0x0078df40
EncodedImage *g_ttsprite_extra_sprite_entries[10];
// GLOBAL: SHANDALAR 0x0058b584
int g_menu_context_index = 0;
// GLOBAL: SHANDALAR 0x0058b588
AdvMenuControl g_main_menu_controls[4] = {
    {3, 0x47, 0x39, 0x36, 3, 0x47, 0x39, 0x36, 1, HandleMainMenuButtonControlEvent, ActivateMainMenuControl, 1, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {3, 0x7e, 0x39, 0x36, 3, 0x7e, 0x39, 0x36, 1, HandleMainMenuButtonControlEvent, ActivateMainMenuControl, 2, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {3, 0xb5, 0x39, 0x36, 3, 0xb5, 0x39, 0x36, 1, HandleMainMenuButtonControlEvent, ActivateMainMenuControl, 4, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {3, 0xec, 0x39, 0x36, 3, 0xec, 0x39, 0x36, 1, HandlePortraitMainMenuControlEvent, ActivateMainMenuControl, 5, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
};
// GLOBAL: SHANDALAR 0x0058b6d8
AdvMenuControl g_world_magic_choice_controls[5] = {
    {0x28, 0x17c, 0x42, 0x28, 0x28, 0x17c, 0x42, 0x28, 1, HandleWorldMagicChoiceControlEvent, ActivateWorldMagicChoiceControl, 0x31, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x73, 0x17c, 0x48, 0x28, 0x73, 0x17c, 0x48, 0x28, 1, HandleWorldMagicChoiceControlEvent, ActivateWorldMagicChoiceControl, 0x32, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0xc5, 0x17c, 0x48, 0x28, 0xc5, 0x17c, 0x48, 0x28, 1, HandleWorldMagicChoiceControlEvent, ActivateWorldMagicChoiceControl, 0x33, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x114, 0x17c, 0x48, 0x28, 0x114, 0x17c, 0x48, 0x28, 1, HandleWorldMagicChoiceControlEvent, ActivateWorldMagicChoiceControl, 0x34, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
    {0x166, 0x17c, 0x48, 0x28, 0x166, 0x17c, 0x48, 0x28, 1, HandleWorldMagicChoiceControlEvent, ActivateWorldMagicChoiceControl, 0x35, 0, (char *)0, (char *)0, 0, 0, {0, 0, 0, 0}},
};
// GLOBAL: SHANDALAR 0x0058b580
FacemakerWindowBounds *g_menu_control_draw_target_page = &g_menu_saved_window_bounds;
// GLOBAL: SHANDALAR 0x0058b87c
int g_world_magic_icon_rect_table_padding = 0;
// GLOBAL: SHANDALAR 0x0058b880
AdvMenuRect g_world_magic_icon_rects[0xc] = {
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
// GLOBAL: SHANDALAR 0x0058b940
char *g_world_magic_sound_paths[5] = {
    "x:sound\\blackwm.wav",
    "x:sound\\bluewm.wav",
    "x:sound\\greenwm.wav",
    "x:sound\\redwm.wav",
    "x:sound\\whitewm.wav",
};
// GLOBAL: SHANDALAR 0x0058b954
int g_menu_current_control_index = -1;
// GLOBAL: SHANDALAR 0x0058b958
int g_menu_prev_control_index = -1;
// GLOBAL: SHANDALAR 0x005b80d8
int g_menu_input_unhandled;
// GLOBAL: SHANDALAR 0x005b80e0
int g_menu_allow_arrow_nav_by_context[50];
// GLOBAL: SHANDALAR 0x008c7408
int g_duel_special_rules_by_color[7];
// GLOBAL: SHANDALAR 0x0073e990
jmp_buf g_adventure_session_restart_jump_buffer;
// GLOBAL: SHANDALAR 0x0073e9e0
jmp_buf g_adventure_world_exit_jump_buffer;
// GLOBAL: SHANDALAR 0x006527b0
int g_pending_world_location;
// GLOBAL: SHANDALAR 0x00590764
int g_current_world_location = -1;
// GLOBAL: SHANDALAR 0x00590768
int g_adventure_ui_layout_dirty = 1;
// GLOBAL: SHANDALAR 0x00650f28
int g_adventure_ui_edge_pages_initialized;
// GLOBAL: SHANDALAR 0x0073eaa0
int g_world_scroll_cache_ready;
// GLOBAL: SHANDALAR 0x0073e880
int g_pending_ui_action_code;
// GLOBAL: SHANDALAR 0x005a6074
int g_text_menu_left;
// GLOBAL: SHANDALAR 0x005a6078
int g_text_menu_top;
// GLOBAL: SHANDALAR 0x00580d80
int g_text_menu_initial_selection = -1;
// GLOBAL: SHANDALAR 0x00580d84
int g_text_menu_mouse_released = 0;
// GLOBAL: SHANDALAR 0x00580d88
int g_text_menu_last_selection = -1;
// GLOBAL: SHANDALAR 0x00580d8c
int g_text_menu_show_ok_button = 0;
// GLOBAL: SHANDALAR 0x00580d90
int g_text_menu_mouse_active = 0;
// GLOBAL: SHANDALAR 0x00580d94
int g_text_menu_timeout_seconds = -1;
// GLOBAL: SHANDALAR 0x00580d98
int g_text_menu_line_height = 8;
// GLOBAL: SHANDALAR 0x00580d9c
int g_text_menu_color_normal = 0x71;
// GLOBAL: SHANDALAR 0x00580da0
int g_text_menu_color_selected = 0xe3;
// GLOBAL: SHANDALAR 0x005a5fe8
int g_text_menu_line_offsets[0x20];
// GLOBAL: SHANDALAR 0x005a6068
int g_text_menu_color_base;
// GLOBAL: SHANDALAR 0x005a606c
int g_text_menu_finish_flash;
// GLOBAL: SHANDALAR 0x005a6070
int g_text_menu_line_count;
// GLOBAL: SHANDALAR 0x005a607c
int g_text_menu_hovered_selection;

// GLOBAL: SHANDALAR 0x0078cef0
char g_text_menu_timer_itoa_buffer[0xc];
// GLOBAL: SHANDALAR 0x00789934
int g_shandalar_state_00789934;
// GLOBAL: SHANDALAR 0x0097ec50
signed char g_text_menu_hotkey_by_option[0x20];
// GLOBAL: SHANDALAR 0x0097ec70
int g_text_menu_caret_prefix_mask;
// GLOBAL: SHANDALAR 0x0097ec74
int g_text_menu_force_cancel;
// GLOBAL: SHANDALAR 0x0097ec78
int g_text_menu_needs_layout;
// GLOBAL: SHANDALAR 0x0097ec7c
int g_text_menu_abort_requested;
// GLOBAL: SHANDALAR 0x0097ec80
int g_text_menu_option_count;
// GLOBAL: SHANDALAR 0x0097ec84
int g_text_menu_right;
// GLOBAL: SHANDALAR 0x0097ec88
int g_text_menu_max_line_width;
// GLOBAL: SHANDALAR 0x0097ec8c
int g_text_menu_ok_mode;
// GLOBAL: SHANDALAR 0x0097ec90
int g_text_menu_first_option_line;
// GLOBAL: SHANDALAR 0x0097ec94
int g_text_box_frame_color_override;
// GLOBAL: SHANDALAR 0x0097ec98
int g_text_menu_disabled_option_mask;

// GLOBAL: SHANDALAR 0x005aa414
int g_shandalar_state_005aa414;
// GLOBAL: SHANDALAR 0x005aa62c
int g_last_matching_deck_card_index;
// GLOBAL: SHANDALAR 0x005873d4
int g_shandalar_initialized = 0;


#ifdef _DEBUG
int g_debug_lair_event_type = -1;
int RunWorldLairMonsterEncounter(int slot_index, int monster_color);
#endif

// FUNCTION: SHANDALAR 0x0055db50
int RunAdventureSession(void)
{
  struct
  {
    clock_t last_tick;
    int temp_value;
    int initialized_world;
    unsigned int tile_mask;
    int game_setup_mode;
    int loop_index;
    int proceed_to_main_loop;
  } s;

  s.initialized_world = 0;
  s.proceed_to_main_loop = 0;
  g_opponent_starting_card_id_1 = -1;
  g_opponent_starting_card_id_2 = g_opponent_starting_card_id_1;
  InitializeDuelUiGlobalIds();
  for (s.loop_index = 0; s.loop_index < 4; s.loop_index = s.loop_index + 1)
  {
    g_ai_combat_value_weights[s.loop_index] = 8;
  }

  LoadTownHintMetadata();
  LoadCardRaritiesAndCsvOffsets();
  NoopSetSpecialSprite(1, 1, (int)g_ttsprite_special_sprite_a);
  ShowMouseCursorNested();
  HideMouseCursorNested();
  s.temp_value = GdiGetBatchLimit();
  GdiSetBatchLimit(100);
  LoadPcxIntoPageNoPalette("advfac64.pic");
  ReadSpriteEntryPointers(&g_dialog_box_sprite_bank.frame[0][0], "dbox.spr");

opening_menu:
  s.game_setup_mode = RunOpeningMenu();
  AnimatePaletteToColor(0, g_default_palette_fade_steps);

  switch (s.game_setup_mode)
  {
  case 0:
  difficult_menu:
    g_shandalar_difficulty = RunDifficultyMenu();
    AnimatePaletteToColor(0, g_default_palette_fade_steps);
    if (g_shandalar_difficulty == -1)
    {
      goto opening_menu;
    }

  color_menu:
    g_selected_wizard_color = RunColorMenu();
    g_starting_color = g_selected_wizard_color;
    g_duel_interface_options.player_territory_color = g_selected_wizard_color;
    g_duel_interface_options.player_territory_type = internal_rand(3);
    AnimatePaletteToColor(0, g_default_palette_fade_steps);
    if (g_starting_color == -1)
    {
      goto difficult_menu;
    }

    g_player_is_male = RunFacemakerFlow();
    AnimatePaletteToColor(0, g_default_palette_fade_steps);
    if (g_player_is_male == -1)
    {
      goto color_menu;
    }

    g_deck_color_bitmap = 1 << ((unsigned char)g_selected_wizard_color & 0xff);
    LoadPcxIntoPageNoPalette("advfac64.pic");
    LoadPcxIntoPage(1, (char *)PTR_s_advinter800_pic_00589de8);
    BlitGraphicsRect(g_page1_window_bounds, 0, 0, global_screen_width, global_screen_height, g_page0_window_bounds, 0, 0);
    g_skip_new_game_state_initialization = 0;
    if (s.initialized_world == 0)
    {
      setup_shared_startup();
    }
    s.initialized_world = 1;
    g_page0_window_bounds->font_slot = 5;
    LoadTextSectionLines("ADVstrings.txt", "STARTUP");
    DrawTextAt(g_page0_window_bounds, 0xff, 0x140, 0xbc, g_text_lines[0]);
    g_world_scene_reveal_effect_pending = 1;
    InitializeNewGameState();
    GenerateAdventureWorldMap();
    InitializeCastleDungeonSlots();
    Gold = (5 - g_shandalar_difficulty) * 0x32;
    g_world_location_entry_enabled = 0;
    do
    {
      do
      {
        g_world_player_x = internal_rand(0x40) * 0x20 + 0x10;
        g_world_player_y = internal_rand(0x40) * 0x20 + 0x10;
        s.tile_mask = GetWorldTileMagicMask(GetWorldTileType(g_world_player_x / 0x20, g_world_player_y / 0x20));
      } while ((g_deck_color_bitmap & s.tile_mask) == 0);
    } while ((GetWorldMapPixelFlags(g_world_player_x / 0x20, g_world_player_y / 0x20) & 0x10) != 0);

    SaveGameToSlot(3);
    break;
  case 1:
    g_skip_new_game_state_initialization = 0;
    load_selected_duel_save_slot(RunLoadGameMenu());
    break;

  case 2:
    g_skip_new_game_state_initialization = 0;
    load_selected_duel_save_slot(3);
    break;

  case 3:
    break;

  case 4:
    g_adventure_world_exit_requested = 1;
    return 0;
  }

  for (s.loop_index = 0; s.loop_index < 0x80; s.loop_index = s.loop_index + 1)
  {
    if (g_town_slots[s.loop_index].location_type == 5)
    {
      s.tile_mask = single_color_test_bit_to_color_t(GetWorldTileMagicMask(GetWorldTileType(g_town_slots[s.loop_index].world_x, g_town_slots[s.loop_index].world_y))) - 1;
      g_world_magic_town_flags[s.tile_mask] = 1;
    }
  }

  if (s.proceed_to_main_loop == 0)
  {
    s.loop_index = LoadStatWinDllExports();
    if (s.initialized_world == 0)
    {
      setup_shared_startup();
    }

    LoadOpeningMenuSpriteResources();
    InitializeMainMenuAndWorldMagicChoiceControls();
    UpdateWorldMagicUnlockProgress();
  }

  if (s.game_setup_mode != 0)
  {
    RebuildDeckEntriesByCardGroup();
  }

  LoadPcxIntoPageNoPalette("advfac64.pic");
  RefreshAdventureInterfaceLayout();
  g_adventure_demo_idle_ticks = 0;
  g_deck_active_card_count = g_deck_total_card_count = 0;
  for (s.loop_index = 0; s.loop_index < 500; s.loop_index = s.loop_index + 1)
  {
    if (deck[s.loop_index] != -1)
    {
      g_deck_total_card_count = g_deck_total_card_count + 1;
      if ((deck[s.loop_index] & 0x4000) == 0)
      {
        g_deck_active_card_count = g_deck_active_card_count + 1;
      }
    }
  }

  s.proceed_to_main_loop = 1;

  // Jumpbufs for exiting the game
  setjmp(g_adventure_session_restart_jump_buffer);
  setjmp(g_adventure_world_exit_jump_buffer);

  if (g_adventure_world_exit_requested != 0)
  {
  }
  else
  {

#ifdef _DEBUG
    // This normally only runs on new world creation which is annoying for debugging.
    g_world_scene_reveal_effect_pending = 1;
#endif

#ifdef MODERN_FIXES
    s.last_tick = 0;
    do
    {
#else
    do
    {
      s.last_tick = 0;
#endif
      ConsumeUiTickCount();
      RenderAdventureWorldScene(g_world_player_x, g_world_player_y, g_world_scene_force_redraw);
      g_world_scene_force_redraw = 0;

      while (clock() - s.last_tick < 0x3c)
      {
      }
      s.last_tick = clock();

      if ((g_mouse_button_down_mask & 2U) != 0)
      {
        RunRightClickMenuAndQueueInput();
      }
      QueuePendingMenuActionInput();

      UpdateAdventureWorldInputAndMovement();
      UpdateWorldLairAndMonsterSlots();

      TickWorldMagicSlotTimers();
      UpdateMouseSnapshot();
      UpdateMenuControlSelection(g_mouse_x_snapshot, g_mouse_y_snapshot, g_mouse_button_down_mask);
      g_monster_timer = g_monster_timer + 1;
      ClearInputAndWaitForMouseRelease();
      ClearQueuedKeyInput();
      setjmp(g_adventure_world_exit_jump_buffer);
    } while (g_adventure_world_exit_requested == 0);
  }

  AnimatePaletteToColor(0, g_default_palette_fade_steps);
  UnloadStatWinDllExports();
  ShutdownSharedStartup();
}

#ifdef _DEBUG
void RunDebugEncounterLairMenu(void)
{
  int lair_index;
  int lair_color;

  strcpy(g_ui_message_buffer, "Encounter Lair\n");
  for (lair_index = 0; lair_index < 0x13; lair_index = lair_index + 1)
  {
    strcat(g_ui_message_buffer, " ");
    strcat(g_ui_message_buffer, gs_lair_names_0077c020[lair_index]);
    strcat(g_ui_message_buffer, "\n");
  }

  lair_index = RunTextMenuAt(g_ui_message_buffer, ScaleUiCoordinate(0x50), ScaleUiCoordinate(0x40));
  if (lair_index != -1)
  {
    if (lair_index < 5)
    {
      lair_color = lair_index + 1;
    }
    else
    {
      lair_color = g_selected_wizard_color;
      if ((lair_color < 1) || (lair_color > 5))
      {
        lair_color = 1;
      }
    }
    g_debug_lair_event_type = lair_index;
    RunLairExplorationEvent(lair_color);
    g_debug_lair_event_type = -1;
  }
}

void RunDebugToggleWorldMagicMenu(void)
{
  int loaded_count;
  int menu_selection;
  int world_magic_index;
  char *world_magic_name;

  loaded_count = LoadTextSectionLines("ADVstrings.txt", "WORLDMAGIC_NAMES");
  if (loaded_count < 0)
  {
    loaded_count = 0;
  }
  if (loaded_count > 0xc)
  {
    loaded_count = 0xc;
  }

  do
  {
    strcpy(g_ui_message_buffer, "Toggle World Magic\n");
    for (world_magic_index = 0; world_magic_index < 0xc; world_magic_index = world_magic_index + 1)
    {
      if (world_magic_index < loaded_count)
      {
        world_magic_name = g_text_lines[world_magic_index];
      }
      else
      {
        world_magic_name = gs_worldmagic_names_00780660[world_magic_index];
      }
      sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), " %s [%s]\n", world_magic_name,
              ((g_world_magic_bitmap & (1 << (unsigned char)world_magic_index)) != 0) ? "ON" : "OFF");
    }

    menu_selection = RunTextMenuAt(g_ui_message_buffer, ScaleUiCoordinate(0x50), ScaleUiCoordinate(0x40));
    if (menu_selection != -1)
    {
      if ((g_world_magic_bitmap & (1 << (unsigned char)menu_selection)) != 0)
      {
        g_world_magic_bitmap = g_world_magic_bitmap & ~(1 << (unsigned char)menu_selection);
      }
      else
      {
        g_world_magic_bitmap = g_world_magic_bitmap | (1 << (unsigned char)menu_selection);
        Scards[menu_selection].worldmagic_city = 0;
      }
    }
  } while (menu_selection != -1);
}

void RunDebugEncounterCreatureMenu(void)
{
  struct
  {
    int menu_selection;
    int creature_index;
    int tier;
    int tier_count;
    char unique_tiers[0x100];
    int submenu_indices[0x39];
    int submenu_count;
    int slot_index;
    int monster_color;
  } s;

  memset(s.unique_tiers, 0, sizeof(s.unique_tiers));
  for (s.creature_index = 1; s.creature_index < (int)gs_creature_name_count_00593934; s.creature_index++)
  {
    s.tier = (int)g_shandalar_monster_definitions[s.creature_index].tier;
    if (s.tier != 0 && s.tier < 0x100)
    {
      s.unique_tiers[s.tier] = 1;
    }
  }

  strcpy(g_ui_message_buffer, "DEBUG - Creature Tier\n");
  for (s.tier = 1; s.tier < 0x100; s.tier++)
  {
    if (s.unique_tiers[s.tier])
    {
      sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), " Tier %d\n", s.tier);
    }
  }

  s.menu_selection = RunTextMenuAt(g_ui_message_buffer, ScaleUiCoordinate(0x50), ScaleUiCoordinate(0x40));
  if (s.menu_selection == -1)
  {
    return;
  }

  s.tier_count = 0;
  for (s.tier = 1; s.tier < 0x100; s.tier++)
  {
    if (s.unique_tiers[s.tier])
    {
      if (s.tier_count == s.menu_selection)
      {
        break;
      }
      s.tier_count++;
    }
  }

  if (s.tier >= 0x100)
  {
    return;
  }

  s.submenu_count = 0;
  strcpy(g_ui_message_buffer, "DEBUG - Creature\n");
  for (s.creature_index = 1; s.creature_index < (int)gs_creature_name_count_00593934; s.creature_index++)
  {
    if ((int)g_shandalar_monster_definitions[s.creature_index].tier == s.tier)
    {
      s.submenu_indices[s.submenu_count] = s.creature_index;
      s.submenu_count++;
      sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), " %s%s\n",
              g_shandalar_monster_definitions[s.creature_index].article,
              g_shandalar_monster_definitions[s.creature_index].name);
    }
  }

  s.menu_selection = RunTextMenuAt(g_ui_message_buffer, ScaleUiCoordinate(0x50), ScaleUiCoordinate(0x40));
  if (s.menu_selection == -1 || s.menu_selection >= s.submenu_count)
  {
    return;
  }

  s.creature_index = s.submenu_indices[s.menu_selection];
  for (s.slot_index = 0; s.slot_index < 8; s.slot_index++)
  {
    if (g_lair_or_monster_slots[s.slot_index].entry_type == SHANDALAR_ENTRY_NONE)
    {
      break;
    }
  }

  if (s.slot_index >= 8)
  {
    return;
  }

  s.monster_color =
      (int)single_color_test_bit_to_color_t((int)g_shandalar_monster_definitions[s.creature_index].color_mask);
  if (s.monster_color == 0)
  {
    s.monster_color = g_selected_wizard_color;
    if (s.monster_color < 1 || s.monster_color > 5)
    {
      s.monster_color = 1;
    }
  }

  g_lair_or_monster_slots[s.slot_index].entry_type = (ShandalarEntryType)s.creature_index;
  g_lair_or_monster_slots[s.slot_index].world_x = g_world_player_x;
  g_lair_or_monster_slots[s.slot_index].world_y = g_world_player_y;
  g_lair_or_monster_slots[s.slot_index].color = s.monster_color;

  RunWorldLairMonsterEncounter(s.slot_index, s.monster_color);

  g_lair_or_monster_slots[s.slot_index].entry_type = SHANDALAR_ENTRY_NONE;
  RefreshAdventureInterfaceLayout();
}

void RunDebugRightClickMenu(void)
{
  int menu_selection;

  sprintf(g_ui_message_buffer, "DEBUG\n Encounter Lair\n Encounter Creature\n Toggle World Magic\n Reveal All World Info [%s]\n",
          (g_reveal_all_world_info != 0) ? "ON" : "OFF");
  menu_selection = RunTextMenuAt(g_ui_message_buffer, ScaleUiCoordinate(0x50), ScaleUiCoordinate(0x40));
  switch (menu_selection)
  {
  case 0:
    RunDebugEncounterLairMenu();
    break;
  case 1:
    RunDebugEncounterCreatureMenu();
    break;
  case 2:
    RunDebugToggleWorldMagicMenu();
    break;
  case 3:
    // Shows entire map and enables map screen click to teleport
    g_reveal_all_world_info = (g_reveal_all_world_info == 0);
    break;
  }
}
#endif

// FUNCTION: SHANDALAR 0x0046e6f0
DWORD AdventureWorkerThread(LPVOID unused)
{
  struct
  {
    void *page;
    int i;
    int max_pages;
  } s;

  (void)unused;

  g_advbuttons_ini_file = fopen("advButtons.txt", "rt");
  strcpy(g_ini_string_scratch, "");
  g_done_text_table_entry = LoadIniEscapedStringTable(g_advbuttons_ini_file, "done", g_ini_string_scratch, 0)[0];

  IgnoreFontConfigLoadResult(LoadFontConfigIfPresent("misc.exe", (char *)0));
  IgnoreFontConfigLoadResult(LoadFontConfigIfPresent("mgraphic.exe", "fonts.cv"));
  IgnoreFontConfigLoadResult(LoadFontConfigIfPresent("nsound.cvl", (char *)0));

  if (global_screen_width == 0x280)
  {
    LoadSystemFont(1, 0xb, "tt0300m_.ttf", "MPZurich Cn BT", 400, 0);
    LoadSystemFont(2, 9, "tt0298m_.ttf", "MPZurich Cn BT", 400, 0);
    LoadSystemFont(4, ScaleUiCoordinate(10), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
    LoadSystemFont(5, ScaleUiCoordinate(0x14), "tt0127m_.ttf", "Benguiat Bk BT", 700, 1);
    LoadSystemFont(6, ScaleUiCoordinate(0x1c), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
    LoadSystemFont(7, ScaleUiCoordinate(0xb), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
  }
  else if (global_screen_width == 800)
  {
    LoadSystemFont(1, ScaleUiCoordinate(8), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
    LoadSystemFont(2, 9, "tt0298m_.ttf", "MPZurich Cn BT", 400, 0);
    LoadSystemFont(4, ScaleUiCoordinate(10), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
    LoadSystemFont(5, ScaleUiCoordinate(0x14), "tt0127m_.ttf", "Benguiat Bk BT", 700, 1);
    LoadSystemFont(6, ScaleUiCoordinate(0x1c), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
    LoadSystemFont(7, ScaleUiCoordinate(0xb), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
  }
  else if (global_screen_width == 0x400)
  {
    LoadSystemFont(1, ScaleUiCoordinate(9), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
    LoadSystemFont(2, 10, "tt0298m_.ttf", "MPZurich Cn BT", 400, 0);
    LoadSystemFont(4, ScaleUiCoordinate(0xb), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
    LoadSystemFont(5, ScaleUiCoordinate(0x14), "tt0127m_.ttf", "Benguiat Bk BT", 700, 1);
    LoadSystemFont(6, ScaleUiCoordinate(0x1c), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
    LoadSystemFont(7, ScaleUiCoordinate(0xb), "tt0530m_.ttf", "Benguiat BkCn BT", 100, 0);
  }

  GetPrimaryPage(0);
  ReadPalette("todpal.tr", (char *)0);

  s.max_pages = 3;
  for (s.i = 0; s.i < s.max_pages; s.i++)
  {
    if ((s.i == 1) && (g_graphics_pages[0]->width <= 0x400))
    {
      s.page = CreateGraphicsPage(s.i, 0x400, 800, 8);
    }
    else
    {
      s.page = CreateGraphicsPage(s.i, global_screen_width, global_screen_height + 1, 8);
    }
    SetGraphicsPage(s.i, s.page);
  }

  s.page = CreateGraphicsPage(5, ScaleUiCoordinate(0x40), ScaleUiCoordinate(0x148), 8);
  SetGraphicsPage(5, s.page);

  s.page = CreateGraphicsPage(3, ScaleUiCoordinate(0x280),
                              (ScaleUiCoordinate(0x1e0) - ScaleUiCoordinate(0x148)) + 3, 8);
  SetGraphicsPage(3, s.page);

  g_page3_window_bounds->max_x = ScaleUiCoordinate(0x280);
  g_page2_window_bounds->max_x = g_page3_window_bounds->max_x;
  g_page1_window_bounds->max_x = g_page2_window_bounds->max_x;
  g_page0_window_bounds->max_x = g_page1_window_bounds->max_x;
  g_page2_window_bounds->max_x = ScaleUiCoordinate(0x1e0);
  g_page1_window_bounds->max_y = g_page2_window_bounds->max_x;
  g_page0_window_bounds->max_y = g_page1_window_bounds->max_y;
  g_page3_window_bounds->max_y = ScaleUiCoordinate(0x1e0) - ScaleUiCoordinate(0x148);
  g_page5_window_bounds->max_y = ScaleUiCoordinate(0x148);
  g_page5_window_bounds->max_x = ScaleUiCoordinate(0x40);
  g_page0_window_bounds->font_slot = 1;

  PresentGraphicsPage(0);
  g_legacy_mouse_active = InitializeLegacyMouseStub();
  do
  {
    RunAdventureSession();
  } while (g_adventure_world_exit_requested == 0);

  UnloadFontSlot(5);
  if (g_legacy_mouse_active != 0)
  {
    ShutdownLegacyMouseStub();
  }

  PostMessageA(g_main_window_hwnd, 0x10, 0, 0);
}

// FUNCTION: SHANDALAR 0x004ce61a
LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  switch (msg)
  {
  case 0x10101010:
    update_snd();
    return 0;
  case 0xf:
    break;
  case 0x200:
    g_mouse_x = (int)((unsigned int)lparam & 0xffff);
    g_mouse_y = (int)(((unsigned int)lparam >> 0x10) & 0xffff);
    break;
  case 0x201:
    g_mouse_button_down_mask = 1;
    g_mouse_x = (int)((unsigned int)lparam & 0xffff);
    g_mouse_y = (int)(((unsigned int)lparam >> 0x10) & 0xffff);
    break;
  case 0x204:
    g_mouse_button_down_mask = 2;
    g_mouse_x = (int)((unsigned int)lparam & 0xffff);
    g_mouse_y = (int)(((unsigned int)lparam >> 0x10) & 0xffff);
    break;
  case 0x205:
    g_mouse_button_released_mask |= 1;
    g_mouse_button_down_mask = 0;
    break;
  case 0x202:
    g_mouse_button_released_mask |= 2;
    g_mouse_button_down_mask = 0;
    break;
  case 0x100:
  case 0x104:
    QueueKeyInputFromMessage(wparam, lparam);
    if (wparam != 0x7a)
    {
      return DefWindowProcA(hwnd, msg, wparam, lparam);
    }

    if (g_palette_class_registered == 0)
    {
      RegisterPaletteClass(g_app_instance);
      g_palette_class_registered = 1;
    }

    g_palette_window_hwnd = FindWindowExA((HWND)0, (HWND)0, "ShowPaletteClass", "Current Palette");
    if (g_palette_window_hwnd != (HWND)0)
    {
      BringWindowToTop(g_palette_window_hwnd);
    }
    else
    {
      g_palette_window_hwnd = CreatePalettePopupWindow(g_app_instance, (void *)0);
      if (g_palette_window_hwnd == (HWND)0)
      {
        return 0;
      }
      ShowWindow(g_palette_window_hwnd, 5);
    }

    UpdateWindow(g_palette_window_hwnd);
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  case 2:
    sound_close();
    PostQuitMessage(0);
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }

  return DefWindowProcA(hwnd, msg, wparam, lparam);
}

/*
 * Main entrypoint for shandalar.exe.
 * Decompilation/matching work will replace/refine this as we recurse into callees.
 */
// FUNCTION: SHANDALAR 0x004cdd70
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int nShowCmd)
{
  struct
  {
    char *slash;
    char exe_cmd[264];
    HDC screen_hdc;
    DWORD thread_id;
    int horzres;
    int resolution_option_matched;
    char game_dir[256];
    DWORD bytes_per_sector;
    DWORD total_number_of_clusters;
    DWORD sectors_per_cluster;
    int disk_free_bytes;
    DWORD number_of_free_clusters;
    char module_path[256];
    HWND existing_main;
    MSG msg;
    int show_shell_on_exit;
    char cmd_copy[100];
    WNDCLASSA wndclass;
    HWND main_hwnd;
    int pad;
    HWND magic_shell_hwnd;
  } s;

  (void)prevInstance;

  s.show_shell_on_exit = 0;
  strcpy(s.cmd_copy, cmdLine);
  g_shared_startup_lock_initialized = 0;

  s.existing_main = FindWindowA("ShandalarMainClass", (LPCSTR)0);
  if (s.existing_main != (HWND)0)
  {
    ShowWindow(s.existing_main, 9);
    SetForegroundWindow(s.existing_main);
    return 0;
  }

  s.magic_shell_hwnd = FindWindowA("Magic Shell", (LPCSTR)0);
  if (s.magic_shell_hwnd != (HWND)0 && _strnicmp(cmdLine, "/MTGshell", 9) != 0)
  {
    PostMessageA(s.magic_shell_hwnd, 0x400, 2, 0);
    return 0;
  }

  if (InitLicenseSecretsFromRegistry() != 0)
  {
    return 0;
  }

  GetWindowsDirectoryA(s.module_path, 0x100);
  s.module_path[3] = 0;
  GetDiskFreeSpaceA(s.module_path, &s.sectors_per_cluster, &s.bytes_per_sector, &s.number_of_free_clusters,
                    &s.total_number_of_clusters);

  s.disk_free_bytes = s.number_of_free_clusters * s.sectors_per_cluster * s.bytes_per_sector;
  if (s.disk_free_bytes / 0x20 / 0x400 / 0x400 == 0)
  {
    MessageBoxA((HWND)0, "We have determined that you may not have enough free swap file space to play Magic: The Gathering reliably.\nThis is just a warning, and you will be able to continue from here, but you may want to free space on your windows drive before you play.", "Low Memory/Swap File Space Warning",
                0x30);
  }

  srand(GetTickCount());
  set_global_base_directory(global_base_directory);
  _chdir(global_base_directory);

  if (LoadAdvStringsFile("AdvStrings.txt") == 0)
  {
    MessageBoxA((HWND)0, "Couldn't load the strings from the AdvStrings.TXT file", (LPCSTR)0, 0x1010);
    return 0;
  }

  if (LoadAdvBlocksFile("AdvBlocks.txt") == 0)
  {
    MessageBoxA((HWND)0, "Couldn't load the strings from the AdvBlocks.TXT file", (LPCSTR)0, 0x1010);
    return 0;
  }

  InitializeDuelUiGlobalIds();

  /* Switch to executable directory */
#ifndef _DEBUG
  strcpy(s.game_dir, (*__p___argv())[0]);
  s.slash = strrchr(s.game_dir, '\\');
  *s.slash = '\0';
  _chdir(s.game_dir);
#endif

  g_frontbuffer_direct_blit_enabled = 0;
  g_app_instance = hInstance;

  s.wndclass.style = 0x23;
  s.wndclass.lpfnWndProc = MainWindowProc;
  s.wndclass.cbClsExtra = 0;
  s.wndclass.cbWndExtra = 0;
  s.wndclass.hInstance = hInstance;
  s.wndclass.hIcon = LoadIconA(g_app_instance, (LPCSTR)0x65);
  s.wndclass.hCursor = LoadCursorA((HINSTANCE)0, (LPCSTR)0x7f00);
  s.wndclass.hbrBackground = GetStockObject(4);
  s.wndclass.lpszMenuName = (LPCSTR)0;
  s.wndclass.lpszClassName = "ShandalarMainClass";

  if ((RegisterClassA(&s.wndclass) & 0xffff) == 0)
  {
    MessageBoxA((HWND)0, "Couldn't register the classes", (LPCSTR)0, 0x1010);
    return 0;
  }

  atexit(RestoreDisplayResolution);

  if (0x400 < GetDeviceCaps(GetDC((HWND)0), 8))
  {
    ChangeDisplayResolution(0x400, 0x300);
  }

cmd_parse:
  if (cmdLine[0] != 0 && cmdLine[1] != 0)
  {
    s.resolution_option_matched = 0;
    if (cmdLine[1] == '6')
    {
      global_screen_width = 0x280;
      global_screen_height = 0x1e0;
      PTR_s_advinter800_pic_00589de8 = "advinter.pic";
      CreateGraphicsPage(0, global_screen_width, global_screen_height, -1);
      s.resolution_option_matched = 1;
      s.show_shell_on_exit = 1;
    }
    if (cmdLine[1] == '8')
    {
      global_screen_width = 800;
      global_screen_height = 600;
      PTR_s_advinter800_pic_00589de8 = "advinter800.pic";
      CreateGraphicsPage(0, global_screen_width, global_screen_height, -1);
      s.resolution_option_matched = 1;
      s.show_shell_on_exit = 1;
    }
    if (cmdLine[1] == '1')
    {
      global_screen_width = 0x400;
      global_screen_height = 0x300;
      PTR_s_advinter800_pic_00589de8 = "advinter1024.pic";
      CreateGraphicsPage(0, global_screen_width, global_screen_height, -1);
      s.resolution_option_matched = 1;
      s.show_shell_on_exit = 1;
    }

    if (s.resolution_option_matched == 0)
    {
      cmdLine[0] = 0;
      goto cmd_parse;
    }
    g_page0_window_bounds->max_x = global_screen_width - 1;
    g_page0_window_bounds->max_y = global_screen_height - 1;
  }
  else
  {
    s.horzres = GetDeviceCaps(GetDC((HWND)0), 8);
    switch (s.horzres)
    {
    case 0x280:
      global_screen_width = 0x280;
      global_screen_height = 0x1e0;
      PTR_s_advinter800_pic_00589de8 = "advinter.pic";
      break;
    case 0x320:
      global_screen_width = 0x320;
      global_screen_height = 0x258;
      PTR_s_advinter800_pic_00589de8 = "advinter800.pic";
      break;
#ifdef MODERN_FIXES
    default:
#else
    case 0x400:
#endif
      global_screen_width = 0x400;
      global_screen_height = 0x300;
      PTR_s_advinter800_pic_00589de8 = "advinter1024.pic";
      break;
    }
    g_page0_window_bounds->max_x = global_screen_width - 1;
    g_page0_window_bounds->max_y = global_screen_height - 1;
  }
#ifdef _DEBUG
  // Annoying trying to debug with window always on top
  s.main_hwnd = CreateWindowExA(0, "ShandalarMainClass", "Magic: Shandalar", 0x80000000, 0, 0,
                                global_screen_width, global_screen_height, (HWND)0, (HMENU)0, hInstance, (LPVOID)0);
#else
  s.main_hwnd = CreateWindowExA(8, "ShandalarMainClass", "Magic: Shandalar", 0x80000000, 0, 0,
                                global_screen_width, global_screen_height, (HWND)0, (HMENU)0, hInstance, (LPVOID)0);
#endif
  g_main_window_hwnd = s.main_hwnd;
  ShowWindow(s.main_hwnd, nShowCmd);
  global_main_hdc = GetDC(s.main_hwnd);

  if (s.show_shell_on_exit)
  {
    g_graphics_pages[0]->hTempDC = global_main_hdc;
    SelectPalette(g_graphics_pages[0]->hTempDC, g_graphics_pages[0]->hPalette, FALSE);
    RealizePalette(g_graphics_pages[0]->hTempDC);
    SetStretchBltMode(g_graphics_pages[0]->hTempDC, 3);
  }

  InitializeSoundPresenceState();
  init_sound_dll(s.main_hwnd, 0, 1);

  if (g_skip_world_sfx_preload == 0)
  {
    PreloadWorldAmbientSounds();
  }

  timeBeginPeriod(g_timer_resolution_ms);
  g_timer_event_handle = timeSetEvent(g_timer_period_ms, g_timer_resolution_ms, (LPTIMECALLBACK)UiTimerTickCallback, 0, 1);
  if (g_timer_event_handle == 0)
  {
    assert(0, "D:\\Newmagic\\multiplayer\\sid\\Test.c", 0x15a, "Timer failed to initialize!\n");
  }
  atexit(ShutdownUiTimer);
  assert(g_timer_event_handle != -1, "D:\\Newmagic\\multiplayer\\sid\\Test.c", 0x15c,
         "Could not start timer\n");

  SetSystemPaletteUse(global_main_hdc, 2);
  atexit(RestoreSystemPaletteAtExit);

  g_main_thread_handle = GetCurrentThread();
  DuplicateHandle(GetCurrentProcess(), g_main_thread_handle, GetCurrentProcess(), &g_main_thread_handle, 0x1f03ff, FALSE, 0);

  if (g_shared_startup_lock_initialized == 0)
  {
    InitializeCriticalSection(&g_shared_startup_lock);
    g_shared_startup_lock_initialized = 1;
  }

  g_loader_thread_handle = CreateThread((LPSECURITY_ATTRIBUTES)0, 0x2000,
                                        (LPTHREAD_START_ROUTINE)AdventureWorkerThread,
                                        (LPVOID)0, 0, &s.thread_id);

  while (GetMessageA(&s.msg, (HWND)0, 0, 0))
  {
    TranslateMessage(&s.msg);
    DispatchMessageA(&s.msg);
  }

  if (g_shared_startup_lock_initialized != 0)
  {
    DeleteCriticalSection(&g_shared_startup_lock);
    g_shared_startup_lock_initialized = 0;
  }

  FreeAdvblocksFileBuffer();
  ChangeDisplayResolution(0, 0);

  s.screen_hdc = GetDC((HWND)0);
  SelectPalette(s.screen_hdc, GetStockObject(0xf), FALSE);
  SetSystemPaletteUse(s.screen_hdc, 1);
  RealizePalette(s.screen_hdc);
  ReleaseDC((HWND)0, s.screen_hdc);

  if (_strnicmp(s.cmd_copy, "/MTGshell", 8) == 0)
  {
    strcpy(s.exe_cmd, global_base_directory);
    strcat(s.exe_cmd, "\\Magic.exe");
    strcat(s.exe_cmd, " /start3,1");
    WinExec(s.exe_cmd, 5);
  }

  return (int)s.msg.wParam;
}
