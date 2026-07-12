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
#include "magic/src/global_duel_ui_ids.h"
#include "magic/src/global_state.h"
#include "magic/src/global_strings.h"
#include "magic/src/global_other.h"
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
extern DIBSurface *g_graphics_pages[10];

typedef struct
{
  int first;
  int second;
} HintPair;

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

typedef struct
{
  EncodedImage *frame[4][9];
  EncodedImage *icon_rows[4][4];
} DialogBoxSpriteBank;

int HandleMainMenuButtonControlEvent(AdvMenuControl *control_ptr, int event_type);
int HandlePortraitMainMenuControlEvent(AdvMenuControl *control_ptr, int event_type);
int HandleWorldMagicChoiceControlEvent(AdvMenuControl *control_ptr, int event_type);
int ActivateWorldMagicChoiceControl(AdvMenuControl *control);
int ActivateMainMenuControl(AdvMenuControl *control);

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
// GLOBAL: SHANDALAR 0x005863c8
WorldMagicSlotTimer g_world_magic_slot_timers[0xc] = {
    {0, 0, 114, 1000},
    {0, 0, 62, 700},
    {0, 0, 24, 200},
    {0, 0, 133, 800},
    {0, 0, 25, 1500},
    {0, 0, 121, 400},
    {0, 0, 285, 500},
    {0, 0, 145, 600},
    {0, 0, 26, 300},
    {0, 0, 205, 600},
    {0, 0, 28, 1200},
    {0, 0, 0x03020100, 0x07060004},
};
// GLOBAL: SHANDALAR 0x00589de8
char *PTR_s_advinter800_pic_00589de8 = "advinter800.pic";
// GLOBAL: SHANDALAR 0x00583290
FacemakerWindowBounds DAT_00583290 = {0, 0, 0, 0x27f, 0x1df, 1, 0x0f, 4, 0};
// GLOBAL: SHANDALAR 0x005832b4
FacemakerWindowBounds *PTR_DAT_005832b4 = &DAT_00583290;
// GLOBAL: SHANDALAR 0x005832b8
FacemakerWindowBounds DAT_005832b8 = {1, 0, 0, 0x320, 0x258, 1, 0x0f, 4, 0};
// GLOBAL: SHANDALAR 0x005832dc
FacemakerWindowBounds *PTR_DAT_005832dc = &DAT_005832b8;
// GLOBAL: SHANDALAR 0x005832e0
FacemakerWindowBounds DAT_005832e0 = {2, 0, 0, 0x27f, 0x1df, 1, 0x0f, 4, 0};
// GLOBAL: SHANDALAR 0x00583304
FacemakerWindowBounds *PTR_DAT_00583304 = &DAT_005832e0;
// GLOBAL: SHANDALAR 0x00583308
FacemakerWindowBounds DAT_00583308 = {3, 0, 0, 0x27f, 0x1df, 1, 0x0f, 4, 0};
// GLOBAL: SHANDALAR 0x0058332c
FacemakerWindowBounds *PTR_DAT_0058332c = &DAT_00583308;
// GLOBAL: SHANDALAR 0x00583330
FacemakerWindowBounds DAT_00583330 = {5, 0, 0, 0x27f, 0x1df, 1, 0x0f, 4, 0};
// GLOBAL: SHANDALAR 0x00583354
FacemakerWindowBounds *PTR_DAT_00583354 = &DAT_00583330;
// GLOBAL: SHANDALAR 0x00748420
HWND g_main_window_hwnd;
// GLOBAL: SHANDALAR 0x00939160
HINSTANCE g_app_instance;
// GLOBAL: SHANDALAR 0x005a6198
long DAT_005a6198[0x4e2];

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
int _DAT_0074840c;
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
// GLOBAL: SHANDALAR 0x0078cf10
char g_ui_message_buffer[0x1000];
// GLOBAL: SHANDALAR 0x0078df10
char g_ini_string_scratch[0x28];
// GLOBAL: SHANDALAR 0x0078df38
int g_done_text_table_entry;
// GLOBAL: SHANDALAR 0x00586494
int DAT_00586494 = 0;
// GLOBAL: SHANDALAR 0x00586498
int DAT_00586498 = 0;
// GLOBAL: SHANDALAR 0x009300f0
int g_adventure_world_exit_requested;
// GLOBAL: SHANDALAR 0x007486d0
EncodedImage *g_ttsprite_special_sprite_a;
// GLOBAL: SHANDALAR 0x007486e0
EncodedImage *g_world_magic_avatar_sprites[5];
// GLOBAL: SHANDALAR 0x00781728
EncodedImage *g_face_preview_sprite_selected;
// GLOBAL: SHANDALAR 0x0078172c
EncodedImage *g_face_preview_sprite_group[6];
// GLOBAL: SHANDALAR 0x0077f1c8
unsigned int g_load_menu_valid_slot_mask_0077f1c8;
// GLOBAL: SHANDALAR 0x0073e9dc
int g_world_scene_reveal_effect_pending;
// GLOBAL: SHANDALAR 0x00591214
int DAT_00591214 = 1;
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
int DAT_006696f4;
// GLOBAL: SHANDALAR 0x006696f8
int g_advfac64_load_state;
// GLOBAL: SHANDALAR 0x006696fc
int DAT_006696fc;
// GLOBAL: SHANDALAR 0x00669700
int DAT_00669700;
// GLOBAL: SHANDALAR 0x00669710
int DAT_00669710;
// GLOBAL: SHANDALAR 0x00789938
int DAT_00789938;
// GLOBAL: SHANDALAR 0x0078df68
int DAT_0078df68;
// GLOBAL: SHANDALAR 0x0097db40
long DAT_0097db40[0x100];
// GLOBAL: SHANDALAR 0x0097df40
int DAT_0097df40[0x100];
// GLOBAL: SHANDALAR 0x0097e340
char DAT_0097e340[0x110];
// GLOBAL: SHANDALAR 0x0097e450
HintPair DAT_0097e450[0x100];
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
// GLOBAL: SHANDALAR 0x0058c038
char g_save_file_path[] = "D:MAGIC0.SVE";
// GLOBAL: SHANDALAR 0x0058a86c
char s_advfac64_pic_path_0058a86c[] = "advfac64.pic";
// GLOBAL: SHANDALAR 0x0058a87c
char s_todpal_tr_path_0058a87c[] = "todpal.tr";
// GLOBAL: SHANDALAR 0x00591684
char s_advfac64_pic_path_00591684[] = "advfac64.pic";
// GLOBAL: SHANDALAR 0x0058c13c
int g_save_path_needs_init = -1;
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
char g_animated_noise_grid[0x2c0];
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
EncodedImage *DAT_00748f10;
// GLOBAL: SHANDALAR 0x00748f14
EncodedImage *DAT_00748f14;
// GLOBAL: SHANDALAR 0x00748f18
EncodedImage *DAT_00748f18;
// GLOBAL: SHANDALAR 0x00748f1c
EncodedImage *DAT_00748f1c;
// GLOBAL: SHANDALAR 0x00748f20
EncodedImage *DAT_00748f20;
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
EncodedImage *DAT_00749418;
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
OpeningMenuSpriteWorkEntry g_opening_menu_sprite_work_buffer[0x14];
// GLOBAL: SHANDALAR 0x0078df40
EncodedImage *DAT_0078df40[10];
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
int DAT_008c7408[7];
// GLOBAL: SHANDALAR 0x0073e990
jmp_buf DAT_0073e990;
// GLOBAL: SHANDALAR 0x0073e9e0
jmp_buf DAT_0073e9e0;
// GLOBAL: SHANDALAR 0x006527b0
int DAT_006527b0;
// GLOBAL: SHANDALAR 0x00590764
int DAT_00590764 = -1;
// GLOBAL: SHANDALAR 0x00590768
int g_adventure_ui_layout_dirty = 1;
// GLOBAL: SHANDALAR 0x00650f28
int DAT_00650f28;
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
int DAT_005a5fe8[0x20];
// GLOBAL: SHANDALAR 0x005a6068
int g_text_menu_color_base;
// GLOBAL: SHANDALAR 0x005a606c
int g_text_menu_finish_flash;
// GLOBAL: SHANDALAR 0x005a6070
int g_text_menu_line_count;
// GLOBAL: SHANDALAR 0x005a607c
int g_text_menu_hovered_selection;
// GLOBAL: SHANDALAR 0x007483f0
int DAT_007483f0;
// GLOBAL: SHANDALAR 0x007483f4
int _DAT_007483f4;
// GLOBAL: SHANDALAR 0x0078cef0
char DAT_0078cef0[0xc];
// GLOBAL: SHANDALAR 0x00789934
int DAT_00789934;
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
int DAT_005aa414;
// GLOBAL: SHANDALAR 0x005aa62c
int DAT_005aa62c;
// GLOBAL: SHANDALAR 0x005873d4
int DAT_005873d4 = 0;

// GLOBAL: SHANDALAR 0x0074cfe4
int g_selected_save_slot_index;

int InitLicenseSecretsFromRegistry(void);
int LoadAdvStringsFile(const char *filename);
int LoadTextSectionStringTable(const char *filename, const char *section, char **out_table, int max_entries, char *string_buf,
                               char *string_buf_end, char **out_next_buf);
int LoadAdvBlocksFile(const char *filename);
int FindNextTextBlock(char *scan_start, char *scan_end, int *out_block_start, int *out_next_scan);
void ReadCsvFieldByCsvid(char *out, int csvid, int field, char *csv_name);
void *CreateGraphicsPage(int page_number, int width, int height, int bits_per_pixel);
void FUN_00562d03(void);
void FUN_00565faa(void);
void QueueKeyInputFromMessage(WPARAM wparam, LPARAM lparam);
ATOM RegisterPaletteClass(HINSTANCE hinst);
HWND CreatePalettePopupWindow(HINSTANCE hinst, HWND parent_hwnd);
int *LoadIniEscapedStringTable(FILE *ini_file, char *section_name, int unk1, int unk2);
int MeasureMultilineTextWidth(FacemakerWindowBounds *window_bounds, char *text);
int SetFontStyleSize(int font_id, unsigned int style);
void LoadPcxIntoPageNoPalette(char *path);
void ClearGraphicsPageWithPaletteColor(int page_number, int color_index);
void CopyGraphicsRect(FacemakerWindowBounds *src, int src_x, int src_y, unsigned int width, int height,
                      FacemakerWindowBounds *dst, int dst_x, int dst_y);
AdvMenuRect *PushGraphicsClipRect(AdvMenuRect *saved_clip_rect, FacemakerWindowBounds *page, int x, int y, int width, int height);
void BeginSpriteEncodeSession(void);
EncodedImage *EncodeSpriteFromPage(int page_number, int x, int y, int width, int height);
void FinalizeSpriteEncodeSession(void);
int DrawTextFormatted(FacemakerWindowBounds *dst, int text_color, int draw_shadow, int scale_to_screen, int center_x, int center_y, int x,
                      int y, int *format_and_args);
void ReadGraphicsScanline(unsigned int *out_scanline, int page_number, int src_x, int src_y, unsigned int byte_count);
void PlaySoundEffectOnChannel(char *sound_path, int channel, int volume, int pitch_percent, int pan_percent);
void PutGraphicsPixel(FacemakerWindowBounds *window_bounds, int x, int y, int color_index);
unsigned int ReadGraphicsPixel(int page_number, int x, int y);
void BlitGraphicsRect(FacemakerWindowBounds *dst, unsigned int dst_x, int dst_y, unsigned int width, DWORD height,
                      FacemakerWindowBounds *src, int src_x, int src_y);
void DrawEncodedImageResampled(FacemakerWindowBounds *dst, int x, int y, int width, int height, EncodedImage *encoded_image);
void StretchBlitGraphicsRect(FacemakerWindowBounds *dst, int dst_x, int dst_y, int src_w, int src_h,
                             FacemakerWindowBounds *src, int src_x, int src_y, int copy_w, int copy_h);
int FUN_00578c80(int param_1);
int FUN_00578c20(void);
int FUN_00578c30(void);
void *InitializeGraphicsSystemDefaultMode(void);
void SetGraphicsPage(int page_number, void *page);
void PresentGraphicsPage(int page_number);
int ReadPalette(char *palette_text_path, char *palette_binary_path);
int ScaleUiCoordinate(int value);
int LoadFontConfigIfPresent(char *executable_name, char *config_name);
int LoadSystemFont(int font_id, unsigned int point_size, char *font_file, char *font_name, int weight, DWORD italic);
int FUN_0055db50(void);
BOOL UnloadFontSlot(int font_slot);
int FileExists(const char *filename);
char FindDriveWithAsset(char *filename);
char GetSoundAssetDriveLetter(void);
unsigned int LoadSoundWithDriveFallback(char *filename, int channel, int flags);
LONG ChangeDisplayResolution(DWORD width, DWORD height);
void RestoreDisplayResolution(void);
DWORD WINAPI FUN_0046e6f0(LPVOID);
void FUN_00578c70(int param_1, int param_2, int param_3);
char *BuildResolutionSpritePath(char *sprite_filename);
int ReadSpriteEntryPointersWithLimit(EncodedImage **out_entries, char *path, int max_entries);
void FUN_004184d2(void);
void FUN_0041786e(void);
void FUN_0046ed03(void);
void FUN_0046ed33(void);
void ShowMouseCursor(void);
void HideMouseCursor(void);
int ReadSpriteEntryPointers(EncodedImage **out_sprite_entries, char *sprite_path);
void FreeSpriteBlob(void *memory);
void AnimatePaletteToColor(int color_index, int palette_id);
int RunOpeningMenu(void);
int RunDifficultyMenu(void);
int RunColorMenu(void);
int RandomIntLessThan(int param_1);
int RunFacemakerFlow(void);
void InitializeNewGameState(void);
void FUN_004f6d90(void);
void InitializeAnimatedNoiseGrid(void);
int FUN_004f7fb9(int param_1, int param_2);
void PropagatePathConnectivity(void);
int GenerateWorldTownSlots(void);
void GenerateTownConnections(void);
int CreateTownConnectionPath(int start_x, int start_y, int target_x, int target_y);
void FloodFillPathConnectivity(int x, int y, unsigned int depth);
void InitializeCastleDungeonSlots(void);
unsigned int GetWorldTileType(int param_1, int param_2);
unsigned int GetWorldTileMagicMask(unsigned int param_1);
unsigned int FUN_004314ca(int param_1, int param_2);
void SetWorldMapPixelFlags(unsigned int param_1, int param_2, int param_3);
void ClearWorldMapPixelFlags(unsigned int param_1, int param_2, int param_3);
void MarkPathConnection(int x, int y, int direction_index);
int FUN_004f82f2(int param_1, int param_2);
void SaveGameToSlot(int save_slot_index);
int FUN_005031a8(void);
int LoadGameFromSlot(int save_slot_index);
int GetFirstManaColorIndex(int param_1);
void AddJournalEntry(int entry_type, int entry_arg);
int LoadStatWinDllExports(void);
void LoadOpeningMenuSpriteResources(void);
int RenderAdvMenuControlDisabled(AdvMenuControl *control);
int InitializeMainMenuAndWorldMagicChoiceControls(void);
void UpdateWorldMagicUnlockProgress(void);
void RebuildDeckEntriesByCardGroup(void);
void RefreshAdventureInterfaceLayout(void);
int ConsumeUiTickCount(void);
void RenderAdventureWorldScene(int world_x, int world_y, int world_state);
int RunStartupMenuAndQueueInput(void);
int QueuePendingMenuActionInput(void);
void UpdateAdventureWorldInputAndMovement(void);
void UpdateWorldLairAndMonsterSlots(void);
void TickWorldMagicSlotTimers(void);
void UpdateMouseSnapshot(void);
int BeginMenuContext(void);
int ResetMenuContext(int context_index);
int AddMenuControlsToContext(AdvMenuControl *controls, int control_count, int context_index);
int EndMenuContext(void);
int FadeInPaletteFromGray(int gray, int steps);
void DrawFormattedTextShadowedCenterY(FacemakerWindowBounds *window, int color_index, int x, int y, char *format, ...);
void DrawFormattedTextShadowedCentered(FacemakerWindowBounds *window, int color_index, int x, int y, char *format, ...);
void DrawFormattedTextShadowed(FacemakerWindowBounds *window, int color_index, int x, int y, char *format, ...);
void LoadPcxIntoPageOpaque(int page_number, char *path);
int ExportGraphicsPage(int page_number, char *path);
int ExportEncodedImage(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, char *param_7);
unsigned int WaitForInputEventUnlessBlocked(void);
int UpdateMenuControlSelection(int mouse_x, int mouse_y, int allow_activate_on_click);
unsigned int PeekQueuedKeyInput(void);
unsigned int WaitForInputEvent(void);
int ApproximateDistance(int param_1, int param_2);
void ClearInputAndWaitForMouseRelease(void);
void ClearQueuedKeyInput(void);
void UnloadStatWinDllExports(void);
int HasQueuedKeyInput(void);
int PopQueuedKeyInput(void);
int PopNormalizedQueuedKeyInput(void);
void destroy_create_fonts_resources(void);
void FillGraphicsRect(FacemakerWindowBounds *window_bounds, int x, int y, int width, int height, unsigned int color_index);
unsigned int __cdecl save_or_load_ver1(void);
void BuildFacemakerPortraitSprites(FacemakerWindowBounds *page);

void DelayUiTicks(int param_1);
int FUN_0056c705(int param_1);
unsigned int FUN_004bdccc(unsigned int param_1);
int AddRandomStarterDeckCards(unsigned int color_mask, int land_count, int spell_count, int creature_count, int add_rare, int allow_artifact_spells);
int PickRandomCardMatchingTypeAndColor(unsigned int type_mask, unsigned int color_mask);
int FUN_0056c0e5(int param_1, int param_2, int param_3);
int FUN_0056bd9d(unsigned int param_1);
int FUN_0056c5ea(int param_1);
int FUN_004bb1cf(unsigned int param_1);
unsigned int GetGraphicsPixelColorRef(FacemakerWindowBounds *param_1, int param_2, int param_3);
int RunLoadSaveMenu(int param_1);
int FUN_0052280c(void *param_1);
int FUN_0056302b(int param_1);
int ConsumeMouseButtonReleaseMask(void);
int RunTextMenuAt(char *menu_text, int left_x, int top_y);
int RunTextMenuCore(char *menu_text, int clear_input_before_show);
int RunTextMenuAtScaled(char *menu_text, int x_320_scale, unsigned int y_200_scale);
int DrawTextMenu(char *menu_text, int selected_option);
void DrawDialogBoxFrameAutoStyle(int x, int y, int width, int height);
void DrawRectangleBorder(int x, int y, int width, int height, int color_index);
void DrawTiledDialogBoxFrame(int x, int y, int width, int height, int frame_style);
void DrawUiLine(int x1, int y1, int x2, int y2, int color_index);
void DrawTextLineClamped(char *text, int x, int y, int color_index);
void DrawTextLineNoShadow(char *text, int x, int y, int color_index);
void DrawCenteredTextLineClamped(char *text, int center_x, int y, int color_index);
int MeasureTextLineWidth(char *text);
void DrawEncodedImageUiScaled(FacemakerWindowBounds *dst, int x_320, int y_200, EncodedImage *sprite, int width_320, int height_200);
int FUN_004bb458(int param_1);
void PushQueuedKeyInput(int param_1);
int FUN_0055e31f(int param_1, int param_2);
int RenderMenuControlRange(int first_index, int count);
int GetUiTickCount(void);
int FUN_004ece9a(void);
int ClampIntToRange(int value, int min_value, int max_value);
int FUN_004ece40(int param_1);
unsigned int ValidateOrLoadSaveGame(char *save_file_path, int validate_only);
int SaveGameWithMessage(char *save_file_path);
int GetSaveDriveIndex(void);
int LoadGameFromPath(char *save_file_path);
int SaveGameToPath(char *save_file_path);
int GetFontCharWidth(int param_1, char param_2);
int GetFontLineHeight(int param_1);
void DrawGraphicsLine(FacemakerWindowBounds *window_bounds, int x1, int y1, int x2, int y2, int color_index);
int DrawTextLine(FacemakerWindowBounds *window_bounds, int x, int y, char *text);
void DrawEncodedImageUnscaled(FacemakerWindowBounds *dst, int x, int y, EncodedImage *encoded_image);
WPARAM WINAPI DeckBuilderMain(HWND parent_hwnd, int db_flags_1, int db_flags_2);
int LoadTextSectionLines(char *filename, char *section);
int IsKeyInputQueueEmpty(void);
unsigned int WorldRoadTileHasDirection(int tile_x, int tile_y, int direction_index);
int FreeOpeningMenuSpriteWorkEntries(int work_entry_index_a, int work_entry_index_b);
int SignNonZero(int value);
int RestoreAdventureUiPaletteAndFocus(void);
int VisitTownSlot(int town_index);
int RunSaveMenuAndSelectSlot(void);
void *DrawAdventureInterfaceLayout(int force_redraw);
void SetupRandomAiDuelDecks(void);
void EnsureAdvfac64Loaded(int state);
void StartWizardTownSiege(void);
void ResolveWizardTownSiege(void);
void FUN_00562736(int param_1, int param_2, int param_3, int param_4);
void FUN_0056279e(int param_1, int param_2, int param_3);
void FUN_00562835(char *param_1, int param_2);
void UpdateAmbientWizardColorSound(int param_1);
void EnterCastleDungeon(int param_1);
void ShowCityInfoScreen(int param_1);
void ShowDungeonCluesScreen(int unused);
void RunAdventureStatsMenu(void);
void ShowWorldMapScreen(int mode);
void ShowStatsWindow(int mode, int highlight);
void AnalyzeDeckAndMaybeShowReport(int show_ui);
int FUN_004bb040(int world_x, int world_y);
int FUN_00508b89(int world_x, int world_y);
extern int g_world_player_tile_x;
extern int g_world_player_tile_y;
extern char *global_base_txt;
extern HPALETTE global_cart_art_hpalette;
extern HPALETTE g_palette_handle;
extern int Gold;
extern int g_graphics_bpp;
extern RpBitsPalettePacket g_palette_data_words;
extern FontSlot g_font_slots[0x10];
extern int g_key_input_queue_count;
extern unsigned int g_key_input_queue[50];
extern int g_menu_selection_value;
extern int g_color_menu_selection_to_deck_color[5];
extern card_data_t global_cards_data[];
extern DIBSurface *g_facemaker_page4_dib;
extern HBITMAP g_facemaker_page4_bitmap;
extern int g_frontbuffer_direct_blit_enabled;
extern int(__cdecl *g_town_dialog_callback)();

// FUNCTION: SHANDALAR 0x005501fe
int ScaleUiCoordinate(int value)
{
  return (value * global_screen_width) / 0x280;
}

// FUNCTION: SHANDALAR 0x0055db50
int FUN_0055db50(void)
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
  DAT_008cf6d0 = -1;
  unk_00789308 = DAT_008cf6d0;
  InitializeDuelUiGlobalIds();
  for (s.loop_index = 0; s.loop_index < 4; s.loop_index = s.loop_index + 1)
  {
    DAT_007a7d10[s.loop_index] = 8;
  }

  FUN_004184d2();
  FUN_0041786e();
  FUN_00578c70(1, 1, (int)g_ttsprite_special_sprite_a);
  FUN_0046ed03();
  FUN_0046ed33();
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
    DAT_0091c99c = g_selected_wizard_color;
    DAT_0091c9a0 = RandomIntLessThan(3);
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
    BlitGraphicsRect((int *)PTR_DAT_005832dc, 0, 0, global_screen_width, global_screen_height, (int *)PTR_DAT_005832b4, 0, 0);
    DAT_00715f10 = 0;
    if (s.initialized_world == 0)
    {
      setup_shared_startup();
    }
    s.initialized_world = 1;
    PTR_DAT_005832b4->font_slot = 5;
    LoadTextSectionLines("ADVstrings.txt", "STARTUP");
    DrawTextAt((int *)PTR_DAT_005832b4, 0xff, 0x140, 0xbc, text_lines[0]);
    g_world_scene_reveal_effect_pending = 1;
    InitializeNewGameState();
    FUN_004f6d90();
    InitializeCastleDungeonSlots();
    Gold = (5 - g_shandalar_difficulty) * 0x32;
    DAT_00591214 = 0;
    do
    {
      do
      {
        g_world_player_x = RandomIntLessThan(0x40) * 0x20 + 0x10;
        g_world_player_y = RandomIntLessThan(0x40) * 0x20 + 0x10;
        s.tile_mask = GetWorldTileMagicMask(GetWorldTileType(g_world_player_x / 0x20, g_world_player_y / 0x20));
      } while ((g_deck_color_bitmap & s.tile_mask) == 0);
    } while ((FUN_004314ca(g_world_player_x / 0x20, g_world_player_y / 0x20) & 0x10) != 0);

    SaveGameToSlot(3);
    break;
  case 1:
    DAT_00715f10 = 0;
    LoadGameFromSlot(FUN_005031a8());
    break;

  case 2:
    DAT_00715f10 = 0;
    LoadGameFromSlot(3);
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
      s.tile_mask = GetFirstManaColorIndex(GetWorldTileMagicMask(GetWorldTileType(g_town_slots[s.loop_index].world_x, g_town_slots[s.loop_index].world_y))) - 1;
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
  DAT_00669700 = 0;
  DAT_0078df68 = DAT_00789938 = 0;
  for (s.loop_index = 0; s.loop_index < 500; s.loop_index = s.loop_index + 1)
  {
    if (deck[s.loop_index] != -1)
    {
      DAT_00789938 = DAT_00789938 + 1;
      if ((deck[s.loop_index] & 0x4000) == 0)
      {
        DAT_0078df68 = DAT_0078df68 + 1;
      }
    }
  }

  s.proceed_to_main_loop = 1;

  // Jumpbufs for exiting the game
  setjmp(&DAT_0073e990);
  setjmp(&DAT_0073e9e0);

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
      RenderAdventureWorldScene(g_world_player_x, g_world_player_y, DAT_00669710);
      DAT_00669710 = 0;

      while (clock() - s.last_tick < 0x3c)
      {
      }
      s.last_tick = clock();

      if ((g_mouse_button_down_mask & 2U) != 0)
      {
        RunStartupMenuAndQueueInput();
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
      setjmp(&DAT_0073e9e0);
    } while (g_adventure_world_exit_requested == 0);
  }

  AnimatePaletteToColor(0, g_default_palette_fade_steps);
  UnloadStatWinDllExports();
  ShutdownSharedStartup();
}

// FUNCTION: SHANDALAR 0x00412bff
int RunTextMenuAt(char *menu_text, int left_x, int top_y)
{
  g_text_menu_left = left_x;
  g_text_menu_top = top_y;
  g_text_menu_initial_selection = -1;
  return RunTextMenuCore(menu_text, 1);
}

// FUNCTION: SHANDALAR 0x00412bae
int RunTextMenuAtScaled(char *menu_text, int x_320_scale, unsigned int y_200_scale)
{
  return RunTextMenuAt(menu_text, (global_screen_width * x_320_scale) / 0x140,
                       (int)((y_200_scale - (y_200_scale & 1)) * global_screen_height) / 200 + (y_200_scale & 1));
}

// FUNCTION: SHANDALAR 0x00412c37
int RunTextMenuCore(char *menu_text, int clear_input_before_show)
{
  struct
  {
    int selected_menu_entry;
    int key_code;
    int current_menu_entry;
    int next_menu_entry;
    int done;
    int line_height_copy;
    int previous_menu_entry;
    char timer_text[4];
    int line_height;
    int timer_seconds_left;
    int mouse_active;
    int last_timer_seconds;
  } s;

  if ((clear_input_before_show != 0) && (g_text_menu_abort_requested == 0))
  {
    ClearInputAndWaitForMouseRelease();
  }

  s.previous_menu_entry = -1;
  s.selected_menu_entry = s.previous_menu_entry;
  g_text_menu_mouse_active = 0;
  g_text_menu_mouse_released = g_text_menu_mouse_active;
  if (DAT_00586494 == 0)
  {
    if (s.selected_menu_entry == -1)
    {
      if (g_text_menu_ok_mode == 0)
      {
        if (g_text_menu_force_cancel == 0)
        {
          s.selected_menu_entry = 0;
        }
      }
    }
  }
  g_text_menu_finish_flash = 0;
  s.selected_menu_entry = g_text_menu_finish_flash;
  if (g_text_menu_initial_selection != -1)
  {
    s.selected_menu_entry = g_text_menu_initial_selection;
  }

  g_text_menu_first_option_line = -1;
  g_text_menu_needs_layout = 1;
  g_text_menu_hovered_selection = -1;
  g_text_menu_line_height = GetFontLineHeight(PTR_DAT_005832b4->font_slot);
  s.current_menu_entry = 3;
  s.line_height = 0x16;
  g_text_menu_color_base = 0xff;
  if (DAT_00789934 == 0)
  {
    s.line_height = -1;
  }
  s.line_height_copy = g_text_menu_line_height;

  s.mouse_active = 0;
  s.done = s.mouse_active;
  FUN_0046ed33();
  DrawTextMenu(menu_text, g_text_menu_initial_selection);
  FUN_0046ed03();

  if (g_text_menu_abort_requested != 0)
  {
    g_text_menu_abort_requested = 0;
    return -1;
  }

  if (g_text_menu_timeout_seconds != -1)
  {
    ConsumeUiTickCount();
    s.last_timer_seconds = -1;
  }

  do
  {
    g_text_menu_needs_layout = 0;
    g_mouse_button_mask_snapshot = 0;
    if (DAT_007483f0 == 0)
    {
      FUN_004ece9a();
    }

    if (g_text_menu_timeout_seconds != -1)
    {
      s.timer_seconds_left = g_text_menu_timeout_seconds - GetUiTickCount() / (DAT_0057d9e4 * 0x3c);
      if (s.timer_seconds_left == 0)
      {
        s.selected_menu_entry = -1;
        s.done = 1;
      }
      if (s.timer_seconds_left != s.last_timer_seconds)
      {
        strcpy(s.timer_text, _itoa(s.timer_seconds_left, DAT_0078cef0, 10));
        FillGraphicsRect(PTR_DAT_005832b4, g_text_menu_right - 0xe, g_text_menu_top + 4, 0xc, 7, 0xbc);
        DrawCenteredTextLineClamped(s.timer_text, g_text_menu_right - 8, g_text_menu_top + 5, 0xff);
        s.last_timer_seconds = s.timer_seconds_left;
      }
    }

    UpdateMouseSnapshot();
    if ((g_mouse_button_mask_snapshot != 0) || (s.mouse_active != 0))
    {
      g_text_menu_mouse_active = 1;
      s.mouse_active = g_text_menu_mouse_active;
      if (g_mouse_button_mask_snapshot == 2)
      {
        g_text_menu_mouse_released = 1;
      }

      s.selected_menu_entry = ((g_mouse_y_snapshot - g_text_menu_top) - 4) / g_text_menu_line_height - g_text_menu_first_option_line;
      if ((g_mouse_x_snapshot < g_text_menu_left) || (g_text_menu_right < g_mouse_x_snapshot))
      {
        s.selected_menu_entry = -1;
      }

      if ((g_text_menu_disabled_option_mask & (1 << (unsigned char)s.selected_menu_entry)) != 0)
      {
        s.selected_menu_entry = s.previous_menu_entry;
      }
      else
      {
        if (g_mouse_button_mask_snapshot == 0)
        {
          s.done = 1;
        }
      }
    }
    else
    {
      if (HasQueuedKeyInput() != 0)
      {
        s.key_code = PopNormalizedQueuedKeyInput();
        switch (s.key_code)
        {
        case 0x5000:
          if (g_text_menu_option_count - 1 > s.selected_menu_entry)
          {
            s.selected_menu_entry = s.selected_menu_entry + 1;
          }
          break;
        case 0x4800:
          if (0 < s.selected_menu_entry)
          {
            s.selected_menu_entry = s.selected_menu_entry - 1;
          }
          break;
        case 0xd:
        case 0x20:
          if ((g_text_menu_disabled_option_mask & (1 << (unsigned char)s.selected_menu_entry)) == 0)
          {
            s.done = 1;
          }
          break;
        case 0x1b:
          s.selected_menu_entry = -1;
          s.done = 1;
          break;
        default:
          for (s.next_menu_entry = s.selected_menu_entry + 1; s.next_menu_entry < 0x20; s.next_menu_entry = s.next_menu_entry + 1)
          {
            if ((g_text_menu_hotkey_by_option[s.next_menu_entry] != -1) &&
                ((((int)(char)g_text_menu_hotkey_by_option[s.next_menu_entry] ^ (s.key_code & 0x1f)) & 0x1f) == 0))
            {
              s.selected_menu_entry = s.next_menu_entry;
              break;
            }
          }
          break;
        }
      }
    }

    if ((s.selected_menu_entry < 0) || (g_text_menu_option_count <= s.selected_menu_entry) || (g_text_menu_ok_mode != 0) || (g_text_menu_force_cancel != 0))
    {
      s.selected_menu_entry = -1;
    }

    if (s.previous_menu_entry != s.selected_menu_entry)
    {
      DrawTextMenu(menu_text, s.selected_menu_entry);
      s.previous_menu_entry = s.selected_menu_entry;
      g_text_menu_hovered_selection = s.previous_menu_entry;
    }
  } while (!s.done);

  g_text_menu_finish_flash = 1;
  if (s.selected_menu_entry != -1)
  {
    FUN_0046ed33();
    DrawTextMenu(menu_text, s.selected_menu_entry);
    DelayUiTicks(0x14);
    FUN_0046ed03();
  }
  else
  {
    g_text_menu_mouse_released = 0;
  }
  g_text_menu_timeout_seconds = -1;
  g_text_menu_last_selection = g_text_menu_timeout_seconds;
  g_text_menu_initial_selection = g_text_menu_last_selection;
  g_text_menu_show_ok_button = 0;
  g_text_menu_force_cancel = g_text_menu_show_ok_button;
  g_text_menu_disabled_option_mask = g_text_menu_force_cancel;
  g_text_menu_caret_prefix_mask = g_text_menu_disabled_option_mask;
  return s.selected_menu_entry;
}

// FUNCTION: SHANDALAR 0x004ecec6
int ClampIntToRange(int value, int min_value, int max_value)
{
  if (value < min_value)
  {
    value = min_value;
  }
  if (max_value < value)
  {
    value = max_value;
  }
  return value;
}

// FUNCTION: SHANDALAR 0x00430e00
void DrawUiLine(int x1, int y1, int x2, int y2, int color_index)
{
  DrawGraphicsLine(PTR_DAT_005832b4, x1, y1, x2, y2, color_index);
}

// FUNCTION: SHANDALAR 0x004138bd
void DrawTiledDialogBoxFrame(int x, int y, int width, int height, int frame_style)
{
  int tile_columns;
  int tile_rows;
  int row;
  int col;

  x -= 4;
  y -= 4;
  width += 8;
  height += 8;

  tile_columns = (width + 0xf) / 0x10;
  tile_rows = (height + 0xf) / 0x10;
  x -= ((tile_columns << 4) - width) / 2;
  y -= ((tile_rows << 4) - height) / 2;

  PTR_DAT_005832b4->page_number = 1;
  FillGraphicsRect(PTR_DAT_005832b4, x, y, tile_columns << 4, tile_rows << 4, 0xe3);

  for (row = 0; tile_rows > row; row = row + 1)
  {
    for (col = 0; tile_columns > col; col = col + 1)
    {
      DrawEncodedImageUnscaled(PTR_DAT_005832b4, col * 0x10 + x, row * 0x10 + y, g_dialog_box_sprite_bank.frame[frame_style][0]);
    }
  }

  for (col = 0; tile_columns > col; col = col + 1)
  {
    DrawEncodedImageUnscaled(PTR_DAT_005832b4, col * 0x10 + x, y - 10, g_dialog_box_sprite_bank.frame[frame_style][5]);
    DrawEncodedImageUnscaled(PTR_DAT_005832b4, col * 0x10 + x, tile_rows * 0x10 + y - 6, g_dialog_box_sprite_bank.frame[frame_style][7]);
  }

  for (row = 0; tile_rows > row; row = row + 1)
  {
    DrawEncodedImageUnscaled(PTR_DAT_005832b4, x - 10, row * 0x10 + y, g_dialog_box_sprite_bank.frame[frame_style][8]);
    DrawEncodedImageUnscaled(PTR_DAT_005832b4, tile_columns * 0x10 + x - 6, row * 0x10 + y, g_dialog_box_sprite_bank.frame[frame_style][6]);
  }

  DrawEncodedImageUnscaled(PTR_DAT_005832b4, x - 10, y - 10, g_dialog_box_sprite_bank.frame[frame_style][1]);
  DrawEncodedImageUnscaled(PTR_DAT_005832b4, tile_columns * 0x10 + x - 6, y - 10, g_dialog_box_sprite_bank.frame[frame_style][2]);
  DrawEncodedImageUnscaled(PTR_DAT_005832b4, x - 10, tile_rows * 0x10 + y - 6, g_dialog_box_sprite_bank.frame[frame_style][3]);
  DrawEncodedImageUnscaled(PTR_DAT_005832b4, tile_columns * 0x10 + x - 6, tile_rows * 0x10 + y - 6, g_dialog_box_sprite_bank.frame[frame_style][4]);

  PTR_DAT_005832b4->page_number = 0;
  BlitGraphicsRect(PTR_DAT_005832dc, x - 10, y - 10, tile_columns * 0x10 + 0x14, tile_rows * 0x10 + 0x14,
                   PTR_DAT_005832b4, x - 10, y - 10);
}

// FUNCTION: SHANDALAR 0x004136ab
void DrawDialogBoxFrameAutoStyle(int x, int y, int width, int height)
{
  if (g_text_box_frame_color_override != 0)
  {
    g_town_dialog_callback(x, y, width, height, g_text_box_frame_color_override);
    g_text_box_frame_color_override = 0;
  }
  else
  {
    g_town_dialog_callback(x, y, width, height, (y / 8) & 3);
  }
}

// FUNCTION: SHANDALAR 0x00430e2d
void DrawTextLineClamped(char *text, int x, int y, int color_index)
{
  if (x < 0)
  {
    x = 0;
  }
  if (y < 0)
  {
    y = 0;
  }

  if (global_screen_width <= x + MeasureTextLineWidth(text))
  {
    x = global_screen_width - 1 - MeasureTextLineWidth(text);
  }

  if (global_screen_height <= y + GetFontLineHeight(PTR_DAT_005832b4->font_slot))
  {
    y = global_screen_height - 1 - GetFontLineHeight(PTR_DAT_005832b4->font_slot);
  }

  PTR_DAT_005832b4->text_color = color_index;
  DrawTextLine(PTR_DAT_005832b4, x, y, text);
}

// FUNCTION: SHANDALAR 0x00413796
void DrawRectangleBorder(int x, int y, int width, int height, int color_index)
{
  DrawUiLine(x, y, width + x, y, color_index);
  DrawUiLine(x, height + y, width + x, height + y, color_index);
  DrawUiLine(width + x, y, width + x, height + y, color_index);
  DrawUiLine(x, y, x, height + y, color_index);
}

// FUNCTION: SHANDALAR 0x00430ef4
void DrawTextLineNoShadow(char *text, int x, int y, int color_index)
{
  PTR_DAT_005832b4->draw_shadow_enabled = 0;
  DrawTextLineClamped(text, x, y, color_index);
  PTR_DAT_005832b4->draw_shadow_enabled = 1;
}

// FUNCTION: SHANDALAR 0x004310e8
int MeasureTextLineWidth(char *text)
{
  unsigned char *cursor = text;
  int font_slot = PTR_DAT_005832b4->font_slot;
  int result = 0;
  while (*cursor)
  {
    result += GetFontCharWidth(font_slot, *cursor++);
  }
  return result;
}

// FUNCTION: SHANDALAR 0x0043104f
void DrawCenteredTextLineClamped(char *text, int center_x, int y, int color_index)
{
  center_x -= MeasureTextLineWidth(text) / 2;
  PTR_DAT_005832b4->draw_shadow_enabled = 0;
  DrawTextLineClamped(text, center_x, y, color_index);
  PTR_DAT_005832b4->draw_shadow_enabled = 1;
}

// FUNCTION: SHANDALAR 0x004ce97d
int GetUiTickCount(void)
{
  return g_ui_tick_count;
}

// FUNCTION: SHANDALAR 0x004ce955
int ConsumeUiTickCount(void)
{
  int queued_ticks;

  queued_ticks = g_ui_tick_count;
  g_ui_tick_count = 0;
  return queued_ticks;
}

// FUNCTION: SHANDALAR 0x004ece9a
int FUN_004ece9a(void)
{
  _DAT_007483f4 = GetTickCount() & 0x7fff;
  DAT_007483f0 = 1;
  return 0;
}

// FUNCTION: SHANDALAR 0x0041318b
int DrawTextMenu(char *menu_text, int selected_option)
{
  size_t text_len;
  int char_width;
  int text_color;
  int highlight_color;
  int line_idx;
  int menu_bottom;
  int menu_option_idx;
  int line_width;

  DAT_005a5fe8[0] = 0;
  if (g_text_menu_needs_layout == 1)
  {
    g_text_menu_line_height = GetFontLineHeight(PTR_DAT_005832b4->font_slot);
    for (line_idx = 0; line_idx < 0x20; line_idx = line_idx + 1)
    {
      g_text_menu_hotkey_by_option[line_idx] = -1;
    }

    menu_option_idx = 0;
    g_text_menu_line_count = 0;
    line_width = 0;
    g_text_menu_max_line_width = 0;
    for (line_idx = 0, text_len = strlen(menu_text); line_idx < (int)text_len; line_idx = line_idx + 1)
    {
      if (menu_text[line_idx] == '\n')
      {
        if (g_text_menu_max_line_width < line_width)
        {
          g_text_menu_max_line_width = line_width;
        }
        line_width = 0;
        g_text_menu_line_count = g_text_menu_line_count + 1;
        DAT_005a5fe8[g_text_menu_line_count] = line_idx + 1;
      }
      else
      {
        if ((line_width == 0) && ((menu_text[line_idx] == ' ') || (menu_text[line_idx] == '_')))
        {
          if (menu_option_idx < 0x20)
          {
            g_text_menu_hotkey_by_option[menu_option_idx] = menu_text[line_idx + 1];
          }
          if (g_text_menu_first_option_line == -1)
          {
            g_text_menu_first_option_line = g_text_menu_line_count;
          }
          menu_option_idx = menu_option_idx + 1;
        }
        char_width = GetFontCharWidth(PTR_DAT_005832b4->font_slot, menu_text[line_idx]);
        line_width = line_width + char_width;
      }
    }

    g_text_menu_line_count = ClampIntToRange(g_text_menu_line_count, 0, (global_screen_height - g_text_menu_top) / g_text_menu_line_height);
    if (g_text_menu_left == -1)
    {
      g_text_menu_left = 0xa0 - (g_text_menu_max_line_width + 8) / 2;
    }
    g_text_menu_right = g_text_menu_max_line_width + g_text_menu_left + 8;
    menu_bottom = g_text_menu_line_count * g_text_menu_line_height + g_text_menu_top + 6;
    if (g_text_menu_show_ok_button != 0)
    {
      menu_bottom = g_text_menu_line_count * g_text_menu_line_height + g_text_menu_top + 8;
    }
    text_len = strlen(menu_text);
    if (menu_text[text_len - 1] != '\n')
    {
      menu_option_idx = menu_option_idx - 1;
    }
    g_text_menu_option_count = menu_option_idx;
    DrawDialogBoxFrameAutoStyle(g_text_menu_left, g_text_menu_top, g_text_menu_right - g_text_menu_left, menu_bottom - g_text_menu_top);
    if (g_text_menu_ok_mode != 0)
    {
      DrawTextLineClamped("OK", g_text_menu_right - 0x11, menu_bottom - 8, 0xfe);
      DrawRectangleBorder(g_text_menu_right - 0x14, menu_bottom - 10, 0x14, 10, 0xfe);
    }
  }

  if ((*menu_text == ' ') || (*menu_text == '_'))
  {
    menu_option_idx = 0;
  }
  else
  {
    menu_option_idx = -1;
  }

  g_text_menu_color_base = g_text_menu_color_normal;
  PTR_DAT_005832b4->text_color = g_text_menu_color_normal;
  for (line_idx = 0; line_idx < g_text_menu_line_count; line_idx = line_idx + 1)
  {
    if ((g_text_menu_needs_layout != 0) || (menu_option_idx == g_text_menu_hovered_selection) || (menu_option_idx == selected_option))
    {
      menu_text[DAT_005a5fe8[line_idx + 1] - 1] = '\0';
      if ((menu_option_idx < 0) || ((g_text_menu_caret_prefix_mask & (1 << ((unsigned char)menu_option_idx & 0x1f))) == 0))
      {
        if (menu_option_idx < 0)
        {
          text_color = g_text_menu_color_base;
        }
        else if (menu_option_idx == selected_option)
        {
          text_color = g_text_menu_color_selected;
        }
        else
        {
          text_color = g_text_menu_color_normal;
        }
        DrawTextLineNoShadow(menu_text + DAT_005a5fe8[line_idx], g_text_menu_left + 5, line_idx * g_text_menu_line_height + g_text_menu_top + 5, text_color);
      }
      else
      {
        menu_text[DAT_005a5fe8[line_idx]] = '^';
        if ((g_text_menu_finish_flash == 0) || (menu_option_idx != selected_option))
        {
          if (menu_option_idx < 0)
          {
            highlight_color = g_text_menu_color_base;
          }
          else if (menu_option_idx == selected_option)
          {
            highlight_color = g_text_menu_color_selected;
          }
          else
          {
            highlight_color = g_text_menu_color_normal;
          }
          DrawTextLineNoShadow(menu_text + DAT_005a5fe8[line_idx], g_text_menu_left + 5, line_idx * g_text_menu_line_height + g_text_menu_top + 5, highlight_color);
        }
        else
        {
          DrawTextLineNoShadow(menu_text + DAT_005a5fe8[line_idx], g_text_menu_left + 5, line_idx * g_text_menu_line_height + g_text_menu_top + 5, 0xff);
        }
        menu_text[DAT_005a5fe8[line_idx]] = ' ';
      }
      menu_text[DAT_005a5fe8[line_idx + 1] - 1] = '\n';
    }

    if ((menu_text[DAT_005a5fe8[line_idx + 1]] == ' ') || (menu_text[DAT_005a5fe8[line_idx + 1]] == '_'))
    {
      menu_option_idx = menu_option_idx + 1;
    }
  }
  return selected_option;
}

// FUNCTION: SHANDALAR 0x004184d2
void FUN_004184d2(void)
{
  struct
  {
    int local_124;
    char local_120[8];
    int local_118;
    int local_114;
    int local_110;
    char location_block_start_index[256];
    FILE *inner_index;
    int entry_index;
  } s;

  s.inner_index = fopen("hints.txt", "rt");
  s.local_118 = 0;
  do
  {
    s.entry_index = fscanf(s.inner_index, "%[^\n]", s.location_block_start_index);
    if (s.location_block_start_index[0] == '.')
    {
      sscanf(s.location_block_start_index + 1, "%d %d %s", &s.local_124, &s.local_114, s.local_120);
      DAT_0097e450[s.local_118].first = s.local_124;
      DAT_0097e450[s.local_118].second = s.local_114;
      DAT_0097df40[s.local_118] = 0;

      if (strchr(s.local_120, 'A') != (char *)0)
      {
        DAT_0097df40[s.local_118] = DAT_0097df40[s.local_118] | 1;
      }

      if (strchr(s.local_120, 'B') != (char *)0)
      {
        DAT_0097df40[s.local_118] = DAT_0097df40[s.local_118] | 2;
      }

      if (strchr(s.local_120, 'C') != (char *)0)
      {
        DAT_0097df40[s.local_118] = DAT_0097df40[s.local_118] | 4;
      }

      if (strchr(s.local_120, 'D') != (char *)0)
      {
        DAT_0097df40[s.local_118] = DAT_0097df40[s.local_118] | 8;
      }

      s.local_110 = FUN_0056c705(s.local_124);
      s.local_110 = FUN_0056c705(s.local_114);
      s.entry_index = fscanf(s.inner_index, "%[\n]", s.location_block_start_index);
      DAT_0097db40[s.local_118] = ftell(s.inner_index);
      s.local_118 = s.local_118 + 1;
    }
    else
    {
      s.entry_index = fscanf(s.inner_index, "%[\n]", s.location_block_start_index);
    }
  } while ((s.local_118 < 0x100) && (s.entry_index != -1));

  do
  {
    DAT_0097e450[s.local_118].second = -1;
    DAT_0097e450[s.local_118].first = DAT_0097e450[s.local_118].second;
    s.local_118 = s.local_118 + 1;
  } while (s.local_118 < 0x100);

  fclose(s.inner_index);
}
// FUNCTION: SHANDALAR 0x0041786e
void FUN_0041786e(void)
{
  struct
  {
    int icon_height_scaled;
    int icon_width_scaled;
    long selected_state_sprite;
    int location_block_start_index;
    int inner_index;
    FILE *entry_index;
    int local_4;
  } s;

  for (s.location_block_start_index = 0; s.location_block_start_index < 0x4e2; s.location_block_start_index = s.location_block_start_index + 1)
  {
    DAT_005a6198[s.location_block_start_index] = -1;
  }

  s.entry_index = fopen("concise.csv", "rt");
  s.inner_index = 0;

  for (s.location_block_start_index = 0; s.location_block_start_index < g_card_count; s.location_block_start_index = s.location_block_start_index + 1)
  {
    s.inner_index = global_cards_data[s.location_block_start_index].id;
    s.local_4 = fscanf(s.entry_index, "%d %d %ld\n", &s.icon_width_scaled, &s.icon_height_scaled, &s.selected_state_sprite);
    global_cards_data[s.location_block_start_index].rarity = (unsigned char)s.icon_height_scaled;
    DAT_005a6198[s.inner_index] = s.selected_state_sprite;
  }

  fclose(s.entry_index);
}
// FUNCTION: SHANDALAR 0x0046ed03
void FUN_0046ed03(void)
{
  g_cursor_visibility_depth = g_cursor_visibility_depth + 1;
  if ((DAT_00586494 != 0) && (g_cursor_visibility_depth == 1))
  {
    ShowMouseCursor();
  }
}

// FUNCTION: SHANDALAR 0x0046ed33
void FUN_0046ed33(void)
{
  if ((DAT_00586494 != 0) && (g_cursor_visibility_depth == 1))
  {
    HideMouseCursor();
  }
  g_cursor_visibility_depth = g_cursor_visibility_depth - 1;
}

// FUNCTION: SHANDALAR 0x00500321
int BeginMenuContext(void)
{
  g_menu_context_index = g_menu_context_index + 1;
  g_menu_current_control_index = -1;
  g_menu_prev_control_index = -1;
  ResetMenuContext(g_menu_context_index);
  return g_menu_context_index;
}

// FUNCTION: SHANDALAR 0x005000fb
int ResetMenuContext(int context_index)
{
  g_menu_control_count_by_context[context_index] = 0;
  g_menu_allow_arrow_nav_by_context[context_index] = 1;
  return 0;
}

// FUNCTION: SHANDALAR 0x0050014e
int AddMenuControlsToContext(AdvMenuControl *controls, int control_count, int context_index)
{
  int write_index;
  int i;

  g_menu_render_guard = 1;
  write_index = g_menu_control_count_by_context[context_index];
  for (i = 0; i < control_count; write_index++, i++)
  {
    g_menu_controls_by_context[context_index][write_index] = &controls[i];
  }
  g_menu_control_count_by_context[context_index] = g_menu_control_count_by_context[context_index] + control_count;
  g_menu_render_guard = 0;
  return g_menu_control_count_by_context[context_index];
}

// FUNCTION: SHANDALAR 0x0050027e
int RenderMenuControlRange(int first_index, int count)
{
  struct
  {
    int i;
    int limit;
  } s;

  s.limit = MIN(g_menu_control_count_by_context[g_menu_context_index], first_index + count);

  g_menu_render_guard = 1;
  for (s.i = first_index; s.i < s.limit; s.i = s.i + 1)
  {
    g_menu_controls_by_context[g_menu_context_index][s.i]->on_render(g_menu_controls_by_context[g_menu_context_index][s.i], 0);
  }
  g_menu_render_guard = 0;
  return 1;
}

// FUNCTION: SHANDALAR 0x0050035e
int EndMenuContext(void)
{
  ResetMenuContext(g_menu_context_index);
  if (g_menu_context_index == 0)
  {
    g_menu_context_index = 0;
  }
  else
  {
    g_menu_context_index = g_menu_context_index - 1;
  }
  g_menu_current_control_index = -1;
  g_menu_prev_control_index = -1;
  return g_menu_context_index;
}

// FUNCTION: SHANDALAR 0x00522508
int RandomIntLessThan(int param_1)
{
  return (param_1 > 1) ? rand() % param_1 : 0;
}

// FUNCTION: SHANDALAR 0x004ce992
void DelayUiTicks(int delay)
{
  int start = g_ui_tick_count;
  while ((g_ui_tick_count - start) < (delay / 2))
  {
  }
}

// FUNCTION: SHANDALAR 0x004bdd0a
void InitializeNewGameState(void)
{
  struct
  {
    int icon_width_scaled;
    unsigned int uVar3;
    unsigned int uVar2;
    int location_block_start_index;
    int entry_index;
    int local_4;
  } s;

  if (g_duel_ai_mode_state != 0)
  {
    return;
  }

  for (s.location_block_start_index = 0; s.location_block_start_index < 500; s.location_block_start_index = s.location_block_start_index + 1)
  {
    deck[s.location_block_start_index] = -1;
  }

  if (DAT_00715f10)
  {
    return;
  }

  for (s.location_block_start_index = 0; s.location_block_start_index < 7; s.location_block_start_index = s.location_block_start_index + 1)
  {
    DAT_008b3240[s.location_block_start_index] = 0;
    DAT_008c7408[s.location_block_start_index] = -1;
  }

  g_world_magic_bitmap = g_world_magic_bitmap | (1 << (g_selected_wizard_color * 2));
  g_world_magic_slot_timers[g_selected_wizard_color * 2].town_index = 0;

  s.uVar2 = 1 << (BYTE)g_selected_wizard_color;
  s.entry_index = g_shandalar_difficulty + 1;
  s.local_4 = s.entry_index;
  if (g_shandalar_difficulty == 3)
  {
    s.entry_index = 1;
    s.entry_index = 3;
  }

  switch (g_shandalar_difficulty)
  {
  case 0:
    if ((s.uVar2 < 1) & 0x20)
    {
      AddRandomStarterDeckCards(s.uVar2, 0xd, 7, 0xf, 1, 1);
    }
    else
    {
      AddRandomStarterDeckCards(s.uVar2, 0xd, 0xc, 10, 1, 1);
    }
    break;
  case 1:
    AddRandomStarterDeckCards(s.uVar2, 0xb, 4, 0xc, 1, 1);
    AddRandomStarterDeckCards(FUN_004bdccc(s.uVar2), 4, 3, 4, 0, 1);
    break;
  case 2:
    AddRandomStarterDeckCards(s.uVar2, 9, 3, 9, 1, 1);
    s.uVar3 = FUN_004bdccc(s.uVar2);
    AddRandomStarterDeckCards(s.uVar3, 5, 3, 4, 0, 1);
    AddRandomStarterDeckCards(FUN_004bdccc(s.uVar2 | s.uVar3), 4, 3, 3, 0, 1);
    break;
  case 3:
    AddRandomStarterDeckCards(s.uVar2, 6, 3, 5, 1, 1);
    AddRandomStarterDeckCards(1, 0xb, 5, 0xe, 0, 1);
    break;
  }

  g_journal_entry_count = 0;
  for (s.location_block_start_index = 0; s.location_block_start_index < 5; s.location_block_start_index = s.location_block_start_index + 1)
  {
    g_amulet_inventory[s.location_block_start_index] = 0;
  }

  g_amulet_inventory[g_selected_wizard_color - 1]++;
  for (s.location_block_start_index = 0; s.location_block_start_index < 3 - g_shandalar_difficulty; s.location_block_start_index = s.location_block_start_index + 1)
  {
    s.icon_width_scaled = RandomIntLessThan(5);
    g_amulet_inventory[s.icon_width_scaled]++;
  }

  for (s.location_block_start_index = 0; s.location_block_start_index < 0x80; s.location_block_start_index = s.location_block_start_index + 1)
  {
    g_town_slots[s.location_block_start_index].location_type = -1;
  }

  for (s.location_block_start_index = 0; s.location_block_start_index < 8; s.location_block_start_index = s.location_block_start_index + 1)
  {
    g_lair_or_monster_slots[s.location_block_start_index].entry_type = SHANDALAR_ENTRY_NONE;
  }

  for (s.location_block_start_index = 0; s.location_block_start_index < 1000; s.location_block_start_index = s.location_block_start_index + 1)
  {
    g_duel_victory_log[0] = 0;
  }

  for (s.location_block_start_index = 0; s.location_block_start_index < 4; s.location_block_start_index = s.location_block_start_index + 1)
  {
    DAT_007a7d10[s.location_block_start_index] = 8;
  }

  for (s.location_block_start_index = 0; s.location_block_start_index < 0x96; s.location_block_start_index = s.location_block_start_index + 1)
  {
    deck[s.location_block_start_index] = deck[s.location_block_start_index] | 0x10000;
  }
  g_selected_wizard_color = -1;
}

// FUNCTION: SHANDALAR 0x004bdccc
unsigned int FUN_004bdccc(unsigned int param_1)
{
  int entry_index;

  do
  {
    entry_index = RandomIntLessThan(5) + 1;
  } while ((param_1 & (1U << (entry_index & 0xff))) != 0);

  return 1U << (entry_index & 0xff);
}

// FUNCTION: SHANDALAR 0x004be0bf
int AddRandomStarterDeckCards(unsigned int color_mask, int land_count, int spell_count, int creature_count, int add_rare, int allow_artifact_spells)
{
  struct
  {
    unsigned int color_filter;
    unsigned int candidate_color_filter;
    unsigned int pick_artifact;
    unsigned int selected_card;
    unsigned int count;
    int duplicate_attempt_count;
    int card_ok;
  } s;

  for (s.count = 0; land_count > (int)s.count; s.count = s.count + 1)
  {
    s.selected_card = PickRandomCardMatchingTypeAndColor(1, color_mask);
    if ((FUN_0056c0e5((int)(char)global_cards_data[s.selected_card].color, color_mask, 0) == 0) ||
        (4 < (int)s.selected_card) ||
        ((global_cards_data[s.selected_card].expansion & 0xc1) == 0))
    {
      s.count = s.count - 1;
    }
    else
    {
      FUN_0056bd9d(s.selected_card);
    }
  }

  for (s.count = 0; spell_count > (int)s.count; s.count = s.count + 1)
  {
    if (allow_artifact_spells == 0)
    {
      s.pick_artifact = 0;
    }
    else
    {
      s.pick_artifact = rand();
      s.pick_artifact = s.pick_artifact & 1;
    }

    if (s.pick_artifact == 0)
    {
      s.candidate_color_filter = color_mask;
    }
    else
    {
      s.candidate_color_filter = 1;
    }

    s.selected_card = PickRandomCardMatchingTypeAndColor((-(unsigned int)(s.pick_artifact == 0) & 0xffffffc4) + 0x40, s.candidate_color_filter);
    if (((g_shandalar_difficulty == 0) && ((global_cards_data[s.selected_card].static_ability & 3) != 0)) ||
        ((global_cards_data[s.selected_card].extra_ability & 0x900) != 0))
    {
      s.count = s.count - 1;
    }
    else
    {
      if ((global_cards_data[s.selected_card].type & 4) != 0)
      {
        s.pick_artifact = 0;
      }

      if (s.pick_artifact == 0)
      {
        s.color_filter = color_mask;
      }
      else
      {
        s.color_filter = 1;
      }

      if ((FUN_0056c0e5((int)(char)global_cards_data[s.selected_card].color, s.color_filter, 0) == 0) ||
          ((int)((~s.count & 1) + 1) < FUN_0056c5ea(s.selected_card)) ||
          ((global_cards_data[s.selected_card].expansion & 0xc1) == 0))
      {
        s.count = s.count - 1;
      }
      else
      {
        FUN_0056bd9d(s.selected_card);
      }
    }
  }

  s.duplicate_attempt_count = 0;
  for (s.count = 0; creature_count > (int)s.count; s.count = s.count + 1)
  {
    s.selected_card = PickRandomCardMatchingTypeAndColor(2, color_mask);
    if (((g_shandalar_difficulty < 4) && ((global_cards_data[s.selected_card].static_ability & 3) != 0)) ||
        ((global_cards_data[s.selected_card].extra_ability & 0x900) != 0))
    {
      s.count = s.count - 1;
      s.duplicate_attempt_count = s.duplicate_attempt_count - 1;
    }
    else
    {
      if (s.duplicate_attempt_count < 1000)
      {
        s.card_ok = 0 < FUN_004bb1cf(s.selected_card);
      }
      else
      {
        s.card_ok = 1;
      }

      if ((FUN_0056c0e5((int)(char)global_cards_data[s.selected_card].color, color_mask, 0) == 0) ||
          ((int)((~s.count & 1) + 1) < FUN_0056c5ea(s.selected_card)) ||
          ((!s.card_ok || ((global_cards_data[s.selected_card].expansion & 0xc1) == 0))))
      {
        s.count = s.count - 1;
      }
      else
      {
        FUN_0056bd9d(s.selected_card);
      }
    }
    s.duplicate_attempt_count = s.duplicate_attempt_count + 1;
  }

  if (add_rare != 0)
  {
    do
    {
      do
      {
        s.selected_card = PickRandomCardMatchingTypeAndColor(0xe, 1);
      } while (FUN_0056c0e5((int)(char)global_cards_data[s.selected_card].color, color_mask, 1) == 0);
    } while ((FUN_0056c5ea(s.selected_card) < 3) ||
             (FUN_004bb1cf(s.selected_card) < 1) ||
             ((g_shandalar_difficulty == 0 && ((global_cards_data[s.selected_card].static_ability & 3) != 0))) ||
             ((global_cards_data[s.selected_card].extra_ability & 0x900) != 0) ||
             ((global_cards_data[s.selected_card].expansion & 0xc1) == 0));
  }

  FUN_0056bd9d(s.selected_card);
  return 0;
}

// FUNCTION: SHANDALAR 0x0056c705
int FUN_0056c705(int param_1)
{
  int ret;
  int entry_index;

  if (param_1 == -1)
  {
    return -1;
  }

  ret = -1;
  for (entry_index = 0; entry_index < g_card_count + 0x10; entry_index = entry_index + 1)
  {
    if (global_cards_data[entry_index].id == param_1)
    {
      return entry_index;
    }
  }
  return ret;
}

// FUNCTION: SHANDALAR 0x0056bcf7
int PickRandomCardMatchingTypeAndColor(unsigned int type_mask, unsigned int color_mask)
{
  struct
  {
    int attempt_count;
    int card_index;
    int found;
  } s;

  s.attempt_count = 0;
  do
  {
    s.found = 0;
    s.card_index = RandomIntLessThan(g_card_count - 0x39);
    if ((type_mask != 0) && ((type_mask & global_cards_data[s.card_index].type) == 0))
    {
    }
    else if ((color_mask != 1) && ((color_mask & (int)(char)global_cards_data[s.card_index].color) == 0))
    {
    }
    else
    {
      s.found = 1;
    }
  } while (s.found == 0 && ++s.attempt_count < 999);

  return s.card_index;
}

// FUNCTION: SHANDALAR 0x0056c0e5
int FUN_0056c0e5(int param_1, int param_2, int param_3)
{
  // GLOBAL: SHANDALAR 0x00593e20
  static const signed char DAT_00593e20[18] = {
      0, 0, 0,
      1, 4, 2,
      2, 1, 5,
      3, 5, 4,
      4, 3, 1,
      5, 2, 5};

  if ((param_1 == 1) || (param_2 == 1))
  {
    return 1;
  }

  param_1 = GetFirstManaColorIndex(param_1);
  param_2 = GetFirstManaColorIndex(param_2);
  if (DAT_00593e20[param_2 * 3] == param_1)
  {
    return 1;
  }

  if (param_3 > 1 && DAT_00593e20[param_2 * 3 + 1] == param_1)
  {
    return 1;
  }

  if (param_3 > 2 && DAT_00593e20[param_2 * 3 + 2] == param_1)
  {
    return 1;
  }

  if (param_3 > 3)
  {
    return 1;
  }
  return 0;
}

// FUNCTION: SHANDALAR 0x0056c5ea
int FUN_0056c5ea(int param_1)
{
  struct
  {
    char rarity_str[32];
    int rarity;
  } s;

  if (((global_cards_data[param_1].extra_ability & 0x180U) != 0) || (global_cards_data[param_1].expansion == '@'))
  {
    global_cards_data[param_1].rarity = 4;
  }

  if ((signed char)global_cards_data[param_1].rarity != -1)
  {
    s.rarity = (int)(signed char)global_cards_data[param_1].rarity;

    return s.rarity;
  }

  ReadCsvFieldByCsvid(s.rarity_str, global_cards_data[param_1].id, 9, "info.csv");

  s.rarity = 1;

  if (strcmp(s.rarity_str, "Special") == 0)
  {
    s.rarity = 3;
  }

  if (strcmp(s.rarity_str, "Rare") == 0)
  {
    s.rarity = 3;
  }

  if (strcmp(s.rarity_str, "Uncommon") == 0)
  {
    s.rarity = 2;
  }

  global_cards_data[param_1].rarity = (unsigned char)s.rarity;

  return s.rarity;
}

// FUNCTION: SHANDALAR 0x004bb1cf
int FUN_004bb1cf(unsigned int param_1)
{
  struct
  {
    int color_index;
    int copies_in_deck;
    int total_non_market_cards;
    int i;
    int max_color_count;
    int color_counts[7];
  } s;

  if ((int)param_1 <= 4)
  {
    return 99;
  }

  for (s.i = 0; s.i < 7; s.i = s.i + 1)
  {
    s.color_counts[s.i] = 0;
  }

  s.total_non_market_cards = 0;
  s.copies_in_deck = s.total_non_market_cards;

  for (s.i = 0; s.i < 500; s.i = s.i + 1)
  {
    if ((deck[s.i] != -1) && ((deck[s.i] & 0x4000) == 0))
    {
      s.total_non_market_cards = s.total_non_market_cards + 1;
      s.color_index = GetFirstManaColorIndex(global_cards_data[deck[s.i] & 0xfff].color);
      s.color_counts[s.color_index] = s.color_counts[s.color_index] + 1;
    }

    if (((unsigned int)deck[s.i] & 0xffff7fffU) == param_1)
    {
      s.copies_in_deck = s.copies_in_deck + 1;
      DAT_005aa62c = s.i;
    }
  }

  s.max_color_count = -1;
  for (s.i = 1; s.i < 7; s.i = s.i + 1)
  {
    if (s.max_color_count < s.color_counts[s.i])
    {
      s.max_color_count = s.color_counts[s.i];
    }
  }

  g_deck_color_bitmap = 0;
  for (s.i = 1; s.i < 7; s.i = s.i + 1)
  {
    if (((s.max_color_count * 2) / 3) <= s.color_counts[s.i])
    {
      g_deck_color_bitmap = g_deck_color_bitmap | (1 << (unsigned char)s.i);
    }
  }

  s.max_color_count = 1;
  if (s.total_non_market_cards >= 0x14)
  {
    s.max_color_count = 1;
  }
  if (s.total_non_market_cards >= 0x28)
  {
    s.max_color_count = 2;
  }
  if (s.total_non_market_cards >= 0x3c)
  {
    s.max_color_count = 3;
  }
  if ((g_world_magic_bitmap & 0x20) != 0)
  {
    s.max_color_count = s.max_color_count + 1;
  }

  if ((global_cards_data[param_1].extra_ability & 0x100) != 0)
  {
    s.max_color_count = (g_shandalar_difficulty <= s.max_color_count);
  }
  else if ((global_cards_data[param_1].expansion & 0xc1) == 0)
  {
    s.max_color_count = s.max_color_count / 2;
  }

  return s.max_color_count - s.copies_in_deck;
}

// FUNCTION: SHANDALAR 0x004bb458
int FUN_004bb458(shandalar_worldmagic_index_t inx)
{
  int entry_index;

  for (entry_index = 0; entry_index < g_card_count + 0x10; entry_index = entry_index + 1)
  {
    if (global_cards_data[entry_index].id == Scards[inx].worldmagic_csvid)
    {
      return entry_index;
    }
  }
}

// FUNCTION: SHANDALAR 0x004290e2
void AddJournalEntry(int entry_type, int entry_arg)
{
  if (g_journal_entry_count < 2000)
  {
    g_journal_entries[g_journal_entry_count].type = entry_type;
    g_journal_entries[g_journal_entry_count].arg = entry_arg;
    g_journal_entries[g_journal_entry_count].tile_x = g_world_player_x / 32;
    g_journal_entries[g_journal_entry_count].tile_y = g_world_player_y / 32;
    g_journal_entry_count = g_journal_entry_count + 1;
  }
}

// FUNCTION: SHANDALAR 0x0056bd9d
int FUN_0056bd9d(unsigned int param_1)
{

  struct
  {
    int selected_state_sprite;
    int location_block_start_index;
    unsigned int inner_index;
    int entry_index;
    int local_4;
  } s;

  if (FUN_0056c5ea(param_1) >= 3)
  {
    AddJournalEntry((int)param_1 / 0x100 + JOURNAL_ENTRY_CARD_FOUND_SET0, param_1 & 0xff);
  }
  s.entry_index = GetFirstManaColorIndex(global_cards_data[param_1].color) * 0x20 + (unsigned int)global_cards_data[param_1].type * 0x100 +
                  (int)global_cards_data[param_1].name[0];
  s.selected_state_sprite = 0;
  for (s.location_block_start_index = 0; s.location_block_start_index < 500; s.location_block_start_index = s.location_block_start_index + 1)
  {
    if (deck[s.location_block_start_index] == -1)
    {
      s.selected_state_sprite = 1;
    }
  }
  if (s.selected_state_sprite == 0)
  {
    return -1;
  }
  for (s.location_block_start_index = 0x1f2; s.location_block_start_index >= 0; s.location_block_start_index = s.location_block_start_index - 1)
  {
    if (deck[s.location_block_start_index] == -1)
    {
      continue;
    }

    s.inner_index = (unsigned int)deck[s.location_block_start_index] & 0xfff;
    s.local_4 = GetFirstManaColorIndex(global_cards_data[s.inner_index].color) * 0x20 + (int)global_cards_data[s.inner_index].name[0] +
                (unsigned int)global_cards_data[s.inner_index].type * 0x100;
    if (s.local_4 >= s.entry_index)
    {
      deck[s.location_block_start_index + 1] = param_1;
      param_1 = (unsigned int)-1;
      return s.location_block_start_index + 1;
    }
    deck[s.location_block_start_index + 1] = (unsigned int)deck[s.location_block_start_index];
  }
  deck[0] = param_1;
  return 0;
}

// FUNCTION: SHANDALAR 0x0052280c
int FUN_0052280c(void *param_1)
{
  if (g_statwin_exports_by_ordinal[1] != 0)
  {
    return ((int(__cdecl *)(void *))g_statwin_exports_by_ordinal[1])(param_1);
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x0056302b
int FUN_0056302b(int param_1)
{
  switch (param_1)
  {
  case 1:
    return 4;
  case 2:
    return 2;
  case 3:
    return 3;
  case 4:
    return 1;
  case 5:
    return 0;
  case 6:
    return 5;
  default:
    return -1;
  }
}

// FUNCTION: SHANDALAR 0x004f6d90
void FUN_004f6d90(void)
{
  struct
  {
    int icon_y_scaled;
    int neighbor_is_open;           // ebp - 0x20
    int uVar1;                      // ebp - 0x1c
    int icon_width_scaled;          // ebp - 0x18
    int i;                          // ebp - 0x14
    int location_block_start_index; // ebp - 0x10
    unsigned int entry_index;       // ebp - 0xc
    int inner_index;                // ebp - 0x8
    int selected_state_sprite;      // ebp - 0x4
  } s;

start:

  InitializeAnimatedNoiseGrid();
  FillGraphicsRect(PTR_DAT_00583304, 0, 0, 0x140, 200, 0);
  s.icon_y_scaled = 0;
  for (s.location_block_start_index = 0; s.location_block_start_index < 0x40; s.location_block_start_index++)
  {
    for (s.icon_width_scaled = 0; s.icon_width_scaled < 0x40; s.icon_width_scaled++)
    {
      if (((s.location_block_start_index < 2) || (s.icon_width_scaled < 2)) || ((0x3e <= s.location_block_start_index) || (0x3e <= s.icon_width_scaled)))
      {
        PutGraphicsPixel(PTR_DAT_00583304, s.location_block_start_index, s.icon_width_scaled, 0);
        continue;
      }

      s.selected_state_sprite = (s.location_block_start_index + s.icon_width_scaled) * 3 - 0x20;
      s.inner_index = (s.icon_width_scaled - s.location_block_start_index) * 3 + 100;
      if ((s.selected_state_sprite < 4) || (s.inner_index < 4) || ((0x13c <= s.selected_state_sprite) || (0xc4 <= s.inner_index)))
      {
        PutGraphicsPixel(PTR_DAT_00583304, s.location_block_start_index, s.icon_width_scaled, 0);
        continue;
      }

      s.uVar1 = FUN_004f7fb9(s.location_block_start_index, s.icon_width_scaled);
      switch (s.uVar1 / 8)
      {
      case 0:
      case 1:
        s.entry_index = 0;
        break;
      case 2:
        s.entry_index = 1;
        if (0x16 <= s.uVar1)
        {
          s.entry_index = 8;
        }
        break;
      case 3:
        s.entry_index = 3;
        break;
      case 4:
        s.entry_index = 6;
        if (s.uVar1 < 0x22)
        {
          s.entry_index = 0xd;
        }
        break;
      case 5:
        if (0x2a >= s.uVar1)
        {
          s.entry_index = 10;
          break;
        }
      case 6:
        s.entry_index = 2;
        break;
      case 7:
        if (s.uVar1 >= 0x3c)
        {
          s.entry_index = 4;
        }
        else
        {
          s.entry_index = 0xc;
        }
        if (s.icon_width_scaled < 0x10 || s.icon_width_scaled > 0x30)
        {
          s.entry_index = 0xf;
        }

        if (s.uVar1 >= 0x3c)
        {
          s.entry_index = 5;
        }
        else
        {
          s.entry_index = 0xf;
        }

        break;

      case 8:
      case 9:
      case 10:
      case 11:
        s.entry_index = 5;
        break;
      }

      PutGraphicsPixel(PTR_DAT_00583304, s.location_block_start_index, s.icon_width_scaled, (int)s.entry_index);
      if (s.entry_index != 0)
      {
        s.icon_y_scaled++;
      }
    }
  }

  if (s.icon_y_scaled < 0x6d6)
  {
    // TODO : Why didn't they just write this as do/while?
    goto start;
  }

  for (s.location_block_start_index = 1; s.location_block_start_index < 0x3f; s.location_block_start_index++)
  {
    for (s.icon_width_scaled = 1; s.icon_width_scaled < 0x3f; s.icon_width_scaled++)
    {
      s.entry_index = GetWorldTileType(s.location_block_start_index, s.icon_width_scaled);
      if (s.entry_index == 0)
      {
        s.neighbor_is_open = 0;
        for (s.i = 1; s.i <= 8; s.i += 2)
        {
          if (GetWorldTileType(s.location_block_start_index + g_neighbor_dx[s.i], s.icon_width_scaled + g_neighbor_dy[s.i]) == 0)
          {
            s.neighbor_is_open = 1;
            break;
          }
        }
        if (s.neighbor_is_open == 0)
        {
          PutGraphicsPixel(PTR_DAT_00583304, s.location_block_start_index, s.icon_width_scaled, 6);
        }
      }
    }
  }

  PropagatePathConnectivity();
  if (GenerateWorldTownSlots() == 0)
  {
    goto start;
  }

  GenerateTownConnections();
  for (s.i = 0; s.i < 7; s.i++)
  {
    g_lair_or_monster_slots[s.i].respawn_timestamp = (4 - g_shandalar_difficulty) * s.i * -100;
  }
}

// FUNCTION: SHANDALAR 0x004f717a
int GenerateWorldTownSlots(void)
{
  int preview_panel_y_offset;
  int avatar_sprite_index;
  int clamped_required_wins;
  int seeded_world_magic_town_count;
  int scan_index;
  int placement_attempt_count;
  int avatar_draw_y;
  int local_34;
  int local_30;
  int local_2c;
  int icon_width_scaled;
  int location_block_start_index;
  int inner_index;
  int entry_index;
  unsigned int tile_type;
  unsigned int world_magic_mask;
  unsigned int uVar7;
  signed char special_location_world_magic_index;
  int generated_valid_world;

  scan_index = 0;
  do
  {
    scan_index = scan_index + 1;
    if (4 < scan_index)
    {
      return 0;
    }

    seeded_world_magic_town_count = 0;
    entry_index = 0;
    location_block_start_index = 0;

    for (local_2c = 0; local_2c < 0xc; local_2c = local_2c + 1)
    {
      g_world_magic_slot_timers[local_2c].town_index = 0;
    }

    tile_type = (unsigned int)clock();
    uVar7 = (int)tile_type >> 0x1f;
    inner_index = ((tile_type ^ uVar7) - uVar7 & 0x7f ^ uVar7) - uVar7;

    memset(g_town_slots, 0xff, 0x3200);

    for (local_2c = 0; local_2c < 0x80; local_2c = local_2c + 1)
    {
      placement_attempt_count = 0;
      do
      {
        generated_valid_world = 0;
        preview_panel_y_offset = RandomIntLessThan(0x40);
        avatar_sprite_index = RandomIntLessThan(0x40);
        tile_type = GetWorldTileType(preview_panel_y_offset, avatar_sprite_index);
        if (tile_type != 0)
        {
          avatar_draw_y = 0x7fff;
          icon_width_scaled = 0x7fff;
          for (local_34 = 0; local_34 < 0x80; local_34 = local_34 + 1)
          {
            if (g_town_slots[local_34].world_x != -1)
            {
              clamped_required_wins = ApproximateDistance(preview_panel_y_offset - g_town_slots[local_34].world_x, avatar_sprite_index - g_town_slots[local_34].world_y);
              if (clamped_required_wins < avatar_draw_y)
              {
                avatar_draw_y = clamped_required_wins;
              }
              if ((clamped_required_wins < icon_width_scaled) && (g_town_slots[local_34].location_type == 3))
              {
                icon_width_scaled = clamped_required_wins;
              }
            }
          }

          placement_attempt_count = placement_attempt_count + 1;
          if (7 - placement_attempt_count / 100 <= avatar_draw_y)
          {
            generated_valid_world = 1;

            g_town_slots[inner_index].world_x = preview_panel_y_offset;
            g_town_slots[inner_index].world_y = avatar_sprite_index;

            if (icon_width_scaled < 0x21)
            {
              if (avatar_draw_y < 0xb)
              {
                g_town_slots[inner_index].location_type = 1;
              }
              else
              {
                g_town_slots[inner_index].location_type = 2;
              }
            }
            else
            {
              g_town_slots[inner_index].location_type = 3;
            }

            g_town_slots[inner_index].trade_color_and_type = 0;
            g_town_slots[inner_index].status_and_ruling_wizard = g_town_slots[inner_index].trade_color_and_type;

            for (local_34 = 0; local_34 < 8; local_34 = local_34 + 1)
            {
              g_town_slots[inner_index].card_restock_timers[local_34] = 0xfffffc18;
            }
            g_town_slots[inner_index].quest_restock_timer = 0xfffffc18;
            g_town_slots[inner_index].special_card_restock_timer = 0xfffffc18;

            special_location_world_magic_index = (signed char)(tile_type == 3);
            if (tile_type == 1)
            {
              special_location_world_magic_index = 2;
            }
            if (tile_type == 2)
            {
              special_location_world_magic_index = 3;
            }
            if (tile_type == 5)
            {
              special_location_world_magic_index = 4;
            }
            if (tile_type == 6)
            {
              special_location_world_magic_index = 5;
            }

            if ((0x10 < icon_width_scaled) && (special_location_world_magic_index != 0) && ((location_block_start_index & 1 << special_location_world_magic_index) == 0))
            {
              g_town_slots[inner_index].location_type = 4;
              location_block_start_index = location_block_start_index | 1 << special_location_world_magic_index;
            }

            world_magic_mask = GetWorldTileMagicMask(tile_type);
            if ((inner_index != 0) && ((g_town_slots[inner_index].location_type == 3) || (g_town_slots[inner_index].location_type == 2)))
            {
              for (local_34 = 0; local_34 < 99; local_34 = local_34 + 1)
              {
                clamped_required_wins = RandomIntLessThan(10);
                clamped_required_wins = clamped_required_wins + 2;
                if ((g_world_magic_slot_timers[clamped_required_wins].town_index == 0) && ((world_magic_mask & 1 << (((char)(clamped_required_wins / 2)) & 0x1f)) != 0))
                {
                  g_world_magic_slot_timers[clamped_required_wins].town_index = inner_index;
                  break;
                }
              }

              if (0x62 < local_34)
              {
                clamped_required_wins = RandomIntLessThan(2);
                g_world_magic_slot_timers[clamped_required_wins].town_index = inner_index;
              }

              if (seeded_world_magic_town_count < 10)
              {
                g_town_slots[inner_index].status_and_ruling_wizard = g_town_slots[inner_index].status_and_ruling_wizard | 1;
                seeded_world_magic_town_count = seeded_world_magic_town_count + 1;
              }
            }

            SetWorldMapPixelFlags(0x10, preview_panel_y_offset, avatar_sprite_index);
            tile_type = inner_index * 5 + 1;
            uVar7 = (int)tile_type >> 0x1f;
            inner_index = ((tile_type ^ uVar7) - uVar7 & 0x7f ^ uVar7) - uVar7;
          }
        }
      } while (generated_valid_world == 0);

      if (1 < g_town_slots[local_2c].location_type)
      {
        entry_index = entry_index + 1;
      }
    }

    generated_valid_world = 1;
    if ((location_block_start_index != 0x3e) || (entry_index < 0x1e))
    {
      generated_valid_world = 0;
    }

    for (local_30 = 0; local_30 < 6; local_30 = local_30 + 1)
    {
      do
      {
        do
        {
          preview_panel_y_offset = RandomIntLessThan(0x80);
        } while (g_town_slots[preview_panel_y_offset].location_type < 2);
      } while ((g_town_slots[preview_panel_y_offset].location_type == 4) || (g_town_slots[preview_panel_y_offset].trade_color_and_type != 0));
      g_town_slots[preview_panel_y_offset].trade_color_and_type = 1 << (((char)local_30) & 0x1f);
    }

    local_34 = RandomIntLessThan(0xc);
    for (local_2c = 0; local_2c < 0x80; local_2c = local_2c + 1)
    {
      if ((1 < g_town_slots[local_2c].location_type) && (g_town_slots[local_2c].location_type < 4))
      {
        if ((local_34 & 1) == 0)
        {
          g_town_slots[local_2c].trade_color_and_type = (((local_34 % 10) / 2) + 1) * 0x100;
        }
        else
        {
          g_town_slots[local_2c].trade_color_and_type = 1 << (((char)((local_34 % 0xc) / 2)) & 0x1f);
        }
        local_34 = local_34 + 1;
      }
    }

    for (local_2c = 0; local_2c < 0xc; local_2c = local_2c + 1)
    {
      if (g_world_magic_slot_timers[local_2c].town_index == 0)
      {
        generated_valid_world = 0;
      }
      if ((g_world_magic_bitmap & 1 << (((char)local_2c) & 0x1f)) != 0)
      {
        g_world_magic_slot_timers[local_2c].town_index = 0;
      }
    }

    if (generated_valid_world != 0)
    {
      return 1;
    }

    for (local_2c = 0; local_2c < 0x80; local_2c = local_2c + 1)
    {
      ClearWorldMapPixelFlags(0x10, g_town_slots[local_2c].world_x, g_town_slots[local_2c].world_y);
    }

    for (local_2c = 0; local_2c < 0xc; local_2c = local_2c + 1)
    {
      g_world_magic_slot_timers[local_2c].town_index = 0;
    }
  } while (1);
}

// FUNCTION: SHANDALAR 0x004f78d3
void GenerateTownConnections(void)
{
  struct
  {
    int connection_index; /* -0x24 */
    int best_dist;        /* -0x20 */
    int candidate_index;  /* -0x1c */
    int town_index;       /* -0x18 */
    int sample_index;     /* -0x14 */
    int distance;         /* -0x10 */
    int chosen_index;     /* -0x0c */
    int best_index;       /* -0x08 */
    int attempts;         /* -0x04 */
  } s;

  for (s.town_index = 0; s.town_index < 0x80; s.town_index = s.town_index + 1)
  {
    for (s.connection_index = 0; s.connection_index < g_town_slots[s.town_index].location_type; s.connection_index = s.connection_index + 1)
    {
      s.attempts = 0;
    retry:
      s.best_dist = 0x7fff;
      s.sample_index = 0;
      for (; s.sample_index < 0x2a; s.sample_index = s.sample_index + 1)
      {
        s.candidate_index = RandomIntLessThan(0x80);
        s.distance =
            ApproximateDistance(g_town_slots[s.town_index].world_x - g_town_slots[s.candidate_index].world_x,
                         g_town_slots[s.town_index].world_y - g_town_slots[s.candidate_index].world_y);
        if (s.distance < s.best_dist)
        {
          s.best_dist = s.distance;
          s.chosen_index = s.best_index;
          s.best_index = s.candidate_index;
        }
      }

      if (CreateTownConnectionPath(g_town_slots[s.town_index].world_x, g_town_slots[s.town_index].world_y,
                                   g_town_slots[s.chosen_index].world_x, g_town_slots[s.chosen_index].world_y) == 0)
      {
        if (++s.attempts < 3)
        {
          goto retry;
        }
      }
    }
  }
}

// FUNCTION: SHANDALAR 0x004f7a3c
int CreateTownConnectionPath(int start_x, int start_y, int target_x, int target_y)
{
  struct
  {
    int step_count;      /* -0x30 */
    int best_direction;  /* -0x2c */
    int best_score;      /* -0x28 */
    int flags_masked;    /* -0x24 */
    int direction_index; /* -0x20 */
    int cur_y;           /* -0x1c */
    int cur_x;           /* -0x18 */
    int target_dist;     /* -0x14 */
    int next_y;          /* -0x10 */
    int next_x;          /* -0x0c */
    int tile_type;       /* -0x08 */
    int score;           /* -0x04 */
  } s;

  s.cur_x = start_x;
  s.cur_y = start_y;
  s.step_count = 0;

  BlitGraphicsRect(PTR_DAT_00583304, 0, 0, 0x40, 0x80, PTR_DAT_00583304, 0x40, 0);
  SetWorldMapPixelFlags(0x20, s.cur_x, s.cur_y);

  do
  {
    s.target_dist = ApproximateDistance(target_x - s.cur_x, target_y - s.cur_y);
    s.best_direction = -1;
    s.best_score = 0x7fff;

    for (s.direction_index = 1; s.direction_index <= 8; s.direction_index = s.direction_index + 1)
    {
      s.next_x = g_neighbor_dx[s.direction_index] + s.cur_x;
      s.next_y = g_neighbor_dy[s.direction_index] + s.cur_y;

      s.tile_type = GetWorldTileType(s.next_x, s.next_y);
      if (s.tile_type == 0)
      {
        continue;
      }

      s.score = ApproximateDistance(target_x - s.next_x, target_y - s.next_y);
      if (s.target_dist <= s.score)
      {
        continue;
      }

      if ((s.tile_type == 2) || (s.tile_type == 0xb))
      {
        s.score = s.score + 1;
      }
      if ((s.tile_type == 4) || (s.tile_type == 5))
      {
        s.score = s.score + 4;
      }

      if ((FUN_004314ca(s.next_x, s.next_y) & 0x20) != 0)
      {
        if (s.step_count >= 1)
        {
          s.score = s.score - 4;
        }
      }

      if (s.best_score > s.score)
      {
        s.best_score = s.score;
        s.best_direction = s.direction_index;
      }
    }

    if (s.best_direction == -1)
    {
      BlitGraphicsRect(PTR_DAT_00583304, 0x40, 0, 0x40, 0x80, PTR_DAT_00583304, 0, 0);
      return 0;
    }

    s.next_x = g_neighbor_dx[s.best_direction] + s.cur_x;
    s.next_y = g_neighbor_dy[s.best_direction] + s.cur_y;
    s.flags_masked = FUN_004314ca(s.next_x, s.next_y) & 0x20;

    MarkPathConnection(s.cur_x, s.cur_y, s.best_direction);

    if ((s.flags_masked != 0) && (s.step_count >= 1))
    {
      return 1;
    }

    s.cur_x = s.next_x;
    s.cur_y = s.next_y;
    s.step_count = s.step_count + 1;
  } while ((s.cur_x != target_x) || (s.cur_y != target_y));

  return 1;
}

// FUNCTION: SHANDALAR 0x004f7c7d
void PropagatePathConnectivity(void)
{
  struct
  {
    int scan_x;       /* -0x14 */
    int y;            /* -0x10 */
    int x;            /* -0x0c */
    int found_change; /* -0x08 */
    int scan_y;       /* -0x04 */
  } s;

  PutGraphicsPixel(PTR_DAT_00583304, 0xa8, 0x58, 0xff);
  do
  {
    s.found_change = 0;
    for (s.x = 4; s.x < 0x40; s.x = s.x + 4)
    {
      for (s.y = 4; s.y < 0x40; s.y = s.y + 4)
      {
        if (ReadGraphicsPixel(PTR_DAT_00583304->page_number, s.x + 0x80, s.y + 0x40) != 0)
        {
          FillGraphicsRect(PTR_DAT_00583304, 0x40, 0, 0x40, 0x40, 0);
          FloodFillPathConnectivity(s.x, s.y, 8);
          PutGraphicsPixel(PTR_DAT_00583304, s.x + 0x80, s.y + 0x40, 0);

          for (s.scan_x = 0; s.scan_x < 0x40; s.scan_x = s.scan_x + 1)
          {
            for (s.scan_y = 0; s.scan_y < 0x40; s.scan_y = s.scan_y + 1)
            {
              if (ReadGraphicsPixel(PTR_DAT_00583304->page_number, s.scan_x + 0x40, s.scan_y) != 0)
              {
                if (ReadGraphicsPixel(PTR_DAT_00583304->page_number, s.scan_x + 0x80, s.scan_y) == 0)
                {
                  s.found_change = 1;
                  PutGraphicsPixel(PTR_DAT_00583304, s.scan_x + 0x80, s.scan_y, 0xff);
                  PutGraphicsPixel(PTR_DAT_00583304, s.scan_x + 0x80, s.scan_y + 0x40, 0xff);
                }
              }
            }
          }
        }
      }
    }
  } while (s.found_change != 0);

  for (s.x = 0; s.x < 0x40; s.x = s.x + 1)
  {
    for (s.y = 0; s.y < 0x40; s.y = s.y + 1)
    {
      if (ReadGraphicsPixel(PTR_DAT_00583304->page_number, s.x + 0x80, s.y) == 0)
      {
        PutGraphicsPixel(PTR_DAT_00583304, s.x, s.y, 0);
      }
    }
  }
}

// FUNCTION: SHANDALAR 0x004f7eb2
void FloodFillPathConnectivity(int x, int y, unsigned int depth)
{
  struct
  {
    char tile_value; /* -0x10 */
    char pad_0d[3];
    char direction_index; /* -0x0c */
    char pad_09[3];
    char next_y; /* -0x08 */
    char pad_05[3];
    char next_x; /* -0x04 */
    char pad_01[3];
  } s;

  PutGraphicsPixel(PTR_DAT_00583304, x + 0x40, y, depth);
  if ((int)depth <= 1)
  {
  }
  else
  {
    for (s.direction_index = 1; (int)s.direction_index <= 8; s.direction_index = s.direction_index + 2)
    {
      s.next_x = (char)(g_neighbor_dx[(int)s.direction_index] + x);
      s.next_y = (char)(g_neighbor_dy[(int)s.direction_index] + y);
      s.tile_value = (char)ReadGraphicsPixel(PTR_DAT_00583304->page_number, s.next_x + 0x40, s.next_y);
      if ((int)s.tile_value < (int)depth)
      {
        if (ReadGraphicsPixel(PTR_DAT_00583304->page_number, s.next_x, s.next_y) != 0)
        {
          FloodFillPathConnectivity(s.next_x, s.next_y, depth - 1);
        }
      }
    }
  }
}

// FUNCTION: SHANDALAR 0x004f7fb9
int FUN_004f7fb9(int param_1, int param_2)
{
  int entry_index;
  int local_4;

  entry_index = 0x10 / (param_1 + 2);
  entry_index = entry_index + 0x10 / (param_2 + 2);
  entry_index = entry_index + 0x10 / (0x41 - param_1);
  entry_index = entry_index + 0x10 / (0x41 - param_2);

  entry_index = abs(param_1 - 0x20) + abs(param_2 - 0x20);
  entry_index = (entry_index * entry_index) / 512;
  entry_index += abs(param_1 - param_2) / 16;

  param_1 = param_1 << 5;
  param_2 = param_2 << 5;
  local_4 = FUN_004f82f2(param_1, param_2) << 2;

  local_4 = local_4 + FUN_004f82f2(param_1 << 3, param_2 << 3) * 2;
  local_4 = local_4 + FUN_004f82f2(param_1 << 4, param_2 << 4);
  local_4 = local_4 - ClampIntToRange(entry_index, 0, 0xc) * 0x200;

  return ClampIntToRange(((7 * local_4) / 64) / 4, 0, 100);
}

// FUNCTION: SHANDALAR 0x004f8101
void InitializeAnimatedNoiseGrid(void)
{
  struct
  {
    int weighted_neighbor_sum;
    int x;
    int direction_index;
    int y;
  } s;

  for (s.y = 0; s.y < 0x12; s.y = s.y + 1)
  {
    for (s.x = 0; s.x < 0x12; s.x = s.x + 1)
    {
      g_animated_noise_grid[s.x + s.y * 0x13] = (char)RandomIntLessThan(0x10);
    }
    g_animated_noise_grid[s.y * 0x13 + 0x12] = g_animated_noise_grid[s.y * 0x13];
  }
  for (s.x = 0; s.x < 0x12; s.x = s.x + 1)
  {
    g_animated_noise_grid[s.x + 0x156] = g_animated_noise_grid[s.x];
  }
  for (s.y = 0; s.y <= 0x10; s.y = s.y + 1)
  {
    for (s.x = 0; s.x <= 0x10; s.x = s.x + 1)
    {
      s.weighted_neighbor_sum = (int)g_animated_noise_grid[s.x + s.y * 0x13] << 2;
      for (s.direction_index = 1; s.direction_index <= 8; s.direction_index = s.direction_index + 1)
      {
        s.weighted_neighbor_sum +=
            (int)g_animated_noise_grid[s.x + g_neighbor_dy[s.direction_index] + (s.y + g_neighbor_dx[s.direction_index] + 1) * 0x13 + 1];
      }
      g_animated_noise_grid[s.x + s.y * 0x13 + 0x170] = g_animated_noise_grid[s.x + s.y * 0x13];
    }
  }
  for (s.y = 0; s.y <= 0x10; s.y = s.y + 1)
  {
    g_animated_noise_grid[s.y * 0x13 + 0x180] = g_animated_noise_grid[s.y * 0x13 + 0x170];
  }
  for (s.x = 0; s.x <= 0x10; s.x = s.x + 1)
  {
    g_animated_noise_grid[s.x + 0x2a0] = g_animated_noise_grid[s.x + 0x170];
  }
}

// FUNCTION: SHANDALAR 0x004f82f2
int FUN_004f82f2(int param_1, int param_2)
{
  int local_4;
  unsigned int entry_index;
  unsigned int inner_index;
  unsigned int location_block_start_index;
  unsigned int selected_state_sprite;

  param_1 = param_1 - 0x80;
  param_2 = param_2 - 0x80;
  entry_index = (param_1 >> 8) & 0xf;
  location_block_start_index = (param_1 & 0xff) >> 3;
  inner_index = (param_2 >> 8) & 0xf;
  selected_state_sprite = (param_2 & 0xff) >> 3;

  local_4 = (int)g_animated_noise_grid[inner_index + entry_index * 0x13 + 0x170] * (0x20 - selected_state_sprite) * (0x20 - location_block_start_index);
  local_4 = local_4 + (int)g_animated_noise_grid[inner_index + (entry_index + 1) * 0x13 + 0x170] * (0x20 - selected_state_sprite) * location_block_start_index;
  local_4 = local_4 + (int)g_animated_noise_grid[inner_index + entry_index * 0x13 + 0x171] * (0x20 - location_block_start_index) * selected_state_sprite;
  local_4 = local_4 + (int)g_animated_noise_grid[inner_index + (entry_index + 1) * 0x13 + 0x171] * selected_state_sprite * location_block_start_index;
  return (local_4 + (local_4 >> 0x1f & 0x1fU)) >> 5;
}

// FUNCTION: SHANDALAR 0x005081fa
void InitializeCastleDungeonSlots(void)
{
  struct
  {
    int card_index;
    int nearest_distance;
    int nearest_town_index;
    int scan_index;
    int candidate_y;
    int candidate_x;
    int distance;
    int mana_castle_town_index;
    int inner_index;
    int entry_index;
  } s;

  for (s.entry_index = 0; s.entry_index < 0xf; s.entry_index = s.entry_index + 1)
  {
    g_castle_dungeon_slots[s.entry_index].card_slot_3 = -1;
    g_castle_dungeon_slots[s.entry_index].card_slot_2 = g_castle_dungeon_slots[s.entry_index].card_slot_3;
    g_castle_dungeon_slots[s.entry_index].card_slot_1 = g_castle_dungeon_slots[s.entry_index].card_slot_2;
    g_castle_dungeon_slots[s.entry_index].times_entered = -1;
    g_castle_dungeon_slots[s.entry_index].reserved_2c = g_castle_dungeon_slots[s.entry_index].times_entered;
  }

  s.entry_index = 0;
  for (s.card_index = 0; s.card_index < g_card_count - 0x39; s.card_index = s.card_index + 1)
  {
    if ((global_cards_data[s.card_index].extra_ability & 0x100) == 0)
    {
      continue;
    }

    do
    {
      s.entry_index = RandomIntLessThan(10) + 5;
    } while (g_castle_dungeon_slots[s.entry_index].card_slot_3 != -1);

    if (g_castle_dungeon_slots[s.entry_index].card_slot_1 == -1)
    {
      g_castle_dungeon_slots[s.entry_index].card_slot_1 = s.card_index;
    }
    else if (g_castle_dungeon_slots[s.entry_index].card_slot_2 == -1)
    {
      g_castle_dungeon_slots[s.entry_index].card_slot_2 = s.card_index;
    }
    else
    {
      g_castle_dungeon_slots[s.entry_index].card_slot_3 = s.card_index;
    }
  }

  for (s.entry_index = 0; s.entry_index < 0xf; s.entry_index = s.entry_index + 1)
  {
    do
    {
      do
      {
        do
        {
          s.candidate_x = RandomIntLessThan(0x40);
          s.candidate_y = RandomIntLessThan(0x40);
        } while (GetWorldTileType(s.candidate_x, s.candidate_y) == 0);
      } while ((FUN_004314ca(s.candidate_x, s.candidate_y) & 0x30) != 0);

      s.nearest_distance = 0xff;
      for (s.scan_index = 0; s.scan_index < 0x80; s.scan_index = s.scan_index + 1)
      {
        if ((s.entry_index < 5) && (g_town_slots[s.scan_index].location_type == 4))
        {
          if (GetWorldTileMagicMask(GetWorldTileType(g_town_slots[s.scan_index].world_x, g_town_slots[s.scan_index].world_y)) == 1 << (s.entry_index + 1))
          {
            s.mana_castle_town_index = s.scan_index;
          }
        }

        if ((g_town_slots[s.scan_index].location_type <= 1) || (g_town_slots[s.scan_index].location_type == 4))
        {
          continue;
        }

        s.distance = ApproximateDistance(g_town_slots[s.scan_index].world_x - s.candidate_x,
                                  g_town_slots[s.scan_index].world_y - s.candidate_y);
        if (s.nearest_distance > s.distance)
        {
          s.nearest_distance = s.distance;
          s.nearest_town_index = s.scan_index;
        }
      }

      for (s.scan_index = 0; s.scan_index < s.entry_index; s.scan_index = s.scan_index + 1)
      {
        s.distance = ApproximateDistance(g_castle_dungeon_slots[s.scan_index].world_x - s.candidate_x, g_castle_dungeon_slots[s.scan_index].world_y - s.candidate_y);
        if (s.nearest_distance > s.distance)
        {
          s.nearest_distance = s.distance;
        }
      }
    } while (s.nearest_distance < 4);

    SetWorldMapPixelFlags(0x40, s.candidate_x, s.candidate_y);
    g_castle_dungeon_slots[s.entry_index].world_x = s.candidate_x;
    g_castle_dungeon_slots[s.entry_index].world_y = s.candidate_y;
    g_castle_dungeon_slots[s.entry_index].north_of_town_index = s.nearest_town_index;
    g_castle_dungeon_slots[s.entry_index].color = (unsigned char)(RandomIntLessThan(5) + 1);
    g_castle_dungeon_slots[s.entry_index].monster_flags = 2;

    if (s.entry_index < 5)
    {
      g_castle_dungeon_slots[s.entry_index].color = s.entry_index + 1;
      g_castle_dungeon_slots[s.entry_index].monster_flags = 0x81;
      g_castle_dungeon_slots[s.entry_index].world_x = g_town_slots[s.mana_castle_town_index].world_x;
      g_castle_dungeon_slots[s.entry_index].world_y = g_town_slots[s.mana_castle_town_index].world_y;
    }

    s.inner_index = 0;
    if ((int)(unsigned int)(unsigned char)g_castle_dungeon_slots[s.entry_index].monster_flags <= RandomIntLessThan(2) + 1)
    {
      if (1 < (int)(unsigned int)(unsigned char)g_castle_dungeon_slots[s.entry_index].monster_flags)
      {
        s.inner_index = 0x1c;
      }
      else
      {
        s.inner_index = 0x10;
      }
      g_castle_dungeon_slots[s.entry_index].monster_flags = g_castle_dungeon_slots[s.entry_index].monster_flags | 0x80;
    }
    else
    {
      s.inner_index = 0;
      for (s.scan_index = 0; s.scan_index < (int)(unsigned int)(unsigned char)g_castle_dungeon_slots[s.entry_index].monster_flags; s.scan_index = s.scan_index + 1)
      {
        s.inner_index = s.inner_index + s.scan_index * 2 + 4;
      }
    }

    if (g_castle_dungeon_slots[s.entry_index].card_slot_2 == -1)
    {
      s.inner_index = (s.inner_index * 3) / 2;
    }
    if (g_castle_dungeon_slots[s.entry_index].card_slot_3 != -1)
    {
      s.inner_index = (s.inner_index * 2) / 3;
    }

    g_castle_dungeon_slots[s.entry_index].card_in_effect =
        g_dungeon_card_effect_by_monster_flags[((g_castle_dungeon_slots[s.entry_index].monster_flags & 0xc0) != 0 ? 4 : 0) +
                                               (g_castle_dungeon_slots[s.entry_index].monster_flags & 0x7f) + RandomIntLessThan(2)];
    g_castle_dungeon_slots[s.entry_index].rules_bitmap = 1;

    switch (s.inner_index / 4)
    {
    case 0:
    case 1:
    case 2:
      g_castle_dungeon_slots[s.entry_index].monster_flags++;
    case 3:
      g_castle_dungeon_slots[s.entry_index].card_in_effect = g_dungeon_card_effect_by_color[(char)g_castle_dungeon_slots[s.entry_index].color];
      g_castle_dungeon_slots[s.entry_index].rules_bitmap = g_castle_dungeon_slots[s.entry_index].rules_bitmap | 1 << (char)(RandomIntLessThan(5) + 4);
      break;
    case 4:
      g_castle_dungeon_slots[s.entry_index].card_in_effect = g_dungeon_card_effect_by_color[(char)g_castle_dungeon_slots[s.entry_index].color];
      break;
    case 5:
      g_castle_dungeon_slots[s.entry_index].rules_bitmap = g_castle_dungeon_slots[s.entry_index].rules_bitmap | 1 << (char)(RandomIntLessThan(5) + 4);
      g_castle_dungeon_slots[s.entry_index].card_in_effect = -1;
      break;
    case 6:
      g_castle_dungeon_slots[s.entry_index].card_in_effect = -1;
      break;
    }

    if (((g_castle_dungeon_slots[s.entry_index].rules_bitmap & 0x100) != 0) ||
        (((g_castle_dungeon_slots[s.entry_index].monster_flags & 0x3f) < 2) && ((g_castle_dungeon_slots[s.entry_index].rules_bitmap & 0x20) != 0)))
    {
      g_castle_dungeon_slots[s.entry_index].card_in_effect = g_dungeon_card_effect_by_color[(char)g_castle_dungeon_slots[s.entry_index].color];
      g_castle_dungeon_slots[s.entry_index].rules_bitmap = g_castle_dungeon_slots[s.entry_index].rules_bitmap & 0xfffffedf;
    }

    if ((g_castle_dungeon_slots[s.entry_index].monster_flags & 0x7f) == 1)
    {
      g_castle_dungeon_slots[s.entry_index].rules_bitmap = g_castle_dungeon_slots[s.entry_index].rules_bitmap & 0xfffffffe;
    }

    if (s.entry_index < 5)
    {
      g_castle_dungeon_slots[s.entry_index].rules_bitmap = g_castle_dungeon_slots[s.entry_index].rules_bitmap | 1;
      g_castle_dungeon_slots[s.entry_index].rules_bitmap = g_castle_dungeon_slots[s.entry_index].rules_bitmap | 2;
      g_castle_dungeon_slots[s.entry_index].card_in_effect = g_mana_castle_card_effect_by_difficulty[s.entry_index][g_shandalar_difficulty];
    }
  }
}

// FUNCTION: SHANDALAR 0x00431351
void DrawEncodedImageUiScaled(FacemakerWindowBounds *dst, int x_320, int y_200, EncodedImage *sprite, int width_320, int height_200)
{
  x_320 = (x_320 * global_screen_width) / 0x280;
  y_200 = (y_200 * global_screen_height) / 0x1e0;
  width_320 = (width_320 * global_screen_width) / 0x280;
  height_200 = (height_200 * global_screen_height) / 0x1e0;
  DrawEncodedImageResampled(dst, x_320, y_200, width_320, height_200, sprite);
}

// FUNCTION: SHANDALAR 0x0043146b
unsigned int GetWorldTileType(int x, int y)
{
  if ((0x40 <= x) || (x < 0))
  {
    x = 0;
  }
  if ((0x40 <= y) || (y < 0))
  {
    y = 0;
  }
  return GetGraphicsPixelColorRef(PTR_DAT_00583304, x, y) & 0xf;
}

// FUNCTION: SHANDALAR 0x005611c8
unsigned int GetWorldTileMagicMask(unsigned int tile_type)
{
  unsigned int entry_index;

  switch (tile_type)
  {
  case 3:
    entry_index = 2;
    break;
  case 0xb:
    entry_index = 10;
    break;
  case 1:
    entry_index = 4;
    break;
  case 2:
    entry_index = 8;
    break;
  case 4:
    entry_index = 0x30;
    break;
  case 5:
    entry_index = 0x10;
    break;
  case 6:
    entry_index = 0x20;
    break;
  case 10:
    entry_index = 0x28;
    break;
  case 0xf:
    entry_index = 0x18;
    break;
  case 7:
    entry_index = 0x24;
    break;
  case 0xc:
    entry_index = 0x12;
    break;
  case 0xd:
    entry_index = 0x22;
    break;
  case 0xe:
    entry_index = 0xc;
    break;
  case 9:
    entry_index = 0x14;
    break;
  case 8:
    entry_index = 6;
    break;
  default:
    entry_index = 0;
    break;
  }

  return entry_index;
}

// FUNCTION: SHANDALAR 0x004314ca
unsigned int FUN_004314ca(int x, int y)
{
  if (x >= 0x40 || x < 0)
    return 0;

  if (y >= 0x40 || y < 0)
    return 0;

  return GetGraphicsPixelColorRef(PTR_DAT_00583304, x, y);
}

// FUNCTION: SHANDALAR 0x00431526
void SetWorldMapPixelFlags(unsigned int param_1, int x, int y)
{
  if (x >= 0x40 || x < 0)
    return;

  if (y >= 0x40 || y < 0)
    return;

  PutGraphicsPixel(PTR_DAT_00583304, x, y, GetGraphicsPixelColorRef(PTR_DAT_00583304, x, y) | param_1);
}

// FUNCTION: SHANDALAR 0x00431593
void ClearWorldMapPixelFlags(unsigned int param_1, int x, int y)
{
  if (x >= 0x40 || x < 0)
    return;

  if (y >= 0x40 || y < 0)
    return;

  PutGraphicsPixel(PTR_DAT_00583304, x, y, GetGraphicsPixelColorRef(PTR_DAT_00583304, x, y) & ~param_1);
}

// FUNCTION: SHANDALAR 0x0043174d
void MarkPathConnection(int x, int y, int direction_index)
{
  if (x >= 0x40 || x < 0)
    return;

  if (y >= 0x40 || y < 0)
    return;

  PutGraphicsPixel(PTR_DAT_00583304, x, y + 0x40,
                   GetGraphicsPixelColorRef(PTR_DAT_00583304, x, y + 0x40) | (1 << (direction_index - 1)));
  SetWorldMapPixelFlags(0x20, x, y);

  x += g_neighbor_dx[direction_index];
  y += g_neighbor_dy[direction_index];
  direction_index = ((direction_index + 3) & 7) + 1;

  PutGraphicsPixel(PTR_DAT_00583304, x, y + 0x40,
                   GetGraphicsPixelColorRef(PTR_DAT_00583304, x, y + 0x40) | (1 << (direction_index - 1)));
  SetWorldMapPixelFlags(0x20, x, y);
}

// FUNCTION: SHANDALAR 0x005019ad
void SaveGameToSlot(int save_slot_index)
{
#ifdef _DEBUG
  return;
#endif
  int save_drive_index;
  g_save_errno = 0;
  global_saveload_loading = g_save_errno;
  FUN_0046ed33();
  save_drive_index = GetSaveDriveIndex();
  if (save_drive_index != -1)
  {
    if (save_slot_index == -1)
    {
      FUN_0046ed03();
      save_slot_index = RunTextMenuAt(g_ui_message_buffer, 0x30, 0x20);
      FUN_0046ed33();
    }

    if (save_slot_index != -1)
    {
      g_selected_save_slot_index = save_slot_index;
      g_save_file_path[7] = (char)FUN_004ece40(save_slot_index);
      if (SaveGameWithMessage(g_save_file_path) != 0)
      {
        if (g_save_errno == 0)
        {
          strcpy(g_ui_message_buffer, " Game has been saved.\n");
        }
        else
        {
          strcpy(g_ui_message_buffer, " Game NOT saved.\n");
          FillGraphicsRect(PTR_DAT_005832b4, 0x40, 0x7f, 0xc0, 0x22, 0xc);
        }

        if (g_save_errno == 0xd)
        {
          strcat(g_ui_message_buffer, " Write access denied.\n");
        }

        if (g_save_errno == 0x1c)
        {
          strcat(g_ui_message_buffer, " Disk Full.\n");
        }
        strcat(g_ui_message_buffer, " Press key to continue.\n");
      }
    }
  }
  FUN_0046ed03();
}

// FUNCTION: SHANDALAR 0x005031a8
int FUN_005031a8(void)
{
  return RunLoadSaveMenu(0);
}

// FUNCTION: SHANDALAR 0x00501760
int LoadGameFromSlot(int save_slot_index)
{
  int save_drive_index;
  unsigned int slot_index;

  global_saveload_loading = 1;
  FUN_0046ed33();
  save_drive_index = GetSaveDriveIndex();
  if (save_drive_index != -1)
  {
    if (save_slot_index == -1)
    {
      strcpy(g_ui_message_buffer, "\x8cSelect Load File...\n");
      g_load_menu_valid_slot_mask_0077f1c8 = 0;

      for (slot_index = 0; slot_index < 10; slot_index = slot_index + 1)
      {
        g_save_file_path[7] = (char)FUN_004ece40(slot_index);
        if (ValidateOrLoadSaveGame(g_save_file_path, 1) != 0)
        {
          g_load_menu_valid_slot_mask_0077f1c8 |= (1 << (unsigned char)slot_index);
        }
      }

      FUN_0046ed03();
      g_selected_save_slot_index = RunTextMenuAt(g_ui_message_buffer, 0x30, 0x40);
      FUN_0046ed33();
      if ((g_load_menu_valid_slot_mask_0077f1c8 & (1 << (unsigned char)g_selected_save_slot_index)) == 0)
      {
        g_selected_save_slot_index = -1;
      }
    }
    else
    {
      g_selected_save_slot_index = save_slot_index;
    }

    if (g_selected_save_slot_index != -1)
    {
      g_save_file_path[7] = (char)FUN_004ece40(g_selected_save_slot_index);
      if (ValidateOrLoadSaveGame(g_save_file_path, 0) == 0)
      {
        g_selected_save_slot_index = -1;
      }
    }

    if (g_selected_save_slot_index == -1)
    {
      RunTextMenuAt("Error Loading Save Game File:EXITING\n ", 100, 0x50);
      exit(1);
    }

    FUN_0046ed03();
    return g_selected_save_slot_index;
  }
  else
  {
    FUN_0046ed03();
    return -1;
  }
}

// FUNCTION: SHANDALAR 0x004ece40
int FUN_004ece40(int param_1)
{
  if ((0 <= param_1) && (param_1 <= 9))
  {
    return param_1 + 0x30;
  }
  else if ((10 <= param_1) && (param_1 <= 0xf))
  {
    return param_1 + 0x57;
  }
  else
  {
    return 0;
  }
}

// FUNCTION: SHANDALAR 0x00501b7d
int GetSaveDriveIndex(void)
{
  char current_directory_buffer[0x104];

  if (g_save_path_needs_init == -1)
  {
    GetCurrentDirectoryA(0x100, current_directory_buffer);
    g_save_file_path[0] = current_directory_buffer[0];
  }

  return tolower(g_save_file_path[0]) - 0x61;
}

// FUNCTION: SHANDALAR 0x00501e44
int LoadGameFromPath(char *save_file_path)
{
  struct
  {
    int saved_clip_rect[4];
    FacemakerWindowBounds page4_bounds;
    FacemakerWindowBounds *page4_bounds_ptr;
    DIBSurface *page4_dib;
    int image_height;
    int image_width;
    int slot_index;
    int player_index;
  } s;

  strcpy(save_file_path + 9, "SVE");
  g_save_file_fd = _open(save_file_path, 0x8000);
  if (g_save_file_fd == -1)
  {
    strcpy(g_ui_message_buffer, "File Error: ");
    strcat(g_ui_message_buffer, save_file_path);
    strcat(g_ui_message_buffer, "\n");
    RunTextMenuAt(g_ui_message_buffer, 100, 0x50);
    return 0;
  }

  global_saveload_loading = 1;
  save_or_load_ver1();
  _close(g_save_file_fd);

  for (s.player_index = 0; s.player_index < 2; s.player_index = s.player_index + 1)
  {
    for (s.slot_index = 0; s.slot_index < 0x96; s.slot_index = s.slot_index + 1)
    {
      if (global_card_instances[s.player_index][s.slot_index].internal_card_id != -1)
      {
        active_cards_count[s.player_index] = s.slot_index;
      }
    }
  }

  if (unk_00742fc4 == 0)
  {
    FUN_0046ed33();
    strcpy(save_file_path + 9, "map");
    LoadPcxIntoPage(2, save_file_path);
    FUN_0046ed03();
  }

  s.page4_bounds.page_number = 4;
  s.page4_bounds.clip_left = 0;
  s.page4_bounds.clip_top = 0;
  s.page4_bounds.max_x = 800;
  s.page4_bounds.max_y = 600;
  s.page4_bounds.draw_shadow_enabled = 1;
  s.page4_bounds.text_color = 0xf;
  s.page4_bounds.unk_1c = 4;
  s.page4_bounds.font_slot = 0;
  s.page4_bounds_ptr = &s.page4_bounds;
  s.image_width = 0x89;
  s.image_height = 0xa9;
  if (g_facemaker_page4_dib == 0)
  {
    s.page4_dib = (DIBSurface *)CreateGraphicsPage(4, s.image_width * 2, s.image_height, 8);
    SetGraphicsPage(4, s.page4_dib);
  }
  else
  {
    g_graphics_pages[4] = g_facemaker_page4_dib;
  }

  PushGraphicsClipRect((AdvMenuRect *)s.saved_clip_rect, s.page4_bounds_ptr, 0, 0, s.image_width * 2, s.image_height);
  FillGraphicsRect(s.page4_bounds_ptr, 0, 0, s.image_width * 2, s.image_height, 0);
  strcpy(save_file_path + 9, "fce");
  LoadPcxIntoPage(4, save_file_path);
  BuildFacemakerPortraitSprites(&s.page4_bounds);
  LoadPcxIntoPage(4, save_file_path);
  g_facemaker_page4_bitmap = (int)g_graphics_pages[4]->hBitmap;
  g_facemaker_page4_dib = (int)g_graphics_pages[4];
  SelectObject(g_graphics_pages[4]->hTempDC, g_graphics_pages[4]->hPreviousBitmap);
  g_graphics_pages[4] = (DIBSurface *)0;
  return 1;
}

// FUNCTION: SHANDALAR 0x005018e8
unsigned int ValidateOrLoadSaveGame(char *save_file_path, int validate_only)
{
  strcpy(save_file_path + 9, "SVE");
  if (validate_only != 0)
  {
    g_save_file_fd = open(save_file_path, 0x8000);
    if (g_save_file_fd != -1)
    {
      strcat(g_ui_message_buffer, "OK\n");
    }
    else
    {
      sprintf(g_ui_message_buffer, "%s\n", gs_loadsave_0077d1b0[2]);
    }

    close(g_save_file_fd);
    return (g_save_file_fd != -1) ? 1 : 0;
  }

  return (unsigned int)LoadGameFromPath(save_file_path);

  return 1;
}

// FUNCTION: SHANDALAR 0x005020fe
int SaveGameToPath(char *save_file_path)
{
  struct
  {
    FacemakerWindowBounds page4_bounds;
    FacemakerWindowBounds *page4_bounds_ptr;
    int save_succeeded;
  } s;

  if (unk_00742fc4 == 0)
  {
    strcpy(save_file_path + 9, "map");
    if (ExportGraphicsPage(2, save_file_path) != 0)
    {
      strcpy(g_ui_message_buffer, "Error writing map file.\n");
      RunTextMenuAt(g_ui_message_buffer, 4, 0x40);
      g_save_errno = 1;
      return 0;
    }
  }

  strcpy(save_file_path + 9, "SVE");
  g_save_file_fd = _open(save_file_path, 0x8301, 0x80);
  if (g_save_file_fd == -1)
  {
    strcpy(g_ui_message_buffer, "File Error: ");
    strcat(g_ui_message_buffer, save_file_path);
    strcat(g_ui_message_buffer, "\n");
    RunTextMenuAt(g_ui_message_buffer, 100, 0x50);
    return 0;
  }

  global_saveload_loading = 0;
  save_or_load_ver1();
  _close(g_save_file_fd);

  s.page4_bounds.page_number = 4;
  s.page4_bounds.clip_left = 0;
  s.page4_bounds.clip_top = 0;
  s.page4_bounds.max_x = 800;
  s.page4_bounds.max_y = 600;
  s.page4_bounds.draw_shadow_enabled = 1;
  s.page4_bounds.text_color = 0xf;
  s.page4_bounds.unk_1c = 4;
  s.page4_bounds.font_slot = 0;
  s.page4_bounds_ptr = &s.page4_bounds;

  g_graphics_pages[4] = g_facemaker_page4_dib;
  SelectObject(g_facemaker_page4_dib->hTempDC, g_facemaker_page4_bitmap);

  strcpy(save_file_path + 9, "fce");
  ExportEncodedImage(4, 0, 0, g_facemaker_page4_dib->width, g_facemaker_page4_dib->height, 0, save_file_path);

  SelectObject(g_graphics_pages[4]->hTempDC, g_graphics_pages[4]->hPreviousBitmap);
  g_graphics_pages[4] = (DIBSurface *)0;

  s.save_succeeded = 0;
  if (g_save_errno == 0)
  {
    s.save_succeeded = 1;
  }

  return s.save_succeeded;
}

// FUNCTION: SHANDALAR 0x00501aec
int SaveGameWithMessage(char *save_file_path)
{
  strcpy(g_ui_message_buffer, " ");
  strcat(g_ui_message_buffer, "\n ... save in progress.\n");
  SaveGameToPath(save_file_path);
  return 1;
}

// FUNCTION: SHANDALAR 0x0040dffd
int GetFirstManaColorIndex(int color_mask)
{
  if (color_mask & 2)
    return 1;

  if (color_mask & 4)
    return 2;

  if (color_mask & 8)
    return 3;

  if (color_mask & 0x10)
    return 4;

  if (color_mask & 0x20)
    return 5;

  return 0;
}

// FUNCTION: SHANDALAR 0x005226e0
int LoadStatWinDllExports(void)
{
  int i;

  for (i = 0; i < 3; i = i + 1)
  {
    g_statwin_exports_by_ordinal[i] = 0;
  }

  g_statwin_dll_module = LoadLibraryA("statwin.dll");
  if (g_statwin_dll_module == (HANDLE)0)
  {
    return 1;
  }

  for (i = 0; i < 3; i = i + 1)
  {
    g_statwin_exports_by_ordinal[i] = (int)GetProcAddress((HMODULE)g_statwin_dll_module, (LPCSTR)((i + 1U) & 0xffff));
  }

  return 0;
}

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
    DAT_0078df40[s.ttsprite_col_index] = s.ttsprite_entries[s.ttsprite_entry_index];
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

  DAT_00748f14 = g_location_marker_sprite_entries[s.location_block_start_index + 2];
  DAT_00748f20 = g_location_marker_sprite_entries[s.location_block_start_index + 3];
  DAT_00748f18 = g_location_marker_sprite_entries[s.location_block_start_index + 8];
  DAT_00748f10 = g_location_marker_sprite_entries[s.location_block_start_index + 10];
  DAT_00749418 = g_location_marker_sprite_entries[s.location_block_start_index + 6];
  s.entry_index += ReadSpriteEntryPointers(&g_location_marker_sprite_entries[s.entry_index], BuildResolutionSpritePath("locatn05.spr"));
  s.location_block_start_index = s.entry_index;
  s.entry_index += ReadSpriteEntryPointers(&g_location_marker_sprite_entries[s.entry_index], BuildResolutionSpritePath("locatn06.spr"));

  DAT_00748f1c = g_location_marker_sprite_entries[s.location_block_start_index];

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

  s.entry_index = ReadSpriteEntryPointers(&g_opening_menu_sprite_work_buffer[0x10].sprites[0], BuildResolutionSpritePath((g_player_is_male == 0) ? "ego_f.spr" : "ego_m.spr"));
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
    DrawEncodedImageResampled(g_menu_control_draw_target_page, control->x, control->y, control->width, control->height, (EncodedImage *)control->mode_data[event_type]);
  }
  else
  {
    preview_panel_y_offset = ScaleUiCoordinate(4);
    avatar_sprite_index = ScaleUiCoordinate(8);
    BlitGraphicsRect(PTR_DAT_00583354, (unsigned int)control->x, control->y, (unsigned int)control->width, (DWORD)control->height, PTR_DAT_005832dc,
                     control->x, control->y);
    DrawEncodedImageResampled(PTR_DAT_005832dc, control->x + preview_panel_y_offset, control->y + preview_panel_y_offset, control->width - avatar_sprite_index, control->height - avatar_sprite_index,
                              (EncodedImage *)control->mode_data[event_type]);
    BlitGraphicsRect(PTR_DAT_005832dc, (unsigned int)control->x, control->y, (unsigned int)control->width, (DWORD)control->height, PTR_DAT_005832b4,
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
    int local_y;
    DWORD local_height;
    int local_x;
    unsigned int local_width;
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
  s.local_height = (DWORD)ScaleUiCoordinate(0x30);
  s.local_width = (unsigned int)ScaleUiCoordinate((s.sprite->width * 0x30) / (int)s.sprite->height);
  s.local_x = ScaleUiCoordinate(0x20) - (int)s.local_width / 2;
  s.local_y = ScaleUiCoordinate(0x106) - (int)s.local_height / 2;
  if (event_type != 2)
  {
    DrawEncodedImageResampled(g_menu_control_draw_target_page, s.local_x, s.local_y, s.local_width, (int)s.local_height, s.sprite);
  }
  else
  {
    s.avatar_sprite_index = ScaleUiCoordinate(4);
    s.clamped_required_wins = ScaleUiCoordinate(8);
    BlitGraphicsRect(PTR_DAT_00583354, s.local_x, s.local_y, s.local_width, s.local_height, PTR_DAT_005832dc, s.local_x, s.local_y);
    DrawEncodedImageResampled(PTR_DAT_005832dc, s.avatar_sprite_index + s.local_x, s.avatar_sprite_index + s.local_y, s.local_width - s.clamped_required_wins, (int)s.local_height - s.clamped_required_wins, s.sprite);
    BlitGraphicsRect(PTR_DAT_005832dc, s.local_x, s.local_y, s.local_width, s.local_height, PTR_DAT_005832b4, s.local_x, s.local_y);
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
    s.world_magic_unlock_score = FUN_004bb458(s.world_magic_slot_index);
    if (g_amulet_inventory[s.world_magic_slot_index / 2 - 1] != 0)
    {
      if (event_type != 2)
      {
        DrawEncodedImageUiScaled(PTR_DAT_005832b4, g_world_magic_icon_rects[s.world_magic_slot_index].x, g_world_magic_icon_rects[s.world_magic_slot_index].y,
                                 (EncodedImage *)g_world_magic_choice_controls[control_ptr->selection_value - 0x31].mode_data[event_type],
                                 g_world_magic_icon_rects[s.world_magic_slot_index].width, g_world_magic_icon_rects[s.world_magic_slot_index].height);
      }
      else
      {
        s.selected_state_sprite = (EncodedImage *)g_world_magic_choice_controls[control_ptr->selection_value - 0x31].mode_data[event_type];
        s.icon_x_scaled = (unsigned int)ScaleUiCoordinate(g_world_magic_icon_rects[s.world_magic_slot_index].x);
        s.icon_y_scaled = ScaleUiCoordinate(g_world_magic_icon_rects[s.world_magic_slot_index].y);
        s.icon_width_scaled = (unsigned int)ScaleUiCoordinate(g_world_magic_icon_rects[s.world_magic_slot_index].width);
        s.icon_height_scaled = (DWORD)ScaleUiCoordinate(g_world_magic_icon_rects[s.world_magic_slot_index].height);
        s.icon_border_padding = ScaleUiCoordinate(4);
        BlitGraphicsRect(PTR_DAT_0058332c, s.icon_x_scaled, s.icon_y_scaled - ScaleUiCoordinate(0x148), s.icon_width_scaled, s.icon_height_scaled, PTR_DAT_005832dc, s.icon_x_scaled, s.icon_y_scaled);
        DrawEncodedImageUiScaled(PTR_DAT_005832dc, g_world_magic_icon_rects[s.world_magic_slot_index].x, g_world_magic_icon_rects[s.world_magic_slot_index].y, (EncodedImage *)g_world_magic_choice_button_sprite_bank.named.icon[s.world_magic_slot_index],
                                 g_world_magic_icon_rects[s.world_magic_slot_index].width, g_world_magic_icon_rects[s.world_magic_slot_index].height);
        DrawEncodedImageResampled(PTR_DAT_005832dc,
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
        PTR_DAT_005832b4->font_slot = 4;
        s.avatar_draw_y -= (int)s.avatar_sprite->height / 2;
        DrawEncodedImageUiScaled(PTR_DAT_005832dc, s.avatar_x_positions[s.avatar_sprite_draw_order[5]] - 0x1e, s.avatar_draw_y, g_world_magic_avatar_sprites[s.avatar_sprite_draw_order[s.avatar_sprite_draw_order[5]]], (int)s.avatar_sprite->width,
                                 (int)s.avatar_sprite->height);
        BlitGraphicsRect(PTR_DAT_005832dc, s.icon_x_scaled, s.icon_y_scaled, s.icon_width_scaled, s.icon_height_scaled, PTR_DAT_005832b4, s.icon_x_scaled, s.icon_y_scaled);
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

  g_menu_saved_window_bounds = *PTR_DAT_005832b4;

  for (i = 0; i < 4; i = i + 1)
  {
    g_main_menu_controls[i].mode_data[0] = (int)g_main_menu_button_sprites_normal[i];
    g_main_menu_controls[i].mode_data[1] = (int)g_main_menu_button_sprites_highlight[i];
    g_main_menu_controls[i].mode_data[2] = (int)g_main_menu_button_sprites_highlight[i];
    g_main_menu_controls[i].mode_data[3] = (int)g_main_menu_button_sprites_normal[i];
  }

  for (i = 0; i < 5; i = i + 1)
  {
    sprite_index = i * 2 + 2;
    g_world_magic_choice_controls[i].mode_data[0] = (int)g_world_magic_choice_button_sprite_bank.named.normal[sprite_index];
    g_world_magic_choice_controls[i].mode_data[1] = (int)g_world_magic_choice_button_sprite_bank.named.highlight[sprite_index];
    g_world_magic_choice_controls[i].mode_data[2] = (int)g_world_magic_choice_button_sprite_bank.named.highlight[sprite_index];
    g_world_magic_choice_controls[i].mode_data[3] = (int)g_world_magic_choice_button_sprite_bank.named.pressed[sprite_index];
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
    s.mapped_world_magic_index = FUN_0056302b(s.wizard_color_index + 1);
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
      for (s.scan_index = 0; (s.scan_index < 1000) && ((char)g_duel_victory_log[s.scan_index] != '\0'); s.scan_index = s.scan_index + 1)
      {
        if (((int)(char)g_duel_victory_log[s.scan_index] >> 4) == s.wizard_color_index + 1)
        {
          s.world_magic_duel_win_count = s.world_magic_duel_win_count + 1;
        }
      }
      s.world_magic_victory_counts[s.mapped_world_magic_index] = s.world_magic_duel_win_count;
      s.required_duel_wins = MAX(g_shandalar_difficulty * 5 + 20, s.required_duel_wins - s.original_required_duel_wins);
      s.world_magic_progress_values[s.mapped_world_magic_index] = 0x1e - (s.original_required_duel_wins - s.required_duel_wins);
    }
  }
  s.unused_flag_a = 0;
  s.unused_flag_b = 0;
  s.unused_flag_c = 0;
  FUN_0052280c(s.world_magic_progress_values);
}

// FUNCTION: SHANDALAR 0x0056bff1
void RebuildDeckEntriesByCardGroup(void)
{
  struct
  {
    int deck_slot_index;
    int card_index;
    unsigned int saved_deck_entries[500];
    int saved_journal_entry_count;
  } s;

  for (s.card_index = 0; s.card_index < 500; s.card_index = s.card_index + 1)
  {
    s.saved_deck_entries[s.card_index] = deck[s.card_index];
    deck[s.card_index] = -1;
  }
  s.saved_journal_entry_count = g_journal_entry_count;
  for (s.card_index = 0; s.card_index < 500; s.card_index = s.card_index + 1)
  {
    if (s.saved_deck_entries[s.card_index] != 0xffffffff)
    {
      s.deck_slot_index = FUN_0056bd9d(s.saved_deck_entries[s.card_index] & 0xfff);
      deck[s.deck_slot_index] |= ((int)s.saved_deck_entries[s.card_index] & -4096);
    }
  }
  g_journal_entry_count = s.saved_journal_entry_count;
}

// FUNCTION: SHANDALAR 0x0054cdbd
void RefreshAdventureInterfaceLayout(void)
{
  DAT_00590764 = DAT_006527b0 = -1;
  g_adventure_ui_layout_dirty = 1;
  if (g_duel_ai_mode_state == 0)
  {
    LoadPcxIntoPage(1, PTR_s_advinter800_pic_00589de8);
    BlitGraphicsRect(PTR_DAT_005832dc, 0, 0, global_screen_width, global_screen_height, PTR_DAT_005832b4, 0, 0);
    RenderMenuControlRange(0, 4);
  }

  g_world_scroll_cache_ready = 0;
  if (DAT_00650f28 == 0)
  {
    BlitGraphicsRect(PTR_DAT_005832dc, 0, ScaleUiCoordinate(0x148), ScaleUiCoordinate(0x280), ScaleUiCoordinate(0x1e0) - ScaleUiCoordinate(0x148), PTR_DAT_0058332c, 0, 0);
    BlitGraphicsRect(PTR_DAT_005832dc, 0, 0, ScaleUiCoordinate(0x40), ScaleUiCoordinate(0x148), PTR_DAT_00583354, 0, 0);
    DAT_00650f28 = 1;
  }
}

// FUNCTION: SHANDALAR 0x0055e1b2
int RunStartupMenuAndQueueInput(void)
{
  int menu_x;
  int menu_y;
  int previous_font_slot;

  previous_font_slot = PTR_DAT_005832b4->font_slot;
  LoadTextSectionLines("ADVstrings.txt", "STARTUP");
  PTR_DAT_005832b4->font_slot = 4;
  menu_x = ScaleUiCoordinate(0x40);
  menu_y = ScaleUiCoordinate(0x50);

  switch (RunTextMenuAt(text_lines[1], menu_y, menu_x))
  {
  case 0:
    PushQueuedKeyInput(0x53);
    break;
  case 1:
    PushQueuedKeyInput(0x4c);
    break;
  case 2:
    PushQueuedKeyInput(0x51);
    break;
  case 3:
    PushQueuedKeyInput(0x3b00);
    break;
  case 4:
    PushQueuedKeyInput(0x3c00);
    break;
  case 5:
    PushQueuedKeyInput(0x3d00);
    break;
  case 6:
    PushQueuedKeyInput(0x3e00);
    break;
  case 7:
    PushQueuedKeyInput(0x3f00);
    break;
  case 8:
    PushQueuedKeyInput(0x4000);
    break;
  default:
    RefreshAdventureInterfaceLayout();
    break;
  }

  PTR_DAT_005832b4->font_slot = previous_font_slot;
}

// FUNCTION: SHANDALAR 0x0055e651
int QueuePendingMenuActionInput(void)
{
  int unused_mapped_key;

  unused_mapped_key = -1;
  switch (g_pending_ui_action_code)
  {
  case 1:
    PushQueuedKeyInput(0x3b00);
    break;
  case 2:
    PushQueuedKeyInput(0x3c00);
    break;
  case 3:
    PushQueuedKeyInput(0x3d00);
    break;
  case 4:
    PushQueuedKeyInput(0x3e00);
    break;
  case 5:
    PushQueuedKeyInput(0x3f00);
    break;
  case 6:
    PushQueuedKeyInput(0x4000);
    break;
  case 0x31:
    PushQueuedKeyInput(0x31);
    break;
  case 0x32:
    PushQueuedKeyInput(0x32);
    break;
  case 0x33:
    PushQueuedKeyInput(0x33);
    break;
  case 0x34:
    PushQueuedKeyInput(0x34);
    break;
  case 0x35:
    PushQueuedKeyInput(0x35);
    break;
  default:
    UpdateMouseSnapshot();
    if (g_mouse_button_mask_snapshot != 0)
    {
      unused_mapped_key = FUN_0055e31f(g_mouse_x_snapshot, g_mouse_y_snapshot);
    }
    if (unused_mapped_key != -1)
    {
      PushQueuedKeyInput(unused_mapped_key);
    }
    break;
  }

  g_pending_ui_action_code = 0;
  return 0;
}

// FUNCTION: SHANDALAR 0x0055e31f
int FUN_0055e31f(int x, int y)
{
  struct
  {
    int local_18;
    int local_14;
    int local_10;
    int local_c;
    unsigned int direction;
  } s;
  int keycode_map[10];

  keycode_map[0] = 0x4800;
  keycode_map[1] = 0x4900;
  keycode_map[2] = 0x4d00;
  keycode_map[3] = 0x5100;
  keycode_map[4] = 0x5000;
  keycode_map[5] = 0x4f00;
  keycode_map[6] = 0x4b00;
  keycode_map[7] = 0x4700;
  keycode_map[8] = 0x4800;

  if ((x < ScaleUiCoordinate(0x40) || ScaleUiCoordinate(0x240) < x) || (y < ScaleUiCoordinate(0x30) || ScaleUiCoordinate(0x148) < y))
  {
    return -1;
  }

  if ((ScaleUiCoordinate(0x130) < x && x < ScaleUiCoordinate(0x158)) &&
      (ScaleUiCoordinate(0xa8) < y && y < ScaleUiCoordinate(0xdd)))
  {
    return 0x20;
  }

  s.local_14 = ScaleUiCoordinate(0x140);
  s.local_18 = ScaleUiCoordinate(0xbc);
  x = x - s.local_14;
  y = s.local_18 - y;
  s.local_c = abs(x);
  s.local_10 = abs(y);

  if (x >= 0 && y >= 0)
  {
    s.direction = 0;
  }
  else if (x >= 0 && y < 0)
  {
    s.direction = 2;
  }
  else if (x < 0 && y < 0)
  {
    s.direction = 4;
  }
  else if (x < 0 && y >= 0)
  {
    s.direction = 6;
  }

  if ((s.direction & 2) == 0 && s.local_10 < s.local_c)
  {
    s.direction++;
  }
  else if ((s.direction & 2) != 0 && s.local_c < s.local_10)
  {
    s.direction++;
  }

  switch (s.direction)
  {
  case 0:
  case 3:
  case 4:
  case 7:
    keycode_map[9] = (s.local_c * 0x9a85) >> 0xe;
    if (y < 0)
    {
      keycode_map[9] = -keycode_map[9];
    }
    break;
  case 1:
  case 2:
  case 5:
  case 6:
    keycode_map[9] = (s.local_c * 0x1a82) >> 0xe;
    if (y < 0)
    {
      keycode_map[9] = -keycode_map[9];
    }
    break;
  }

  switch (s.direction)
  {
  case 0:
  case 1:
  case 2:
  case 3:
    if (y < keycode_map[9])
    {
      s.direction = s.direction + 1;
    }
    break;
  case 4:
  case 5:
  case 6:
  case 7:
    if (keycode_map[9] < y)
    {
      s.direction = s.direction + 1;
    }
    break;
  }

  return keycode_map[s.direction];
}

// FUNCTION: SHANDALAR 0x0055e808
void UpdateAdventureWorldInputAndMovement(void)
{
  struct
  {
    int move_offset_y;             // ebp - 0x70
    int move_offset_x;             // ebp - 0x6c
    int nearest_town_distance;     // ebp - 0x68
    int nearest_town_index;        // ebp - 0x64
    int town_index;                // ebp - 0x60
    int castle_index;              // ebp - 0x5c
    int ambient_track_id;          // ebp - 0x58
    int previous_world_x_adjusted; // ebp - 0x54
    int previous_world_y_adjusted; // ebp - 0x50
    int delta_y;                   // ebp - 0x4c
    int delta_x;                   // ebp - 0x48
    int move_step_divisor;         // ebp - 0x44
    unsigned int tile_magic_mask;  // ebp - 0x40
    int abs_delta_y;               // ebp - 0x3c
    int previous_world_y;          // ebp - 0x38
    int previous_world_x;          // ebp - 0x34
    int key_code;                  // ebp - 0x30
    int key_magic_index;           // ebp - 0x2c
    int slot_index;                // ebp - 0x28
    int abs_delta_x;               // ebp - 0x24
    int random_world_y;            // ebp - 0x20
    int random_world_x;            // ebp - 0x1c
    int random_value;              // ebp - 0x18
    unsigned int tile_type;        // ebp - 0x14
    int nearest_slot_distance;     // ebp - 0x10
    int nearest_slot_index;        // ebp - 0xc
    int deck_index;                // ebp - 0x8
    int dungeon_index;             // ebp - 0x4
  } s;

  if (IsKeyInputQueueEmpty() == 0)
  {
    s.key_code = PopNormalizedQueuedKeyInput();
    DAT_00669700 = 0;

    switch (s.key_code)
    {
    case 0x1b:
    case 0x51:
    case 0x71:
      PTR_DAT_005832b4->font_slot = 4;
      LoadTextSectionLines("ADVstrings.txt", "SHUTDOWN");
      strcpy(g_ui_message_buffer, text_lines[0]);
      if (RunTextMenuAtScaled(g_ui_message_buffer, 100, 0x50) == 1)
      {
        g_adventure_world_exit_requested = 1;
      }
      else
      {
        RefreshAdventureInterfaceLayout();
      }
      SaveGameToSlot(3);
      break;

    case 0x4800:
      g_world_move_dir_index = 2;
      break;
    case 0x4900:
      g_world_move_dir_index = 3;
      break;
    case 0x4d00:
      g_world_move_dir_index = 4;
      break;
    case 0x5100:
      g_world_move_dir_index = 5;
      break;
    case 0x5000:
      g_world_move_dir_index = 6;
      break;
    case 0x4f00:
      g_world_move_dir_index = 7;
      break;
    case 0x4b00:
      g_world_move_dir_index = 8;
      break;
    case 0x4700:
      g_world_move_dir_index = 1;
      break;
    case 0x20:
      g_world_move_dir_index = 0;
      break;

    case 0x3b00:
      if ((g_world_magic_bitmap & 8U) != 0)
      {
        s.key_code = 0x31;
        goto handle_world_magic_hotkey;
      }

      AnimatePaletteToColor(0, g_default_palette_fade_steps);
      DeckBuilderMain(g_main_window_hwnd, 1, 0);
      RestoreAdventureUiPaletteAndFocus();
      RefreshAdventureInterfaceLayout();

      break;
    case 0x3c00:
      ClearInputAndWaitForMouseRelease();
      ShowWorldMapScreen(0);
      RefreshAdventureInterfaceLayout();
      break;
    case 0x3d00:
      ClearInputAndWaitForMouseRelease();
      ShowCityInfoScreen(1);
      RefreshAdventureInterfaceLayout();
      break;
    case 0x3e00:
      ClearInputAndWaitForMouseRelease();
      ShowDungeonCluesScreen(1);
      RefreshAdventureInterfaceLayout();
      break;
    case 0x3f00:
      RunAdventureStatsMenu();
      RefreshAdventureInterfaceLayout();
      break;
    case 0x4000:
      ClearInputAndWaitForMouseRelease();
      ShowStatsWindow(0, -1);
      RefreshAdventureInterfaceLayout();
      break;
    case 0x4c:
    case 0x6c:
      g_loadsave_skip_esc = 1;
      s.previous_world_y_adjusted = FUN_005031a8();
      if (s.previous_world_y_adjusted != -1)
      {
        LoadGameFromSlot(s.previous_world_y_adjusted);
      }
      LoadPcxIntoPageNoPalette("advfac64.pic");
      g_world_move_dir_index = 0;
      RefreshAdventureInterfaceLayout();
      DrawAdventureInterfaceLayout(1);
      g_loadsave_skip_esc = 0;
      break;
    case 0x53:
    case 0x73:
      g_world_move_dir_index = 0;
      s.previous_world_x_adjusted = RunSaveMenuAndSelectSlot();
      if (s.previous_world_x_adjusted != -1)
      {
        SaveGameToSlot(s.previous_world_x_adjusted);
      }
      LoadPcxIntoPageNoPalette("advfac64.pic");
      RefreshAdventureInterfaceLayout();
      break;
    case 0x55:
      g_frontbuffer_direct_blit_enabled = g_frontbuffer_direct_blit_enabled ^ 1;
    case 0x31:
    case 0x32:
    case 0x33:
    case 0x34:
    case 0x35:

    handle_world_magic_hotkey:
      s.key_magic_index = s.key_code - 0x30;
      if ((g_amulet_inventory[s.key_magic_index - 1] != 0) && ((g_world_magic_bitmap & (1 << (s.key_magic_index * 2))) != 0))
      {
        if (RandomIntLessThan(4 - g_shandalar_difficulty) == 0)
        {
          g_amulet_inventory[s.key_magic_index - 1] = g_amulet_inventory[s.key_magic_index - 1] - 1;
        }
        RefreshAdventureInterfaceLayout();

        switch (s.key_magic_index)
        {
        case 1:
          AnimatePaletteToColor(0, g_default_palette_fade_steps);
          DeckBuilderMain(g_main_window_hwnd, 1, 1);
          RestoreAdventureUiPaletteAndFocus();
          ClearGraphicsPageWithPaletteColor(0, 7);
          RefreshAdventureInterfaceLayout();
          break;
        case 2:
          do
          {
            s.random_world_y = RandomIntLessThan(0x40);
            s.abs_delta_x = RandomIntLessThan(0x40);
          } while (GetWorldTileType(s.random_world_y, s.abs_delta_x) == 0);
          AddJournalEntry(JOURNAL_ENTRY_WORLD_MAGIC_EVENT, JOURNAL_WORLD_MAGIC_EVENT_TELEPORT_RANDOM);
          g_world_player_x = s.random_world_y * 0x20 + 0x10;
          g_world_player_y = s.abs_delta_x * 0x20 + 0x10;
          RefreshAdventureInterfaceLayout();
          g_world_scene_reveal_effect_pending = 1;
          break;
        case 3:
          g_world_magic_slot_timers[s.key_magic_index * 2].timer = 0x96;
          AddJournalEntry(JOURNAL_ENTRY_WORLD_MAGIC_EVENT, JOURNAL_WORLD_MAGIC_EVENT_DELAY_MONSTERS);
          break;
        case 4:
          s.tile_magic_mask = 0x7fff;
          s.nearest_slot_index = -1;
          for (s.slot_index = 0; s.slot_index < 6; s.slot_index = s.slot_index + 1)
          {
            if (SHANDALAR_ENTRY_LAIR < g_lair_or_monster_slots[s.slot_index].entry_type)
            {
              s.nearest_slot_distance = ApproximateDistance(g_world_player_x - g_lair_or_monster_slots[s.slot_index].world_x,
                                                     g_world_player_y - g_lair_or_monster_slots[s.slot_index].world_y);
              if ((int)s.nearest_slot_distance < (int)s.tile_magic_mask)
              {
                s.tile_magic_mask = s.nearest_slot_distance;
                s.nearest_slot_index = s.slot_index;
              }
            }
          }
          if (s.nearest_slot_index != -1)
          {
            FreeOpeningMenuSpriteWorkEntries(s.nearest_slot_index, s.nearest_slot_index + 8);
            AddJournalEntry(JOURNAL_ENTRY_WORLD_MAGIC_EVENT,
                            (g_lair_or_monster_slots[s.nearest_slot_index].entry_type << 0x10) |
                                JOURNAL_WORLD_MAGIC_EVENT_BANISH_MONSTER);
            g_lair_or_monster_slots[s.nearest_slot_index].entry_type = SHANDALAR_ENTRY_NONE;
          }
          break;
        case 5:
          if (g_lair_or_monster_slots[7].entry_type != SHANDALAR_ENTRY_NONE)
          {
            g_world_player_x = (g_lair_or_monster_slots[7].world_x & 0xffe0U) + 0x10;
            g_world_player_y = (g_lair_or_monster_slots[7].world_y & 0xffe0U) + 0x1f;
            AddJournalEntry(JOURNAL_ENTRY_WORLD_MAGIC_EVENT, JOURNAL_WORLD_MAGIC_EVENT_TELEPORT_TO_CASTLE);
          }
          g_world_scene_reveal_effect_pending = 1;
          break;
        }
      }

    default:
      break;
    }

    EnsureAdvfac64Loaded(0);
  }

  if ((DAT_006696fc != 0) && (++DAT_00669700 > 500))
  {
    // Some sort of demo? Looks unreachable since DAT_006696fc is never set
    SetupRandomAiDuelDecks();
    DAT_00669700 = 300;
  }

  s.random_value = g_neighbor_dx[g_world_move_dir_index] + g_world_player_x;
  s.random_world_x = g_neighbor_dy[g_world_move_dir_index] + g_world_player_y;
  s.tile_type = GetWorldTileType(s.random_value / 32, s.random_world_x / 32);
  s.key_magic_index = GetWorldTileMagicMask(s.tile_type);
  if (s.key_magic_index != 0)
  {
    do
    {
      s.abs_delta_y = RandomIntLessThan(5) + 1;
    } while ((s.key_magic_index & (1 << (unsigned char)s.abs_delta_y)) == 0);
  }
  else
  {
    s.abs_delta_y = 0;
  }

  s.deck_index = 1;
  g_world_player_tile_x = g_world_player_x / 32;
  g_world_player_tile_y = g_world_player_y / 32;
  if ((s.tile_type == 2) || ((s.tile_type == 3 && ((g_world_magic_bitmap & 8U) == 0)) || (s.tile_type == 4 && ((g_world_magic_bitmap & 0x200U) == 0))))
  {
    s.deck_index = 3;
  }
  if ((s.tile_type == 5) && ((g_world_magic_bitmap & 0x200U) == 0))
  {
    s.deck_index = 3;
  }
  if (WorldRoadTileHasDirection(g_world_player_tile_x, g_world_player_tile_y, g_world_move_dir_index) != 0)
  {
    s.deck_index = 1;
  }
  if (WorldRoadTileHasDirection(g_world_player_tile_x, g_world_player_tile_y, (g_world_move_dir_index + 3U & 7) + 1) != 0)
  {
    s.deck_index = 1;
  }
  if (g_food == 0)
  {
    s.deck_index = ClampIntToRange(s.deck_index + 2, 0, 4);
  }

  s.random_value = g_world_player_x / 32;
  s.random_world_x = g_world_player_y / 32;
  s.previous_world_x = g_world_player_x;
  s.previous_world_y = g_world_player_y;
  if (g_monster_timer % s.deck_index == 0)
  {
    if (s.deck_index == 3)
    {
      g_world_player_x += g_neighbor_dx[g_world_move_dir_index] * 2;
    }
    else
    {
      g_world_player_x += g_neighbor_dx[g_world_move_dir_index];
    }

    if (s.deck_index == 3)
    {
      g_world_player_y += g_neighbor_dy[g_world_move_dir_index] * 2;
    }
    else
    {
      g_world_player_y += g_neighbor_dy[g_world_move_dir_index];
    }

    g_world_player_animation_frame = g_world_player_animation_frame + 1;
    if (4 < g_world_player_animation_frame)
    {
      g_world_player_animation_frame = 1;
    }

    if (g_world_move_dir_index != 0)
    {
      FUN_00562736(((g_world_player_animation_frame & 1U) - 2) + s.abs_delta_y * 2, RandomIntLessThan(0x19) + 0x4b, RandomIntLessThan(0x28) + 0x50, 0);
    }

    if (g_world_move_dir_index == 0)
    {
      g_world_player_animation_frame = 0;
    }
    else
    {
      g_world_player_animation_direction = g_world_move_dir_index;
    }

    if ((g_world_magic_slot_timers[WORLDMAGIC_QUICKENING].timer != 0) ||
        (((g_monster_timer & 1U) != 0 &&
          (WorldRoadTileHasDirection(g_world_player_tile_x, g_world_player_tile_y, (g_world_move_dir_index + 3U & 7) + 1) != 0))))
    {
      g_world_player_x = g_world_player_x + g_neighbor_dx[g_world_move_dir_index];
      g_world_player_y = g_world_player_y + g_neighbor_dy[g_world_move_dir_index];
    }

    s.tile_type = GetWorldTileType(g_world_player_x / 32, g_world_player_y / 32);
    if ((s.tile_type == 0) &&
        ((abs(g_world_player_x - ((g_world_player_x & 0xffffffe0U) + 0x10)) < 0xc) || (abs(g_world_player_y - ((g_world_player_y & 0xffffffe0U) + 0x10)) < 0xc)))
    {
      g_world_move_dir_index = 0;
      g_world_player_x = s.previous_world_x;
      g_world_player_y = s.previous_world_y;
    }

    if ((RandomIntLessThan(0x28) == 0) && (g_skip_world_sfx_preload == 0))
    {
      UpdateAmbientWizardColorSound(s.abs_delta_y);
    }

    if (((g_monster_timer & 0x1fU) == 0) && (g_world_move_dir_index != 0))
    {
      if (g_food != 0)
      {
        g_food = g_food - 1;
      }
      if ((g_world_magic_slot_timers[WORLDMAGIC_FRUIT_OF_SUSTENANCE].town_index == 0) && (s.tile_type == 2))
      {
        g_food = g_food + 2;
      }
      g_siege_timer = g_siege_timer + 1;
      g_quest_restock_timer = g_quest_restock_timer + 1;

      if ((g_siege_timer & 0x3fU) == 0)
      {
        StartWizardTownSiege();
        g_siege_timer = g_siege_timer + ClampIntToRange(g_shandalar_difficulty + g_siege_timer / 0x100, 0, 0x10);
      }
      if ((g_siege_timer & 0x3f) == 0x18)
      {
        ResolveWizardTownSiege();
        g_siege_timer = g_siege_timer + ClampIntToRange(g_shandalar_difficulty + g_siege_timer / 0x40 + g_shandalar_difficulty, 0, 0x20);
      }

      DAT_00669710 = 1;
      if (g_siege_timer >= 8)
      {
        DAT_00591214 = 1;
      }
    }

    g_world_player_tile_x = g_world_player_x / 32;
    g_world_player_tile_y = g_world_player_y / 32;
    if ((s.random_value != g_world_player_tile_x) || (s.random_world_x != g_world_player_tile_y))
    {
      DAT_006696f4 = 0;
    }

    if ((g_monster_timer & 1U) == 0)
    {
      s.nearest_town_distance = 0x7fff;
      for (s.nearest_town_index = 0; s.nearest_town_index < 0x80; s.nearest_town_index = s.nearest_town_index + 1)
      {
        if (g_town_slots[s.nearest_town_index].location_type != -1)
        {
          s.castle_index = ApproximateDistance(g_town_slots[s.nearest_town_index].world_x * 0x20 + 0x10 - g_world_player_x,
                                        g_town_slots[s.nearest_town_index].world_y * 0x20 + 0x10 - g_world_player_y);
          if (s.castle_index < s.nearest_town_distance)
          {
            s.nearest_town_distance = s.castle_index;
            s.ambient_track_id = s.nearest_town_index;
          }
        }
      }

      s.town_index = ClampIntToRange(0x80 - s.nearest_town_distance, 0, 100);
      if ((s.town_index > 0xa) && (g_town_slots[s.ambient_track_id].location_type >= 1))
      {
        if (s.ambient_track_id != g_world_location_music_town_index)
        {
          g_world_location_music_town_index = s.ambient_track_id;
          if (g_town_slots[s.ambient_track_id].location_type == 4)
          {
            for (s.key_magic_index = 0; s.key_magic_index < 5; s.key_magic_index = s.key_magic_index + 1)
            {
              if (g_town_slots[s.ambient_track_id].world_x == g_castle_dungeon_slots[s.key_magic_index].world_x)
              {
                if (g_castle_dungeon_slots[s.key_magic_index].world_y == g_town_slots[s.ambient_track_id].world_y)
                {
                  break;
                }
              }
            }

            if (s.key_magic_index + 0x15 != g_world_location_music_track_id)
            {
              if ((g_world_location_music_track_id != -1) && (s.key_magic_index + 0x15 != g_world_location_music_track_id))
              {
                sound_unload(0x10);
              }

              switch (s.key_magic_index + 1)
              {
              case 1:
                FUN_00562835("x:sound\\bcastle.wav", 0x10);
                break;
              case 2:
                FUN_00562835("x:sound\\ucastle.wav", 0x10);
                break;
              case 3:
                FUN_00562835("x:sound\\gcastle.wav", 0x10);
                break;
              case 4:
                FUN_00562835("x:sound\\rcastle.wav", 0x10);
                break;
              case 5:
                FUN_00562835("x:sound\\wcastle.wav", 0x10);
                break;
              }
              s.key_magic_index += +0x15;
              g_world_location_music_track_id = s.key_magic_index;
            }
          }
          else if (g_town_slots[s.ambient_track_id].location_type == 1)
          {
            if ((g_world_location_music_track_id != -1) && (g_world_location_music_track_id != 0x32))
            {
              sound_unload(0x10);
            }
            if (g_world_location_music_track_id != 0x32)
            {
              FUN_00562835("x:sound\\locmus0.wav", 0x10);
            }
            g_world_location_music_track_id = s.key_magic_index = 0x32;
          }
          else
          {
            s.key_magic_index = s.ambient_track_id % 0x14;
            if (s.key_magic_index != g_world_location_music_track_id)
            {
              if (g_world_location_music_track_id != -1)
              {
                sound_unload(0x10);
              }
              switch (s.key_magic_index)
              {
              case 0:
                FUN_00562835("x:sound\\locmus1.wav", 0x10);
                break;
              case 1:
                FUN_00562835("x:sound\\locmus2.wav", 0x10);
                break;
              case 2:
                FUN_00562835("x:sound\\locmus3.wav", 0x10);
                break;
              case 3:
                FUN_00562835("x:sound\\locmus4.wav", 0x10);
                break;
              case 4:
                FUN_00562835("x:sound\\locmus5.wav", 0x10);
                break;
              case 5:
                FUN_00562835("x:sound\\locmus6.wav", 0x10);
                break;
              case 6:
                FUN_00562835("x:sound\\locmus7.wav", 0x10);
                break;
              case 7:
                FUN_00562835("x:sound\\locmus8.wav", 0x10);
                break;
              case 8:
                FUN_00562835("x:sound\\locmus9.wav", 0x10);
                break;
              case 9:
                FUN_00562835("x:sound\\locmus10.wav", 0x10);
                break;
              case 10:
                FUN_00562835("x:sound\\locmus11.wav", 0x10);
                break;
              case 0xb:
                FUN_00562835("x:sound\\locmus12.wav", 0x10);
                break;
              case 0xc:
                FUN_00562835("x:sound\\locmus13.wav", 0x10);
                break;
              case 0xd:
                FUN_00562835("x:sound\\locmus14.wav", 0x10);
                break;
              case 0xe:
                FUN_00562835("x:sound\\locmus15.wav", 0x10);
                break;
              case 0xf:
                FUN_00562835("x:sound\\locmus16.wav", 0x10);
                break;
              case 0x10:
                FUN_00562835("x:sound\\locmus17.wav", 0x10);
                break;
              case 0x11:
                FUN_00562835("x:sound\\locmus18.wav", 0x10);
                break;
              case 0x12:
                FUN_00562835("x:sound\\locmus19.wav", 0x10);
                break;
              case 0x13:
                FUN_00562835("x:sound\\tmplmus1.wav", 0x10);
                break;
              default:
                FUN_00562835("x:sound\\locmus0.wav", 0x10);
                break;
              }
            }
            g_world_location_music_track_id = s.key_magic_index;
          }
          FUN_0056279e(0x10, s.town_index, 0);
          set_sound_loop(0x10, 1);
        }
        else
        {
          sound_set_vol(0x10, s.town_index << 2);
        }
        g_world_location_music_active = 1;
      }
      else
      {
        if (g_world_location_music_active != 0)
        {
          sound_stop(0x10);
        }
        g_world_location_music_active = 0;
        g_world_location_music_town_index = -1;
      }
    }

    if ((DAT_006696f4 == 0) && (abs((g_world_player_x & 0x1fU) - 0x10) < 0xc) &&
        (abs((g_world_player_y & 0x1fU) - 0x10) < 0xc) && ((FUN_004314ca(g_world_player_tile_x, g_world_player_tile_y) & 0x10) != 0))
    {
      s.move_step_divisor = FUN_004bb040(g_world_player_tile_x, g_world_player_tile_y);
      if (s.move_step_divisor != -1)
      {
        sound_set_vol(0x10, 400);
        play_snd_marker(0x10, 1);
        VisitTownSlot(s.move_step_divisor);
        DAT_00591214 = 1;
        DAT_006696f4 = DAT_00591214;
        g_world_move_dir_index = 0;
        for (s.slot_index = 0; s.slot_index < 6; s.slot_index = s.slot_index + 1)
        {
          if (SHANDALAR_ENTRY_LAIR < g_lair_or_monster_slots[s.slot_index].entry_type)
          {
            if (ApproximateDistance(g_world_player_x - g_lair_or_monster_slots[s.slot_index].world_x,
                             g_world_player_y - g_lair_or_monster_slots[s.slot_index].world_y) < 0x60)
            {
              s.delta_x = g_world_player_x - g_lair_or_monster_slots[s.slot_index].world_x;
              s.delta_y = g_world_player_y - g_lair_or_monster_slots[s.slot_index].world_y;
              if (abs(s.delta_y) / 2 < abs(s.delta_x))
              {
                s.move_offset_x = SignNonZero(s.delta_x) * 0x30;
              }
              else
              {
                s.move_offset_x = 0;
              }
              g_lair_or_monster_slots[s.slot_index].world_x = g_world_player_x - s.move_offset_x;

              if (abs(s.delta_x) / 2 < abs(s.delta_y))
              {
                s.move_offset_y = SignNonZero(s.delta_y) * 0x30;
              }
              else
              {
                s.move_offset_y = 0;
              }
              g_lair_or_monster_slots[s.slot_index].world_y = g_world_player_y - s.move_offset_y;
            }
          }
        }
        AnalyzeDeckAndMaybeShowReport(0);
        SaveGameToSlot(3);
        EnsureAdvfac64Loaded(0);
        RefreshAdventureInterfaceLayout();
        g_monster_timer = g_monster_timer | 0x1f;
      }
      else
      {
        ClearWorldMapPixelFlags(0x10, g_world_player_tile_x, g_world_player_tile_y);
      }
    }

    if ((DAT_006696f4 == 0) && ((g_world_player_x - 8U & 0x10) == 0) && ((g_world_player_y - 8U & 0x10) == 0) &&
        ((FUN_004314ca(g_world_player_tile_x, g_world_player_tile_y) & 0x40) != 0))
    {
      s.dungeon_index = FUN_00508b89(g_world_player_tile_x, g_world_player_tile_y);
      DAT_006696f4 = 1;
      if ((s.dungeon_index != -1) && (g_castle_dungeon_slots[s.dungeon_index].card_slot_1 != -1) && (g_castle_dungeon_slots[s.dungeon_index].clues_bitmap != 0))
      {
        EnterCastleDungeon(s.dungeon_index);
      }
    }

    DAT_00789938 = 0;
    DAT_0078df68 = DAT_00789938;
    for (s.slot_index = 0; s.slot_index < 500; s.slot_index = s.slot_index + 1)
    {
      if (deck[s.slot_index] != -1)
      {
        DAT_00789938 = DAT_00789938 + 1;
        if ((deck[s.slot_index] & 0x4000) == 0)
        {
          DAT_0078df68 = DAT_0078df68 + 1;
        }
      }
    }

    ClearQueuedKeyInput();
  }
}

// FUNCTION: SHANDALAR 0x0041d1ef
int IsKeyInputQueueEmpty(void)
{
  if (g_key_input_queue_count == 0)
  {
    return -1;
  }
  else
  {
    return 0;
  }
}

// FUNCTION: SHANDALAR 0x00431859
unsigned int WorldRoadTileHasDirection(int tile_x, int tile_y, int direction_index)
{
  if ((tile_x >= 0x40) || (tile_x < 0))
  {
    return 0;
  }

  if ((tile_y >= 0x40) || (tile_y < 0))
  {
    return 0;
  }

  return GetGraphicsPixelColorRef(PTR_DAT_00583304, tile_x, tile_y + 0x40) & (1U << ((unsigned char)(direction_index - 1U)));
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

// FUNCTION: SHANDALAR 0x004ecefd
int SignNonZero(int value)
{
  if (0 < value)
  {
    return 1;
  }
  if (value < 0)
  {
    return -1;
  }
}

// FUNCTION: SHANDALAR 0x004ed0bd
int RecountDeckCardTotals(void)
{
  int deck_index;

  DAT_0078df68 = DAT_00789938 = 0;
  for (deck_index = 0; deck_index < 500; deck_index = deck_index + 1)
  {
    if (deck[deck_index] != -1)
    {
      DAT_00789938 = DAT_00789938 + 1;
      if ((deck[deck_index] & 0x4000) == 0)
      {
        DAT_0078df68++;
      }
    }
  }
  return 0;
}

// FUNCTION: SHANDALAR 0x004ed135
int RestoreAdventureUiPaletteAndFocus(void)
{
  SetFocus(g_main_window_hwnd);
  LoadPcxIntoPageNoPalette(s_advfac64_pic_path_0058a86c);
  ReadPalette(s_todpal_tr_path_0058a87c, (char *)0);
  SelectPalette(g_graphics_pages[0]->hTempDC, g_palette_handle, FALSE);
  RealizePalette(g_graphics_pages[0]->hTempDC);
  RecountDeckCardTotals();
  ClearInputAndWaitForMouseRelease();
  return 0;
}

// FUNCTION: SHANDALAR 0x0050318e
int RunSaveMenuAndSelectSlot(void)
{
  return RunLoadSaveMenu(1);
}

// FUNCTION: SHANDALAR 0x00559807
void SetupRandomAiDuelDecks(void)
{
  // TODO(decomp): Periodic "demo/attract" style action: chooses some prebuilt decks, resets ante state, then kicks off
  // a larger sequence (calls FUN_00568320) and refreshes the screen.
}

// FUNCTION: SHANDALAR 0x005616cb
void EnsureAdvfac64Loaded(int state)
{
  if (state != g_advfac64_load_state)
  {
    LoadPcxIntoPageNoPalette(s_advfac64_pic_path_00591684);
    g_advfac64_load_state = state;
  }
}

// FUNCTION: SHANDALAR 0x00562736
void FUN_00562736(int param_1, int param_2, int param_3, int param_4)
{
  Sound snd;

  memset(&snd, 0, 0x20);
  snd.volume = param_2 << 2;
  snd.sampleRate = (param_3 * 0x5622) / 100;
  snd.pan = param_4 << 2;
  snd.flags &= 0xfffffffe;
  snd.flags &= 0xffffffef;
  sound_play(param_1, &snd);
}

// FUNCTION: SHANDALAR 0x005627ee
void FUN_005627ee(int sound_id, int volume, int pan)
{
  Sound snd;

  memset(&snd, 0, 0x20);
  snd.volume = volume << 2;
  snd.sampleRate = 0x5622;
  snd.pan = pan << 2;
  sound_play(sound_id, &snd);
}

// FUNCTION: SHANDALAR 0x0056279e
void FUN_0056279e(int param_1, int param_2, int param_3)
{
  Sound snd;

  memset(&snd, 0, 0x20);
  snd.volume = param_2 << 2;
  snd.sampleRate = 0x5622;
  snd.pan = param_3 << 2;
  snd.flags = (int)snd.flags | 1;
  sound_play(param_1, &snd);
}

// FUNCTION: SHANDALAR 0x00562835
void FUN_00562835(char *filename, int param_2)
{
  Sound snd;

  memset(&snd, 0, 0x20);
  snd.flags = (int)snd.flags | 4;
  snd.volume = 400;
  snd.sampleRate = 0;
  snd.pan = 0;
  LoadSoundWithDriveFallback(filename, param_2, &snd);
  set_sound_loop(param_2, 1);
}

// FUNCTION: SHANDALAR 0x00562a29
void UpdateAmbientWizardColorSound(int param_1)
{
  int channel = param_1 + 9;

  FUN_00562736(channel, 100, RandomIntLessThan(0x32) + 0x46, RandomIntLessThan(200) - 100);

  if (RandomIntLessThan(3) == 0)
  {
    sound_unload(channel);
    switch (param_1)
    {
    case 1:
      switch (RandomIntLessThan(3))
      {
      case 0:
        LoadSoundWithDriveFallback("x:sound\\kbird1.wav", channel, 0);
        break;
      case 1:
        LoadSoundWithDriveFallback("x:sound\\kland1.wav", channel, 0);
        break;
      case 2:
        LoadSoundWithDriveFallback("x:sound\\kland2.wav", channel, 0);
        break;
      }
      break;
    case 2:
      switch (RandomIntLessThan(3))
      {
      case 0:
        LoadSoundWithDriveFallback("x:sound\\bbird1.wav", channel, 0);
        break;
      case 1:
        LoadSoundWithDriveFallback("x:sound\\bland1.wav", channel, 0);
        break;
      case 2:
        LoadSoundWithDriveFallback("x:sound\\bland2.wav", channel, 0);
        break;
      }
      break;
    case 3:
      switch (RandomIntLessThan(2))
      {
      case 0:
        LoadSoundWithDriveFallback("x:sound\\gbird1.wav", channel, 0);
        break;
      case 1:
        LoadSoundWithDriveFallback("x:sound\\gland1.wav", channel, 0);
        break;
      }
      break;
    case 4:
      switch (RandomIntLessThan(2))
      {
      case 0:
        LoadSoundWithDriveFallback("x:sound\\rbird1.wav", channel, 0);
        break;
      case 1:
        LoadSoundWithDriveFallback("x:sound\\rland1.wav", channel, 0);
        break;
      }
      break;
    case 5:
      switch (RandomIntLessThan(2))
      {
      case 0:
        LoadSoundWithDriveFallback("x:sound\\wbird1.wav", channel, 0);
        break;
      case 1:
        LoadSoundWithDriveFallback("x:sound\\wland1.wav", channel, 0);
        break;
      }
      break;
    }
  }
}

// FUNCTION: SHANDALAR 0x00561647
void TickWorldMagicSlotTimers(void)
{
  int slot_index;

  for (slot_index = 0; slot_index < 0xc; slot_index = slot_index + 1)
  {
    if (0 < g_world_magic_slot_timers[slot_index].timer)
    {
      g_world_magic_slot_timers[slot_index].timer = g_world_magic_slot_timers[slot_index].timer - 1;
      if (g_world_magic_slot_timers[slot_index].timer == 0)
      {
        RefreshAdventureInterfaceLayout();
      }
    }
  }
}

// FUNCTION: SHANDALAR 0x0046eca4
void UpdateMouseSnapshot(void)
{
  if (DAT_00586494 != 0)
  {
    g_mouse_button_mask_snapshot = ConsumeMouseButtonReleaseMask() | g_mouse_button_down_mask;
    g_mouse_x_snapshot = g_mouse_x;
    g_mouse_y_snapshot = g_mouse_y;
  }
  else
  {
    g_mouse_button_mask_snapshot = g_mouse_x_snapshot = g_mouse_y_snapshot = 0;
  }
}

// FUNCTION: SHANDALAR 0x0041d21d
unsigned int PeekQueuedKeyInput(void)
{
  if (g_key_input_queue_count == 0)
  {
    return 0;
  }
  else
  {
    return g_key_input_queue[0];
  }
}

// FUNCTION: SHANDALAR 0x0041d24b
void PushQueuedKeyInput(int key_code)
{
  memmove(g_key_input_queue + 1, g_key_input_queue, (size_t)g_key_input_queue_count * sizeof(g_key_input_queue[0]));
  g_key_input_queue_count = g_key_input_queue_count + 1;
  g_key_input_queue[0] = (unsigned int)key_code;
}

// FUNCTION: SHANDALAR 0x005003b7
int UpdateMenuControlSelection(int mouse_x, int mouse_y, int allow_activate_on_click)
{
  struct
  {
    int direction;
    int control_index;
    int next_search_index;
    int queued_key;
    int i;
    int full_key;
  } s;

  (void)mouse_x;
  (void)mouse_y;

  g_menu_input_unhandled = 1;
  if (HasQueuedKeyInput() != 0)
  {
    s.queued_key = PeekQueuedKeyInput();
    if ((s.queued_key == 0xf09) || (s.queued_key == 0xf0f) ||
        ((g_menu_allow_arrow_nav_by_context[g_menu_context_index] != 0) && ((s.queued_key == 0x4800) || (s.queued_key == 0x5000))))
    {
      PopQueuedKeyInput();
      switch (s.queued_key)
      {
      case 0xf09:
        s.direction = 1;
        break;
      case 0xf0f:
        s.direction = -1;
        break;
      case 0x4800:
        s.direction = -1;
        break;
      case 0x5000:
        s.direction = 1;
        break;
      default:
        break;
      }

      if (g_menu_current_control_index == -1)
      {
        if (s.direction > 0)
        {
          g_menu_current_control_index = 0;
        }
        else
        {
          g_menu_current_control_index = g_menu_control_count_by_context[g_menu_context_index] - 1;
        }
      }
      else
      {
        g_menu_current_control_index = (g_menu_control_count_by_context[g_menu_context_index] + g_menu_current_control_index + s.direction) % g_menu_control_count_by_context[g_menu_context_index];
      }

      while (g_menu_controls_by_context[g_menu_context_index][g_menu_current_control_index]->state == 3)
      {
        g_menu_current_control_index = (g_menu_control_count_by_context[g_menu_context_index] + g_menu_current_control_index + s.direction) % g_menu_control_count_by_context[g_menu_context_index];
      }

      if (g_menu_prev_control_index != -1)
      {
        g_menu_controls_by_context[g_menu_context_index][g_menu_prev_control_index]->on_render(g_menu_controls_by_context[g_menu_context_index][g_menu_prev_control_index], 0);
      }

      if (g_menu_controls_by_context[g_menu_context_index][g_menu_current_control_index]->x != -1)
      {
        SetCursorPos(g_menu_controls_by_context[g_menu_context_index][g_menu_current_control_index]->x + g_menu_controls_by_context[g_menu_context_index][g_menu_current_control_index]->width / 2,
                     g_menu_controls_by_context[g_menu_context_index][g_menu_current_control_index]->y + g_menu_controls_by_context[g_menu_context_index][g_menu_current_control_index]->height / 2);
      }
      g_menu_controls_by_context[g_menu_context_index][g_menu_current_control_index]->on_render(g_menu_controls_by_context[g_menu_context_index][g_menu_current_control_index], 1);
    }

    s.full_key = (int)s.queued_key;
    s.queued_key &= 0xff;
    if ((s.queued_key == 0) && (g_menu_allow_arrow_nav_by_context[g_menu_context_index] != 0))
    {
    }
    else
    {
      s.i = 0;
      if (g_menu_current_control_index == -1)
      {
        s.next_search_index = 0;
      }
      else
      {
        s.next_search_index = g_menu_current_control_index + 1;
      }

      for (; s.i < g_menu_control_count_by_context[g_menu_context_index]; s.i = s.i + 1)
      {
        s.control_index = (s.next_search_index + s.i) % g_menu_control_count_by_context[g_menu_context_index];
        if (g_menu_controls_by_context[g_menu_context_index][s.control_index]->state == 3)
        {
          continue;
        }
        if ((g_menu_controls_by_context[g_menu_context_index][s.control_index]->direct_hotkey == s.full_key) ||
            ((s.queued_key != 0) && (g_menu_controls_by_context[g_menu_context_index][s.control_index]->navigate_hotkeys != (char *)0) &&
             (strchr(g_menu_controls_by_context[g_menu_context_index][s.control_index]->navigate_hotkeys, s.queued_key) != (char *)0)))
        {
          g_menu_current_control_index = s.control_index;
          if (g_menu_controls_by_context[g_menu_context_index][g_menu_current_control_index]->x != -1)
          {
            SetCursorPos(
                g_menu_controls_by_context[g_menu_context_index][g_menu_current_control_index]->x + g_menu_controls_by_context[g_menu_context_index][g_menu_current_control_index]->width / 2,
                g_menu_controls_by_context[g_menu_context_index][g_menu_current_control_index]->y + g_menu_controls_by_context[g_menu_context_index][g_menu_current_control_index]->height / 2);
          }

          if (g_menu_prev_control_index != -1)
          {
            g_menu_controls_by_context[g_menu_context_index][g_menu_prev_control_index]->on_render(g_menu_controls_by_context[g_menu_context_index][g_menu_prev_control_index], 0);
          }

          g_menu_controls_by_context[g_menu_context_index][g_menu_current_control_index]->on_render(g_menu_controls_by_context[g_menu_context_index][g_menu_current_control_index], 1);
          if ((g_menu_controls_by_context[g_menu_context_index][s.control_index]->direct_hotkey == s.full_key) ||
              ((s.queued_key != 0) && (g_menu_controls_by_context[g_menu_context_index][s.control_index]->activate_hotkeys != (char *)0) &&
               (strchr(g_menu_controls_by_context[g_menu_context_index][s.control_index]->activate_hotkeys, s.queued_key) != (char *)0)))
          {
            g_menu_render_guard = 1;
            g_menu_controls_by_context[g_menu_context_index][g_menu_current_control_index]->on_render(g_menu_controls_by_context[g_menu_context_index][g_menu_current_control_index], 2);
            g_menu_render_guard = 0;
            g_menu_prev_control_index = g_menu_current_control_index;
            PopQueuedKeyInput();
            g_menu_input_unhandled = 0;
            return g_menu_current_control_index;
          }
          PopQueuedKeyInput();
          break;
        }
      }

      if ((s.queued_key == 0xd) && (g_menu_current_control_index != -1))
      {
        if (g_menu_prev_control_index != -1)
        {
          g_menu_controls_by_context[g_menu_context_index][g_menu_current_control_index]->on_render(g_menu_controls_by_context[g_menu_context_index][g_menu_prev_control_index], 0);
        }
        g_menu_controls_by_context[g_menu_context_index][g_menu_current_control_index]->on_render(g_menu_controls_by_context[g_menu_context_index][g_menu_current_control_index], 2);
        g_menu_prev_control_index = g_menu_current_control_index;
        PopQueuedKeyInput();
        g_menu_input_unhandled = 0;
        return g_menu_current_control_index;
      }

      g_menu_prev_control_index = g_menu_current_control_index;
    }
  }

  PopQueuedKeyInput();
  if (g_menu_prev_control_index >= 0)
  {
    int in_bounds;

    if ((g_mouse_x < g_menu_controls_by_context[g_menu_context_index][g_menu_prev_control_index]->x) ||
        (g_menu_controls_by_context[g_menu_context_index][g_menu_prev_control_index]->x + g_menu_controls_by_context[g_menu_context_index][g_menu_prev_control_index]->width < g_mouse_x))
    {
      in_bounds = 0;
    }
    else if ((g_mouse_y < g_menu_controls_by_context[g_menu_context_index][g_menu_prev_control_index]->y) ||
             (g_menu_controls_by_context[g_menu_context_index][g_menu_prev_control_index]->height + g_menu_controls_by_context[g_menu_context_index][g_menu_prev_control_index]->y < g_mouse_y))
    {
      in_bounds = 0;
    }
    else
    {
      in_bounds = 1;
    }

    if (in_bounds == 0)
    {
      g_menu_render_guard = 1;
      g_menu_controls_by_context[g_menu_context_index][g_menu_prev_control_index]->on_render(g_menu_controls_by_context[g_menu_context_index][g_menu_prev_control_index], 0);
      g_menu_current_control_index = g_menu_prev_control_index = -1;
      g_menu_render_guard = 0;
    }
  }

  for (s.i = 0; s.i < g_menu_control_count_by_context[g_menu_context_index]; s.i = s.i + 1)
  {
    if (s.i != g_menu_prev_control_index)
    {
      if (g_menu_controls_by_context[g_menu_context_index][s.i]->on_render(g_menu_controls_by_context[g_menu_context_index][s.i], 0) != 0)
      {
        g_menu_current_control_index = s.i;
      }
    }
  }

  if (g_menu_current_control_index != g_menu_prev_control_index)
  {
    if (g_menu_prev_control_index >= 0)
    {
      g_menu_render_guard = 1;
      g_menu_controls_by_context[g_menu_context_index][g_menu_prev_control_index]->on_render(g_menu_controls_by_context[g_menu_context_index][g_menu_prev_control_index], 0);
      g_menu_render_guard = 0;
    }

    g_menu_controls_by_context[g_menu_context_index][g_menu_current_control_index]->on_render(g_menu_controls_by_context[g_menu_context_index][g_menu_current_control_index], 1);
  }

  if ((allow_activate_on_click == 1) && (g_menu_current_control_index >= 0))
  {
    g_menu_controls_by_context[g_menu_context_index][g_menu_current_control_index]->on_render(g_menu_controls_by_context[g_menu_context_index][g_menu_current_control_index], 2);
  }

  g_menu_prev_control_index = g_menu_current_control_index;
  g_menu_input_unhandled = 0;
  return g_menu_current_control_index;
}

// FUNCTION: SHANDALAR 0x004ed005
unsigned int WaitForInputEvent(void)
{
  int has_queued_key;

  if (g_duel_ai_mode_state != 0)
    return 0;

  do
  {
    UpdateMouseSnapshot();
  } while (g_mouse_button_mask_snapshot == 0 && !HasQueuedKeyInput());

  has_queued_key = g_mouse_button_mask_snapshot;
  if (has_queued_key == 0)
  {
    has_queued_key = PopNormalizedQueuedKeyInput();
  }

  ClearInputAndWaitForMouseRelease();
  return has_queued_key;
}

// FUNCTION: SHANDALAR 0x005597ca
unsigned int WaitForInputEventUnlessBlocked(void)
{
  if (g_duel_ai_mode_state == 1)
  {
    return 0;
  }

  return (unk_00742fc4 != 0) ? 0 : WaitForInputEvent();
}

// FUNCTION: SHANDALAR 0x004ecf30
int ApproximateDistance(int param_1, int param_2)
{
  int local_8;

  if (param_1 < 0)
  {
    param_1 = -param_1;
  }
  if (param_2 < 0)
  {
    param_2 = -param_2;
  }
  if (param_2 < param_1)
  {
    local_8 = param_1 * 2 + param_2;
  }
  else
  {
    local_8 = param_2 * 2 + param_1;
  }
  if (local_8 < 0)
  {
    local_8 = 0x7ffe;
  }
  return local_8;
}

// FUNCTION: SHANDALAR 0x004ecfa2
void ClearInputAndWaitForMouseRelease(void)
{
  if (DAT_00586494 != 0)
  {
    do
    {
      UpdateMouseSnapshot();
    } while (g_mouse_button_mask_snapshot != 0);
  }

  while (HasQueuedKeyInput() != 0)
  {
    PopNormalizedQueuedKeyInput();
  }
}

// FUNCTION: SHANDALAR 0x004ecfe3
void ClearQueuedKeyInput(void)
{
  while (HasQueuedKeyInput() != 0)
  {
    PopNormalizedQueuedKeyInput();
  }
}

// FUNCTION: SHANDALAR 0x00522786
void UnloadStatWinDllExports(void)
{
  int i;

  if (g_statwin_dll_module != (HANDLE)0)
  {
    FreeLibrary((HMODULE)g_statwin_dll_module);
    g_statwin_dll_module = (HANDLE)0;
  }

  for (i = 0; i < 3; i = i + 1)
  {
    g_statwin_exports_by_ordinal[i] = 0;
  }
}

// FUNCTION: SHANDALAR 0x00414127
int PopNormalizedQueuedKeyInput(void)
{
  int queued_key;

loop:
  queued_key = PopQueuedKeyInput();
  if (queued_key == 0)
    goto loop;

  switch (queued_key)
  {
  case 0x487e:
  case 0x4800:
    queued_key = 0x4800;
    break;
  case 0x4900:
    queued_key = 0x4900;
    break;
  case 0xf400:
  case 0x4d00:
    queued_key = 0x4d00;
    break;
  case 0x5100:
    queued_key = 0x5100;
    break;
  case 0x5060:
  case 0x5000:
    queued_key = 0x5000;
    break;
  case 0x4f00:
    queued_key = 0x4f00;
    break;
  case 0x4b7c:
  case 0x4b00:
    queued_key = 0x4b00;
    break;
  case 0x475c:
  case 0x4700:
    queued_key = 0x4700;
    break;
  case 0x4838:
    queued_key = 0x4838;
    break;
  case 0x4939:
    queued_key = 0x4939;
    break;
  case 0x4d46:
  case 0x4d36:
    queued_key = 0x4d36;
    break;
  case 0x5133:
    queued_key = 0x5133;
    break;
  case 0x5032:
    queued_key = 0x5032;
    break;
  case 0x4f31:
    queued_key = 0x4f31;
    break;
  case 0x4b43:
  case 0x4b34:
    queued_key = 0x4b34;
    break;
  case 0x4737:
    queued_key = 0x4737;
    break;
  default:
    if (queued_key & 0xff)
    {
      queued_key = queued_key & 0xff;
    }
    break;
  }

  return (int)queued_key;
}

// FUNCTION: SHANDALAR 0x00564ee7
int LoadAdvStringsFile(const char *filename)
{
  struct
  {
    int ok;
    int line_count;
    int i;
    char *next_buf;
  } s;

  s.ok = 1;
  s.ok &= LoadTextSectionStringTable(filename, "PLAYERNAMES", gs_playernames_0077c5a0, 0xe, gs_playernames_buf_0077e2c0, gs_playernames_buf_0077e2c0 + 0x2bc, (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "DIFFICULTYLEVELS", gs_difficultylevels_0077d130, 4, gs_difficultylevels_buf_0074d790, gs_difficultylevels_buf_0074d790 + 0x64, (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "DUNGEON_NAMES", gs_dungeon_names_00780820, 0x11, gs_dungeon_names_buf_0074c970, gs_dungeon_names_buf_0074c970 + 0x352, (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "LAIR_NAMES", gs_lair_names_0077c020, 0x13, gs_lair_names_buf_0074bd30, gs_lair_names_buf_0074bd30 + 0x3b6, (char **)0);

  LoadTextSectionLines(filename, "CITYNAMES_FORMAT");
  strcpy(gs_cityname_format_left_0074c950, text_lines[0]);
  strcpy(gs_cityname_format_right_0077f190, text_lines[1]);

  LoadTextSectionLines(filename, "CITYNAME_VILLAGE");
  strcpy(gs_cityname_village_0077cfd0, text_lines[0]);

  LoadTextSectionLines(filename, "CITYNAME_CASTLE");
  strcpy(gs_cityname_castle_00765dc0, text_lines[0]);

  LoadTextSectionLines(filename, "CITYNAME_MANACASTLE");

  for (s.i = 0; s.i < 5; s.i = s.i + 1)
  {
    strcpy(gs_cityname_manacastle_0077de00[s.i + 1], text_lines[s.i]);
  }
  strcpy(gs_cityname_manacastle_0077de00[0], gs_cityname_castle_00765dc0);

  s.next_buf = gs_citynames_buf_0077d610;
  s.ok &= LoadTextSectionStringTable(filename, "CITYNAMES_FIRSTHALF", gs_citynames_firsthalf_0077e060, 0x10, gs_citynames_buf_0077d610,
                                     gs_citynames_buf_0077d610 + sizeof(gs_citynames_buf_0077d610), &s.next_buf);
  s.ok &= LoadTextSectionStringTable(filename, "CITYNAMES_SECONDHALF", gs_citynames_secondhalf_007653e0, 0x10, s.next_buf,
                                     gs_citynames_buf_0077d610 + sizeof(gs_citynames_buf_0077d610), (char **)0);

  s.next_buf = gs_worldmagic_buf_0074b160;
  LoadTextSectionLines(filename, "WORLDMAGIC");
  strcpy(gs_worldmagic_title_0077e1d0, text_lines[0]);
  s.ok &= LoadTextSectionStringTable(filename, "WORLDMAGIC_NAMES", gs_worldmagic_names_00780660, 0xc, gs_worldmagic_buf_0074b160,
                                     gs_worldmagic_buf_0074b160 + sizeof(gs_worldmagic_buf_0074b160), &s.next_buf);
  s.ok &= LoadTextSectionStringTable(filename, "WORLDMAGIC_EXPLAINS", gs_worldmagic_explains_0074b8f0, 0xc, s.next_buf,
                                     gs_worldmagic_buf_0074b160 + sizeof(gs_worldmagic_buf_0074b160), (char **)0);

  s.ok &= LoadTextSectionStringTable(filename, "LOGSTRINGS", gs_logstrings_0077c9a0, 0x10, gs_logstrings_buf_0077c680,
                                     gs_logstrings_buf_0077c680 + sizeof(gs_logstrings_buf_0077c680), (char **)0);

  LoadTextSectionLines(filename, "WIZARDNAMES");
  for (s.i = 0; s.i < 5; s.i = s.i + 1)
  {
    strcpy(gs_wizardnames_0077ee70[s.i + 1], text_lines[s.i]);
  }
  strcpy(gs_wizardnames_0077ee70[0], "");

  s.line_count = LoadTextSectionLines(filename, "CREATURENAMES");
  s.line_count = MIN(s.line_count, (int)gs_creature_name_count_00593934);
  for (s.i = 0; s.i < s.line_count; s.i = s.i + 1)
  {
    strcpy(gs_creature_names_00591a08[s.i].name, text_lines[s.i]);
  }
  for (s.i = s.line_count; s.i < (int)gs_creature_name_count_00593934; s.i = s.i + 1)
  {
    strcpy(gs_creature_names_00591a08[s.i].name, "");
  }

  s.line_count = LoadTextSectionLines(filename, "CREATURENAME_ARTICLES");
  s.line_count = MIN(s.line_count, (int)gs_creature_name_count_00593934);
  for (s.i = 0; s.i < s.line_count; s.i = s.i + 1)
  {
    strcpy(gs_creature_names_00591a08[s.i].article, text_lines[s.i]);
  }
  for (s.i = s.line_count; s.i < (int)gs_creature_name_count_00593934; s.i = s.i + 1)
  {
    strcpy(gs_creature_names_00591a08[s.i].name, "");
  }

  s.line_count = LoadTextSectionLines(filename, "CREATURENAMES_PLURAL");
  s.line_count = MIN(s.line_count, (int)gs_creature_name_count_00593934);
  for (s.i = 0; s.i < s.line_count; s.i = s.i + 1)
  {
    strcpy(gs_creature_names_00591a08[s.i].plural, text_lines[s.i]);
  }
  for (s.i = s.line_count; s.i < (int)gs_creature_name_count_00593934; s.i = s.i + 1)
  {
    strcpy(gs_creature_names_00591a08[s.i].plural, "");
  }

  LoadTextSectionLines(filename, "DIRECTIONS");
  for (s.i = 0; s.i < 4; s.i = s.i + 1)
  {
    strcpy(gs_directions_00765d50[s.i], text_lines[s.i]);
  }

  s.next_buf = gs_cardclassnames_buf_0074c5c0;
  s.ok &= LoadTextSectionStringTable(filename, "CARDCLASSNAMES", gs_cardclassnames_0077cf70, 9, gs_cardclassnames_buf_0074c5c0,
                                     gs_cardclassnames_buf_0074c5c0 + sizeof(gs_cardclassnames_buf_0074c5c0), &s.next_buf);
  s.ok &= LoadTextSectionStringTable(filename, "CARDCLASSNAMES_PLURAL", gs_cardclassnames_plural_0077e1f0, 9, s.next_buf,
                                     gs_cardclassnames_buf_0074c5c0 + sizeof(gs_cardclassnames_buf_0074c5c0), (char **)0);

  LoadTextSectionLines(filename, "SPELLNAMES");
  strcpy(gs_spellname_primary_0077e6e0, text_lines[0]);
  for (s.i = 1; s.i < 5; s.i = s.i + 1)
  {
    strcpy(gs_spellnames_0077e220[s.i], text_lines[s.i]);
  }
  strcpy(gs_spellnames_0077e220[0], text_lines[6]);

  LoadTextSectionLines(filename, "MANANAMES");
  for (s.i = 0; s.i < 5; s.i = s.i + 1)
  {
    strcpy(gs_mananames_0074d980[s.i + 1], text_lines[s.i]);
  }
  strcpy(gs_mananames_0074d980[0], "");

  LoadTextSectionLines(filename, "LANDWALKS");
  for (s.i = 0; s.i < 5; s.i = s.i + 1)
  {
    strcpy(gs_landwalks_007806f0[s.i + 1], text_lines[s.i]);
  }
  strcpy(gs_landwalks_007806f0[0], "");

  LoadTextSectionLines(filename, "AMULETNAMES");
  for (s.i = 0; s.i < 6; s.i = s.i + 1)
  {
    strcpy(gs_amuletnames_0077d090[s.i], text_lines[s.i]);
  }

  LoadTextSectionLines(filename, "AMULETNAMES_PLURAL");
  for (s.i = 0; s.i < 6; s.i = s.i + 1)
  {
    strcpy(gs_amuletnames_plural_0077edd0[s.i], text_lines[s.i]);
  }

  LoadTextSectionLines(filename, "COLORCARDS");
  for (s.i = 0; s.i < 6; s.i = s.i + 1)
  {
    strcpy(gs_colorcards_0077c5e0[s.i], text_lines[s.i]);
  }

  s.ok &= LoadTextSectionStringTable(filename, "CAVE_SHOWCLUES", gs_cave_showclues_0077efa0, 0x17, gs_cave_showclues_buf_007658d0,
                                     gs_cave_showclues_buf_007658d0 + sizeof(gs_cave_showclues_buf_007658d0), (char **)0);

  s.next_buf = gs_encounter_buf_0074da70;
  s.ok &= LoadTextSectionStringTable(filename, "ENCOUNTER_PREDUEL", gs_encounter_preduel_0077f0d0, 0x30, gs_encounter_buf_0074da70,
                                     gs_encounter_buf_0074da70 + sizeof(gs_encounter_buf_0074da70), &s.next_buf);
  s.ok &= LoadTextSectionStringTable(filename, "ENCOUNTER_POSTDUEL", gs_encounter_postduel_0077f050, 0x20, s.next_buf,
                                     gs_encounter_buf_0074da70 + sizeof(gs_encounter_buf_0074da70), (char **)0);

  s.next_buf = gs_visit_buf_0077f610;
  s.ok &= LoadTextSectionStringTable(filename, "VISIT", gs_visit_0077c4f0, 0x23, gs_visit_buf_0077f610, gs_visit_buf_0077f610 + sizeof(gs_visit_buf_0077f610),
                                     &s.next_buf);
  s.ok &= LoadTextSectionStringTable(filename, "VISIT_CITYBUY", gs_visit_citybuy_0077f1d0, 4, s.next_buf,
                                     gs_visit_buf_0077f610 + sizeof(gs_visit_buf_0077f610), (char **)0);

  s.ok &= LoadTextSectionStringTable(filename, "CASTLEWIN", gs_castlewin_0074b8c0, 0xb, gs_castlewin_buf_0077e700,
                                     gs_castlewin_buf_0077e700 + sizeof(gs_castlewin_buf_0077e700), (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "DUNGEON", gs_dungeon_0077f000, 0x14, gs_dungeon_buf_00780870, gs_dungeon_buf_00780870 + sizeof(gs_dungeon_buf_00780870),
                                     (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "HINTTEXT", gs_hinttext_0077e580, 2, gs_hinttext_buf_0077e5a0, gs_hinttext_buf_0077e5a0 + sizeof(gs_hinttext_buf_0077e5a0),
                                     (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "QUESTFAILED", gs_questfailed_0077c580, 6, gs_questfailed_buf_0074b930,
                                     gs_questfailed_buf_0074b930 + sizeof(gs_questfailed_buf_0074b930), (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "MONSTERLAIR", gs_monsterlair_0074cff0, 6, gs_monsterlair_buf_0074d010,
                                     gs_monsterlair_buf_0074d010 + sizeof(gs_monsterlair_buf_0074d010), (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "BUYANYCARD", gs_buyanycard_0074ccd0, 6, gs_buyanycard_buf_0077f1e0,
                                     gs_buyanycard_buf_0077f1e0 + sizeof(gs_buyanycard_buf_0077f1e0), (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "QUESTSTATUS", gs_queststatus_0077e0a0, 0x1a, gs_queststatus_buf_0074d270,
                                     gs_queststatus_buf_0074d270 + sizeof(gs_queststatus_buf_0074d270), (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "NEWSFLASH", gs_newsflash_0077d140, 0xb, gs_newsflash_buf_0077d1c0,
                                     gs_newsflash_buf_0077d1c0 + sizeof(gs_newsflash_buf_0077d1c0), (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "CITYCARDTEXT", gs_citycardtext_0074ccf0, 0x31, gs_citycardtext_buf_0074c0f0,
                                     gs_citycardtext_buf_0074c0f0 + sizeof(gs_citycardtext_buf_0074c0f0), (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "BROWSE", gs_browse_0074da20, 0x11,
                                     gs_city_browse_buf_0077dc50,
                                     gs_city_browse_buf_0077dc50 + sizeof(gs_city_browse_buf_0077dc50),
                                     (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "SHOWDECK", gs_showdeck_0074b920, 3, gs_showdeck_buf_0074d890,
                                     gs_showdeck_buf_0074d890 + sizeof(gs_showdeck_buf_0074d890), (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "LOADSAVE", gs_loadsave_0077d1b0, 3, gs_loadsave_buf_0077d040, gs_loadsave_buf_0077d040 + sizeof(gs_loadsave_buf_0077d040),
                                     (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "STATS", gs_stats_0077cfa0, 9, gs_stats_buf_0074ce20, gs_stats_buf_0074ce20 + sizeof(gs_stats_buf_0074ce20),
                                     (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "ANALYZE", gs_analyze_0074b870, 0x11, gs_analyze_buf_0074afb0, gs_analyze_buf_0074afb0 + sizeof(gs_analyze_buf_0074afb0),
                                     (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "RIDDLE", gs_riddle_0077cf20, 0x11, gs_riddle_buf_0077ca20, gs_riddle_buf_0077ca20 + sizeof(gs_riddle_buf_0077ca20),
                                     (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "LAIR", gs_lair_0077e180, 0x13, gs_lair_buf_007800c0, gs_lair_buf_007800c0 + sizeof(gs_lair_buf_007800c0),
                                     (char **)0);

  s.next_buf = gs_wiseman_buf_00780c60;
  s.ok &= LoadTextSectionStringTable(filename, "WISEMAN", gs_wiseman_0074d840, 0x13, gs_wiseman_buf_00780c60, gs_wiseman_buf_00780c60 + sizeof(gs_wiseman_buf_00780c60),
                                     &s.next_buf);
  s.ok &= LoadTextSectionStringTable(filename, "CITYWISEMAN", gs_citywiseman_0074d800, 0xd, s.next_buf, gs_wiseman_buf_00780c60 + sizeof(gs_wiseman_buf_00780c60),
                                     (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "CITYSCREEN_BUTTONS", gs_cityscreen_buttons_0077f5e0, 8, gs_cityscreen_buttons_buf_0077f450,
                                     gs_cityscreen_buttons_buf_0077f450 + sizeof(gs_cityscreen_buttons_buf_0077f450), (char **)0);

  LoadTextSectionLines(filename, "SHOWLIST");
  strcpy(gs_showcard_text_0077e110.title, text_lines[0]);
  strcpy(gs_showcard_text_0077e110.subtitle, text_lines[1]);

  LoadTextSectionLines(filename, "SHOWLIBRARY");
  strcpy(gs_showlibrary_text_0074bcc0.title, text_lines[0]);
  strcpy(gs_showlibrary_text_0074bcc0.subtitle, text_lines[1]);

  return s.ok;
}

// FUNCTION: SHANDALAR 0x00565c7e
int LoadTextSectionStringTable(const char *filename, const char *section, char **out_table, int max_entries, char *string_buf,
                               char *string_buf_end, char **out_next_buf)
{
  struct
  {
    int count;
    int overflow;
    size_t line_len;
    int i;
    char *end;
    char *cursor;
  } s;

  s.overflow = 0;
  s.count = LoadTextSectionLines(filename, section);
  s.count = MIN(s.count, max_entries);

  s.cursor = string_buf;
  s.end = string_buf_end;

  for (s.i = 0; s.i < s.count && s.overflow == 0; s.i++)
  {
    s.line_len = strlen(text_lines[s.i]);
    if (s.cursor + s.line_len < s.end)
    {
      strcpy(s.cursor, text_lines[s.i]);
      out_table[s.i] = s.cursor;
      s.cursor = s.cursor + s.line_len + 1;
    }
    else
    {
      s.overflow = 1;
    }
  }

  for (s.i = s.count; s.i < max_entries; s.i++)
  {
    out_table[s.i] = (char *)"";
  }

  if (out_next_buf != (char **)0)
  {
    *out_next_buf = s.cursor;
  }

  if (s.overflow != 0)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}

// FUNCTION: SHANDALAR 0x00565fdb
int FindNextTextBlock(char *scan_start, char *scan_end, int *out_block_start, int *out_next_scan)
{
  if (scan_start == (char *)0 || scan_end == (char *)0 || scan_end <= scan_start || out_block_start == (int *)0 ||
      out_next_scan == (int *)0)
  {
    return 0;
  }

  while (*scan_start != '\0' && scan_start < scan_end && strncmp(scan_start, "STARTBLOCK", 10))
  {
    ++scan_start;
  }

  if (strncmp(scan_start, "STARTBLOCK", 10) == 0)
  {
    scan_start += 0xc;
    if (out_block_start != (int *)0)
    {
      *out_block_start = (int)scan_start;
    }

    while (*scan_start != '\0' && scan_start < scan_end && strncmp(scan_start, "ENDBLOCK", 8))
    {
      ++scan_start;
    }

    if (strncmp(scan_start, "ENDBLOCK", 8) == 0)
    {
      *scan_start = '\0';
      if (out_next_scan != (int *)0)
      {
        *out_next_scan = (int)(scan_start + 8);
      }
    }
    else if (out_next_scan != (int *)0)
    {
      *out_next_scan = 0;
    }
    return 1;
  }
  else
  {
    return 0;
  }
}

// FUNCTION: SHANDALAR 0x00565dbc
int LoadAdvBlocksFile(const char *filename)
{
  struct
  {
    HANDLE file_handle;
    int ok;
    DWORD bytes_read;
    DWORD file_size;
    int block_text_start;
    int i;
    char *buffer_end;
    char *carriage_return;
    char *cursor;
    char *next_block_scan;
  } s;

  s.ok = 1;
  s.file_handle = CreateFileA(filename, 0x80000000, 1, (LPSECURITY_ATTRIBUTES)0, 3, 0x8000080, (HANDLE)0);
  if (s.file_handle != (HANDLE)-1)
  {
    s.file_size = GetFileSize(s.file_handle, (LPDWORD)0);
    g_advblocks_file_buffer = (char *)malloc(s.file_size + 1);
    if (g_advblocks_file_buffer != (char *)0)
    {
      ReadFile(s.file_handle, (void *)g_advblocks_file_buffer, s.file_size, &s.bytes_read, (LPOVERLAPPED)0);
      s.cursor = g_advblocks_file_buffer;
      s.buffer_end = s.cursor + s.bytes_read;

      for (s.i = 0; s.i < 4; ++s.i)
      {
        if (FindNextTextBlock(s.cursor, s.buffer_end, &s.block_text_start, (int *)&s.next_block_scan) != 0)
        {
          gs_advblock_city_0074c930[s.i] = (char *)s.block_text_start;
          s.cursor = s.next_block_scan;
          while (s.carriage_return = strchr(gs_advblock_city_0074c930[s.i], 0xd))
          {
            strcpy(s.carriage_return, s.carriage_return + 1);
          }
        }
        else
        {
          s.ok = 0;
        }
      }

      for (s.i = 0; s.i < 0xc; ++s.i)
      {
        if (FindNextTextBlock(s.cursor, s.buffer_end, &s.block_text_start, (int *)&s.next_block_scan) != 0)
        {
          gs_advblock_general_0077c9e0[s.i] = (char *)s.block_text_start;
          s.cursor = s.next_block_scan;
          while (s.carriage_return = strchr(gs_advblock_general_0077c9e0[s.i], 0xd))
          {
            strcpy(s.carriage_return, s.carriage_return + 1);
          }
        }
        else
        {
          s.ok = 0;
        }
      }
    }
    else
    {
      s.ok = 0;
    }

    CloseHandle(s.file_handle);
  }
  else
  {
    s.ok = 0;
  }

  return s.ok;
}

// FUNCTION: SHANDALAR 0x00562d03
void FUN_00562d03(void)
{
  LoadSoundWithDriveFallback("x:sound\\kwalkl.wav", 0, 0);
  LoadSoundWithDriveFallback("x:sound\\kwalkr.wav", 1, 0);
  LoadSoundWithDriveFallback("x:sound\\bwalkl.wav", 2, 0);
  LoadSoundWithDriveFallback("x:sound\\bwalkr.wav", 3, 0);
  LoadSoundWithDriveFallback("x:sound\\gwalkl.wav", 4, 0);
  LoadSoundWithDriveFallback("x:sound\\gwalkr.wav", 5, 0);
  LoadSoundWithDriveFallback("x:sound\\rwalkl.wav", 6, 0);
  LoadSoundWithDriveFallback("x:sound\\rwalkr.wav", 7, 0);
  LoadSoundWithDriveFallback("x:sound\\wwalkl.wav", 8, 0);
  LoadSoundWithDriveFallback("x:sound\\wwalkr.wav", 9, 0);
  LoadSoundWithDriveFallback("x:sound\\kbird1.wav", 10, 0);
  LoadSoundWithDriveFallback("x:sound\\bbird1.wav", 0xb, 0);
  LoadSoundWithDriveFallback("x:sound\\gbird1.wav", 0xc, 0);
  LoadSoundWithDriveFallback("x:sound\\rbird1.wav", 0xd, 0);
  LoadSoundWithDriveFallback("x:sound\\wbird1.wav", 0xe, 0);
}

// FUNCTION: SHANDALAR 0x00565faa
void FUN_00565faa(void)
{
  if (g_advblocks_file_buffer != 0)
  {
    free(g_advblocks_file_buffer);
    g_advblocks_file_buffer = 0;
  }
}

// FUNCTION: SHANDALAR 0x00562e0d
char FindDriveWithAsset(char *filename)
{
  FILE *asset_file;
  char drive_string[256] = "a:\\";

  for (; drive_string[0] <= (int)'z'; ++drive_string[0])
  {
    if (GetDriveTypeA(drive_string) == 5)
    {
      strcat(drive_string, filename);
      asset_file = fopen(drive_string, "rb");
      if (asset_file != (FILE *)0)
      {
        fclose(asset_file);
        return drive_string[0];
      }
      else
      {
        drive_string[2] = 0;
      }
    }
  }
}

// FUNCTION: SHANDALAR 0x005626b0
void PlaySoundEffectOnChannel(char *sound_path, int channel, int volume, int pitch_percent, int pan_percent)
{
  Sound sound_playback;

  sound_unload(channel);
  LoadSoundWithDriveFallback(sound_path, channel, 0);
  memset(&sound_playback, 0, sizeof(sound_playback));
  sound_playback.volume = volume << 2;
  sound_playback.sampleRate = (pitch_percent * 0x5622) / 100;
  sound_playback.pan = pan_percent << 2;
  sound_playback.flags &= 0xfffffffe;
  sound_playback.flags &= 0xffffffef;
  sound_play(channel, &sound_playback);
}

// FUNCTION: SHANDALAR 0x00562ed0
char GetSoundAssetDriveLetter(void)
{
  char cwd[256];

  if (g_sound_drive_initialized == 0)
  {
    g_sound_drive_letter = FindDriveWithAsset("sound\\locmus1.wav");
    g_sound_drive_initialized = 1;
  }

  if (g_local_sound_missing == 0)
  {
    getcwd(cwd, 0x100);
    return cwd[0];
  }
  else
  {
    return g_sound_drive_letter;
  }
}

// FUNCTION: SHANDALAR 0x00562f92
unsigned int LoadSoundWithDriveFallback(char *filename, int channel, int flags)
{
  if (g_cached_cwd_initialized == 0)
  {
    _getcwd(g_cached_cwd, 0x100);
    g_cached_cwd_initialized = 1;
  }

  if (filename[0] == 'x')
  {
    filename[0] = g_cached_cwd[0];
    if (FileExists(filename) == 0)
    {
      filename[0] = GetSoundAssetDriveLetter();
    }
  }

  while (g_sound_loader_busy != 0)
  {
  }

  sound_load(filename, channel, flags);
  return 0;
}

// FUNCTION: SHANDALAR 0x00417dc6
int FileExists(const char *filename)
{
  FILE *file;

  file = fopen(filename, "rt");
  if (file == (FILE *)0)
  {
    return 0;
  }

  fclose(file);
  return 1;
}

// FUNCTION: SHANDALAR 0x0046e6f0
DWORD WINAPI FUN_0046e6f0(LPVOID param_1)
{
  struct
  {
    void *page;
    int i;
    int max_pages;
  } s;

  (void)param_1;

  g_advbuttons_ini_file = fopen("advButtons.txt", "rt");
  strcpy(g_ini_string_scratch, "");
  g_done_text_table_entry = LoadIniEscapedStringTable(g_advbuttons_ini_file, "done", g_ini_string_scratch, 0)[0];

  FUN_00578c80(LoadFontConfigIfPresent("misc.exe", (char *)0));
  FUN_00578c80(LoadFontConfigIfPresent("mgraphic.exe", "fonts.cv"));
  FUN_00578c80(LoadFontConfigIfPresent("nsound.cvl", (char *)0));

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

  PTR_DAT_0058332c->max_x = ScaleUiCoordinate(0x280);
  PTR_DAT_00583304->max_x = PTR_DAT_0058332c->max_x;
  PTR_DAT_005832dc->max_x = PTR_DAT_00583304->max_x;
  PTR_DAT_005832b4->max_x = PTR_DAT_005832dc->max_x;
  PTR_DAT_00583304->max_x = ScaleUiCoordinate(0x1e0);
  PTR_DAT_005832dc->max_y = PTR_DAT_00583304->max_x;
  PTR_DAT_005832b4->max_y = PTR_DAT_005832dc->max_y;
  PTR_DAT_0058332c->max_y = ScaleUiCoordinate(0x1e0) - ScaleUiCoordinate(0x148);
  PTR_DAT_00583354->max_y = ScaleUiCoordinate(0x148);
  PTR_DAT_00583354->max_x = ScaleUiCoordinate(0x40);
  PTR_DAT_005832b4->font_slot = 1;

  PresentGraphicsPage(0);
  DAT_00586494 = FUN_00578c20();
  do
  {
    FUN_0055db50();
  } while (g_adventure_world_exit_requested == 0);

  UnloadFontSlot(5);
  if (DAT_00586494 != 0)
  {
    FUN_00578c30();
  }

  PostMessageA(g_main_window_hwnd, 0x10, 0, 0);
}

// FUNCTION: SHANDALAR 0x004cea4c
LONG ChangeDisplayResolution(DWORD width, DWORD height)
{
  DEVMODEA dev_mode;

  if (height == 0 && width == 0)
  {
    return ChangeDisplaySettingsA((DEVMODEA *)0, 0);
  }

  if (height == 0 || width == 0)
  {
    return -1;
  }

  dev_mode.dmSize = 0x9c;
  dev_mode.dmFields = 0x180000;
  dev_mode.dmPelsWidth = width;
  dev_mode.dmPelsHeight = height;
  return ChangeDisplaySettingsA(&dev_mode, 0);
}

// FUNCTION: SHANDALAR 0x004ceacb
void RestoreDisplayResolution(void)
{
  ChangeDisplayResolution(0, 0);
}

// FUNCTION: SHANDALAR 0x004cea02
unsigned int InitializeSoundPresenceState(void)
{
  g_local_sound_missing = (unsigned int)(FileExists("sound\\locmus1.wav") == 0);
  _DAT_0074840c = 0;
  return 0;
}

// FUNCTION: SHANDALAR 0x004ce9c6
void ShutdownUiTimer(void)
{
  timeKillEvent(g_timer_event_handle);
  timeEndPeriod(g_timer_resolution_ms);
}

// FUNCTION: SHANDALAR 0x004ce9e9
void FUN_004ce9e9(void)
{
  SetSystemPaletteUse(global_main_hdc, 1);
}

// FUNCTION: SHANDALAR 0x004ce8cd
void CALLBACK UiTimerTickCallback(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
  (void)uID;
  (void)uMsg;
  (void)dwUser;
  (void)dw1;
  (void)dw2;

  if (g_timer_thread_handle_ready == 0)
  {
    g_timer_thread_handle_ready = 1;
    g_timer_thread_handle = GetCurrentThread();
    DuplicateHandle(GetCurrentProcess(), g_timer_thread_handle, GetCurrentProcess(), &g_timer_thread_handle, 0x1f03ff, FALSE, 0);
  }

  ++g_ui_tick_count;

  if (g_sound_loader_busy != 0)
  {
  }
  else
  {
    g_sound_loader_busy = 1;
    update_snd();
    g_sound_loader_busy = 0;
  }
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
    PTR_DAT_005832b4->max_x = global_screen_width - 1;
    PTR_DAT_005832b4->max_y = global_screen_height - 1;
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
    PTR_DAT_005832b4->max_x = global_screen_width - 1;
    PTR_DAT_005832b4->max_y = global_screen_height - 1;
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
    FUN_00562d03();
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
  atexit(FUN_004ce9e9);

  g_main_thread_handle = GetCurrentThread();
  DuplicateHandle(GetCurrentProcess(), g_main_thread_handle, GetCurrentProcess(), &g_main_thread_handle, 0x1f03ff, FALSE, 0);

  if (g_shared_startup_lock_initialized == 0)
  {
    InitializeCriticalSection(&g_shared_startup_lock);
    g_shared_startup_lock_initialized = 1;
  }

  g_loader_thread_handle = CreateThread((LPSECURITY_ATTRIBUTES)0, 0x2000, FUN_0046e6f0, (LPVOID)0, 0, &s.thread_id);

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

  FUN_00565faa();
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
