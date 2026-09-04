#ifndef SHANDALAR_INTERNAL_H
#define SHANDALAR_INTERNAL_H

#include <windows.h>
#include <stdio.h>
#include <setjmp.h>
#include "defs.h"
#include "shandalar.h"
#include "facemaker/src/facemaker_types.h"
#include "drawcardlib/src/pic.h"
#include "deckdll/src/magsnd.h"

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

typedef struct
{
  char samples[0x13][0x13];
  char padding[7];
} AnimatedNoiseGridPlane;

int HandleMainMenuButtonControlEvent(AdvMenuControl *control_ptr, int event_type);
int HandlePortraitMainMenuControlEvent(AdvMenuControl *control_ptr, int event_type);
int HandleWorldMagicChoiceControlEvent(AdvMenuControl *control_ptr, int event_type);
int ActivateWorldMagicChoiceControl(AdvMenuControl *control);
int ActivateMainMenuControl(AdvMenuControl *control);

// GLOBAL: SHANDALAR 0x007483f8
extern HDC global_main_hdc;
// GLOBAL: SHANDALAR 0x005863b8
extern int global_screen_width;
// GLOBAL: SHANDALAR 0x005863bc
extern int global_screen_height;
// GLOBAL: SHANDALAR 0x005862d8
extern int g_neighbor_dx[9];
// GLOBAL: SHANDALAR 0x00586340
extern int g_neighbor_dy[9];
// GLOBAL: SHANDALAR 0x00589de8
extern char *PTR_s_advinter800_pic_00589de8;
// GLOBAL: SHANDALAR 0x00583290
extern FacemakerWindowBounds g_page0_window_bounds_storage;
// GLOBAL: SHANDALAR 0x005832b4
extern FacemakerWindowBounds *g_page0_window_bounds;
// GLOBAL: SHANDALAR 0x005832b8
extern FacemakerWindowBounds g_page1_window_bounds_storage;
// GLOBAL: SHANDALAR 0x005832dc
extern FacemakerWindowBounds *g_page1_window_bounds;
// GLOBAL: SHANDALAR 0x005832e0
extern FacemakerWindowBounds g_page2_window_bounds_storage;
// GLOBAL: SHANDALAR 0x00583304
extern FacemakerWindowBounds *g_page2_window_bounds;
// GLOBAL: SHANDALAR 0x00583308
extern FacemakerWindowBounds g_page3_window_bounds_storage;
// GLOBAL: SHANDALAR 0x0058332c
extern FacemakerWindowBounds *g_page3_window_bounds;
// GLOBAL: SHANDALAR 0x00583330
extern FacemakerWindowBounds g_page5_window_bounds_storage;
// GLOBAL: SHANDALAR 0x00583354
extern FacemakerWindowBounds *g_page5_window_bounds;
// GLOBAL: SHANDALAR 0x00939160
extern HINSTANCE g_app_instance;
// GLOBAL: SHANDALAR 0x00591210
extern int g_skip_world_sfx_preload;
// GLOBAL: SHANDALAR 0x00591224
extern int g_loadsave_skip_esc;
// GLOBAL: SHANDALAR 0x00748418
extern UINT g_timer_resolution_ms;
// GLOBAL: SHANDALAR 0x007483fc
extern UINT g_timer_event_handle;
// GLOBAL: SHANDALAR 0x0078cefc
extern int g_cursor_visibility_depth;
// GLOBAL: SHANDALAR 0x00589de4
extern UINT g_timer_period_ms;
// GLOBAL: SHANDALAR 0x00589df0
extern int g_ui_tick_count;
// GLOBAL: SHANDALAR 0x00589dec
extern int g_default_palette_fade_steps;
// GLOBAL: SHANDALAR 0x00748400
extern int g_sound_loader_busy;
// GLOBAL: SHANDALAR 0x00748404
extern HANDLE g_main_thread_handle;
// GLOBAL: SHANDALAR 0x00748424
extern HANDLE g_timer_thread_handle;
// GLOBAL: SHANDALAR 0x00748408
extern int g_local_sound_missing;
// GLOBAL: SHANDALAR 0x0074840c
extern int g_local_sound_state;
// GLOBAL: SHANDALAR 0x00748410
extern HANDLE g_loader_thread_handle;
// GLOBAL: SHANDALAR 0x005b7d90
extern int g_palette_class_registered;
// GLOBAL: SHANDALAR 0x005b7d94
extern int g_timer_thread_handle_ready;
// GLOBAL: SHANDALAR 0x005b7d98
extern HWND g_palette_window_hwnd;
// GLOBAL: SHANDALAR 0x00986d94
extern int g_mouse_button_down_mask;
// GLOBAL: SHANDALAR 0x00986d98
extern int g_mouse_y;
// GLOBAL: SHANDALAR 0x00986d9c
extern int g_mouse_x;
// GLOBAL: SHANDALAR 0x00986da0
extern int g_mouse_button_released_mask;
// GLOBAL: SHANDALAR 0x00669704
extern int g_cached_cwd_initialized;
// GLOBAL: SHANDALAR 0x0066970c
extern int g_sound_drive_initialized;
// GLOBAL: SHANDALAR 0x0073e890
extern char g_cached_cwd[0x100];
// GLOBAL: SHANDALAR 0x0073e9d8
extern char g_sound_drive_letter;
// GLOBAL: SHANDALAR 0x0067a3b8
extern char *g_advblocks_file_buffer;
// GLOBAL: SHANDALAR 0x0078cf08
extern FILE *g_advbuttons_ini_file;
// GLOBAL: SHANDALAR 0x0078df10
extern char g_ini_string_scratch[0x28];
// GLOBAL: SHANDALAR 0x0078df38
extern int g_done_text_table_entry;
// GLOBAL: SHANDALAR 0x00586494
extern int g_legacy_mouse_active;
// GLOBAL: SHANDALAR 0x00586498
extern int g_hide_world_map_overlays;
// GLOBAL: SHANDALAR 0x007486d0
extern EncodedImage *g_ttsprite_special_sprite_a;
// GLOBAL: SHANDALAR 0x007486e0
extern EncodedImage *g_world_magic_avatar_sprites[5];
// GLOBAL: SHANDALAR 0x00781728
extern EncodedImage *g_face_preview_sprite_selected;
// GLOBAL: SHANDALAR 0x0078172c
extern EncodedImage *g_face_preview_sprite_group[6];
// GLOBAL: SHANDALAR 0x0073e9dc
extern int g_world_scene_reveal_effect_pending;
// GLOBAL: SHANDALAR 0x00591214
extern int g_world_location_entry_enabled;
// GLOBAL: SHANDALAR 0x00591218
extern int g_world_move_dir_index;
// GLOBAL: SHANDALAR 0x0059121c
extern int g_world_location_music_active;
// GLOBAL: SHANDALAR 0x00591220
extern int g_world_location_music_track_id;
// GLOBAL: SHANDALAR 0x0059126c
extern int g_world_location_music_town_index;
// GLOBAL: SHANDALAR 0x0073ea70
extern int g_world_magic_town_flags[5];
// GLOBAL: SHANDALAR 0x0073ea84
extern int g_world_player_animation_direction;
// GLOBAL: SHANDALAR 0x0073ea8c
extern int g_world_player_animation_frame;
// GLOBAL: SHANDALAR 0x006696f4
extern int g_world_location_entry_latched;
// GLOBAL: SHANDALAR 0x006696f8
extern int g_advfac64_load_state;
// GLOBAL: SHANDALAR 0x006696fc
extern int g_adventure_demo_enabled;
// GLOBAL: SHANDALAR 0x00669700
extern int g_adventure_demo_idle_ticks;
// GLOBAL: SHANDALAR 0x00669710
extern int g_world_scene_force_redraw;
// GLOBAL: SHANDALAR 0x00789938
extern int g_deck_total_card_count;
// GLOBAL: SHANDALAR 0x0078df68
extern int g_deck_active_card_count;
// GLOBAL: SHANDALAR 0x0097db40
extern long g_hint_text_offsets[0x100];
// GLOBAL: SHANDALAR 0x0097df40
extern int g_hint_difficulty_masks[0x100];
// GLOBAL: SHANDALAR 0x0097e340
extern char g_last_parsed_deck_path[0x110];
// GLOBAL: SHANDALAR 0x0097e450
extern HintPair g_hint_card_pairs[0x100];
// GLOBAL: SHANDALAR 0x007898f4
extern int g_mouse_x_snapshot;
// GLOBAL: SHANDALAR 0x007898f8
extern int g_mouse_y_snapshot;
// GLOBAL: SHANDALAR 0x007898f0
extern int g_mouse_button_mask_snapshot;
// GLOBAL: SHANDALAR 0x007490f0
extern DialogBoxSpriteBank g_dialog_box_sprite_bank;
// GLOBAL: SHANDALAR 0x0058e050
extern HANDLE g_statwin_dll_module;
// GLOBAL: SHANDALAR 0x0058c5ec
extern int g_dungeon_card_effect_by_color[4];
// GLOBAL: SHANDALAR 0x0058c5fc
extern int g_dungeon_card_effect_by_monster_flags[9];
// GLOBAL: SHANDALAR 0x0058c620
extern int g_mana_castle_card_effect_by_difficulty[5][4];
// GLOBAL: SHANDALAR 0x00746e00
extern int g_statwin_exports_by_ordinal[3];
// GLOBAL: SHANDALAR 0x00747ee0
extern int g_menu_render_guard;
// GLOBAL: SHANDALAR 0x00747ef0
extern AnimatedNoiseGridPlane g_animated_noise_grid[2];
// GLOBAL: SHANDALAR 0x00746ec0
extern FacemakerWindowBounds g_menu_saved_window_bounds;
// GLOBAL: SHANDALAR 0x00746ef0
extern int g_menu_control_count_by_context[50];
// GLOBAL: SHANDALAR 0x00746f40
extern AdvMenuControl *g_menu_controls_by_context[50][50];
// GLOBAL: SHANDALAR 0x00748d10
extern EncodedImage *g_main_menu_button_sprites_normal[4];
// GLOBAL: SHANDALAR 0x00748d20
extern EncodedImage *g_main_menu_button_sprites_highlight[4];
// GLOBAL: SHANDALAR 0x007491c0
extern WorldMagicChoiceButtonSpriteBank g_world_magic_choice_button_sprite_bank;
// GLOBAL: SHANDALAR 0x00748440
extern EncodedImage *g_road_sprite_entries[0xc];
// GLOBAL: SHANDALAR 0x00748470
extern EncodedImage *g_location07_sprite_entries[0xc];
// GLOBAL: SHANDALAR 0x007484a0
extern EncodedImage *g_land_sprite_entries[22][5];
// GLOBAL: SHANDALAR 0x00748660
extern EncodedImage *g_tsprite2_grid_sprite_entries[0xc];
// GLOBAL: SHANDALAR 0x00748690
extern EncodedImage *g_land_tile_sprite_entries[0x10];
// GLOBAL: SHANDALAR 0x00748700
extern EncodedImage *g_ttsprite_alt_sprite_entries[8];
// GLOBAL: SHANDALAR 0x00748720
extern EncodedImage *g_cstline1_sprite_entries[12][7];
// GLOBAL: SHANDALAR 0x00748870
extern EncodedImage *g_ttsprite_aux_sprite_entries[8];
// GLOBAL: SHANDALAR 0x00748890
extern int g_world_lair_monster_sprite_top_clips[0x10];
// GLOBAL: SHANDALAR 0x007488d0
extern int g_ego_sprite_draw_height;
// GLOBAL: SHANDALAR 0x007488d4
extern int g_sego_sprite_draw_height;
// GLOBAL: SHANDALAR 0x00748910
extern EncodedImage *g_location_marker_sprite_entries[0x9e];
// GLOBAL: SHANDALAR 0x00748d30
extern EncodedImage *g_questnew_sprite_entries[4];
// GLOBAL: SHANDALAR 0x00748d40
extern EncodedImage *g_asprite_sprite_entries[0x3c];
// GLOBAL: SHANDALAR 0x00748e30
extern EncodedImage *g_tsprite2_extra_sprite_entries[0xd];
// GLOBAL: SHANDALAR 0x00748e64
extern EncodedImage *g_gsprite_sprite_entries[3];
// GLOBAL: SHANDALAR 0x00748e70
extern EncodedImage *g_sunmoon_sprite_entries[0x14];
// GLOBAL: SHANDALAR 0x00748ec0
extern EncodedImage *g_worlds_extra_sprite_entries[4];
// GLOBAL: SHANDALAR 0x00748ed0
extern EncodedImage *g_tips_frame_sprite;
// GLOBAL: SHANDALAR 0x00748ee0
extern EncodedImage *g_clocknew_sprite_entries[9];
// GLOBAL: SHANDALAR 0x00748f04
extern EncodedImage *g_tips_icon_sprite;
// GLOBAL: SHANDALAR 0x00748f10
extern EncodedImage *g_wizard_controlled_town_sprite_entries[5];
// GLOBAL: SHANDALAR 0x00748f30
extern EncodedImage *g_sland_sprite_entries[22][5];
// GLOBAL: SHANDALAR 0x00749280
extern EncodedImage *g_daysnew_sprite_entries[0xc];
// GLOBAL: SHANDALAR 0x007492b0
extern EncodedImage *g_icons_sprite_entries[0x18];
// GLOBAL: SHANDALAR 0x00749310
extern int g_world_lair_monster_sprite_widths[0x10];
// GLOBAL: SHANDALAR 0x00749350
extern int g_ego_sprite_width;
// GLOBAL: SHANDALAR 0x00749354
extern int g_sego_sprite_width;
// GLOBAL: SHANDALAR 0x00749390
extern int g_world_lair_monster_sprite_heights[0x10];
// GLOBAL: SHANDALAR 0x007493d0
extern int g_ego_sprite_height;
// GLOBAL: SHANDALAR 0x007493d4
extern int g_sego_sprite_height;
// GLOBAL: SHANDALAR 0x00749410
extern EncodedImage *g_ttsprite_special_sprite_b;
// GLOBAL: SHANDALAR 0x00749414
extern EncodedImage *g_ttsprite_special_sprite_c;
// GLOBAL: SHANDALAR 0x00749418
extern EncodedImage *g_current_location_marker_sprite;
// GLOBAL: SHANDALAR 0x00749420
extern EncodedImage *g_compnew_sprite_entries[5];
// GLOBAL: SHANDALAR 0x00749434
extern EncodedImage *g_endtop_banner_sprite;
// GLOBAL: SHANDALAR 0x00749440
extern EncodedImage *g_tsprite2_overlay_sprite_entries[8];
// GLOBAL: SHANDALAR 0x00749460
extern EncodedImage *g_ttsprite_grid_sprite_entries[0x40];
// GLOBAL: SHANDALAR 0x00749560
extern EncodedImage *g_castles_sprite_entries[20];
// GLOBAL: SHANDALAR 0x007496a0
extern OpeningMenuSpriteWorkEntry g_opening_menu_sprite_work_buffer[0x20];
// GLOBAL: SHANDALAR 0x0078df40
extern EncodedImage *g_ttsprite_extra_sprite_entries[10];
// GLOBAL: SHANDALAR 0x0058b584
extern int g_menu_context_index;
// GLOBAL: SHANDALAR 0x0058b588
extern AdvMenuControl g_main_menu_controls[4];
// GLOBAL: SHANDALAR 0x0058b6d8
extern AdvMenuControl g_world_magic_choice_controls[5];
// GLOBAL: SHANDALAR 0x0058b580
extern FacemakerWindowBounds *g_menu_control_draw_target_page;
// GLOBAL: SHANDALAR 0x0058b87c
extern int g_world_magic_icon_rect_table_padding;
// GLOBAL: SHANDALAR 0x0058b880
extern AdvMenuRect g_world_magic_icon_rects[0xc];
// GLOBAL: SHANDALAR 0x0058b940
extern char *g_world_magic_sound_paths[5];
// GLOBAL: SHANDALAR 0x0058b954
extern int g_menu_current_control_index;
// GLOBAL: SHANDALAR 0x0058b958
extern int g_menu_prev_control_index;
// GLOBAL: SHANDALAR 0x005b80d8
extern int g_menu_input_unhandled;
// GLOBAL: SHANDALAR 0x005b80e0
extern int g_menu_allow_arrow_nav_by_context[50];
// GLOBAL: SHANDALAR 0x008c7408
extern int g_duel_special_rules_by_color[7];
// GLOBAL: SHANDALAR 0x0073e990
extern jmp_buf g_adventure_session_restart_jump_buffer;
// GLOBAL: SHANDALAR 0x0073e9e0
extern jmp_buf g_adventure_world_exit_jump_buffer;
// GLOBAL: SHANDALAR 0x006527b0
extern int g_pending_world_location;
// GLOBAL: SHANDALAR 0x00590764
extern int g_current_world_location;
// GLOBAL: SHANDALAR 0x00590768
extern int g_adventure_ui_layout_dirty;
// GLOBAL: SHANDALAR 0x00650f28
extern int g_adventure_ui_edge_pages_initialized;
// GLOBAL: SHANDALAR 0x0073eaa0
extern int g_world_scroll_cache_ready;
// GLOBAL: SHANDALAR 0x0073e880
extern int g_pending_ui_action_code;
// GLOBAL: SHANDALAR 0x005a6074
extern int g_text_menu_left;
// GLOBAL: SHANDALAR 0x005a6078
extern int g_text_menu_top;
// GLOBAL: SHANDALAR 0x00580d80
extern int g_text_menu_initial_selection;
// GLOBAL: SHANDALAR 0x00580d84
extern int g_text_menu_mouse_released;
// GLOBAL: SHANDALAR 0x00580d88
extern int g_text_menu_last_selection;
// GLOBAL: SHANDALAR 0x00580d8c
extern int g_text_menu_show_ok_button;
// GLOBAL: SHANDALAR 0x00580d90
extern int g_text_menu_mouse_active;
// GLOBAL: SHANDALAR 0x00580d94
extern int g_text_menu_timeout_seconds;
// GLOBAL: SHANDALAR 0x00580d98
extern int g_text_menu_line_height;
// GLOBAL: SHANDALAR 0x00580d9c
extern int g_text_menu_color_normal;
// GLOBAL: SHANDALAR 0x00580da0
extern int g_text_menu_color_selected;
// GLOBAL: SHANDALAR 0x005a5fe8
extern int g_text_menu_line_offsets[0x20];
// GLOBAL: SHANDALAR 0x005a6068
extern int g_text_menu_color_base;
// GLOBAL: SHANDALAR 0x005a606c
extern int g_text_menu_finish_flash;
// GLOBAL: SHANDALAR 0x005a6070
extern int g_text_menu_line_count;
// GLOBAL: SHANDALAR 0x005a607c
extern int g_text_menu_hovered_selection;
// GLOBAL: SHANDALAR 0x0078cef0
extern char g_text_menu_timer_itoa_buffer[0xc];
// GLOBAL: SHANDALAR 0x00789934
extern int g_shandalar_state_00789934;
// GLOBAL: SHANDALAR 0x0097ec50
extern signed char g_text_menu_hotkey_by_option[0x20];
// GLOBAL: SHANDALAR 0x0097ec70
extern int g_text_menu_caret_prefix_mask;
// GLOBAL: SHANDALAR 0x0097ec74
extern int g_text_menu_force_cancel;
// GLOBAL: SHANDALAR 0x0097ec78
extern int g_text_menu_needs_layout;
// GLOBAL: SHANDALAR 0x0097ec7c
extern int g_text_menu_abort_requested;
// GLOBAL: SHANDALAR 0x0097ec80
extern int g_text_menu_option_count;
// GLOBAL: SHANDALAR 0x0097ec84
extern int g_text_menu_right;
// GLOBAL: SHANDALAR 0x0097ec88
extern int g_text_menu_max_line_width;
// GLOBAL: SHANDALAR 0x0097ec8c
extern int g_text_menu_ok_mode;
// GLOBAL: SHANDALAR 0x0097ec90
extern int g_text_menu_first_option_line;
// GLOBAL: SHANDALAR 0x0097ec94
extern int g_text_box_frame_color_override;
// GLOBAL: SHANDALAR 0x0097ec98
extern int g_text_menu_disabled_option_mask;
// GLOBAL: SHANDALAR 0x005aa414
extern int g_shandalar_state_005aa414;
// GLOBAL: SHANDALAR 0x005aa62c
extern int g_last_matching_deck_card_index;
// GLOBAL: SHANDALAR 0x005873d4
extern int g_shandalar_initialized;

extern DIBSurface *g_graphics_pages[10];
extern char g_ui_message_buffer[0x1000];
extern int g_adventure_world_exit_requested;
extern unsigned int g_load_menu_valid_slot_mask;
extern char g_save_file_path[13];
extern int g_selected_save_slot_index;
extern int g_reveal_all_world_info;
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

int InitLicenseSecretsFromRegistry(void);
int LoadAdvStringsFile(const char *filename);
int LoadTextSectionStringTable(const char *filename, const char *section, char **out_table, int max_entries, char *string_buf,
                               char *string_buf_end, char **out_next_buf);
int LoadAdvBlocksFile(const char *filename);
int FindNextTextBlock(char *scan_start, char *scan_end, int *out_block_start, int *out_next_scan);
void ReadCsvFieldByCsvid(char *out, int csvid, int field, const char *csv_name);
void *CreateGraphicsPage(int page_number, int width, int height, int bits_per_pixel);
void PreloadWorldAmbientSounds(void);
void FreeAdvblocksFileBuffer(void);
void QueueKeyInputFromMessage(WPARAM wparam, LPARAM lparam);
ATOM RegisterPaletteClass(HINSTANCE hinst);
HWND CreatePalettePopupWindow(HINSTANCE hinst, HWND parent_hwnd);
int *LoadIniEscapedStringTable(FILE *ini_file, char *section_name, char *scratch, ...);
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
int IgnoreFontConfigLoadResult(int ignored_result);
int InitializeLegacyMouseStub(void);
int ShutdownLegacyMouseStub(void);
void *InitializeGraphicsSystemDefaultMode(void);
void SetGraphicsPage(int page_number, void *page);
void PresentGraphicsPage(int page_number);
void ReadPalette(char *palette_text_path, char *palette_binary_path);
int ScaleUiCoordinate(int value);
int LoadFontConfigIfPresent(char *executable_name, char *config_name);
int LoadSystemFont(int font_id, unsigned int point_size, char *font_file, char *font_name, int weight, DWORD italic);
int RunAdventureSession(void);
BOOL UnloadFontSlot(int font_slot);
int FileExists(const char *filename);
char FindDriveWithAsset(char *filename);
char GetSoundAssetDriveLetter(void);
unsigned int LoadSoundWithDriveFallback(char *filename, int channel, Sound *sound);
LONG ChangeDisplayResolution(DWORD width, DWORD height);
void RestoreDisplayResolution(void);
DWORD AdventureWorkerThread(LPVOID);
void NoopSetSpecialSprite(int unused_a, int unused_b, int unused_c);
char *BuildResolutionSpritePath(char *sprite_filename);
int ReadSpriteEntryPointersWithLimit(EncodedImage **out_entries, char *path, int max_entries);
void LoadTownHintMetadata(void);
void LoadCardRaritiesAndCsvOffsets(void);
void ShowMouseCursorNested(void);
void HideMouseCursorNested(void);
void ShowMouseCursor(void);
void HideMouseCursor(void);
int ReadSpriteEntryPointers(EncodedImage **out_sprite_entries, char *sprite_path);
void FreeSpriteBlob(void *memory);
void AnimatePaletteToColor(int color_index, int palette_id);
int RunOpeningMenu(void);
int RunDifficultyMenu(void);
int RunColorMenu(void);
int RunFacemakerFlow(void);
void InitializeNewGameState(void);
void GenerateAdventureWorldMap(void);
void InitializeAnimatedNoiseGrid(void);
int CalculateWorldTerrainValue(int x, int y);
void PropagatePathConnectivity(void);
int GenerateWorldTownSlots(void);
void GenerateTownConnections(void);
int CreateTownConnectionPath(int start_x, int start_y, int target_x, int target_y);
void FloodFillPathConnectivity(int x, int y, unsigned int depth);
void InitializeCastleDungeonSlots(void);
unsigned int GetWorldTileType(int x, int y);
unsigned int GetWorldTileMagicMask(unsigned int tile_type);
unsigned int GetWorldMapPixelFlags(int x, int y);
void SetWorldMapPixelFlags(unsigned int mask, int x, int y);
void ClearWorldMapPixelFlags(unsigned int mask, int x, int y);
void MarkPathConnection(int x, int y, int direction_index);
int SampleAnimatedNoiseGridBilinear(int x_fixed, int y_fixed);
void SaveGameToSlot(int save_slot_index);
int RunLoadGameMenu(void);
int load_selected_duel_save_slot(int save_slot_index);
int single_color_test_bit_to_color_t(int mask);
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
int RunRightClickMenuAndQueueInput(void);
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
int ExportEncodedImage(int page_number, int x, int y, int width, int height, int write_palette, char *path);
unsigned int WaitForInputEventUnlessBlocked(void);
int UpdateMenuControlSelection(int mouse_x, int mouse_y, int allow_activate_on_click);
unsigned int PeekQueuedKeyInput(void);
unsigned int WaitForInputEvent(void);
int ApproximateDistance(int delta_x, int delta_y);
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

#ifdef _DEBUG
void RunLairExplorationEvent(int color);
#endif

void DelayUiTicks(int ticks);
int find_internal_card_id_by_csv_id(card_id_t card_id);
unsigned int PickRandomColorBitExcludingMask(unsigned int excluded_mask);
int AddRandomStarterDeckCards(unsigned int color_mask, int land_count, int spell_count, int creature_count, int add_rare, int allow_artifact_spells);
int PickRandomCardMatchingTypeAndColor(unsigned int type_mask, unsigned int color_mask);
int AddCardToDeckSorted(int card_id);
int GetCardRarity(int card_id);
int GetRemainingAllowedCardCopies(unsigned int card_id);
unsigned int GetGraphicsPixelColorRef(FacemakerWindowBounds *window, int x, int y);
int RunLoadSaveMenu(int save_mode);
int UpdateStatWindowProgress(void *progress_values);
int MapWizardColorToDisplayIndex(int wizard_color);
int ConsumeMouseButtonReleaseMask(void);
int RunTextMenuAt(char *menu_text, int left_x, int top_y);
int RunTextMenuCore(char *menu_text, int clear_input_before_show);
int RunTextMenuAtScaled(char *menu_text, int x_320_scale, unsigned int y_200_scale);
int DrawTextMenu(char *menu_text, int selected_option);
void DrawDialogBoxFrameAutoStyle(int x, int y, int width, int height, int unk1);
void DrawRectangleBorder(int x, int y, int width, int height, int color_index);
void DrawTiledDialogBoxFrame(int x, int y, int width, int height, int frame_style);
void DrawUiLine(int x1, int y1, int x2, int y2, int color_index);
void DrawTextLineClamped(char *text, int x, int y, int color_index);
void DrawTextLineNoShadow(char *text, int x, int y, int color_index);
void DrawCenteredTextLineClamped(char *text, int center_x, int y, int color_index);
int MeasureTextLineWidth(char *text);
void DrawEncodedImageUiScaled(FacemakerWindowBounds *dst, int x_320, int y_200, EncodedImage *sprite, int width_320, int height_200);
int FindWorldMagicCardIndex(int world_magic_slot_index);
void PushQueuedKeyInput(int key_code);
int MapWorldClickToMovementKey(int x, int y);
int RenderMenuControlRange(int first_index, int count);
int GetUiTickCount(void);
int SeedRandomFromTickCount(void);
int ClampIntToRange(int value, int min_value, int max_value);
int int_to_hex_digit(int value);
unsigned int load_or_probe_duel_save_slot(char *save_file_path, int validate_only);
int SaveGameWithMessage(char *save_file_path);
int GetSaveDriveIndex(void);
int IsSaveDriveAvailable(int drive_index);
int LoadGameFromPath(char *save_file_path);
int SaveGameToPath(char *save_file_path);
int GetFontCharWidth(int font_slot, char ch);
int GetFontLineHeight(int font_slot);
void DrawGraphicsLine(FacemakerWindowBounds *window_bounds, int x1, int y1, int x2, int y2, int color_index);
int DrawTextLine(FacemakerWindowBounds *window_bounds, int x, int y, char *text);
void DrawEncodedImageUnscaled(FacemakerWindowBounds *dst, int x, int y, EncodedImage *encoded_image);
void DrawTextAt(FacemakerWindowBounds *window, int color, int x, int y, char *text, ...);
WPARAM WINAPI DeckBuilderMain(HWND parent_hwnd, int db_flags_1, int db_flags_2);
int LoadTextSectionLines(const char *filename, const char *section);
int IsKeyInputQueueEmpty(void);
unsigned int WorldRoadTileHasDirection(int tile_x, int tile_y, int direction_index);
int FreeOpeningMenuSpriteWorkEntries(int work_entry_index_a, int work_entry_index_b);
int SignNonZero(int value);
int RestoreAdventureUiPaletteAndFocus(void);
int VisitTownSlot(int town_index);
int RunSaveMenuAndSelectSlot(void);
void *DrawAdventureInterfaceLayout(int force_redraw);
int RunRandomAiDuelDemo(void);
void EnsureAdvfac64Loaded(int state);
void StartWizardTownSiege(void);
void ResolveWizardTownSiege(void);
void PlaySoundWithPitchAndPan(int sound_id, int volume, int pitch_percent, int pan_percent);
void PlayLoopingSoundWithPan(int sound_id, int volume, int pan_percent);
void LoadLoopingSound(char *sound_path, int channel);
void UpdateAmbientWizardColorSound(int wizard_color);
void EnterCastleDungeon(int dungeon_index);
void ShowCityInfoScreen(int unused);
void ShowDungeonCluesScreen(int unused);
int RunAdventureStatsMenu(void);
void ShowWorldMapScreen(int mode);
int ShowStatsWindow(int mode, int highlight);
void AnalyzeDeckAndMaybeShowReport(int show_ui);
int FindTownAtWorldCoordinates(int world_x, int world_y);
int FindCastleDungeonAtWorldCoordinates(int world_x, int world_y);
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

void ClearAndLoadInitialLibraryFromDeckFile(char *deck_path, int library_index, unsigned int color_filter, int speed_filter);
int RunDuelEngine(unsigned int card_id, int creature_type);


#endif
