#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "shandalar.h"
#include "magic/src/global_state.h"
#include "magic/src/game_support.h"
#include "magic/src/global_strings.h"
#include "shandalar_global_strings.h"
#include "deckdll/src/magsnd.h"
#include "facemaker/src/facemaker_types.h"
#include "drawcardlib/src/pic.h"
#include "cardartlib/src/palette.h"

/* Dungeon / castle encounter flow rooted at SHANDALAR 0x0050a5c1. */

typedef unsigned int uint;
typedef unsigned char byte;
typedef unsigned char uchar;
typedef int undefined3;
typedef int undefined4;
typedef int bool;
#define DUNGEON_GRID_WIDTH 0xf
#define DUNGEON_GRID_HEIGHT 0xd
typedef int DungeonGrid[DUNGEON_GRID_WIDTH][DUNGEON_GRID_HEIGHT];
typedef char DungeonGrid_size_must_be_0x30c[(sizeof(DungeonGrid) == 0x30c) ? 1 : -1];
#define DUNGEON_CELL_DISTANCE(x, y) (*(char *)((char *)&g_dungeon_runtime_state + 0x364 + (y) + (x) * DUNGEON_GRID_HEIGHT))
typedef struct DungeonRuntimeState
{
  union
  {
    int monster_sound_ids[8];
    struct
    {
      int unused_sound_ids[3];
      int monster_creature_types[5];
    } selected;
  } encounter;                      // 0x000
  EncodedImage *button_sprite_blob; // 0x020
  int cell_tile_variants[171];      // 0x024
  char unk_2d0[0x60];               // 0x2d0
  int current_dungeon_index;        // 0x330
  int player_x;                     // 0x334
  int player_y;                     // 0x338
  char unk_33c[0xec];               // 0x33c
  int reveal_dirty;                 // 0x428
  EncodedImage *button_sprite_aux0; // 0x42c
  EncodedImage *button_sprite_aux1; // 0x430
  EncodedImage *button_sprite_aux2; // 0x434
  char unk_438[0xf8];               // 0x438
} DungeonRuntimeState;
typedef char DungeonRuntimeState_size_must_be_0x530[(sizeof(DungeonRuntimeState) == 0x530) ? 1 : -1];

#ifndef CONCAT31
#define CONCAT31(a, b) ((int)(b))
#endif
#ifndef true
#define true 1
#endif

extern int global_screen_width;
extern int global_screen_height;
extern int g_default_palette_fade_steps;
extern int g_adventure_world_exit_requested;

extern int g_shandalar_difficulty;
extern int g_mouse_button_mask_snapshot;
extern int g_mouse_x_snapshot;
extern int g_mouse_y_snapshot;
extern int g_defeated_wizards_bitmap;
extern int g_next_duel_card_id;
extern int g_next_duel_life_delta;
extern int g_duel_ante_card_ids[16];
extern int g_neighbor_dx[9];
extern int g_neighbor_dy[9];
extern int g_statwin_exports_by_ordinal[3];
extern int g_wizard_siege_count;
extern int g_ego_sprite_width;
extern int g_ego_sprite_height;
extern int g_ego_sprite_draw_height;
extern int g_sego_sprite_width;
extern int g_sego_sprite_draw_height;
extern int g_world_lair_monster_sprite_top_clips[0x10];
extern int g_world_lair_monster_sprite_widths[0x10];
extern int g_world_location_music_track_id;
extern int g_world_location_music_active;
extern int DAT_00742fc0;
extern int DAT_00742fd0;
extern int DAT_007a7874;
extern int DAT_007a7d10[4];
extern int DAT_008ce538;
extern int DAT_008cf6d0;
extern int unk_00789308;
extern int Gold;
extern int life[2];
extern int deck[500];
extern char g_ui_message_buffer[0x1000];
extern HPALETTE g_palette_handle;
extern HWND g_main_window_hwnd;
extern DIBSurface *g_graphics_pages[10];
extern FacemakerWindowBounds *PTR_DAT_005832b4;
extern FacemakerWindowBounds *PTR_DAT_005832dc;
extern FacemakerWindowBounds *PTR_DAT_00583304;
extern OpeningMenuSpriteWorkEntry g_opening_menu_sprite_work_buffer[0x14];
extern int(__cdecl *g_town_dialog_callback)(void);

char *BuildCreatureNameWithArticle(int creature_type);
char *BuildTownDisplayName(int town_index);
char *GetCreatureName(int creature_type);
DWORD FormatMessageFromStringStripCarriageReturns(char *dst, DWORD max_length, LPCVOID format, ...);
int BeginMenuContext(void);
int ClampIntToRange(int value, int min_value, int max_value);
int CountDuelPoolEligibleTowns(void);
int EndMenuContext(void);
int ExitIfNoUsableDeckCards(void);
int single_color_test_bit_to_color_t(int mask);
int PopNormalizedQueuedKeyInput(void);
int PopulateDungeonCellEvents(void);
void SetDungeonPassageHighlight(int direction, int enable);
void DrawCastleDungeonBoard(int animation_step, int initial_draw, int dungeon_index);
void ConvertDungeonCellToScreen(int dungeon_x, int dungeon_y, int *out_screen_x, int *out_screen_y);
void ConvertMouseToDungeonCell(int mouse_x, int mouse_y, int *out_x, int *out_y);
int QueueDungeonMouseNavigationInput(int mouse_x, int mouse_y);
int ShutdownSharedStartup(void);
int FUN_004bb040(int world_x, int world_y);
int FUN_004bc6d3(int creature_type, int work_entry_index_a, int work_entry_index_b);
int SeedRandomFromTickCount(void);
char *GetDungeonName(int dungeon_index);
int CalculateDungeonEndgameScore(void);
int RandomIntLessThan(int max_value);
int RunCardRiddleChallenge(void);
int ScaleUiCoordinateFrom320(int value);
int FUN_005501fe(int value);
int AddCardToDeckSorted(int card_id);
int FUN_0056c705(int csvid);
int FUN_0056d5c0(int sound_id, int *out_state);
int GetUiTickCount(void);
int IsKeyInputQueueEmpty(void);
int IsWizardColorFeminine(int color_index);
int LoadCreatureDuelDeck(int creature_type, unsigned int name_id, unsigned int color_filter, int speed_filter);
ShandalarEntryType PickRandomCreatureTypeForWizardTier(int wizard_color, int creature_tier);
int QuestCardChooserCallback(void);
int ReadSpriteEntryPointers(EncodedImage **out_sprite_entries, char *sprite_path);
int ReadSpriteEntryPointersWithLimit(EncodedImage **out_sprite_entries, char *sprite_path, int max_entries);
int RemoveCardFromDeckById(unsigned int card_id);
int ResetMenuContext(int context_index);
int RunCardBrowser(char *title, int color_mask, unsigned int param_3, int param_4, int param_5);
int RunCastleDungeonBoard(int dungeon_index);
int RunDungeonMonsterDuel(int dungeon_index, int monster_slot, int final_battle);
undefined4 HandleDefeatedWizardCastle(int dungeon_index);
int RunDuelEngine(unsigned int card_id, int creature_type);
int RunTextMenuAt(char *menu_text, int x, int y);
int RunTextMenuAtScaled(char *menu_text, int x_320_scale, unsigned int y_200_scale);
int ScaleUiCoordinate(int value);
int MeasureTextLineWidth(char *text);
int sound_stop(int sound_id);
int sound_unload(int sound_id);
unsigned int GetWorldTileType(int x, int y);
unsigned int FUN_004314ca(int x, int y);
unsigned int LoadSoundWithDriveFallback(char *filename, int channel, int flags);
unsigned int WaitForInputEventUnlessBlocked(void);
void AnimatePaletteToColor(int color_index, int palette_id);
void ClearGraphicsPageWithPaletteColor(int page_number, int color_index);
void ClearInputAndWaitForMouseRelease(void);
void ConsumeUiTickCount(void);
WPARAM WINAPI DeckBuilderMain(HWND parent_hwnd, int db_flags_1, int db_flags_2);
void DrawTextAt(FacemakerWindowBounds *window, int color, int x, int y, char *text, ...);
void DrawCenteredTextLineWithShadow(char *text, int center_x, int y, int color_index);
void DrawFormattedTextShadowedCentered(FacemakerWindowBounds *window, int color_index, int x, int y, char *format, ...);
void DrawUiScaledCenteredText(char *text, int center_x_320, int y_200, int color_index);
void DrawTiledDialogBoxFrame(int x, int y, int width, int height, int frame_style);
void FadeInPaletteFromGray(int color_index, int palette_id);
void FreeOpeningMenuSpriteWorkEntries(int work_entry_index_a, int work_entry_index_b);
void FreeSpriteBlob(EncodedImage *sprite_blob);
void AddJournalEntry(int entry_type, int entry_arg);
void SetWorldMapPixelFlags(unsigned int mask, int x, int y);
void ClearWorldMapPixelFlags(unsigned int mask, int x, int y);
void RevealDungeonCellAndNeighbors(int x, int y);
void AppendDungeonRewardDescription(void);
void InitDungeonScratchPage(void);
void FreeDungeonScratchPage(void);
void DelayUiTicks(int ticks);
void FUN_004ed135(void);
undefined4 TransitionPcxToScreenRandomTiles8(char *path);
undefined4 TransitionPcxToScreenRandomTiles(char *path, int x_scale, int y_scale);
void FUN_004ecfa2(void);
void DrawAdventureCard(unsigned int card_id, int x, int y, int full_card, char *banner_text);
void DrawAdventureCardSized(int card_id, int x, int y, int width, int height, int full_card, char *label);
void FUN_005000fb(int context_index);
void FUN_005616aa(int param_1);
undefined4 PlayStatWinMovie(char *path, int x, int y, int flags);
void FUN_00562736(int sound_id, int volume, int pitch_percent, int pan_percent);
void FUN_0056279e(int sound_id, int volume, int pan_percent);
void FUN_00562835(char *sound_path, int channel);
void StopWorldLocationMusic(void);
void PlayCastleMusic(int param_1);
int FreeGraphicsPage(int page_number);
void DrawUiScaledCenteredTextWithShadow(FacemakerWindowBounds *window, int color, int x, int y, char *format, ...);
void BlitGraphicsRect(FacemakerWindowBounds *dst, unsigned int dst_x, int dst_y, unsigned int width, DWORD height,
                      FacemakerWindowBounds *src, int src_x, int src_y);
void CopyUiScaledGraphicsRect(FacemakerWindowBounds *src_page, int src_x_320, int src_y_240, int width_320,
                              int height_240, FacemakerWindowBounds *dst_page, int dst_x_320, int dst_y_240);
void CopyGraphicsRect(FacemakerWindowBounds *src_page, int src_x, int src_y, int width, int height,
                      FacemakerWindowBounds *dst_page, int dst_x, int dst_y);
void FillGraphicsRect(FacemakerWindowBounds *window_bounds, int x, int y, int width, int height, int color_index);
void FillUiScaledGraphicsRect(FacemakerWindowBounds *window_bounds, int x_320, int y_240, int width_320,
                              int height_240, int color_index);
void DrawEncodedImageResampled(FacemakerWindowBounds *dst, int x, int y, int width, int height, EncodedImage *encoded_image);
void DrawEncodedImageUiScaled(FacemakerWindowBounds *dst, int x_320, int y_200, EncodedImage *sprite, int width_320, int height_200);
void DrawEncodedImageUnscaled(FacemakerWindowBounds *dst, int x, int y, EncodedImage *encoded_image);
unsigned int BlitRectByRandomTileOrderInPlace(HDC dst, int dst_x, int dst_y, int width, int height, int tile_w,
                                              int tile_h, HDC src);
DIBSurface *CreateGraphicsPage(int page_number, int width, int height, int bits_per_pixel);
void LoadPcxIntoPage(int page_number, char *path);
void LoadPcxIntoPageNoPalette(char *path);
void LoadPcxIntoPageOpaque(int page_number, char *path);
void LoadPcxResource(int page_number, int x, int y, char *path, void *opaque);
void PlaySoundEffectOnChannel(char *sound_path, int channel, int volume, int pitch_percent, int pan_percent);
void PushQueuedKeyInput(int key_code);
void ReadPalette(char *path, char *out_palette);
void RefreshAdventureInterfaceLayout(void);
void RestoreAdventureUiPaletteAndFocus(void);
void RunAdventureStatsMenu(void);
void ShowStatsWindow(int mode, int highlight);
void StretchBlitGraphicsRect(FacemakerWindowBounds *dst, int dst_x, int dst_y, int src_w, int src_h,
                             FacemakerWindowBounds *src, int src_x, int src_y, int copy_w, int copy_h);
void SetGraphicsPage(int page_number, DIBSurface *page);
void UnloadStatWinDllExports(void);
void UpdateMouseSnapshot(void);

// GLOBAL: SHANDALAR 0x00583a28
int g_dungeon_move_direction = 4;
// GLOBAL: SHANDALAR 0x00583a50
FacemakerWindowBounds g_dungeon_scratch_page9_bounds = {9, 0, 0, 1, 0xf, 4, 0, 0, 0};
// GLOBAL: SHANDALAR 0x00583a74
FacemakerWindowBounds *g_dungeon_scratch_page9 = &g_dungeon_scratch_page9_bounds;
// GLOBAL: SHANDALAR 0x005a8b34
DungeonRuntimeState g_dungeon_runtime_state;

// GLOBAL: SHANDALAR 0x007495b0
EncodedImage *g_dungeon_sprite_entries[0x3c];
#define g_dungeon_sprite_blob g_dungeon_sprite_entries[0]
// GLOBAL: SHANDALAR 0x0078df88
int g_dungeon_life_reward_delta;
// GLOBAL: SHANDALAR 0x0078df8c
int g_dungeon_exit_x;
// GLOBAL: SHANDALAR 0x0078df90
int g_dungeon_exit_y;
// GLOBAL: SHANDALAR 0x0078dfa0
DungeonGrid g_dungeon_grid_backup;
// GLOBAL: SHANDALAR 0x0078e2ac
int g_dungeon_force_final_encounter;
// GLOBAL: SHANDALAR 0x0078e2b0
DungeonGrid g_dungeon_grid;
#define DUNGEON_GRID_CELL(x, y) g_dungeon_grid[(x)][(y)]
// GLOBAL: SHANDALAR 0x0078e44c
int g_dungeon_grid_generation_state;
// GLOBAL: SHANDALAR 0x00716024
int g_dungeon_pre_duel_life_total;

static __inline int GetDungeonGridCellOrZero(int x, int y)
{
  if (x < 0 || x >= DUNGEON_GRID_WIDTH || y < 0 || y >= DUNGEON_GRID_HEIGHT)
  {
    return 0;
  }
  return DUNGEON_GRID_CELL(x, y);
}

static __inline int GetDungeonCellDistanceOrZero(int x, int y)
{
  if (x < 0 || x >= DUNGEON_GRID_WIDTH || y < 0 || y >= DUNGEON_GRID_HEIGHT)
  {
    return 0;
  }
  return DUNGEON_CELL_DISTANCE(x, y);
}

// GLOBAL: SHANDALAR 0x00583a30
static char *g_dungeon_sprite_paths[] = {"dungeon1.spr", "dungeon2.spr", "dungeon3.spr", "dungeonb.spr", "dungeonu.spr", "dungeong.spr", "dungeonr.spr", "dungeonw.spr"};
#define g_dungeon_wizard_sprite_paths_start g_dungeon_sprite_paths[2]
// GLOBAL: SHANDALAR 0x0058c670
int g_dungeon_monster_duel_music_csvids[] = {0x343, 0x24d, 0x75, 0xa0, 0x35b, 0x1c8, 0xde, 0x95, 0x5e, 0xa3, 0x130, 0x140, 0x0d, 0xa1, 0x1cf};
// GLOBAL: SHANDALAR 0x0058caa0
static int g_castle_defeat_music_ids[] = {0, 2, 3, 4, 1};

// GLOBAL: SHANDALAR 0x00583af8
static char g_dungeon_ambient_sound_path[] = "x:sound\\damb1.wav";

// FUNCTION: SHANDALAR 0x00443940
int RunCastleDungeonBoard(int dungeon_index)
{
  struct
  {
    int result;                                 // ebp-0x50c
    uint random_or_flags;                       // ebp-0x508
    int scratch_504;                            // ebp-0x504
    char mana_reward_text[52];                  // ebp-0x500
    int mana_color;                             // ebp-0x4cc
    int has_mana_reward;                        // ebp-0x4c8
    int mana_reward_delta;                      // ebp-0x4c4
    int gold_reward;                            // ebp-0x4c0
    char reward_description[100];               // ebp-0x4bc
    int candidate_card_id;                      // ebp-0x458
    int pre_move_y;                             // ebp-0x454
    int pre_move_x;                             // ebp-0x450
    EncodedImage *button_sprite_entries[0x100]; // ebp-0x44c
    int creature_tier;                          // ebp-0x4c
    int card_slot_attempts;                     // ebp-0x48
    int entry_x;                                // ebp-0x44
    int entry_y;                                // ebp-0x40
    int passage_direction;                      // ebp-0x3c
    int next_ambient_ticks;                     // ebp-0x38
    int board_state;                            // ebp-0x34
    uint event_code;                            // ebp-0x30
    int input_or_sound_code;                    // ebp-0x2c
    int tile_row;                               // ebp-0x28
    uint grid_y;                                // ebp-0x24
    int tile_col;                               // ebp-0x20
    uint grid_x;                                // ebp-0x1c
    int target_y;                               // ebp-0x18
    int ambient_repeat_count;                   // ebp-0x14
    int target_x;                               // ebp-0x10
    int monster_slot;                           // ebp-0xc
    char *ambient_sound_path;                   // ebp-0x8
    int lost_required_duel;                     // ebp-0x4
  } s;

  s.ambient_sound_path = g_dungeon_ambient_sound_path;
  s.ambient_repeat_count = 0;
  s.lost_required_duel = 0;
  if (dungeon_index >= 5)
  {
    AddJournalEntry(JOURNAL_ENTRY_DUNGEON_ENTERED, dungeon_index);
  }
  else
  {
    AddJournalEntry(JOURNAL_ENTRY_CASTLE_DUNGEON_ENTERED, dungeon_index);
  }
  g_dungeon_force_final_encounter = 0;
  SeedRandomFromTickCount();
  InitDungeonScratchPage();
  g_castle_dungeon_slots[dungeon_index].times_entered = g_castle_dungeon_slots[dungeon_index].times_entered + 1;
  g_dungeon_runtime_state.current_dungeon_index = dungeon_index;
  g_dungeon_life_reward_delta = 0;
  for (s.tile_row = 0; s.tile_row < DUNGEON_GRID_HEIGHT; s.tile_row = s.tile_row + 1)
  {
    for (s.tile_col = 0; s.tile_col < DUNGEON_GRID_WIDTH; s.tile_col = s.tile_col + 1)
    {
      *(uint *)((char *)g_dungeon_runtime_state.cell_tile_variants + s.tile_col * 4 + s.tile_row * 0x34) = rand() & 3;
    }
  }
  if (dungeon_index < 5)
  {
    ReadSpriteEntryPointersWithLimit((EncodedImage **)&g_dungeon_sprite_entries,
                                     (&g_dungeon_wizard_sprite_paths_start)[(char)g_castle_dungeon_slots[dungeon_index].color], 0x3c);
  }
  else
  {
    ReadSpriteEntryPointers((EncodedImage **)&g_dungeon_sprite_entries, g_dungeon_sprite_paths[RandomIntLessThan(3)]);
  }
  FUN_00562835("x:sound\\dambloop.wav", 100);
  FUN_0056279e(100, 0x50, 0);
  set_sound_loop(100, 1);
  LoadPcxIntoPage(2, "CaveBkgd.pic");
  ReadSpriteEntryPointers(s.button_sprite_entries, "dungbutt.spr");
  g_dungeon_runtime_state.button_sprite_blob = s.button_sprite_entries[0];
  g_dungeon_runtime_state.button_sprite_aux0 = s.button_sprite_entries[1];
  g_dungeon_runtime_state.button_sprite_aux1 = s.button_sprite_entries[2];
  g_dungeon_runtime_state.button_sprite_aux2 = s.button_sprite_entries[3];
  for (s.monster_slot = 0; s.monster_slot < 5; s.monster_slot = s.monster_slot + 1)
  {
    do
    {
      switch (((g_castle_dungeon_slots[dungeon_index].monster_flags & 0x80) >> 6) + s.monster_slot)
      {
      case 0:
        s.creature_tier = 4;
        break;
      case 1:
        s.creature_tier = 6;
        break;
      case 2:
        s.creature_tier = 8;
        break;
      case 3:
        s.creature_tier = 0xc;
        break;
      case 4:
        s.creature_tier = 0x10;
        break;
      case 5:
        s.creature_tier = 0x12;
        break;
      case 6:
        s.creature_tier = 0x12;
        break;
      }

      if (dungeon_index < 5)
      {
        switch (RandomIntLessThan(6))
        {
        case 0:
          s.creature_tier = 10;
          break;
        case 1:
          s.creature_tier = 0xb;
          break;
        case 2:
          s.creature_tier = 0xc;
          break;
        case 3:
          s.creature_tier = 0xd;
          break;
        case 4:
          s.creature_tier = 0xe;
          break;
        case 5:
          s.creature_tier = 0x10;
          break;
        }

        if (s.monster_slot == 4)
        {
          s.creature_tier = 0x12;
        }
      }
      g_dungeon_runtime_state.encounter.selected.monster_creature_types[s.monster_slot] =
          PickRandomCreatureTypeForWizardTier((int)(char)g_castle_dungeon_slots[dungeon_index].color, s.creature_tier);
    } while (g_dungeon_runtime_state.encounter.selected.monster_creature_types[s.monster_slot] == 0);
    FreeOpeningMenuSpriteWorkEntries(s.monster_slot, s.monster_slot + 8);
    FUN_004bc6d3(g_dungeon_runtime_state.encounter.selected.monster_creature_types[s.monster_slot], s.monster_slot, s.monster_slot + 8);
    if ((dungeon_index < 5) && (s.monster_slot == 4))
    {
      g_dungeon_runtime_state.encounter.selected.monster_creature_types[s.monster_slot] =
          PickRandomCreatureTypeForWizardTier((int)(char)g_castle_dungeon_slots[dungeon_index].color, 0x14);
    }
    g_lair_or_monster_slots[s.monster_slot].entry_type = SHANDALAR_ENTRY_NONE;
  }
  for (s.grid_x = 0; (int)s.grid_x < DUNGEON_GRID_WIDTH; s.grid_x = s.grid_x + 1)
  {
    for (s.grid_y = 0; (int)s.grid_y < DUNGEON_GRID_HEIGHT; s.grid_y = s.grid_y + 1)
    {
      DUNGEON_GRID_CELL(s.grid_x, s.grid_y) = 1;
      if ((((s.grid_x == 0) || (s.grid_y == 0)) || ((int)s.grid_x >= 0xe)) || ((int)s.grid_y >= 0xc))
      {
        DUNGEON_GRID_CELL(s.grid_x, s.grid_y) = 0;
      }
      if (((s.grid_x & 1) == 0) && ((s.grid_y & 1) == 0))
      {
        DUNGEON_GRID_CELL(s.grid_x, s.grid_y) = 0;
      }
      if (((int)s.grid_x < 3) && ((int)s.grid_y < 3))
      {
        DUNGEON_GRID_CELL(s.grid_x, s.grid_y) = 0;
      }
      if (((int)s.grid_x < 3) && ((int)s.grid_y >= 10))
      {
        DUNGEON_GRID_CELL(s.grid_x, s.grid_y) = 0;
      }
      if (((int)s.grid_x >= 12) && ((int)s.grid_y < 3))
      {
        DUNGEON_GRID_CELL(s.grid_x, s.grid_y) = 0;
      }
      if (((int)s.grid_x >= 12) && ((int)s.grid_y >= 10))
      {
        DUNGEON_GRID_CELL(s.grid_x, s.grid_y) = 0;
      }
    }
  }
  s.entry_x = 7;
  g_dungeon_runtime_state.player_x = s.entry_x;
  g_dungeon_exit_x = g_dungeon_runtime_state.player_x;
  s.entry_y = 0xc;
  g_dungeon_runtime_state.player_y = s.entry_y;
  g_dungeon_exit_y = g_dungeon_runtime_state.player_y;
  DUNGEON_GRID_CELL(g_dungeon_runtime_state.player_x, g_dungeon_runtime_state.player_y) = 1;
  do
  {
    s.grid_x = RandomIntLessThan(7) * 2;
    s.grid_y = RandomIntLessThan(6) * 2;
    if (RandomIntLessThan(2) != 0)
    {
      s.grid_x = s.grid_x + 1;
    }
    else
    {
      s.grid_y = s.grid_y + 1;
    }
    memcpy(g_dungeon_grid_backup, g_dungeon_grid, sizeof(g_dungeon_grid));
    DUNGEON_GRID_CELL(s.grid_x, s.grid_y) = 0;
    s.board_state = PopulateDungeonCellEvents();
    if (s.board_state == 0)
    {
      memcpy(g_dungeon_grid, g_dungeon_grid_backup, sizeof(g_dungeon_grid));
    }
    else
    {
    }
  } while (s.board_state < 2);
  g_dungeon_runtime_state.player_y = g_dungeon_runtime_state.player_y + -1;
  RevealDungeonCellAndNeighbors(g_dungeon_runtime_state.player_x, g_dungeon_runtime_state.player_y);
  DrawCastleDungeonBoard(0, 1, dungeon_index);
  WaitForInputEventUnlessBlocked();
  DrawCastleDungeonBoard(0, 0, dungeon_index);
  s.next_ambient_ticks = RandomIntLessThan(0x5a) + 0x5a;
  ConsumeUiTickCount();
  s.board_state = 1;
  do
  {
    if (s.next_ambient_ticks < GetUiTickCount())
    {
      if ((s.ambient_sound_path[0xc] == '5') && (s.ambient_repeat_count != 0))
      {
        s.ambient_repeat_count = s.ambient_repeat_count + -1;
        FUN_00562736(0x65, RandomIntLessThan(0x14) + 0x50, 100, RandomIntLessThan(100) + -0x32);
        s.next_ambient_ticks = RandomIntLessThan(0x3c) + 0x1e;
        ConsumeUiTickCount();
        continue;
      }
      else
      {
        do
        {
          s.input_or_sound_code = RandomIntLessThan(5) + 0x31;
        } while (s.ambient_sound_path[0xc] == s.input_or_sound_code);
        s.ambient_sound_path[0xc] = (char)s.input_or_sound_code;
      }
      sound_unload(0x65);
      LoadSoundWithDriveFallback(s.ambient_sound_path, 0x65, 0);
      FUN_00562736(0x65, RandomIntLessThan(0x14) + 0x50, 100, RandomIntLessThan(100) + -0x32);
      if ((s.ambient_sound_path[0xc] == '5') && (s.ambient_repeat_count == 0))
      {
        s.ambient_repeat_count = RandomIntLessThan(3) + 2;
        s.next_ambient_ticks = RandomIntLessThan(0x1e) + 0x3c;
      }
      else
      {
        s.next_ambient_ticks = RandomIntLessThan(0x5a) + 0x78;
      }
      ConsumeUiTickCount();
    }

    UpdateMouseSnapshot();
    if ((IsKeyInputQueueEmpty() != 0) && (g_mouse_button_mask_snapshot == 0))
    {
    }
    else
    {
      if (g_mouse_button_mask_snapshot != 0)
      {
        s.board_state = QueueDungeonMouseNavigationInput(g_mouse_x_snapshot, g_mouse_y_snapshot);
      }
      s.passage_direction = -1;
      s.input_or_sound_code = PopNormalizedQueuedKeyInput();
      switch (s.input_or_sound_code)
      {
      case 0x4700:
        s.pre_move_x = g_dungeon_runtime_state.player_x;
        s.pre_move_y = g_dungeon_runtime_state.player_y;
        s.target_x = g_dungeon_runtime_state.player_x;
        s.target_y = g_dungeon_runtime_state.player_y + -1;
        g_dungeon_move_direction = 3;
        s.passage_direction = 1;
        goto default_case;
      case 0x4900:
        s.target_y = g_dungeon_runtime_state.player_y;
        s.target_x = g_dungeon_runtime_state.player_x + 1;
        g_dungeon_move_direction = 5;
        s.passage_direction = 3;
        goto default_case;
      case 0x5100:
        s.target_x = g_dungeon_runtime_state.player_x;
        s.target_y = g_dungeon_runtime_state.player_y + 1;
        g_dungeon_move_direction = 7;
        s.passage_direction = 5;
        goto default_case;
      case 0x4f00:
        s.target_y = g_dungeon_runtime_state.player_y;
        s.target_x = g_dungeon_runtime_state.player_x + -1;
        g_dungeon_move_direction = 1;
        s.passage_direction = 7;
        goto default_case;
      default_case:
        if ((((s.target_x >= 0) && (s.target_x < DUNGEON_GRID_WIDTH)) && (s.target_y >= 0)) &&
            ((s.target_y < DUNGEON_GRID_HEIGHT && (DUNGEON_GRID_CELL(s.target_x, s.target_y) != 0))))
        {
          if ((DUNGEON_GRID_CELL(s.target_x, s.target_y) & 0xf0) != 0)
          {
            if (s.passage_direction != 0)
            {
              SetDungeonPassageHighlight(s.passage_direction, 1);
            }
            s.event_code = DUNGEON_GRID_CELL(s.target_x, s.target_y) / 16 & 0xf;
            if (g_dungeon_force_final_encounter != 0)
            {
              s.event_code = 0xf;
            }
            switch (s.event_code)
            {
            case 1:
              PlaySoundEffectOnChannel("x:sound\\dice.wav", 0xf, 100, 100, 0);
              if ((rand() & 1) != 0)
              {
                g_next_duel_card_id = RandomIntLessThan(3) + 1;
                g_dungeon_life_reward_delta = g_dungeon_life_reward_delta + g_next_duel_card_id;
                goto LAB_004443bd;
              }
            LAB_00444355:
              do
              {
                do
                {
                  s.event_code = deck[RandomIntLessThan(500)];
                  s.candidate_card_id = s.event_code & 0xfff;
                } while (s.candidate_card_id < 6);
              } while ((s.event_code & 0x4000) != 0);
              s.event_code &= 0xfff;
              if ((global_cards_data[s.event_code].type & 0x42) == 0)
                goto LAB_00444355;
              g_next_duel_card_id = s.event_code;
            LAB_004443bd:
              strcpy(g_ui_message_buffer, "");
              AppendDungeonRewardDescription();
              strcpy(s.reward_description, g_ui_message_buffer);
              sprintf(g_ui_message_buffer, gs_dungeon_0077f000[0], s.reward_description);
              if (g_next_duel_card_id == -1)
                goto LAB_00444355;
              RunTextMenuAtScaled(g_ui_message_buffer, 100, 0x50);
              DUNGEON_GRID_CELL(s.target_x, s.target_y) = 0x101;
              DrawCastleDungeonBoard(0, 0, dungeon_index);
              break;
            case 2:
              PlaySoundEffectOnChannel("x:sound\\scroll.wav", 0xf, 100, 100, 0);
              if (RunCardRiddleChallenge() == 0)
              {
                PlaySoundEffectOnChannel("x:sound\\dsummon.wav", 0xf, 100, 100, 0);
                DUNGEON_GRID_CELL(s.target_x, s.target_y) = 0x61;
                s.target_x = g_dungeon_runtime_state.player_x;
                s.target_y = g_dungeon_runtime_state.player_y;
                s.passage_direction = 0;
              }
              else
              {
                DUNGEON_GRID_CELL(s.target_x, s.target_y) = 0x101;
              }
              DrawCastleDungeonBoard(0, 0, dungeon_index);
              break;
            case 3:
            case 4:
            case 5:
            case 6:
              if (RunDungeonMonsterDuel(dungeon_index, s.event_code - 3, 0) == 0)
              {
                DUNGEON_GRID_CELL(s.target_x, s.target_y) = 0;
                s.target_x = g_dungeon_runtime_state.player_x;
                s.target_y = g_dungeon_runtime_state.player_y;
                s.passage_direction = 0;
                if ((g_castle_dungeon_slots[dungeon_index].rules_bitmap & 1) != 0)
                {
                  s.board_state = 0;
                  s.lost_required_duel = 1;
                  RefreshAdventureInterfaceLayout();
                }
                AddJournalEntry(JOURNAL_ENTRY_CREATURE_DUEL, g_dungeon_runtime_state.encounter.monster_sound_ids[s.event_code]);
              }
              else
              {
                DUNGEON_GRID_CELL(s.target_x, s.target_y) = 0x101;
                AddJournalEntry(JOURNAL_ENTRY_CREATURE_DUEL,
                                (uint)g_dungeon_runtime_state.encounter.monster_sound_ids[s.event_code] | 0x80);
              }
              DrawCastleDungeonBoard(0, 0, dungeon_index);
              break;
            case 7:
              s.card_slot_attempts = 0;
              do
              {
                s.event_code = RandomIntLessThan(3);
              } while ((&g_castle_dungeon_slots[dungeon_index].card_slot_1)[s.event_code] == -1 && ++s.card_slot_attempts < 100);

              if ((&g_castle_dungeon_slots[dungeon_index].card_slot_1)[s.event_code] != -1)
              {
                PlaySoundEffectOnChannel("x:sound\\findcard.wav", 0xf, 100, 100, 0);
                TransitionPcxToScreenRandomTiles8("staceybk.pic");
                AddJournalEntry(JOURNAL_ENTRY_DUNGEON_TREASURE,
                                (&g_castle_dungeon_slots[dungeon_index].card_slot_1)[s.event_code] | 0x10000);
                DrawAdventureCardSized((&g_castle_dungeon_slots[dungeon_index].card_slot_1)[s.event_code], 0x22, 0x53, 0x4b, 0x70, 1, "");
                DrawTextAt(PTR_DAT_005832b4, 0x1b, 0x90, 0x93, gs_dungeon_0077f000[3]);
                ClearInputAndWaitForMouseRelease();
                WaitForInputEventUnlessBlocked();
                DrawCastleDungeonBoard(0, 0, dungeon_index);
                s.result = AddCardToDeckSorted((&g_castle_dungeon_slots[dungeon_index].card_slot_1)[s.event_code]);
                deck[s.result] = deck[s.result] | 0x4000;
                (&g_castle_dungeon_slots[dungeon_index].card_slot_1)[s.event_code] = -1;
              }
              else
              {
                s.gold_reward = RandomIntLessThan(100) * (g_shandalar_difficulty + 2) + 100;
                s.has_mana_reward = 0;
                s.gold_reward = s.gold_reward - s.gold_reward % 10;
                PlaySoundEffectOnChannel("x:sound\\treasure.wav", 0xf, 100, 100, 0);
                Gold = Gold + s.gold_reward;
                s.mana_reward_text[0] = '\0';
                for (s.mana_color = 1; s.mana_color <= 5; s.mana_color = s.mana_color + 1)
                {
                  s.mana_reward_delta = RandomIntLessThan(2);
                  if (s.mana_reward_delta != 0)
                  {
                    sprintf((char *)((int)s.mana_reward_text + strlen(s.mana_reward_text)), gs_dungeon_0077f000[s.mana_color + 5], s.mana_reward_delta);
                    s.has_mana_reward = 1;
                    g_amulet_inventory[s.mana_color - 1] += s.mana_reward_delta;
                  }
                }
                if (s.has_mana_reward != 0)
                {
                  sprintf(g_ui_message_buffer, gs_dungeon_0077f000[5], s.gold_reward, s.mana_reward_text);
                }
                else
                {
                  sprintf(g_ui_message_buffer, gs_dungeon_0077f000[4], s.gold_reward);
                }
                AddJournalEntry(JOURNAL_ENTRY_DUNGEON_TREASURE, 100);
                RunTextMenuAtScaled(g_ui_message_buffer, 100, 0x50);
              }
              DUNGEON_GRID_CELL(s.target_x, s.target_y) = 0x101;
              DrawCastleDungeonBoard(0, 0, dungeon_index);
              break;
            case 0xf:
              if (dungeon_index < 5)
              {
                s.scratch_504 = RunDungeonMonsterDuel(dungeon_index, 4, 1);
                if (s.scratch_504 == 1)
                {
                  FreeSpriteBlob(g_dungeon_runtime_state.button_sprite_blob);
                  FreeSpriteBlob(g_dungeon_sprite_blob);
                  HandleDefeatedWizardCastle(dungeon_index);
                }
                if (s.scratch_504 != -1)
                {
                  RefreshAdventureInterfaceLayout();
                  FreeDungeonScratchPage();
                  g_next_duel_life_delta = 0;
                  g_next_duel_card_id = -1;
                  g_dungeon_life_reward_delta = 0;
                  return 0;
                }
                else
                {
                  s.passage_direction = 0;
                  s.target_x = g_dungeon_runtime_state.player_x;
                  s.target_y = g_dungeon_runtime_state.player_y;
                  DrawCastleDungeonBoard(0, 0, dungeon_index);
                }
              }
              break;
            }
            RevealDungeonCellAndNeighbors(s.target_x, s.target_y);
            if (g_dungeon_runtime_state.reveal_dirty != 0)
            {
              DrawCastleDungeonBoard(0, 0, dungeon_index);
            }
            SetFocus(g_main_window_hwnd);
          }
          if (s.passage_direction != 0)
          {
            SetDungeonPassageHighlight(s.passage_direction, 0);
          }
          ClearInputAndWaitForMouseRelease();
          g_dungeon_runtime_state.player_x = s.target_x;
          g_dungeon_runtime_state.player_y = s.target_y;
          RevealDungeonCellAndNeighbors(g_dungeon_runtime_state.player_x, g_dungeon_runtime_state.player_y);
          if (g_dungeon_runtime_state.reveal_dirty != 0)
          {
            DrawCastleDungeonBoard(0, 0, dungeon_index);
          }
          if ((s.entry_x == g_dungeon_runtime_state.player_x) && (s.entry_y == g_dungeon_runtime_state.player_y))
          {
            s.board_state = 0;
          }
        }

        break;
      case 0x1b:
        s.board_state = 0;
      default:
        break;
      }
    }

  } while (s.board_state != 0);

  if ((dungeon_index < 5) && (s.lost_required_duel != 0))
  {
    if (IsWizardColorFeminine(dungeon_index + 1) != 0)
    {
      sprintf(g_ui_message_buffer, gs_dungeon_0077f000[0xb],
              gs_wizardnames_0077ee70[dungeon_index + 1]);
    }
    else
    {
      sprintf(g_ui_message_buffer, gs_dungeon_0077f000[0xc],
              gs_wizardnames_0077ee70[dungeon_index + 1]);
    }
    PlaySoundEffectOnChannel("x:sound\\dsummon.wav", 0xf, 100, 100, 0);
    RunTextMenuAtScaled(g_ui_message_buffer, 0xa0, 0x78);
  }
  g_next_duel_life_delta = 0;
  g_next_duel_card_id = -1;
  g_dungeon_life_reward_delta = 0;
  FreeSpriteBlob(g_dungeon_runtime_state.button_sprite_blob);
  FreeSpriteBlob(g_dungeon_sprite_blob);
  RefreshAdventureInterfaceLayout();
  FreeDungeonScratchPage();
  sound_stop(100);
  sound_unload(100);
  if (dungeon_index >= 5)
  {
    ClearWorldMapPixelFlags(0x40, g_castle_dungeon_slots[dungeon_index].world_x,
                            g_castle_dungeon_slots[dungeon_index].world_y);
    g_castle_dungeon_slots[dungeon_index].clues_bitmap &= -2;
    g_castle_dungeon_slots[dungeon_index].clues_bitmap |= 6;
    do
    {
      do
      {
        s.grid_x = RandomIntLessThan(0x40);
        s.grid_y = RandomIntLessThan(0x40);
        s.random_or_flags = FUN_004314ca(s.grid_x, s.grid_y) & 0xf;
      } while (GetWorldTileType(s.grid_x, s.grid_y) == 0);
    } while ((FUN_004314ca(s.grid_x, s.grid_y) & 0x30) != 0);
    g_castle_dungeon_slots[dungeon_index].world_x = s.grid_x;
    g_castle_dungeon_slots[dungeon_index].world_y = s.grid_y;
    SetWorldMapPixelFlags(0x40, s.grid_x, s.grid_y);
  }
}

// FUNCTION: SHANDALAR 0x00446220
int RunDungeonMonsterDuel(int param_1, int param_2, int param_3)
{
  uchar uVar1;
  int iVar2;
  uint name_id;
  char *pcVar3;
  size_t sVar4;
  int iVar5;
  DWORD DVar6;
  uint color_filter;
  int full_card;
  int local_1c;
  int local_18;
  int local_14;
  uint local_10;
  int local_c;
  int local_8;

  for (local_14 = 0; local_14 < 500; local_14 = local_14 + 1)
  {
    local_10 = deck[local_14] & 0xfff;
    if ((g_castle_dungeon_slots[param_1].rules_bitmap & 0x80) != 0)
    {
      DAT_00742fc0 = 0;
      if ((global_cards_data[local_10].type & 0x30) != 0)
      {
        deck[local_14] = deck[local_14] | 0x8000;
      }
    }
    if ((g_castle_dungeon_slots[param_1].rules_bitmap & 0x40) != 0)
    {
      DAT_00742fc0 = 0;
      if ((global_cards_data[local_10].type & 0x40) != 0)
      {
        deck[local_14] = deck[local_14] | 0x8000;
      }
    }
    if ((g_castle_dungeon_slots[param_1].rules_bitmap & 0x10) != 0)
    {
      DAT_00742fc0 = 0;
      if (1 << (byte)g_castle_dungeon_slots[param_1].color == (int)global_cards_data[local_10].color)
      {
        deck[local_14] = deck[local_14] | 0x8000;
      }
    }
  }
  DAT_008ce538 = (int)(char)g_castle_dungeon_slots[param_1].color;
  if (g_castle_dungeon_slots[param_1].card_in_effect != -1)
  {
    unk_00789308 = FUN_0056c705(g_castle_dungeon_slots[param_1].card_in_effect);
  }
  local_c = g_dungeon_runtime_state.encounter.selected.monster_creature_types[param_2];
  DAT_007a7874 = (int)gs_creature_names_00591a08[local_c].base_strength;
  DAT_00742fd0 = 0;
  if (param_2 != 4)
  {
    DAT_007a7d10[0] = 0x10;
  }
  if (param_1 < 5)
  {
    DAT_007a7874 = 2;
    local_18 = g_shandalar_difficulty + param_2 + -3;
    if (-1 < local_18)
    {
      uVar1 = g_castle_dungeon_slots[param_1].color;
      iVar2 = ClampIntToRange(local_18, 0, 2);
      DAT_008cf6d0 = FUN_0056c705(*(int *)(&g_dungeon_monster_duel_music_csvids + ((char)uVar1 * 3 + -3) * 4 + iVar2 * 4));
    }
  }
  iVar2 = -1;
  color_filter = 0;
  name_id = FUN_0056c705(gs_creature_names_00591a08[local_c].deck_number);
  LoadCreatureDuelDeck(local_c, name_id, color_filter, iVar2);
  PlaySoundEffectOnChannel("x:sound\\dngnduel.wav", 0xf, 100, 100, 0);
  if (param_1 < 5)
  {
    pcVar3 = BuildCreatureNameWithArticle(local_c);
    sprintf(g_ui_message_buffer, gs_dungeon_0077f000[0xf], pcVar3);
  }
  else
  {
    pcVar3 = BuildCreatureNameWithArticle(local_c);
    sprintf(g_ui_message_buffer, gs_dungeon_0077f000[0x10], pcVar3);
  }
  if (DAT_008cf6d0 != -1)
  {
    local_18 = g_dungeon_monster_duel_music_csvids[(g_castle_dungeon_slots[param_1].color - 1) * 3 + ClampIntToRange(local_18, 0, 2)];
    FormatMessageFromStringStripCarriageReturns(g_ui_message_buffer + strlen(g_ui_message_buffer), 0x1000, gs_dungeon_0077f000[17], GetCreatureName(local_c), global_cards_data[FUN_0056c705(local_18)].name);
  }
  if (unk_00789308 != -1)
  {
    sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), gs_dungeon_0077f000[0x12], global_cards_data[FUN_0056c705(g_castle_dungeon_slots[param_1].card_in_effect)].name);
  }
  RunTextMenuAtScaled(g_ui_message_buffer, 100, 0x50);
  for (local_18 = 0; local_18 < 0x10; local_18 = local_18 + 1)
  {
    g_duel_ante_card_ids[local_18] = -1;
    global_ante_cards[0][local_18] = g_duel_ante_card_ids[local_18];
  }
  ExitIfNoUsableDeckCards();
  do
  {
    do
    {
      local_18 = RandomIntLessThan(500);
    } while (deck[local_18] == -1);
  } while (((*(byte *)((int)deck + local_18 * 4 + 1) & 0x40) != 0) ||
           ((deck[local_18] & 0xfffU) < 5));
  global_ante_cards[0][0] = deck[local_18] & 0xfff;
  if ((0 < g_next_duel_card_id) && (g_next_duel_card_id <= 5))
  {
    g_next_duel_card_id = -1;
  }
  life[0] = CountDuelPoolEligibleTowns();
  life[0] += g_next_duel_life_delta;
  life[0] = life[0] + g_dungeon_life_reward_delta;
  if ((0 < g_next_duel_card_id) && (g_next_duel_card_id <= 5))
  {
    life[0] = life[0] + g_next_duel_card_id;
  }
  g_dungeon_pre_duel_life_total = life[0];
  if (param_3 != 0)
  {
    sound_stop(100);
    do
    {
      iVar2 = FUN_0056d5c0(100, &local_1c);
      if (iVar2 == 4)
        break;
    } while (local_1c == 1);
    sound_unload(100);
  }
  else
  {
    sound_stop(100);
  }
  AnimatePaletteToColor(0, g_default_palette_fade_steps);
  local_8 = RunDuelEngine(0xffffffff, local_c);
  LoadPcxIntoPageNoPalette("advfac64.pic");
  FadeInPaletteFromGray(0, g_default_palette_fade_steps);
  if ((local_8 != 1) && (local_8 == 0))
  {
    for (local_14 = 0; local_14 < 0x10; local_14 = local_14 + 1)
    {
      if (global_ante_cards[0][local_14] != -1)
      {
        strcpy(g_ui_message_buffer, gs_dungeon_0077f000[0x13]);
        LoadPcxIntoPage(1, "losedul2.pic");
        StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, 0, 0x280, 0x1e0, PTR_DAT_005832b4, 0, 0, global_screen_width,
                                global_screen_height);
        iVar2 = ScaleUiCoordinate(10);
        iVar2 = RandomIntLessThan(iVar2);
        iVar2 = iVar2 + 0x50;
        iVar5 = ScaleUiCoordinate(10);
        iVar5 = RandomIntLessThan(iVar5);
        DrawAdventureCard(global_ante_cards[0][0], iVar5 + local_14 * 0x62 + 0x21, iVar2, 1,
                          g_ui_message_buffer);
        ClearInputAndWaitForMouseRelease();
        WaitForInputEventUnlessBlocked();
        RemoveCardFromDeckById(global_ante_cards[0][local_14]);
      }
    }
  }
  if ((g_castle_dungeon_slots[param_1].rules_bitmap & 1) != 0)
  {
    g_dungeon_life_reward_delta = g_dungeon_life_reward_delta + (life[0] - g_dungeon_pre_duel_life_total);
    g_next_duel_life_delta = 0;
  }
  if ((g_castle_dungeon_slots[param_1].rules_bitmap & 2) != 0)
  {
    g_next_duel_life_delta = 0;
    g_dungeon_life_reward_delta = 0;
  }
  g_next_duel_card_id = -1;
  if ((param_3 == 0) || (local_8 == -1))
  {
    FUN_00562835("x:sound\\dambloop.wav", 100);
    FUN_0056279e(100, 0x50, 0);
    set_sound_loop(100, 1);
  }
  return local_8;
}

// FUNCTION: SHANDALAR 0x0050c0ff
undefined4 HandleDefeatedWizardCastle(int param_1)
{
  struct
  {
    int transition_tile_size;     // ebp - 0x130
    int menu_choice;              // ebp - 0x12c
    char card_browser_title[100]; // ebp - 0x128
    int card_browser_page;        // ebp - 0x28
    uint chosen_card;             // ebp - 0x24
    int wizard_color;             // ebp - 0x20
    int endgame_score;            // ebp - 0x1c
    int loop_index;               // ebp - 0x18
    int town_index;               // ebp - 0x14
    int final_creature;           // ebp - 0x10
    uint selected_cards[3];       // ebp - 0xc
  } s;

  s.wizard_color = (int)(char)g_castle_dungeon_slots[param_1].color;
  AddJournalEntry(JOURNAL_ENTRY_CREATURE_DUEL, (g_castle_defeat_music_ids[param_1] + 1) * 7 | 0x80);
  ShowStatsWindow(1, (int)(char)g_castle_dungeon_slots[param_1].color);
  LoadPcxIntoPageNoPalette("advfac64.pic");
  LoadPcxIntoPage(1, "tradscrn.pic");
  StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, 0, 0x280, 0x1e0, PTR_DAT_005832b4, 0, 0, global_screen_width,
                          global_screen_height);
  PlayCastleMusic(param_1 + 1);
  if (IsWizardColorFeminine(s.wizard_color) != 0)
  {
    sprintf(g_ui_message_buffer, gs_castlewin_0074b8c0[0], gs_wizardnames_0077ee70[s.wizard_color]);
  }
  else
  {
    sprintf(g_ui_message_buffer, gs_castlewin_0074b8c0[1], gs_wizardnames_0077ee70[s.wizard_color]);
  }
  for (s.loop_index = 0; s.loop_index < 0x80; s.loop_index = s.loop_index + 1)
  {
    if ((g_town_slots[s.loop_index].status_and_ruling_wizard & 0xff00) == s.wizard_color << 8)
    {
      g_town_slots[s.loop_index].status_and_ruling_wizard &= -65281;
      strcat(g_ui_message_buffer, BuildTownDisplayName(s.loop_index));
      strcat(g_ui_message_buffer, gs_castlewin_0074b8c0[2]);
    }
  }
  DrawUiScaledCenteredTextWithShadow(PTR_DAT_005832b4, 0xfe, 0x140, 0x72, g_ui_message_buffer);
  g_defeated_wizards_bitmap = g_defeated_wizards_bitmap | 1 << (byte)s.wizard_color;
  s.town_index = FUN_004bb040(g_castle_dungeon_slots[param_1].world_x,
                              g_castle_dungeon_slots[param_1].world_y);
  g_town_slots[s.town_index].location_type = 5;
  for (s.loop_index = 0; s.loop_index < 8; s.loop_index = s.loop_index + 1)
  {
    if (g_lair_or_monster_slots[s.loop_index].color == s.wizard_color)
    {
      FreeOpeningMenuSpriteWorkEntries(s.loop_index, s.loop_index + 8);
      g_lair_or_monster_slots[s.loop_index].entry_type = SHANDALAR_ENTRY_NONE;
    }
  }
  s.card_browser_page = 1;
  sprintf(g_ui_message_buffer, gs_castlewin_0074b8c0[3], gs_colorcards_0077c5e0[s.wizard_color]);
  DrawUiScaledCenteredTextWithShadow(PTR_DAT_005832b4, 0xfe, 0x140, 200, g_ui_message_buffer);
  ClearInputAndWaitForMouseRelease();
  WaitForInputEventUnlessBlocked();
  g_town_dialog_callback = QuestCardChooserCallback;
  ResetMenuContext(BeginMenuContext());
  FUN_005616aa(1);
  g_defeated_wizards_bitmap = g_defeated_wizards_bitmap & ~(1 << (byte)s.wizard_color);

  for (s.loop_index = 0; s.loop_index < 3; s.loop_index = s.loop_index + 1)
  {
    sprintf(s.card_browser_title, gs_castlewin_0074b8c0[4], 3 - s.loop_index);
    s.chosen_card = RunCardBrowser(s.card_browser_title, 1 << (byte)s.wizard_color, 0xffffffff,
                                   s.card_browser_page, 0);
    if (s.chosen_card != 0xffffffff)
    {
      strcpy(g_ui_message_buffer, gs_castlewin_0074b8c0[5]);
      do
      {
        s.menu_choice = RunTextMenuAt(g_ui_message_buffer, ScaleUiCoordinate(0xf4) + 10,
                                      ScaleUiCoordinate(0x15c) + 10);
      } while (s.menu_choice < 0);
      if (s.menu_choice == 0)
      {
        s.town_index = AddCardToDeckSorted(s.chosen_card);
        if (s.town_index != -1)
        {
          deck[s.town_index] = deck[s.town_index] | 0x4000;
        }
        s.selected_cards[s.loop_index] = s.chosen_card;
        global_cards_data[s.chosen_card].extra_ability |= 0x100;
      }
      else
      {
        s.loop_index = s.loop_index + -1;
        continue;
      }
    }
    else
    {
      s.loop_index = s.loop_index + -1;
    }
    DelayUiTicks(0xf);
  }

  for (s.loop_index = 0; s.loop_index < 3; s.loop_index = s.loop_index + 1)
  {
    global_cards_data[s.selected_cards[s.loop_index]].extra_ability &= -257;
  }
  EndMenuContext();
  g_defeated_wizards_bitmap = g_defeated_wizards_bitmap | 1 << (byte)s.wizard_color;
  g_town_dialog_callback = (int(__cdecl *)(void))DrawTiledDialogBoxFrame;
  if (g_defeated_wizards_bitmap != 0x3e)
  {
    sound_stop(0x10);
  }
  AnimatePaletteToColor(0, g_default_palette_fade_steps);
  if (g_defeated_wizards_bitmap == 0x3e)
  {
    DelayUiTicks(0x3c);
    LoadPcxIntoPageOpaque(1, "5thwiz.pic");
    StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, 0, 0x280, 0x1e0, PTR_DAT_005832b4, 0, 0, global_screen_width,
                            global_screen_height);
    strcpy(g_ui_message_buffer, gs_castlewin_0074b8c0[6]);
    *(undefined4 *)((int)PTR_DAT_005832b4 + 0x20) = 5;
    DrawUiScaledCenteredTextWithShadow(PTR_DAT_005832b4, 99, 0x140, 0x100, g_ui_message_buffer);
    ClearInputAndWaitForMouseRelease();
    WaitForInputEventUnlessBlocked();
    s.endgame_score = CalculateDungeonEndgameScore();
    sprintf(g_ui_message_buffer, gs_castlewin_0074b8c0[7], MAX(s.endgame_score, 0));
    strcat(g_ui_message_buffer, gs_castlewin_0074b8c0[8]);
    DrawUiScaledCenteredTextWithShadow(PTR_DAT_005832b4, 99, 0x140, 0x15e, g_ui_message_buffer);
    ClearInputAndWaitForMouseRelease();
    WaitForInputEventUnlessBlocked();
    AnimatePaletteToColor(0, g_default_palette_fade_steps);
    DeckBuilderMain(g_main_window_hwnd, 1, 1);
    RestoreAdventureUiPaletteAndFocus();
    s.final_creature = PickRandomCreatureTypeForWizardTier(0, 100);
    gs_creature_names_00591a08[s.final_creature].tier =
        gs_creature_names_00591a08[s.final_creature].tier *
        (g_shandalar_difficulty + 1);
    LoadCreatureDuelDeck(s.final_creature, 0, 0, -1);
    DAT_008ce538 = s.wizard_color;
    DAT_00742fd0 = 0;
    DAT_007a7874 = 3;
    g_next_duel_life_delta = 0;
    sound_stop(0x10);
    DAT_008cf6d0 = FUN_0056c705(0x11);
    unk_00789308 = FUN_0056c705(0x1d);
    RunDuelEngine(0, s.final_creature);
    AddJournalEntry(JOURNAL_ENTRY_CREATURE_DUEL, 0xb7);
    AnimatePaletteToColor(0, g_default_palette_fade_steps);
    ClearGraphicsPageWithPaletteColor(0, 0);
    LoadPcxIntoPageNoPalette("advfac64.pic");
    PlayStatWinMovie("mtgend.avi", (global_screen_width + -0x230) / 2,
                     (global_screen_height + -0x1a4) / 2, 0);
    SetForegroundWindow(g_main_window_hwnd);
    BringWindowToTop(g_main_window_hwnd);
    SetFocus(g_main_window_hwnd);
    LoadPcxIntoPageNoPalette("advfac64.pic");
    ReadPalette("todpal.tr", (char *)0x0);
    SelectPalette(g_graphics_pages[0]->hTempDC, g_palette_handle, 0);
    RealizePalette(g_graphics_pages[0]->hTempDC);
    PlayCastleMusic(6);
    s.endgame_score = s.endgame_score + (100 - life[1]) * 10;
    LoadPcxIntoPageNoPalette("wingame.pic");
    if (global_screen_width == 0x280)
    {
      s.transition_tile_size = 3;
    }
    else if (global_screen_width == 800)
    {
      s.transition_tile_size = 4;
    }
    else
    {
      s.transition_tile_size = 6;
    }
    TransitionPcxToScreenRandomTiles("wingame.pic", s.transition_tile_size, s.transition_tile_size);
    sprintf(g_ui_message_buffer, gs_castlewin_0074b8c0[9], s.endgame_score);
    strcat(g_ui_message_buffer, gs_castlewin_0074b8c0[10]);
    DrawTextAt(PTR_DAT_005832b4, 0xd8, 0x140, 0x81, g_ui_message_buffer);
    ClearInputAndWaitForMouseRelease();
    WaitForInputEventUnlessBlocked();
    AnimatePaletteToColor(0, g_default_palette_fade_steps);
    LoadPcxIntoPageNoPalette("advfac64.pic");
    FreeDungeonScratchPage();
    RunAdventureStatsMenu();
    g_adventure_world_exit_requested = 1;
    UnloadStatWinDllExports();
    ShutdownSharedStartup();
    exit(0xff);
  }
  return 0;
}

// FUNCTION: SHANDALAR 0x0050a5c1
void EnterCastleDungeon(int dungeon_index)
{
  RunCastleDungeonBoard(dungeon_index);
  DAT_008cf6d0 = -1;
  unk_00789308 = -1;
  return;
}

// FUNCTION: SHANDALAR 0x00446a57
void FreeDungeonScratchPage(void)
{
  BlitGraphicsRect(g_dungeon_scratch_page9, 0, 0, global_screen_width, 0x80, PTR_DAT_00583304, 0, 0);
  FreeGraphicsPage(9);
}

// FUNCTION: SHANDALAR 0x0043125f
void CopyUiScaledGraphicsRect(FacemakerWindowBounds *src_page, int src_x_320, int src_y_240, int width_320,
                              int height_240, FacemakerWindowBounds *dst_page, int dst_x_320, int dst_y_240)
{
  CopyGraphicsRect(src_page, (global_screen_width * src_x_320) / 0x140,
                   (global_screen_height * src_y_240) / 0xf0,
                   (width_320 * global_screen_width) / 0x140,
                   (height_240 * global_screen_height) / 0xf0,
                   dst_page, (dst_x_320 * global_screen_width) / 0x140,
                   (dst_y_240 * global_screen_height) / 0xf0);
}

// FUNCTION: SHANDALAR 0x004312ea
void FillUiScaledGraphicsRect(FacemakerWindowBounds *window_bounds, int x_320, int y_240, int width_320,
                              int height_240, int color_index)
{
  FillGraphicsRect(window_bounds, (x_320 * global_screen_width) / 0x140,
                   (y_240 * global_screen_height) / 0xf0,
                   (width_320 * global_screen_width) / 0x140,
                   (height_240 * global_screen_height) / 0xf0, color_index);
}

// FUNCTION: SHANDALAR 0x0044553d
void DrawCastleDungeonBoard(int animation_step, int initial_draw, int dungeon_index)
{
  struct
  {
    int title_left;
    int title_piece_count;
    int title_middle_width;
    int title_width;
    int title_piece_width;
    int title_end_width;
    int title_tmp;
    int draw_direction;
    int screen_y;
    int screen_x;
    int work;
    int cell_y;
    int monster_direction;
    int cell_x;
    int player_screen_y;
    int player_screen_x;
    int direction;
  } s;

  if (initial_draw == 0)
  {
    PTR_DAT_005832b4->page_number = 1;
  }
  StretchBlitGraphicsRect(PTR_DAT_00583304, 0, 0, 0x280, 0x1e0, PTR_DAT_005832b4, 0, 0,
                          ScaleUiCoordinateFrom320(0x140), ScaleUiCoordinateFrom320(0xf0));
  PTR_DAT_005832b4->font_slot = 4;
  strcpy(g_ui_message_buffer, GetDungeonName(dungeon_index));
  s.title_width = MeasureTextLineWidth(g_ui_message_buffer);
  s.title_piece_count = 0;
  s.title_piece_width = g_dungeon_runtime_state.button_sprite_aux0->width;
  s.title_middle_width = g_dungeon_runtime_state.button_sprite_aux1->width;
  s.title_end_width = g_dungeon_runtime_state.button_sprite_aux2->width;
  s.title_tmp = s.title_end_width + s.title_piece_width + -0x32;
  while (s.title_tmp < s.title_width)
  {
    s.title_piece_count++;
    s.title_tmp += s.title_middle_width;
  }
  s.title_left = ScaleUiCoordinate(0x140) - (s.title_middle_width * s.title_piece_count) / 2 - s.title_piece_width;
  DrawEncodedImageUnscaled(PTR_DAT_005832b4, s.title_left, 1, g_dungeon_runtime_state.button_sprite_aux0);
  s.title_left += s.title_piece_width;
  while (s.title_piece_count-- != 0)
  {
    DrawEncodedImageUnscaled(PTR_DAT_005832b4, s.title_left, 1, g_dungeon_runtime_state.button_sprite_aux1);
    s.title_left += s.title_middle_width;
  }
  DrawEncodedImageUnscaled(PTR_DAT_005832b4, s.title_left, 1, g_dungeon_runtime_state.button_sprite_aux2);
  DrawCenteredTextLineWithShadow(g_ui_message_buffer, ScaleUiCoordinate(0x140), 0x12, 0xff);

  PTR_DAT_005832b4->font_slot = 1;
  if (g_next_duel_card_id != -1)
  {
    strcpy(g_ui_message_buffer, "");
    AppendDungeonRewardDescription();
    DrawFormattedTextShadowedCentered(PTR_DAT_005832b4, 0xd0, ScaleUiCoordinate(0x140), 0x30, g_ui_message_buffer);
  }

  for (s.cell_x = DUNGEON_GRID_WIDTH - 1; s.cell_x >= 0; s.cell_x--)
  {
    for (s.cell_y = 0; s.cell_y < DUNGEON_GRID_HEIGHT; s.cell_y++)
    {
      ConvertDungeonCellToScreen(s.cell_x, s.cell_y, &s.screen_x, &s.screen_y);
      if ((animation_step == 0) && ((DUNGEON_GRID_CELL(s.cell_x, s.cell_y) & 0x100) == 0))
      {
        if ((s.screen_x >= 0) && (s.screen_y >= 0) && (s.screen_x <= 0x240) && (s.screen_y <= 0x190))
        {
          if (DUNGEON_GRID_CELL(s.cell_x, s.cell_y) == 0)
          {
            s.work = 0;
            for (s.direction = 1; s.direction <= 8; s.direction += 2)
            {
              if ((DUNGEON_GRID_CELL(s.cell_x + g_neighbor_dx[s.direction],
                                     s.cell_y + g_neighbor_dy[s.direction]) &
                   0x100) != 0)
              {
                s.work++;
              }
            }
            if (s.work > 1)
            {
              DrawEncodedImageUiScaled(PTR_DAT_005832b4, s.screen_x - 0x20, s.screen_y - 0x30,
                                       g_dungeon_sprite_entries[0x14], 0x40, 0x40);
            }
          }
        }
        continue;
      }
      if (DUNGEON_GRID_CELL(s.cell_x, s.cell_y) != 0)
      {
        s.work = 0;
        if (DUNGEON_GRID_CELL(s.cell_x + 1, s.cell_y) == 0)
        {
          s.work |= 1;
        }
        if (DUNGEON_GRID_CELL(s.cell_x, s.cell_y - 1) == 0)
        {
          s.work |= 2;
        }
        switch (((int (*)[DUNGEON_GRID_HEIGHT])g_dungeon_runtime_state.cell_tile_variants)[s.cell_x][s.cell_y])
        {
        case 0:
          break;
        case 1:
          s.work += 4;
          break;
        case 2:
          s.work += 0x18;
          break;
        case 3:
          s.work += 0x1c;
          break;
        }
        DrawEncodedImageUiScaled(PTR_DAT_005832b4, s.screen_x - 0x20, s.screen_y - 0x30,
                                 g_dungeon_sprite_entries[s.work], 0x40, 0x40);

        s.work = 0;
        if (DUNGEON_GRID_CELL(s.cell_x, s.cell_y + 1) == 0)
        {
          s.work |= 1;
        }
        if (DUNGEON_GRID_CELL(s.cell_x - 1, s.cell_y) == 0)
        {
          s.work |= 2;
        }
        if (s.cell_y == 0xc)
        {
          s.work |= 1;
        }
        if ((s.cell_y & 1) != 0)
        {
          s.work += 4;
        }
        DrawEncodedImageUiScaled(PTR_DAT_005832b4, s.screen_x - 0x20, s.screen_y - 0x30,
                                 g_dungeon_sprite_entries[s.work + 0xc], 0x40, 0x40);
      }

      strcpy(g_ui_message_buffer, "");
      if (((DUNGEON_GRID_CELL(s.cell_x, s.cell_y) & 0xf0) != 0) || (s.cell_y == 0xc))
      {
        s.work = (DUNGEON_GRID_CELL(s.cell_x, s.cell_y) / 16) & 0xf;
        switch (s.work)
        {
        case 1:
          DrawEncodedImageUiScaled(PTR_DAT_005832b4, s.screen_x - 0x20, s.screen_y - 0x30,
                                   g_dungeon_sprite_entries[10], 0x40, 0x40);
          break;
        case 2:
          DrawEncodedImageUiScaled(PTR_DAT_005832b4, s.screen_x - 0x20, s.screen_y - 0x30,
                                   g_dungeon_sprite_entries[9], 0x40, 0x40);
          break;
        case 3:
          break;
          FillUiScaledGraphicsRect(PTR_DAT_005832b4, s.screen_x + 2, s.screen_y + 2, 0xc, 0xc, 0xd8);
          break;
        case 4:
          break;
          FillUiScaledGraphicsRect(PTR_DAT_005832b4, s.screen_x + 2, s.screen_y + 2, 0xc, 0xc, 0xdc);
          break;
        case 5:
          break;
          FillUiScaledGraphicsRect(PTR_DAT_005832b4, s.screen_x + 2, s.screen_y + 2, 0xc, 0xc, 0xbc);
          break;
        case 6:
          break;
          FillUiScaledGraphicsRect(PTR_DAT_005832b4, s.screen_x + 2, s.screen_y + 2, 0xc, 0xc, 0xbe);
          break;
        case 7:
          DrawEncodedImageUiScaled(PTR_DAT_005832b4, s.screen_x - 0x20, s.screen_y - 0x30,
                                   g_dungeon_sprite_entries[8], 0x40, 0x40);
          break;
        case 0xf:
          DrawEncodedImageUiScaled(PTR_DAT_005832b4, s.screen_x - 0x20, s.screen_y - 0x30,
                                   g_dungeon_sprite_entries[0x31], 0x40, 0x40);
          break;
        }

        if ((s.work >= 3) && (s.work < 7))
        {
          s.work -= 3;
          strcpy(g_ui_message_buffer, GetCreatureName(g_dungeon_runtime_state.encounter.selected.monster_creature_types[s.work]));
          s.draw_direction = 5;
          for (s.monster_direction = 1; s.monster_direction <= 8; s.monster_direction += 2)
          {
            s.direction = DUNGEON_CELL_DISTANCE(s.cell_x + g_neighbor_dx[s.monster_direction],
                                                s.cell_y + g_neighbor_dy[s.monster_direction]);
            if ((s.direction != 0) && (s.direction < DUNGEON_CELL_DISTANCE(s.cell_x, s.cell_y)))
            {
              s.draw_direction = s.monster_direction;
            }
          }
          DrawEncodedImageUnscaled(PTR_DAT_005832b4,
                                   ScaleUiCoordinate(s.screen_x + 4) - g_world_lair_monster_sprite_widths[s.work] / 2,
                                   ScaleUiCoordinate(s.screen_y + 4) - g_world_lair_monster_sprite_top_clips[s.work],
                                   g_opening_menu_sprite_work_buffer[s.work + 8].sprites[((s.draw_direction + 2) & 7) * 5]);
          DrawEncodedImageUnscaled(PTR_DAT_005832b4,
                                   ScaleUiCoordinate(s.screen_x + 4) - g_world_lair_monster_sprite_widths[s.work] / 2,
                                   ScaleUiCoordinate(s.screen_y + 4) - g_world_lair_monster_sprite_top_clips[s.work],
                                   g_opening_menu_sprite_work_buffer[s.work].sprites[((s.draw_direction + 2) & 7) * 5]);
        }
        else
        {
          if (s.cell_y == 0xc)
          {
            strcpy(g_ui_message_buffer, gs_dungeon_0077f000[0xd]);
          }
          DrawUiScaledCenteredText(g_ui_message_buffer, s.screen_x / 2, s.screen_y / 2 - 2, 0xff);
        }
      }

      if (g_dungeon_runtime_state.player_x == s.cell_x)
      {
        if (s.cell_y == g_dungeon_runtime_state.player_y)
        {
          s.player_screen_x = s.screen_x;
          s.player_screen_y = s.screen_y;
        }
      }
    }
  }

  if ((initial_draw != 0) && (g_castle_dungeon_slots[dungeon_index].card_in_effect != -1))
  {
    s.work = FUN_0056c705(g_castle_dungeon_slots[dungeon_index].card_in_effect);
    sprintf(g_ui_message_buffer, gs_dungeon_0077f000[0xe], global_cards_data[s.work].name);
    DrawEncodedImageResampled(PTR_DAT_005832b4, ScaleUiCoordinateFrom320(0xdc) / 2, ScaleUiCoordinateFrom320(0x34) / 2,
                              ScaleUiCoordinateFrom320(0xc5) / 2, ScaleUiCoordinateFrom320(0x10f) / 2,
                              g_dungeon_runtime_state.button_sprite_blob);
    DrawAdventureCardSized(s.work, 0x7a, 0x29, 0x4b, 0x70, 1, "");
    DrawScaledTextNoShadow(g_ui_message_buffer, 0x76, 0x20, 0x1b);
  }

  PTR_DAT_005832b4->page_number = 0;
  if (initial_draw == 0)
  {
    CopyGraphicsRect(PTR_DAT_005832dc, 0, 0, global_screen_width, global_screen_height, PTR_DAT_005832b4, 0, 0);
  }
  else
  {
    CopyGraphicsRect(PTR_DAT_005832b4, 0, 0, global_screen_width, global_screen_height, PTR_DAT_005832dc, 0, 0);
  }
  DrawEncodedImageUnscaled(PTR_DAT_005832b4,
                           ScaleUiCoordinate(s.player_screen_x) - g_sego_sprite_width / 2,
                           ScaleUiCoordinate(s.player_screen_y) - g_sego_sprite_draw_height,
                           g_opening_menu_sprite_work_buffer[0x11].sprites[g_dungeon_move_direction * 5]);
  DrawEncodedImageUnscaled(PTR_DAT_005832b4,
                           ScaleUiCoordinate(s.player_screen_x) - g_ego_sprite_width / 2,
                           ScaleUiCoordinate(s.player_screen_y) - g_ego_sprite_draw_height,
                           g_opening_menu_sprite_work_buffer[0x10].sprites[g_dungeon_move_direction * 5]);
}

// FUNCTION: SHANDALAR 0x00446049
void ConvertDungeonCellToScreen(int dungeon_x, int dungeon_y, int *out_screen_x, int *out_screen_y)
{
  *out_screen_x = dungeon_y * 0x20 + dungeon_x * 0x20 - 0x60;
  *out_screen_y = dungeon_x * -0x10 + dungeon_y * 0x10 + 0x110;
}

// FUNCTION: SHANDALAR 0x00446084
void ConvertMouseToDungeonCell(int mouse_x, int mouse_y, int *out_x, int *out_y)
{
  mouse_x = (mouse_x * 0x280) / global_screen_width;
  mouse_y = (mouse_y - 0x10) * 480 / global_screen_height;
  *out_x = ((mouse_x + 96) / 2 - (mouse_y - 272)) / 32;
  *out_y = (mouse_y + (mouse_x + 96) / 2 - 272) / 32 + 1;
}

// FUNCTION: SHANDALAR 0x00446101
void RevealDungeonCellAndNeighbors(int x, int y)
{
  struct
  {
    int neighbor_cell;
    int neighbor_y;
    int neighbor_x;
    int direction_index;
  } s;

  DUNGEON_GRID_CELL(x, y) = DUNGEON_GRID_CELL(x, y) | 0x100;
  g_dungeon_runtime_state.reveal_dirty = 0;
  for (s.direction_index = 1; s.direction_index <= 8; s.direction_index = s.direction_index + 2)
  {
    s.neighbor_x = x;
    s.neighbor_y = y;
    while (1)
    {
      s.neighbor_x = s.neighbor_x + g_neighbor_dx[s.direction_index];
      s.neighbor_y = s.neighbor_y + g_neighbor_dy[s.direction_index];
      if ((s.neighbor_x < 0) || (s.neighbor_x >= DUNGEON_GRID_WIDTH))
      {
        break;
      }
      if ((s.neighbor_y < 0) || (s.neighbor_y >= DUNGEON_GRID_HEIGHT))
      {
        break;
      }
      s.neighbor_cell = DUNGEON_GRID_CELL(s.neighbor_x, s.neighbor_y);
      if (s.neighbor_cell == 0)
      {
        break;
      }
      if ((DUNGEON_GRID_CELL(s.neighbor_x, s.neighbor_y) & 0x100) == 0)
      {
        g_dungeon_runtime_state.reveal_dirty = 1;
      }
      DUNGEON_GRID_CELL(s.neighbor_x, s.neighbor_y) = DUNGEON_GRID_CELL(s.neighbor_x, s.neighbor_y) | 0x100;
    }
  }
}

// FUNCTION: SHANDALAR 0x00445365
void SetDungeonPassageHighlight(int direction, int enable)
{
  struct
  {
    int ego_sprite_frame;
    int sego_sprite_frame;
    int draw_x;
    int screen_y;
    int screen_x;
    int frame;
    int draw_y;
  } s;

  ConvertDungeonCellToScreen(g_dungeon_runtime_state.player_x, g_dungeon_runtime_state.player_y, &s.screen_x, &s.screen_y);
  for (s.frame = 1; s.frame <= ((enable == 0) ? 8 : 4); s.frame++)
  {
    s.draw_x = s.screen_x - g_ego_sprite_width / 2;
    s.draw_y = s.screen_y - g_ego_sprite_draw_height;
    CopyUiScaledGraphicsRect(PTR_DAT_005832dc, s.draw_x / 2, s.draw_y / 2, g_ego_sprite_width / 2,
                             g_ego_sprite_height / 2, PTR_DAT_005832b4, s.draw_x / 2, s.draw_y / 2);
    s.draw_x = ScaleUiCoordinate(s.screen_x) - g_ego_sprite_width / 2;
    s.draw_y = ScaleUiCoordinate(s.screen_y) - g_ego_sprite_draw_height;
    if (s.frame == 8)
    {
      s.sego_sprite_frame = 0;
    }
    else
    {
      s.sego_sprite_frame = (s.frame & 3) + 1;
    }
    DrawEncodedImageUnscaled(PTR_DAT_005832b4, s.draw_x, s.draw_y,
                             g_opening_menu_sprite_work_buffer[0x11].sprites[g_dungeon_move_direction * 5 + s.sego_sprite_frame]);
    if (s.frame == 8)
    {
      s.ego_sprite_frame = 0;
    }
    else
    {
      s.ego_sprite_frame = (s.frame & 3) + 1;
    }
    DrawEncodedImageUnscaled(PTR_DAT_005832b4, s.draw_x, s.draw_y,
                             g_opening_menu_sprite_work_buffer[0x10].sprites[g_dungeon_move_direction * 5 + s.ego_sprite_frame]);
    s.screen_x += (g_neighbor_dx[((direction - 2) & 7) + 1] << 4) / 4;
    s.screen_y += (g_neighbor_dy[((direction - 2) & 7) + 1] << 4) / 8;
    DelayUiTicks(5);
    ClearInputAndWaitForMouseRelease();
  }
}

// FUNCTION: SHANDALAR 0x004ed8d3
undefined4 TransitionPcxToScreenRandomTiles8(char *path)
{
  DIBSurface *work_page;
  DIBSurface *screen_page;

  work_page = g_graphics_pages[PTR_DAT_005832dc->page_number];
  screen_page = g_graphics_pages[PTR_DAT_005832b4->page_number];
  LoadPcxResource(1, 0, global_screen_height + -0x1e0, path, (void *)0);
  StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, global_screen_height + -0x1e0, 0x280, 0x1e0, PTR_DAT_005832dc, 0,
                          0, global_screen_width, global_screen_height);
  BlitRectByRandomTileOrderInPlace(screen_page->hTempDC, 0, 0, global_screen_width, global_screen_height, 8, 8,
                                   work_page->hTempDC);
  return 0;
}

// FUNCTION: SHANDALAR 0x00446922
void AppendDungeonRewardDescription(void)
{
  int town_reward_count = CountDuelPoolEligibleTowns();
  if (g_next_duel_card_id == 5)
  {
    g_next_duel_card_id = -1;
  }

  if ((g_next_duel_card_id > 0) && (g_next_duel_card_id <= 5))
  {
    sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), gs_dungeon_0077f000[1], g_next_duel_card_id, g_dungeon_life_reward_delta + g_next_duel_life_delta + town_reward_count);
  }
  else if (g_next_duel_card_id == 0)
  {
    strcat(g_ui_message_buffer, gs_dungeon_0077f000[2]);
  }
  else if (5 < g_next_duel_card_id)
  {
    strcat(g_ui_message_buffer, global_cards_data[g_next_duel_card_id].name);
  }
}

// FUNCTION: SHANDALAR 0x00446b89
int QueueDungeonMouseNavigationInput(int mouse_x, int mouse_y)
{
  struct
  {
    int click_grid_x;
    int key_codes[8];
    int current_screen_y;
    int current_screen_x;
    int abs_dy;
    int abs_dx;
    int direction_base;
    int click_grid_y;
  } s;

  s.key_codes[0] = 0x4800;
  s.key_codes[1] = 0x4900;
  s.key_codes[2] = 0x4d00;
  s.key_codes[3] = 0x5100;
  s.key_codes[4] = 0x5000;
  s.key_codes[5] = 0x4f00;
  s.key_codes[6] = 0x4b00;
  s.key_codes[7] = 0x4700;
  ConvertMouseToDungeonCell(mouse_x, mouse_y, &s.click_grid_x, &s.click_grid_y);
  if (s.click_grid_x == g_dungeon_exit_x && s.click_grid_y == g_dungeon_exit_y)
  {
    PushQueuedKeyInput(0x1b);
    return 0;
  }
  ConvertDungeonCellToScreen(g_dungeon_runtime_state.player_x, g_dungeon_runtime_state.player_y,
                             &s.current_screen_x, &s.current_screen_y);
  s.current_screen_x = ScaleUiCoordinate(s.current_screen_x);
  s.current_screen_y = ScaleUiCoordinate(s.current_screen_y);
  mouse_x = mouse_x - s.current_screen_x;
  mouse_y = s.current_screen_y - mouse_y;
  s.abs_dx = abs(mouse_x);
  s.abs_dy = abs(mouse_y);
  if ((mouse_x >= 0) && (mouse_y >= 0))
  {
    s.direction_base = 0;
  }
  else if ((mouse_x >= 0) && (mouse_y < 0))
  {
    s.direction_base = 2;
  }
  else if ((mouse_x < 0) && (mouse_y < 0))
  {
    s.direction_base = 4;
  }
  else if ((mouse_x < 0) && (mouse_y >= 0))
  {
    s.direction_base = 6;
  }
  PushQueuedKeyInput(s.key_codes[s.direction_base + 1]);
  return 1;
}

// FUNCTION: SHANDALAR 0x00445256
static void MarkDungeonDistanceFromCell(int x, int y, int distance)
{
  struct
  {
    int direction;
    int neighbor_y;
    int neighbor_x;
  } s;

  distance++;
  DUNGEON_CELL_DISTANCE(x, y) = (char)distance;
  for (s.direction = 1; s.direction <= 8; s.direction += 2)
  {
    s.neighbor_x = x + g_neighbor_dx[s.direction];
    s.neighbor_y = y + g_neighbor_dy[s.direction];
    if (DUNGEON_GRID_CELL(s.neighbor_x, s.neighbor_y) == 0)
    {
      continue;
    }
    if ((DUNGEON_CELL_DISTANCE(s.neighbor_x, s.neighbor_y) != 0) &&
        (DUNGEON_CELL_DISTANCE(s.neighbor_x, s.neighbor_y) <= distance))
    {
      continue;
    }
    if ((((s.neighbor_x < 0) || (s.neighbor_x >= DUNGEON_GRID_WIDTH)) || (s.neighbor_y < 0)) ||
        (s.neighbor_y >= DUNGEON_GRID_HEIGHT))
    {
      continue;
    }
    MarkDungeonDistanceFromCell(s.neighbor_x, s.neighbor_y, distance);
  }
}

// FUNCTION: SHANDALAR 0x00444cef
int PopulateDungeonCellEvents(void)
{
  struct
  {
    int connected_count;
    int diagonal_count;
    int min_dead_end_depth;
    int previous_row;
    int generation_ok;
    int event_code;
    int y;
    int direction;
    int x;
    int max_reachable_depth;
    int branch_count;
  } s;

  for (s.x = 0; s.x < DUNGEON_GRID_WIDTH; s.x++)
  {
    for (s.y = 0; s.y < DUNGEON_GRID_HEIGHT; s.y++)
    {
      DUNGEON_CELL_DISTANCE(s.x, s.y) = 0;
    }
  }
  MarkDungeonDistanceFromCell(g_dungeon_runtime_state.player_x, g_dungeon_runtime_state.player_y, 0);

  s.min_dead_end_depth = 0x63;
  s.max_reachable_depth = 0;
  s.generation_ok = 1;
  s.previous_row = 0;
  s.branch_count = s.previous_row;
  for (s.y = 0; s.y < DUNGEON_GRID_HEIGHT; s.y++)
  {
    for (s.x = 0; s.x < DUNGEON_GRID_WIDTH; s.x++)
    {
      if (DUNGEON_GRID_CELL(s.x, s.y) == 0)
      {
        continue;
      }
      if ((s.x == g_dungeon_runtime_state.player_x) && (s.y == g_dungeon_runtime_state.player_y))
      {
        continue;
      }
      if (DUNGEON_CELL_DISTANCE(s.x, s.y) == 0)
      {
        s.generation_ok = 0;
      }
      s.connected_count = 0;
      for (s.direction = 1; s.direction <= 8; s.direction += 2)
      {
        if (DUNGEON_GRID_CELL(s.x + g_neighbor_dx[s.direction], s.y + g_neighbor_dy[s.direction]) != 0)
        {
          s.connected_count++;
        }
      }
      if (s.connected_count == 1)
      {
        if ((DUNGEON_CELL_DISTANCE(s.x, s.y) < 0x10) || (s.previous_row == s.y))
        {
          s.generation_ok = 0;
        }
        else if (s.previous_row != s.y)
        {
          s.branch_count++;
          s.previous_row = s.y;
          if (DUNGEON_CELL_DISTANCE(s.x, s.y) < s.min_dead_end_depth)
          {
            s.min_dead_end_depth = DUNGEON_CELL_DISTANCE(s.x, s.y);
          }
          if (DUNGEON_CELL_DISTANCE(s.x, s.y) > s.max_reachable_depth)
          {
            s.max_reachable_depth = DUNGEON_CELL_DISTANCE(s.x, s.y);
          }
        }
      }
    }
  }
  if ((s.generation_ok != 0) && (2 < s.branch_count))
  {
    s.previous_row = 0;
    s.branch_count = s.previous_row;
    for (s.y = 0; s.y < DUNGEON_GRID_HEIGHT; s.y++)
    {
      for (s.x = 0; s.x < DUNGEON_GRID_WIDTH; s.x++)
      {
        if (DUNGEON_GRID_CELL(s.x, s.y) == 0)
        {
          continue;
        }
        if ((s.x == g_dungeon_runtime_state.player_x) && (s.y == g_dungeon_runtime_state.player_y))
        {
          continue;
        }
        s.diagonal_count = 0;
        s.connected_count = 0;
        for (s.direction = 1; s.direction <= 8; s.direction += 2)
        {
          if (DUNGEON_GRID_CELL(s.x + g_neighbor_dx[s.direction], s.y + g_neighbor_dy[s.direction]) != 0)
          {
            s.connected_count++;
          }
          if (DUNGEON_GRID_CELL(s.x + g_neighbor_dx[s.direction + 1], s.y + g_neighbor_dy[s.direction + 1]) != 0)
          {
            s.diagonal_count++;
          }
        }
        if (s.connected_count == 1)
        {
          if ((DUNGEON_CELL_DISTANCE(s.x, s.y) >= 0x10) && (s.previous_row == s.y))
          {
            s.generation_ok = 0;
          }
          else if (s.previous_row != s.y)
          {
            s.branch_count++;
            s.previous_row = s.y;
            if ((g_dungeon_runtime_state.current_dungeon_index < 5) && (s.branch_count == 1))
            {
              DUNGEON_GRID_CELL(s.x, s.y) |= 0xf0;
            }
            else
            {
              DUNGEON_GRID_CELL(s.x, s.y) |= 0x70;
            }
          }
        }
        else
        {
          s.event_code = g_castle_dungeon_slots[g_dungeon_runtime_state.current_dungeon_index].times_entered +
                         g_shandalar_difficulty;
          if (s.event_code > 2)
          {
            s.event_code = 3;
          }
          if ((((s.min_dead_end_depth - DUNGEON_CELL_DISTANCE(s.x, s.y)) + 1) % (5 - s.event_code)) == 0)
          {
            if ((DUNGEON_CELL_DISTANCE(s.x, s.y) < s.max_reachable_depth) && (DUNGEON_CELL_DISTANCE(s.x, s.y) >= 8))
            {
              s.event_code = ClampIntToRange(((DUNGEON_CELL_DISTANCE(s.x, s.y) - s.min_dead_end_depth) / 4) + -2 +
                                                 RandomIntLessThan(6) + s.diagonal_count + s.connected_count,
                                             0, 6);
              if (RandomIntLessThan(g_shandalar_difficulty + 3) == 0)
              {
                s.event_code = 1;
              }
              DUNGEON_GRID_CELL(s.x, s.y) |= s.event_code << 4;
            }
          }
        }
      }
    }
    return 2;
  }
  return s.generation_ok;
}

// FUNCTION: SHANDALAR 0x004469f7
void InitDungeonScratchPage(void)
{
  SetGraphicsPage(9, CreateGraphicsPage(9, global_screen_width, 0x81, 8));
  g_dungeon_scratch_page9->font_slot = 1;
  BlitGraphicsRect(PTR_DAT_00583304, 0, 0, global_screen_width, 0x80, g_dungeon_scratch_page9, 0, 0);
}

// FUNCTION: SHANDALAR 0x004ed98c
undefined4 TransitionPcxToScreenRandomTiles(char *path, int x_scale, int y_scale)
{
  DIBSurface *work_page;
  DIBSurface *screen_page;

  work_page = g_graphics_pages[PTR_DAT_005832dc->page_number];
  screen_page = g_graphics_pages[PTR_DAT_005832b4->page_number];
  LoadPcxResource(1, 0, global_screen_height + -0x1e0, path, (void *)0);
  StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, global_screen_height + -0x1e0, 0x280, 0x1e0, PTR_DAT_005832dc, 0,
                          0, global_screen_width, global_screen_height);
  BlitRectByRandomTileOrderInPlace(screen_page->hTempDC, 0, 0, global_screen_width, global_screen_height, x_scale,
                                   y_scale, work_page->hTempDC);
  return 0;
}

// FUNCTION: SHANDALAR 0x005628bf
void PlayCastleMusic(int param_1)
{
  StopWorldLocationMusic();
  if (g_world_location_music_track_id != -1)
  {
    sound_unload(0x10);
  }
  g_world_location_music_track_id = param_1 + 0x15;
  switch (param_1)
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
  case 6:
    FUN_00562835("x:sound\\wingame.wav", 0x10);
    break;
  }
  FUN_0056279e(0x10, 100, 0);
  g_world_location_music_active = 1;
}

// FUNCTION: SHANDALAR 0x0052283d
undefined4 PlayStatWinMovie(char *path, int x, int y, int flags)
{
  if (g_statwin_exports_by_ordinal[2] != 0)
  {
    return ((int(__cdecl *)(char *, int, int, int))g_statwin_exports_by_ordinal[2])(path, x, y, flags);
  }

  return 0;
}

// FUNCTION: SHANDALAR 0x0050c03d
int CalculateDungeonEndgameScore(void)
{
  struct
  {
    int score;
    int victory_value;
    int index;
  } s;

  s.score = -g_wizard_siege_count;
  for (s.index = 0; s.index < 0x80; s.index = s.index + 1)
  {
    if (g_town_slots[s.index].status_and_ruling_wizard & 0xff00)
    {
      s.score = s.score + -2;
    }
  }
  s.score = s.score * 3;
  for (s.index = 0; s.index < 1000; s.index = s.index + 1)
  {
    if ((char)g_duel_victory_log[s.index] != '\0')
    {
      s.victory_value = (int)(char)g_duel_victory_log[s.index] & 0xf;
      s.score += s.victory_value + 2;
      if (s.victory_value == 0xc)
      {
        s.score += 0x19;
      }
    }
  }
  s.score <<= 1;
  return s.score;
}
