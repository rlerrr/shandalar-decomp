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
extern int DAT_0073ea70[8];
extern int DAT_00589dec;
extern int DAT_009300f0;

extern int g_world_player_tile_x;
extern int g_world_player_tile_y;
extern OpeningMenuSpriteWorkEntry g_opening_menu_sprite_work_buffer[0x14];
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
int PickRandomCreatureTypeForWizardTier(int wizard_color, int creature_tier);
int FindNearestTownIndex(int world_x, int world_y);
int ClampIntToRange(int value, int min_value, int max_value);
int FUN_004ecf30(int x, int y);
int FUN_00522508(int param_1);
int FUN_0040dffd(int mask);
unsigned int FUN_0043146b(int x, int y);
unsigned int FUN_004314ca(int x, int y);
void FUN_00431526(unsigned int mask, int x, int y);
unsigned int FUN_005611c8(unsigned int tile_mask);
void FUN_004290e2(int param_1, int param_2);
void SaveGameToSlot(int save_slot_index);
void EnsureAdvfac64Loaded(int state);
void RefreshAdventureInterfaceLayout(void);
void FUN_00562736(int param_1, int param_2, int param_3, int param_4);
void PlaySoundEffectOnChannel(char *sound_path, int channel, int volume, int pitch_percent, int pan_percent);
void AnimateVisitBackdropZoomIn(char *pcx_path);
void DrawTextAt(FacemakerWindowBounds *window, int color, int x, int y, char *text);
char *BuildTownDisplayName(int town_index);
char *BuildCreatureNameWithArticle(int creature_type);
DWORD FUN_00564e70(char *dst, DWORD max_length, LPCVOID format, ...);
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
int FUN_00469099(void);
unsigned int LoadSoundWithDriveFallback(char *filename, int channel, int flags);
int sound_unload(int sound_id);
void FUN_005614c3(int creature_type, int volume, int pitch_percent, int pan_percent);
void FUN_00562169(void);
int FUN_004f2ec0(int slot_index, int monster_color);

// GLOBAL: SHANDALAR 0x00591270
int DAT_00591270;

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

  switch (gs_creature_names_00591a08[creature_type].metadata[3])
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
    int nearest_distance;
    int nearest_slot;
    int slot_index;
    int distance_to_player;
    int tile_dist_x;
    int tile_dist_y;
    int tile_x;
    int tile_y;
    int tile_mask;
    int color;
    int creature_tier;
    int victory_count;
    int spawn_x;
    int spawn_y;
    int can_run_movement;
    int speed;
    int range;
    int move_dir;
    int target_dx;
    int target_dy;
    int prev_x;
    int prev_y;
    int new_distance;
    int tmp;
    int i;
    unsigned int slot_u;
    unsigned int terrain_mask;
    unsigned char selected_color;
  } s;

  s.nearest_distance = 0x7fff;
  s.nearest_slot = 0;

  for (s.slot_index = 0; s.slot_index < 6; s.slot_index = s.slot_index + 1)
  {
    s.distance_to_player = FUN_004ecf30(g_world_player_x - g_lair_or_monster_slots[s.slot_index].world_x,
                                        g_world_player_y - g_lair_or_monster_slots[s.slot_index].world_y);
    if ((s.distance_to_player < s.nearest_distance) && (g_lair_or_monster_slots[s.slot_index].entry_type != 0))
    {
      s.nearest_slot = s.slot_index;
      s.nearest_distance = s.distance_to_player;
    }
  }

  if (g_lair_or_monster_slots[7].entry_type == -1)
  {
    g_siege_indicator = 0;
  }

  for (s.slot_index = 0; s.slot_index < 8; s.slot_index = s.slot_index + 1)
  {
    s.distance_to_player = FUN_004ecf30(g_world_player_x - g_lair_or_monster_slots[s.slot_index].world_x,
                                        g_world_player_y - g_lair_or_monster_slots[s.slot_index].world_y);

    s.tile_dist_x = abs(g_world_player_tile_x - (int)((g_lair_or_monster_slots[s.slot_index].world_x +
                                                       ((g_lair_or_monster_slots[s.slot_index].world_x >> 0x1f) & 0x1fU)) >>
                                                      5));
    s.tile_dist_y = abs(g_world_player_tile_y - (int)((g_lair_or_monster_slots[s.slot_index].world_y +
                                                       ((g_lair_or_monster_slots[s.slot_index].world_y >> 0x1f) & 0x1fU)) >>
                                                      5));

    s.can_run_movement = 0;

    if ((s.slot_index < 6) &&
        ((g_lair_or_monster_slots[s.slot_index].entry_type == -1) || (s.tile_dist_x > 4) || (s.tile_dist_y > 4)))
    {
      do
      {
        s.color = FUN_00522508(2);
        if (s.color == 0)
        {
          s.color = FUN_00522508(2);
          s.spawn_y = ((-(unsigned int)(s.color == 0) & 0xfffffff8) + 4) + g_world_player_tile_y;
          s.color = FUN_00522508(9);
          s.spawn_x = g_world_player_tile_x + s.color - 4;
        }
        else
        {
          s.color = FUN_00522508(2);
          s.spawn_x = ((-(unsigned int)(s.color == 0) & 0xfffffff8) + 4) + g_world_player_tile_x;
          s.color = FUN_00522508(9);
          s.spawn_y = g_world_player_tile_y + s.color - 4;
        }

        s.terrain_mask = FUN_005611c8(FUN_0043146b(s.spawn_x, s.spawn_y));
      } while (s.terrain_mask == 0);

      do
      {
        s.color = FUN_00522508(6);
        s.selected_color = (unsigned char)s.color;
      } while ((s.terrain_mask & (1U << (s.selected_color & 0x1f))) == 0);

      s.victory_count = (*(int *)&g_ini_string_scratch[0x20] + ((*(int *)&g_ini_string_scratch[0x20] >> 0x1f) & 7U)) >> 3;
      for (s.i = 0; s.i < 1000; s.i = s.i + 1)
      {
        if (((int)(char)g_duel_victory_log[s.i] >> 4) == s.color)
        {
          s.victory_count = s.victory_count + 1;
        }
      }

      s.tmp = ClampIntToRange(0x80 / (s.victory_count + 4), 6, 0x14);
      s.tmp = FUN_00522508(s.tmp);

      switch (s.tmp + 5 / (s.victory_count + 1))
      {
      case 0:
      case 1:
      case 2:
      case 4:
        s.creature_tier = 10;
        break;
      case 3:
      case 9:
      case 0xc:
      case 0xe:
      default:
        s.creature_tier = 0;
        break;
      case 5:
        s.creature_tier = 8;
        break;
      case 6:
      case 8:
      case 0xb:
        s.creature_tier = 6;
        break;
      case 7:
      case 0xa:
      case 0xd:
      case 0xf:
        s.creature_tier = 4;
        break;
      }

      if (s.creature_tier == 10)
      {
        s.creature_tier = FUN_00522508(200);
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
        s.creature_tier = (int)gs_creature_names_00591a08[-g_current_quest_type].metadata[1];
      }

      if ((s.creature_tier == 0) ||
          (((g_shandalar_difficulty != 0) || ((s.slot_index != 5) && (s.slot_index != 6))) &&
           ((g_shandalar_difficulty != 1) || (s.slot_index != 6)) &&
           (((4 - g_shandalar_difficulty) * 0x163f5000) /
                ((g_monster_timer / 0xf) * (g_monster_timer / 0xf) + 0x515a19) <=
            g_monster_timer - g_lair_or_monster_slots[s.slot_index].respawn_timestamp)))
      {
        if (s.creature_tier == 0)
        {
          g_lair_or_monster_slots[s.slot_index].entry_type = 0;
        }
        else
        {
          s.creature_tier = PickRandomCreatureTypeForWizardTier(s.color, s.creature_tier);
          g_lair_or_monster_slots[s.slot_index].entry_type = s.creature_tier;

          s.victory_count = 0;
          for (s.i = 0; (s.i < 1000) && (g_duel_victory_log[s.i] != '\0'); s.i = s.i + 1)
          {
            if ((gs_creature_names_00591a08[s.creature_tier].metadata[3] == (g_duel_victory_log[s.i] & 0xf)) &&
                (((int)(char)g_duel_victory_log[s.i] >> 4) == s.color))
            {
              s.victory_count = s.victory_count + 1;
            }
          }

          if (s.victory_count > 8)
          {
            s.creature_tier = -1;
            g_lair_or_monster_slots[s.slot_index].entry_type = -1;
          }
        }

        g_lair_or_monster_slots[s.slot_index].world_x = s.spawn_x * 0x20 + 0x10;
        g_lair_or_monster_slots[s.slot_index].world_y = s.spawn_y * 0x20 + 0x10;
        g_lair_or_monster_slots[s.slot_index].color = s.color;
        ((unsigned char *)&g_lair_or_monster_slots[s.slot_index].movement_heading)[0] = 0;
        ((unsigned char *)&g_lair_or_monster_slots[s.slot_index].movement_heading)[1] = 0;

        if (((1U << (s.selected_color & 0x1f)) & (unsigned int)gs_creature_names_00591a08[s.creature_tier].metadata[4]) &&
            ((g_defeated_wizards_bitmap & (1U << (s.selected_color & 0x1f))) != 0))
        {
          s.terrain_mask = (unsigned int)gs_creature_names_00591a08[s.creature_tier].metadata[4] & ~(1U << (s.selected_color & 0x1f));
          if ((s.terrain_mask == 0) || ((s.terrain_mask & g_defeated_wizards_bitmap) != 0))
          {
            FreeOpeningMenuSpriteWorkEntries(s.slot_index, s.slot_index + 8);
            g_lair_or_monster_slots[s.slot_index].entry_type = -1;
          }
          else
          {
            s.color = FUN_0040dffd((int)s.terrain_mask);
            g_lair_or_monster_slots[s.slot_index].color = s.color;
          }
        }

        if ((FUN_004314ca(s.spawn_x, s.spawn_y) & 0x10) != 0)
        {
          FreeOpeningMenuSpriteWorkEntries(s.slot_index, s.slot_index + 8);
          g_lair_or_monster_slots[s.slot_index].entry_type = -1;
        }

        if ((s.slot_index == 0) && (g_current_quest_type < 0) && (g_lair_or_monster_slots[0].entry_type != -g_current_quest_type))
        {
          FreeOpeningMenuSpriteWorkEntries(0, 8);
          g_lair_or_monster_slots[0].entry_type = -1;
        }

        if (g_lair_or_monster_slots[s.slot_index].entry_type < 1)
        {
          if (g_lair_or_monster_slots[s.slot_index].entry_type != -1)
          {
            for (s.slot_u = 0; s.slot_u < 8; s.slot_u = s.slot_u + 1)
            {
              if ((s.slot_u != (unsigned int)s.slot_index) &&
                  (g_lair_or_monster_slots[s.slot_u].entry_type != -1) &&
                  (g_lair_or_monster_slots[s.slot_u].world_x == g_lair_or_monster_slots[s.slot_index].world_x) &&
                  (g_lair_or_monster_slots[s.slot_u].world_y == g_lair_or_monster_slots[s.slot_index].world_y))
              {
                g_lair_or_monster_slots[s.slot_index].entry_type = -1;
              }
            }
          }
        }
        else
        {
          FreeOpeningMenuSpriteWorkEntries(s.slot_index, s.slot_index + 8);
        }

        s.can_run_movement = 1;
      }
    }
    else
    {
      s.can_run_movement = 1;
    }

    if (s.can_run_movement)
    {
      s.creature_tier = g_lair_or_monster_slots[s.slot_index].entry_type;
      if (s.creature_tier != -1)
      {
        if (s.creature_tier > 0)
        {
          FUN_004bc6d3(s.creature_tier, s.slot_index, s.slot_index + 8);
        }

        s.speed = 1;
        switch (gs_creature_names_00591a08[s.creature_tier].metadata[3])
        {
        case 0:
          s.range = -1;
          s.speed = 0;
          break;
        case 1:
        case 2:
          s.range = 0x20;
          break;
        case 3:
        case 4:
        case 0xc:
          s.range = 0x30;
          break;
        case 5:
        case 0xb:
          s.range = 0x40;
          break;
        case 6:
        case 0xa:
          s.range = 0x60;
          break;
        case 9:
          s.range = 0x20;
          s.speed = 2;
          break;
        default:
          s.range = 0;
          break;
        }

        s.range = (s.range * 3) / 2;
        if ((gs_creature_names_00591a08[s.creature_tier].metadata[0xb] & 2U) != 0)
        {
          s.range = s.range << 1;
        }

        s.tmp = s.distance_to_player;
        if (s.slot_index != s.nearest_slot)
        {
          s.range = s.range / 2;
        }
        while (abs(s.range) < s.tmp)
        {
          s.speed = s.speed << 1;
          s.tmp = s.tmp / 2;
        }

        s.prev_x = g_lair_or_monster_slots[s.slot_index].world_x;
        s.prev_y = g_lair_or_monster_slots[s.slot_index].world_y;

        if (DAT_0073ea70[7] == 0)
        {
          s.target_dx = g_world_player_x - s.prev_x;
          s.target_dy = g_world_player_y - s.prev_y;
        }
        else
        {
          s.tmp = ClampIntToRange(s.distance_to_player / 3, 0, g_shandalar_difficulty << 4);
          s.target_dx = (s.tmp * g_neighbor_dx[DAT_0073ea70[5]] + g_world_player_x) - s.prev_x;
          s.tmp = ClampIntToRange(s.distance_to_player / 3, 0, g_shandalar_difficulty << 4);
          s.target_dy = (s.tmp * g_neighbor_dy[DAT_0073ea70[5]] + g_world_player_y) - s.prev_y;
        }

        if ((gs_creature_names_00591a08[s.creature_tier].metadata[10] & 1U) != 0)
        {
          s.speed = 2;
        }

        if ((s.distance_to_player < 0x40) && (s.slot_index < 7))
        {
          s.victory_count = 0;
          for (s.i = 0; (s.i < 1000) && (g_duel_victory_log[s.i] != '\0'); s.i = s.i + 1)
          {
            if ((gs_creature_names_00591a08[s.creature_tier].metadata[3] == (g_duel_victory_log[s.i] & 0xf)) &&
                (((int)(char)g_duel_victory_log[s.i] >> 4) == g_lair_or_monster_slots[s.slot_index].color))
            {
              s.victory_count = s.victory_count + 1;
            }
          }

          if (g_shandalar_difficulty + 3 <= s.victory_count)
          {
            s.target_dx = -s.target_dx;
            s.target_dy = -s.target_dy;
          }
        }

        if ((s.slot_index == 7) && (s.distance_to_player > 0x18))
        {
          s.tmp = FindNearestTownIndex((int)((g_lair_or_monster_slots[7].world_x + ((g_lair_or_monster_slots[7].world_x >> 0x1f) & 0x1fU)) >> 5),
                                       (int)((g_lair_or_monster_slots[7].world_y + ((g_lair_or_monster_slots[7].world_y >> 0x1f) & 0x1fU)) >> 5));
          s.target_dx = (g_town_slots[s.tmp].world_x * 0x20 - s.prev_x) + (g_monster_timer & 0x1fU);
          s.target_dy = (g_town_slots[s.tmp].world_y * 0x20 - s.prev_y) + ((g_monster_timer & 0x3eU) >> 1);
        }

        s.terrain_mask = FUN_005611c8(FUN_0043146b(
            (int)((g_lair_or_monster_slots[s.slot_index].world_x + ((g_lair_or_monster_slots[s.slot_index].world_x >> 0x1f) & 0x1fU)) >> 5),
            (int)((g_lair_or_monster_slots[s.slot_index].world_y + ((g_lair_or_monster_slots[s.slot_index].world_y >> 0x1f) & 0x1fU)) >> 5)));

        if (((gs_creature_names_00591a08[s.creature_tier].metadata[10] & 0xf8U) != 0) &&
            ((*(unsigned int *)(gs_creature_names_00591a08[s.creature_tier].metadata + 10) & (s.terrain_mask << 3)) != 0) &&
            (s.speed > 1))
        {
          s.speed = s.speed / 2;
        }

        s.move_dir = 0;
        s.target_dx = s.target_dx + g_neighbor_dx[(signed char)g_lair_or_monster_slots[s.slot_index].movement_heading] * 8;
        s.target_dy = s.target_dy + g_neighbor_dy[(signed char)g_lair_or_monster_slots[s.slot_index].movement_heading] * 8;

        if (abs(s.target_dy) * 2 < abs(s.target_dx))
        {
          if (s.target_dx < 1)
          {
            s.move_dir = 7;
          }
          else
          {
            s.move_dir = 3;
          }
        }

        if (abs(s.target_dx) * 2 < abs(s.target_dy))
        {
          if (s.target_dy < 1)
          {
            s.move_dir = 1;
          }
          else
          {
            s.move_dir = 5;
          }
        }

        if (s.move_dir == 0)
        {
          if (s.target_dx < 1)
          {
            if (s.target_dy < 1)
            {
              s.move_dir = 8;
            }
            else
            {
              s.move_dir = 6;
            }
          }
          else if (s.target_dy < 1)
          {
            s.move_dir = 2;
          }
          else
          {
            s.move_dir = 4;
          }
        }

        if (((g_monster_timer + s.slot_index) & 3) == 0)
        {
          ((unsigned char *)&g_lair_or_monster_slots[s.slot_index].movement_heading)[0] = (unsigned char)s.move_dir;
        }
        else
        {
          s.move_dir = (signed char)((unsigned char *)&g_lair_or_monster_slots[s.slot_index].movement_heading)[0];
        }

        g_lair_or_monster_slots[s.slot_index].world_x = g_lair_or_monster_slots[s.slot_index].world_x + g_neighbor_dx[s.move_dir] * 4;
        g_lair_or_monster_slots[s.slot_index].world_y = g_lair_or_monster_slots[s.slot_index].world_y + g_neighbor_dy[s.move_dir] * 4;

        s.tmp = 1;
        for (s.i = 0; s.i < 6; s.i = s.i + 1)
        {
          if ((s.i != s.slot_index) && (g_lair_or_monster_slots[s.i].entry_type != 0) && (s.distance_to_player > 0x1f))
          {
            s.new_distance = FUN_004ecf30(g_lair_or_monster_slots[s.slot_index].world_x - g_lair_or_monster_slots[s.i].world_x,
                                          g_lair_or_monster_slots[s.slot_index].world_y - g_lair_or_monster_slots[s.i].world_y);
            if (s.new_distance < 0x20)
            {
              s.color = FUN_004ecf30(s.prev_x - g_lair_or_monster_slots[s.i].world_x,
                                     s.prev_y - g_lair_or_monster_slots[s.i].world_y);
              if (s.new_distance < s.color)
              {
                s.tmp = 0;
              }
            }
          }
        }

        if (s.tmp != 0)
        {
          if (((gs_creature_names_00591a08[s.creature_tier].metadata[10] & 4U) != 0) && ((g_monster_timer & 0x3fU) == 0))
          {
            s.color = FUN_00522508(2);
            if (s.color == 0)
            {
              s.color = FUN_00522508(2);
              if (s.color == 0)
              {
                g_lair_or_monster_slots[s.slot_index].world_y = g_lair_or_monster_slots[s.slot_index].world_y - 0x20;
              }
              else
              {
                g_lair_or_monster_slots[s.slot_index].world_y = g_lair_or_monster_slots[s.slot_index].world_y + 0x20;
              }
            }
            else
            {
              s.color = FUN_00522508(2);
              if (s.color == 0)
              {
                g_lair_or_monster_slots[s.slot_index].world_x = g_lair_or_monster_slots[s.slot_index].world_x - 0x20;
              }
              else
              {
                g_lair_or_monster_slots[s.slot_index].world_x = g_lair_or_monster_slots[s.slot_index].world_x + 0x20;
              }
            }
          }

          s.terrain_mask = FUN_005611c8(FUN_0043146b(
              (int)((g_lair_or_monster_slots[s.slot_index].world_x + ((g_lair_or_monster_slots[s.slot_index].world_x >> 0x1f) & 0x1fU)) >> 5),
              (int)((g_lair_or_monster_slots[s.slot_index].world_y + ((g_lair_or_monster_slots[s.slot_index].world_y >> 0x1f) & 0x1fU)) >> 5)));

          if (((s.terrain_mask & (unsigned int)gs_creature_names_00591a08[s.creature_tier].metadata[4]) == 0) &&
              (s.range >= 0) && (s.slot_index != 7))
          {
            g_lair_or_monster_slots[s.slot_index].world_x = s.prev_x;
            g_lair_or_monster_slots[s.slot_index].world_y = s.prev_y;
            s.terrain_mask = FUN_005611c8(FUN_0043146b((int)((s.prev_x + ((s.prev_x >> 0x1f) & 0x1fU)) >> 5),
                                                       (int)((s.prev_y + ((s.prev_y >> 0x1f) & 0x1fU)) >> 5)));
            ((unsigned char *)&g_lair_or_monster_slots[s.slot_index].movement_heading)[1] = 0;

            if ((s.terrain_mask & (unsigned int)gs_creature_names_00591a08[s.creature_tier].metadata[4]) == 0)
            {
              FreeOpeningMenuSpriteWorkEntries(s.slot_index, s.slot_index + 8);
              g_lair_or_monster_slots[s.slot_index].entry_type = -1;
            }
          }
          else
          {
            g_lair_or_monster_slots[s.slot_index].world_x = g_neighbor_dx[s.move_dir] * s.speed + s.prev_x;
            g_lair_or_monster_slots[s.slot_index].world_y = g_neighbor_dy[s.move_dir] * s.speed + s.prev_y;
            ((unsigned char *)&g_lair_or_monster_slots[s.slot_index].movement_heading)[0] = (unsigned char)s.move_dir;
            ((unsigned char *)&g_lair_or_monster_slots[s.slot_index].movement_heading)[1] =
                ((unsigned char *)&g_lair_or_monster_slots[s.slot_index].movement_heading)[1] + 1;
            if (((unsigned char *)&g_lair_or_monster_slots[s.slot_index].movement_heading)[1] > 4)
            {
              ((unsigned char *)&g_lair_or_monster_slots[s.slot_index].movement_heading)[1] = 1;
            }
          }

          s.new_distance = FUN_004ecf30(g_world_player_x - g_lair_or_monster_slots[s.slot_index].world_x,
                                        g_world_player_y - g_lair_or_monster_slots[s.slot_index].world_y);
          if (s.new_distance < ((g_lair_or_monster_slots[s.slot_index].entry_type == 0) ? 0x1a : 0x10))
          {
            SaveGameToSlot(3);
            s.tmp = FUN_004f2ec0(s.slot_index, g_lair_or_monster_slots[s.slot_index].color);
            g_lair_or_monster_slots[s.slot_index].respawn_timestamp = g_monster_timer;
            RefreshAdventureInterfaceLayout();
            if ((s.slot_index == 7) && (s.tmp <= 0))
            {
              FUN_00562169();
            }
            else if (s.slot_index == 7)
            {
              FUN_004290e2(0xd, g_lair_or_monster_slots[s.slot_index].color);
              g_siege_indicator = 0;
            }
            FreeOpeningMenuSpriteWorkEntries(s.slot_index, s.slot_index + 8);
            g_lair_or_monster_slots[s.slot_index].entry_type = -1;
            EnsureAdvfac64Loaded(0);
            g_monster_timer = g_monster_timer | 0x1f;
            SaveGameToSlot(3);
          }
          else if ((s.creature_tier != 0) &&
                   (((signed char *)&g_lair_or_monster_slots[s.slot_index].movement_heading)[1] != 0) &&
                   (s.distance_to_player < 0x50) && (FUN_00522508(s.distance_to_player) < 4))
          {
            FUN_005614c3(s.creature_tier, 0x68 - s.distance_to_player / 2, 100 - s.distance_to_player / 3,
                         g_neighbor_dx[((signed char)((unsigned char *)&g_lair_or_monster_slots[s.slot_index].movement_heading)[0] + 2U) &
                                       7] *
                             100);
          }
        }
        else
        {
          g_lair_or_monster_slots[s.slot_index].world_x = s.prev_x;
          g_lair_or_monster_slots[s.slot_index].world_y = s.prev_y;
          ((unsigned char *)&g_lair_or_monster_slots[s.slot_index].movement_heading)[1] = 0;
        }
      }
    }
  }
}

// FUNCTION: SHANDALAR 0x005616f9
void FUN_005616f9(void)
{
  struct
  {
    int ruled_by_color_count[7];
    int lair_world_x_by_color[7];
    int lair_world_y_by_color[7];
    int town_index;
    int color;
    int best_score;
    int nearest_distance;
    int best_color;
    int best_town_index;
    int creature_tier;
    int dist;
    int scan_index;
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
      s.color = FUN_0040dffd((int)FUN_005611c8(FUN_0043146b(g_town_slots[s.scan_index].world_x, g_town_slots[s.scan_index].world_y)));
      s.lair_world_x_by_color[s.color] = g_town_slots[s.scan_index].world_x;
      s.lair_world_y_by_color[s.color] = g_town_slots[s.scan_index].world_y;
    }

    if (*(char *)((char *)&g_town_slots[s.scan_index].status_and_ruling_wizard + 1) != '\0')
    {
      *(int *)((char *)s.ruled_by_color_count + (((int)(g_town_slots[s.scan_index].status_and_ruling_wizard & 0xffffff3f)) >> 6)) += 1;
    }
  }

  s.best_score = 0x7fff;
  s.best_color = -1;
  s.best_town_index = -1;

  for (s.town_index = 0; s.town_index < 0x80; s.town_index = s.town_index + 1)
  {
    if ((((*(char *)((char *)&g_town_slots[s.town_index].status_and_ruling_wizard + 1) == '\0') && (g_town_slots[s.town_index].location_type != 4)) &&
         (g_town_slots[s.town_index].location_type != 1)) &&
        (g_town_slots[s.town_index].location_type != 5))
    {
      s.nearest_distance = 0x7fff;
      for (s.scan_index = 1; s.scan_index < 6; s.scan_index = s.scan_index + 1)
      {
        if (s.lair_world_x_by_color[s.scan_index] != -1)
        {
          s.dist = FUN_004ecf30(g_town_slots[s.town_index].world_x - s.lair_world_x_by_color[s.scan_index],
                                g_town_slots[s.town_index].world_y - s.lair_world_y_by_color[s.scan_index]);
          if (s.dist < s.nearest_distance)
          {
            s.nearest_distance = s.dist;
            s.color = s.scan_index;
          }
        }
      }

      s.score = FUN_00522508(0x80) + s.ruled_by_color_count[s.color] * 0x20;
      if (s.score < s.best_score)
      {
        s.best_score = s.score;
        s.best_town_index = s.town_index;
        s.best_color = s.color;
      }
    }
  }

  s.color = s.best_color;
  s.town_index = s.best_town_index;

  if (s.color != -1)
  {
    s.scan_index = 7;

    switch ((int)((g_siege_timer + ((g_siege_timer >> 0x1f) & 0x7fU)) >> 7))
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
      if ((g_town_slots[s.town_index].world_y & 1) == 0)
      {
        s.creature_tier = 0x10;
      }
      else
      {
        s.creature_tier = 0xc;
      }
      break;
    }

    FreeOpeningMenuSpriteWorkEntries(7, 0xf);
    g_lair_or_monster_slots[s.scan_index].entry_type = PickRandomCreatureTypeForWizardTier(s.color, s.creature_tier);
    g_lair_or_monster_slots[s.scan_index].world_x = g_town_slots[s.town_index].world_x * 0x20 + 0x10;
    g_lair_or_monster_slots[s.scan_index].world_y = g_town_slots[s.town_index].world_y * 0x20 + 0x10;
    g_lair_or_monster_slots[s.scan_index].color = s.color;

    g_wizard_siege_count = g_wizard_siege_count + 1;
    FUN_00431526(0x80, g_town_slots[s.town_index].world_x, g_town_slots[s.town_index].world_y);

    PlaySoundEffectOnChannel("x:sound\\newsflash.wav", 0x97, 100, 100, 0);
    AnimateVisitBackdropZoomIn("newsback.pic");

    strcpy(g_ui_message_buffer, gs_newsflash_0077d140[0]);
    FUN_00564e70(g_ui_message_buffer + strlen(g_ui_message_buffer), 0x1000, gs_newsflash_0077d140[1],
                 gs_wizardnames_0077ee70 + s.color * 0x32,
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
void FUN_00562169(void)
{
  int town_index;
  int wizard_color;
  int ruled_count;
  int required_count;
  int i;
  int font_size;
  size_t text_len;

  g_siege_indicator = 0;

  if (g_lair_or_monster_slots[7].entry_type == -1)
  {
    return;
  }

  ClearInputAndWaitForMouseRelease();
  wizard_color = g_lair_or_monster_slots[7].color;
  town_index = FindNearestTownIndex((int)((g_lair_or_monster_slots[7].world_x + ((g_lair_or_monster_slots[7].world_x >> 0x1f) & 0x1fU)) >> 5),
                                    (int)((g_lair_or_monster_slots[7].world_y + ((g_lair_or_monster_slots[7].world_y >> 0x1f) & 0x1fU)) >> 5));

  g_town_slots[town_index].status_and_ruling_wizard = (g_town_slots[town_index].status_and_ruling_wizard & 0xffff00ff) | (wizard_color << 8);

  ruled_count = 0;
  for (i = 0; i < 0x80; i = i + 1)
  {
    if ((int)g_town_slots[i].status_and_ruling_wizard >> 8 == wizard_color)
    {
      ruled_count = ruled_count + 1;
    }
  }

  required_count = (Scards[0xb].worldmagic_city == 0) ? 5 : 3;

  PlaySoundEffectOnChannel("x:sound\\newsflash.wav", 0x97, 100, 100, 0);
  AnimateVisitBackdropZoomIn("newsback.pic");

  strcpy(g_ui_message_buffer, gs_newsflash_0077d140[0]);
  BuildTownDisplayName(town_index);
  text_len = strlen(g_ui_message_buffer);
  if (IsWizardColorFeminine(wizard_color) == 0)
  {
    FUN_00564e70(g_ui_message_buffer + text_len, 0x1000, gs_newsflash_0077d140[5]);
  }
  else
  {
    FUN_00564e70(g_ui_message_buffer + text_len, 0x1000, gs_newsflash_0077d140[4]);
  }

  if (required_count == ruled_count)
  {
    if (IsWizardColorFeminine(wizard_color) == 0)
    {
      strcat(g_ui_message_buffer, gs_newsflash_0077d140[9]);
    }
    else
    {
      strcat(g_ui_message_buffer, gs_newsflash_0077d140[8]);
    }
  }
  else
  {
    text_len = strlen(g_ui_message_buffer);
    if (IsWizardColorFeminine(wizard_color) == 0)
    {
      sprintf(g_ui_message_buffer + text_len, gs_newsflash_0077d140[7], required_count - ruled_count);
    }
    else
    {
      sprintf(g_ui_message_buffer + text_len, gs_newsflash_0077d140[6], required_count - ruled_count);
    }
  }

  if ((g_town_slots[town_index].status_and_ruling_wizard & 1) != 0)
  {
    g_town_slots[town_index].status_and_ruling_wizard = g_town_slots[town_index].status_and_ruling_wizard & 0xfffffffe;
    strcat(g_ui_message_buffer, gs_newsflash_0077d140[10]);
  }

  g_wizard_siege_count = g_wizard_siege_count + 1;
  PTR_DAT_005832b4->font_slot = 5;
  DrawTextAt(PTR_DAT_005832b4, 0xbe, 0x140, 0xf7, g_ui_message_buffer);
  PTR_DAT_005832b4->font_slot = 1;
  ClearInputAndWaitForMouseRelease();
  WaitForInputEventUnlessBlocked();

  FreeOpeningMenuSpriteWorkEntries(7, 0xf);
  g_lair_or_monster_slots[7].entry_type = -1;
  ShowStatsWindow(3, wizard_color);
  RefreshAdventureInterfaceLayout();
  g_siege_indicator = 0;

  if (required_count <= ruled_count)
  {
    AnimatePaletteToColor(0, DAT_00589dec);
    LoadPcxIntoPageOpaque(1, "uth-arz.pic");
    font_size = ((global_screen_width == 0x280) || (global_screen_width == 800)) ? 0x10 : 0xc;
    PTR_DAT_005832dc->font_slot = 5;
    SetFontStyleSize(5, (unsigned int)ScaleUiCoordinate(font_size));

    strcpy(g_ui_message_buffer, gs_questfailed_0077c580[1]);
    if (IsWizardColorFeminine(wizard_color) == 0)
    {
      sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), gs_questfailed_0077c580[3], gs_wizardnames_0077ee70 + wizard_color * 0x32);
    }
    else
    {
      sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), gs_questfailed_0077c580[2], gs_wizardnames_0077ee70 + wizard_color * 0x32);
    }
    strcat(g_ui_message_buffer, gs_questfailed_0077c580[5]);

    DrawFormattedTextShadowedCentered(PTR_DAT_005832dc, 0xea, 0x140, 0x1e0 - GetFontLineHeight(PTR_DAT_005832dc->font_slot) * 7, g_ui_message_buffer);
    StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, 0, 0x280, 0x1e0, PTR_DAT_005832b4, 0, 0, global_screen_width, global_screen_height);
    ClearInputAndWaitForMouseRelease();
    WaitForInputEventUnlessBlocked();
    AnimatePaletteToColor(0, DAT_00589dec);
    DAT_009300f0 = 1;
    UnloadStatWinDllExports();
    FUN_00469099();
    exit(0);
  }
}
