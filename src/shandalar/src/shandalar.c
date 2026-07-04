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
#include "magic/src/global_state.h"
#include "magic/src/global_strings.h"
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
  int unk_00;
  int timer;
  int unk_08;
  int unk_0c;
} WorldMagicSlotTimer;

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

int HandleMainMenuButtonControlEvent(void *control_ptr, int event_type);
int HandlePortraitMainMenuControlEvent(void *control_ptr, int event_type);
int HandleWorldMagicChoiceControlEvent(void *control_ptr, int event_type);
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
char *PTR_s_advinter800_pic_00589de8;
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
int g_skip_world_sfx_preload;
// GLOBAL: SHANDALAR 0x00591224
int g_loadsave_skip_esc;
// GLOBAL: SHANDALAR 0x00748418
UINT g_timer_resolution_ms;
// GLOBAL: SHANDALAR 0x007483fc
UINT g_timer_event_handle;
// GLOBAL: SHANDALAR 0x0078cefc
int g_cursor_visibility_depth;
// GLOBAL: SHANDALAR 0x00589de4
UINT g_timer_period_ms = 0x21;
// GLOBAL: SHANDALAR 0x00589df0
int g_ui_tick_count;
// GLOBAL: SHANDALAR 0x00589dec
int DAT_00589dec = 0x30;
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
// GLOBAL: SHANDALAR 0x0073c00c
int g_card_count = 722;
// GLOBAL: SHANDALAR 0x0073e9d8
char g_sound_drive_letter;
// GLOBAL: SHANDALAR 0x0067a3b8
char *g_advblocks_file_buffer;
// GLOBAL: SHANDALAR 0x005919ac
int g_drive_prefix_a_colon_backslash_dword = 0x005c3a61;
// GLOBAL: SHANDALAR 0x0078cf08
FILE *g_advbuttons_ini_file;
// GLOBAL: SHANDALAR 0x0078cf10
char g_ui_message_buffer[0x1000];
// GLOBAL: SHANDALAR 0x0078df10
char g_ini_string_scratch[0x28];
// GLOBAL: SHANDALAR 0x0078df38
int g_done_text_table_entry;
// GLOBAL: SHANDALAR 0x00586494
int DAT_00586494;
// GLOBAL: SHANDALAR 0x009300f0
int DAT_009300f0;
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
int DAT_00591214 = 1;
// GLOBAL: SHANDALAR 0x00591218
int g_world_move_dir_index;
// GLOBAL: SHANDALAR 0x0059121c
int DAT_0059121c;
// GLOBAL: SHANDALAR 0x00591220
int DAT_00591220 = -1;
// GLOBAL: SHANDALAR 0x0059126c
int DAT_0059126c = -1;
// GLOBAL: SHANDALAR 0x0078990c
int DAT_0078990c[10];
// GLOBAL: SHANDALAR 0x0073ea70
int DAT_0073ea70[8];
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
HANDLE g_statwin_dll_module;
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
int DAT_0058c5ec[0x20];
// GLOBAL: SHANDALAR 0x0058c5fc
int DAT_0058c5fc[0x100];
// GLOBAL: SHANDALAR 0x0058c620
int DAT_0058c620[0x20];
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
EncodedImage *g_land_sprite_entries[110];

// GLOBAL: SHANDALAR 0x00748660
EncodedImage *g_tsprite2_grid_sprite_entries[0xc];
// GLOBAL: SHANDALAR 0x00748690
EncodedImage *g_land_tile_sprite_entries[0x10];
// GLOBAL: SHANDALAR 0x00748700
EncodedImage *g_ttsprite_alt_sprite_entries[8];
// GLOBAL: SHANDALAR 0x00748720
EncodedImage *g_cstline1_sprite_entries[0x54];
// GLOBAL: SHANDALAR 0x00748870
EncodedImage *g_ttsprite_aux_sprite_entries[0x18];
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
EncodedImage *g_sland_sprite_entries[110];
// GLOBAL: SHANDALAR 0x00749280
EncodedImage *g_daysnew_sprite_entries[0xc];
// GLOBAL: SHANDALAR 0x007492b0
EncodedImage *g_icons_sprite_entries[0x20];
// GLOBAL: SHANDALAR 0x00749350
int g_ego_sprite_width;
// GLOBAL: SHANDALAR 0x00749354
int g_sego_sprite_width;
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
char g_opening_menu_sprite_work_buffer[0x1680];
// GLOBAL: SHANDALAR 0x0078df40
EncodedImage *DAT_0078df40[10];
// GLOBAL: SHANDALAR 0x0058b584
int g_menu_context_index;
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
// GLOBAL: SHANDALAR 0x008c84f0
BITMAPINFO DAT_008c84f0;
// GLOBAL: SHANDALAR 0x008c7408
int DAT_008c7408[7];
// GLOBAL: SHANDALAR 0x0073e990
jmp_buf DAT_0073e990;
// GLOBAL: SHANDALAR 0x0073e9e0
jmp_buf DAT_0073e9e0;
// GLOBAL: SHANDALAR 0x006527b0
int DAT_006527b0;
// GLOBAL: SHANDALAR 0x00590764
int DAT_00590764;
// GLOBAL: SHANDALAR 0x00590768
int g_adventure_ui_layout_dirty;
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
int g_text_menu_initial_selection;
// GLOBAL: SHANDALAR 0x00580d84
int g_text_menu_mouse_released;
// GLOBAL: SHANDALAR 0x00580d88
int g_text_menu_last_selection;
// GLOBAL: SHANDALAR 0x00580d8c
int g_text_menu_show_ok_button;
// GLOBAL: SHANDALAR 0x00580d90
int g_text_menu_mouse_active;
// GLOBAL: SHANDALAR 0x00580d94
int g_text_menu_timeout_seconds;
// GLOBAL: SHANDALAR 0x00580d98
int g_text_menu_line_height = 8;
// GLOBAL: SHANDALAR 0x00580d9c
int g_text_menu_color_normal = 0x71;
// GLOBAL: SHANDALAR 0x00580da0
int g_text_menu_color_selected = 0xe3;
// GLOBAL: SHANDALAR 0x005863ac
int DAT_005863ac = 5;
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
int DAT_005873d4;

// GLOBAL: SHANDALAR 0x0074cfe4
int g_selected_save_slot_index;

int InitLicenseSecretsFromRegistry(void);
int LoadAdvStringsFile(const char *filename);
int LoadTextSectionStringTable(const char *filename, const char *section, char **out_table, int max_entries, char *string_buf,
                               char *string_buf_end, char **out_next_buf);
int LoadAdvBlocksFile(const char *filename);
int FindNextTextBlock(char *scan_start, char *scan_end, int *out_block_start, int *out_next_scan);
void FUN_00417946(char *out, int csvid, int field, char *csv_name);
void FUN_00559999(void);
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
unsigned int FindDriveWithAsset(char *filename);
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
int FUN_00522508(int param_1);
int RunFacemakerFlow(void);
void InitializeNewGameState(void);
void FUN_004f6d90(void);
void InitializeAnimatedNoiseGrid(void);
int FUN_004f7fb9(int param_1, int param_2);
void PropagatePathConnectivity(void);
int FUN_004f717a(void);
void GenerateTownConnections(void);
int CreateTownConnectionPath(int start_x, int start_y, int target_x, int target_y);
void FloodFillPathConnectivity(int x, int y, unsigned int depth);
void FUN_005081fa(void);
unsigned int FUN_0043146b(int param_1, int param_2);
unsigned int FUN_005611c8(unsigned int param_1);
unsigned int FUN_004314ca(int param_1, int param_2);
void FUN_00431526(unsigned int param_1, int param_2, int param_3);
void FUN_00431593(unsigned int param_1, int param_2, int param_3);
void MarkPathConnection(int x, int y, int direction_index);
int FUN_004f82f2(int param_1, int param_2);
void SaveGameToSlot(int save_slot_index);
int FUN_005031a8(void);
int LoadGameFromSlot(int save_slot_index);
int FUN_0040dffd(int param_1);
void FUN_004290e2(int param_1, int param_2);
int LoadStatWinDllExports(void);
void LoadOpeningMenuSpriteResources(void);
int RenderAdvMenuControlDisabled(AdvMenuControl *control);
void InitializeMainMenuAndWorldMagicChoiceControls(void);
void UpdateWorldMagicUnlockProgress(void);
void RebuildDeckEntriesByCardGroup(void);
void RefreshAdventureInterfaceLayout(void);
int ConsumeUiTickCount(void);
void RenderAdventureWorldScene(int world_x, int world_y, int world_state);
int RunStartupMenuAndQueueInput(void);
int QueuePendingMenuActionInput(void);
void FUN_0055e808(void);
void FUN_0055fd27(void);
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
int FUN_004ecf30(int param_1, int param_2);
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

void FUN_004ce992(int param_1);
int FUN_0056c705(int param_1);
unsigned int FUN_004bdccc(unsigned int param_1);
int FUN_004be0bf(unsigned int param_1, int param_2, int param_3, int param_4, int param_5, int param_6);
int FUN_0056bcf7(unsigned int param_1, unsigned int param_2);
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
unsigned int WorldRoadTileHasDirection(int tile_x, int tile_y, char direction_index);
int FreeOpeningMenuSpriteWorkEntries(int work_entry_index_a, int work_entry_index_b);
int SignNonZero(int value);
int RestoreAdventureUiPaletteAndFocus(void);
int VisitTownSlot(int town_index);
int RunSaveMenuAndSelectSlot(void);
void *FUN_0055060c(int param_1);
void FUN_00559807(void);
void EnsureAdvfac64Loaded(int state);
void FUN_005616f9(void);
void FUN_00562169(void);
void FUN_00562736(int param_1, int param_2, int param_3, int param_4);
void FUN_0056279e(int param_1, int param_2, int param_3);
void FUN_00562835(char *param_1, int param_2);
void FUN_00562a29(int param_1);
void FUN_0050a5c1(int param_1);
void ShowCityInfoScreen(int param_1);
void ShowDungeonCluesScreen(int unused);
void FUN_00549002(void);
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
  FUN_00559999();
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
  AnimatePaletteToColor(0, DAT_00589dec);

  switch (s.game_setup_mode)
  {
  case 0:
  difficult_menu:
    g_shandalar_difficulty = RunDifficultyMenu();
    AnimatePaletteToColor(0, DAT_00589dec);
    if (g_shandalar_difficulty == -1)
    {
      goto opening_menu;
    }

  color_menu:
    g_selected_wizard_color = RunColorMenu();
    g_starting_color = g_selected_wizard_color;
    DAT_0091c99c = g_selected_wizard_color;
    DAT_0091c9a0 = FUN_00522508(3);
    AnimatePaletteToColor(0, DAT_00589dec);
    if (g_starting_color == -1)
    {
      goto difficult_menu;
    }

    g_player_is_male = RunFacemakerFlow();
    AnimatePaletteToColor(0, DAT_00589dec);
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
    FUN_005081fa();
    Gold = (5 - g_shandalar_difficulty) * 0x32;
    DAT_00591214 = 0;
    do
    {
      do
      {
        g_world_player_x = FUN_00522508(0x40) * 0x20 + 0x10;
        g_world_player_y = FUN_00522508(0x40) * 0x20 + 0x10;
        s.tile_mask = FUN_005611c8(FUN_0043146b(g_world_player_x / 0x20, g_world_player_y / 0x20));
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
    DAT_009300f0 = 1;
    return 0;
  }

  for (s.loop_index = 0; s.loop_index < 0x80; s.loop_index = s.loop_index + 1)
  {
    if (g_town_slots[s.loop_index].location_type == 5)
    {
      s.tile_mask = FUN_0040dffd(FUN_005611c8(FUN_0043146b(g_town_slots[s.loop_index].world_x, g_town_slots[s.loop_index].world_y))) - 1;
      DAT_0073ea70[s.tile_mask] = 1;
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

  if (DAT_009300f0 != 0)
  {
  }
  else
  {

#ifdef _DEBUG
    // This normally only runs on new world creation which is annoying for debugging.
    g_world_scene_reveal_effect_pending = 1;
#endif
    do
    {
#ifndef MODERN_FIXES
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

      FUN_0055e808();
      FUN_0055fd27();

      TickWorldMagicSlotTimers();
      UpdateMouseSnapshot();
      UpdateMenuControlSelection(g_mouse_x_snapshot, g_mouse_y_snapshot, g_mouse_button_down_mask);
      g_monster_timer = g_monster_timer + 1;
      ClearInputAndWaitForMouseRelease();
      ClearQueuedKeyInput();
      setjmp(&DAT_0073e9e0);
    } while (DAT_009300f0 == 0);
  }

  AnimatePaletteToColor(0, DAT_00589dec);
  UnloadStatWinDllExports();
  FUN_00469099();
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
  int done;
  int selected_menu_entry;
  int next_menu_entry;
  int current_menu_entry;
  int timer_seconds_left;
  int mouse_active;
  int previous_menu_entry;
  int line_height;
  int last_timer_seconds;
  int key_code;
  char icon_width_scaled[4];
  char *seconds_text;

  if ((clear_input_before_show != 0) && (g_text_menu_abort_requested == 0))
  {
    ClearInputAndWaitForMouseRelease();
  }

  previous_menu_entry = -1;
  g_text_menu_mouse_active = 0;
  g_text_menu_mouse_released = 0;
  g_text_menu_finish_flash = 0;
  selected_menu_entry = 0;
  if (g_text_menu_initial_selection != -1)
  {
    selected_menu_entry = g_text_menu_initial_selection;
  }

  g_text_menu_first_option_line = -1;
  g_text_menu_needs_layout = 1;
  g_text_menu_hovered_selection = -1;
  g_text_menu_line_height = GetFontLineHeight(PTR_DAT_005832b4->font_slot);
  line_height = 0x16;
  g_text_menu_color_base = 0xff;
  if (DAT_00789934 == 0)
  {
    line_height = -1;
  }
  (void)line_height;

  mouse_active = 0;
  done = 0;
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
    last_timer_seconds = -1;
  }

  while (!done)
  {
    g_text_menu_needs_layout = 0;
    g_mouse_button_mask_snapshot = 0;
    if (DAT_007483f0 == 0)
    {
      FUN_004ece9a();
    }

    current_menu_entry = g_text_menu_timeout_seconds;
    if (g_text_menu_timeout_seconds != -1)
    {
      timer_seconds_left = current_menu_entry - GetUiTickCount() / (DAT_005863ac * 0x3c);
      if (timer_seconds_left == 0)
      {
        selected_menu_entry = -1;
        done = 1;
      }
      if (last_timer_seconds != timer_seconds_left)
      {
        seconds_text = _itoa(timer_seconds_left, DAT_0078cef0, 10);
        strcpy(icon_width_scaled, seconds_text);
        FillGraphicsRect(PTR_DAT_005832b4, g_text_menu_right - 0xe, g_text_menu_top + 4, 0xc, 7, 0xbc);
        DrawCenteredTextLineClamped(icon_width_scaled, g_text_menu_right - 8, g_text_menu_top + 5, 0xff);
        last_timer_seconds = timer_seconds_left;
      }
    }

    UpdateMouseSnapshot();
    if ((g_mouse_button_mask_snapshot == 0) && (mouse_active == 0))
    {
      if (HasQueuedKeyInput() != 0)
      {
        key_code = PopNormalizedQueuedKeyInput();
        current_menu_entry = selected_menu_entry;
        if (key_code < 0x1c)
        {
          if (key_code == 0x1b)
          {
            selected_menu_entry = -1;
            done = 1;
          }
          else if (key_code == 0xd)
          {
            if ((g_text_menu_disabled_option_mask & (1 << ((unsigned char)selected_menu_entry & 0x1f))) == 0)
            {
              done = 1;
            }
          }
          else
          {
            next_menu_entry = current_menu_entry + 1;
            while (next_menu_entry <= 0x1f)
            {
              if ((g_text_menu_hotkey_by_option[next_menu_entry] != -1) &&
                  ((((int)(char)g_text_menu_hotkey_by_option[next_menu_entry] ^ key_code) & 0x1f) == 0))
              {
                selected_menu_entry = next_menu_entry;
                break;
              }
              next_menu_entry = next_menu_entry + 1;
            }
          }
        }
        else if (key_code == 0x20)
        {
          if ((g_text_menu_disabled_option_mask & (1 << ((unsigned char)selected_menu_entry & 0x1f))) == 0)
          {
            done = 1;
          }
        }
        else if (key_code == 0x4800)
        {
          if (0 < selected_menu_entry)
          {
            selected_menu_entry = selected_menu_entry - 1;
          }
        }
        else if (key_code == 0x5000)
        {
          if (selected_menu_entry < g_text_menu_option_count - 1)
          {
            selected_menu_entry = selected_menu_entry + 1;
          }
        }
        else
        {
          next_menu_entry = current_menu_entry + 1;
          while (next_menu_entry <= 0x1f)
          {
            if ((g_text_menu_hotkey_by_option[next_menu_entry] != -1) &&
                ((((int)(char)g_text_menu_hotkey_by_option[next_menu_entry] ^ key_code) & 0x1f) == 0))
            {
              selected_menu_entry = next_menu_entry;
              break;
            }
            next_menu_entry = next_menu_entry + 1;
          }
        }
      }
    }
    else
    {
      g_text_menu_mouse_active = 1;
      mouse_active = 1;
      if (g_mouse_button_mask_snapshot == 2)
      {
        g_text_menu_mouse_released = 1;
      }

      selected_menu_entry = ((g_mouse_y_snapshot - g_text_menu_top) - 4) / g_text_menu_line_height - g_text_menu_first_option_line;
      if ((g_mouse_x_snapshot < g_text_menu_left) || (g_text_menu_right < g_mouse_x_snapshot))
      {
        selected_menu_entry = -1;
      }

      if ((g_text_menu_disabled_option_mask & (1 << ((unsigned char)selected_menu_entry & 0x1f))) == 0)
      {
        if (g_mouse_button_mask_snapshot == 0)
        {
          done = 1;
        }
      }
      else
      {
        selected_menu_entry = previous_menu_entry;
      }
    }

    if ((selected_menu_entry < 0) || (g_text_menu_option_count <= selected_menu_entry) || (g_text_menu_ok_mode != 0) || (g_text_menu_force_cancel != 0))
    {
      selected_menu_entry = -1;
    }

    if (selected_menu_entry != previous_menu_entry)
    {
      DrawTextMenu(menu_text, selected_menu_entry);
      previous_menu_entry = selected_menu_entry;
      g_text_menu_hovered_selection = selected_menu_entry;
    }
  }

  g_text_menu_finish_flash = 1;
  if (selected_menu_entry == -1)
  {
    g_text_menu_mouse_released = 0;
  }
  else
  {
    FUN_0046ed33();
    DrawTextMenu(menu_text, selected_menu_entry);
    FUN_004ce992(0x14);
    FUN_0046ed03();
  }
  g_text_menu_timeout_seconds = -1;
  g_text_menu_last_selection = -1;
  g_text_menu_initial_selection = -1;
  g_text_menu_show_ok_button = 0;
  g_text_menu_force_cancel = 0;
  g_text_menu_disabled_option_mask = 0;
  g_text_menu_caret_prefix_mask = 0;
  return selected_menu_entry;
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
  int tile_origin_x;
  int tile_origin_y;
  int row;
  int col;
  EncodedImage **box_sprites;

  tile_columns = (width + 0x17 + ((width + 0x17 >> 0x1f) & 0xf)) >> 4;
  tile_rows = (height + 0x17 + ((height + 0x17 >> 0x1f) & 0xf)) >> 4;
  tile_origin_x = (x - 4) - ((tile_columns * 0x10) - (width + 8)) / 2;
  tile_origin_y = (y - 4) - ((tile_rows * 0x10) - (height + 8)) / 2;
  box_sprites = (EncodedImage **)&g_dialog_box_sprite_bank.frame[frame_style][0];

  PTR_DAT_005832b4->page_number = 1;
  FillGraphicsRect(PTR_DAT_005832b4, tile_origin_x, tile_origin_y, tile_columns << 4, tile_rows << 4, 0xe3);

  for (row = 0; row < tile_rows; row = row + 1)
  {
    for (col = 0; col < tile_columns; col = col + 1)
    {
      DrawEncodedImageUnscaled(PTR_DAT_005832b4, col * 0x10 + tile_origin_x, row * 0x10 + tile_origin_y, box_sprites[0]);
    }
  }

  for (col = 0; col < tile_columns; col = col + 1)
  {
    DrawEncodedImageUnscaled(PTR_DAT_005832b4, col * 0x10 + tile_origin_x, tile_origin_y - 10, box_sprites[5]);
    DrawEncodedImageUnscaled(PTR_DAT_005832b4, col * 0x10 + tile_origin_x, tile_rows * 0x10 + tile_origin_y - 6, box_sprites[7]);
  }

  for (row = 0; row < tile_rows; row = row + 1)
  {
    DrawEncodedImageUnscaled(PTR_DAT_005832b4, tile_origin_x - 10, row * 0x10 + tile_origin_y, box_sprites[8]);
    DrawEncodedImageUnscaled(PTR_DAT_005832b4, tile_columns * 0x10 + tile_origin_x - 6, row * 0x10 + tile_origin_y, box_sprites[6]);
  }

  DrawEncodedImageUnscaled(PTR_DAT_005832b4, tile_origin_x - 10, tile_origin_y - 10, box_sprites[1]);
  DrawEncodedImageUnscaled(PTR_DAT_005832b4, tile_columns * 0x10 + tile_origin_x - 6, tile_origin_y - 10, box_sprites[2]);
  DrawEncodedImageUnscaled(PTR_DAT_005832b4, tile_origin_x - 10, tile_rows * 0x10 + tile_origin_y - 6, box_sprites[3]);
  DrawEncodedImageUnscaled(PTR_DAT_005832b4, tile_columns * 0x10 + tile_origin_x - 6, tile_rows * 0x10 + tile_origin_y - 6, box_sprites[4]);

  PTR_DAT_005832b4->page_number = 0;
  BlitGraphicsRect(PTR_DAT_005832dc, tile_origin_x - 10, tile_origin_y - 10, tile_columns * 0x10 + 0x14, tile_rows * 0x10 + 0x14,
                   PTR_DAT_005832b4, tile_origin_x - 10, tile_origin_y - 10);
}

// FUNCTION: SHANDALAR 0x004136ab
void DrawDialogBoxFrameAutoStyle(int x, int y, int width, int height)
{
  if (g_text_box_frame_color_override != 0)
  {
    DrawTiledDialogBoxFrame(x, y, width, height, g_text_box_frame_color_override);
    g_text_box_frame_color_override = 0;
  }
  else
  {
    DrawTiledDialogBoxFrame(x, y, width, height, ((int)(y + ((y >> 0x1f) & 7U)) >> 3) & 3);
  }
}

// FUNCTION: SHANDALAR 0x00430e2d
void DrawTextLineClamped(char *text, int x, int y, int color_index)
{
  int text_width;
  int font_height;
  int right_limit;
  int bottom_limit;

  if (x < 0)
  {
    x = 0;
  }
  if (y < 0)
  {
    y = 0;
  }

  text_width = MeasureTextLineWidth(text);
  if (global_screen_width <= x + text_width)
  {
    right_limit = global_screen_width - 1;
    text_width = MeasureTextLineWidth(text);
    x = right_limit - text_width;
  }
  font_height = GetFontLineHeight(*(int *)((char *)PTR_DAT_005832b4 + 0x20));
  if (global_screen_height <= y + font_height)
  {
    bottom_limit = global_screen_height - 1;
    font_height = GetFontLineHeight(*(int *)((char *)PTR_DAT_005832b4 + 0x20));
    y = bottom_limit - font_height;
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
  int font_slot;
  int text_width;
  char *cursor;

  cursor = text;
  font_slot = PTR_DAT_005832b4->font_slot;
  text_width = 0;
  while (*cursor != '\0')
  {
    text_width = text_width + GetFontCharWidth(font_slot, *cursor);
    cursor = cursor + 1;
  }
  return text_width;
}

// FUNCTION: SHANDALAR 0x0043104f
void DrawCenteredTextLineClamped(char *text, int center_x, int y, int color_index)
{
  int text_width;

  text_width = MeasureTextLineWidth(text);
  PTR_DAT_005832b4->draw_shadow_enabled = 0;
  DrawTextLineClamped(text, center_x - text_width / 2, y, color_index);
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
    char location_block_start_indexc;
    char location_block_start_indexb[255];
    FILE *inner_index;
    int entry_index;
  } locals;

  locals.inner_index = fopen("hints.txt", "rt");
  locals.local_118 = 0;
  do
  {
    locals.entry_index = fscanf(locals.inner_index, "%[^\n]", &locals.location_block_start_indexc);
    if (locals.location_block_start_indexc == '.')
    {
      sscanf(locals.location_block_start_indexb, "%d %d %s", &locals.local_124, &locals.local_114, locals.local_120);
      DAT_0097e450[locals.local_118].first = locals.local_124;
      DAT_0097e450[locals.local_118].second = locals.local_114;
      DAT_0097df40[locals.local_118] = 0;

      if (strchr(locals.local_120, 'A') != (char *)0)
      {
        DAT_0097df40[locals.local_118] = DAT_0097df40[locals.local_118] | 1;
      }

      if (strchr(locals.local_120, 'B') != (char *)0)
      {
        DAT_0097df40[locals.local_118] = DAT_0097df40[locals.local_118] | 2;
      }

      if (strchr(locals.local_120, 'C') != (char *)0)
      {
        DAT_0097df40[locals.local_118] = DAT_0097df40[locals.local_118] | 4;
      }

      if (strchr(locals.local_120, 'D') != (char *)0)
      {
        DAT_0097df40[locals.local_118] = DAT_0097df40[locals.local_118] | 8;
      }

      locals.local_110 = FUN_0056c705(locals.local_124);
      locals.local_110 = FUN_0056c705(locals.local_114);
      locals.entry_index = fscanf(locals.inner_index, "%[\n]", &locals.location_block_start_indexc);
      DAT_0097db40[locals.local_118] = ftell(locals.inner_index);
      locals.local_118 = locals.local_118 + 1;
    }
    else
    {
      locals.entry_index = fscanf(locals.inner_index, "%[\n]", &locals.location_block_start_indexc);
    }
  } while ((locals.local_118 < 0x100) && (locals.entry_index != -1));

  do
  {
    DAT_0097e450[locals.local_118].second = -1;
    DAT_0097e450[locals.local_118].first = DAT_0097e450[locals.local_118].second;
    locals.local_118 = locals.local_118 + 1;
  } while (locals.local_118 < 0x100);

  fclose(locals.inner_index);
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
  } locals;

  for (locals.location_block_start_index = 0; locals.location_block_start_index < 0x4e2; locals.location_block_start_index = locals.location_block_start_index + 1)
  {
    DAT_005a6198[locals.location_block_start_index] = -1;
  }

  locals.entry_index = fopen("concise.csv", "rt");
  locals.inner_index = 0;

  for (locals.location_block_start_index = 0; locals.location_block_start_index < g_card_count; locals.location_block_start_index = locals.location_block_start_index + 1)
  {
    locals.inner_index = global_cards_data[locals.location_block_start_index].id;
    locals.local_4 = fscanf(locals.entry_index, "%d %d %ld\n", &locals.icon_width_scaled, &locals.icon_height_scaled, &locals.selected_state_sprite);
    global_cards_data[locals.location_block_start_index].rarity = (unsigned char)locals.icon_height_scaled;
    DAT_005a6198[locals.inner_index] = locals.selected_state_sprite;
  }

  fclose(locals.entry_index);
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
  } locals;

  locals.limit = MIN(g_menu_control_count_by_context[g_menu_context_index], first_index + count);

  g_menu_render_guard = 1;
  for (locals.i = first_index; locals.i < locals.limit; locals.i = locals.i + 1)
  {
    g_menu_controls_by_context[g_menu_context_index][locals.i]->on_render(g_menu_controls_by_context[g_menu_context_index][locals.i], 0);
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
int FUN_00522508(int param_1)
{
  return (param_1 > 1) ? rand() % param_1 : 0;
}

// FUNCTION: SHANDALAR 0x004ce992
void FUN_004ce992(int delay)
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
  } locals;

  if (DAT_008bd200 != 0)
  {
    return;
  }

  for (locals.location_block_start_index = 0; locals.location_block_start_index < 500; locals.location_block_start_index = locals.location_block_start_index + 1)
  {
    deck[locals.location_block_start_index] = -1;
  }

  if (DAT_00715f10)
  {
    return;
  }

  for (locals.location_block_start_index = 0; locals.location_block_start_index < 7; locals.location_block_start_index = locals.location_block_start_index + 1)
  {
    DAT_008b3240[locals.location_block_start_index] = 0;
    DAT_008c7408[locals.location_block_start_index] = -1;
  }

  g_world_magic_bitmap = g_world_magic_bitmap | (1 << (g_selected_wizard_color * 2));
  g_world_magic_slot_timers[g_selected_wizard_color * 2].unk_00 = 0;

  locals.uVar2 = 1 << (BYTE)g_selected_wizard_color;
  locals.entry_index = g_shandalar_difficulty + 1;
  locals.local_4 = locals.entry_index;
  if (g_shandalar_difficulty == 3)
  {
    locals.entry_index = 1;
    locals.entry_index = 3;
  }

  switch (g_shandalar_difficulty)
  {
  case 0:
    if ((locals.uVar2 < 1) & 0x20)
    {
      FUN_004be0bf(locals.uVar2, 0xd, 7, 0xf, 1, 1);
    }
    else
    {
      FUN_004be0bf(locals.uVar2, 0xd, 0xc, 10, 1, 1);
    }
    break;
  case 1:
    FUN_004be0bf(locals.uVar2, 0xb, 4, 0xc, 1, 1);
    FUN_004be0bf(FUN_004bdccc(locals.uVar2), 4, 3, 4, 0, 1);
    break;
  case 2:
    FUN_004be0bf(locals.uVar2, 9, 3, 9, 1, 1);
    locals.uVar3 = FUN_004bdccc(locals.uVar2);
    FUN_004be0bf(locals.uVar3, 5, 3, 4, 0, 1);
    FUN_004be0bf(FUN_004bdccc(locals.uVar2 | locals.uVar3), 4, 3, 3, 0, 1);
    break;
  case 3:
    FUN_004be0bf(locals.uVar2, 6, 3, 5, 1, 1);
    FUN_004be0bf(1, 0xb, 5, 0xe, 0, 1);
    break;
  }

  g_journal_entry_count = 0;
  for (locals.location_block_start_index = 0; locals.location_block_start_index < 5; locals.location_block_start_index = locals.location_block_start_index + 1)
  {
    g_amulet_inventory[locals.location_block_start_index] = 0;
  }

  DAT_0078990c[g_selected_wizard_color]++;
  for (locals.location_block_start_index = 0; locals.location_block_start_index < 3 - g_shandalar_difficulty; locals.location_block_start_index = locals.location_block_start_index + 1)
  {
    locals.icon_width_scaled = FUN_00522508(5);
    g_amulet_inventory[locals.icon_width_scaled]++;
  }

  for (locals.location_block_start_index = 0; locals.location_block_start_index < 0x80; locals.location_block_start_index = locals.location_block_start_index + 1)
  {
    g_town_slots[locals.location_block_start_index].location_type = -1;
  }

  for (locals.location_block_start_index = 0; locals.location_block_start_index < 8; locals.location_block_start_index = locals.location_block_start_index + 1)
  {
    g_lair_or_monster_slots[locals.location_block_start_index].entry_type = -1;
  }

  for (locals.location_block_start_index = 0; locals.location_block_start_index < 1000; locals.location_block_start_index = locals.location_block_start_index + 1)
  {
    g_duel_victory_log[0] = 0;
  }

  for (locals.location_block_start_index = 0; locals.location_block_start_index < 4; locals.location_block_start_index = locals.location_block_start_index + 1)
  {
    DAT_007a7d10[locals.location_block_start_index] = 8;
  }

  for (locals.location_block_start_index = 0; locals.location_block_start_index < 0x96; locals.location_block_start_index = locals.location_block_start_index + 1)
  {
    deck[locals.location_block_start_index] = deck[locals.location_block_start_index] | 0x10000;
  }
  g_selected_wizard_color = -1;
}

// FUNCTION: SHANDALAR 0x004bdccc
unsigned int FUN_004bdccc(unsigned int param_1)
{
  int entry_index;

  do
  {
    entry_index = FUN_00522508(5) + 1;
  } while ((param_1 & (1U << (entry_index & 0xff))) != 0);

  return 1U << (entry_index & 0xff);
}

// FUNCTION: SHANDALAR 0x004be0bf
int FUN_004be0bf(unsigned int param_1, int param_2, int param_3, int param_4, int param_5, int param_6)
{
  int bVar1;
  int avatar_sprite_index;
  unsigned int icon_x_scaled;
  unsigned int icon_height_scaled;
  unsigned int icon_width_scaled;
  int selected_state_sprite;
  int location_block_start_index;
  int inner_index;

  for (location_block_start_index = 0; location_block_start_index < param_2; location_block_start_index = location_block_start_index + 1)
  {
    selected_state_sprite = FUN_0056bcf7(1, param_1);
    avatar_sprite_index = FUN_0056c0e5((int)(char)global_cards_data[selected_state_sprite].color, param_1, 0);
    if ((avatar_sprite_index == 0) || (4 < selected_state_sprite) || ((global_cards_data[selected_state_sprite].expansion & 0xc1) == 0))
    {
      location_block_start_index = location_block_start_index - 1;
    }
    else
    {
      FUN_0056bd9d(selected_state_sprite);
    }
  }

  for (location_block_start_index = 0; location_block_start_index < param_3; location_block_start_index = location_block_start_index + 1)
  {
    if (param_6 == 0)
    {
      icon_width_scaled = 0;
    }
    else
    {
      icon_width_scaled = rand();
      icon_width_scaled = icon_width_scaled & 1;
    }

    if (icon_width_scaled == 0)
    {
      icon_height_scaled = param_1;
    }
    else
    {
      icon_height_scaled = 1;
    }

    selected_state_sprite = FUN_0056bcf7((-(unsigned int)(icon_width_scaled == 0) & 0xffffffc4) + 0x40, icon_height_scaled);
    if (((g_shandalar_difficulty == 0) && ((global_cards_data[selected_state_sprite].static_ability & 3) != 0)) ||
        ((global_cards_data[selected_state_sprite].extra_ability & 0x900) != 0))
    {
      location_block_start_index = location_block_start_index - 1;
    }
    else
    {
      if ((global_cards_data[selected_state_sprite].type & 4) != 0)
      {
        icon_width_scaled = 0;
      }

      if (icon_width_scaled == 0)
      {
        icon_x_scaled = param_1;
      }
      else
      {
        icon_x_scaled = 1;
      }

      avatar_sprite_index = FUN_0056c0e5((int)(char)global_cards_data[selected_state_sprite].color, icon_x_scaled, 0);
      if ((avatar_sprite_index == 0) || ((avatar_sprite_index = FUN_0056c5ea(selected_state_sprite), (int)((~location_block_start_index & 1) + 1) < avatar_sprite_index)) ||
          ((global_cards_data[selected_state_sprite].expansion & 0xc1) == 0))
      {
        location_block_start_index = location_block_start_index - 1;
      }
      else
      {
        FUN_0056bd9d(selected_state_sprite);
      }
    }
  }

  inner_index = 0;
  for (location_block_start_index = 0; location_block_start_index < param_4; location_block_start_index = location_block_start_index + 1)
  {
    selected_state_sprite = FUN_0056bcf7(2, param_1);
    if (((g_shandalar_difficulty < 4) && ((global_cards_data[selected_state_sprite].static_ability & 3) != 0)) ||
        ((global_cards_data[selected_state_sprite].extra_ability & 0x900) != 0))
    {
      location_block_start_index = location_block_start_index - 1;
      inner_index = inner_index - 1;
    }
    else
    {
      if (inner_index < 1000)
      {
        avatar_sprite_index = FUN_004bb1cf(selected_state_sprite);
        bVar1 = 0 < avatar_sprite_index;
      }
      else
      {
        bVar1 = 1;
      }

      avatar_sprite_index = FUN_0056c0e5((int)(char)global_cards_data[selected_state_sprite].color, param_1, 0);
      if ((avatar_sprite_index == 0) || ((avatar_sprite_index = FUN_0056c5ea(selected_state_sprite), (int)((~location_block_start_index & 1) + 1) < avatar_sprite_index)) ||
          ((!bVar1 || ((global_cards_data[selected_state_sprite].expansion & 0xc1) == 0))))
      {
        location_block_start_index = location_block_start_index - 1;
      }
      else
      {
        FUN_0056bd9d(selected_state_sprite);
      }
    }
    inner_index = inner_index + 1;
  }

  if (param_5 != 0)
  {
    do
    {
      do
      {
        selected_state_sprite = FUN_0056bcf7(0xe, 1);
        avatar_sprite_index = FUN_0056c0e5((int)(char)global_cards_data[selected_state_sprite].color, param_1, 1);
      } while (avatar_sprite_index == 0);
      avatar_sprite_index = FUN_0056c5ea(selected_state_sprite);
    } while ((avatar_sprite_index < 3) || ((avatar_sprite_index = FUN_004bb1cf(selected_state_sprite), avatar_sprite_index < 1)) ||
             ((g_shandalar_difficulty == 0 && ((global_cards_data[selected_state_sprite].static_ability & 3) != 0))) ||
             ((global_cards_data[selected_state_sprite].extra_ability & 0x900) != 0) ||
             ((global_cards_data[selected_state_sprite].expansion & 0xc1) == 0));
  }

  FUN_0056bd9d(selected_state_sprite);
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
int FUN_0056bcf7(unsigned int param_1, unsigned int param_2)
{
  int bVar1;
  int avatar_sprite_index;
  int location_block_start_index;

  location_block_start_index = 0;
  do
  {
    bVar1 = 0;
    avatar_sprite_index = FUN_00522508(g_card_count - 0x39);
    if (((param_1 == 0) || ((param_1 & global_cards_data[avatar_sprite_index].type) != 0)) &&
        ((param_2 == 1) || ((param_2 & (int)(char)global_cards_data[avatar_sprite_index].color) != 0)))
    {
      bVar1 = 1;
    }
    if (bVar1 == 0)
    {
      location_block_start_index = location_block_start_index + 1;
    }
  } while ((bVar1 == 0) && (location_block_start_index < 999));

  return avatar_sprite_index;
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

  param_1 = FUN_0040dffd(param_1);
  param_2 = FUN_0040dffd(param_2);
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

  FUN_00417946(s.rarity_str, global_cards_data[param_1].id, 9, "info.csv");

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
  int color_index;
  int copies_in_deck;
  int total_non_market_cards;
  int i;
  int max_color_count;
  int aiStack_20[7];

  if ((int)param_1 < 5)
  {
    copies_in_deck = 99;
  }
  else
  {
    for (i = 0; i < 7; i = i + 1)
    {
      aiStack_20[i] = 0;
    }

    total_non_market_cards = 0;
    copies_in_deck = 0;

    for (i = 0; i < 500; i = i + 1)
    {
      if ((deck[i] != -1) && ((deck[i] & 0x4000) == 0))
      {
        total_non_market_cards = total_non_market_cards + 1;
        color_index = FUN_0040dffd(global_cards_data[(unsigned int)deck[i] & 0xfff].color);
        aiStack_20[color_index] = aiStack_20[color_index] + 1;
      }

      if (((unsigned int)deck[i] & 0xffff7fffU) == param_1)
      {
        copies_in_deck = copies_in_deck + 1;
        DAT_005aa62c = i;
      }
    }

    max_color_count = -1;
    for (i = 1; i < 7; i = i + 1)
    {
      if (max_color_count < aiStack_20[i])
      {
        max_color_count = aiStack_20[i];
      }
    }

    g_deck_color_bitmap = 0;
    for (i = 1; i < 7; i = i + 1)
    {
      if (((max_color_count * 2) / 3) <= aiStack_20[i])
      {
        g_deck_color_bitmap = g_deck_color_bitmap | (1 << (unsigned char)i);
      }
    }

    max_color_count = 1;
    if (0x27 < total_non_market_cards)
    {
      max_color_count = 2;
    }
    if (0x3b < total_non_market_cards)
    {
      max_color_count = 3;
    }
    if ((g_world_magic_bitmap & 0x20) != 0)
    {
      max_color_count = max_color_count + 1;
    }

    if ((global_cards_data[param_1].extra_ability & 0x100) != 0)
    {
      max_color_count = (g_shandalar_difficulty <= max_color_count);
    }
    else if ((global_cards_data[param_1].expansion & 0xc1) == 0)
    {
      max_color_count = max_color_count / 2;
    }

    copies_in_deck = max_color_count - copies_in_deck;
  }

  return copies_in_deck;
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
void FUN_004290e2(int param_1, int param_2)
{
  if (g_journal_entry_count < 2000)
  {
    g_journal_entries[g_journal_entry_count][0] = param_1;
    g_journal_entries[g_journal_entry_count][1] = param_2;
    g_journal_entries[g_journal_entry_count][2] = g_world_player_x / 32;
    g_journal_entries[g_journal_entry_count][3] = g_world_player_y / 32;
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
    FUN_004290e2((int)param_1 / 0x100 + 8, param_1 & 0xff);
  }
  s.entry_index = FUN_0040dffd(global_cards_data[param_1].color) * 0x20 + (unsigned int)global_cards_data[param_1].type * 0x100 +
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
    s.local_4 = FUN_0040dffd(global_cards_data[s.inner_index].color) * 0x20 + (int)global_cards_data[s.inner_index].name[0] +
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
      s.entry_index = FUN_0043146b(s.location_block_start_index, s.icon_width_scaled);
      if (s.entry_index == 0)
      {
        s.neighbor_is_open = 0;
        for (s.i = 1; s.i <= 8; s.i += 2)
        {
          if (FUN_0043146b(s.location_block_start_index + g_neighbor_dx[s.i], s.icon_width_scaled + g_neighbor_dy[s.i]) == 0)
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
  if (FUN_004f717a() == 0)
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
int FUN_004f717a(void)
{
  int preview_panel_y_offset;
  int avatar_sprite_index;
  int clamped_required_wins;
  int iVar4;
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
      g_world_magic_slot_timers[local_2c].unk_00 = 0;
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
        preview_panel_y_offset = FUN_00522508(0x40);
        avatar_sprite_index = FUN_00522508(0x40);
        tile_type = FUN_0043146b(preview_panel_y_offset, avatar_sprite_index);
        if (tile_type != 0)
        {
          avatar_draw_y = 0x7fff;
          icon_width_scaled = 0x7fff;
          for (local_34 = 0; local_34 < 0x80; local_34 = local_34 + 1)
          {
            if (g_town_slots[local_34].world_x != -1)
            {
              clamped_required_wins = FUN_004ecf30(preview_panel_y_offset - g_town_slots[local_34].world_x, avatar_sprite_index - g_town_slots[local_34].world_y);
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
              *(int *)&g_town_slots[inner_index].data_18_to_63[local_34 * 4] = 0xfffffc18;
            }
            *(int *)&g_town_slots[inner_index].data_18_to_63[0x3c] = 0xfffffc18;
            *(int *)&g_town_slots[inner_index].data_18_to_63[0x40] = 0xfffffc18;

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

            world_magic_mask = FUN_005611c8(tile_type);
            if ((inner_index != 0) && ((g_town_slots[inner_index].location_type == 3) || (g_town_slots[inner_index].location_type == 2)))
            {
              for (local_34 = 0; local_34 < 99; local_34 = local_34 + 1)
              {
                clamped_required_wins = FUN_00522508(10);
                clamped_required_wins = clamped_required_wins + 2;
                if ((g_world_magic_slot_timers[clamped_required_wins].unk_00 == 0) && ((world_magic_mask & 1 << (((char)(clamped_required_wins / 2)) & 0x1f)) != 0))
                {
                  g_world_magic_slot_timers[clamped_required_wins].unk_00 = inner_index;
                  break;
                }
              }

              if (0x62 < local_34)
              {
                clamped_required_wins = FUN_00522508(2);
                g_world_magic_slot_timers[clamped_required_wins].unk_00 = inner_index;
              }

              if (seeded_world_magic_town_count < 10)
              {
                g_town_slots[inner_index].status_and_ruling_wizard = g_town_slots[inner_index].status_and_ruling_wizard | 1;
                seeded_world_magic_town_count = seeded_world_magic_town_count + 1;
              }
            }

            FUN_00431526(0x10, preview_panel_y_offset, avatar_sprite_index);
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
          preview_panel_y_offset = FUN_00522508(0x80);
        } while (g_town_slots[preview_panel_y_offset].location_type < 2);
      } while ((g_town_slots[preview_panel_y_offset].location_type == 4) || (g_town_slots[preview_panel_y_offset].trade_color_and_type != 0));
      g_town_slots[preview_panel_y_offset].trade_color_and_type = 1 << (((char)local_30) & 0x1f);
    }

    local_34 = FUN_00522508(0xc);
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
      if (g_world_magic_slot_timers[local_2c].unk_00 == 0)
      {
        generated_valid_world = 0;
      }
      if ((g_world_magic_bitmap & 1 << (((char)local_2c) & 0x1f)) != 0)
      {
        g_world_magic_slot_timers[local_2c].unk_00 = 0;
      }
    }

    if (generated_valid_world != 0)
    {
      return 1;
    }

    for (local_2c = 0; local_2c < 0x80; local_2c = local_2c + 1)
    {
      FUN_00431593(0x10, g_town_slots[local_2c].world_x, g_town_slots[local_2c].world_y);
    }

    for (local_2c = 0; local_2c < 0xc; local_2c = local_2c + 1)
    {
      g_world_magic_slot_timers[local_2c].unk_00 = 0;
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
        s.candidate_index = FUN_00522508(0x80);
        s.distance =
            FUN_004ecf30(g_town_slots[s.town_index].world_x - g_town_slots[s.candidate_index].world_x,
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
  FUN_00431526(0x20, s.cur_x, s.cur_y);

  do
  {
    s.target_dist = FUN_004ecf30(target_x - s.cur_x, target_y - s.cur_y);
    s.best_direction = -1;
    s.best_score = 0x7fff;

    for (s.direction_index = 1; s.direction_index <= 8; s.direction_index = s.direction_index + 1)
    {
      s.next_x = g_neighbor_dx[s.direction_index] + s.cur_x;
      s.next_y = g_neighbor_dy[s.direction_index] + s.cur_y;

      s.tile_type = FUN_0043146b(s.next_x, s.next_y);
      if (s.tile_type == 0)
      {
        continue;
      }

      s.score = FUN_004ecf30(target_x - s.next_x, target_y - s.next_y);
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
  int random_nibble;
  int location_block_start_index;
  int inner_index;
  int entry_index;

  for (entry_index = 0; entry_index < 0x12; entry_index = entry_index + 1)
  {
    for (location_block_start_index = 0; location_block_start_index < 0x12; location_block_start_index = location_block_start_index + 1)
    {
      random_nibble = FUN_00522508(0x10);
      g_animated_noise_grid[location_block_start_index + entry_index * 0x13] = (char)random_nibble;
    }
    g_animated_noise_grid[entry_index * 0x13 + 0x12] = g_animated_noise_grid[entry_index * 0x13];
  }
  for (location_block_start_index = 0; location_block_start_index < 0x12; location_block_start_index = location_block_start_index + 1)
  {
    g_animated_noise_grid[location_block_start_index + 0x156] = g_animated_noise_grid[location_block_start_index];
  }
  for (entry_index = 0; entry_index < 0x11; entry_index = entry_index + 1)
  {
    for (location_block_start_index = 0; location_block_start_index < 0x11; location_block_start_index = location_block_start_index + 1)
    {
      for (inner_index = 1; inner_index < 9; inner_index = inner_index + 1)
      {
      }
      g_animated_noise_grid[location_block_start_index + entry_index * 0x13 + 0x170] = g_animated_noise_grid[location_block_start_index + entry_index * 0x13];
    }
  }
  for (entry_index = 0; entry_index < 0x11; entry_index = entry_index + 1)
  {
    g_animated_noise_grid[entry_index * 0x13 + 0x180] = g_animated_noise_grid[entry_index * 0x13 + 0x170];
  }
  for (location_block_start_index = 0; location_block_start_index < 0x11; location_block_start_index = location_block_start_index + 1)
  {
    g_animated_noise_grid[location_block_start_index + 0x2a0] = g_animated_noise_grid[location_block_start_index + 0x170];
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
void FUN_005081fa(void)
{
  unsigned char bVar1;
  unsigned char bVar2;
  int clamped_required_wins;
  int param1;
  unsigned int uVar4;
  int iVar5;
  int local_2c;
  int local_28;
  int icon_y_scaled;
  int icon_x_scaled;
  int location_block_start_index;
  int inner_index;
  int entry_index;
  ShandalarEncounterSlot *slot;

  for (entry_index = 0; entry_index < 0xf; entry_index = entry_index + 1)
  {
    g_castle_dungeon_slots[entry_index].card_slot_3 = -1;
    g_castle_dungeon_slots[entry_index].card_slot_2 = g_castle_dungeon_slots[entry_index].card_slot_3;
    g_castle_dungeon_slots[entry_index].card_slot_1 = g_castle_dungeon_slots[entry_index].card_slot_2;
    g_castle_dungeon_slots[entry_index].times_entered = -1;
    g_castle_dungeon_slots[entry_index].reserved_2c = g_castle_dungeon_slots[entry_index].times_entered;
  }

  for (local_2c = 0; local_2c < g_card_count - 0x39; local_2c = local_2c + 1)
  {
    if ((global_cards_data[local_2c].extra_ability & 0x100) != 0)
    {
      do
      {
        clamped_required_wins = FUN_00522508(10);
        clamped_required_wins = clamped_required_wins + 5;
      } while (g_castle_dungeon_slots[clamped_required_wins].card_slot_3 != -1);

      if (g_castle_dungeon_slots[clamped_required_wins].card_slot_1 == -1)
      {
        g_castle_dungeon_slots[clamped_required_wins].card_slot_1 = local_2c;
      }
      else if (g_castle_dungeon_slots[clamped_required_wins].card_slot_2 == -1)
      {
        g_castle_dungeon_slots[clamped_required_wins].card_slot_2 = local_2c;
      }
      else
      {
        g_castle_dungeon_slots[clamped_required_wins].card_slot_3 = local_2c;
      }
    }
  }

  entry_index = 0;
  do
  {
    if (0xe < entry_index)
    {
      return;
    }

    do
    {
      do
      {
        do
        {
          clamped_required_wins = FUN_00522508(0x40);
          param1 = FUN_00522508(0x40);
          uVar4 = FUN_0043146b(clamped_required_wins, param1);
        } while (uVar4 == 0);
        uVar4 = FUN_004314ca(clamped_required_wins, param1);
      } while ((uVar4 & 0x30) != 0);

      local_28 = 0xff;
      for (icon_x_scaled = 0; icon_x_scaled < 0x80; icon_x_scaled = icon_x_scaled + 1)
      {
        if ((entry_index < 5) && (g_town_slots[icon_x_scaled].location_type == 4))
        {
          uVar4 = FUN_0043146b(g_town_slots[icon_x_scaled].world_x, g_town_slots[icon_x_scaled].world_y);
          uVar4 = FUN_005611c8(uVar4);
          if (uVar4 == 1 << (((char)entry_index + 1U) & 0x1f))
          {
            location_block_start_index = icon_x_scaled;
          }
        }

        if (((1 < g_town_slots[icon_x_scaled].location_type) && (g_town_slots[icon_x_scaled].location_type != 4)) &&
            ((iVar5 = FUN_004ecf30(g_town_slots[icon_x_scaled].world_x - clamped_required_wins,
                                   g_town_slots[icon_x_scaled].world_y - param1),
              iVar5 < local_28)))
        {
          icon_y_scaled = icon_x_scaled;
          local_28 = iVar5;
        }
      }

      for (icon_x_scaled = 0; icon_x_scaled < entry_index; icon_x_scaled = icon_x_scaled + 1)
      {
        iVar5 = FUN_004ecf30(g_castle_dungeon_slots[icon_x_scaled].world_x - clamped_required_wins, g_castle_dungeon_slots[icon_x_scaled].world_y - param1);
        if (iVar5 < local_28)
        {
          local_28 = iVar5;
        }
      }
    } while (local_28 < 4);

    FUN_00431526(0x40, clamped_required_wins, param1);
    slot = g_castle_dungeon_slots + entry_index;
    slot->world_x = clamped_required_wins;
    slot->world_y = param1;
    slot->north_of_town_index = icon_y_scaled;
    clamped_required_wins = FUN_00522508(5);
    slot->color = (unsigned char)clamped_required_wins + 1;
    slot->monster_flags = 2;

    if (entry_index < 5)
    {
      slot->color = (unsigned char)entry_index + 1;
      slot->monster_flags = 0x81;
      slot->world_x = g_town_slots[location_block_start_index].world_x;
      slot->world_y = g_town_slots[location_block_start_index].world_y;
    }

    bVar1 = slot->monster_flags;
    clamped_required_wins = FUN_00522508(2);
    if (clamped_required_wins + 1 < (int)(unsigned int)bVar1)
    {
      inner_index = 0;
      for (icon_x_scaled = 0; icon_x_scaled < (int)(unsigned int)(unsigned char)slot->monster_flags; icon_x_scaled = icon_x_scaled + 1)
      {
        inner_index = inner_index + icon_x_scaled * 2 + 4;
      }
    }
    else
    {
      if ((unsigned char)slot->monster_flags < 2)
      {
        inner_index = 0x10;
      }
      else
      {
        inner_index = 0x1c;
      }
      slot->monster_flags = slot->monster_flags | 0x80;
    }

    if (slot->card_slot_2 == -1)
    {
      inner_index = (inner_index * 3) / 2;
    }
    if (slot->card_slot_3 != -1)
    {
      inner_index = (inner_index * 2) / 3;
    }

    bVar1 = slot->monster_flags;
    bVar2 = slot->monster_flags;
    clamped_required_wins = FUN_00522508(2);
    slot->card_in_effect = DAT_0058c5fc[((((bVar1 & 0xc0) == 0) - 1 & 4) + (bVar2 & 0x7f) + clamped_required_wins)];
    slot->rules_bitmap = 1;

    switch ((int)(inner_index + (inner_index >> 0x1f & 3U)) >> 2)
    {
    case 0:
    case 1:
    case 2:
      slot->monster_flags = slot->monster_flags + 1;
    case 3:
      slot->card_in_effect = DAT_0058c5ec[(char)slot->color];
      clamped_required_wins = FUN_00522508(5);
      slot->rules_bitmap = slot->rules_bitmap | 1 << (((char)clamped_required_wins + 4U) & 0x1f);
      break;
    case 4:
      slot->card_in_effect = DAT_0058c5ec[(char)slot->color];
      break;
    case 5:
      clamped_required_wins = FUN_00522508(5);
      slot->rules_bitmap = slot->rules_bitmap | 1 << (((char)clamped_required_wins + 4U) & 0x1f);
      slot->card_in_effect = -1;
      break;
    case 6:
      slot->card_in_effect = -1;
      break;
    }

    if (((slot->rules_bitmap & 0x100) != 0) || (((slot->monster_flags & 0x3f) < 2) && ((slot->rules_bitmap & 0x20) != 0)))
    {
      slot->card_in_effect = DAT_0058c5ec[(char)slot->color];
      slot->rules_bitmap = slot->rules_bitmap & 0xfffffedf;
    }

    if ((slot->monster_flags & 0x7f) == 1)
    {
      slot->rules_bitmap = slot->rules_bitmap & 0xfffffffe;
    }

    if (entry_index < 5)
    {
      slot->rules_bitmap = slot->rules_bitmap | 1;
      slot->rules_bitmap = slot->rules_bitmap | 2;
      slot->card_in_effect = DAT_0058c620[g_shandalar_difficulty + entry_index * 4];
    }
    entry_index = entry_index + 1;
  } while (1);
}

// FUNCTION: SHANDALAR 0x00431351
void DrawEncodedImageUiScaled(FacemakerWindowBounds *dst, int x_320, int y_200, EncodedImage *sprite, int width_320, int height_200)
{
  DrawEncodedImageResampled(dst, (x_320 * global_screen_width) / 0x280, (global_screen_height * y_200) / 0x1e0,
                            (width_320 * global_screen_width) / 0x280, (height_200 * global_screen_height) / 0x1e0, sprite);
}

// FUNCTION: SHANDALAR 0x0043146b
unsigned int FUN_0043146b(int param_1, int param_2)
{
  if ((0x40 <= param_1) || (param_1 < 0))
  {
    param_1 = 0;
  }
  if ((0x40 <= param_2) || (param_2 < 0))
  {
    param_2 = 0;
  }
  return GetGraphicsPixelColorRef(PTR_DAT_00583304, param_1, param_2) & 0xf;
}

// FUNCTION: SHANDALAR 0x005611c8
unsigned int FUN_005611c8(unsigned int param_1)
{
  unsigned int entry_index;

  switch (param_1)
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
void FUN_00431526(unsigned int param_1, int x, int y)
{
  if (x >= 0x40 || x < 0)
    return;

  if (y >= 0x40 || y < 0)
    return;

  PutGraphicsPixel(PTR_DAT_00583304, x, y, GetGraphicsPixelColorRef(PTR_DAT_00583304, x, y) | param_1);
}

// FUNCTION: SHANDALAR 0x00431593
void FUN_00431593(unsigned int param_1, int x, int y)
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
  FUN_00431526(0x20, x, y);

  x += g_neighbor_dx[direction_index];
  y += g_neighbor_dy[direction_index];
  direction_index = ((direction_index + 3) & 7) + 1;

  PutGraphicsPixel(PTR_DAT_00583304, x, y + 0x40,
                   GetGraphicsPixelColorRef(PTR_DAT_00583304, x, y + 0x40) | (1 << (direction_index - 1)));
  FUN_00431526(0x20, x, y);
}

// FUNCTION: SHANDALAR 0x005019ad
void SaveGameToSlot(int save_slot_index)
{
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
  char save_filename[0xd];

  global_saveload_loading = 1;
  FUN_0046ed33();
  save_drive_index = GetSaveDriveIndex();
  if (save_drive_index != -1)
  {
    if (save_slot_index == -1)
    {
      strcpy(g_ui_message_buffer, "\x8cSelect Load File...\n");
      *(int *)(DAT_0077f190 + 0x38) = 0;

      for (slot_index = 0; slot_index < 10; slot_index = slot_index + 1)
      {
        g_save_file_path[7] = (char)FUN_004ece40(slot_index);
        if (ValidateOrLoadSaveGame(g_save_file_path, 1) != 0)
        {
          *(unsigned int *)(DAT_0077f190 + 0x38) = *(unsigned int *)(DAT_0077f190 + 0x38) | (1 << (unsigned char)slot_index);
        }
      }

      FUN_0046ed03();
      g_selected_save_slot_index = RunTextMenuAt(g_ui_message_buffer, 0x30, 0x40);
      FUN_0046ed33();
      if ((*(unsigned int *)(DAT_0077f190 + 0x38) & (1 << (unsigned char)g_selected_save_slot_index)) == 0)
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
  int preview_panel_y_offset;

  if ((param_1 < 0) || (9 < param_1))
  {
    if ((param_1 < 10) || (0xf < param_1))
    {
      preview_panel_y_offset = 0;
    }
    else
    {
      preview_panel_y_offset = param_1 + 0x57;
    }
  }
  else
  {
    preview_panel_y_offset = param_1 + 0x30;
  }

  return preview_panel_y_offset;
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
int FUN_0040dffd(int param_1)
{
  if (param_1 & 2)
    return 1;

  if (param_1 & 4)
    return 2;

  if (param_1 & 8)
    return 3;

  if (param_1 & 0x10)
    return 4;

  if (param_1 & 0x20)
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

  s.entry_index = ReadSpriteEntryPointersWithLimit(g_cstline1_sprite_entries, BuildResolutionSpritePath("cstline1.spr"), 0x54);
  s.entry_index = ReadSpriteEntryPointersWithLimit(g_land_tile_sprite_entries, BuildResolutionSpritePath("landtile.spr"), 0x10);
  s.entry_index = ReadSpriteEntryPointersWithLimit(g_land_sprite_entries, BuildResolutionSpritePath("land.spr"), 55);
  s.entry_index = ReadSpriteEntryPointersWithLimit(g_sland_sprite_entries, BuildResolutionSpritePath("sland.spr"), 55);
  s.entry_index = ReadSpriteEntryPointersWithLimit(&g_land_sprite_entries[55], BuildResolutionSpritePath("land2.spr"), 55);
  s.entry_index = ReadSpriteEntryPointersWithLimit(&g_sland_sprite_entries[55], BuildResolutionSpritePath("sland2.spr"), 55);
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
    *(int *)(g_opening_menu_sprite_work_buffer + s.entry_index * 0xb4) = 0;
  }

  s.entry_index = ReadSpriteEntryPointers((EncodedImage **)((int)g_opening_menu_sprite_work_buffer + 0xb40), BuildResolutionSpritePath((g_player_is_male == 0) ? "ego_f.spr" : "ego_m.spr"));
  s.ego_sprite_header_ptr = *(EncodedImage **)((int)g_opening_menu_sprite_work_buffer + 0xb40);
  g_ego_sprite_width = s.ego_sprite_header_ptr->width;
  g_ego_sprite_height = s.ego_sprite_header_ptr->height;
  g_ego_sprite_draw_height = s.ego_sprite_header_ptr->top_clip;
  if (g_ego_sprite_height < g_ego_sprite_draw_height)
  {
    g_ego_sprite_draw_height = (g_ego_sprite_height * 2) / 3;
  }

  s.entry_index = ReadSpriteEntryPointers((EncodedImage **)((int)g_opening_menu_sprite_work_buffer + 0xbf4), BuildResolutionSpritePath("sego_f.spr"));
  s.sego_sprite_header_ptr = *(EncodedImage **)((int)g_opening_menu_sprite_work_buffer + 0xbf4);
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
int HandleMainMenuButtonControlEvent(void *control_ptr, int event_type)
{
  AdvMenuControl *control;
  int preview_panel_y_offset;
  int avatar_sprite_index;

  control = (AdvMenuControl *)control_ptr;
  if (g_menu_render_guard == 0)
  {
    if ((g_mouse_x < control->x) || (control->x + control->width < g_mouse_x))
    {
      return 0;
    }
    if ((g_mouse_y < control->y) || (control->y + control->height < g_mouse_y))
    {
      return 0;
    }
  }
  if (control->state == 3)
  {
    return 0;
  }
  if (event_type == 2)
  {
    preview_panel_y_offset = ScaleUiCoordinate(4);
    avatar_sprite_index = ScaleUiCoordinate(8);
    BlitGraphicsRect(PTR_DAT_00583354, (unsigned int)control->x, control->y, (unsigned int)control->width, (DWORD)control->height, PTR_DAT_005832dc,
                     control->x, control->y);
    DrawEncodedImageResampled(PTR_DAT_005832dc, control->x + preview_panel_y_offset, control->y + preview_panel_y_offset, control->width - avatar_sprite_index, control->height - avatar_sprite_index,
                              (EncodedImage *)control->mode_data[3]);
    BlitGraphicsRect(PTR_DAT_005832dc, (unsigned int)control->x, control->y, (unsigned int)control->width, (DWORD)control->height, PTR_DAT_005832b4,
                     control->x, control->y);
    if (control->on_activate != (AdvMenuActivateCallback)0)
    {
      control->on_activate(control);
    }
  }
  else
  {
    DrawEncodedImageResampled(g_menu_control_draw_target_page, control->x, control->y, control->width, control->height, (EncodedImage *)control->mode_data[event_type]);
  }
  return 1;
}

// FUNCTION: SHANDALAR 0x004ff652
int HandlePortraitMainMenuControlEvent(void *control_ptr, int event_type)
{
  AdvMenuControl *control;
  EncodedImage *sprite;
  int preview_panel_y_offset;
  int avatar_sprite_index;
  int clamped_required_wins;
  unsigned int local_width;
  DWORD local_height;
  unsigned int local_x;
  int local_y;

  control = (AdvMenuControl *)control_ptr;
  if (event_type == 0)
  {
    sprite = g_face_preview_sprite_selected;
  }
  else
  {
    sprite = g_face_preview_sprite_group[0];
  }
  if (g_menu_render_guard == 0)
  {
    if ((g_mouse_x < control->x) || (control->x + control->width < g_mouse_x))
    {
      return 0;
    }
    if ((g_mouse_y < control->y) || (control->y + control->height < g_mouse_y))
    {
      return 0;
    }
  }
  if (control->state == 3)
  {
    return 0;
  }
  local_height = (DWORD)ScaleUiCoordinate(0x30);
  local_width = (unsigned int)ScaleUiCoordinate((sprite->width * 0x30) / (int)sprite->height);
  preview_panel_y_offset = ScaleUiCoordinate(0x20);
  local_x = (unsigned int)(preview_panel_y_offset - (int)local_width / 2);
  preview_panel_y_offset = ScaleUiCoordinate(0x106);
  local_y = preview_panel_y_offset - (int)local_height / 2;
  if (event_type == 2)
  {
    avatar_sprite_index = ScaleUiCoordinate(4);
    clamped_required_wins = ScaleUiCoordinate(8);
    BlitGraphicsRect(PTR_DAT_00583354, local_x, local_y, local_width, local_height, PTR_DAT_005832dc, local_x, local_y);
    DrawEncodedImageResampled(PTR_DAT_005832dc, avatar_sprite_index + local_x, avatar_sprite_index + local_y, local_width - clamped_required_wins, (int)local_height - clamped_required_wins, sprite);
    BlitGraphicsRect(PTR_DAT_005832dc, local_x, local_y, local_width, local_height, PTR_DAT_005832b4, local_x, local_y);
    if (control->on_activate != (AdvMenuActivateCallback)0)
    {
      control->on_activate(control);
    }
  }
  else
  {
    DrawEncodedImageResampled(g_menu_control_draw_target_page, local_x, local_y, local_width, (int)local_height, sprite);
  }
  return 1;
}

// FUNCTION: SHANDALAR 0x004ff888
int HandleWorldMagicChoiceControlEvent(void *control_ptr, int event_type)
{
  AdvMenuControl *control;
  int avatar_sprite_draw_order[6];
  int avatar_x_positions[6];
  int avatar_draw_y;
  int icon_y_scaled;
  unsigned int icon_x_scaled;
  DWORD icon_height_scaled;
  unsigned int icon_width_scaled;
  EncodedImage *selected_state_sprite;
  int icon_border_padding;
  int world_magic_unlock_score;
  int world_magic_slot_index;
  int preview_panel_y_offset;
  int avatar_sprite_index;

  control = (AdvMenuControl *)control_ptr;
  world_magic_slot_index = control->selection_value * 2 + -0x60;
  if (g_menu_render_guard == 0)
  {
    if ((g_mouse_x < control->x) || (control->x + control->width < g_mouse_x))
    {
      return 0;
    }
    if ((g_mouse_y < control->y) || (control->y + control->height < g_mouse_y))
    {
      return 0;
    }
  }
  if (control->state == 3)
  {
    return 0;
  }
  if (((g_world_magic_bitmap & (1 << ((unsigned char)world_magic_slot_index & 0x1f))) != 0) && (DAT_0078990c[world_magic_slot_index / 2] != 0))
  {
    world_magic_unlock_score = FUN_004bb458(world_magic_slot_index);
    if (event_type == 2)
    {
      selected_state_sprite = (EncodedImage *)control->mode_data[3];
      icon_x_scaled = (unsigned int)ScaleUiCoordinate(g_world_magic_icon_rects[world_magic_slot_index].x);
      icon_y_scaled = ScaleUiCoordinate(g_world_magic_icon_rects[world_magic_slot_index].y);
      icon_width_scaled = (unsigned int)ScaleUiCoordinate(g_world_magic_icon_rects[world_magic_slot_index].width);
      icon_height_scaled = (DWORD)ScaleUiCoordinate(g_world_magic_icon_rects[world_magic_slot_index].height);
      icon_border_padding = ScaleUiCoordinate(4);
      preview_panel_y_offset = ScaleUiCoordinate(0x148);
      BlitGraphicsRect(PTR_DAT_0058332c, icon_x_scaled, icon_y_scaled - preview_panel_y_offset, icon_width_scaled, icon_height_scaled, PTR_DAT_005832dc, icon_x_scaled, icon_y_scaled);
      DrawEncodedImageUiScaled(PTR_DAT_005832dc, g_world_magic_icon_rects[world_magic_slot_index].x, g_world_magic_icon_rects[world_magic_slot_index].y, (EncodedImage *)g_world_magic_choice_button_sprite_bank.named.icon[world_magic_slot_index],
                               g_world_magic_icon_rects[world_magic_slot_index].width, g_world_magic_icon_rects[world_magic_slot_index].height);
      DrawEncodedImageResampled(PTR_DAT_005832dc, icon_border_padding + icon_x_scaled, icon_border_padding + icon_y_scaled, icon_width_scaled - icon_border_padding * 2, (int)icon_height_scaled - icon_border_padding * 2, selected_state_sprite);

      avatar_x_positions[0] = 0x6c;
      avatar_x_positions[1] = 0xbf;
      avatar_x_positions[2] = 0x10e;
      avatar_x_positions[3] = 0x15e;
      avatar_x_positions[4] = 0x1b1;
      avatar_sprite_draw_order[0] = 2;
      avatar_sprite_draw_order[1] = 1;
      avatar_sprite_draw_order[2] = 4;
      avatar_sprite_draw_order[3] = 3;
      avatar_sprite_draw_order[4] = 0;
      avatar_sprite_draw_order[5] = world_magic_slot_index / 2 - 1;
      PTR_DAT_005832b4->font_slot = 4;
      avatar_draw_y = 400 - (int)g_world_magic_avatar_sprites[0]->height / 2;
      avatar_sprite_index = avatar_sprite_draw_order[avatar_sprite_draw_order[5]];
      DrawEncodedImageUiScaled(PTR_DAT_005832dc, avatar_x_positions[avatar_sprite_draw_order[5]] - 0x1e, avatar_draw_y, g_world_magic_avatar_sprites[avatar_sprite_index], (int)g_world_magic_avatar_sprites[0]->width,
                               (int)g_world_magic_avatar_sprites[0]->height);
      BlitGraphicsRect(PTR_DAT_005832dc, icon_x_scaled, icon_y_scaled, icon_width_scaled, icon_height_scaled, PTR_DAT_005832b4, icon_x_scaled, icon_y_scaled);
      if (control->on_activate != (AdvMenuActivateCallback)0)
      {
        control->on_activate(control);
      }
    }
    else
    {
      DrawEncodedImageUiScaled(PTR_DAT_005832b4, g_world_magic_icon_rects[world_magic_slot_index].x, g_world_magic_icon_rects[world_magic_slot_index].y, (EncodedImage *)control->mode_data[event_type],
                               g_world_magic_icon_rects[world_magic_slot_index].width, g_world_magic_icon_rects[world_magic_slot_index].height);
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
void InitializeMainMenuAndWorldMagicChoiceControls(void)
{
  int i;
  int sprite_index;

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
    for (i = 0; i < 4; i = i + 1)
    {
      g_main_menu_controls[i].x = (g_main_menu_controls[i].x * global_screen_width) / 0x280;
      g_main_menu_controls[i].y = (g_main_menu_controls[i].y * global_screen_width) / 0x280;
      g_main_menu_controls[i].width = (g_main_menu_controls[i].width * global_screen_width) / 0x280;
      g_main_menu_controls[i].height = (g_main_menu_controls[i].height * global_screen_width) / 0x280;
    }
    for (i = 0; i < 5; i = i + 1)
    {
      g_world_magic_choice_controls[i].x = (g_world_magic_choice_controls[i].x * global_screen_width) / 0x280;
      g_world_magic_choice_controls[i].y = (g_world_magic_choice_controls[i].y * global_screen_width) / 0x280;
      g_world_magic_choice_controls[i].width = (g_world_magic_choice_controls[i].width * global_screen_width) / 0x280;
      g_world_magic_choice_controls[i].height = (g_world_magic_choice_controls[i].height * global_screen_width) / 0x280;
    }
  }

  AddMenuControlsToContext(g_main_menu_controls, 4, 0);
  AddMenuControlsToContext(g_world_magic_choice_controls, 5, 0);
}

// FUNCTION: SHANDALAR 0x005631ca
void UpdateWorldMagicUnlockProgress(void)
{
  int mapped_world_magic_index;
  int required_duel_wins;
  int clamped_required_wins;
  int world_magic_duel_win_count;
  int scan_index;
  int wizard_color_index;
  int world_magic_progress_values[5];
  int world_magic_victory_counts[5];
  unsigned char world_magic_town_counts[5];
  unsigned char unused_flag_a;
  unsigned char unused_flag_b;
  unsigned char unused_flag_c;
  int town_count_for_wizard_color;

  for (wizard_color_index = 0; wizard_color_index < 5; wizard_color_index = wizard_color_index + 1)
  {
    world_magic_duel_win_count = 0;
    town_count_for_wizard_color = 0;
    mapped_world_magic_index = FUN_0056302b(wizard_color_index + 1);
    for (scan_index = 0; scan_index < 0x80; scan_index = scan_index + 1)
    {
      if ((((unsigned int)g_town_slots[scan_index].status_and_ruling_wizard & 0xff00U) != 0) &&
          (((g_town_slots[scan_index].status_and_ruling_wizard >> 8) - 1) == wizard_color_index))
      {
        town_count_for_wizard_color = town_count_for_wizard_color + 1;
      }
    }
    world_magic_town_counts[mapped_world_magic_index] = (unsigned char)town_count_for_wizard_color;
    if (DAT_0073ea70[wizard_color_index] == 0)
    {
      required_duel_wins = g_shandalar_difficulty * town_count_for_wizard_color + g_shandalar_difficulty * 5 + 0x1e;
      for (scan_index = 0; (scan_index < 1000) && (((unsigned char *)&g_duel_victory_log)[scan_index] != '\0'); scan_index = scan_index + 1)
      {
        if ((((int)(char)((unsigned char *)&g_duel_victory_log)[scan_index]) >> 4) == wizard_color_index + 1)
        {
          world_magic_duel_win_count = world_magic_duel_win_count + 1;
        }
      }
      world_magic_victory_counts[mapped_world_magic_index] = world_magic_duel_win_count;
      clamped_required_wins = g_shandalar_difficulty * 5 + 0x14;
      world_magic_duel_win_count = required_duel_wins - world_magic_duel_win_count;
      if (clamped_required_wins <= world_magic_duel_win_count)
      {
        clamped_required_wins = world_magic_duel_win_count;
      }
      world_magic_progress_values[mapped_world_magic_index] = 0x1e - (required_duel_wins - clamped_required_wins);
    }
    else
    {
      world_magic_progress_values[mapped_world_magic_index] = 0;
    }
  }
  unused_flag_a = 0;
  unused_flag_b = 0;
  unused_flag_c = 0;
  FUN_0052280c(world_magic_progress_values);
}

// FUNCTION: SHANDALAR 0x0056bff1
void RebuildDeckEntriesByCardGroup(void)
{
  int deck_slot_index;
  int card_index;
  unsigned int saved_deck_entries[500];
  int saved_journal_entry_count;

  for (card_index = 0; card_index < 500; card_index = card_index + 1)
  {
    saved_deck_entries[card_index] = deck[card_index];
    deck[card_index] = -1;
  }
  saved_journal_entry_count = g_journal_entry_count;
  for (card_index = 0; card_index < 500; card_index = card_index + 1)
  {
    if (saved_deck_entries[card_index] != 0xffffffff)
    {
      deck_slot_index = FUN_0056bd9d(saved_deck_entries[card_index] & 0xfff);
      deck[deck_slot_index] = deck[deck_slot_index] | (saved_deck_entries[card_index] & 0xfffff000);
    }
  }
  g_journal_entry_count = saved_journal_entry_count;
}

// FUNCTION: SHANDALAR 0x0054cdbd
void RefreshAdventureInterfaceLayout(void)
{
  DAT_00590764 = DAT_006527b0 = -1;
  g_adventure_ui_layout_dirty = 1;
  if (DAT_008bd200 == 0)
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
  } locals;
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

  locals.local_14 = ScaleUiCoordinate(0x140);
  locals.local_18 = ScaleUiCoordinate(0xbc);
  x = x - locals.local_14;
  y = locals.local_18 - y;
  locals.local_c = abs(x);
  locals.local_10 = abs(y);

  if (x >= 0 && y >= 0)
  {
    locals.direction = 0;
  }
  else if (x >= 0 && y < 0)
  {
    locals.direction = 2;
  }
  else if (x < 0 && y < 0)
  {
    locals.direction = 4;
  }
  else if (x < 0 && y >= 0)
  {
    locals.direction = 6;
  }

  if ((locals.direction & 2) == 0 && locals.local_10 < locals.local_c)
  {
    locals.direction++;
  }
  else if ((locals.direction & 2) != 0 && locals.local_c < locals.local_10)
  {
    locals.direction++;
  }

  switch (locals.direction)
  {
  case 0:
  case 3:
  case 4:
  case 7:
    keycode_map[9] = (locals.local_c * 0x9a85) >> 0xe;
    if (y < 0)
    {
      keycode_map[9] = -keycode_map[9];
    }
    break;
  case 1:
  case 2:
  case 5:
  case 6:
    keycode_map[9] = (locals.local_c * 0x1a82) >> 0xe;
    if (y < 0)
    {
      keycode_map[9] = -keycode_map[9];
    }
    break;
  }

  switch (locals.direction)
  {
  case 0:
  case 1:
  case 2:
  case 3:
    if (y < keycode_map[9])
    {
      locals.direction = locals.direction + 1;
    }
    break;
  case 4:
  case 5:
  case 6:
  case 7:
    if (keycode_map[9] < y)
    {
      locals.direction = locals.direction + 1;
    }
    break;
  }

  return keycode_map[locals.direction];
}

// FUNCTION: SHANDALAR 0x0055e808
void FUN_0055e808(void)
{
  struct
  {
    int key_code;
    int key_magic_index;
    int handle_world_magic_hotkey;
    int random_value;
    int random_world_x;
    int random_world_y;
    int nearest_slot_index;
    int nearest_slot_distance;
    int slot_index;
    int move_step_divisor;
    int previous_world_x;
    int previous_world_y;
    int previous_world_x_adjusted;
    int previous_world_y_adjusted;
    int nearest_town_distance;
    int nearest_town_index;
    int town_index;
    int castle_index;
    int ambient_track_id;
    int proximity_value;
    int delta_x;
    int delta_y;
    int abs_delta_x;
    int abs_delta_y;
    int move_offset_x;
    int move_offset_y;
    int dungeon_index;
    int deck_index;
    unsigned int tile_type;
    unsigned int tile_magic_mask;
    int audio_pitch;
    int audio_pan;
  } s;
  shandalar_worldmagic_t *worldmagic_slot_ptr;

  if (IsKeyInputQueueEmpty() == 0)
  {
    s.key_code = PopNormalizedQueuedKeyInput();
    DAT_00669700 = 0;
    s.handle_world_magic_hotkey = 0;

    if ((s.key_code == 0x1b) || (s.key_code == 0x51) || (s.key_code == 0x71))
    {
      PTR_DAT_005832b4->font_slot = 4;
      LoadTextSectionLines("ADVstrings.txt", "SHUTDOWN");
      strcpy(g_ui_message_buffer, text_lines[0]);
      if (RunTextMenuAtScaled(g_ui_message_buffer, 100, 0x50) == 1)
      {
        DAT_009300f0 = 1;
      }
      else
      {
        RefreshAdventureInterfaceLayout();
      }
      SaveGameToSlot(3);
    }
    else
    {
      if ((s.key_code == 0x4c) || (s.key_code == 0x6c))
      {
        g_loadsave_skip_esc = 1;
        s.slot_index = FUN_005031a8();
        if (s.slot_index != -1)
        {
          LoadGameFromSlot(s.slot_index);
        }
        LoadPcxIntoPageNoPalette("advfac64.pic");
        g_world_move_dir_index = 0;
        RefreshAdventureInterfaceLayout();
        FUN_0055060c(1);
        g_loadsave_skip_esc = 0;
      }
      else if ((0x30 < s.key_code) && (s.key_code < 0x36))
      {
        s.handle_world_magic_hotkey = 1;
      }
      else if ((s.key_code == 0x53) || (s.key_code == 0x73))
      {
        g_world_move_dir_index = 0;
        s.slot_index = RunSaveMenuAndSelectSlot();
        if (s.slot_index != -1)
        {
          SaveGameToSlot(s.slot_index);
        }
        LoadPcxIntoPageNoPalette("advfac64.pic");
        RefreshAdventureInterfaceLayout();
      }
      else
      {
        switch (s.key_code)
        {
        case 0x20:
          g_world_move_dir_index = 0;
          break;
        case 0x55:
          g_frontbuffer_direct_blit_enabled = g_frontbuffer_direct_blit_enabled ^ 1;
          s.handle_world_magic_hotkey = 1;
          break;
        case 0x3b00:
          if ((g_world_magic_bitmap & 8U) != 0)
          {
            s.key_code = 0x31;
            s.handle_world_magic_hotkey = 1;
          }
          else
          {
            AnimatePaletteToColor(0, DAT_00589dec);
            DeckBuilderMain(g_main_window_hwnd, 1, 0);
            RestoreAdventureUiPaletteAndFocus();
            RefreshAdventureInterfaceLayout();
          }
          break;
        case 0x3c00:
          ClearInputAndWaitForMouseRelease();
          // map
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
          // dungeon clues
          ShowDungeonCluesScreen(1);
          RefreshAdventureInterfaceLayout();
          break;
        case 0x3f00:
          // stats
          FUN_00549002();
          RefreshAdventureInterfaceLayout();
          break;
        case 0x4000:
          ClearInputAndWaitForMouseRelease();
          ShowStatsWindow(0, -1);
          RefreshAdventureInterfaceLayout();
          break;
        case 0x4700:
          g_world_move_dir_index = 1;
          break;
        case 0x4800:
          g_world_move_dir_index = 2;
          break;
        case 0x4900:
          g_world_move_dir_index = 3;
          break;
        case 0x4b00:
          g_world_move_dir_index = 8;
          break;
        case 0x4d00:
          g_world_move_dir_index = 4;
          break;
        case 0x4f00:
          g_world_move_dir_index = 7;
          break;
        case 0x5000:
          g_world_move_dir_index = 6;
          break;
        case 0x5100:
          g_world_move_dir_index = 5;
          break;
        default:
          break;
        }
      }

      if (s.handle_world_magic_hotkey != 0)
      {
        s.key_magic_index = s.key_code - 0x30;
        if ((DAT_0078990c[s.key_magic_index] != 0) && ((g_world_magic_bitmap & (1 << (((char)s.key_magic_index * 2) & 0x1f))) != 0))
        {
          if (FUN_00522508(4 - g_shandalar_difficulty) == 0)
          {
            DAT_0078990c[s.key_magic_index] = DAT_0078990c[s.key_magic_index] - 1;
          }
          RefreshAdventureInterfaceLayout();

          switch (s.key_code)
          {
          case 0x31:
            AnimatePaletteToColor(0, DAT_00589dec);
            DeckBuilderMain(g_main_window_hwnd, 1, 1);
            RestoreAdventureUiPaletteAndFocus();
            ClearGraphicsPageWithPaletteColor(0, 7);
            RefreshAdventureInterfaceLayout();
            break;
          case 0x32:
            do
            {
              s.random_world_x = FUN_00522508(0x40);
              s.random_world_y = FUN_00522508(0x40);
              s.tile_type = FUN_0043146b(s.random_world_x, s.random_world_y);
            } while (s.tile_type == 0);
            FUN_004290e2(0x12, 2);
            g_world_player_x = s.random_world_x * 0x20 + 0x10;
            g_world_player_y = s.random_world_y * 0x20 + 0x10;
            RefreshAdventureInterfaceLayout();
            g_world_scene_reveal_effect_pending = 1;
            break;
          case 0x33:
            worldmagic_slot_ptr = &Scards[s.key_magic_index];
            worldmagic_slot_ptr->worldmagic_duration = 0x96;
            FUN_004290e2(0x12, 3);
            break;
          case 0x34:
            s.nearest_slot_distance = 0x7fff;
            s.nearest_slot_index = -1;
            for (s.slot_index = 0; s.slot_index < 6; s.slot_index = s.slot_index + 1)
            {
              if (0 < g_lair_or_monster_slots[s.slot_index].entry_type)
              {
                s.random_value = FUN_004ecf30(g_world_player_x - g_lair_or_monster_slots[s.slot_index].world_x,
                                              g_world_player_y - g_lair_or_monster_slots[s.slot_index].world_y);
                if (s.random_value < s.nearest_slot_distance)
                {
                  s.nearest_slot_index = s.slot_index;
                  s.nearest_slot_distance = s.random_value;
                }
              }
            }
            if (s.nearest_slot_index != -1)
            {
              FreeOpeningMenuSpriteWorkEntries(s.nearest_slot_index, s.nearest_slot_index + 8);
              FUN_004290e2(0x12, (g_lair_or_monster_slots[s.nearest_slot_index].entry_type << 8) | 4);
              g_lair_or_monster_slots[s.nearest_slot_index].entry_type = -1;
            }
            break;
          case 0x35:
            if (g_lair_or_monster_slots[7].entry_type != -1)
            {
              g_world_player_x = (g_lair_or_monster_slots[7].world_x & 0xffe0U) + 0x10;
              g_world_player_y = (g_lair_or_monster_slots[7].world_y & 0xffe0U) + 0x1f;
              FUN_004290e2(0x12, 5);
            }
            g_world_scene_reveal_effect_pending = 1;
            break;
          }
        }
      }
    }

    EnsureAdvfac64Loaded(0);
  }

  if ((DAT_006696fc != 0) && (++DAT_00669700 > 500))
  {
    // Some sort of demo? Looks unreachable since DAT_006696fc is never set
    FUN_00559807();
    DAT_00669700 = 300;
  }

  s.delta_x = g_neighbor_dx[g_world_move_dir_index] + g_world_player_x;
  s.delta_y = g_neighbor_dy[g_world_move_dir_index] + g_world_player_y;
  s.tile_type = FUN_0043146b((s.delta_x + ((s.delta_x >> 0x1f) & 0x1f)) >> 5, (s.delta_y + ((s.delta_y >> 0x1f) & 0x1f)) >> 5);
  s.tile_magic_mask = FUN_005611c8(s.tile_type);
  if (s.tile_magic_mask == 0)
  {
    s.key_magic_index = 0;
  }
  else
  {
    do
    {
      s.key_magic_index = FUN_00522508(5) + 1;
    } while ((s.tile_magic_mask & (1 << ((unsigned char)s.key_magic_index & 0x1f))) == 0);
  }

  s.move_step_divisor = 1;
  g_world_player_tile_x = (g_world_player_x + ((g_world_player_x >> 0x1f) & 0x1f)) >> 5;
  g_world_player_tile_y = (g_world_player_y + ((g_world_player_y >> 0x1f) & 0x1f)) >> 5;
  if ((s.tile_type == 2) || ((s.tile_type == 3 && ((g_world_magic_bitmap & 8U) == 0)) || (s.tile_type == 4 && ((g_world_magic_bitmap & 0x200U) == 0))))
  {
    s.move_step_divisor = 3;
  }
  if ((s.tile_type == 5) && ((g_world_magic_bitmap & 0x200U) == 0))
  {
    s.move_step_divisor = 3;
  }
  if (WorldRoadTileHasDirection(g_world_player_tile_x, g_world_player_tile_y, (char)g_world_move_dir_index) != 0)
  {
    s.move_step_divisor = 1;
  }
  if (WorldRoadTileHasDirection(g_world_player_tile_x, g_world_player_tile_y, ((char)g_world_move_dir_index + 3U & 7) + 1) != 0)
  {
    s.move_step_divisor = 1;
  }
  if (g_food == 0)
  {
    s.move_step_divisor = ClampIntToRange(s.move_step_divisor + 2, 0, 4);
  }

  s.previous_world_y = g_world_player_y;
  s.previous_world_x = g_world_player_x;
  s.previous_world_x_adjusted = g_world_player_x + ((g_world_player_x >> 0x1f) & 0x1f);
  s.previous_world_y_adjusted = g_world_player_y + ((g_world_player_y >> 0x1f) & 0x1f);
  if (g_monster_timer % s.move_step_divisor == 0)
  {
    if (s.move_step_divisor == 3)
    {
      s.delta_x = g_neighbor_dx[g_world_move_dir_index] * 2;
    }
    else
    {
      s.delta_x = g_neighbor_dx[g_world_move_dir_index];
    }
    g_world_player_x = g_world_player_x + s.delta_x;

    if (s.move_step_divisor == 3)
    {
      s.delta_y = g_neighbor_dy[g_world_move_dir_index] * 2;
    }
    else
    {
      s.delta_y = g_neighbor_dy[g_world_move_dir_index];
    }
    g_world_player_y = g_world_player_y + s.delta_y;

    DAT_0073ea70[7] = DAT_0073ea70[7] + 1;
    if (4 < DAT_0073ea70[7])
    {
      DAT_0073ea70[7] = 1;
    }

    if (g_world_move_dir_index != 0)
    {
      s.audio_pitch = FUN_00522508(0x28) + 0x50;
      s.audio_pan = FUN_00522508(0x19) + 0x4b;
      FUN_00562736(((DAT_0073ea70[7] & 1U) - 2) + s.key_magic_index * 2, s.audio_pan, s.audio_pitch, 0);
    }

    if (g_world_move_dir_index == 0)
    {
      DAT_0073ea70[7] = 0;
    }
    else
    {
      DAT_0073ea70[5] = g_world_move_dir_index;
    }

    if ((Scards[WORLDMAGIC_QUICKENING].worldmagic_duration != 0) ||
        (((g_monster_timer & 1U) != 0 &&
          (WorldRoadTileHasDirection(g_world_player_tile_x, g_world_player_tile_y, ((char)g_world_move_dir_index + 3U & 7) + 1) != 0))))
    {
      g_world_player_x = g_world_player_x + g_neighbor_dx[g_world_move_dir_index];
      g_world_player_y = g_world_player_y + g_neighbor_dy[g_world_move_dir_index];
    }

    s.tile_type = FUN_0043146b((g_world_player_x + ((g_world_player_x >> 0x1f) & 0x1f)) >> 5, (g_world_player_y + ((g_world_player_y >> 0x1f) & 0x1f)) >> 5);
    if ((s.tile_type == 0) &&
        ((abs(g_world_player_x - ((g_world_player_x & 0xffffffe0U) + 0x10)) < 0xc) || (abs(g_world_player_y - ((g_world_player_y & 0xffffffe0U) + 0x10)) < 0xc)))
    {
      g_world_move_dir_index = 0;
      g_world_player_x = s.previous_world_x;
      g_world_player_y = s.previous_world_y;
    }

    if ((FUN_00522508(0x28) == 0) && (g_skip_world_sfx_preload == 0))
    {
      FUN_00562a29(s.key_magic_index);
    }

    if (((g_monster_timer & 0x1fU) == 0) && (g_world_move_dir_index != 0))
    {
      if (g_food != 0)
      {
        g_food = g_food - 1;
      }
      if ((Scards[WORLDMAGIC_FRUIT_OF_SUSTENANCE].worldmagic_city == 0) && (s.tile_type == 2))
      {
        g_food = g_food + 2;
      }
      g_siege_timer = g_siege_timer + 1;
      g_quest_restock_timer = g_quest_restock_timer + 1;

      if ((g_siege_timer & 0x3fU) == 0)
      {
        FUN_005616f9();
        g_siege_timer = g_siege_timer + ClampIntToRange(g_shandalar_difficulty + ((g_siege_timer + ((g_siege_timer >> 0x1f) & 0xffU)) >> 8), 0, 0x10);
      }
      if (((unsigned char)g_siege_timer & 0x3f) == 0x18)
      {
        FUN_00562169();
        g_siege_timer = g_siege_timer + ClampIntToRange(g_shandalar_difficulty * 2 + ((g_siege_timer + ((g_siege_timer >> 0x1f) & 0x3fU)) >> 6), 0, 0x20);
      }

      DAT_00669710 = 1;
      if (g_siege_timer > 7)
      {
        DAT_00591214 = 1;
      }
    }

    g_world_player_tile_x = (g_world_player_x + ((g_world_player_x >> 0x1f) & 0x1f)) >> 5;
    g_world_player_tile_y = (g_world_player_y + ((g_world_player_y >> 0x1f) & 0x1f)) >> 5;
    if (((s.previous_world_x_adjusted >> 5) != g_world_player_tile_x) || ((s.previous_world_y_adjusted >> 5) != g_world_player_tile_y))
    {
      DAT_006696f4 = 0;
    }

    if ((g_monster_timer & 1U) == 0)
    {
      s.nearest_town_distance = 0x7fff;
      s.nearest_town_index = 0;
      for (s.town_index = 0; s.town_index < 0x80; s.town_index = s.town_index + 1)
      {
        if (g_town_slots[s.town_index].location_type != -1)
        {
          s.random_value = FUN_004ecf30(g_town_slots[s.town_index].world_x * 0x20 + 0x10 - g_world_player_x,
                                        g_town_slots[s.town_index].world_y * 0x20 + 0x10 - g_world_player_y);
          if (s.random_value < s.nearest_town_distance)
          {
            s.nearest_town_index = s.town_index;
            s.nearest_town_distance = s.random_value;
          }
        }
      }

      s.proximity_value = ClampIntToRange(0x80 - s.nearest_town_distance, 0, 100);
      if ((s.proximity_value < 0xb) || (g_town_slots[s.nearest_town_index].location_type < 1))
      {
        if (DAT_0059121c != 0)
        {
          sound_stop(0x10);
        }
        DAT_0059121c = 0;
        DAT_0059126c = -1;
      }
      else
      {
        if (DAT_0059126c == s.nearest_town_index)
        {
          sound_set_vol(0x10, s.proximity_value << 2);
        }
        else
        {
          DAT_0059126c = s.nearest_town_index;
          if (g_town_slots[s.nearest_town_index].location_type == 4)
          {
            for (s.castle_index = 0; (s.castle_index < 5) &&
                                     ((g_town_slots[s.nearest_town_index].world_x != g_castle_dungeon_slots[s.castle_index].world_x) ||
                                      (g_town_slots[s.nearest_town_index].world_y != g_castle_dungeon_slots[s.castle_index].world_y));
                 s.castle_index = s.castle_index + 1)
            {
            }

            s.ambient_track_id = DAT_00591220;
            if (s.castle_index + 0x15 != DAT_00591220)
            {
              if ((DAT_00591220 != -1) && (s.castle_index + 0x15 != DAT_00591220))
              {
                sound_unload(0x10);
              }

              switch (s.castle_index)
              {
              case 0:
                FUN_00562835("sound\\bcastle.wav", 0x10);
                break;
              case 1:
                FUN_00562835("sound\\ucastle.wav", 0x10);
                break;
              case 2:
                FUN_00562835("sound\\gcastle.wav", 0x10);
                break;
              case 3:
                FUN_00562835("sound\\rcastle.wav", 0x10);
                break;
              case 4:
                FUN_00562835("sound\\wcastle.wav", 0x10);
                break;
              default:
                break;
              }
              s.ambient_track_id = s.castle_index + 0x15;
            }
          }
          else if (g_town_slots[s.nearest_town_index].location_type == 1)
          {
            if ((DAT_00591220 != -1) && (DAT_00591220 != 0x32))
            {
              sound_unload(0x10);
            }
            if (DAT_00591220 != 0x32)
            {
              FUN_00562835("sound\\locmus0.wav", 0x10);
            }
            DAT_00591220 = 0x32;
            s.ambient_track_id = DAT_00591220;
          }
          else
          {
            s.ambient_track_id = s.nearest_town_index % 0x14;
            if (DAT_00591220 != s.ambient_track_id)
            {
              if (DAT_00591220 != -1)
              {
                sound_unload(0x10);
              }
              switch (s.ambient_track_id)
              {
              case 0:
                FUN_00562835("sound\\locmus1.wav", 0x10);
                break;
              case 1:
                FUN_00562835("sound\\locmus2.wav", 0x10);
                break;
              case 2:
                FUN_00562835("sound\\locmus3.wav", 0x10);
                break;
              case 3:
                FUN_00562835("sound\\locmus4.wav", 0x10);
                break;
              case 4:
                FUN_00562835("sound\\locmus5.wav", 0x10);
                break;
              case 5:
                FUN_00562835("sound\\locmus6.wav", 0x10);
                break;
              case 6:
                FUN_00562835("sound\\locmus7.wav", 0x10);
                break;
              case 7:
                FUN_00562835("sound\\locmus8.wav", 0x10);
                break;
              case 8:
                FUN_00562835("sound\\locmus9.wav", 0x10);
                break;
              case 9:
                FUN_00562835("sound\\locmus10.wav", 0x10);
                break;
              case 10:
                FUN_00562835("sound\\locmus11.wav", 0x10);
                break;
              case 0xb:
                FUN_00562835("sound\\locmus12.wav", 0x10);
                break;
              case 0xc:
                FUN_00562835("sound\\locmus13.wav", 0x10);
                break;
              case 0xd:
                FUN_00562835("sound\\locmus14.wav", 0x10);
                break;
              case 0xe:
                FUN_00562835("sound\\locmus15.wav", 0x10);
                break;
              case 0xf:
                FUN_00562835("sound\\locmus16.wav", 0x10);
                break;
              case 0x10:
                FUN_00562835("sound\\locmus17.wav", 0x10);
                break;
              case 0x11:
                FUN_00562835("sound\\locmus18.wav", 0x10);
                break;
              case 0x12:
                FUN_00562835("sound\\locmus19.wav", 0x10);
                break;
              case 0x13:
                FUN_00562835("sound\\tmplmus1.wav", 0x10);
                break;
              default:
                FUN_00562835("sound\\locmus0.wav", 0x10);
                break;
              }
            }
          }
          DAT_00591220 = s.ambient_track_id;
          FUN_0056279e(0x10, s.proximity_value, 0);
          set_sound_loop(0x10, 1);
        }
        DAT_0059121c = 1;
      }
    }

    if ((DAT_006696f4 == 0) && (abs((g_world_player_x & 0x1fU) - 0x10) < 0xc) &&
        (abs((g_world_player_y & 0x1fU) - 0x10) < 0xc) && ((FUN_004314ca(g_world_player_tile_x, g_world_player_tile_y) & 0x10) != 0))
    {
      s.dungeon_index = FUN_004bb040(g_world_player_tile_x, g_world_player_tile_y);
      if (s.dungeon_index == -1)
      {
        FUN_00431593(0x10, g_world_player_tile_x, g_world_player_tile_y);
      }
      else
      {
        sound_set_vol(0x10, 400);
        play_snd_marker(0x10, 1);
        VisitTownSlot(s.dungeon_index);
        DAT_00591214 = 1;
        DAT_006696f4 = 1;
        g_world_move_dir_index = 0;
        for (s.slot_index = 0; s.slot_index < 6; s.slot_index = s.slot_index + 1)
        {
          if (0 < g_lair_or_monster_slots[s.slot_index].entry_type)
          {
            if (FUN_004ecf30(g_world_player_x - g_lair_or_monster_slots[s.slot_index].world_x,
                             g_world_player_y - g_lair_or_monster_slots[s.slot_index].world_y) < 0x60)
            {
              s.delta_x = g_world_player_x - g_lair_or_monster_slots[s.slot_index].world_x;
              s.delta_y = g_world_player_y - g_lair_or_monster_slots[s.slot_index].world_y;
              s.abs_delta_y = abs(s.delta_y);
              s.abs_delta_x = abs(s.delta_x);
              if (s.abs_delta_y / 2 < s.abs_delta_x)
              {
                s.move_offset_x = SignNonZero(s.delta_x) * 0x30;
              }
              else
              {
                s.move_offset_x = 0;
              }
              g_lair_or_monster_slots[s.slot_index].world_x = g_world_player_x - s.move_offset_x;

              s.abs_delta_x = abs(s.delta_x);
              s.abs_delta_y = abs(s.delta_y);
              if (s.abs_delta_x / 2 < s.abs_delta_y)
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
    }

    if ((DAT_006696f4 == 0) && ((g_world_player_x - 8U & 0x10) == 0) && ((g_world_player_y - 8U & 0x10) == 0) &&
        ((FUN_004314ca(g_world_player_tile_x, g_world_player_tile_y) & 0x40) != 0))
    {
      s.dungeon_index = FUN_00508b89(g_world_player_tile_x, g_world_player_tile_y);
      DAT_006696f4 = 1;
      if ((s.dungeon_index != -1) && (g_castle_dungeon_slots[s.dungeon_index].card_slot_1 != -1) && (g_castle_dungeon_slots[s.dungeon_index].clues_bitmap != 0))
      {
        FUN_0050a5c1(s.dungeon_index);
      }
    }

    DAT_00789938 = 0;
    DAT_0078df68 = 0;
    for (s.deck_index = 0; s.deck_index < 500; s.deck_index = s.deck_index + 1)
    {
      if (deck[s.deck_index] != -1)
      {
        DAT_00789938 = DAT_00789938 + 1;
        if ((((unsigned char *)&deck[s.deck_index])[1] & 0x40) == 0)
        {
          DAT_0078df68 = DAT_0078df68 + 1;
        }
      }
    }

    ClearQueuedKeyInput();
  }
}

// FUNCTION: SHANDALAR 0x0055fd27
void FUN_0055fd27(void)
{
  // TODO(decomp): Overworld lair/monster tick. Spawns/moves entries in g_lair_or_monster_slots, triggers encounters,
  // and handles the special siege slot (index 7).
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
unsigned int WorldRoadTileHasDirection(int tile_x, int tile_y, char direction_index)
{
  unsigned int road_mask;

  if ((tile_x < 0x40) && (-1 < tile_x))
  {
    if ((tile_y < 0x40) && (-1 < tile_y))
    {
      road_mask = GetGraphicsPixelColorRef(PTR_DAT_00583304, tile_x, tile_y + 0x40);
      road_mask = road_mask & (1U << ((unsigned char)(direction_index - 1U) & 0x1f));
    }
    else
    {
      road_mask = 0;
    }
  }
  else
  {
    road_mask = 0;
  }
  return road_mask;
}

// FUNCTION: SHANDALAR 0x004bdaad
int FreeOpeningMenuSpriteWorkEntries(int work_entry_index_a, int work_entry_index_b)
{
  if (*(int *)(g_opening_menu_sprite_work_buffer + work_entry_index_a * 0xb4) == 0)
  {
    return 0;
  }

  FreeSpriteBlob(*(void **)(g_opening_menu_sprite_work_buffer + work_entry_index_a * 0xb4));
  FreeSpriteBlob(*(void **)(g_opening_menu_sprite_work_buffer + work_entry_index_b * 0xb4));
  *(int *)&g_opening_menu_sprite_work_buffer[work_entry_index_a * 0xb4] = 0;
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
void FUN_00559807(void)
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

// FUNCTION: SHANDALAR 0x005616f9
// TODO(decomp): Starts a siege event (chooses a target town, populates g_lair_or_monster_slots[7], shows newsflash).
void FUN_005616f9(void) {}

// FUNCTION: SHANDALAR 0x00562169
// TODO(decomp): Resolves/cleans up an active siege and may trigger a "quest failed / game over" path.
void FUN_00562169(void) {}

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
void FUN_00562a29(int param_1)
{
  (void)param_1;
  // TODO(decomp): Plays random ambient overworld SFX for a given terrain/magic index (random pan/pitch).
}

// FUNCTION: SHANDALAR 0x0050a5c1
void FUN_0050a5c1(int param_1)
{
  (void)param_1;
  // TODO(decomp): Enters a dungeon/castle by index (used when stepping onto a dungeon tile / dungeon clues flow).
}

// FUNCTION: SHANDALAR 0x00549002
void FUN_00549002(void)
{
  // TODO(decomp): Adventure "Stats" screen UI (renders stats, world magic list, handles button/menu loop).
}

// FUNCTION: SHANDALAR 0x0056335f
void ShowStatsWindow(int mode, int highlight)
{
  (void)mode;
  (void)highlight;
  // TODO(decomp): Prepares data and calls into the STATWIN DLL to show a stats/progress window; param selects mode/highlight.
}

// FUNCTION: SHANDALAR 0x00533ccd
void AnalyzeDeckAndMaybeShowReport(int show_ui)
{
  (void)show_ui;
  // TODO(decomp): Deck analyzer. Computes deck composition stats and (when show_ui!=0) draws the ANALYZE report UI.
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
  int i;
  int queued_key;
  int full_key;

  (void)mouse_x;
  (void)mouse_y;

  g_menu_input_unhandled = 1;
  if (HasQueuedKeyInput() != 0)
  {
    queued_key = PeekQueuedKeyInput();
    if ((queued_key == 0xf09) || (queued_key == 0xf0f) ||
        ((g_menu_allow_arrow_nav_by_context[g_menu_context_index] != 0) && ((queued_key == 0x4800) || (queued_key == 0x5000))))
    {
      int direction;

      PopQueuedKeyInput();
      full_key = (int)queued_key;
      switch (full_key)
      {
      case 0xf0f:
      case 0x4800:
        direction = -1;
        break;
      case 0xf09:
      case 0x5000:
        direction = 1;
        break;
      default:
        break;
      }

      if (g_menu_current_control_index == -1)
      {
        if (direction <= 0)
        {
          g_menu_current_control_index = g_menu_control_count_by_context[g_menu_context_index] - 1;
        }
        else
        {
          g_menu_current_control_index = 0;
        }
      }
      else
      {
        g_menu_current_control_index = (g_menu_control_count_by_context[g_menu_context_index] + g_menu_current_control_index + direction) % g_menu_control_count_by_context[g_menu_context_index];
      }

      while (g_menu_controls_by_context[g_menu_context_index][g_menu_current_control_index]->state == 3)
      {
        g_menu_current_control_index = (g_menu_control_count_by_context[g_menu_context_index] + g_menu_current_control_index + direction) % g_menu_control_count_by_context[g_menu_context_index];
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

    full_key = (int)queued_key;
    queued_key &= 0xff;
    if ((queued_key != 0) || (g_menu_allow_arrow_nav_by_context[g_menu_context_index] == 0))
    {
      int next_search_index;
      int control_index;

      i = 0;
      if (g_menu_current_control_index == -1)
      {
        next_search_index = 0;
      }
      else
      {
        next_search_index = g_menu_current_control_index + 1;
      }

      for (; i < g_menu_control_count_by_context[g_menu_context_index]; i = i + 1)
      {
        control_index = (next_search_index + i) % g_menu_control_count_by_context[g_menu_context_index];
        if ((g_menu_controls_by_context[g_menu_context_index][control_index]->state != 3) &&
            ((g_menu_controls_by_context[g_menu_context_index][control_index]->direct_hotkey == full_key) ||
             ((queued_key != 0) && (g_menu_controls_by_context[g_menu_context_index][control_index]->navigate_hotkeys != (char *)0) &&
              (strchr(g_menu_controls_by_context[g_menu_context_index][control_index]->navigate_hotkeys, queued_key) != (char *)0))))
        {
          g_menu_current_control_index = control_index;
          if (g_menu_controls_by_context[g_menu_context_index][control_index]->x != -1)
          {
            SetCursorPos(
                g_menu_controls_by_context[g_menu_context_index][control_index]->x + g_menu_controls_by_context[g_menu_context_index][control_index]->width / 2,
                g_menu_controls_by_context[g_menu_context_index][control_index]->y + g_menu_controls_by_context[g_menu_context_index][control_index]->height / 2);
          }

          if (g_menu_prev_control_index != -1)
          {
            g_menu_controls_by_context[g_menu_context_index][g_menu_prev_control_index]->on_render(g_menu_controls_by_context[g_menu_context_index][g_menu_prev_control_index], 0);
          }

          g_menu_controls_by_context[g_menu_context_index][g_menu_current_control_index]->on_render(g_menu_controls_by_context[g_menu_context_index][g_menu_current_control_index], 1);
          if ((g_menu_controls_by_context[g_menu_context_index][control_index]->direct_hotkey == full_key) ||
              ((queued_key != 0) && (g_menu_controls_by_context[g_menu_context_index][control_index]->activate_hotkeys != (char *)0) &&
               (strchr(g_menu_controls_by_context[g_menu_context_index][control_index]->activate_hotkeys, queued_key) != (char *)0)))
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

      if ((queued_key == 0xd) && (g_menu_current_control_index != -1))
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
      g_menu_prev_control_index = -1;
      g_menu_current_control_index = -1;
      g_menu_render_guard = 0;
    }
  }

  for (i = 0; i < g_menu_control_count_by_context[g_menu_context_index]; i = i + 1)
  {
    if (g_menu_prev_control_index != i)
    {
      if (g_menu_controls_by_context[g_menu_context_index][i]->on_render(g_menu_controls_by_context[g_menu_context_index][i], 0) != 0)
      {
        g_menu_current_control_index = i;
      }
    }
  }

  if (g_menu_prev_control_index != g_menu_current_control_index)
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

  if (DAT_008bd200 != 0)
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
  if (DAT_008bd200 == 1)
  {
    return 0;
  }

  return (unk_00742fc4 != 0) ? 0 : WaitForInputEvent();
}

// FUNCTION: SHANDALAR 0x004ecf30
int FUN_004ecf30(int param_1, int param_2)
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
    int local_c;
    char *local_8;
    int iVar36;
  } s;

  s.ok = 1;
  s.ok &= LoadTextSectionStringTable(filename, "PLAYERNAMES", gs_playernames_0077c5a0, 0xe, DAT_0077e2c0, DAT_0077e2c0 + 0x2bc, (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "DIFFICULTYLEVELS", gs_difficultylevels_0077d130, 4, DAT_0074d790, DAT_0074d790 + 0x64, (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "DUNGEON_NAMES", gs_dungeon_names_00780820, 0x11, DAT_0074c970, DAT_0074c970 + 0x352, (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "LAIR_NAMES", gs_lair_names_0077c020, 0x13, DAT_0074bd30, DAT_0074bd30 + 0x3b6, (char **)0);

  LoadTextSectionLines(filename, "CITYNAMES_FORMAT");
  strcpy(DAT_0074c950, text_lines[0]);
  strcpy(DAT_0077f190, text_lines[1]);

  LoadTextSectionLines(filename, "CITYNAME_VILLAGE");
  strcpy(DAT_0077cfd0, text_lines[0]);

  LoadTextSectionLines(filename, "CITYNAME_CASTLE");
  strcpy(DAT_00765dc0, text_lines[0]);

  LoadTextSectionLines(filename, "CITYNAME_MANACASTLE");

  for (s.local_c = 0; s.local_c < 5; s.local_c = s.local_c + 1)
  {
    s.iVar36 = s.local_c * 4 + 4;
    s.iVar36 = s.iVar36 + s.iVar36 * 4;
    s.iVar36 = s.iVar36 + s.iVar36 * 4;
    strcpy(DAT_0077de00 + s.iVar36, text_lines[s.local_c]);
  }
  strcpy(DAT_0077de00, DAT_00765dc0);

  s.local_8 = DAT_0077d610;
  s.ok &= LoadTextSectionStringTable(filename, "CITYNAMES_FIRSTHALF", gs_citynames_firsthalf_0077e060, 0x10, DAT_0077d610,
                                     DAT_0077d610 + sizeof(gs_city_text_cluster_0077d610.citynames_buf_0077d610), &s.local_8);
  s.ok &= LoadTextSectionStringTable(filename, "CITYNAMES_SECONDHALF", gs_citynames_secondhalf_007653e0, 0x10, s.local_8,
                                     DAT_0077d610 + sizeof(gs_city_text_cluster_0077d610.citynames_buf_0077d610), (char **)0);

  s.local_8 = DAT_0074b160;
  LoadTextSectionLines(filename, "WORLDMAGIC");
  strcpy(DAT_0077e1d0, text_lines[0]);
  s.ok &= LoadTextSectionStringTable(filename, "WORLDMAGIC_NAMES", gs_worldmagic_names_00780660, 0xc, DAT_0074b160,
                                     DAT_0074b160 + sizeof(gs_worldmagic_buf_0074b160), &s.local_8);
  s.ok &= LoadTextSectionStringTable(filename, "WORLDMAGIC_EXPLAINS", gs_worldmagic_explains_0074b8f0, 0xc, s.local_8,
                                     DAT_0074b160 + sizeof(gs_worldmagic_buf_0074b160), (char **)0);

  s.ok &= LoadTextSectionStringTable(filename, "LOGSTRINGS", gs_logstrings_0077c9a0, 0x10, DAT_0077c680,
                                     DAT_0077c680 + sizeof(gs_logstrings_buf_0077c680), (char **)0);

  LoadTextSectionLines(filename, "WIZARDNAMES");
  for (s.local_c = 0; s.local_c < 5; s.local_c = s.local_c + 1)
  {
    strcpy(DAT_0077ee70 + (s.local_c * 5 + 5) * 10, text_lines[s.local_c]);
  }
  strcpy(DAT_0077ee70, "");

  s.iVar36 = LoadTextSectionLines(filename, "CREATURENAMES");
  if ((int)DAT_00593934 <= s.iVar36)
  {
    s.iVar36 = DAT_00593934;
  }
  for (s.local_c = 0; s.local_c < s.iVar36; s.local_c = s.local_c + 1)
  {
    strcpy(DAT_00591a12 + s.local_c * 0x8c, text_lines[s.local_c]);
  }
  while (s.local_c = s.iVar36, s.local_c < (int)DAT_00593934)
  {
    strcpy(DAT_00591a12 + s.local_c * 0x8c, "");
    s.iVar36 = s.local_c + 1;
  }

  s.iVar36 = LoadTextSectionLines(filename, "CREATURENAME_ARTICLES");
  if ((int)DAT_00593934 <= s.iVar36)
  {
    s.iVar36 = DAT_00593934;
  }
  for (s.local_c = 0; s.local_c < s.iVar36; s.local_c = s.local_c + 1)
  {
    strcpy(DAT_00591a08 + s.local_c * 0x8c, text_lines[s.local_c]);
  }
  while (s.local_c = s.iVar36, s.local_c < (int)DAT_00593934)
  {
    strcpy(DAT_00591a08 + s.local_c * 0x8c, "");
    s.iVar36 = s.local_c + 1;
  }

  s.iVar36 = LoadTextSectionLines(filename, "CREATURENAMES_PLURAL");
  if ((int)DAT_00593934 <= s.iVar36)
  {
    s.iVar36 = DAT_00593934;
  }
  for (s.local_c = 0; s.local_c < s.iVar36; s.local_c = s.local_c + 1)
  {
    strcpy(DAT_00591a44 + s.local_c * 0x8c, text_lines[s.local_c]);
  }
  while (s.local_c = s.iVar36, s.local_c < (int)DAT_00593934)
  {
    strcpy(DAT_00591a44 + s.local_c * 0x8c, "");
    s.iVar36 = s.local_c + 1;
  }

  LoadTextSectionLines(filename, "DIRECTIONS");
  for (s.local_c = 0; s.local_c < 4; s.local_c = s.local_c + 1)
  {
    strcpy(DAT_00765d50 + s.local_c * 0x19, text_lines[s.local_c]);
  }

  s.local_8 = DAT_0074c5c0;
  s.ok &= LoadTextSectionStringTable(filename, "CARDCLASSNAMES", gs_cardclassnames_0077cf70, 9, DAT_0074c5c0,
                                     DAT_0074c5c0 + sizeof(gs_cardclassnames_buf_0074c5c0), &s.local_8);
  s.ok &= LoadTextSectionStringTable(filename, "CARDCLASSNAMES_PLURAL", gs_cardclassnames_plural_0077e1f0, 9, s.local_8,
                                     DAT_0074c5c0 + sizeof(gs_cardclassnames_buf_0074c5c0), (char **)0);

  LoadTextSectionLines(filename, "SPELLNAMES");
  strcpy(DAT_0077e6e0, text_lines[0]);
  for (s.local_c = 1; s.local_c < 5; s.local_c = s.local_c + 1)
  {
    strcpy(DAT_0077e220 + s.local_c * 0x19, text_lines[s.local_c]);
  }
  strcpy(DAT_0077e220, text_lines[6]);

  LoadTextSectionLines(filename, "MANANAMES");
  for (s.local_c = 0; s.local_c < 5; s.local_c = s.local_c + 1)
  {
    strcpy(DAT_0074d980 + (s.local_c * 5 + 5) * 5, text_lines[s.local_c]);
  }
  strcpy(DAT_0074d980, "");

  LoadTextSectionLines(filename, "LANDWALKS");
  for (s.local_c = 0; s.local_c < 5; s.local_c = s.local_c + 1)
  {
    strcpy(DAT_007806f0 + (s.local_c * 5 + 5) * 10, text_lines[s.local_c]);
  }
  strcpy(DAT_007806f0, "");

  LoadTextSectionLines(filename, "AMULETNAMES");
  for (s.local_c = 0; s.local_c < 6; s.local_c = s.local_c + 1)
  {
    strcpy(DAT_0077d090 + s.local_c * 0x19, text_lines[s.local_c]);
  }

  LoadTextSectionLines(filename, "AMULETNAMES_PLURAL");
  for (s.local_c = 0; s.local_c < 6; s.local_c = s.local_c + 1)
  {
    strcpy(DAT_0077edd0 + s.local_c * 0x19, text_lines[s.local_c]);
  }

  LoadTextSectionLines(filename, "COLORCARDS");
  for (s.local_c = 0; s.local_c < 6; s.local_c = s.local_c + 1)
  {
    strcpy(DAT_0077c5e0 + s.local_c * 0x19, text_lines[s.local_c]);
  }

  s.ok &= LoadTextSectionStringTable(filename, "CAVE_SHOWCLUES", gs_cave_showclues_0077efa0, 0x17, DAT_007658d0,
                                     DAT_007658d0 + sizeof(gs_cave_showclues_buf_007658d0), (char **)0);

  s.local_8 = DAT_0074da70;
  s.ok &= LoadTextSectionStringTable(filename, "ENCOUNTER_PREDUEL", gs_encounter_preduel_0077f0d0, 0x30, DAT_0074da70,
                                     DAT_0074da70 + sizeof(gs_encounter_buf_0074da70), &s.local_8);
  s.ok &= LoadTextSectionStringTable(filename, "ENCOUNTER_POSTDUEL", gs_encounter_postduel_0077f050, 0x20, s.local_8,
                                     DAT_0074da70 + sizeof(gs_encounter_buf_0074da70), (char **)0);

  s.local_8 = DAT_0077f610;
  s.ok &= LoadTextSectionStringTable(filename, "VISIT", gs_visit_0077c4f0, 0x23, DAT_0077f610, DAT_0077f610 + sizeof(gs_visit_buf_0077f610),
                                     &s.local_8);
  s.ok &= LoadTextSectionStringTable(filename, "VISIT_CITYBUY", gs_visit_citybuy_0077f1d0, 4, s.local_8,
                                     DAT_0077f610 + sizeof(gs_visit_buf_0077f610), (char **)0);

  s.ok &= LoadTextSectionStringTable(filename, "CASTLEWIN", gs_castlewin_0074b8c0, 0xb, DAT_0077e700,
                                     DAT_0077e700 + sizeof(gs_castlewin_buf_0077e700), (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "DUNGEON", gs_dungeon_0077f000, 0x14, DAT_00780870, DAT_00780870 + sizeof(gs_dungeon_buf_00780870),
                                     (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "HINTTEXT", gs_hinttext_0077e580, 2, DAT_0077e5a0, DAT_0077e5a0 + sizeof(gs_hinttext_buf_0077e5a0),
                                     (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "QUESTFAILED", gs_questfailed_0077c580, 6, DAT_0074b930,
                                     DAT_0074b930 + sizeof(gs_questfailed_buf_0074b930), (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "MONSTERLAIR", gs_monsterlair_0074cff0, 6, DAT_0074d010,
                                     DAT_0074d010 + sizeof(gs_monsterlair_buf_0074d010), (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "BUYANYCARD", gs_buyanycard_0074ccd0, 6, DAT_0077f1e0,
                                     DAT_0077f1e0 + sizeof(gs_buyanycard_buf_0077f1e0), (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "QUESTSTATUS", gs_queststatus_0077e0a0, 0x1a, DAT_0074d270,
                                     DAT_0074d270 + sizeof(gs_queststatus_buf_0074d270), (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "NEWSFLASH", gs_newsflash_0077d140, 0xb, DAT_0077d1c0,
                                     DAT_0077d1c0 + sizeof(gs_newsflash_buf_0077d1c0), (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "CITYCARDTEXT", gs_citycardtext_0074ccf0, 0x31, DAT_0074c0f0,
                                     DAT_0074c0f0 + sizeof(gs_citycardtext_buf_0074c0f0), (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "BROWSE", gs_browse_0074da20, 0x11,
                                     DAT_0077d610 + sizeof(gs_city_text_cluster_0077d610.citynames_buf_0077d610),
                                     DAT_0077d610 + sizeof(gs_city_text_cluster_0077d610.citynames_buf_0077d610) + sizeof(gs_city_text_cluster_0077d610.browse_buf_0077dc50),
                                     (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "SHOWDECK", gs_showdeck_0074b920, 3, DAT_0074d890,
                                     DAT_0074d890 + sizeof(gs_showdeck_buf_0074d890), (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "LOADSAVE", gs_loadsave_0077d1b0, 3, DAT_0077d040, DAT_0077d040 + sizeof(gs_loadsave_buf_0077d040),
                                     (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "STATS", gs_stats_0077cfa0, 9, DAT_0074ce20, DAT_0074ce20 + sizeof(gs_stats_buf_0074ce20),
                                     (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "ANALYZE", gs_analyze_0074b870, 0x11, DAT_0074afb0, DAT_0074afb0 + sizeof(gs_analyze_buf_0074afb0),
                                     (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "RIDDLE", gs_riddle_0077cf20, 0x11, DAT_0077ca20, DAT_0077ca20 + sizeof(gs_riddle_buf_0077ca20),
                                     (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "LAIR", gs_lair_0077e180, 0x13, DAT_007800c0, DAT_007800c0 + sizeof(gs_lair_buf_007800c0),
                                     (char **)0);

  s.local_8 = DAT_00780c60;
  s.ok &= LoadTextSectionStringTable(filename, "WISEMAN", gs_wiseman_0074d840, 0x13, DAT_00780c60, DAT_00780c60 + sizeof(gs_wiseman_buf_00780c60),
                                     &s.local_8);
  s.ok &= LoadTextSectionStringTable(filename, "CITYWISEMAN", gs_citywiseman_0074d800, 0xd, s.local_8, DAT_00780c60 + sizeof(gs_wiseman_buf_00780c60),
                                     (char **)0);
  s.ok &= LoadTextSectionStringTable(filename, "CITYSCREEN_BUTTONS", gs_cityscreen_buttons_0077f5e0, 8, DAT_0077f450,
                                     DAT_0077f450 + sizeof(gs_cityscreen_buttons_buf_0077f450), (char **)0);

  LoadTextSectionLines(filename, "SHOWLIST");
  strcpy(DAT_0077e110, text_lines[0]);
  strcpy(DAT_0077e142, text_lines[1]);

  LoadTextSectionLines(filename, "SHOWLIBRARY");
  strcpy(DAT_0074bcc0, text_lines[0]);
  strcpy(DAT_0074bcf2, text_lines[1]);

  return s.ok;
}

// FUNCTION: SHANDALAR 0x00565c7e
int LoadTextSectionStringTable(const char *filename, const char *section, char **out_table, int max_entries, char *string_buf,
                               char *string_buf_end, char **out_next_buf)
{
  int overflow;
  int count;
  size_t line_len;
  int i;
  char *cursor;

  overflow = 0;
  count = LoadTextSectionLines(filename, section);
  if (max_entries <= count)
  {
    count = max_entries;
  }

  cursor = string_buf;
  i = 0;
  while (i < count && !overflow)
  {
    line_len = strlen(text_lines[i]);
    if (cursor + line_len < string_buf_end)
    {
      strcpy(cursor, text_lines[i]);
      out_table[i] = cursor;
      cursor = cursor + line_len + 1;
    }
    else
    {
      overflow = 1;
    }
    ++i;
  }

  while (i = count, i < max_entries)
  {
    out_table[i] = (char *)"";
    count = i + 1;
  }

  if (out_next_buf != (char **)0)
  {
    *out_next_buf = cursor;
  }

  return !overflow;
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

  ok = 1;
  file_handle = CreateFileA(filename, 0x80000000, 1, (LPSECURITY_ATTRIBUTES)0, 3, 0x8000080, (HANDLE)0);
  if (file_handle == (HANDLE)-1)
  {
    ok = 0;
  }
  else
  {
    file_size = GetFileSize(file_handle, (LPDWORD)0);
    g_advblocks_file_buffer = (char *)malloc(file_size + 1);
    if (g_advblocks_file_buffer == (char *)0)
    {
      ok = 0;
    }
    else
    {
      ReadFile(file_handle, (void *)g_advblocks_file_buffer, file_size, &bytes_read, (LPOVERLAPPED)0);
      cursor = g_advblocks_file_buffer;
      buffer_end = g_advblocks_file_buffer + bytes_read;

      for (i = 0; i < 4; ++i)
      {
        if (FindNextTextBlock(cursor, buffer_end, &block_text_start, (int *)&next_block_scan) == 0)
        {
          ok = 0;
        }
        else
        {
          DAT_0074c930[i] = (char *)block_text_start;
          cursor = next_block_scan;
          while (carriage_return = strchr(DAT_0074c930[i], 0xd), carriage_return != (char *)0)
          {
            strcpy(carriage_return, carriage_return + 1);
          }
        }
      }

      for (i = 0; i < 0xc; ++i)
      {
        if (FindNextTextBlock(cursor, buffer_end, &block_text_start, (int *)&next_block_scan) == 0)
        {
          ok = 0;
        }
        else
        {
          DAT_0077c9e0[i] = (char *)block_text_start;
          cursor = next_block_scan;
          while (carriage_return = strchr(DAT_0077c9e0[i], 0xd), carriage_return != (char *)0)
          {
            strcpy(carriage_return, carriage_return + 1);
          }
        }
      }
    }

    CloseHandle(file_handle);
  }

  return ok;
}

// FUNCTION: SHANDALAR 0x00559999
void FUN_00559999(void)
{
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
unsigned int FindDriveWithAsset(char *filename)
{
  UINT drive_type;
  FILE *asset_file;
  int i;
  DWORD *scratch_cursor;
  char drive_string[260];
  DWORD scratch[63];

  *(DWORD *)drive_string = (DWORD)g_drive_prefix_a_colon_backslash_dword;

  scratch_cursor = scratch;
  for (i = 0x3f; i != 0; --i)
  {
    *scratch_cursor++ = 0;
  }

  for (;;)
  {
    if ('z' < drive_string[0])
    {
      return (unsigned int)drive_string[0];
    }

    drive_type = GetDriveTypeA(drive_string);
    if (drive_type == 5)
    {
      strcat(drive_string, filename);
      asset_file = fopen(drive_string, "rb");
      if (asset_file != (FILE *)0)
      {
        fclose(asset_file);
        return (unsigned int)drive_string[0];
      }

      drive_string[3] = 0;
    }

    ++drive_string[0];
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
  int *font_cfg_entry;
  int i;
  void *page;

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

  InitializeGraphicsSystemDefaultMode();
  ReadPalette("todpal.tr", (char *)0);

  for (i = 0; i < 3; i = i + 1)
  {
    if ((i == 1) && (g_graphics_pages[0]->width < 0x401))
    {
      page = CreateGraphicsPage(1, 0x400, 800, 8);
    }
    else
    {
      page = CreateGraphicsPage(i, global_screen_width, global_screen_height + 1, 8);
    }
    SetGraphicsPage(i, page);
  }

  page = CreateGraphicsPage(5, ScaleUiCoordinate(0x40), ScaleUiCoordinate(0x148), 8);
  SetGraphicsPage(5, page);

  page = CreateGraphicsPage(3, ScaleUiCoordinate(0x280),
                            (ScaleUiCoordinate(0x1e0) - ScaleUiCoordinate(0x148)) + 3, 8);
  SetGraphicsPage(3, page);

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
  } while (DAT_009300f0 == 0);

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
  case 2:
    sound_close();
    PostQuitMessage(0);
    return DefWindowProcA(hwnd, 2, wparam, lparam);
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
    if (g_palette_window_hwnd == (HWND)0)
    {
      g_palette_window_hwnd = CreatePalettePopupWindow(g_app_instance, (void *)0);
      if (g_palette_window_hwnd == (HWND)0)
      {
        return 0;
      }
      ShowWindow(g_palette_window_hwnd, 5);
    }
    else
    {
      BringWindowToTop(g_palette_window_hwnd);
    }

    UpdateWindow(g_palette_window_hwnd);
    return DefWindowProcA(hwnd, msg, 0x7a, lparam);
  case 0x200:
    g_mouse_x = (int)((unsigned int)lparam & 0xffff);
    g_mouse_y = (int)(((unsigned int)lparam >> 0x10) & 0xffff);
    break;
  case 0x201:
    g_mouse_button_down_mask = 1;
    g_mouse_x = (int)((unsigned int)lparam & 0xffff);
    g_mouse_y = (int)(((unsigned int)lparam >> 0x10) & 0xffff);
    break;
  case 0x202:
    g_mouse_button_released_mask |= 2;
    g_mouse_button_down_mask = 0;
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
  int custom_mode_selected;
  HWND existing_main;
  HWND magic_shell_hwnd;
  int horzres;
  char module_path[100];
  char game_dir[256];
  char cmd_copy[100];
  char *slash;
  WNDCLASSA wndclass;
  HWND main_hwnd;
  MSG msg;
  DWORD thread_id;
  DWORD sectors_per_cluster;
  DWORD bytes_per_sector;
  DWORD number_of_free_clusters;
  DWORD total_number_of_clusters;
  int disk_free_bytes;
  int tick;
  int show_shell_on_exit;
  int resolution_option_matched;
  HDC screen_hdc;
  int event_timer_ok;
  HPALETTE stock_palette;

  (void)prevInstance;

  custom_mode_selected = 0;
  strcpy(cmd_copy, cmdLine);
  DAT_00715fa0 = 0;

  existing_main = FindWindowA("ShandalarMainClass", (LPCSTR)0);
  if (existing_main != (HWND)0)
  {
    ShowWindow(existing_main, 9);
    SetForegroundWindow(existing_main);
    return 0;
  }

  magic_shell_hwnd = FindWindowA("Magic Shell", (LPCSTR)0);
  if (magic_shell_hwnd != (HWND)0 && _strnicmp(cmdLine, "/MTGshell", 9) != 0)
  {
    PostMessageA(magic_shell_hwnd, 0x400, 2, 0);
    return 0;
  }

  if (InitLicenseSecretsFromRegistry() != 0)
  {
    return 0;
  }

  GetWindowsDirectoryA(module_path, 0x100);
  module_path[3] = 0;
  GetDiskFreeSpaceA(module_path, &sectors_per_cluster, &bytes_per_sector, &number_of_free_clusters,
                    &total_number_of_clusters);

  disk_free_bytes = total_number_of_clusters * sectors_per_cluster * number_of_free_clusters;
  tick = disk_free_bytes + ((unsigned int)disk_free_bytes >> 0x1f & 0x1fU);
  tick = (tick >> 5) + ((unsigned int)tick >> 0x1f & 0x3ffU);

  if (((tick >> 10) + ((unsigned int)tick >> 0x1f & 0x3ffU)) >> 10 == 0)
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

  FUN_00559999();

  /* Switch to executable directory */
#ifndef _DEBUG
  strcpy(game_dir, (*__p___argv())[0]);
  slash = strrchr(game_dir, '\\');
  if (slash)
  {
    *slash = 0;
  }
  _chdir(game_dir);
#endif

  g_frontbuffer_direct_blit_enabled = 0;
  g_app_instance = hInstance;

  memset(&wndclass, 0, sizeof(wndclass));
  wndclass.style = 0x23;
  wndclass.lpfnWndProc = MainWindowProc;
  wndclass.hInstance = hInstance;
  wndclass.hIcon = LoadIconA(hInstance, (LPCSTR)0x65);
  wndclass.hCursor = LoadCursorA((HINSTANCE)0, (LPCSTR)0x7f00);
  wndclass.hbrBackground = GetStockObject(4);
  wndclass.lpszClassName = "ShandalarMainClass";

  if (RegisterClassA(&wndclass) == 0)
  {
    MessageBoxA((HWND)0, "Couldn't register the classes", (LPCSTR)0, 0x1010);
    return 0;
  }

  atexit(RestoreDisplayResolution);

  screen_hdc = GetDC((HWND)0);
  horzres = GetDeviceCaps(screen_hdc, 8);
  if (0x400 < horzres)
  {
    ChangeDisplayResolution(0x400, 0x300);
  }

  /* Command line resolution override parsing (still being refined for matching) */
  resolution_option_matched = 0;
  show_shell_on_exit = 0;
  if (cmdLine && cmdLine[0] && cmdLine[1])
  {
    if (cmdLine[1] == '6')
    {
      global_screen_width = 0x280;
      global_screen_height = 0x1e0;
      PTR_s_advinter800_pic_00589de8 = "advinter.pic";
      CreateGraphicsPage(0, 0x280, 0x1e0, -1);
      show_shell_on_exit = 1;
      resolution_option_matched = 1;
    }
    else if (cmdLine[1] == '8')
    {
      global_screen_width = 800;
      global_screen_height = 600;
      PTR_s_advinter800_pic_00589de8 = "advinter800.pic";
      CreateGraphicsPage(0, 800, 600, -1);
      show_shell_on_exit = 1;
      resolution_option_matched = 1;
    }
    else if (cmdLine[1] == '1')
    {
      global_screen_width = 0x400;
      global_screen_height = 0x300;
      PTR_s_advinter800_pic_00589de8 = "advinter1024.pic";
      CreateGraphicsPage(0, 0x400, 0x300, -1);
      show_shell_on_exit = 1;
      resolution_option_matched = 1;
    }
  }

  if (!resolution_option_matched)
  {
    screen_hdc = GetDC((HWND)0);
    horzres = GetDeviceCaps(screen_hdc, 8);
    if (horzres == 0x280)
    {
      global_screen_width = 0x280;
      global_screen_height = 0x1e0;
      PTR_s_advinter800_pic_00589de8 = "advinter.pic";
    }
    else if (horzres == 800)
    {
      global_screen_width = 800;
      global_screen_height = 600;
      PTR_s_advinter800_pic_00589de8 = "advinter800.pic";
    }
    else
    {
      global_screen_width = 0x400;
      global_screen_height = 0x300;
      PTR_s_advinter800_pic_00589de8 = "advinter1024.pic";
    }
  }

  if (PTR_DAT_005832b4)
  {
    PTR_DAT_005832b4->max_x = global_screen_width - 1;
    PTR_DAT_005832b4->max_y = global_screen_height - 1;
  }

#ifdef _DEBUG
  // Annoying trying to debug with window always on top
  main_hwnd = CreateWindowExA(0, "ShandalarMainClass", "Magic: Shandalar", 0x80000000, 0, 0,
                              global_screen_width, global_screen_height, (HWND)0, (HMENU)0, hInstance, (LPVOID)0);
#else
  main_hwnd = CreateWindowExA(8, "ShandalarMainClass", "Magic: Shandalar", 0x80000000, 0, 0,
                              global_screen_width, global_screen_height, (HWND)0, (HMENU)0, hInstance, (LPVOID)0);
#endif
  g_main_window_hwnd = main_hwnd;
  ShowWindow(main_hwnd, nShowCmd);
  global_main_hdc = GetDC(main_hwnd);

  if (show_shell_on_exit)
  {
    g_graphics_pages[0]->hTempDC = global_main_hdc;
    SelectPalette(g_graphics_pages[0]->hTempDC, g_graphics_pages[0]->hPalette, FALSE);
    RealizePalette(g_graphics_pages[0]->hTempDC);
    SetStretchBltMode(g_graphics_pages[0]->hTempDC, 3);
  }

  InitializeSoundPresenceState();
  init_sound_dll(main_hwnd, 0, 1);

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
  assert((unsigned int)(g_timer_event_handle != 0xffffffff), "D:\\Newmagic\\multiplayer\\sid\\Test.c", 0x15c,
         "Could not start timer\n");

  SetSystemPaletteUse(global_main_hdc, 2);
  atexit(FUN_004ce9e9);

  g_main_thread_handle = GetCurrentThread();
  DuplicateHandle(GetCurrentProcess(), g_main_thread_handle, GetCurrentProcess(), &g_main_thread_handle, 0x1f03ff, FALSE, 0);

  if (DAT_00715fa0 == 0)
  {
    InitializeCriticalSection(&DAT_00926910);
    DAT_00715fa0 = 1;
  }

  g_loader_thread_handle = CreateThread((LPSECURITY_ATTRIBUTES)0, 0x2000, FUN_0046e6f0, (LPVOID)0, 0, &thread_id);

  while (GetMessageA(&msg, (HWND)0, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessageA(&msg);
  }

  if (DAT_00715fa0 != 0)
  {
    DeleteCriticalSection(&DAT_00926910);
    DAT_00715fa0 = 0;
  }

  FUN_00565faa();
  ChangeDisplayResolution(0, 0);

  screen_hdc = GetDC((HWND)0);
  stock_palette = GetStockObject(0xf);
  SelectPalette(screen_hdc, stock_palette, FALSE);
  SetSystemPaletteUse(screen_hdc, 1);
  RealizePalette(screen_hdc);
  ReleaseDC((HWND)0, screen_hdc);

  if (_strnicmp(cmd_copy, "/MTGshell", 8) == 0)
  {
    char exe_cmd[264];

    strcpy(exe_cmd, global_base_directory);
    strcat(exe_cmd, "\\Magic.exe");
    strcat(exe_cmd, " /start3,1");
    WinExec(exe_cmd, 5);
  }

  return (int)msg.wParam;
}
