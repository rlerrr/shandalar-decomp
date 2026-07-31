#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "shandalar.h"
#include "magic/src/global_state.h"
#include "shandalar_global_strings.h"
#include "facemaker/src/facemaker_types.h"
#include "drawcardlib/src/pic.h"

extern int global_screen_width;
extern int global_screen_height;
extern int g_neighbor_dx[9];
extern int g_neighbor_dy[9];
extern int g_world_player_animation_direction;
extern int g_world_player_animation_frame;
extern int g_default_palette_fade_steps;
extern int g_adventure_world_exit_requested;

extern int g_world_player_tile_x;
extern int g_world_player_tile_y;
extern WorldMagicSlotTimer g_world_magic_slot_timers[0xc];
extern OpeningMenuSpriteWorkEntry g_opening_menu_sprite_work_buffer[0x20];
extern int g_world_lair_monster_sprite_widths[0x10];
extern int g_world_lair_monster_sprite_heights[0x10];
extern int g_world_lair_monster_sprite_top_clips[0x10];

#define OPENING_MENU_WORK_ENTRY_SPRITES(entry_index) (g_opening_menu_sprite_work_buffer[(entry_index)].sprites)

extern FacemakerWindowBounds *PTR_DAT_005832b4;
extern FacemakerWindowBounds *PTR_DAT_005832dc;

extern int g_wizard_siege_count;
extern char g_ui_message_buffer[0x1000];
extern char g_ini_string_scratch[0x28];

void FreeOpeningMenuSpriteWorkEntries(int work_entry_index_a, int work_entry_index_b);
char *BuildResolutionSpritePath(char *sprite_filename);
int ReadSpriteEntryPointers(EncodedImage **out_sprite_entries, char *sprite_path);
ShandalarEntryType PickRandomCreatureTypeForWizardTier(int wizard_color, int creature_tier);
int FindNearestTownIndex(int world_x, int world_y);
int ClampIntToRange(int value, int min_value, int max_value);
int ApproximateDistance(int x, int y);
int RandomIntLessThan(int param_1);
int single_color_test_bit_to_color_t(int mask);
unsigned int GetWorldTileType(int x, int y);
unsigned int FUN_004314ca(int x, int y);
void SetWorldMapPixelFlags(unsigned int mask, int x, int y);
unsigned int GetWorldTileMagicMask(unsigned int tile_mask);
void AddJournalEntry(int entry_type, int entry_arg);
void SaveGameToSlot(int save_slot_index);
void EnsureAdvfac64Loaded(int state);
void RefreshAdventureInterfaceLayout(void);
void FUN_00562736(int param_1, int param_2, int param_3, int param_4);
void PlaySoundEffectOnChannel(char *sound_path, int channel, int volume, int pitch_percent, int pan_percent);
void AnimateVisitBackdropZoomIn(char *pcx_path);
void DrawTextAt(FacemakerWindowBounds *window, int color, int x, int y, char *text, ...);
char *BuildTownDisplayName(int town_index);
char *BuildCreatureNameWithArticle(int creature_type);
DWORD FormatMessageFromStringStripCarriageReturns(char *dst, DWORD max_length, LPCVOID format, ...);
unsigned int WaitForInputEventUnlessBlocked(void);
void ClearInputAndWaitForMouseRelease(void);
int IsWizardColorFeminine(int color_index);
void ShowStatsWindow(int mode, int highlight);
void AnimatePaletteToColor(int color_index, int palette_id);
void LoadPcxIntoPageOpaque(int page_number, char *path);
int ScaleUiCoordinate(int value);
void SetFontStyleSize(int font_slot, unsigned int scaled_size);
int GetFontLineHeight(int font_slot);
void DrawFormattedTextShadowedCentered(FacemakerWindowBounds *window, int color_index, int x, int y, char *format, ...);
void StretchBlitGraphicsRect(FacemakerWindowBounds *dst, int dst_x, int dst_y, int src_w, int src_h,
                             FacemakerWindowBounds *src, int src_x, int src_y, int copy_w, int copy_h);
void UnloadStatWinDllExports(void);
int ShutdownSharedStartup(void);
unsigned int LoadSoundWithDriveFallback(char *filename, int channel, int flags);
int sound_unload(int sound_id);
void FUN_005614c3(int creature_type, int volume, int pitch_percent, int pan_percent);
void ResolveWizardTownSiege(void);
int RunWorldLairMonsterEncounter(int slot_index, int monster_color);

// GLOBAL: SHANDALAR 0x00591270
int DAT_00591270 = 1;

// FUNCTION: SHANDALAR 0x004bc6d3
void FUN_004bc6d3(int creature_type, int work_entry_index_a, int work_entry_index_b)
{
  struct
  {
    EncodedImage *sprite_header;
    int sprite_count_a;
    int ok;
    int sprite_count_b;
  } s;

#define sprite_header s.sprite_header
#define sprite_count_a s.sprite_count_a
#define ok s.ok
#define sprite_count_b s.sprite_count_b

  ok = 1;
  if (g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites[0] != (EncodedImage *)0)
  {
    return;
  }

  switch (creature_type)
  {
  case 1:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("BK_FWZ.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SFWZ.spr"));
    break;
  case 2:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("BK_KHT.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SB_KHT.spr"));
    break;
  case 3:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("BK_MWZ.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SMWZ.spr"));
    break;
  case 4:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("BK_LRD.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SB_LRD.spr"));
    break;
  case 5:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("BK_WG.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SBK_WG.spr"));
    break;
  case 6:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("BK_AMG.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SB_AMG.spr"));
    break;
  case 8:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("W_MWZ.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SW_MWZ.spr"));
    break;
  case 9:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("W_FWZ.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SFWZ.spr"));
    break;
  case 0xa:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("W_KHT.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SKHT.spr"));
    break;
  case 0xb:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("W_LRD.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SW_LRD.spr"));
    break;
  case 0xc:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("W_WG.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SW_WG.spr"));
    break;
  case 0xd:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("W_AMG.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SW_AMG.spr"));
    break;
  case 0xf:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("BU_FWZ.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SFWZ.spr"));
    break;
  case 0x10:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("BU_LRD.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SU_LRD.spr"));
    break;
  case 0x11:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("BU_MWZ.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SMWZ.spr"));
    break;
  case 0x12:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("BU_WRM.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SWRM.spr"));
    break;
  case 0x13:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("B_SFR.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SB_SFT.spr"));
    break;
  case 0x14:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("BU_AMG.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SU_AMG.spr"));
    break;
  case 0x16:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("G_MWZ.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SMWZ.spr"));
    break;
  case 0x17:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("G_KHT.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SKHT.spr"));
    break;
  case 0x18:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("G_FWZ.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SFWZ.spr"));
    break;
  case 0x19:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("G_WRM.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SWRM.spr"));
    break;
  case 0x1a:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("G_LRD.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SG_LRD.spr"));
    break;
  case 0x1b:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("G_AMG.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SG_AMG.spr"));
    break;
  case 0x1d:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("R_FWZ.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SR_FWZ.spr"));
    break;
  case 0x1e:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("R_MWZ.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SMWZ.spr"));
    break;
  case 0x1f:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("TROLL.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("STRL.spr"));
    break;
  case 0x20:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("R_LRD.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SR_LRD.spr"));
    break;
  case 0x21:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("R_WRM.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SR_WRM.spr"));
    break;
  case 0x22:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("R_AMG.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SR_AMG.spr"));
    break;
  case 0x23:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("R_AMG.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SR_AMG.spr"));
    break;
  case 0x24:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("M_TSK.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SM_TSK.spr"));
    break;
  case 0x25:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("M_TRL.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SM_TRL.spr"));
    break;
  case 0x26:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("M_APE.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SM_APE.spr"));
    break;
  case 0x27:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("M_CEN2.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SM_CEN.spr"));
    break;
  case 0x28:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("M_WG.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SM_WG.spr"));
    break;
  case 0x29:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("M_FNG.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SM_FNG.spr"));
    break;
  case 0x2a:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("M_CEN.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SM_CEN2.spr"));
    break;
  case 0x2b:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("M_LRD.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SM_LRD.spr"));
    break;
  case 0x2c:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("M_KHT.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SKHT.spr"));
    break;
  case 0x2d:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("M_FWZ.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SFWZ.spr"));
    break;
  case 0x2e:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("BK_DJN.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SDJN.spr"));
    break;
  case 0x2f:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("G_DJN.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SDJN.spr"));
    break;
  case 0x30:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("R_DJN.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SDJN.spr"));
    break;
  case 0x31:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("BU_DJN.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("SDJN.spr"));
    break;
  case 0x32:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("DG_BRU.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("S_DG.spr"));
    break;
  case 0x33:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("DG_UWB.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("S_DG.spr"));
    break;
  case 0x34:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("DG_GWR.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("S_DG.spr"));
    break;
  case 0x35:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("DG_RBG.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("S_DG.spr"));
    break;
  case 0x36:
    sprite_count_a = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites, BuildResolutionSpritePath("DG_WUG.spr"));
    sprite_count_b = ReadSpriteEntryPointers(g_opening_menu_sprite_work_buffer[work_entry_index_b].sprites, BuildResolutionSpritePath("S_DG.spr"));
    break;
  default:
    ok = 0;
  }

  if (ok)
  {
    sprite_header = g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites[0];
    g_world_lair_monster_sprite_widths[work_entry_index_a] = sprite_header->width;
    g_world_lair_monster_sprite_heights[work_entry_index_a] = sprite_header->height;
    g_world_lair_monster_sprite_top_clips[work_entry_index_a] = sprite_header->top_clip;
    if (g_world_lair_monster_sprite_heights[work_entry_index_a] < g_world_lair_monster_sprite_top_clips[work_entry_index_a])
    {
      g_world_lair_monster_sprite_top_clips[work_entry_index_a] = (g_world_lair_monster_sprite_heights[work_entry_index_a] * 2) / 3;
    }
    g_world_lair_monster_sprite_widths[work_entry_index_b] = g_world_lair_monster_sprite_widths[work_entry_index_a];
    g_world_lair_monster_sprite_heights[work_entry_index_b] = g_world_lair_monster_sprite_heights[work_entry_index_a];
    g_world_lair_monster_sprite_top_clips[work_entry_index_b] = g_world_lair_monster_sprite_top_clips[work_entry_index_a];
  }
  else
  {
    g_opening_menu_sprite_work_buffer[work_entry_index_a].sprites[0] = (EncodedImage *)0;
  }

#undef ok
#undef sprite_count_b
#undef sprite_count_a
#undef sprite_header
}

// FUNCTION: SHANDALAR 0x005614c3
void FUN_005614c3(int creature_type, int volume, int pitch_percent, int pan_percent)
{
  if (DAT_00591270 == 0)
  {
    sound_unload(0xf);
  }
  DAT_00591270 = 0;

  switch (g_shandalar_monster_definitions[creature_type].encounter_type)
  {
  case 1:
    LoadSoundWithDriveFallback("x:sound\\malewiz.wav", 0xf, 0);
    break;
  case 2:
    LoadSoundWithDriveFallback("x:sound\\fewiz.wav", 0xf, 0);
    break;
  case 3:
    LoadSoundWithDriveFallback("x:sound\\knight.wav", 0xf, 0);
    break;
  case 4:
    LoadSoundWithDriveFallback("x:sound\\lord.wav", 0xf, 0);
    break;
  case 5:
    LoadSoundWithDriveFallback("x:sound\\djinn.wav", 0xf, 0);
    break;
  case 6:
    LoadSoundWithDriveFallback("x:sound\\troll.wav", 0xf, 0);
    break;
  case 7:
    LoadSoundWithDriveFallback("x:sound\\wyrm.wav", 0xf, 0);
    break;
  case 8:
    LoadSoundWithDriveFallback("x:sound\\dragon.wav", 0xf, 0);
    break;
  case 9:
    LoadSoundWithDriveFallback("x:sound\\flyer.wav", 0xf, 0);
    break;
  case 10:
    LoadSoundWithDriveFallback("x:sound\\archmage.wav", 0xf, 0);
    break;
  }

  FUN_00562736(0xf, volume, pitch_percent, -pan_percent);
}

// FUNCTION: SHANDALAR 0x0055fd27
void UpdateWorldLairAndMonsterSlots(void)
{
  struct
  {
    int slot_u;             // ebp - 0x6c
    int terrain_mask;       // ebp - 0x68
    int target_dy;          // ebp - 0x64
    int tile_dist_y;        // ebp - 0x60
    int tile_dist_x;        // ebp - 0x5c
    int can_run_movement;   // ebp - 0x58
    int color;              // ebp - 0x54
    int nearest_distance;   // ebp - 0x50
    int tile_mask;          // ebp - 0x4c
    int tile_x;             // ebp - 0x48
    int i;                  // ebp - 0x44
    int victory_count;      // ebp - 0x40
    int slot_index;         // ebp - 0x3c
    int spawn_y;            // ebp - 0x38
    int prev_y;             // ebp - 0x34
    int range;              // ebp - 0x30
    int move_dir;           // ebp - 0x2c
    int new_distance;       // ebp - 0x28
    int spawn_x;            // ebp - 0x24
    int creature_tier;      // ebp - 0x20
    int movement_step;      // ebp - 0x1c
    int tile_y;             // ebp - 0x18
    int distance_to_player; // ebp - 0x14
    int tmp;                // ebp - 0x10
    int nearest_slot;       // ebp - 0xc
    int speed;              // ebp - 0x8
    int prev_x;             // ebp - 0x4
  } s;

  s.nearest_distance = 0x7fff;
#ifdef MODERN_FIXES
  s.nearest_slot = -1;
#endif

  for (s.slot_index = 0; s.slot_index < 6; s.slot_index = s.slot_index + 1)
  {
    s.distance_to_player = ApproximateDistance(g_world_player_x - g_lair_or_monster_slots[s.slot_index].world_x,
                                               g_world_player_y - g_lair_or_monster_slots[s.slot_index].world_y);
    if ((s.distance_to_player < s.nearest_distance) && (g_lair_or_monster_slots[s.slot_index].entry_type != SHANDALAR_ENTRY_LAIR))
    {
      s.nearest_distance = s.distance_to_player;
      s.nearest_slot = s.slot_index;
    }
  }

  if (g_lair_or_monster_slots[7].entry_type == SHANDALAR_ENTRY_NONE)
  {
    g_siege_indicator = 0;
  }

  for (s.slot_index = 0; s.slot_index < 8; s.slot_index = s.slot_index + 1)
  {
    s.distance_to_player = ApproximateDistance(g_world_player_x - g_lair_or_monster_slots[s.slot_index].world_x,
                                               g_world_player_y - g_lair_or_monster_slots[s.slot_index].world_y);

    s.tile_dist_x = abs(g_world_player_tile_x - g_lair_or_monster_slots[s.slot_index].world_x / 0x20);
    s.tile_dist_y = abs(g_world_player_tile_y - g_lair_or_monster_slots[s.slot_index].world_y / 0x20);

    if ((s.slot_index < 6) &&
        ((g_lair_or_monster_slots[s.slot_index].entry_type == SHANDALAR_ENTRY_NONE) || (s.tile_dist_x > 4) || (s.tile_dist_y > 4)))
    {
      do
      {
        if (RandomIntLessThan(2) != 0)
        {
          s.spawn_x = (RandomIntLessThan(2) ? 4 : -4) + g_world_player_tile_x;
          s.spawn_y = g_world_player_tile_y + RandomIntLessThan(9) - 4;
        }
        else
        {
          s.spawn_y = (RandomIntLessThan(2) ? 4 : -4) + g_world_player_tile_y;
          s.spawn_x = g_world_player_tile_x + RandomIntLessThan(9) - 4;
        }

        s.tile_y = GetWorldTileType(s.spawn_x, s.spawn_y);
        s.color = GetWorldTileMagicMask(s.tile_y);
      } while (s.color == 0);

      do
      {
        s.victory_count = RandomIntLessThan(6);
      } while ((s.color & (1U << (byte)s.victory_count)) == 0);
      s.color = s.victory_count;

      s.victory_count = g_lairs_explored / 8;
      for (s.i = 0; s.i < 1000; s.i = s.i + 1)
      {
        if (((int)(char)g_duel_victory_log[s.i] >> 4) == s.color)
        {
          s.victory_count = s.victory_count + 1;
        }
      }

      s.move_dir = ClampIntToRange(0x80 / (s.victory_count + 4), 6, 0x14);

      switch (RandomIntLessThan(s.move_dir) + 5 / (s.victory_count + 1))
      {
      case 0:
        s.creature_tier = 10;
        break;
      case 1:
        s.creature_tier = 10;
        break;
      case 2:
        s.creature_tier = 10;
        break;
      case 3:
        s.creature_tier = 0;
        break;
      case 4:
        s.creature_tier = 10;
        break;
      case 5:
        s.creature_tier = 8;
        break;
      case 6:
        s.creature_tier = 6;
        break;
      case 7:
        s.creature_tier = 4;
        break;
      case 8:
        s.creature_tier = 6;
        break;
      case 9:
        s.creature_tier = 0;
        break;
      case 0xa:
        s.creature_tier = 4;
        break;
      case 0xb:
        s.creature_tier = 6;
        break;
      case 0xc:
        s.creature_tier = 0;
        break;
      case 0xd:
        s.creature_tier = 4;
        break;
      case 0xe:
        s.creature_tier = 0;
        break;
      case 0xf:
        s.creature_tier = 4;
        break;
      default:
        s.creature_tier = 0;
        break;
      }

      if (s.creature_tier == 10)
      {
        s.creature_tier = RandomIntLessThan(200);
        if (s.creature_tier < 0x32)
        {
          s.creature_tier = 10;
        }
        else if (s.creature_tier < 0x55)
        {
          s.creature_tier = 0xb;
        }
        else if (s.creature_tier < 0x73)
        {
          s.creature_tier = 0xc;
        }
        else if (s.creature_tier < 0x8e)
        {
          s.creature_tier = 0xd;
        }
        else if (s.creature_tier < 0xa5)
        {
          s.creature_tier = 0xe;
        }
        else if (s.creature_tier < 0xb9)
        {
          s.creature_tier = 0x10;
        }
        else if (s.creature_tier < 200)
        {
          s.creature_tier = 0x12;
        }
      }

      if ((s.slot_index == 0) && (g_current_quest_type < 0) && (-100 < g_current_quest_type))
      {
        s.creature_tier = (int)g_shandalar_monster_definitions[-g_current_quest_type].tier;
      }

      if (s.creature_tier != 0)
      {
        if (((g_shandalar_difficulty == 0) && ((s.slot_index == 5) || (s.slot_index == 6))) ||
            ((g_shandalar_difficulty == 1) && (s.slot_index == 6)))
        {
          continue;
        }

#define WORLD_MONSTER_RESPAWN_TIMER_SCALE (4320 * 86400)
#define WORLD_MONSTER_RESPAWN_TIMER_BIAS 2309

        if (((4 - g_shandalar_difficulty) * WORLD_MONSTER_RESPAWN_TIMER_SCALE) /
                ((g_monster_timer / 0xf) * (g_monster_timer / 0xf) +
                 WORLD_MONSTER_RESPAWN_TIMER_BIAS * WORLD_MONSTER_RESPAWN_TIMER_BIAS) >
            g_monster_timer - g_lair_or_monster_slots[s.slot_index].respawn_timestamp)
        {
          continue;
        }
#undef WORLD_MONSTER_RESPAWN_TIMER_SCALE
#undef WORLD_MONSTER_RESPAWN_TIMER_BIAS
      }

      {
        if (s.creature_tier != 0)
        {
          s.creature_tier = PickRandomCreatureTypeForWizardTier(s.color, s.creature_tier);
          g_lair_or_monster_slots[s.slot_index].entry_type = (ShandalarEntryType)s.creature_tier;

          s.new_distance = 0;
          for (s.i = 0; (s.i < 1000) && ((char)g_duel_victory_log[s.i] != '\0'); s.i = s.i + 1)
          {
            if ((g_shandalar_monster_definitions[s.creature_tier].encounter_type == ((char)g_duel_victory_log[s.i] & 0xf)) &&
                (((int)(char)g_duel_victory_log[s.i] >> 4) == s.color))
            {
              s.new_distance = s.new_distance + 1;
            }
          }

          if (s.new_distance > 8)
          {
            g_lair_or_monster_slots[s.slot_index].entry_type = s.creature_tier = SHANDALAR_ENTRY_NONE;
          }
        }
        else
        {
          g_lair_or_monster_slots[s.slot_index].entry_type = SHANDALAR_ENTRY_LAIR;
        }

        g_lair_or_monster_slots[s.slot_index].world_x = s.spawn_x * 0x20 + 0x10;
        g_lair_or_monster_slots[s.slot_index].world_y = s.spawn_y * 0x20 + 0x10;
        g_lair_or_monster_slots[s.slot_index].color = s.color;
        g_lair_or_monster_slots[s.slot_index].movement_heading = 0;
        g_lair_or_monster_slots[s.slot_index].movement_anim_frame = 0;

        if (((1U << (byte)s.color) & (unsigned int)g_shandalar_monster_definitions[s.creature_tier].color_mask) &&
            ((g_defeated_wizards_bitmap & (1U << (byte)s.color)) != 0))
        {
          s.target_dy = (unsigned int)g_shandalar_monster_definitions[s.creature_tier].color_mask & ~(1U << (byte)s.color);
          if ((s.target_dy == 0) || ((g_defeated_wizards_bitmap & s.target_dy) != 0))
          {
            FreeOpeningMenuSpriteWorkEntries(s.slot_index, s.slot_index + 8);
            g_lair_or_monster_slots[s.slot_index].entry_type = SHANDALAR_ENTRY_NONE;
          }
          else if (s.target_dy != 0)
          {
            g_lair_or_monster_slots[s.slot_index].color = single_color_test_bit_to_color_t((int)s.target_dy);
          }
        }

        if ((FUN_004314ca(s.spawn_x, s.spawn_y) & 0x10) != 0)
        {
          FreeOpeningMenuSpriteWorkEntries(s.slot_index, s.slot_index + 8);
          g_lair_or_monster_slots[s.slot_index].entry_type = SHANDALAR_ENTRY_NONE;
        }

        if ((s.slot_index == 0) && (g_current_quest_type < 0) &&
            (g_lair_or_monster_slots[s.slot_index].entry_type != -g_current_quest_type))
        {
          FreeOpeningMenuSpriteWorkEntries(s.slot_index, s.slot_index + 8);
          g_lair_or_monster_slots[s.slot_index].entry_type = SHANDALAR_ENTRY_NONE;
        }

        if (g_lair_or_monster_slots[s.slot_index].entry_type > SHANDALAR_ENTRY_LAIR)
        {
          FreeOpeningMenuSpriteWorkEntries(s.slot_index, s.slot_index + 8);
        }
        else if (g_lair_or_monster_slots[s.slot_index].entry_type != SHANDALAR_ENTRY_NONE)
        {
          for (s.terrain_mask = 0; s.terrain_mask < 8; s.terrain_mask = s.terrain_mask + 1)
          {
            if ((g_lair_or_monster_slots[s.terrain_mask].entry_type == SHANDALAR_ENTRY_NONE) ||
                (s.slot_index == s.terrain_mask))
            {
              continue;
            }
            if ((g_lair_or_monster_slots[s.slot_index].world_x == g_lair_or_monster_slots[s.terrain_mask].world_x) &&
                (g_lair_or_monster_slots[s.slot_index].world_y == g_lair_or_monster_slots[s.terrain_mask].world_y))
            {
              g_lair_or_monster_slots[s.slot_index].entry_type = SHANDALAR_ENTRY_NONE;
            }
          }
        }
      }
    }

    s.creature_tier = g_lair_or_monster_slots[s.slot_index].entry_type;
    if (s.creature_tier == SHANDALAR_ENTRY_NONE)
    {
      continue;
    }

    if (s.creature_tier > SHANDALAR_ENTRY_LAIR)
    {
      FUN_004bc6d3(s.creature_tier, s.slot_index, s.slot_index + 8);
    }

    s.movement_step = 1;
#ifdef MODERN_FIXES
    s.speed = 0;
    s.range = 0;
#endif
    switch (g_shandalar_monster_definitions[s.creature_tier].encounter_type)
    {
    case 0:
      s.range = -1;
      s.movement_step = 0;
      break;
    case 2:
      s.speed = 1;
      s.range = 0x20;
      s.movement_step = 1;
      break;
    case 3:
      s.speed = 1;
      s.range = 0x30;
      s.movement_step = 1;
      break;
    case 1:
      s.speed = 1;
      s.range = 0x20;
      s.movement_step = 1;
      break;
    case 4:
      s.speed = 1;
      s.range = 0x30;
      s.movement_step = 1;
      break;
    case 5:
      s.speed = 1;
      s.range = 0x40;
      s.movement_step = 1;
      break;
    case 6:
      s.speed = 1;
      s.range = 0x60;
      s.movement_step = 1;
      break;
    case 7:
      s.speed = 1;
      s.range = 0x60;
      s.movement_step = 1;
      break;
    case 8:
      s.speed = 1;
      s.range = 0x40;
      s.movement_step = 1;
      break;
    case 9:
      s.speed = 1;
      s.range = 0x20;
      s.movement_step = 2;
      break;
    case 0xa:
      s.speed = 1;
      s.range = 0x30;
      s.movement_step = 1;
      break;
    }

    s.range = (s.range * 3) / 2;
    if (g_shandalar_monster_definitions[s.creature_tier].flags_0a & 0x200)
    {
      s.range = s.range << 1;
    }

    if (s.slot_index != s.nearest_slot)
    {
      s.range = s.range / 2;
    }
    s.tmp = s.distance_to_player;
    while (abs(s.range) < s.tmp)
    {
      s.speed = s.speed << 1;
      s.tmp = s.tmp / 2;
    }

    s.prev_x = g_lair_or_monster_slots[s.slot_index].world_x;
    s.prev_y = g_lair_or_monster_slots[s.slot_index].world_y;

    if (g_world_player_animation_frame != 0)
    {
      s.tile_dist_x = (ClampIntToRange(s.distance_to_player / 3, 0, g_shandalar_difficulty << 4) *
                           g_neighbor_dx[g_world_player_animation_direction] +
                       g_world_player_x) -
                      s.prev_x;
      s.tile_dist_y = (ClampIntToRange(s.distance_to_player / 3, 0, g_shandalar_difficulty << 4) *
                           g_neighbor_dy[g_world_player_animation_direction] +
                       g_world_player_y) -
                      s.prev_y;
    }
    else
    {
      s.tile_dist_x = g_world_player_x - s.prev_x;
      s.tile_dist_y = g_world_player_y - s.prev_y;
    }

    if (g_shandalar_monster_definitions[s.creature_tier].flags_0a & 1)
    {
      s.movement_step = 2;
    }

    if ((s.distance_to_player < 0x40) && (s.slot_index < 7))
    {
      s.new_distance = 0;
      for (s.i = 0; (s.i < 1000) && ((char)g_duel_victory_log[s.i] != '\0'); s.i = s.i + 1)
      {
        if ((g_shandalar_monster_definitions[s.creature_tier].encounter_type == ((char)g_duel_victory_log[s.i] & 0xf)) &&
            (((int)(char)g_duel_victory_log[s.i] >> 4) == g_lair_or_monster_slots[s.slot_index].color))
        {
          s.new_distance = s.new_distance + 1;
        }
      }

      if (g_shandalar_difficulty + 3 <= s.new_distance)
      {
        s.tile_dist_x = -s.tile_dist_x;
        s.tile_dist_y = -s.tile_dist_y;
      }
    }

    if ((s.slot_index == 7) && (s.distance_to_player > 0x18))
    {
      s.tile_mask = FindNearestTownIndex(g_lair_or_monster_slots[s.slot_index].world_x / 0x20,
                                         g_lair_or_monster_slots[s.slot_index].world_y / 0x20);
      s.tile_dist_x = g_town_slots[s.tile_mask].world_x * 0x20 + 0x10 - s.prev_x;
      s.tile_dist_y = g_town_slots[s.tile_mask].world_y * 0x20 + 0x10 - s.prev_y;
      s.tile_dist_x += (g_monster_timer & 0x1f) - 0x10;
      s.tile_dist_y += ((g_monster_timer & 0x3e) >> 1) - 0x10;
    }

    s.tile_y = GetWorldTileType(
        g_lair_or_monster_slots[s.slot_index].world_x / 0x20,
        g_lair_or_monster_slots[s.slot_index].world_y / 0x20);
    s.color = GetWorldTileMagicMask(s.tile_y);

    if ((g_shandalar_monster_definitions[s.creature_tier].flags_0a & 0xf8) &&
        ((g_shandalar_monster_definitions[s.creature_tier].flags_0a & (s.color << 3)) != 0) &&
        (s.speed > 1))
    {
      s.speed = s.speed / 2;
    }

    s.can_run_movement = 0;
    s.tile_dist_x = s.tile_dist_x + g_neighbor_dx[g_lair_or_monster_slots[s.slot_index].movement_heading] * 8;
    s.tile_dist_y = s.tile_dist_y + g_neighbor_dy[g_lair_or_monster_slots[s.slot_index].movement_heading] * 8;

    if (abs(s.tile_dist_y) * 2 < abs(s.tile_dist_x))
    {
      if (s.tile_dist_x > 0)
      {
        s.can_run_movement = 3;
      }
      else
      {
        s.can_run_movement = 7;
      }
    }

    if (abs(s.tile_dist_x) * 2 < abs(s.tile_dist_y))
    {
      if (s.tile_dist_y > 0)
      {
        s.can_run_movement = 5;
      }
      else
      {
        s.can_run_movement = 1;
      }
    }

    if (s.can_run_movement == 0)
    {
      if (s.tile_dist_x > 0)
      {
        if (s.tile_dist_y > 0)
        {
          s.can_run_movement = 4;
        }
        else
        {
          s.can_run_movement = 2;
        }
      }
      else
      {
        if (s.tile_dist_y > 0)
        {
          s.can_run_movement = 6;
        }
        else
        {
          s.can_run_movement = 8;
        }
      }
    }

    if (((g_monster_timer + s.slot_index) & 3) == 0)
    {
      g_lair_or_monster_slots[s.slot_index].movement_heading = (signed char)s.can_run_movement;
    }
    else
    {
      s.can_run_movement = g_lair_or_monster_slots[s.slot_index].movement_heading;
    }

    g_lair_or_monster_slots[s.slot_index].world_x = g_lair_or_monster_slots[s.slot_index].world_x + g_neighbor_dx[s.can_run_movement] * 4;
    g_lair_or_monster_slots[s.slot_index].world_y = g_lair_or_monster_slots[s.slot_index].world_y + g_neighbor_dy[s.can_run_movement] * 4;

    s.tile_x = 1;
    for (s.i = 0; s.i < 6; s.i = s.i + 1)
    {
      if ((g_lair_or_monster_slots[s.i].entry_type == SHANDALAR_ENTRY_LAIR) ||
          (s.i == s.slot_index) ||
          (s.distance_to_player < 0x20))
      {
        continue;
      }

      s.victory_count = ApproximateDistance(g_lair_or_monster_slots[s.slot_index].world_x - g_lair_or_monster_slots[s.i].world_x,
                                            g_lair_or_monster_slots[s.slot_index].world_y - g_lair_or_monster_slots[s.i].world_y);
      if (s.victory_count < 0x20)
      {
        if (s.victory_count < ApproximateDistance(s.prev_x - g_lair_or_monster_slots[s.i].world_x,
                                                  s.prev_y - g_lair_or_monster_slots[s.i].world_y))
        {
          s.tile_x = 0;
        }
      }
    }

    if (s.tile_x == 0)
    {
      g_lair_or_monster_slots[s.slot_index].world_x = s.prev_x;
      g_lair_or_monster_slots[s.slot_index].world_y = s.prev_y;
      g_lair_or_monster_slots[s.slot_index].movement_anim_frame = 0;
      continue;
    }

    if ((g_shandalar_monster_definitions[s.creature_tier].flags_0a & 4) && ((g_monster_timer & 0x3fU) == 0))
    {
      if (RandomIntLessThan(2) != 0)
      {
        if (RandomIntLessThan(2) != 0)
        {
          g_lair_or_monster_slots[s.slot_index].world_x = g_lair_or_monster_slots[s.slot_index].world_x + 0x20;
        }
        else
        {
          g_lair_or_monster_slots[s.slot_index].world_x = g_lair_or_monster_slots[s.slot_index].world_x - 0x20;
        }
      }
      else
      {
        if (RandomIntLessThan(2) != 0)
        {
          g_lair_or_monster_slots[s.slot_index].world_y = g_lair_or_monster_slots[s.slot_index].world_y + 0x20;
        }
        else
        {
          g_lair_or_monster_slots[s.slot_index].world_y = g_lair_or_monster_slots[s.slot_index].world_y - 0x20;
        }
      }
    }

    s.tile_y = GetWorldTileType(
        g_lair_or_monster_slots[s.slot_index].world_x / 0x20,
        g_lair_or_monster_slots[s.slot_index].world_y / 0x20);
    s.color = GetWorldTileMagicMask(s.tile_y);

    if (((s.color & (unsigned int)g_shandalar_monster_definitions[s.creature_tier].color_mask) == 0) &&
        (s.range >= 0) && (s.slot_index != 7))
    {
      g_lair_or_monster_slots[s.slot_index].world_x = s.prev_x;
      g_lair_or_monster_slots[s.slot_index].world_y = s.prev_y;
      s.tile_y = GetWorldTileType(s.prev_x / 0x20, s.prev_y / 0x20);
      s.color = GetWorldTileMagicMask(s.tile_y);
      g_lair_or_monster_slots[s.slot_index].movement_anim_frame = 0;

      if ((s.color & (unsigned int)g_shandalar_monster_definitions[s.creature_tier].color_mask) == 0)
      {
        FreeOpeningMenuSpriteWorkEntries(s.slot_index, s.slot_index + 8);
        g_lair_or_monster_slots[s.slot_index].entry_type = SHANDALAR_ENTRY_NONE;
      }
    }
    else
    {
      g_lair_or_monster_slots[s.slot_index].world_x = g_neighbor_dx[s.can_run_movement] * s.movement_step + s.prev_x;
      g_lair_or_monster_slots[s.slot_index].world_y = g_neighbor_dy[s.can_run_movement] * s.movement_step + s.prev_y;
      g_lair_or_monster_slots[s.slot_index].movement_heading = (signed char)s.can_run_movement;
      g_lair_or_monster_slots[s.slot_index].movement_anim_frame++;
      if (g_lair_or_monster_slots[s.slot_index].movement_anim_frame > 4)
      {
        g_lair_or_monster_slots[s.slot_index].movement_anim_frame = 1;
      }
    }

    if (ApproximateDistance(g_world_player_x - g_lair_or_monster_slots[s.slot_index].world_x,
                            g_world_player_y - g_lair_or_monster_slots[s.slot_index].world_y) <
        ((g_lair_or_monster_slots[s.slot_index].entry_type == SHANDALAR_ENTRY_LAIR) ? 0x1a : 0x10))
    {
      SaveGameToSlot(3);
      s.slot_u = RunWorldLairMonsterEncounter(s.slot_index, g_lair_or_monster_slots[s.slot_index].color);
      g_lair_or_monster_slots[s.slot_index].respawn_timestamp = g_monster_timer;
      RefreshAdventureInterfaceLayout();
      if ((s.slot_index == 7) && (s.slot_u <= 0))
      {
        ResolveWizardTownSiege();
      }
      else if (s.slot_index == 7)
      {
        AddJournalEntry(JOURNAL_ENTRY_LAIR_EXPLORED, g_lair_or_monster_slots[s.slot_index].color);
        g_siege_indicator = 0;
      }
      FreeOpeningMenuSpriteWorkEntries(s.slot_index, s.slot_index + 8);
      g_lair_or_monster_slots[s.slot_index].entry_type = SHANDALAR_ENTRY_NONE;
      s.can_run_movement = 0;
      EnsureAdvfac64Loaded(0);
      g_monster_timer = g_monster_timer | 0x1f;
      SaveGameToSlot(3);
    }
    else if ((s.creature_tier != 0) &&
             (g_lair_or_monster_slots[s.slot_index].movement_anim_frame != 0) &&
             (s.distance_to_player < 0x50) && (RandomIntLessThan(s.distance_to_player) < 4))
    {
      FUN_005614c3(s.creature_tier, 0x68 - s.distance_to_player / 2, 100 - s.distance_to_player / 3,
                   g_neighbor_dx[(g_lair_or_monster_slots[s.slot_index].movement_heading + 2U) & 7] *
                       100);
    }
  }
}

// FUNCTION: SHANDALAR 0x005616f9
void StartWizardTownSiege(void)
{
  struct
  {
    int unused_stack_slot;
    int ruled_by_color_count[7];
    int town_index;
    int color;
    int best_score;
    int nearest_distance;
    int best_color;
    int lair_world_y_by_color[7];
    int lair_world_x_by_color[7];
    int scan_index;
    int creature_tier;
    int dist;
    int best_town_index;
    int score;
  } s;

  if (g_siege_indicator != 0)
  {
    return;
  }

  ClearInputAndWaitForMouseRelease();

  for (s.scan_index = 0; s.scan_index < 7; s.scan_index++)
  {
    s.lair_world_x_by_color[s.scan_index] = -1;
    s.ruled_by_color_count[s.scan_index] = 0;
  }

  for (s.scan_index = 0; s.scan_index < 0x80; s.scan_index++)
  {
    if (g_town_slots[s.scan_index].location_type == 4)
    {
      s.color = single_color_test_bit_to_color_t((int)GetWorldTileMagicMask(GetWorldTileType(g_town_slots[s.scan_index].world_x, g_town_slots[s.scan_index].world_y)));
      s.lair_world_x_by_color[s.color] = g_town_slots[s.scan_index].world_x;
      s.lair_world_y_by_color[s.color] = g_town_slots[s.scan_index].world_y;
    }

    if (g_town_slots[s.scan_index].status_and_ruling_wizard & 0xff00)
    {
      s.ruled_by_color_count[g_town_slots[s.scan_index].status_and_ruling_wizard >> 8] += 1;
    }
  }

  s.unused_stack_slot = 0;
  s.best_score = 0x7fff;
  s.color = -1;

  for (s.town_index = 0; s.town_index < 0x80; s.town_index = s.town_index + 1)
  {
    if ((((g_town_slots[s.town_index].status_and_ruling_wizard & 0xff00) ||
          (g_town_slots[s.town_index].location_type == 4)) ||
         (g_town_slots[s.town_index].location_type == 1)) ||
        (g_town_slots[s.town_index].location_type == 5))
    {
      continue;
    }

    s.nearest_distance = 0x7fff;
    for (s.scan_index = 1; s.scan_index < 6; s.scan_index = s.scan_index + 1)
    {
      if (s.lair_world_x_by_color[s.scan_index] == -1)
      {
        continue;
      }

      s.dist = ApproximateDistance(g_town_slots[s.town_index].world_x - s.lair_world_x_by_color[s.scan_index],
                                   g_town_slots[s.town_index].world_y - s.lair_world_y_by_color[s.scan_index]);
      if (s.dist < s.nearest_distance)
      {
        s.nearest_distance = s.dist;
        s.color = s.scan_index;
      }
    }

    s.score = s.ruled_by_color_count[s.color] * 0x20 + RandomIntLessThan(0x80);
    if (s.score < s.best_score)
    {
      s.best_score = s.score;
      s.best_town_index = s.town_index;
      s.best_color = s.color;
    }
  }

  s.color = s.best_color;
  s.town_index = s.best_town_index;

  if (s.color == -1)
  {
  }
  else
  {
    s.scan_index = 7;

    switch (g_siege_timer / 0x80)
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
    default:
      if ((g_town_slots[s.town_index].world_y & 1) != 0)
      {
        s.creature_tier = 0xc;
      }
      else
      {
        s.creature_tier = 0x10;
      }
      break;
    }

    FreeOpeningMenuSpriteWorkEntries(s.scan_index, s.scan_index + 8);
    g_lair_or_monster_slots[s.scan_index].entry_type = PickRandomCreatureTypeForWizardTier(s.color, s.creature_tier);
    g_lair_or_monster_slots[s.scan_index].world_x = g_town_slots[s.town_index].world_x * 0x20 + 0x10;
    g_lair_or_monster_slots[s.scan_index].world_y = g_town_slots[s.town_index].world_y * 0x20 + 0x10;
    g_lair_or_monster_slots[s.scan_index].color = s.color;

    g_wizard_siege_count = g_wizard_siege_count + 1;
    SetWorldMapPixelFlags(0x80, g_town_slots[s.town_index].world_x, g_town_slots[s.town_index].world_y);

    PlaySoundEffectOnChannel("x:sound\\newsflash.wav", 0x97, 100, 100, 0);
    AnimateVisitBackdropZoomIn("newsback.pic");

    strcpy(g_ui_message_buffer, gs_newsflash_0077d140[0]);
    FormatMessageFromStringStripCarriageReturns(g_ui_message_buffer + strlen(g_ui_message_buffer), 0x1000, gs_newsflash_0077d140[1],
                                                gs_wizardnames_0077ee70[s.color],
                                                BuildCreatureNameWithArticle(g_lair_or_monster_slots[s.scan_index].entry_type),
                                                BuildTownDisplayName(s.town_index));

    PTR_DAT_005832b4->font_slot = 5;
    DrawTextAt(PTR_DAT_005832b4, 0xbe, 0x140, 0xf7, g_ui_message_buffer);
    PTR_DAT_005832b4->font_slot = 1;
    ClearInputAndWaitForMouseRelease();
    WaitForInputEventUnlessBlocked();
    RefreshAdventureInterfaceLayout();
    g_siege_indicator = 1;
  }
}

// FUNCTION: SHANDALAR 0x00562169
void ResolveWizardTownSiege(void)
{
  struct
  {
    int font_size;
    int ruled_count;
    int slot_index;
    int town_index;
    int wizard_color;
    int required_count;
    int i;
  } s;

  s.slot_index = 7;
  g_siege_indicator = 0;

  if (g_lair_or_monster_slots[s.slot_index].entry_type == SHANDALAR_ENTRY_NONE)
  {
    return;
  }
  ClearInputAndWaitForMouseRelease();
  s.wizard_color = g_lair_or_monster_slots[s.slot_index].color;
  s.town_index = FindNearestTownIndex(g_lair_or_monster_slots[s.slot_index].world_x / 0x20,
                                      g_lair_or_monster_slots[s.slot_index].world_y / 0x20);

  g_town_slots[s.town_index].status_and_ruling_wizard &= -65281;
  g_town_slots[s.town_index].status_and_ruling_wizard |= s.wizard_color << 8;

  s.ruled_count = 0;
  for (s.i = 0; s.i < 0x80; s.i = s.i + 1)
  {
    if ((int)g_town_slots[s.i].status_and_ruling_wizard >> 8 == s.wizard_color)
    {
      s.ruled_count = s.ruled_count + 1;
    }
  }

  if (g_world_magic_slot_timers[0xb].town_index == 0)
  {
    s.required_count = 5;
  }
  else
  {
    s.required_count = 3;
  }

  PlaySoundEffectOnChannel("x:sound\\newsflash.wav", 0x97, 100, 100, 0);
  AnimateVisitBackdropZoomIn("newsback.pic");

  strcpy(g_ui_message_buffer, gs_newsflash_0077d140[0]);
  if (IsWizardColorFeminine(s.wizard_color) != 0)
  {
    FormatMessageFromStringStripCarriageReturns(g_ui_message_buffer + strlen(g_ui_message_buffer), 0x1000, gs_newsflash_0077d140[4],
                                                gs_wizardnames_0077ee70[s.wizard_color], BuildTownDisplayName(s.town_index));
  }
  else
  {
    FormatMessageFromStringStripCarriageReturns(g_ui_message_buffer + strlen(g_ui_message_buffer), 0x1000, gs_newsflash_0077d140[5],
                                                gs_wizardnames_0077ee70[s.wizard_color], BuildTownDisplayName(s.town_index));
  }

  if (s.required_count - s.ruled_count != 0)
  {
    if (IsWizardColorFeminine(s.wizard_color) != 0)
    {
      sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), gs_newsflash_0077d140[6], s.required_count - s.ruled_count);
    }
    else
    {
      sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), gs_newsflash_0077d140[7], s.required_count - s.ruled_count);
    }
  }
  else
  {
    if (IsWizardColorFeminine(s.wizard_color) != 0)
    {
      strcat(g_ui_message_buffer, gs_newsflash_0077d140[8]);
    }
    else
    {
      strcat(g_ui_message_buffer, gs_newsflash_0077d140[9]);
    }
  }

  if ((g_town_slots[s.town_index].status_and_ruling_wizard & 1) != 0)
  {
    g_town_slots[s.town_index].status_and_ruling_wizard &= -2;
    strcat(g_ui_message_buffer, gs_newsflash_0077d140[10]);
  }

  g_wizard_siege_count = g_wizard_siege_count + 1;
  PTR_DAT_005832b4->font_slot = 5;
  DrawTextAt(PTR_DAT_005832b4, 0xbe, 0x140, 0xf7, g_ui_message_buffer);
  PTR_DAT_005832b4->font_slot = 1;
  ClearInputAndWaitForMouseRelease();
  WaitForInputEventUnlessBlocked();

  FreeOpeningMenuSpriteWorkEntries(s.slot_index, s.slot_index + 8);
  g_lair_or_monster_slots[s.slot_index].entry_type = SHANDALAR_ENTRY_NONE;
  ShowStatsWindow(3, s.wizard_color);
  RefreshAdventureInterfaceLayout();
  g_siege_indicator = 0;

  if (s.ruled_count >= s.required_count)
  {
    AnimatePaletteToColor(0, g_default_palette_fade_steps);
    LoadPcxIntoPageOpaque(1, "uth-arz.pic");
    if ((global_screen_width == 0x280) || (global_screen_width == 800))
    {
      s.font_size = 0x10;
    }
    else
    {
      s.font_size = 0xc;
    }
    PTR_DAT_005832dc->font_slot = 5;
    SetFontStyleSize(5, (unsigned int)ScaleUiCoordinate(s.font_size));

    strcpy(g_ui_message_buffer, gs_questfailed_0077c580[1]);
    if (IsWizardColorFeminine(s.wizard_color) != 0)
    {
      sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), gs_questfailed_0077c580[2], gs_wizardnames_0077ee70[s.wizard_color]);
    }
    else
    {
      sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), gs_questfailed_0077c580[3], gs_wizardnames_0077ee70[s.wizard_color]);
    }
    strcat(g_ui_message_buffer, gs_questfailed_0077c580[5]);

    DrawFormattedTextShadowedCentered(PTR_DAT_005832dc, 0xea, 0x140, 0x1e0 - GetFontLineHeight(PTR_DAT_005832dc->font_slot) * 7, g_ui_message_buffer);
    StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, 0, 0x280, 0x1e0, PTR_DAT_005832b4, 0, 0, global_screen_width, global_screen_height);
    ClearInputAndWaitForMouseRelease();
    WaitForInputEventUnlessBlocked();
    AnimatePaletteToColor(0, g_default_palette_fade_steps);
    g_adventure_world_exit_requested = 1;
    UnloadStatWinDllExports();
    ShutdownSharedStartup();
    exit(0);
  }
}
