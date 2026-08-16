#include <windows.h>
#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vfw.h>

extern "C"
{
#include "magsnd.h"
}

typedef unsigned char byte;
typedef unsigned int uint;

typedef struct DibState_t DibState;

typedef struct StatWinData_t
{
  int progress_by_color[5];
  int duel_wins_by_color[5];
  unsigned char town_count_by_color[5];
  unsigned char highlighted_wizard_color;
  unsigned char highlighted_creature_type;
  unsigned char highlighted_lair_color;
  int field_30;
} StatWinData;

typedef struct AviPosition_t
{
  short x;
  short y;
} AviPosition;

typedef struct DrawRect_t
{
  int x;
  int y;
  int width;
  int height;
} DrawRect;

typedef struct SkullBitmapAsset_t
{
  char filename[0x100];
  int field_100;
  int field_104;
  int field_108;
  int field_10c;
} SkullBitmapAsset;

typedef struct BitmapRectAsset_t
{
  char filename[0x100];
  DrawRect rect;
} BitmapRectAsset;

typedef struct ProgressTotalOffset_t
{
  int y;
  int height;
} ProgressTotalOffset;

typedef struct DibState_t
{
  void *vtable;
  BITMAPINFOHEADER *header;
  void *bits;
  int owns_bits;
  void *field_10;
  int field_14;

  DibState_t();

  // SYNTHETIC: STATWIN 0x10002d20
  // DibState_t::`scalar deleting destructor'
  ~DibState_t();
  int load_bitmap(HWND hwnd, char *path, uint bit_count);
  int save_bitmap(char *path);
  void set_field_10_value(int value);
  BITMAPINFOHEADER *get_header(void);
  void *get_bits(void);
  void *get_pixel_address(int x, int y);
  int get_width(void);
  int get_height(void);
  void get_bounds_rect(RECT *rect);
  int create(int width, int height, int bit_count);
  int attach_external_bits(BITMAPINFOHEADER *source_header, void *source_bits);
  void blit_to(DibState *dest, int dest_x, int dest_y, int width, int height, int source_x, int source_y);
} DibState;

typedef struct WriteCacheLocals_t
{
  char cache_path[256];
  HANDLE file;
  DibState cache_dib;
  BITMAPINFOHEADER *header;
  void *bits;
  void *palette;
  DWORD bytes_written;
  int color_count;
} WriteCacheLocals;

static int __cdecl bitmap_header_color_count(BITMAPINFOHEADER *header);

class StatWinState;
extern StatWinState *g_current_statwin_state;

class StatWinState
{
public:
  StatWinData *cached_data;
  int field_04;
  byte *cached_bitmap_info;
  DibState *field_0c;
  DibState *field_10;
  int cache_x;
  int cache_y;
  int field_1c;
  int has_window;
  int field_24;

  // FUNCTION: STATWIN 0x100052a7
  StatWinState()
  {
    this->cached_data = 0;
    this->has_window = 0;
    g_current_statwin_state = this;
    this->field_24 = 0;

    this->field_1c = 1;
    this->cache_x = 0;
    this->cache_y = 0;
    this->cached_bitmap_info = 0;
    this->field_10 = 0;
    this->field_0c = 0;
  }

  // SYNTHETIC: STATWIN 0x10002ca0
  // StatWinState::`scalar deleting destructor'
  ~StatWinState();

  int apply_progress(StatWinData *data, int flags);
  int prepare_resources(void);
  int choose_mode(int *mode, StatWinData *data, int *selection);
  int update(StatWinData *data, int mode);
  void write_cache(void);
  int release_cache(void);
  int draw_missing_color_progress(uint color);
  void draw_color_progress(uint color);
  int draw_town_count(uint color);
  int draw_progress_total(uint color);
  void prepare_missing_masks(void);
  void draw_initial_town_counts(StatWinData *data);
  void draw_initial_sprites(StatWinData *data);
  void draw_initial_progress_totals(StatWinData *data);
  int draw_initial_duel_wins(void);
  int prepare_duel_foreground(DrawRect *rect, uint color);
  int refresh_town_counts(StatWinData *data);
  int refresh_progress_totals(StatWinData *data);
  int refresh_duel_wins(StatWinData *data);
};

static int __cdecl reset_status_assets(void);
static char __cdecl find_cdrom_drive_for_asset(char *path);
static char __cdecl get_status_asset_drive(void);
extern "C" int __cdecl bitmap_header_has_color_table(BITMAPINFOHEADER *header);
extern "C" int __fastcall dib_state_get_color_count(DibState *state);
extern "C" void *__fastcall dib_state_get_palette(DibState *state);
extern "C" int __fastcall dib_state_get_width(DibState *state);
extern "C" int __fastcall dib_state_get_height(DibState *state);
extern "C" int __fastcall dib_state_get_bit_count(DibState *state);
extern "C" void __fastcall dib_state_release_field_10(DibState *state);
extern "C" uint __fastcall dib_state_get_stride(DibState *state);
extern "C" void __cdecl join_paths(char *out_path, char *base_path, char *filename);
extern "C" int __cdecl duel_wins_to_skull_index(int duel_wins);
static void __cdecl show_status_window(void);
static void __cdecl show_world_magic_detail(StatWinData *data, int unused);
static int __cdecl show_color_missing_message(uint color);
static int __cdecl load_centered_background(DibState *dest, char *path);
extern "C" int __cdecl pump_one_statwin_message(void);
extern "C" void __cdecl play_status_avi(int avi);
extern "C" HWND __cdecl get_sound_hwnd(void);
static void __cdecl play_status_sound(char *path);
static int __cdecl init_video_dll(int hwnd, HINSTANCE instance, int flags);
static int __cdecl release_video_dll(void);
static void __cdecl clear_video_imports_table(void);
static int __cdecl load_avi(char *path, int *out_avi, AviPosition *position, uint flags);
static int __cdecl play_avi(int avi);
static int __cdecl stop_avi(int avi);
static int __cdecl unload_avi(int avi);
extern "C" int __cdecl set_vid_callback(int avi, int callback);
extern "C" int __cdecl set_vid_background(int avi, int background);
extern "C" int __cdecl set_vid_background_to_bmp(int avi, char *path, int flags);
extern "C" int __cdecl paint_vid(int avi);
extern "C" int __cdecl set_vid_pos(int avi, AviPosition *position);
extern "C" int __cdecl set_vid_background_to_dib(int avi, int dib);
extern "C" int __cdecl set_vid_foreground(int avi, int foreground);
extern "C" int __cdecl draw_vid_background(int avi);
extern "C" int __cdecl link_vids(int avi, int linked_avi);
extern "C" int __cdecl set_vid_thread_priority(int avi, int priority);
static int __cdecl set_vid_transparency(int avi, int transparency);
static int __cdecl vid_status(int avi);
static int __cdecl poll_statwin_messages(void);
static BOOL CALLBACK statwin_help_dialog_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
static void __cdecl draw_status_dib_to_window(HWND hwnd);
static LRESULT CALLBACK status_window_wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
static LRESULT CALLBACK fullscreen_video_wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

extern char g_statwin_class_name[13];

// GLOBAL: STATWIN 0x1000d02c
static int g_detail_window_enabled;
// GLOBAL: STATWIN 0x1000d26c
static int g_sound_available;
// GLOBAL: STATWIN 0x1000d270
static int g_video_available;
// GLOBAL: STATWIN 0x1000d274
static int g_status_initialized;
// GLOBAL: STATWIN 0x1000d278
static UINT DAT_1000d278;
// GLOBAL: STATWIN 0x1000d27c
static int g_video_erase_background_pending = 1;
// GLOBAL: STATWIN 0x1000d280
static char *g_statwin_class_name_ptr = g_statwin_class_name;
// GLOBAL: STATWIN 0x1000d284
static StatWinState *g_status_state;
// GLOBAL: STATWIN 0x1000d2a8
static int g_help_dialog_open;
// GLOBAL: STATWIN 0x1000d684
static char g_statwin_class_name[] = "STATWINCLASS";
// GLOBAL: STATWIN 0x1000e510
static int g_asset_drive_initialized;
// GLOBAL: STATWIN 0x1000e514
static HINSTANCE g_statwin_instance;
// GLOBAL: STATWIN 0x1000e518
static int g_use_cdrom_asset_drive;
// GLOBAL: STATWIN 0x1000e51c
static int g_detail_main_avi;
// GLOBAL: STATWIN 0x1000e524
static char g_status_flag_byte;
// GLOBAL: STATWIN 0x1000e528
static int g_video_stop_requested;
// GLOBAL: STATWIN 0x1000e52c
static int g_help_dialog_enabled;
// GLOBAL: STATWIN 0x1000d020
static char *g_status_flag_ptr = "':statwin\\";
// GLOBAL: STATWIN 0x1000d024
static char *g_status_sound_dir = ".\\";
// GLOBAL: STATWIN 0x1000d028
static char *g_status_sound_name = "statscrn.wav";
// GLOBAL: STATWIN 0x1000d034
static AviPosition g_detail_main_avi_position = {0xe0, 0x118};
// GLOBAL: STATWIN 0x1000d038
static char *g_wizard_hit_avi_names[5] = {
    "Whit-hit.avi",
    "Red-hit.avi",
    "Blue-hit.avi",
    "Grn-hit.avi",
    "Blak-hit.avi"};
// GLOBAL: STATWIN 0x1000d050
static char *g_missing_color_avi_names[6] = {
    "bubbles.avi",
    "fire.avi",
    "water.avi",
    "vine.avi",
    "smoke.avi",
    "monster.avi"};
// GLOBAL: STATWIN 0x1000d068
static char *g_missing_color_room_bitmap_names[6] = {
    "room-wht.bmp",
    "room-red.bmp",
    "Room-blu.bmp",
    "room-grn.bmp",
    "room-blk.bmp",
    "room-mon.bmp"};
// GLOBAL: STATWIN 0x1000d208
static DrawRect g_missing_color_avi_rects[5] = {
    {0x8c, 0, 0x168, 0x1b8},
    {0xa0, 0, 0x12c, 0x1b8},
    {0xd4, 0x30, 0xf0, 0x19c},
    {0x88, 0x0c, 0x170, 0x1b0},
    {0xc8, 0, 0x104, 0x1cc}};
// GLOBAL: STATWIN 0x1000d088
static char *g_detail_creature_avi_names[56] = {
    "cball00.avi", "b_fwz.avi",   "b_kht.avi", "b_mwz.avi",  "b_lrd.avi", "b_wg.avi",   "b_amg.avi",
    "cbal00.avi",  "w_mwz.avi",   "w_fwz.avi", "w_kht.avi",  "w_lrd.avi", "w_wg.avi",   "w_amg.avi",
    "cball00.avi", "u_fwz.avi",   "u_lrd.avi", "u_mwz.avi",  "u_wrm.avi", "u_sft.avi",  "u_amg.avi",
    "cball00.avi", "g_mwz.avi",   "g_kht.avi", "g_fwz.avi",  "g_wrm.avi", "g_lrd.avi",  "g_amg.avi",
    "cball00.avi", "r_fwz.avi",   "r_mwz.avi", "r_trl.avi",  "r_lrd.avi", "r_wrm.avi",  "r_amg.avi",
    "r_amg.avi",   "m_tsk.avi",   "m_trl.avi", "m_ape.avi",  "m_cen2.avi", "m_wg.avi",  "m_fng.avi",
    "m_cen.avi",   "m_lrd.avi",   "m_kht.avi", "m_fwz.avi",  "b_djn.avi", "g_djn.avi",  "r_djn.avi",
    "u_djn.avi",   "dg_bru.avi", "dg_uwb.avi", "dg_gwr.avi", "dg_rgb.avi", "dg_wug.avi", "cball00.avi"};
// GLOBAL: STATWIN 0x1000d168
static DrawRect g_detail_wizard_rects[5] = {
    {0xf8, 0x3c, 0x90, 0x12c},
    {0x154, 0x30, 0xc8, 0x12c},
    {0, 0x30, 0x104, 0x140},
    {0x8c, 0x3c, 0xa0, 0x118},
    {0x17c, 0x30, 0x104, 0x12c}};

typedef int(__cdecl *MagVidFn)(void);
// GLOBAL: STATWIN 0x10017d8c
static HMODULE g_magvid_module;
// GLOBAL: STATWIN 0x1000d718
static int g_video_dll_status;
// GLOBAL: STATWIN 0x1000d71c
static char *PTR_s_magvid_1000d71c = "magvid";
// GLOBAL: STATWIN 0x10017d40
static MagVidFn g_magvid_fns[19];
// GLOBAL: STATWIN 0x10017d90
static MagVidFn g_magvid_delay_fn;

// GLOBAL: STATWIN 0x1000e480
static char *g_statwin_asset_dir = "statwin\\";
// GLOBAL: STATWIN 0x1000e488
static char *g_wizard_background_bitmap_name = "wizbg.bmp";
// GLOBAL: STATWIN 0x1000e490
static char *g_stats_cache_bitmap_name = "statscrn.tmp";
// GLOBAL: STATWIN 0x1000e48c
StatWinState *g_current_statwin_state;

// GLOBAL: STATWIN 0x1000dc98
static SkullBitmapAsset g_skull_bitmap_assets[7] = {
    {"1skulls.bmp", 0, 0xf0, 0x280, 0xf0},
    {"2skulls.bmp", 0, 0xf0, 0x280, 0xf0},
    {"3skulls.bmp", 0, 0xf0, 0x280, 0xf0},
    {"4skulls.bmp", 0, 0xf0, 0x280, 0xf0},
    {"5skulls.bmp", 0, 0xf0, 0x280, 0xf0},
    {"8skulls.bmp", 0, 0xf0, 0x280, 0xf0},
    {"12skulls.bmp", 0, 0xf0, 0x280, 0xf0}};

// GLOBAL: STATWIN 0x1000e408
static DrawRect g_duel_win_skull_rects[5] = {
    {0x104, 0xf0, 0x78, 0xf0},
    {0x17c, 0xf0, 0x8c, 0xf0},
    {0, 0xf0, 0x8c, 0xf0},
    {0x8c, 0xf0, 0x78, 0xf0},
    {0x208, 0xf0, 0x78, 0xf0}};

// GLOBAL: STATWIN 0x1000a020
static BitmapRectAsset g_progress_red_assets[5] = {
    {"S-wwizr.bmp", {330, 86, 60, 252}},
    {"S-rwizr.bmp", {468, 80, 60, 260}},
    {"S-buwizr.bmp", {0, 38, 80, 300}},
    {"S-gwizr.bmp", {140, 38, 60, 300}},
    {"S-bkwizr.bmp", {580, 12, 60, 324}}};

// GLOBAL: STATWIN 0x1000a570
static BitmapRectAsset g_progress_yellow_assets[5] = {
    {"S-wwizy.bmp", {330, 86, 60, 252}},
    {"S-rwizy.bmp", {468, 80, 60, 260}},
    {"S-buwizy.bmp", {0, 38, 80, 300}},
    {"S-gwizy.bmp", {140, 38, 60, 300}},
    {"S-bkwizy.bmp", {580, 12, 60, 324}}};

// GLOBAL: STATWIN 0x1000aac0
static BitmapRectAsset g_small_missing_mask_assets[5] = {
    {"S-wmsk.tmp", {330, 86, 60, 252}},
    {"S-rmsk.tmp", {468, 80, 60, 260}},
    {"S-bumsk.tmp", {0, 38, 80, 300}},
    {"S-gmsk.tmp", {140, 38, 60, 300}},
    {"S-bkmsk.tmp", {580, 12, 60, 324}}};

// GLOBAL: STATWIN 0x1000b010
static BitmapRectAsset g_mana_base_assets[5] = {
    {"W-mana.bmp", {294, 30, 48, 48}},
    {"R-mana.bmp", {428, 24, 48, 48}},
    {"U-mana.bmp", {58, 12, 48, 48}},
    {"G-mana.bmp", {184, 24, 48, 48}},
    {"B-mana.bmp", {544, 12, 48, 48}}};

// GLOBAL: STATWIN 0x1000b560
static BitmapRectAsset g_world_missing_mask_assets[5] = {
    {"W-mmask.tmp", {294, 30, 48, 48}},
    {"R-mmask.tmp", {428, 24, 48, 48}},
    {"Bl-mmask.tmp", {58, 12, 48, 48}},
    {"G-mmask.tmp", {184, 24, 48, 48}},
    {"Bk-mmask.tmp", {544, 12, 48, 48}}};

// GLOBAL: STATWIN 0x1000bab0
static DrawRect g_town_mana_source_rects[5][5] = {
    {{31, 13, 16, 15}, {3, 13, 18, 15}, {18, 4, 15, 16}, {9, 28, 17, 15}, {26, 27, 18, 17}},
    {{32, 10, 13, 14}, {4, 13, 14, 14}, {17, 13, 14, 14}, {8, 27, 15, 15}, {24, 25, 17, 17}},
    {{30, 18, 18, 15}, {0, 8, 19, 18}, {19, 1, 15, 18}, {6, 28, 19, 17}, {27, 31, 17, 16}},
    {{32, 15, 14, 16}, {4, 12, 14, 16}, {8, 5, 14, 13}, {8, 28, 16, 14}, {24, 28, 16, 15}},
    {{29, 8, 19, 19}, {0, 14, 17, 20}, {12, 0, 19, 21}, {0, 33, 26, 14}, {23, 29, 20, 16}}};

// GLOBAL: STATWIN 0x1000bc40
static BitmapRectAsset g_color_sprite_assets[5] = {
    {"wht-sprt.bmp", {272, 60, 104, 300}},
    {"red-sprt.bmp", {376, 48, 136, 300}},
    {"blu-sprt.bmp", {0, 48, 160, 300}},
    {"grn-sprt.bmp", {160, 56, 104, 300}},
    {"blk-sprt.bmp", {516, 48, 124, 300}}};

// GLOBAL: STATWIN 0x1000d748
static BitmapRectAsset g_color_mask_assets[5] = {
    {"wht-mask.bmp", {272, 60, 104, 300}},
    {"red-mask.bmp", {376, 48, 136, 300}},
    {"blu-mask.bmp", {0, 48, 160, 300}},
    {"grn-mask.bmp", {160, 56, 104, 300}},
    {"blk-mask.bmp", {516, 48, 124, 300}}};

// GLOBAL: STATWIN 0x1000e458
static ProgressTotalOffset g_progress_total_offsets[5] = {
    {53, 181},
    {45, 186},
    {70, 192},
    {85, 190},
    {20, 290}};

// GLOBAL: STATWIN 0x1000a000
static void *g_dib_state_vtable;

// GLOBAL: STATWIN 0x1000e4e8
WNDCLASSA g_window_class;

// FUNCTION: STATWIN 0x10001000
extern "C" BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
  // int result;

  (void)reserved;
  switch (reason)
  {
  case DLL_PROCESS_ATTACH:
    g_statwin_instance = instance;
    if ((g_status_state = new StatWinState()) == 0)
    {
      return FALSE;
    }

    g_video_available = (init_video_dll(0, g_statwin_instance, 0) == 0);
    g_sound_available = (init_sound_dll(0, 0, 3) == 0);

    g_window_class.style = CS_VREDRAW | CS_HREDRAW;
    g_window_class.lpfnWndProc = status_window_wndproc;
    g_window_class.cbClsExtra = 0;
    g_window_class.cbWndExtra = 0;
    g_window_class.hInstance = g_statwin_instance;
    g_window_class.hIcon = LoadIconA(0, IDI_APPLICATION);
    g_window_class.hCursor = LoadCursorA(0, IDC_ARROW);
    g_window_class.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    g_window_class.lpszMenuName = 0;
    g_window_class.lpszClassName = g_statwin_class_name_ptr;
    RegisterClassA(&g_window_class);
    break;

  case DLL_PROCESS_DETACH:
    if (g_status_state != 0)
    {
      delete g_status_state;
      g_status_state = 0;
    }
    if (g_sound_available != 0)
    {
      sound_close();
    }
    if (g_video_available != 0)
    {
      release_video_dll();
    }
    UnregisterClassA(g_statwin_class_name_ptr, g_window_class.hInstance);
    break;

  case DLL_THREAD_ATTACH:
    break;
  case DLL_THREAD_DETACH:
    break;
  }

  return TRUE;
}

// FUNCTION: STATWIN 0x10001273
extern "C" int __cdecl status_init(StatWinData *data)
{
  int result;

  result = g_status_state->apply_progress(data, 0);
  if (result != 0)
  {
    g_status_initialized = 0;
    return result;
  }
  else
  {
    g_status_initialized = 1;
  }
  reset_status_assets();
  *g_status_flag_ptr = g_status_flag_byte;
  return 0;
}

// FUNCTION: STATWIN 0x100012d9
extern "C" void __cdecl status_show(StatWinData *data, int mode)
{
  struct
  {
    short pad2;
    short pad3;
    int pad;
    int selection;
    int result;
  } s;

  s.pad2 = 0xdf;
  s.pad3 = 0x118;
  s.pad = 0;

  if (g_video_available == 0)
  {
    return;
  }

  if (g_status_state->has_window != 0)
  {
    s.result = g_status_state->prepare_resources();
    if (s.result != 0)
    {
      g_status_state->has_window = 0;
      return;
    }
  }
  else
  {
    return;
  }

  if (g_status_state->choose_mode(&mode, data, &s.selection) == 0)
  {
    mode = 0;
  }

  switch (mode)
  {
  case 0:
    g_status_state->update(data, mode);
    show_status_window();
    return;

  case 2:
    if (g_detail_window_enabled != 0)
    {
      g_status_state->update(data, mode);
      show_status_window();
    }
    else
    {
      show_world_magic_detail(data, s.pad);
    }
    break;

  case 1:
    if (*(int *)((char *)data + data->highlighted_wizard_color * 4) == 0)
    {
      show_color_missing_message(data->highlighted_wizard_color);
      if (data->highlighted_wizard_color < 5)
      {
        g_status_state->update(data, mode);
      }
    }
    else if (g_detail_window_enabled != 0)
    {
      g_status_state->update(data, mode);
      show_status_window();
    }
    else
    {
      show_world_magic_detail(data, s.pad);
    }
    break;

  case 3:
    g_status_state->update(data, 3);
    show_status_window();
    break;

  default:
    return;
  }

  g_status_state->write_cache();
  g_status_state->release_cache();
}

// FUNCTION: STATWIN 0x10002958
extern "C" int __cdecl play_video(char *path, int x, int y, int flags)
{
  struct
  {
    int result;
    AviPosition position;
    int avi;
    int screen_height;
    uint load_flags;
    int screen_width;
    HWND parent;
    HWND video_window;
  } s;

  s.load_flags = 0;
  s.avi = 0;
  if (g_sound_available != 0)
  {
    s.parent = get_sound_hwnd();
  }
  else
  {
    s.parent = 0;
  }

  if ((flags & 1) != 0)
  {
    s.screen_width = GetSystemMetrics(SM_CXSCREEN);
    s.screen_height = GetSystemMetrics(SM_CYSCREEN);
    s.video_window = CreateWindowExA(0, g_statwin_class_name_ptr, 0, WS_POPUP, 0, 0, s.screen_width, s.screen_height, s.parent, 0, g_statwin_instance, 0);
    if (s.video_window == 0)
    {
      return 8;
    }
    if (SetWindowLongA(s.video_window, GWL_WNDPROC, (LONG)fullscreen_video_wndproc) == 0)
    {
      DestroyWindow(s.video_window);
      return 8;
    }
    ShowWindow(s.video_window, SW_SHOW);
  }

  if ((flags & 2) != 0)
  {
    s.load_flags |= 2;
    s.position.x = 0;
    s.position.y = 0;
  }
  else
  {
    s.position.x = x;
    s.position.y = y;
  }

  s.result = load_avi(path, &s.avi, &s.position, s.load_flags);
  if (s.result != 0)
  {
    DestroyWindow(s.video_window);
    return s.result;
  }

  set_vid_transparency(s.avi, 0);
  play_avi(s.avi);
  while (vid_status(s.avi) != 0)
  {
    if (poll_statwin_messages() != 0)
    {
      stop_avi(s.avi);
      break;
    }
    Sleep(50);
  }
  unload_avi(s.avi);
  s.avi = 0;
  DestroyWindow(s.video_window);
  return 0;
}

// FUNCTION: STATWIN 0x10003610
static int __cdecl init_video_dll(int hwnd, HINSTANCE instance, int flags)
{
  int i;
  int result;

  result = 0;
  if (g_video_dll_status != 0)
  {
    return 6;
  }

  g_magvid_module = LoadLibraryA(PTR_s_magvid_1000d71c);
  if (g_magvid_module != 0)
  {
    for (i = 0; i < 19; i++)
    {
      g_magvid_fns[i] = (MagVidFn)GetProcAddress(g_magvid_module, (LPCSTR)((i + 1U) & 0xffff));
      if (g_magvid_fns[i] == 0)
      {
        FreeLibrary(g_magvid_module);
        clear_video_imports_table();
        return 7;
      }
    }

    g_magvid_delay_fn = (MagVidFn)GetProcAddress(g_magvid_module, (LPCSTR)((i + 1U) & 0xffff));
  }
  else
  {
    return 7;
  }

  result = ((int(__cdecl *)(int, HINSTANCE, int))g_magvid_fns[0])(hwnd, instance, flags);
  if (result != 0)
  {
    FreeLibrary(g_magvid_module);
    clear_video_imports_table();
    return result;
  }

  g_video_dll_status = 1;
  return 0;
}

// FUNCTION: STATWIN 0x1000373a
static int __cdecl release_video_dll(void)
{
  if (g_video_dll_status == 0)
  {
    return 7;
  }

  g_video_dll_status = 0;
  ((void(__cdecl *)(void))g_magvid_fns[1])();
  FreeLibrary(g_magvid_module);
  clear_video_imports_table();
  return 0;
}

// FUNCTION: STATWIN 0x10003bae
static void __cdecl clear_video_imports_table(void)
{
  int i;

  for (i = 0; i < 19; i++)
  {
    g_magvid_fns[i] = 0;
  }
}

// FUNCTION: STATWIN 0x10003784
static int __cdecl load_avi(char *path, int *out_avi, AviPosition *position, uint flags)
{
  if (g_video_dll_status == 0)
  {
    return 7;
  }
  return ((int(__cdecl *)(char *, int *, AviPosition *, uint))g_magvid_fns[2])(path, out_avi, position, flags);
}

// FUNCTION: STATWIN 0x100037c4
static int __cdecl unload_avi(int avi)
{
  if (g_video_dll_status == 0)
  {
    return 7;
  }
  return ((int(__cdecl *)(int))g_magvid_fns[3])(avi);
}

// FUNCTION: STATWIN 0x100037f8
static int __cdecl play_avi(int avi)
{
  if ((g_video_dll_status == 0) || (g_video_dll_status == 2))
  {
    return 7;
  }
  return ((int(__cdecl *)(int))g_magvid_fns[4])(avi);
}

// FUNCTION: STATWIN 0x10003839
static int __cdecl stop_avi(int avi)
{
  if ((g_video_dll_status == 0) || (g_video_dll_status == 2))
  {
    return 7;
  }
  return ((int(__cdecl *)(int))g_magvid_fns[5])(avi);
}

// FUNCTION: STATWIN 0x1000387a
extern "C" int __cdecl set_vid_callback(int avi, int callback)
{
  if ((g_video_dll_status == 0) || (g_video_dll_status == 2))
  {
    return 7;
  }
  return ((int(__cdecl *)(int, int))g_magvid_fns[6])(avi, callback);
}

// FUNCTION: STATWIN 0x100038bf
extern "C" int __cdecl set_vid_background(int avi, int background)
{
  if ((g_video_dll_status == 0) || (g_video_dll_status == 2))
  {
    return 7;
  }
  return ((int(__cdecl *)(int, int))g_magvid_fns[7])(avi, background);
}

// FUNCTION: STATWIN 0x10003904
extern "C" int __cdecl set_vid_background_to_bmp(int avi, char *path, int flags)
{
  if ((g_video_dll_status == 0) || (g_video_dll_status == 2))
  {
    return 7;
  }
  return ((int(__cdecl *)(int, char *, int))g_magvid_fns[8])(avi, path, flags);
}

// FUNCTION: STATWIN 0x1000394d
extern "C" int __cdecl paint_vid(int avi)
{
  if ((g_video_dll_status == 0) || (g_video_dll_status == 2))
  {
    return 7;
  }
  return ((int(__cdecl *)(int))g_magvid_fns[13])(avi);
}

// FUNCTION: STATWIN 0x1000398e
extern "C" int __cdecl set_vid_pos(int avi, AviPosition *position)
{
  if ((g_video_dll_status == 0) || (g_video_dll_status == 2))
  {
    return 7;
  }
  return ((int(__cdecl *)(int, AviPosition *))g_magvid_fns[14])(avi, position);
}

// FUNCTION: STATWIN 0x100039d3
extern "C" int __cdecl set_vid_background_to_dib(int avi, int dib)
{
  if ((g_video_dll_status == 0) || (g_video_dll_status == 2))
  {
    return 7;
  }
  return ((int(__cdecl *)(int, int))g_magvid_fns[9])(avi, dib);
}

// FUNCTION: STATWIN 0x10003a18
extern "C" int __cdecl set_vid_foreground(int avi, int foreground)
{
  if ((g_video_dll_status == 0) || (g_video_dll_status == 2))
  {
    return 7;
  }
  return ((int(__cdecl *)(int, int))g_magvid_fns[12])(avi, foreground);
}

// FUNCTION: STATWIN 0x10003a5d
static int __cdecl set_vid_transparency(int avi, int transparency)
{
  if ((g_video_dll_status == 0) || (g_video_dll_status == 2))
  {
    return 7;
  }
  return ((int(__cdecl *)(int, int))g_magvid_fns[18])(avi, transparency);
}

// FUNCTION: STATWIN 0x10003aa2
extern "C" int __cdecl draw_vid_background(int avi)
{
  if ((g_video_dll_status == 0) || (g_video_dll_status == 2))
  {
    return 7;
  }
  return ((int(__cdecl *)(int))g_magvid_fns[10])(avi);
}

// FUNCTION: STATWIN 0x10003ae3
static int __cdecl vid_status(int avi)
{
  if ((g_video_dll_status == 0) || (g_video_dll_status == 2))
  {
    return 7;
  }
  return ((int(__cdecl *)(int))g_magvid_fns[15])(avi);
}

// FUNCTION: STATWIN 0x10003b24
extern "C" int __cdecl link_vids(int avi, int linked_avi)
{
  if ((g_video_dll_status == 0) || (g_video_dll_status == 2))
  {
    return 7;
  }
  return ((int(__cdecl *)(int, int))g_magvid_fns[16])(avi, linked_avi);
}

// FUNCTION: STATWIN 0x10003b69
extern "C" int __cdecl set_vid_thread_priority(int avi, int priority)
{
  if ((g_video_dll_status == 0) || (g_video_dll_status == 2))
  {
    return 7;
  }
  return ((int(__cdecl *)(int, int))g_magvid_fns[17])(avi, priority);
}

// FUNCTION: STATWIN 0x100053b2
int StatWinState::apply_progress(StatWinData *data, int flags)
{
  typedef int (DibState::*DibStateLoadBitmapFn)(HWND hwnd, char *path, uint bit_count);
  DibState *new_dib;
  char path[256];
  DibState unused_dib;
  int color_count;

  this->cached_data = (StatWinData *)operator new(sizeof(StatWinData));
  this->field_04 = flags;
  this->field_04 = 2;
  this->field_1c = 0;
  new_dib = new DibState();
  this->field_0c = new_dib;
  memcpy(this->cached_data, data, sizeof(StatWinData));
  join_paths(path, g_statwin_asset_dir, g_wizard_background_bitmap_name);
  if ((this->field_0c->*(*(DibStateLoadBitmapFn *)this->field_0c->vtable))(0, path, 0x18) == 0)
  {
    return 6;
  }

  this->prepare_missing_masks();
  color_count = dib_state_get_color_count(this->field_0c);
  if (color_count == 0)
  {
    this->cached_bitmap_info = (byte *)operator new(0x2c);
  }
  else
  {
    this->cached_bitmap_info = (byte *)operator new(color_count * 4 + 0x28);
    if (this->cached_bitmap_info != 0)
    {
      memcpy(this->cached_bitmap_info, this->field_0c->get_header(), 0x28);
      memcpy(this->cached_bitmap_info + 0x28, (byte *)this->field_0c->get_header() + 0x28, color_count << 2);
    }
  }
  memcpy(this->cached_bitmap_info, this->field_0c->get_header(), 0x2c);

  switch (flags)
  {
  case 0:
    this->draw_initial_town_counts(data);
    this->draw_initial_sprites(data);
    this->draw_initial_progress_totals(data);
    this->draw_initial_duel_wins();
    break;

  case 1:
    return 0;

  case 2:
    if (this->cached_data->highlighted_wizard_color > 4)
    {
      return 0;
    }
    this->draw_initial_sprites(data);
    this->draw_initial_town_counts(data);
    this->draw_initial_progress_totals(data);
    this->draw_initial_duel_wins();
    break;

  case 3:
    return 0;

  default:
    this->field_04 = 0;
    this->draw_initial_town_counts(data);
    this->draw_initial_progress_totals(data);
    this->draw_initial_duel_wins();
    break;
  }

  this->write_cache();
  this->has_window = 1;
  if (this->field_0c != 0)
  {
    delete this->field_0c;
  }
  this->field_0c = 0;
  operator delete(this->cached_data);
  this->cached_data = 0;
  if (this->field_10 != 0)
  {
    delete this->field_10;
  }
  this->field_10 = 0;
  return 0;
}

// FUNCTION: STATWIN 0x1000250e
static char __cdecl find_cdrom_drive_for_asset(char *path)
{
  FILE *file;
  char probe_path[256] = "a:\\";

  for (; probe_path[0] <= 'z'; probe_path[0]++)
  {
    if (GetDriveTypeA(probe_path) == DRIVE_CDROM)
    {
      strcat(probe_path, path);
      file = fopen(probe_path, "rb");
      if (file != NULL)
      {
        fclose(file);
        return probe_path[0];
      }
      else
      {
        probe_path[2] = 0;
      }
    }
  }
  return 0;
}

// FUNCTION: STATWIN 0x100025d8
static char __cdecl get_status_asset_drive(void)
{
  char current_directory[256];

  if (g_asset_drive_initialized == 0)
  {
    g_status_flag_byte = find_cdrom_drive_for_asset("statwin\\water.avi");
    g_asset_drive_initialized = 1;
  }

  if (g_use_cdrom_asset_drive == 0)
  {
    _getcwd(current_directory, sizeof(current_directory));
    return current_directory[0];
  }
  else
  {
    return g_status_flag_byte;
  }
}

#pragma intrinsic(memcpy)
#pragma intrinsic(memset)

// FUNCTION: STATWIN 0x1000264e
static int __cdecl reset_status_assets(void)
{
  struct
  {
    FILE *file;
    char probe_path[20];
    int padding[3];
    char current_directory[256];
    int tail[3];
  } s;

  memcpy(&s.probe_path, "a:statwin\\water.avi", 20);
  memset(&s.padding, 0, 280);

  _getcwd(s.current_directory, 0x100);
  s.probe_path[0] = s.current_directory[0];
  s.file = fopen(s.probe_path, "rb");
  if (s.file != NULL)
  {
    fclose(s.file);
    g_use_cdrom_asset_drive = 0;
    g_status_flag_byte = s.probe_path[0];
    return 0;
  }
  else
  {
    g_use_cdrom_asset_drive = 1;
    get_status_asset_drive();
    return 1;
  }
}

#pragma function(memcpy)
#pragma function(memset)

// FUNCTION: STATWIN 0x10003fa5
static int __cdecl bitmap_header_color_count(BITMAPINFOHEADER *header)
{
  struct
  {
    int color_count;
    int max_color_count;
    BITMAPINFOHEADER *header_again;
    int bit_count;
    BITMAPINFOHEADER *header;
  } s;

  s.header = header;
  s.header_again = s.header;

  if (bitmap_header_has_color_table(s.header) != 0)
  {
    s.bit_count = s.header->biBitCount;
  }
  else
  {
    s.bit_count = ((BITMAPCOREHEADER *)s.header_again)->bcBitCount;
  }

  switch (s.bit_count)
  {
  case 1:
    s.color_count = 2;
    break;
  case 4:
    s.color_count = 16;
    break;
  case 8:
    s.color_count = 256;
    break;
  default:
    s.color_count = 0;
    break;
  }

  if ((bitmap_header_has_color_table(s.header) != 0) && (s.header->biClrUsed != 0))
  {
    s.color_count = s.header->biClrUsed;
  }

  s.max_color_count = 0;
  switch (s.bit_count)
  {
  case 1:
    s.max_color_count = 2;
    break;
  case 4:
    s.max_color_count = 16;
    break;
  case 8:
    s.max_color_count = 256;
    break;
  default:
    s.max_color_count = 0;
    break;
  }

  if ((s.max_color_count != 0) && (s.max_color_count < s.color_count))
  {
    s.color_count = s.max_color_count;
  }
  if (s.bit_count > 8)
  {
    s.color_count = 0;
  }
  return s.color_count;
}

// FUNCTION: STATWIN 0x1000411c
extern "C" int __cdecl bitmap_header_has_color_table(BITMAPINFOHEADER *header)
{
  if (header->biSize == 12)
  {
    return 0;
  }
  return 1;
}

#define RGBQUAD_TO_RGB555(rgbq)             \
  ((USHORT)((((rgbq).rgbRed >> 3) << 10) |  \
            (((rgbq).rgbGreen >> 3) << 5) | \
            ((rgbq).rgbBlue >> 3)))

// FUNCTION: STATWIN 0x10004144
int DibState_t::load_bitmap(HWND hwnd, char *path, uint bit_count)
{
  struct
  {
    RGBQUAD *bmiColors;             // ebp - 0x78
    byte *dest24;                   // ebp - 0x74
    int row;                        // ebp - 0x70
    int col;                        // ebp - 0x6c
    byte *source;                   // ebp - 0x68
    byte *dest16;                   // ebp - 0x64
    DWORD read_file_err;            // ebp - 0x60
    BITMAPINFOHEADER source_header; // ebp - 0x5c
    int color_count;                // ebp - 0x34
    size_t bits_size;               // ebp - 0x30
    size_t header_size;             // ebp - 0x2c
    HANDLE file;                    // ebp - 0x28
    DWORD bytes_read;               // ebp - 0x24
    BITMAPINFO *new_header;         // ebp - 0x20
    byte *bits;                     // ebp - 0x1c
    size_t palette_size;            // ebp - 0x18
    BITMAPFILEHEADER file_header;   // ebp - 0x14
    byte *original_bits;            // ebp - 0x4
  } s;

  s.new_header = 0;
  s.bits = 0;
  s.original_bits = 0;
  s.file = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL);
  if (s.file == INVALID_HANDLE_VALUE)
  {
    goto cleanup;
  }
  if ((ReadFile(s.file, &s.file_header, sizeof(BITMAPFILEHEADER), &s.bytes_read, NULL) == 0) || (s.bytes_read != sizeof(BITMAPFILEHEADER)))
  {
    s.read_file_err = GetLastError();
    CloseHandle(s.file);
    goto cleanup;
  }

  if (s.file_header.bfType != 0x4d42)
  {
    CloseHandle(s.file);
    goto cleanup;
  }

  if ((ReadFile(s.file, &s.source_header, sizeof(BITMAPINFOHEADER), &s.bytes_read, NULL) == 0) || (s.bytes_read != sizeof(BITMAPINFOHEADER)))
  {
    CloseHandle(s.file);
    goto cleanup;
  }

  if (s.source_header.biSize != sizeof(BITMAPINFOHEADER))
  {
    CloseHandle(s.file);
    MessageBoxA(hwnd, "Not a Windows DIB.", "KPlay error", MB_ICONEXCLAMATION);
    goto cleanup;
  }

  s.color_count = bitmap_header_color_count(&s.source_header);
  s.palette_size = s.color_count << 2;
  s.header_size = 0x428;
  s.bits_size = s.file_header.bfSize - s.file_header.bfOffBits;
  s.new_header = (BITMAPINFO *)malloc(s.header_size);
  if (s.new_header == 0)
  {
    CloseHandle(s.file);
    goto cleanup;
  }

  memset(s.new_header, 0, s.header_size);
  memcpy(s.new_header, &s.source_header, sizeof(BITMAPINFOHEADER));
  s.new_header->bmiHeader.biBitCount = (WORD)bit_count;
  if ((s.color_count != 0) && ((ReadFile(s.file, &s.new_header->bmiColors, s.palette_size, &s.bytes_read, NULL) == 0) || (s.palette_size != s.bytes_read)))
  {
    CloseHandle(s.file);
    goto cleanup;
  }

  s.bits = (byte *)malloc(s.bits_size);
  s.original_bits = s.bits;
  if (s.bits == 0)
  {
    CloseHandle(s.file);
    goto cleanup;
  }

  if ((ReadFile(s.file, s.bits, s.bits_size, &s.bytes_read, NULL) == 0) || (s.bits_size != s.bytes_read))
  {
    goto cleanup;
  }

  if (s.source_header.biBitCount != bit_count)
  {
    s.bits_size = ((((int)(s.source_header.biWidth * bit_count) / 8) + 3) & ~3) * s.source_header.biHeight;
    s.bits = (byte *)malloc(s.bits_size);
    switch (bit_count)
    {
    case 0x18:
      s.source = s.original_bits;
      s.dest24 = s.bits;
      s.bmiColors = s.new_header->bmiColors;
      for (s.row = 0; s.row < s.source_header.biHeight; s.row++)
      {
        for (s.col = 0; s.col < s.source_header.biWidth; s.col++)
        {
          s.dest24[2] = s.bmiColors[(uint)(byte)*s.source].rgbRed;
          s.dest24[1] = s.bmiColors[(uint)(byte)*s.source].rgbGreen;
          s.dest24[0] = s.bmiColors[(uint)(byte)*s.source].rgbBlue;
          s.source++;
          s.dest24 += 3;
        }
        s.source += ((s.source_header.biWidth + 3U) & 0xfffffffc) - s.source_header.biWidth;
        s.dest24 += (((int)(s.source_header.biWidth * bit_count) / 8) + 3U & 0xfffffffc) -
                    ((int)(s.source_header.biWidth * bit_count) / 8);
      }
      break;
    case 0x10:
      s.source = s.original_bits;
      s.dest16 = s.bits;
      s.bmiColors = s.new_header->bmiColors;
      for (s.row = 0; s.row < s.source_header.biHeight; s.row++)
      {
        for (s.col = 0; s.col < s.source_header.biWidth; s.col++)
        {
          *(unsigned short *)s.dest16 = RGBQUAD_TO_RGB555(s.bmiColors[(uint)(byte)*s.source]);
          s.source++;
          s.dest16++;
        }
        s.source += ((s.source_header.biWidth + 3U) & 0xfffffffc) - s.source_header.biWidth;
        s.dest16 += (((int)(s.source_header.biWidth * bit_count) / 8) + 3U & 0xfffffffc) -
                    ((int)(s.source_header.biWidth * bit_count) / 8);
      }
      break;
    }
    free(s.original_bits);
    s.source_header.biBitCount = (WORD)bit_count;
  }
  if (this->header != 0)
  {
    free(this->header);
  }
  this->header = &s.new_header->bmiHeader;
  if (this->bits != 0)
  {
    free(this->bits);
  }
  this->bits = s.bits;
  this->owns_bits = 1;
  CloseHandle(s.file);
  return 1;

cleanup:
  if (s.new_header != 0)
  {
    free(s.new_header);
  }
  if (s.bits != 0)
  {
    free(s.bits);
  }
  return 0;
}

// FUNCTION: STATWIN 0x100046ba
int DibState_t::save_bitmap(char *path)
{
  struct
  {
    DWORD bytes_written;
    void *bits;
    int color_count;
    BITMAPFILEHEADER file_header;
    BITMAPINFOHEADER *header;
    DWORD bits_size;
    HANDLE file;
  } s;

  s.bits_size = 0;
  s.color_count = 0;
  s.file = CreateFileA(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (s.file == INVALID_HANDLE_VALUE)
  {
    return 2;
  }

  s.file_header.bfType = 0x4d42;
  s.file_header.bfReserved1 = 0;
  s.file_header.bfReserved2 = 0;
  s.color_count = dib_state_get_color_count(this);
  s.file_header.bfOffBits = s.color_count * 4 + 0x36;
  s.header = this->get_header();
  s.bits_size = s.header->biWidth * s.header->biHeight * ((dib_state_get_bit_count(this) + 7) >> 3);
  s.file_header.bfSize = s.file_header.bfOffBits + s.bits_size;
  WriteFile(s.file, &s.file_header, sizeof(BITMAPFILEHEADER), &s.bytes_written, NULL);
  s.header->biSizeImage = s.bits_size;
  WriteFile(s.file, s.header, sizeof(BITMAPINFOHEADER), &s.bytes_written, NULL);
  if (s.color_count != 0)
  {
    WriteFile(s.file, dib_state_get_palette(this), s.color_count << 2, &s.bytes_written, NULL);
  }
  s.bits = this->get_bits();
  WriteFile(s.file, s.bits, s.bits_size, &s.bytes_written, NULL);
  CloseHandle(s.file);
  return 0;
}

// FUNCTION: STATWIN 0x10004818
extern "C" int __fastcall dib_state_get_color_count(DibState *state)
{
  return bitmap_header_color_count(state->header);
}

// FUNCTION: STATWIN 0x10005030
extern "C" void *__fastcall dib_state_get_palette(DibState *state)
{
  return (state->header->biBitCount > 8) ? 0 : (char *)state->header + sizeof(BITMAPINFOHEADER);
}

// FUNCTION: STATWIN 0x100050b0
extern "C" int __fastcall dib_state_get_width(DibState *state)
{
  return state->header->biWidth;
}

// FUNCTION: STATWIN 0x100050d0
extern "C" int __fastcall dib_state_get_height(DibState *state)
{
  return state->header->biHeight;
}

// FUNCTION: STATWIN 0x100050f0
extern "C" int __fastcall dib_state_get_bit_count(DibState *state)
{
  return state->header->biBitCount;
}

// FUNCTION: STATWIN 0x10005120
extern "C" void __fastcall dib_state_release_field_10(DibState *state)
{
  void *delete_ptr;
  void *field_10;

  if (state->field_10 != 0)
  {
    delete_ptr = state->field_10;
    field_10 = delete_ptr;
    operator delete(field_10);
  }
}

// FUNCTION: STATWIN 0x10005160
extern "C" uint __fastcall dib_state_get_stride(DibState *state)
{
  return ((state->header->biBitCount * state->header->biWidth) / 8 + 3) & ~3;
}

// FUNCTION: STATWIN 0x100051cf
extern "C" void __cdecl join_paths(char *out_path, char *base_path, char *filename)
{
  strcpy(out_path, base_path);
  strcat(out_path, filename);
}

// FUNCTION: STATWIN 0x10002ce0
void DibState_t::set_field_10_value(int value)
{
  this->field_10 = operator new(4);
  if (this->field_10 != 0)
  {
    *(int *)this->field_10 = value;
  }
}

// FUNCTION: STATWIN 0x10002d60
BITMAPINFOHEADER *DibState_t::get_header(void)
{
  return this->header;
}

// FUNCTION: STATWIN 0x10002d80
void *DibState_t::get_bits(void)
{
  return this->bits;
}

// FUNCTION: STATWIN 0x10004a9f
void *DibState_t::get_pixel_address(int x, int y)
{
  uint stride;

  if ((x >= dib_state_get_width(this)) || (y >= dib_state_get_height(this)))
  {
    return 0;
  }

  stride = dib_state_get_stride(this);
  return (byte *)this->bits +
         (dib_state_get_height(this) - y - 1) * stride +
         ((this->header->biBitCount * x / 8));
}

// FUNCTION: STATWIN 0x10003bf0
DibState_t::DibState_t()
{
  this->vtable = &g_dib_state_vtable;
  this->header = 0;
  this->bits = 0;
  this->owns_bits = 1;
  this->field_10 = 0;
  this->field_14 = 0;
}

// FUNCTION: STATWIN 0x10003c44
DibState_t::~DibState_t()
{
  void *delete_ptr;
  void *field_10;

  this->vtable = &g_dib_state_vtable;
  if (this->header != 0)
  {
    free(this->header);
  }
  if ((this->owns_bits != 0) && (this->bits != 0))
  {
    free(this->bits);
  }
  if (this->field_10 != 0)
  {
    delete_ptr = this->field_10;
    field_10 = delete_ptr;
    operator delete(field_10);
  }
}

// FUNCTION: STATWIN 0x10005321
StatWinState::~StatWinState()
{
  g_current_statwin_state = 0;
  if (this->cached_bitmap_info != 0)
  {
    delete this->cached_bitmap_info;
  }
  if (this->field_0c != 0)
  {
    delete this->field_0c;
  }
  this->field_0c = 0;
  this->cached_bitmap_info = 0;
}

// FUNCTION: STATWIN 0x10005070
int DibState_t::get_width(void)
{
  return dib_state_get_width(this);
}

// FUNCTION: STATWIN 0x10005090
int DibState_t::get_height(void)
{
  return dib_state_get_height(this);
}

// FUNCTION: STATWIN 0x10004b22
void DibState_t::get_bounds_rect(RECT *rect)
{
  rect->top = 0;
  rect->left = 0;
  rect->bottom = dib_state_get_height(this);
  rect->right = dib_state_get_width(this);
}

// FUNCTION: STATWIN 0x10004b69
void DibState_t::blit_to(DibState *dest, int dest_x, int dest_y, int width, int height, int source_x, int source_y)
{
  struct
  {
    int memmove_width;
    uint color;
    int pixel_offset;
    int color_byte;
    int bytes_per_pixel;
    int pixel_index;
    uint color_mask;
    int row_width_bytes;
    int byte_index;
    void *source_line;
    void *dest_line;
    uint dest_stride;
    uint source_stride;
  } s;

  if (source_x < 0)
  {
    width += source_x;
    dest_x -= source_x;
    source_x = 0;
  }
  else
  {
    if (dib_state_get_width(this) - width < source_x)
    {
      if (dib_state_get_width(this) > source_x)
      {
        width = dib_state_get_width(this) - source_x;
      }
      else
      {
        return;
      }
    }
  }
  if (dest_x < 0)
  {
    width += dest_x;
    source_x -= dest_x;
    dest_x = 0;
  }
  else if ((0 < width) && (dib_state_get_width(dest) - width < dest_x))
  {
    width = dib_state_get_width(dest) - dest_x;
  }

  if (source_y < 0)
  {
    height += source_y;
    dest_y -= source_y;
    source_y = 0;
  }
  else
  {
    if (dib_state_get_height(this) - height < source_y)
    {
      if (dib_state_get_height(this) > source_y)
      {
        height = dib_state_get_height(this) - source_y;
      }
      else
      {
        return;
      }
    }
  }
  if (dest_y < 0)
  {
    height += dest_y;
    source_y -= dest_y;
    dest_y = 0;
  }
  else if ((0 < height) && (dib_state_get_height(dest) - height < dest_y))
  {
    height = dib_state_get_height(dest) - dest_y;
  }

  if ((width <= 0) || (height <= 0))
  {
    return;
  }

  if (dib_state_get_height(this) - source_y <= height)
  {
    height = dib_state_get_height(this) - source_y;
  }
  else
  {
  }

  if (dib_state_get_height(dest) - dest_y <= height)
  {
    height = dib_state_get_height(dest) - dest_y;
  }
  else
  {
  }

  s.source_line = this->get_pixel_address(source_x, source_y + height - 1);
  s.dest_line = dest->get_pixel_address(dest_x, dest_y + height - 1);
  s.source_stride = dib_state_get_stride(this);
  s.dest_stride = dib_state_get_stride(dest);

  if (dib_state_get_width(this) - source_x <= width)
  {
    width = dib_state_get_width(this) - source_x;
  }
  else
  {
  }

  if (dib_state_get_width(dest) - dest_x <= width)
  {
    width = dib_state_get_width(dest) - dest_x;
  }
  else
  {
  }

  if (width <= 0)
  {
    return;
  }

  if (dib_state_get_bit_count(this) <= 8)
  {
    s.row_width_bytes = (this->header->biBitCount * width) / 8;
    while (height--)
    {
      for (s.byte_index = 0; s.byte_index < s.row_width_bytes; s.byte_index++)
      {
        if ((this->field_10 != 0) && ((uint)((byte *)s.source_line)[s.byte_index] == *(uint *)this->field_10))
        {
        }
        else
        {
          ((byte *)s.dest_line)[s.byte_index] = ((byte *)s.source_line)[s.byte_index];
        }
      }
      s.source_line = (byte *)s.source_line + s.source_stride;
      s.dest_line = (byte *)s.dest_line + s.dest_stride;
    }
  }
  else
  {
    s.bytes_per_pixel = this->header->biBitCount >> 3;
    s.color_mask = 0xffffff;
    if (this->field_10 != 0)
    {
      while (height--)
      {
        for (s.pixel_index = 0, s.pixel_offset = 0; width > s.pixel_index; s.pixel_index++, s.pixel_offset += s.bytes_per_pixel)
        {
          s.color = *(uint *)((byte *)s.source_line + s.pixel_offset);
          s.color &= s.color_mask;
          if (*(uint *)this->field_10 == s.color)
          {
          }
          else
          {
            for (s.color_byte = 0; s.bytes_per_pixel > s.color_byte; s.color_byte++)
            {
              ((byte *)s.dest_line)[s.pixel_offset + s.color_byte] = ((byte *)&s.color)[s.color_byte];
            }
          }
        }
        s.source_line = (byte *)s.source_line + s.source_stride;
        s.dest_line = (byte *)s.dest_line + s.dest_stride;
      }
    }
    else
    {
      s.memmove_width = width * s.bytes_per_pixel;
      while (height--)
      {
        memmove(s.dest_line, s.source_line, s.memmove_width);
        s.source_line = (byte *)s.source_line + s.source_stride;
        s.dest_line = (byte *)s.dest_line + s.dest_stride;
      }
    }
  }
}

// FUNCTION: STATWIN 0x10003cd2
int DibState_t::create(int width, int height, int bit_count)
{
  struct
  {
    int palette_index;
    byte *palette;
    size_t bits_size;
    BITMAPINFOHEADER *new_header;
  } s;

  if (this->header != 0)
  {
    free(this->header);
  }
  if (this->bits != 0)
  {
    free(this->bits);
  }
  if (bit_count == 8)
  {
    this->header = (BITMAPINFOHEADER *)malloc(0x428);
  }
  else
  {
    this->header = (BITMAPINFOHEADER *)malloc(0x28);
  }
  if (this->header == 0)
  {
    return 0;
  }

  s.bits_size = (((bit_count * width / 8) + 5) & ~3) * height;
  this->bits = malloc(s.bits_size);
  if (this->bits == 0)
  {
    free(this->header);
    this->header = 0;
    return 0;
  }

  s.new_header = this->header;
  s.new_header->biSize = 0x28;
  s.new_header->biWidth = width;
  s.new_header->biHeight = height;
  s.new_header->biPlanes = 1;
  s.new_header->biBitCount = (short)bit_count;
  s.new_header->biCompression = 0;
  s.new_header->biSizeImage = 0;
  s.new_header->biXPelsPerMeter = 0;
  s.new_header->biYPelsPerMeter = 0;
  s.new_header->biClrUsed = 0;
  s.new_header->biClrImportant = 0;
  if (bit_count <= 8)
  {
    s.palette = (byte *)dib_state_get_palette(this);
    for (s.palette_index = 0; s.palette_index < 0x100; s.palette_index++)
    {
      s.palette[2] = (byte)s.palette_index;
      s.palette[1] = s.palette[2];
      s.palette[0] = s.palette[1];
      s.palette[3] = 0;
      s.palette += 4;
    }
  }
  memset(this->bits, 0, s.bits_size);
  return 1;
}

// FUNCTION: STATWIN 0x10003eb8
int DibState_t::attach_external_bits(BITMAPINFOHEADER *source_header, void *source_bits)
{
  if (this->header != 0)
  {
    free(this->header);
  }
  if (source_header->biBitCount == 8)
  {
    this->header = (BITMAPINFOHEADER *)malloc(0x428);
  }
  else
  {
    this->header = (BITMAPINFOHEADER *)malloc(0x28);
  }
  if (this->header == 0)
  {
    return 0;
  }

  memcpy(this->header, source_header, bitmap_header_color_count(source_header) * 4 + 0x28);
  if ((this->owns_bits != 0) && (this->bits != 0))
  {
    free(this->bits);
  }
  this->bits = source_bits;
  this->owns_bits = 0;
  return 1;
}

// FUNCTION: STATWIN 0x10005caa
int StatWinState::prepare_resources(void)
{
  typedef void (DibState::*DibStateGetBoundsRectFn)(RECT *rect);
  DibState cache_dib;
  char cache_path[256];
  int screen_width;
  BITMAPINFOHEADER header;
  void *bits;
  HANDLE file;
  int screen_height;
  BITMAPINFOHEADER *source_header;
  void *palette;
  DWORD bytes_read;
  RECT dest_bounds;
  RECT source_bounds;

  bits = 0;
  palette = 0;
  if (this->field_0c != 0)
  {
    delete this->field_0c;
    this->field_0c = 0;
  }
  this->field_0c = new DibState();
  if (this->cached_data != 0)
  {
    operator delete(this->cached_data);
    this->cached_data = 0;
  }
  this->cached_data = (StatWinData *)operator new(sizeof(StatWinData));
  this->field_1c = 0;
  screen_width = GetSystemMetrics(SM_CXSCREEN);
  screen_height = GetSystemMetrics(SM_CYSCREEN);
  this->field_0c->create(screen_width, screen_height, 0x18);
  join_paths(cache_path, g_statwin_asset_dir, g_stats_cache_bitmap_name);
  file = CreateFileA(cache_path, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
  if (file == INVALID_HANDLE_VALUE)
  {
    if (this->field_0c != 0)
    {
      delete this->field_0c;
      this->field_0c = 0;
    }
    operator delete(this->cached_data);
    this->cached_data = 0;
    return 6;
  }

  if (ReadFile(file, this->cached_data, sizeof(StatWinData), &bytes_read, NULL) == 0)
  {
    CloseHandle(file);
    DeleteFileA(cache_path);
    return 6;
  }
  if (ReadFile(file, &header, sizeof(BITMAPINFOHEADER), &bytes_read, NULL) == 0)
  {
    CloseHandle(file);
    DeleteFileA(cache_path);
    return 6;
  }

  if (header.biBitCount < 9)
  {
    source_header = (BITMAPINFOHEADER *)operator new(0x428);
    palette = (byte *)source_header + 0x28;
    if (this->cached_bitmap_info == 0)
    {
      this->cached_bitmap_info = (byte *)operator new(0x428);
    }
    if (ReadFile(file, palette, 0x400, &bytes_read, NULL) == 0)
    {
      CloseHandle(file);
      DeleteFileA(cache_path);
      return 6;
    }
    memcpy(this->cached_bitmap_info + 0x28, palette, bytes_read);
  }
  else
  {
    source_header = (BITMAPINFOHEADER *)operator new(0x28);
    if (this->cached_bitmap_info == 0)
    {
      this->cached_bitmap_info = (byte *)operator new(0x28);
    }
  }
  memcpy(source_header, &header, 0x28);
  memcpy(this->cached_bitmap_info, &header, 0x28);
  bits = operator new(header.biSizeImage);
  if (ReadFile(file, bits, header.biSizeImage, &bytes_read, NULL) == 0)
  {
    CloseHandle(file);
    DeleteFileA(cache_path);
    operator delete(source_header);
    operator delete(bits);
    return 6;
  }

  cache_dib.attach_external_bits(source_header, bits);
  cache_dib.get_bounds_rect(&source_bounds);
  (this->field_0c->*(((DibStateGetBoundsRectFn *)this->field_0c->vtable)[5]))(&dest_bounds);
  if ((dest_bounds.right == source_bounds.right) && (dest_bounds.bottom == source_bounds.bottom))
  {
    cache_dib.blit_to(this->field_0c, 0, 0, source_bounds.right, source_bounds.bottom, 0, 0);
    this->cache_x = 0;
    this->cache_y = 0;
  }
  else
  {
    if ((dest_bounds.right <= source_bounds.right) || (dest_bounds.bottom <= source_bounds.bottom))
    {
      CloseHandle(file);
      DeleteFileA(cache_path);
      return 6;
    }
    this->cache_x = (dest_bounds.right - source_bounds.right) / 2;
    this->cache_y = (dest_bounds.bottom - source_bounds.bottom) / 2;
    cache_dib.blit_to(this->field_0c, this->cache_x, this->cache_y, source_bounds.right, source_bounds.bottom, 0, 0);
  }
  operator delete(source_header);
  operator delete(bits);
  CloseHandle(file);
  return 0;
}

// FUNCTION: STATWIN 0x100051ff
int StatWinState::choose_mode(int *mode, StatWinData *data, int *selection)
{
  int color;

  if ((*mode == 2) || (*mode == 1))
  {
    return *mode;
  }
  else
  {
    if (*mode == 3)
    {
      for (color = 0; color < 5; color++)
      {
        if (this->cached_data->town_count_by_color[color] != data->town_count_by_color[color])
        {
          *mode = 3;
          *selection = color;
          return 1;
        }
      }
    }
  }
  return 0;
}

// FUNCTION: STATWIN 0x1000586f
int StatWinState::update(StatWinData *data, int mode)
{
  uint highlighted;
  int color;

  highlighted = data->highlighted_wizard_color;

  switch (mode)
  {
  case 1:
    if (data->progress_by_color[highlighted] == 0)
    {
      this->draw_missing_color_progress(highlighted);
    }
    else
    {
      this->draw_color_progress(highlighted);
    }

    break;

  default:
    break;
  }

  for (color = 0; color < 5; color++)
  {
    if (data->town_count_by_color[color] < this->cached_data->town_count_by_color[color])
    {
      this->draw_missing_color_progress(color);
      if (data->progress_by_color[color] != 0)
      {
        this->draw_color_progress(color);
      }
      this->cached_data->progress_by_color[color] = -1;
      this->cached_data->duel_wins_by_color[color] = -1;
      break;
    }
  }

  this->refresh_town_counts(data);
  this->refresh_progress_totals(data);
  this->refresh_duel_wins(data);
  return 0;
}

// FUNCTION: STATWIN 0x10001cb9
static void __cdecl show_status_window(void)
{
  struct
  {
    char path[256];
    int screen_height;
    MSG msg;
    HWND hwnd;
    int unused;
    int screen_width;
    HWND parent;
  } s;

  g_help_dialog_enabled = 1;
  if (g_sound_available != 0)
  {
    s.parent = get_sound_hwnd();
  }
  else
  {
    s.parent = 0;
  }

  s.screen_width = GetSystemMetrics(SM_CXSCREEN);
  s.screen_height = GetSystemMetrics(SM_CYSCREEN);
  s.hwnd = CreateWindowExA(0, g_statwin_class_name_ptr, 0, WS_POPUP | WS_VISIBLE, 0, 0, s.screen_width, s.screen_height, s.parent, 0, g_statwin_instance, 0);
  SetFocus(s.hwnd);
  SetForegroundWindow(s.hwnd);
  join_paths(s.path, g_status_sound_dir, g_status_sound_name);
  play_status_sound(s.path);
  ShowWindow(s.hwnd, SW_SHOWNORMAL);
  UpdateWindow(s.hwnd);
  g_video_stop_requested = 0;
  s.unused = 0;
  while (g_video_stop_requested == 0)
  {
    if (PeekMessageA(&s.msg, 0, 0, 0, PM_REMOVE) != 0)
    {
      TranslateMessage(&s.msg);
      DispatchMessageA(&s.msg);
    }
  }
  sound_stop(0xff);
  DestroyWindow(s.hwnd);
  g_help_dialog_enabled = 0;
}

// FUNCTION: STATWIN 0x1000175a
static void __cdecl show_world_magic_detail(StatWinData *data, int unused)
{
  typedef int (DibState::*DibStateLoadBitmapFn)(HWND hwnd, char *path, uint bit_count);
  typedef void (DibState::*DibStateBlitToFn)(DibState *dest, int dest_x, int dest_y, int width, int height, int source_x, int source_y);
  struct
  {
    int load_result;
    HWND hwnd;
    LPARAM lparam;
    WPARAM wparam;
    UINT msg;
    DibState *mask_dib_new;
    char path[256];
    DibState *mask_dib;
    uint color;
    AviPosition position;
    int hit_avi;
    int main_avi;
    DWORD priority_class;
    HANDLE process;
    int load_flags;
    DrawRect rect;
  } s;

  (void)unused;
  s.color = data->highlighted_wizard_color;
  s.load_flags = 5;
  join_paths(s.path, g_status_flag_ptr, g_detail_creature_avi_names[data->highlighted_creature_type]);
  if (load_avi(s.path, &s.main_avi, &s.position, 1) == 0)
  {
    join_paths(s.path, g_status_sound_dir, g_status_sound_name);
    set_vid_background_to_dib((int)g_status_state->field_0c, s.main_avi);
    set_vid_transparency(s.main_avi, 1);
    s.position.x = (short)(g_status_state->cache_x + g_detail_main_avi_position.x);
    s.position.y = (short)(g_detail_main_avi_position.y + g_status_state->cache_y);
    set_vid_pos(g_detail_main_avi, &s.position);
    s.hit_avi = s.main_avi;
    if (data->progress_by_color[s.color] != 0)
    {
      s.position.x = (short)g_detail_wizard_rects[s.color].x;
      s.position.y = (short)g_detail_wizard_rects[s.color].y;
      join_paths(s.path, g_status_flag_ptr, g_wizard_hit_avi_names[s.color]);
      if (load_avi(s.path, &s.hit_avi, &s.position, 5) == 0)
      {
        set_vid_background_to_dib((int)g_status_state->field_0c, s.hit_avi);
        set_vid_transparency(s.hit_avi, 1);
        s.position.x = (short)(g_status_state->cache_x + s.position.x);
        s.position.y = (short)(s.position.y + g_status_state->cache_y);
        set_vid_pos(s.hit_avi, &s.position);
      }
    }
    s.process = GetCurrentProcess();
    s.priority_class = GetPriorityClass(s.process);
    join_paths(s.path, g_status_sound_dir, g_status_sound_name);
    play_status_sound(s.path);
    draw_vid_background(s.main_avi);
    play_avi(s.main_avi);
    *(int *)g_magvid_delay_fn = 1;
    g_video_stop_requested = 0;
    while (vid_status(s.main_avi) != 0)
    {
      if (poll_statwin_messages() != 0)
      {
        stop_avi(s.main_avi);
        break;
      }
      Sleep(500);
    }
    *(int *)g_magvid_delay_fn = 0;
    draw_vid_background(s.main_avi);
    s.mask_dib_new = new DibState();
    s.mask_dib = s.mask_dib_new;
    s.mask_dib->set_field_10_value(0xff00);
    join_paths(s.path, g_statwin_asset_dir, g_color_mask_assets[s.color].filename);
    s.rect = g_color_mask_assets[s.color].rect;
    if ((s.mask_dib->*(*(DibStateLoadBitmapFn *)s.mask_dib->vtable))(0, s.path, 0x18) == 1)
    {
      (s.mask_dib->*(((DibStateBlitToFn *)s.mask_dib->vtable)[6]))(g_status_state->field_0c,
                                                                   g_status_state->cache_x + s.rect.x,
                                                                   g_status_state->cache_y + s.rect.y,
                                                                   s.rect.width,
                                                                   s.rect.height,
                                                                   0,
                                                                   0);
      if (s.mask_dib != 0)
      {
        delete s.mask_dib;
      }
      s.mask_dib = 0;
    }
    s.load_result = g_status_state->prepare_duel_foreground(&g_detail_wizard_rects[s.color], s.color);
    if (s.load_result == 0)
    {
      set_vid_foreground((int)g_status_state->field_10, s.hit_avi);
      g_status_state->field_10->set_field_10_value(0);
    }
    if (data->progress_by_color[s.color] != 0)
    {
      play_avi(s.hit_avi);
      g_video_stop_requested = 0;
      while (vid_status(s.hit_avi) != 0)
      {
        if (poll_statwin_messages() != 0)
        {
          stop_avi(s.hit_avi);
          break;
        }
        Sleep(0);
      }
    }
    g_status_state->update(data, 1);
    sound_stop(0xff);
    draw_vid_background(s.hit_avi);
    do
    {
      s.load_result = poll_statwin_messages();
    } while (s.load_result == 0);
    SetPriorityClass(s.process, s.priority_class);
    if (data->progress_by_color[s.color] != 0)
    {
      unload_avi(s.hit_avi);
    }
    unload_avi(s.main_avi);
    sound_unload(0xff);
    s.lparam = 0;
    s.wparam = 0;
    s.msg = WM_QUIT;
    s.hwnd = get_sound_hwnd();
    SendMessageA(s.hwnd, s.msg, s.wparam, s.lparam);
  }
}

// FUNCTION: STATWIN 0x100023a1
static int __cdecl load_centered_background(DibState *dest, char *path)
{
  DibState result;

  struct
  {
    int center_y;
    int center_x;
    int screen_height;
    int screen_width;
    RECT dest_bounds;
    RECT source_bounds;
  } s;

  s.screen_width = GetSystemMetrics(SM_CXSCREEN);
  s.screen_height = GetSystemMetrics(SM_CYSCREEN);
  dest->create(s.screen_width, s.screen_height, 0x18);

  if (result.load_bitmap(0, path, 0x18) == 0)
  {
    return 4;
  }

  result.get_bounds_rect(&s.source_bounds);
  dest->get_bounds_rect(&s.dest_bounds);
  if ((s.dest_bounds.right == s.source_bounds.right) && (s.dest_bounds.bottom == s.source_bounds.bottom))
  {
    result.blit_to(dest, 0, 0, s.source_bounds.right, s.source_bounds.bottom, 0, 0);
  }
  else if ((s.source_bounds.right < s.dest_bounds.right) && (s.source_bounds.bottom < s.dest_bounds.bottom))
  {
    s.center_x = (s.dest_bounds.right - s.source_bounds.right) / 2;
    s.center_y = (s.dest_bounds.bottom - s.source_bounds.bottom) / 2;
    result.blit_to(dest, s.center_x, s.center_y, s.source_bounds.right, s.source_bounds.bottom, 0, 0);
  }

  return 0;
}

// FUNCTION: STATWIN 0x100021ad
static int __cdecl show_color_missing_message(uint color)
{
  struct
  {
    char path[256];
    AviPosition position;
    int load_result;
    int avi;
  } s;
  DibState background;

  join_paths(s.path, g_statwin_asset_dir, g_missing_color_room_bitmap_names[color]);
  s.load_result = load_centered_background(&background, s.path);
  if (s.load_result != 0)
  {
    return s.load_result;
  }

  join_paths(s.path, g_status_flag_ptr, g_missing_color_avi_names[color]);
  if (load_avi(s.path, &s.avi, &s.position, 1) != 0)
  {
    return 4;
  }

  set_vid_background_to_dib((int)&background, s.avi);
  set_vid_transparency(s.avi, 0);
  s.position.x = (short)(g_missing_color_avi_rects[color].x + g_status_state->cache_x);
  s.position.y = (short)(g_missing_color_avi_rects[color].y + g_status_state->cache_y);
  set_vid_pos(s.avi, &s.position);
  draw_vid_background(s.avi);
  play_avi(s.avi);
  while (vid_status(s.avi) != 0)
  {
    pump_one_statwin_message();
    Sleep(500);
  }
  while (poll_statwin_messages() == 0)
  {
  }
  unload_avi(s.avi);
  return 0;
}

// FUNCTION: STATWIN 0x10002128
static void __cdecl play_status_sound(char *path)
{
  struct
  {
    Sound sound;
    HWND hwnd;
  } s;

  if (g_sound_available != 0)
  {
    s.hwnd = get_sound_hwnd();
    memset(&s.sound, 0, 0x20);
    s.sound.sampleRate = 0;
    s.sound.volume = 400;
    s.sound.pan = 0;
    s.sound.flags = (uint)s.sound.flags | 4;
    s.sound.flags = (uint)s.sound.flags | 1;
    sound_load(path, 0xff, &s.sound);
    sound_play(0xff, &s.sound);
  }
}

// FUNCTION: STATWIN 0x100014cd
extern "C" int __cdecl pump_one_statwin_message(void)
{
  MSG msg;

  if (PeekMessageA(&msg, 0, 0, 0, PM_REMOVE) != 0)
  {
    TranslateMessage(&msg);
    DispatchMessageA(&msg);
  }
  return 0;
}

// FUNCTION: STATWIN 0x10001510
extern "C" void __cdecl play_status_avi(int avi)
{
  play_avi(avi);
}

// FUNCTION: STATWIN 0x1000599a
void StatWinState::write_cache(void)
{
  WriteCacheLocals s;

  join_paths(s.cache_path, g_statwin_asset_dir, g_stats_cache_bitmap_name);
  s.file = CreateFileA(s.cache_path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
  if (s.file == INVALID_HANDLE_VALUE)
  {
    return;
  }

  if (WriteFile(s.file, this->cached_data, sizeof(StatWinData), &s.bytes_written, NULL) == 0)
  {
    CloseHandle(s.file);
    DeleteFileA(s.cache_path);
    return;
  }

  s.cache_dib.create(((BITMAPINFOHEADER *)this->cached_bitmap_info)->biWidth, ((BITMAPINFOHEADER *)this->cached_bitmap_info)->biHeight, 0x18);
  s.header = s.cache_dib.get_header();
  s.header->biSizeImage = ((BITMAPINFOHEADER *)this->cached_bitmap_info)->biSizeImage;
  if (WriteFile(s.file, s.header, s.header->biSize, &s.bytes_written, NULL) == 0)
  {
    CloseHandle(s.file);
    DeleteFileA(s.cache_path);
    return;
  }

  s.color_count = dib_state_get_color_count(this->field_0c);
  s.palette = dib_state_get_palette(this->field_0c);
  if (s.palette != 0)
  {
    if (WriteFile(s.file, s.palette, s.color_count << 2, &s.bytes_written, NULL) == 0)
    {
      CloseHandle(s.file);
      DeleteFileA(s.cache_path);
      return;
    }
  }

  this->field_0c->blit_to(&s.cache_dib, 0, 0, s.cache_dib.get_width(), s.cache_dib.get_height(), this->cache_x, this->cache_y);
  s.bits = s.cache_dib.get_bits();
  if (WriteFile(s.file, s.bits, s.header->biSizeImage, &s.bytes_written, NULL) == 0)
  {
    CloseHandle(s.file);
    DeleteFileA(s.cache_path);
    return;
  }

  CloseHandle(s.file);
}

// FUNCTION: STATWIN 0x100065c1
int StatWinState::release_cache(void)
{
  int result = 0;
  if (this->field_0c != 0)
  {
    delete this->field_0c;

    this->field_0c = 0;
  }
  if (this->cached_data != 0)
  {
    delete this->cached_data;

    this->cached_data = 0;
  }

  if (this->field_10 != 0)
  {
    delete this->field_10;

    this->field_10 = 0;
  }
  return 0;
}

// FUNCTION: STATWIN 0x100084f9
void StatWinState::prepare_missing_masks(void)
{
  typedef void (DibState::*DibStateBlitToFn)(DibState *dest, int dest_x, int dest_y, int width, int height, int source_x, int source_y);
  DibState *small_mask_dib;
  DibState *world_mask_dib;
  char path[256];
  int color;

  for (color = 0; color < 5; color++)
  {
    small_mask_dib = new DibState();
    small_mask_dib->create(g_small_missing_mask_assets[color].rect.width, g_small_missing_mask_assets[color].rect.height, 0x18);
    (this->field_0c->*(((DibStateBlitToFn *)this->field_0c->vtable)[6]))(small_mask_dib, 0, 0,
                                                                          g_small_missing_mask_assets[color].rect.width,
                                                                          g_small_missing_mask_assets[color].rect.height,
                                                                          g_small_missing_mask_assets[color].rect.x,
                                                                          g_small_missing_mask_assets[color].rect.y);
    join_paths(path, g_statwin_asset_dir, g_small_missing_mask_assets[color].filename);
    small_mask_dib->save_bitmap(path);
    if (small_mask_dib != 0)
    {
      delete small_mask_dib;
    }

    world_mask_dib = new DibState();
    world_mask_dib->create(g_world_missing_mask_assets[color].rect.width, g_world_missing_mask_assets[color].rect.height, 0x18);
    (this->field_0c->*(((DibStateBlitToFn *)this->field_0c->vtable)[6]))(world_mask_dib, 0, 0,
                                                                          g_world_missing_mask_assets[color].rect.width,
                                                                          g_world_missing_mask_assets[color].rect.height,
                                                                          g_world_missing_mask_assets[color].rect.x,
                                                                          g_world_missing_mask_assets[color].rect.y);
    join_paths(path, g_statwin_asset_dir, g_world_missing_mask_assets[color].filename);
    world_mask_dib->save_bitmap(path);
    if (world_mask_dib != 0)
    {
      delete world_mask_dib;
    }
  }
}

// FUNCTION: STATWIN 0x10007564
void StatWinState::draw_initial_town_counts(StatWinData *data)
{
  typedef int (DibState::*DibStateLoadBitmapFn)(HWND hwnd, char *path, uint bit_count);
  typedef void (DibState::*DibStateBlitToFn)(DibState *dest, int dest_x, int dest_y, int width, int height, int source_x, int source_y);
  DibState *mana_dib;
  char path[256];
  int pip_index;
  int color;
  byte town_count;

  for (color = 0; color < 5; color++)
  {
    if (data->progress_by_color[color] != 0)
    {
      mana_dib = new DibState();
      town_count = this->cached_data->town_count_by_color[color];
      if (town_count == 0)
      {
        if (mana_dib != 0)
        {
          delete mana_dib;
        }
      }
      else
      {
        join_paths(path, g_statwin_asset_dir, g_mana_base_assets[color].filename);
        if ((mana_dib->*(*(DibStateLoadBitmapFn *)mana_dib->vtable))(0, path, 0x18) == 0)
        {
          break;
        }
        pip_index = 0;
        for (town_count &= 0x1f; (pip_index < 5) && (town_count != 0); town_count--)
        {
          if (town_count != 0)
          {
            (mana_dib->*(((DibStateBlitToFn *)mana_dib->vtable)[6]))(this->field_0c,
                                                                     g_town_mana_source_rects[color][pip_index].x + g_mana_base_assets[color].rect.x,
                                                                     g_town_mana_source_rects[color][pip_index].y + g_mana_base_assets[color].rect.y,
                                                                     g_town_mana_source_rects[color][pip_index].width,
                                                                     g_town_mana_source_rects[color][pip_index].height,
                                                                     g_town_mana_source_rects[color][pip_index].x,
                                                                     g_town_mana_source_rects[color][pip_index].y);
          }
          pip_index++;
        }
        if (mana_dib != 0)
        {
          delete mana_dib;
        }
      }
    }
  }
}

// FUNCTION: STATWIN 0x10007833
void StatWinState::draw_initial_sprites(StatWinData *data)
{
  typedef int (DibState::*DibStateLoadBitmapFn)(HWND hwnd, char *path, uint bit_count);
  typedef void (DibState::*DibStateBlitToFn)(DibState *dest, int dest_x, int dest_y, int width, int height, int source_x, int source_y);
  DibState *sprite_dib;
  char path[256];
  DrawRect rect;
  int color;

  for (color = 0; color < 5; color++)
  {
    if (data->progress_by_color[color] != 0)
    {
      sprite_dib = new DibState();
      join_paths(path, g_statwin_asset_dir, g_color_sprite_assets[color].filename);
      if ((sprite_dib->*(*(DibStateLoadBitmapFn *)sprite_dib->vtable))(0, path, 0x18) == 0)
      {
        break;
      }
      rect = g_color_sprite_assets[color].rect;
      sprite_dib->set_field_10_value(0xff00);
      (sprite_dib->*(((DibStateBlitToFn *)sprite_dib->vtable)[6]))(this->field_0c, rect.x, rect.y, rect.width, rect.height, 0, 0);
      if (sprite_dib != 0)
      {
        delete sprite_dib;
      }
    }
  }
}

// FUNCTION: STATWIN 0x100066a3
void StatWinState::draw_initial_progress_totals(StatWinData *data)
{
  int color;

  if (this->cached_data != 0)
  {
    for (color = 0; color < 5; color++)
    {
      if (data->progress_by_color[color] != 0)
      {
        this->draw_progress_total(color);
      }
    }
  }
}

// FUNCTION: STATWIN 0x100072c2
int StatWinState::draw_initial_duel_wins(void)
{
  return this->refresh_duel_wins(this->cached_data);
}

// FUNCTION: STATWIN 0x1000882f
int StatWinState::prepare_duel_foreground(DrawRect *rect, uint color)
{
  if (this->field_10 != 0)
  {
    delete this->field_10;
  }
  this->field_10 = new DibState();
  this->field_10->create(rect->width, rect->height, 0x18);
  this->draw_progress_total(color);
  this->draw_town_count(color);
  return 0;
}

// FUNCTION: STATWIN 0x100071f4
extern "C" int __cdecl duel_wins_to_skull_index(int duel_wins)
{
  int skull_index;

  switch (duel_wins)
  {
  case 0:
    skull_index = -1;
    break;
  case 1:
    skull_index = 0;
    break;
  case 2:
    skull_index = 1;
    break;
  case 3:
    skull_index = 2;
    break;
  case 4:
    skull_index = 3;
    break;
  case 5:
  case 6:
  case 7:
    skull_index = 4;
    break;
  case 8:
  case 9:
  case 10:
  case 11:
    skull_index = 5;
    break;
  default:
    skull_index = 6;
    break;
  }
  return skull_index;
}

// FUNCTION: STATWIN 0x10007a7a
int StatWinState::draw_missing_color_progress(uint color)
{
  char path[256];
  DibState unused_dib;
  DibState color_mask_dib;

  DrawRect rect = g_color_mask_assets[color].rect;
  color_mask_dib.set_field_10_value(0xff00);
  join_paths(path, g_statwin_asset_dir, g_color_mask_assets[color].filename);
  if (color_mask_dib.load_bitmap(0, path, 0x18) == 0)
  {
    return 4;
  }

  color_mask_dib.blit_to(this->field_0c, this->cache_x + rect.x, this->cache_y + rect.y, rect.width, rect.height, 0, 0);

  DibState small_mask_dib;
  rect = g_small_missing_mask_assets[color].rect;
  join_paths(path, g_statwin_asset_dir, g_small_missing_mask_assets[color].filename);

  small_mask_dib.load_bitmap(0, path, 0x18);
  small_mask_dib.blit_to(this->field_0c, this->cache_x + rect.x, this->cache_y + rect.y, rect.width, rect.height, 0, 0);

  DibState world_mask_dib;
  rect = g_world_missing_mask_assets[color].rect;
  join_paths(path, g_statwin_asset_dir, g_world_missing_mask_assets[color].filename);

  world_mask_dib.load_bitmap(0, path, 0x18);
  world_mask_dib.blit_to(this->field_0c, this->cache_x + rect.x, this->cache_y + rect.y, rect.width, rect.height, 0, 0);

  return 0;
}

// FUNCTION: STATWIN 0x10008051
void StatWinState::draw_color_progress(uint color)
{
  typedef int (DibState::*DibStateLoadBitmapFn)(HWND hwnd, char *path, uint bit_count);
  typedef void (DibState::*DibStateBlitToFn)(DibState *dest, int dest_x, int dest_y, int width, int height, int source_x, int source_y);
  char path[256];
  DibState unused_dib;
  DibState sprite_dib;
  DibState *yellow_dib;
  DibState *red_dib;
  DrawRect rect;
  int hidden_height;

  rect = g_color_sprite_assets[color].rect;
  sprite_dib.set_field_10_value(0xff00);
  join_paths(path, g_statwin_asset_dir, g_color_sprite_assets[color].filename);
  if (sprite_dib.load_bitmap(0, path, 0x18) == 0)
  {
    return;
  }
  sprite_dib.blit_to(this->field_0c, this->cache_x + rect.x, this->cache_y + rect.y, rect.width, rect.height, 0, 0);

  yellow_dib = new DibState();
  yellow_dib->set_field_10_value(0);
  join_paths(path, g_statwin_asset_dir, g_progress_yellow_assets[color].filename);
  if ((yellow_dib->*(*(DibStateLoadBitmapFn *)yellow_dib->vtable))(0, path, 0x18) == 0)
  {
    return;
  }
  rect = g_progress_yellow_assets[color].rect;
  (yellow_dib->*(((DibStateBlitToFn *)yellow_dib->vtable)[6]))(this->field_0c, this->cache_x + rect.x, this->cache_y + rect.y, rect.width, rect.height, 0, 0);
  if (yellow_dib != 0)
  {
    delete yellow_dib;
  }

  red_dib = new DibState();
  red_dib->set_field_10_value(0);
  join_paths(path, g_statwin_asset_dir, g_progress_red_assets[color].filename);
  if ((red_dib->*(*(DibStateLoadBitmapFn *)red_dib->vtable))(0, path, 0x18) == 0)
  {
    return;
  }
  rect = g_progress_red_assets[color].rect;
  hidden_height = rect.height - (this->cached_data->progress_by_color[color] * rect.height) / 0x1e;
  (red_dib->*(((DibStateBlitToFn *)red_dib->vtable)[6]))(this->field_0c,
                                                         this->cache_x + rect.x,
                                                         this->cache_y + rect.y + hidden_height,
                                                         rect.width,
                                                         rect.height - hidden_height,
                                                         0,
                                                         hidden_height);
  if (red_dib != 0)
  {
    delete red_dib;
  }
}

// FUNCTION: STATWIN 0x10007d77
int StatWinState::draw_town_count(uint color)
{
  typedef int (DibState::*DibStateLoadBitmapFn)(HWND hwnd, char *path, uint bit_count);
  typedef void (DibState::*DibStateBlitToFn)(DibState *dest, int dest_x, int dest_y, int width, int height, int source_x, int source_y);
  DibState *mana_dib;
  char path[256];
  int pip_index;
  uint town_count;

  mana_dib = new DibState();
  town_count = this->cached_data->town_count_by_color[color];
  if (town_count == 0)
  {
    if (mana_dib != 0)
    {
      delete mana_dib;
    }
    return 7;
  }

  join_paths(path, g_statwin_asset_dir, g_mana_base_assets[color].filename);
  if ((mana_dib->*(*(DibStateLoadBitmapFn *)mana_dib->vtable))(0, path, 0x18) == 0)
  {
    if (mana_dib != 0)
    {
      delete mana_dib;
    }
    return 6;
  }

  pip_index = 0;
  for (town_count &= 0x1f; (pip_index < 5) && (0 < (int)town_count); town_count--)
  {
    (mana_dib->*(((DibStateBlitToFn *)mana_dib->vtable)[6]))(this->field_0c,
                                                             g_town_mana_source_rects[color][pip_index].x + g_mana_base_assets[color].rect.x + this->cache_x,
                                                             g_town_mana_source_rects[color][pip_index].y + g_mana_base_assets[color].rect.y + this->cache_y,
                                                             g_town_mana_source_rects[color][pip_index].width,
                                                             g_town_mana_source_rects[color][pip_index].height,
                                                             g_town_mana_source_rects[color][pip_index].x,
                                                             g_town_mana_source_rects[color][pip_index].y);
    pip_index++;
  }
  if (mana_dib != 0)
  {
    delete mana_dib;
  }
  return 0;
}

// FUNCTION: STATWIN 0x10008a26
int StatWinState::draw_progress_total(uint color)
{
  typedef int (DibState::*DibStateLoadBitmapFn)(HWND hwnd, char *path, uint bit_count);
  typedef void (DibState::*DibStateBlitToFn)(DibState *dest, int dest_x, int dest_y, int width, int height, int source_x, int source_y);
  struct
  {
    DibState *yellow_dib;
    DibState *red_dib;
    char path[256];
    int filled_height;
    int visible_height;
    DrawRect rect;
    int total_height;
  } s;

  s.yellow_dib = new DibState();
  s.yellow_dib->set_field_10_value(0);
  join_paths(s.path, g_statwin_asset_dir, g_progress_yellow_assets[color].filename);
  if ((s.yellow_dib->*(*(DibStateLoadBitmapFn *)s.yellow_dib->vtable))(0, s.path, 0x18) == 0)
  {
    if (s.yellow_dib != 0)
    {
      delete s.yellow_dib;
    }
    return 4;
  }
  s.rect = g_progress_yellow_assets[color].rect;
  (s.yellow_dib->*(((DibStateBlitToFn *)s.yellow_dib->vtable)[6]))(this->field_0c, this->cache_x + s.rect.x, this->cache_y + s.rect.y, s.rect.width, s.rect.height, 0, 0);
  if (s.yellow_dib != 0)
  {
    delete s.yellow_dib;
  }

  s.red_dib = new DibState();
  s.red_dib->set_field_10_value(0);
  join_paths(s.path, g_statwin_asset_dir, g_progress_red_assets[color].filename);
  if ((s.red_dib->*(*(DibStateLoadBitmapFn *)s.red_dib->vtable))(0, s.path, 0x18) == 0)
  {
    if (s.red_dib != 0)
    {
      delete s.red_dib;
    }
    return 4;
  }
  s.total_height = g_progress_total_offsets[color].height;
  s.filled_height = (this->cached_data->progress_by_color[color] * s.total_height) / 0x1e;
  s.visible_height = s.total_height - s.filled_height;
  s.rect = g_progress_red_assets[color].rect;
  (s.red_dib->*(((DibStateBlitToFn *)s.red_dib->vtable)[6]))(this->field_0c,
                                                             this->cache_x + s.rect.x,
                                                             g_progress_total_offsets[color].y + this->cache_y + s.rect.y + s.filled_height,
                                                             s.rect.width,
                                                             s.visible_height,
                                                             0,
                                                             g_progress_total_offsets[color].y + s.filled_height);
  if (s.red_dib != 0)
  {
    delete s.red_dib;
  }
  return 0;
}

// FUNCTION: STATWIN 0x1000910d
int StatWinState::refresh_town_counts(StatWinData *data)
{
  int color;

  for (color = 0; color < 5; color++)
  {
    if (this->cached_data->town_count_by_color[color] != data->town_count_by_color[color])
    {
      this->cached_data->town_count_by_color[color] = data->town_count_by_color[color];
      this->draw_town_count(color);
    }
  }
  return 0;
}

// FUNCTION: STATWIN 0x10008ddb
int StatWinState::refresh_progress_totals(StatWinData *data)
{
  int color;

  for (color = 0; color < 5; color++)
  {
    this->cached_data->progress_by_color[color] = data->progress_by_color[color];
    if (data->progress_by_color[color] != 0)
    {
      this->draw_progress_total(color);
    }
  }
  return 0;
}

// FUNCTION: STATWIN 0x10008e43
int StatWinState::refresh_duel_wins(StatWinData *data)
{
  typedef int (DibState::*DibStateLoadBitmapFn)(HWND hwnd, char *path, uint bit_count);
  typedef void (DibState::*DibStateBlitToFn)(DibState *dest, int dest_x, int dest_y, int width, int height, int source_x, int source_y);
  struct
  {
    DibState *skull_dib;
    char skull_path[256];
    int color;
    int unused_34;
    int loaded_skulls[7];
    int skull_index;
    int trailing_pad;
  } s;

  s.skull_dib = 0;
  s.loaded_skulls[0] = 0;
  s.loaded_skulls[1] = 0;
  s.loaded_skulls[2] = 0;
  s.loaded_skulls[3] = 0;
  s.loaded_skulls[4] = 0;
  s.loaded_skulls[5] = 0;
  s.loaded_skulls[6] = 0;

  for (s.color = 0; s.color < 5; s.color++)
  {
    s.skull_index = duel_wins_to_skull_index(data->duel_wins_by_color[s.color]);
    if (s.skull_index < 0)
    {
      continue;
    }

    if (s.loaded_skulls[s.skull_index] == 0)
    {
      if (s.skull_dib != 0)
      {
        delete s.skull_dib;
      }
      s.skull_dib = new DibState();
      join_paths(s.skull_path, g_statwin_asset_dir, g_skull_bitmap_assets[s.skull_index].filename);
      if ((s.skull_dib->*(*(DibStateLoadBitmapFn *)s.skull_dib->vtable))(0, s.skull_path, 0x18) == 0)
      {
        return 4;
      }
      s.skull_dib->set_field_10_value(0);
      memset(s.loaded_skulls, 0, sizeof(s.loaded_skulls));
      s.loaded_skulls[s.skull_index] = 1;
    }

    (s.skull_dib->*(((DibStateBlitToFn *)s.skull_dib->vtable)[6]))(this->field_0c,
                                                                   g_duel_win_skull_rects[s.color].x + this->cache_x,
                                                                   g_duel_win_skull_rects[s.color].y + this->cache_y,
                                                                   g_duel_win_skull_rects[s.color].width,
                                                                   g_duel_win_skull_rects[s.color].height,
                                                                   g_duel_win_skull_rects[s.color].x,
                                                                   0);
    this->cached_data->duel_wins_by_color[s.color] = data->duel_wins_by_color[s.color];
  }

  if (s.skull_dib != 0)
  {
    delete s.skull_dib;
  }
  return 0;
}

// FUNCTION: STATWIN 0x1000152c
static int __cdecl poll_statwin_messages(void)
{
  MSG msg;
  int result;

  result = 0;
  if (PeekMessageA(&msg, 0, 0, 0, PM_REMOVE) != 0)
  {
    TranslateMessage(&msg);
    if ((msg.message == WM_RBUTTONUP) && (g_help_dialog_enabled != 0))
    {
      DialogBoxParamA(g_statwin_instance, (LPCSTR)0x65, get_sound_hwnd(), (DLGPROC)statwin_help_dialog_proc, 0);
    }
    if ((msg.message == WM_LBUTTONUP) || (msg.message == WM_KEYDOWN))
    {
      result = 1;
    }
    DispatchMessageA(&msg);
  }
  return result;
}

// FUNCTION: STATWIN 0x10002713
static BOOL CALLBACK statwin_help_dialog_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  (void)lparam;
  FILE *file;
  char dialog_text[5][100];

  switch (msg)
  {
  case WM_INITDIALOG:
    g_help_dialog_open = 1;
    file = fopen("statLang", "rt");
    if (file != NULL)
    {
      int line_index;
      for (line_index = 0; line_index < 5; line_index++)
      {
        fscanf(file, "%[^\n]\n", dialog_text[line_index]);
      }
      fclose(file);
      SetDlgItemTextA(hwnd, 0x3ec, dialog_text[0]);
      SetDlgItemTextA(hwnd, 0x3ed, dialog_text[1]);
      SetDlgItemTextA(hwnd, 0x3ea, dialog_text[2]);
      SetDlgItemTextA(hwnd, 0x3eb, dialog_text[3]);
      SetDlgItemTextA(hwnd, IDOK, dialog_text[4]);
    }
    return TRUE;

  case WM_COMMAND:
    switch (LOWORD(wparam))
    {
    case IDOK:
      EndDialog(hwnd, TRUE);
      g_help_dialog_open = 0;
      return TRUE;

    case IDCANCEL:
      EndDialog(hwnd, TRUE);
      g_help_dialog_open = 0;
      return TRUE;

    case 0x3ea:
      g_detail_window_enabled = 0;
      return TRUE;

    case 0x3eb:
      g_detail_window_enabled = 1;
      return TRUE;

    default:
      break;
    }
    break;

  case 0x3b9:
    return TRUE;
  }

  return FALSE;
}

// FUNCTION: STATWIN 0x10001e08
static LRESULT CALLBACK status_window_wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    LRESULT result;
    HDC hdc;
  } s;

  switch (msg)
  {
  case WM_KEYDOWN:
  case WM_LBUTTONUP:
    g_video_stop_requested = 1;
    break;

  case WM_RBUTTONUP:
    DialogBoxParamA(g_statwin_instance, (LPCSTR)0x65, hwnd, (DLGPROC)statwin_help_dialog_proc, 0);
    break;

  case WM_CREATE:
    g_video_erase_background_pending = 1;
    return DefWindowProcA(hwnd, msg, wparam, lparam);
    break;

  case WM_DESTROY:
    s.hdc = GetDC(hwnd);
    SetSystemPaletteUse(s.hdc, DAT_1000d278);
    ReleaseDC(hwnd, s.hdc);
    g_video_erase_background_pending = 1;
    break;

  case WM_KILLFOCUS:
  case 9:
  case WM_ENABLE:
  case WM_SETREDRAW:
  case WM_SETTEXT:
  case WM_GETTEXT:
  case WM_GETTEXTLENGTH:
    break;

  case WM_SETFOCUS:
    break;

  case WM_PAINT:
    InvalidateRect(hwnd, NULL, FALSE);
    break;

  case WM_PALETTECHANGED:
    InvalidateRect(hwnd, NULL, FALSE);
    break;

  case WM_ERASEBKGND:
    if (g_video_erase_background_pending != 0)
    {
      s.result = DefWindowProcA(hwnd, msg, wparam, lparam);
      s.hdc = GetDC(hwnd);
      DAT_1000d278 = SetSystemPaletteUse(s.hdc, 1);
      ReleaseDC(hwnd, s.hdc);
      g_video_erase_background_pending = 0;
    }
    draw_status_dib_to_window(hwnd);
    break;

  case WM_SIZE:
    InvalidateRect(hwnd, NULL, FALSE);
    break;

  default:
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }

  if (g_help_dialog_open != 0)
  {
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }
  else
  {
    return 0;
  }
}

// FUNCTION: STATWIN 0x10002056
static void __cdecl draw_status_dib_to_window(HWND hwnd)
{
  typedef int (DibState::*DibStateGetDimensionFn)(void);
  struct
  {
    BITMAPINFOHEADER *header;
    HDRAWDIB draw_dib;
    HDC hdc;
    int dest_y;
    int dest_x;
    int height;
    int width;
    void *bits;
  } s;

  s.hdc = GetDC(hwnd);
  s.draw_dib = DrawDibOpen();
  s.header = g_status_state->field_0c->get_header();
  s.dest_x = 0;
  s.dest_y = 0;
  s.width = (g_status_state->field_0c->*(((DibStateGetDimensionFn *)g_status_state->field_0c->vtable)[2]))();
  s.height = (g_status_state->field_0c->*(((DibStateGetDimensionFn *)g_status_state->field_0c->vtable)[3]))();
  s.bits = g_status_state->field_0c->get_bits();
  DrawDibDraw(s.draw_dib, s.hdc, s.dest_x, s.dest_y, s.width, s.height, s.header, s.bits, 0, 0, s.width, s.height, 0);
  ReleaseDC(hwnd, s.hdc);
  DrawDibClose(s.draw_dib);
}

// FUNCTION: STATWIN 0x10002b15
static LRESULT CALLBACK fullscreen_video_wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    LRESULT result;
    int unused;
  } s;

  switch (msg)
  {
  case WM_KEYDOWN:
  case WM_LBUTTONUP:
    g_video_stop_requested = 1;
    break;

  case WM_CREATE:
    g_video_erase_background_pending = 1;
    return DefWindowProcA(hwnd, msg, wparam, lparam);
    break;

  case WM_DESTROY:
    g_video_erase_background_pending = 1;
    break;

  case WM_KILLFOCUS:
  case 9:
  case WM_ENABLE:
  case WM_SETREDRAW:
  case WM_SETTEXT:
  case WM_GETTEXT:
  case WM_GETTEXTLENGTH:
    break;

  case WM_SETFOCUS:
    break;

  case WM_PAINT:
    InvalidateRect(hwnd, NULL, FALSE);
    break;

  case WM_PALETTECHANGED:
    InvalidateRect(hwnd, NULL, FALSE);
    break;

  case WM_ERASEBKGND:
    if (g_video_erase_background_pending != 0)
    {
      s.result = DefWindowProcA(hwnd, msg, wparam, lparam);
      g_video_erase_background_pending = 0;
    }
    break;

  case WM_SIZE:
    InvalidateRect(hwnd, NULL, FALSE);
    break;
  }

  return DefWindowProcA(hwnd, msg, wparam, lparam);
}
