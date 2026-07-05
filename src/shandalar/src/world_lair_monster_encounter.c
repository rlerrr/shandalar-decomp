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

typedef unsigned int uint;
typedef unsigned char byte;
typedef unsigned char uchar;
typedef int undefined3;
typedef int undefined4;
typedef __int64 longlong;
#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif
#ifndef bool
#define bool int
#endif
#define CONCAT31(a, b) ((int)(b))
#define CONCAT44(a, b) (((__int64)(a) << 32) | (unsigned int)(b))
#define ANTE_CARD(row, slot) (*(int *)(global_ante_cards[(row)] + (slot) * 4))

typedef struct
{
  EncodedImage *frame[4][9];
  EncodedImage *icon_rows[4][4];
} DialogBoxSpriteBank;

extern int global_screen_width;
extern int global_screen_height;
extern int DAT_00589dec;
extern int DAT_009300f0;
extern int DAT_0073ea70[8];
extern int DAT_0059121c;
extern int DAT_0078990c[10];
extern int DAT_00789938;
extern int DAT_0078df68;
extern int DAT_007894f4;
extern int DAT_00716024;
// GLOBAL: SHANDALAR 0x007483e4
int _DAT_007483e4;
// GLOBAL: SHANDALAR 0x0073c7e4
int DAT_0073c7e4;
// GLOBAL: SHANDALAR 0x0093a870
char DAT_0093a870[0x20];
extern int g_world_magic_offer_slot_index;
extern int g_world_scroll_cache_ready;
extern int g_text_menu_color_normal;
extern int g_text_menu_color_selected;
extern int(__cdecl *g_town_dialog_callback)(void);
extern int g_duel_ante_card_ids[16];
// GLOBAL: MAGIC 0x00896714
// GLOBAL: SHANDALAR 0x008aa914
int DAT_00896714;
extern int unk_008b35ec;
extern int unk_00742fc4;
extern int DAT_008bd200;
extern char g_ui_message_buffer[0x1000];
extern char g_creature_name_with_article_buffer[0x80];
extern HDC global_main_hdc;
extern HPALETTE g_realized_palette_handle;
extern RpBitsPalettePacket g_palette_data_words;
extern int g_graphics_bpp;
extern DIBSurface *g_graphics_pages[10];
extern FacemakerWindowBounds *PTR_DAT_005832b4;
extern FacemakerWindowBounds *PTR_DAT_005832dc;
extern DialogBoxSpriteBank g_dialog_box_sprite_bank;
extern EncodedImage *g_world_magic_avatar_sprites[5];
extern EncodedImage *g_worlds_extra_sprite_entries[4];
extern int g_advfac64_load_state;

char *BuildCreatureNameWithArticle(int creature_type);
char *BuildTownDisplayName(int town_index);
char *FUN_00428783(unsigned int mana_mask);
char *FUN_004f2e17(int town_index);
char *FUN_005081e0(int dungeon_index);
char *FUN_00561441(int creature_type);
DWORD FUN_00564e70(char *dst, DWORD max_length, LPCVOID format, ...);
EncodedImage *EncodeSpriteFromPage(int page_number, int x, int y, int width, int height);
int BuyAnyCardFromTown(int color, int type);
int ClampIntToRange(int value, int min_value, int max_value);
int CountDuelPoolEligibleTowns(void);
int DrawRandomCardFromInitialLibrary(int param_1);
int ExitIfNoUsableDeckCards(void);
int FindDeckSlotForQuestColorAndType(unsigned char color, unsigned char type);
int FindNearestTownIndex(int world_x, int world_y);
int FUN_0040dffd(int mask);
int FUN_004bb1cf(unsigned int card_id);
unsigned int FUN_004bb458(int world_magic_slot_index);
unsigned int FUN_0043146b(int x, int y);
int FUN_00469099(void);
int FUN_004ecf30(int x, int y);
int FUN_0050caa0(int dungeon_index);
int FUN_00522508(int param_1);
int FUN_005501dc(int value);
int FUN_0056a515(int player, int *cards, int count, char *title, int allow_cancel, int *out_selection);
int FUN_0056bd9d(unsigned int card_id);
unsigned int FUN_0056bcf7(int param_1, int param_2);
int FUN_0056c0e5(int card_color, int color_mask, int param_3);
int FUN_0056c5ea(unsigned int card_id);
unsigned int FUN_0056c705(int csvid);
int FUN_0056d5c0(int param_1, int *param_2);
int GetCardAvailabilityMask(unsigned int card_id);
int GetFontLineHeight(int font_slot);
int GetQuestCardClassName(int card_class);
int GetRelativeWorldQuadrant(int world_x, int world_y);
int IsCreatureTypeFeminine(int creature_type);
int IsWizardColorFeminine(int color_index);
int LoadCreatureDuelDeck(int creature_type, unsigned int color, int param_3, int param_4);
int MeasureMultilineTextWidth(FacemakerWindowBounds *dst, char *text);
int MeasureTextLineWidth(char *text);
int PickRandomCreatureTypeForWizardTier(int wizard_color, int creature_tier);
int PickWisemanCastleDungeonSlotForHint(void);
int ReadSpriteEntryPointers(EncodedImage **out_sprite_entries, char *sprite_path);
int RecountDeckCardTotals(void);
int RemoveCardFromDeckById(unsigned int card_id);
int RunCardBrowser(char *title, int param_2, unsigned int param_3, int param_4, int param_5);
int RunDuelEngine(unsigned int card_id, int creature_type);
int RunTextMenuAt(char *menu_text, int x, int y);
int RunTextMenuAtScaled(char *menu_text, int x_320_scale, unsigned int y_200_scale);
int ScaleUiCoordinate(int value);
unsigned int WaitForInputEventUnlessBlocked(void);
void AnimatePaletteToColor(int color_index, int palette_id);
void BeginSpriteEncodeSession(void);
void BlitRectByRandomTileOrderInPlace(HDC dst_dc, int dst_x, int dst_y, int width, int height, int tile_w, int tile_h, HDC src_dc);
void ClearInputAndWaitForMouseRelease(void);
int ClearGraphicsPageWithPaletteColor(int page_number, int color_index);
void ClearAndLoadInitialLibraryFromDeckFile(char *deck_path, int library_index, unsigned int color_filter, int speed_filter);
void DestroyCachedCardArt(void);
void DrawCenteredTextLineClamped(char *text, int center_x, int y, int color_index);
void DrawCreaturePortrait(int creature_type, int x, int y, int param_4, int param_5);
void DrawEncodedImageResampled(FacemakerWindowBounds *dst, int x, int y, int width, int height, EncodedImage *encoded_image);
void DrawFormattedTextNoShadowCentered();
void DrawFormattedTextShadowedCentered();
void DrawTiledDialogBoxFrame(int x, int y, int width, int height, int frame_style);
void DrawTextAt(FacemakerWindowBounds *window, int color, int x, int y, char *text);
void DrawUiScaledCenteredText(char *text, int x, int y, int color_index);
void DrawVisitScreenOverlays(FacemakerWindowBounds *window);
void DrawWorldUiFormattedText();
void EnsureAdvfac64Loaded(int state);
void FadeInPaletteFromGray(int color_index, int palette_id);
void FinalizeSpriteEncodeSession(void);
void FreeSpriteBlob(EncodedImage *sprite_blob);
int FreeOpeningMenuSpriteWorkEntries(int work_entry_index_a, int work_entry_index_b);
void FUN_00431526(unsigned int mask, int x, int y);
void FUN_004290e2(int param_1, int param_2);
void FUN_0046ed03(void);
void FUN_0046ed33(void);
void FUN_004ce992(int param_1);
void FUN_004f2407(unsigned int card_id, int x, int y, int full_card, char *banner_text);
void FUN_004f263b(int card_id, int x, int y, int width, int height, int full_card, char *label);
void LoadPcxIntoPage(int page_number, char *path);
void LoadPcxIntoPageNoPalette(char *path);
void LoadPcxIntoPageOpaque(int page_number, char *path);
void LoadPcxResource(int page_number, int dst_x, int dst_y, char *path, void *palette_data);
void PlayDuelMusic(int mode);
void PlaySoundEffectOnChannel(char *sound_path, int channel, int volume, int pitch_percent, int pan_percent);
void RemoveDeckSlot(int deck_slot);
void RevealRandomCastleDungeonClue(int dungeon_index);
void ShowStatsWindow(int mode, int highlight);
void ShowWorldMapScreen(int mode);
void StretchBlitGraphicsRect(FacemakerWindowBounds *dst, int dst_x, int dst_y, int src_w, int src_h,
                             FacemakerWindowBounds *src, int src_x, int src_y, int copy_w, int copy_h);
void WisemanChooseLairCreatureAndSetupDuel(unsigned char color);

// GLOBAL: SHANDALAR 0x0058afb8
static char s_dbox2_spr_0058afb8[] = "dbox2.spr";
// GLOBAL: SHANDALAR 0x0058afd0
static char s_prdblk_pic_0058afd0[] = "prdblk.pic";
// GLOBAL: SHANDALAR 0x0058afe8
static char s_prdblu_pic_0058afe8[] = "prdblu.pic";
// GLOBAL: SHANDALAR 0x0058b000
static char s_prdgrn_pic_0058b000[] = "prdgrn.pic";
// GLOBAL: SHANDALAR 0x0058b018
static char s_prdrd_pic_0058b018[] = "prdrd.pic";
// GLOBAL: SHANDALAR 0x0058b030
static char s_prdwt_pic_0058b030[] = "prdwt.pic";
// GLOBAL: SHANDALAR 0x0058b03c
static char s_prdfrma_pic_0058b03c[] = "prdfrma.pic";
// GLOBAL: SHANDALAR 0x0058b048
static char DAT_0058b048[] = "%d";
// GLOBAL: SHANDALAR 0x0058b04c
static char DAT_0058b04c[] = "%d";
// GLOBAL: SHANDALAR 0x0058b050
static char s__d__d_0058b050[] = "%d/%d";
// GLOBAL: SHANDALAR 0x0058b058
static char DAT_0058b058[] = "%d";
// GLOBAL: SHANDALAR 0x0058b05c
static char s_prdfrmb_pic_0058b05c[] = "prdfrmb.pic";
// GLOBAL: SHANDALAR 0x0058b068
static char DAT_0058b068[] = "";
// GLOBAL: SHANDALAR 0x0058b06c
static char DAT_0058b06c[] = "";
// GLOBAL: SHANDALAR 0x0058b070
static char DAT_0058b070[] = "\n";
// GLOBAL: SHANDALAR 0x0058b074
static char s_dbox_spr_0058b074[] = "dbox.spr";
// GLOBAL: SHANDALAR 0x0058b080
static int DAT_0058b080 = 0;
// GLOBAL: SHANDALAR 0x0058b084
static char DAT_0058b084[] = " ";
// GLOBAL: SHANDALAR 0x0058b088
static char DAT_0058b088[] = "\n";
// GLOBAL: SHANDALAR 0x0058b08c
static char s_losedul2_pic_0058b08c[] = "losedul2.pic";
// GLOBAL: SHANDALAR 0x0058b09c
static char s_Prdblk_pic_0058b09c[] = "Prdblk.pic";
// GLOBAL: SHANDALAR 0x0058b0a8
static char s_advfac64_pic_0058b0a8[] = "advfac64.pic";
// GLOBAL: SHANDALAR 0x0058b0b8
static char s_dbox_spr_0058b0b8[] = "dbox.spr";
// GLOBAL: SHANDALAR 0x0058b0c4
static char s_advfac64_pic_0058b0c4[] = "advfac64.pic";
// GLOBAL: SHANDALAR 0x0058b0d4
static char s_dbox_spr_0058b0d4[] = "dbox.spr";
// GLOBAL: SHANDALAR 0x0058b0e0
static char DAT_0058b0e0[] = "";
// GLOBAL: SHANDALAR 0x0058b0e4
static char s_x_sound_dsummon_wav_0058b0e4[] = "x:sound\\dsummon.wav";
// GLOBAL: SHANDALAR 0x0058b0f8
static char s_decks_0_0058b0f8[] = "decks\\0";
// GLOBAL: SHANDALAR 0x0058b100
static char DAT_0058b100[] = "0";
// GLOBAL: SHANDALAR 0x0058b104
static char DAT_0058b104[] = "0";
// GLOBAL: SHANDALAR 0x0058b108
static char DAT_0058b108[] = ".dck";
// GLOBAL: SHANDALAR 0x0058b110
static char s_dbox_spr_0058b110[] = "dbox.spr";
// GLOBAL: SHANDALAR 0x0058b11c
static char s_advfac64_pic_0058b11c[] = "advfac64.pic";
// GLOBAL: SHANDALAR 0x0058b12c
static char s_winbak01_pic_0058b12c[] = "winbak01.pic";
// GLOBAL: SHANDALAR 0x0058b13c
static char s_endplak_pic_0058b13c[] = "endplak.pic";
// GLOBAL: SHANDALAR 0x0058b148
static char DAT_0058b148[] = "";
// GLOBAL: SHANDALAR 0x0058b14c
static char DAT_0058b14c[] = ", ";
// GLOBAL: SHANDALAR 0x0058b150
static char DAT_0058b150[] = "";
// GLOBAL: SHANDALAR 0x0058b154
static char s_winbak02_pic_0058b154[] = "winbak02.pic";
// GLOBAL: SHANDALAR 0x0058b164
static char s_x_sound_treasure_wav_0058b164[] = "x:sound\\treasure.wav";
// GLOBAL: SHANDALAR 0x0058b17c
static char s_winbak02_pic_0058b17c[] = "winbak02.pic";
// GLOBAL: SHANDALAR 0x0058b18c
static char DAT_0058b18c[] = "";
// GLOBAL: SHANDALAR 0x0058b190
static int DAT_0058b190 = 0;
// GLOBAL: SHANDALAR 0x0058b194
static char s_losedul2_pic_0058b194[] = "losedul2.pic";
// GLOBAL: SHANDALAR 0x0058b1a4
static char s_losedul2_pic_0058b1a4[] = "losedul2.pic";
// GLOBAL: SHANDALAR 0x0058b1b4
static char s_dbox_spr_0058b1b4[] = "dbox.spr";
// GLOBAL: SHANDALAR 0x0058b1c0
static char s_advfac64_pic_0058b1c0[] = "advfac64.pic";

// GLOBAL: SHANDALAR 0x0058ee08
static char s_zero_pic_marker_0058ee08[] = "0";
// GLOBAL: SHANDALAR 0x0058ee0c
static char s_0333_pic_0058ee0c[] = "0333.pic";
// GLOBAL: SHANDALAR 0x0058ee18
static char s_0873_pic_0058ee18[] = "0873.pic";
// GLOBAL: SHANDALAR 0x0058ee24
static char s_0281_pic_0058ee24[] = "0281.pic";
// GLOBAL: SHANDALAR 0x0058ee30
static char s_0329_pic_0058ee30[] = "0329.pic";
// GLOBAL: SHANDALAR 0x0058ee3c
static char s_0569_pic_0058ee3c[] = "0569.pic";
// GLOBAL: SHANDALAR 0x0058edb8
static char *PTR_DAT_0058edb8[6] = {
    s_zero_pic_marker_0058ee08,
    s_0333_pic_0058ee0c,
    s_0873_pic_0058ee18,
    s_0281_pic_0058ee24,
    s_0329_pic_0058ee30,
    s_0569_pic_0058ee3c};
// GLOBAL: SHANDALAR 0x0058ee48
static char s_x_sound_treasure_wav_0058ee48[] = "x:sound\\treasure.wav";
// GLOBAL: SHANDALAR 0x0058ee60
static char s_x_Duelsounds_aswanjag_wav_0058ee60[] = "x:Duelsounds\\aswanjag.wav";
// GLOBAL: SHANDALAR 0x0058ee7c
static char s_x_Duelsounds_aswanjag_wav_0058ee7c[] = "x:Duelsounds\\aswanjag.wav";
// GLOBAL: SHANDALAR 0x0058ee98
static char s_x_Duelsounds_aswanjag_wav_0058ee98[] = "x:Duelsounds\\aswanjag.wav";
// GLOBAL: SHANDALAR 0x0058eeb4
static char s_x_sound_treasure_wav_0058eeb4[] = "x:sound\\treasure.wav";
// GLOBAL: SHANDALAR 0x0058eecc
static char s_x_Duelsounds_aswanjag_wav_0058eecc[] = "x:Duelsounds\\aswanjag.wav";
// GLOBAL: SHANDALAR 0x0058eee8
static char s_x_sound_treasure_wav_0058eee8[] = "x:sound\\treasure.wav";
// GLOBAL: SHANDALAR 0x0058ef00
static char s_x_DuelSounds_summon_wav_0058ef00[] = "x:DuelSounds\\summon.wav";
// GLOBAL: SHANDALAR 0x0058ef18
static char s_x_DuelSounds_artifact_wav_0058ef18[] = "x:DuelSounds\\artifact.wav";
// GLOBAL: SHANDALAR 0x0058ef34
static char s_x_DuelSounds_enchant_wav_0058ef34[] = "x:DuelSounds\\enchant.wav";
// GLOBAL: SHANDALAR 0x0058ef50
static char s_x_DuelSounds_instant_wav_0058ef50[] = "x:DuelSounds\\instant.wav";
// GLOBAL: SHANDALAR 0x0058ef6c
static char s_x_DuelSounds_interupt_wav_0058ef6c[] = "x:DuelSounds\\interupt.wav";
// GLOBAL: SHANDALAR 0x0058ef88
static char s_x_DuelSounds_sorcery_wav_0058ef88[] = "x:DuelSounds\\sorcery.wav";
// GLOBAL: SHANDALAR 0x0058efa4
static char s_x_DuelSounds_black_wav_0058efa4[] = "x:DuelSounds\\black.wav";
// GLOBAL: SHANDALAR 0x0058efbc
static char s_x_DuelSounds_blue_wav_0058efbc[] = "x:DuelSounds\\blue.wav";
// GLOBAL: SHANDALAR 0x0058efd4
static char s_x_DuelSounds_green_wav_0058efd4[] = "x:DuelSounds\\green.wav";
// GLOBAL: SHANDALAR 0x0058efec
static char s_x_DuelSounds_red_wav_0058efec[] = "x:DuelSounds\\red.wav";
// GLOBAL: SHANDALAR 0x0058f004
static char s_x_DuelSounds_white_wav_0058f004[] = "x:DuelSounds\\white.wav";
// GLOBAL: SHANDALAR 0x0058f01c
static char s_x_DuelSounds_grey_wav_0058f01c[] = "x:DuelSounds\\grey.wav";
// GLOBAL: SHANDALAR 0x0058f034
static char s_x_sound_treasure_wav_0058f034[] = "x:sound\\treasure.wav";
// GLOBAL: SHANDALAR 0x0058f04c
static char DAT_0058f04c[] = "";
// GLOBAL: SHANDALAR 0x0058f050
static char s_winbak01_pic_0058f050[] = "winbak01.pic";
// GLOBAL: SHANDALAR 0x0058f180
static char s___0_1_2_3_4_5_6_7_8__0058f180[] = "?\n 0\n 1\n 2\n 3\n 4\n 5\n 6\n 7\n 8+\n";
// GLOBAL: SHANDALAR 0x0058f1a0
static char DAT_0058f1a0[] = " ";
// GLOBAL: SHANDALAR 0x0058f1a4
static char DAT_0058f1a4[] = ".\n";
// GLOBAL: SHANDALAR 0x00581a64
static char s_dungbutt_spr_00581a64[] = "dungbutt.spr";
// GLOBAL: SHANDALAR 0x00581a74
static char DAT_00581a74[] = "OK";
// GLOBAL: SHANDALAR 0x00590b54
static char DAT_00590b54[] = "";
// GLOBAL: SHANDALAR 0x00590b58
static char DAT_00590b58[] = " ";
// GLOBAL: SHANDALAR 0x00590b5c
static char DAT_00590b5c[] = "\n\n";
// GLOBAL: SHANDALAR 0x00590b60
static char DAT_00590b60[] = "\n";
// GLOBAL: SHANDALAR 0x00590b64
static char DAT_00590b64[] = "\n";
// GLOBAL: SHANDALAR 0x00590b68
static char DAT_00590b68[] = "\n";

// FUNCTION: SHANDALAR 0x00562893
void FUN_00562893(void)
{
  if (DAT_0059121c != 0)
  {
    sound_stop(0x10);
  }
  DAT_0059121c = 0;
}

// FUNCTION: SHANDALAR 0x004be570
void *__cdecl FUN_004be570(char *param_1, int param_2, int param_3)
{
  int local_330;
  int local_32c;
  int local_328;
  EncodedImage *local_324[200];

  local_330 = 0;
  if (g_dialog_box_sprite_bank.frame[0][0] != (EncodedImage *)0x0)
  {
    FreeSpriteBlob(g_dialog_box_sprite_bank.frame[0][0]);
  }
  ReadSpriteEntryPointers(local_324, param_1);
  for (local_328 = 0; local_328 < 4; local_328 = local_328 + 1)
  {
    for (local_32c = 0; local_32c < 9; local_32c = local_32c + 1)
    {
      g_dialog_box_sprite_bank.frame[local_328][local_32c] = local_324[local_330];
      local_330 = local_330 + 1;
    }
  }
  g_text_menu_color_normal = param_2;
  g_text_menu_color_selected = param_3;
  return g_dialog_box_sprite_bank.frame[0][0];
}

// FUNCTION: SHANDALAR 0x004ed7c8
undefined4 FUN_004ed7c8(char *param_1)
{
  DIBSurface *pDVar1;
  DIBSurface *pDVar2;
  int src_x;
  int src_y;
  int copy_w;
  int copy_h;

  src_x = ScaleUiCoordinate(0x40);
  src_y = ScaleUiCoordinate(0x30);
  copy_w = ScaleUiCoordinate(0x200);
  copy_h = ScaleUiCoordinate(0x118);
  pDVar1 = g_graphics_pages[PTR_DAT_005832dc->page_number];
  pDVar2 = g_graphics_pages[PTR_DAT_005832b4->page_number];
  LoadPcxResource(1, 0, global_screen_height + -0x118, param_1, (void *)0x0);
  StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, global_screen_height + -0x118, 0x200, 0x118, PTR_DAT_005832dc, src_x,
                          src_y, copy_w, copy_h);
  DrawVisitScreenOverlays(PTR_DAT_005832dc);
  BlitRectByRandomTileOrderInPlace(pDVar2->hTempDC, src_x, src_y, copy_w, copy_h, 5, 5, pDVar1->hTempDC);
  FUN_004ce992(0x2d);
  return 0;
}

// FUNCTION: SHANDALAR 0x00530770
int FUN_00530770(void)
{
  int card_index;

  do
  {
    do
    {
      card_index = FUN_00522508(g_card_count - 0x39);
    } while (FUN_0056c5ea(card_index) < 3);
  } while (GetCardAvailabilityMask(card_index) == 0);
  return card_index;
}

// FUNCTION: SHANDALAR 0x005307c3
char *__cdecl FUN_005307c3(int param_1)
{
  return gs_lair_names_0077c020[param_1];
}

// FUNCTION: SHANDALAR 0x004f2165
undefined4 FUN_004f2165(void)
{
  int i;

  for (i = 0; i < 0xf; i = i + 1)
  {
    if (FUN_00522508(3) == 0)
    {
      g_castle_dungeon_slots[i].rules_bitmap =
          g_castle_dungeon_slots[i].rules_bitmap & 0xfffffdff;
    }
  }
  return 0;
}

// FUNCTION: SHANDALAR 0x004bb400
void FUN_004bb400(char param_1, char param_2)
{
  int local_8;

  local_8 = 0;
  while (true)
  {
    if (999 < local_8)
    {
      return;
    }
    if (g_duel_victory_log[local_8] == '\0')
      break;
    local_8 = local_8 + 1;
  }
  g_duel_victory_log[local_8] = param_2 * '\x10' + param_1;
  return;
}

// FUNCTION: SHANDALAR 0x00430f69
void FUN_00430f69(char *param_1, int param_2, int param_3, int param_4)
{
  param_2 = (param_2 * global_screen_width) / 0x140;
  param_3 = (global_screen_height * param_3) / 0xf0;
  DrawCenteredTextLineClamped(param_1, param_2, param_3, param_4);
}

// FUNCTION: SHANDALAR 0x005616aa
void FUN_005616aa(int param_1)
{
  g_advfac64_load_state = -1;
  EnsureAdvfac64Loaded(param_1);
}

// FUNCTION: SHANDALAR 0x00566238
undefined4 FUN_00566238(void)
{
  undefined4 uVar1;

  if (g_player_is_male == 0)
  {
    uVar1 = 1;
  }
  else if (g_player_is_male == 1)
  {
    uVar1 = 0;
  }
  else if (g_player_is_male == 2)
  {
    uVar1 = 1;
  }
  else if (g_player_is_male == 3)
  {
    uVar1 = 0;
  }
  else if (g_player_is_male == 4)
  {
    uVar1 = 1;
  }
  else if (g_player_is_male == 5)
  {
    uVar1 = 0;
  }
  else if (g_player_is_male == 6)
  {
    uVar1 = 1;
  }
  else if (g_player_is_male == 7)
  {
    uVar1 = 0;
  }
  else if (g_player_is_male == 8)
  {
    uVar1 = 1;
  }
  else if (g_player_is_male == 9)
  {
    uVar1 = 0;
  }
  else if (g_player_is_male == 10)
  {
    uVar1 = 0;
  }
  else if (g_player_is_male == 0xb)
  {
    uVar1 = 1;
  }
  else if (g_player_is_male == 0xc)
  {
    uVar1 = 0;
  }
  else if (g_player_is_male == 0xd)
  {
    uVar1 = 1;
  }
  else
  {
    uVar1 = 0;
  }
  return uVar1;
}

// FUNCTION: SHANDALAR 0x0042811a
void FUN_0042811a(int param_1, int param_2, char *param_3, int param_4)
{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  EncodedImage *encoded_image;
  char local_1fc[100];
  int local_198;
  EncodedImage *local_194[100];

  (void)param_4;

  if (DAT_008bd200 != 1)
  {
    FUN_0046ed33();
    strcpy(local_1fc, param_3);
    ReadSpriteEntryPointers(local_194, s_dungbutt_spr_00581a64);
    encoded_image = local_194[0];
    iVar1 = FUN_005501dc(0x10f);
    iVar1 = iVar1 / 2;
    iVar2 = FUN_005501dc(0xc5);
    iVar2 = iVar2 / 2;
    iVar3 = FUN_005501dc(0x34);
    iVar3 = iVar3 / 2;
    iVar4 = FUN_005501dc(0xdc);
    DrawEncodedImageResampled(PTR_DAT_005832b4, iVar4 / 2, iVar3, iVar2, iVar1, encoded_image);
    FUN_004f263b(param_1, 0x7a, 0x29, 0x4b, 0x70, 1, DAT_00581a74);
    FreeSpriteBlob(local_194[0]);
    PTR_DAT_005832b4->font_slot = 4;
    local_198 = MeasureTextLineWidth(local_1fc);
    iVar3 = 0;
    iVar1 = ScaleUiCoordinate(0x52);
    iVar2 = GetFontLineHeight(PTR_DAT_005832b4->font_slot);
    DrawCenteredTextLineClamped(local_1fc, global_screen_width / 2, (iVar1 - iVar2) + -2, iVar3);
    iVar1 = ScaleUiCoordinate(0x52);
    iVar2 = GetFontLineHeight(PTR_DAT_005832b4->font_slot);
    DrawCenteredTextLineClamped(local_1fc, global_screen_width / 2, (iVar1 - iVar2) + -3, param_2);
    FUN_0046ed03();
  }
  return;
}

// FUNCTION: SHANDALAR 0x00450925
void FUN_00450925(uint param_1, int param_2, int param_3, uint param_4, undefined4 param_5, char *param_6)
{
  BOOL BVar1;
  WPARAM WVar2;
  int *lParam;
  LPARAM lParam_00;
  int local_c;
  int local_8;

  (void)param_4;
  (void)param_5;
  (void)param_6;

  local_c = param_2;
  local_8 = param_3;
  BVar1 = IsWindowVisible((HWND)DAT_00896714);
  if (BVar1 != 0)
  {
    if ((param_2 == -1) || (param_3 == -1))
    {
      lParam_00 = 0;
      WVar2 = CardIDFromType(param_1);
      SendMessageA((HWND)DAT_00896714, 0x401, WVar2, lParam_00);
    }
    else
    {
      lParam = &local_c;
      WVar2 = CardIDFromType(param_1);
      SendMessageA((HWND)DAT_00896714, 0x401, WVar2, (LPARAM)lParam);
    }
  }
  return;
}

// FUNCTION: SHANDALAR 0x005584d5
void FUN_005584d5(uint param_1, undefined4 param_2, char *param_3, int param_4)
{
  if (DAT_008bd200 == 1)
  {
    return;
  }

  if (unk_00742fc4 != 0)
  {
    FUN_00450925(param_1, -1, -1, param_2, param_3, param_4);
  }
  else
  {
    FUN_0042811a(param_1, param_2, param_3, param_4);
  }
}

// FUNCTION: SHANDALAR 0x005307dd
void FUN_005307dd(int param_1, int param_2)
{
  uint uVar1;
  int iVar2;
  uint uVar3;
  char *pcVar4;
  int iVar5;
  int full_card;
  uint auStack_20[5];
  int local_c;
  int local_8;

  for (local_c = 0; local_c < param_2; local_c = local_c + 1)
  {
    do
    {
      do
      {
        uVar1 = FUN_00522508(g_card_count - 0x39);
        iVar2 = FUN_0056c5ea(uVar1);
      } while (iVar2 < 3);
      uVar3 = GetCardAvailabilityMask(uVar1);
    } while (uVar3 == 0);
    auStack_20[local_c] = uVar1;
    FUN_004f2407(uVar1, (int)(0x96 / (longlong)param_2) * local_c + 100,
                 (int)(10 / (longlong)param_2) * local_c + 100, 1, DAT_0058f04c);
  }
  do
  {
    local_8 = PickRandomCreatureTypeForWizardTier(0, param_1);
  } while (0x36 < local_8);
  if (param_1 == 0xd)
  {
    pcVar4 = BuildCreatureNameWithArticle(local_8);
    sprintf(g_ui_message_buffer, gs_monsterlair_0074cff0[0], pcVar4);
  }
  else if (param_1 == 0x10)
  {
    pcVar4 = BuildCreatureNameWithArticle(local_8);
    sprintf(g_ui_message_buffer, gs_monsterlair_0074cff0[1], pcVar4);
  }
  else if (param_1 == 0x12)
  {
    pcVar4 = BuildCreatureNameWithArticle(local_8);
    sprintf(g_ui_message_buffer, gs_monsterlair_0074cff0[2], pcVar4);
  }
  else
  {
    pcVar4 = BuildCreatureNameWithArticle(local_8);
    sprintf(g_ui_message_buffer, gs_monsterlair_0074cff0[3], pcVar4);
  }
  strcat(g_ui_message_buffer, gs_monsterlair_0074cff0[4]);
  iVar2 = RunTextMenuAt(g_ui_message_buffer, 0x5a, 100);
  if (iVar2 == 1)
  {
    uVar1 = FUN_0056c705(*(int *)(gs_creature_names_00591a08[local_8].metadata + 6));
    LoadCreatureDuelDeck(local_8, uVar1, 0, -1);
    DAT_008ce538 = FUN_0040dffd((int)gs_creature_names_00591a08[local_8].metadata[4]);
    DAT_00742fd0 = 0;
    DAT_007a7874 = 3;
    ANTE_CARD(0, 0) = -1;
    for (local_c = 0; local_c < param_2; local_c = local_c + 1)
    {
      g_duel_ante_card_ids[local_c] = auStack_20[local_c];
    }
    iVar2 = RunDuelEngine(uVar1, local_8);
    if (iVar2 != 0)
    {
      LoadPcxIntoPageOpaque(1, s_winbak01_pic_0058f050);
      PlayDuelMusic(1);
      StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, 0, 0x280, 0x1e0, PTR_DAT_005832b4, 0, 0, global_screen_width,
                              global_screen_height);
      for (local_c = 0; local_c < param_2; local_c = local_c + 1)
      {
        iVar2 = (int)(300 / (longlong)(param_2 + 1));
        iVar5 = FUN_0056c5ea(auStack_20[local_c]);
        strcpy(g_ui_message_buffer, gs_browse_0074da20[iVar5 + 0xc]);
        pcVar4 = g_ui_message_buffer;
        full_card = 1;
        iVar5 = FUN_00522508(10);
        FUN_004f2407(auStack_20[local_c], (iVar2 * local_c + 0x6f) - ((param_2 + -1) * iVar2) / 2,
                     iVar5 + 0x10, full_card, pcVar4);
        local_8 = FUN_0056bd9d(auStack_20[local_c]);
        if (local_8 != -1)
        {
          deck[local_8] = deck[local_8] | 0x4000;
        }
      }
      PTR_DAT_005832b4->font_slot = 4;
      RunTextMenuAtScaled(gs_monsterlair_0074cff0[5], 0x40, 0x96);
    }
    sound_stop(0x10);
  }
  return;
}

// FUNCTION: SHANDALAR 0x00534530
bool FUN_00534530(void)
{
  int iVar1;
  uint uVar2;
  bool bVar3;
  int local_54;
  int local_50;
  int local_44;
  int local_40;
  int local_3c;
  int local_38;
  uint local_34[8];
  int local_14;
  int local_10;
  int local_c;
  uint local_8;

  local_14 = 5;
  memset(local_34, 0xff, 0x20);
  do
  {
    local_50 = 0;
    iVar1 = FUN_00522508(10);
    switch (iVar1)
    {
    case 0:
      local_44 = 0;
      break;
    case 1:
      local_44 = 1;
      break;
    case 2:
      local_44 = 2;
      break;
    case 3:
      local_44 = 3;
      break;
    case 4:
      local_44 = 4;
      break;
    case 5:
      local_44 = 5;
      break;
    case 6:
      local_44 = 6;
      break;
    case 7:
      local_44 = 7;
      break;
    case 8:
      local_44 = 8;
      break;
    case 9:
      local_44 = 2;
    }
  switchD_005345f6_default:
    do
    {
      do
      {
        uVar2 = FUN_00522508(g_card_count - 0x39);
        if ((global_cards_data[uVar2].type & 2) == 0)
        {
          if (local_44 != 2)
            goto switchD_005345f6_default;
        }
      } while ((global_cards_data[uVar2].expansion & 0xc1) == 0);
      bVar3 = false;
      switch (local_44)
      {
      case 0:
        if ((0 < global_cards_data[uVar2].power) && (global_cards_data[uVar2].power < 0x4000))
        {
          bVar3 = true;
        }
        break;
      case 1:
        if ((0 < global_cards_data[uVar2].toughness) &&
            (global_cards_data[uVar2].toughness < 0x4000))
        {
          bVar3 = true;
        }
        break;
      case 2:
        if (('\0' < (char)global_cards_data[uVar2].cc[0]) &&
            (-1 < (char)global_cards_data[uVar2].cc[1]))
        {
          bVar3 = true;
        }
        break;
      case 3:
        bVar3 = (global_cards_data[uVar2].static_ability & 0x20) != 0;
        break;
      case 4:
        bVar3 = (global_cards_data[uVar2].static_ability & 0x1f) != 0;
        break;
      case 5:
        bVar3 = (global_cards_data[uVar2].static_ability & 0x200) != 0;
        break;
      case 6:
        bVar3 = (global_cards_data[uVar2].static_ability & 0x40) != 0;
        break;
      case 7:
        bVar3 = (global_cards_data[uVar2].static_ability & 0x100) != 0;
        break;
      case 8:
        bVar3 = (global_cards_data[uVar2].static_ability & 0x80) != 0;
      }
      local_50 = local_50 + 1;
      if (bVar3)
      {
        iVar1 = FUN_00522508(4);
        if (g_shandalar_difficulty <= iVar1)
        {
          local_38 = 0;
          goto LAB_00534aba;
        }
        local_10 = -1;
        switch (local_44)
        {
        case 0:
          sprintf(g_ui_message_buffer, gs_riddle_0077cf20[0], uVar2 * 0x48 + 0x594209);
          local_10 = (int)global_cards_data[uVar2].power;
          break;
        case 1:
          sprintf(g_ui_message_buffer, gs_riddle_0077cf20[1], uVar2 * 0x48 + 0x594209);
          local_10 = (int)global_cards_data[uVar2].toughness;
          break;
        case 2:
          sprintf(g_ui_message_buffer, gs_riddle_0077cf20[2], uVar2 * 0x48 + 0x594209);
          local_10 = (int)(char)global_cards_data[uVar2].cc[0] +
                     (int)(char)global_cards_data[uVar2].cc[1];
          break;
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
          sprintf(g_ui_message_buffer, gs_riddle_0077cf20[3], uVar2 * 0x48 + 0x594209);
          strcat(g_ui_message_buffer, gs_riddle_0077cf20[4]);
        }
        if (local_10 == -1)
        {
          local_3c = RunTextMenuAt(g_ui_message_buffer, 0x50, 100);
          local_c = local_3c;
          if ((global_cards_data[uVar2].static_ability & 1 << ((byte)local_3c & 0x1f)) == 0)
          {
            local_c = 99;
          }
        }
        else
        {
          strcat(g_ui_message_buffer, s___0_1_2_3_4_5_6_7_8__0058f180);
          local_c = ClampIntToRange(local_10, 0, 8);
          local_3c = RunTextMenuAt(g_ui_message_buffer, 0x50, 100);
        }
        EnsureAdvfac64Loaded(1);
        if (local_3c == local_c)
        {
          FUN_005584d5(uVar2, 0xce, gs_riddle_0077cf20[5], 1);
        }
        else
        {
          FUN_005584d5(uVar2, 0xbc, gs_riddle_0077cf20[6], 1);
        }
        WaitForInputEventUnlessBlocked();
        bVar3 = local_3c != local_c;
        goto LAB_00535086;
      }
    } while (local_50 < 4);
  } while (true);
LAB_00534aba:
  if (local_14 <= local_38)
  {
    local_c = FUN_00522508(local_14);
    local_34[local_c] = uVar2;
    switch (local_44)
    {
    case 0:
      sprintf(g_ui_message_buffer, gs_riddle_0077cf20[7], (int)global_cards_data[uVar2].power);
      break;
    case 1:
      sprintf(g_ui_message_buffer, gs_riddle_0077cf20[8], (int)global_cards_data[uVar2].toughness);
      break;
    case 2:
      FUN_0040dffd((int)global_cards_data[uVar2].color);
      FUN_00564e70(g_ui_message_buffer, 0x1000, gs_riddle_0077cf20[9]);
      break;
    case 3:
      strcpy(g_ui_message_buffer, gs_riddle_0077cf20[10]);
      break;
    case 4:
      if ((global_cards_data[uVar2].static_ability & 1) != 0)
      {
        local_54 = 1;
      }
      if ((global_cards_data[uVar2].static_ability & 8) != 0)
      {
        local_54 = 4;
      }
      if ((global_cards_data[uVar2].static_ability & 2) != 0)
      {
        local_54 = 2;
      }
      if ((global_cards_data[uVar2].static_ability & 4) != 0)
      {
        local_54 = 3;
      }
      if ((global_cards_data[uVar2].static_ability & 0x10) != 0)
      {
        local_54 = 5;
      }
      sprintf(g_ui_message_buffer, gs_riddle_0077cf20[0xb], gs_landwalks_007806f0 + local_54 * 0x32);
      break;
    case 5:
      strcpy(g_ui_message_buffer, gs_riddle_0077cf20[0xc]);
      break;
    case 6:
      strcpy(g_ui_message_buffer, gs_riddle_0077cf20[0xd]);
      break;
    case 7:
      strcpy(g_ui_message_buffer, gs_riddle_0077cf20[0xe]);
      break;
    case 8:
      strcpy(g_ui_message_buffer, gs_riddle_0077cf20[0xf]);
    }
    for (local_38 = 0; local_38 < local_14; local_38 = local_38 + 1)
    {
      strcat(g_ui_message_buffer, DAT_0058f1a0);
      strcat(g_ui_message_buffer, global_cards_data[local_34[local_38]].name);
      strcat(g_ui_message_buffer, DAT_0058f1a4);
    }
    iVar1 = RunTextMenuAt(g_ui_message_buffer, 0x50, 100);
    EnsureAdvfac64Loaded(1);
    if (iVar1 == local_c)
    {
      FUN_005584d5(local_34[local_c], 0xce, gs_riddle_0077cf20[5], 1);
    }
    else
    {
      FUN_005584d5(local_34[local_c], 0xbc, gs_riddle_0077cf20[0x10], 1);
    }
    WaitForInputEventUnlessBlocked();
    bVar3 = iVar1 != local_c;
  LAB_00535086:
    return !bVar3;
  }
LAB_00534ac6:
  do
  {
    do
    {
      local_8 = FUN_00522508(g_card_count - 0x39);
      if ((global_cards_data[local_8].type & 2) == 0)
      {
        if (local_44 != 2)
          goto LAB_00534ac6;
      }
    } while ((global_cards_data[local_8].expansion & 0xc1) == 0);
    bVar3 = false;
    switch (local_44)
    {
    case 0:
      bVar3 = global_cards_data[local_8].power != global_cards_data[uVar2].power;
      break;
    case 1:
      bVar3 = global_cards_data[local_8].toughness != global_cards_data[uVar2].toughness;
      break;
    case 2:
      if ((global_cards_data[local_8].color == global_cards_data[uVar2].color) &&
          ((global_cards_data[local_8].cc[0] != global_cards_data[uVar2].cc[0] ||
            (global_cards_data[local_8].color != global_cards_data[uVar2].color))))
      {
        bVar3 = true;
      }
      break;
    case 3:
      bVar3 = (global_cards_data[local_8].static_ability & 0x20) == 0;
      break;
    case 4:
      bVar3 = (global_cards_data[local_8].static_ability & global_cards_data[uVar2].static_ability &
               0x1fU) == 0;
      break;
    case 5:
      bVar3 = (global_cards_data[local_8].static_ability & 0x200) == 0;
      break;
    case 6:
      bVar3 = (global_cards_data[local_8].static_ability & 0x40) == 0;
      break;
    case 7:
      bVar3 = (global_cards_data[local_8].static_ability & 0x100) == 0;
      break;
    case 8:
      bVar3 = (global_cards_data[local_8].static_ability & 0x80) == 0;
    }
    for (local_40 = 0; local_40 < local_38; local_40 = local_40 + 1)
    {
      if (local_34[local_40] == local_8)
      {
        bVar3 = false;
      }
    }
  } while (!bVar3);
  local_34[local_38] = local_8;
  local_38 = local_38 + 1;
  goto LAB_00534aba;
}

// FUNCTION: SHANDALAR 0x0052fae0
void FUN_0052fae0(int param_1)
{
  bool bVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  undefined4 uVar6;
  undefined3 extraout_var;
  char *pcVar7;
  size_t sVar8;
  undefined3 extraout_var_00;
  char *_Format;
  uint local_2c;
  int local_24;
  uint local_20;
  uint auStack_1c[4];
  int local_c;
  int local_8;

  FUN_004ed7c8(PTR_DAT_0058edb8[param_1]);
  DrawVisitScreenOverlays(PTR_DAT_005832b4);
  for (local_20 = 1; (int)local_20 < 4; local_20 = local_20 + 1)
  {
    do
    {
      iVar2 = FUN_00522508(3);
      if (iVar2 == 0)
      {
        local_2c = FUN_0056bcf7(0x40, 1);
      }
      else
      {
        local_2c = FUN_0056bcf7(0x3f, 1 << ((byte)param_1 & 0x1f));
      }
      iVar2 = FUN_0056c5ea(local_2c);
    } while ((((iVar2 != local_20) || (iVar2 = FUN_004bb1cf(local_2c), iVar2 < 0)) ||
              ((global_cards_data[local_2c].extra_ability & 0x900) != 0)) ||
             (uVar3 = GetCardAvailabilityMask(local_2c), uVar3 == 0));
    if ((local_20 == 1) && (iVar2 = FUN_00522508(2), iVar2 != 0))
    {
      local_2c = param_1 - 1;
    }
    auStack_1c[local_20] = local_2c;
  }
  do
  {
    do
    {
      iVar2 = FUN_00522508(500);
    } while (deck[iVar2] == -1);
    iVar4 = FUN_0056c5ea(deck[iVar2] & 0xfff);
  } while ((iVar4 < 2) || (uVar3 = GetCardAvailabilityMask(iVar2), uVar3 == 0));
  EnsureAdvfac64Loaded(1);
  do
  {
    if ((int)(CONCAT44(g_monster_timer >> 0x1f, g_monster_timer >> 2) % 3) != 0)
    {
      iVar2 = FUN_00522508(2);
      if (iVar2 == 0)
      {
        do
        {
          do
          {
            local_20 = FUN_00522508(g_card_count - 0x39);
            iVar2 = FUN_0056c0e5((int)global_cards_data[local_20].color, 1 << ((byte)param_1 & 0x1f),
                                 1);
          } while (iVar2 == 0);
        } while (((global_cards_data[local_20].extra_ability & 0x100) != 0) ||
                 (uVar3 = GetCardAvailabilityMask(local_20), uVar3 == 0));
      }
      else
      {
        local_20 = param_1 - 1;
      }
      goto switchD_00530447_default;
    }
    iVar4 = FUN_00522508(0xe);
    iVar5 = iVar4 + 5;
    FUN_004290e2(5, iVar5);
    FUN_00522508(2);
  } while ((g_next_duel_card_id != -1) && ((iVar5 == 0xb || (iVar5 == 0x11))));
  uVar6 = FUN_005307c3(iVar5);
  sprintf(g_ui_message_buffer, gs_lair_0077e180[0], uVar6);
  ClearInputAndWaitForMouseRelease();
  RunTextMenuAt(g_ui_message_buffer, 0x5a, 100);
  local_20 = 0xffffffff;
  iVar5 = FUN_00522508(4);
  if (iVar5 == 0)
  {
    local_20 = FUN_00530770();
  }
  switch (iVar4)
  {
  case 0:
    local_c = PickWisemanCastleDungeonSlotForHint();
    if (local_c != -1)
    {
      RevealRandomCastleDungeonClue(local_c);
      break;
    }
  case 1:
    bVar1 = FUN_00534530();
    if (CONCAT31(extraout_var, bVar1) != 0)
    {
      local_20 = auStack_1c[3];
    }
    break;
  case 2:
    iVar2 = FUN_00522508(2);
    if ((iVar2 == 0) && (0x7f < g_siege_timer))
    {
      strcpy(g_ui_message_buffer, gs_lair_0077e180[2]);
      Gold = Gold / 2;
      local_20 = FUN_00530770();
    }
    else
    {
      PlaySoundEffectOnChannel(s_x_sound_treasure_wav_0058ee48, 0xf, 100, 100, 0);
      strcpy(g_ui_message_buffer, gs_lair_0077e180[1]);
      Gold = Gold + 500;
      local_20 = 0xffffffff;
    }
    RunTextMenuAt(g_ui_message_buffer, 0x5a, 100);
    break;
  case 3:
    do
    {
      iVar2 = BuyAnyCardFromTown(-1, -1);
    } while (iVar2 != 0);
    break;
  case 4:
    PlaySoundEffectOnChannel(s_x_Duelsounds_aswanjag_wav_0058ee60, 0x97, 100, 100, 0);
    if (g_siege_timer < 0x100)
    {
      FUN_005307dd(8, 2);
    }
    else
    {
      FUN_005307dd(0xd, 3);
    }
    break;
  case 5:
    PlaySoundEffectOnChannel(s_x_Duelsounds_aswanjag_wav_0058ee7c, 0x97, 100, 100, 0);
    FUN_005307dd(0x12, 4);
    break;
  case 6:
    g_next_duel_card_id = FUN_0056c705(0x1b4);
    PlaySoundEffectOnChannel(s_x_Duelsounds_aswanjag_wav_0058ee98, 0x97, 100, 100, 0);
    FUN_005307dd(0xd, 2);
    break;
  case 7:
    PlaySoundEffectOnChannel(s_x_sound_treasure_wav_0058eeb4, 0xf, 100, 100, 0);
    do
    {
      strcpy(g_ui_message_buffer, gs_lair_0077e180[3]);
      strcat(g_ui_message_buffer, gs_lair_0077e180[4]);
      for (local_24 = 0; local_24 < 5; local_24 = local_24 + 1)
      {
        iVar2 = DAT_0078990c[local_24 + 1];
        pcVar7 = gs_amuletnames_0077d090 + (local_24 * 5 + 5) * 5;
        _Format = gs_lair_0077e180[5];
        sVar8 = strlen(g_ui_message_buffer);
        sprintf(g_ui_message_buffer + sVar8, _Format, pcVar7, iVar2);
      }
      iVar2 = RunTextMenuAt(g_ui_message_buffer, 0x5a, 100);
      if ((iVar2 < 1) || (Gold < 200))
        break;
      Gold = Gold + -200;
      DAT_0078990c[iVar2] = DAT_0078990c[iVar2] + 1;
    } while (199 < Gold);
    break;
  case 8:
    local_8 = FUN_00522508(gs_creature_name_count_00593934 - 3);
    local_8 = local_8 + 1;
    gs_creature_names_00591a08[gs_creature_name_count_00593934 - 1].metadata[1] = '\x10';
    *(undefined4 *)(gs_creature_names_00591a08[gs_creature_name_count_00593934 - 1].metadata + 6) =
        *(undefined4 *)(gs_creature_names_00591a08[local_8].metadata + 6);
    strcpy(gs_creature_names_00591a08[gs_creature_name_count_00593934 - 1].name,
           gs_creature_names_00591a08[local_8].name);
    strcpy(gs_creature_names_00591a08[gs_creature_name_count_00593934 - 1].article,
           gs_creature_names_00591a08[local_8].article);
    strcpy(gs_creature_names_00591a08[gs_creature_name_count_00593934 - 1].plural,
           gs_creature_names_00591a08[local_8].plural);
    gs_creature_names_00591a08[gs_creature_name_count_00593934 - 1].metadata[0] =
        gs_creature_names_00591a08[local_8].metadata[0];
    PlaySoundEffectOnChannel(s_x_Duelsounds_aswanjag_wav_0058eecc, 0x97, 100, 100, 0);
    FUN_005307dd(0x10, 3);
    break;
  case 9:
    bVar1 = FUN_00534530();
    if (CONCAT31(extraout_var_00, bVar1) != 0)
    {
      local_20 = FUN_00530770();
    }
    break;
  case 10:
    iVar2 = FUN_00522508(2);
    if ((iVar2 == 0) && (0x7f < g_siege_timer))
    {
      strcpy(g_ui_message_buffer, gs_lair_0077e180[7]);
      for (local_24 = 0; local_24 < 5; local_24 = local_24 + 1)
      {
        DAT_0078990c[local_24 + 1] = (DAT_0078990c[local_24 + 1] + 1) / 2;
      }
      local_20 = FUN_00530770();
    }
    else
    {
      PlaySoundEffectOnChannel(s_x_sound_treasure_wav_0058eee8, 0xf, 100, 100, 0);
      strcpy(g_ui_message_buffer, gs_lair_0077e180[6]);
      for (local_24 = 0; local_24 < 5; local_24 = local_24 + 1)
      {
        DAT_0078990c[local_24 + 1] = DAT_0078990c[local_24 + 1] + 1;
      }
      local_20 = 0xffffffff;
    }
    RunTextMenuAt(g_ui_message_buffer, 0x5a, 100);
    break;
  case 0xb:
    local_2c = 0;
    do
    {
      iVar2 = FUN_00522508(5);
      if (DAT_0078990c[iVar2 + 1] != 0)
        break;
      local_2c = local_2c + 1;
    } while ((int)local_2c < 99);
    if (DAT_0078990c[iVar2 + 1] != 0)
    {
      FUN_00564e70(g_ui_message_buffer, 0x1000, gs_lair_0077e180[8]);
      strcat(g_ui_message_buffer, gs_lair_0077e180[0xe]);
      iVar4 = RunTextMenuAt(g_ui_message_buffer, 0x5a, 100);
      if (iVar4 == 1)
      {
        DAT_0078990c[iVar2 + 1] = DAT_0078990c[iVar2 + 1] + -1;
        WisemanChooseLairCreatureAndSetupDuel((byte)param_1);
      }
    }
    break;
  case 0xc:
    sprintf(g_ui_message_buffer, gs_lair_0077e180[0xf], (deck[iVar2] & 0xfffU) * 0x48 + 0x594209);
    strcat(g_ui_message_buffer, gs_lair_0077e180[0x10]);
    iVar4 = RunTextMenuAt(g_ui_message_buffer, 100, 100);
    if (iVar4 == 1)
    {
      RemoveDeckSlot(iVar2);
      g_next_duel_card_id = 5;
    }
    break;
  case 0xd:
    do
    {
      iVar2 = BuyAnyCardFromTown(0, -1);
    } while (iVar2 != 0);
    RecountDeckCardTotals();
  }
switchD_00530447_default:
  if (local_20 != 0xffffffff)
  {
    if ((global_cards_data[local_20].type & 2) == 0)
    {
      if ((global_cards_data[local_20].type & 0x40) == 0)
      {
        if ((global_cards_data[local_20].type & 4) == 0)
        {
          if ((global_cards_data[local_20].type & 0x10) == 0)
          {
            if ((global_cards_data[local_20].type & 0x20) == 0)
            {
              if ((global_cards_data[local_20].type & 8) == 0)
              {
                if ((global_cards_data[local_20].type & 1) == 0)
                {
                  PlaySoundEffectOnChannel(s_x_sound_treasure_wav_0058f034, 0xf, 100, 100, 0);
                }
                else
                {
                  switch (global_cards_data[local_20].color)
                  {
                  case '\x02':
                    PlaySoundEffectOnChannel(s_x_DuelSounds_black_wav_0058efa4, 0xf, 100, 100, 0);
                    break;
                  default:
                    PlaySoundEffectOnChannel(s_x_DuelSounds_grey_wav_0058f01c, 0xf, 100, 100, 0);
                    break;
                  case '\x04':
                    PlaySoundEffectOnChannel(s_x_DuelSounds_blue_wav_0058efbc, 0xf, 100, 100, 0);
                    break;
                  case '\b':
                    PlaySoundEffectOnChannel(s_x_DuelSounds_green_wav_0058efd4, 0xf, 100, 100, 0);
                    break;
                  case '\x10':
                    PlaySoundEffectOnChannel(s_x_DuelSounds_red_wav_0058efec, 0xf, 100, 100, 0);
                    break;
                  case ' ':
                    PlaySoundEffectOnChannel(s_x_DuelSounds_white_wav_0058f004, 0xf, 100, 100, 0);
                  }
                }
              }
              else
              {
                PlaySoundEffectOnChannel(s_x_DuelSounds_sorcery_wav_0058ef88, 0xf, 100, 100, 0);
              }
            }
            else
            {
              PlaySoundEffectOnChannel(s_x_DuelSounds_interupt_wav_0058ef6c, 0xf, 100, 100, 0);
            }
          }
          else
          {
            PlaySoundEffectOnChannel(s_x_DuelSounds_instant_wav_0058ef50, 0xf, 100, 100, 0);
          }
        }
        else
        {
          PlaySoundEffectOnChannel(s_x_DuelSounds_enchant_wav_0058ef34, 0xf, 100, 100, 0);
        }
      }
      else
      {
        PlaySoundEffectOnChannel(s_x_DuelSounds_artifact_wav_0058ef18, 0xf, 100, 100, 0);
      }
    }
    else
    {
      PlaySoundEffectOnChannel(s_x_DuelSounds_summon_wav_0058ef00, 0xf, 100, 100, 0);
    }
    ClearInputAndWaitForMouseRelease();
    FUN_005584d5(local_20, 0xd0, gs_lair_0077e180[0x12], 1);
    iVar2 = FUN_0056bd9d(local_20);
    if (iVar2 != -1)
    {
      deck[iVar2] = deck[iVar2] | 0x4000;
    }
    WaitForInputEventUnlessBlocked();
  }
  return;
}

// FUNCTION: SHANDALAR 0x0054c6ba
undefined4 FUN_0054c6ba(void)
{
  int iVar1;
  size_t sVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  char *pcVar6;
  char *pcVar7;
  EncodedImage *pEVar8;

  strcpy(g_ui_message_buffer, DAT_00590b54);
  if (g_lair_or_monster_slots[7].entry_type != -1)
  {
    iVar1 = FindNearestTownIndex((int)(g_lair_or_monster_slots[7].world_x +
                                       (g_lair_or_monster_slots[7].world_x >> 0x1f & 0x1fU)) >>
                                     5,
                                 (int)(g_lair_or_monster_slots[7].world_y +
                                       (g_lair_or_monster_slots[7].world_y >> 0x1f & 0x1fU)) >>
                                     5);
    BuildTownDisplayName(iVar1);
    FUN_00561441(g_lair_or_monster_slots[7].entry_type);
    FUN_00564e70(g_ui_message_buffer, 0x1000, gs_queststatus_0077e0a0[0]);
  }
  if ((g_next_duel_life_delta != 0) || (g_next_duel_card_id != -1))
  {
    strcat(g_ui_message_buffer, gs_queststatus_0077e0a0[1]);
    if (g_next_duel_life_delta != 0)
    {
      if (g_next_duel_life_delta < 0)
      {
        iVar1 = abs(g_next_duel_life_delta);
        pcVar6 = gs_queststatus_0077e0a0[3];
        sVar2 = strlen(g_ui_message_buffer);
        sprintf(g_ui_message_buffer + sVar2, pcVar6, iVar1);
      }
      else
      {
        pcVar6 = gs_queststatus_0077e0a0[2];
        iVar1 = g_next_duel_life_delta;
        sVar2 = strlen(g_ui_message_buffer);
        sprintf(g_ui_message_buffer + sVar2, pcVar6, iVar1);
      }
      pEVar8 = g_worlds_extra_sprite_entries[3];
      iVar1 = ScaleUiCoordinate(0x25);
      iVar3 = ScaleUiCoordinate(0x1c);
      iVar4 = ScaleUiCoordinate(200);
      iVar5 = ScaleUiCoordinate(0x254);
      DrawEncodedImageResampled(PTR_DAT_005832b4, iVar5, iVar4, iVar3, iVar1, pEVar8);
    }
    if (g_next_duel_card_id == 0)
    {
      strcat(g_ui_message_buffer, gs_queststatus_0077e0a0[4]);
      pEVar8 = g_worlds_extra_sprite_entries[1];
      iVar1 = ScaleUiCoordinate(0x25);
      iVar3 = ScaleUiCoordinate(0x1c);
      iVar4 = ScaleUiCoordinate(200);
      iVar5 = ScaleUiCoordinate(0x254);
      DrawEncodedImageResampled(PTR_DAT_005832b4, iVar5, iVar4, iVar3, iVar1, pEVar8);
    }
    else if ((g_next_duel_card_id < 1) || (5 < g_next_duel_card_id))
    {
      if (5 < g_next_duel_card_id)
      {
        strcat(g_ui_message_buffer, global_cards_data[g_next_duel_card_id].name);
        strcat(g_ui_message_buffer, DAT_00590b58);
        pEVar8 = g_worlds_extra_sprite_entries[0];
        iVar1 = ScaleUiCoordinate(0x25);
        iVar3 = ScaleUiCoordinate(0x1c);
        iVar4 = ScaleUiCoordinate(0x90);
        iVar5 = ScaleUiCoordinate(0x254);
        DrawEncodedImageResampled(PTR_DAT_005832b4, iVar5, iVar4, iVar3, iVar1, pEVar8);
      }
    }
    else
    {
      pcVar6 = gs_queststatus_0077e0a0[2];
      iVar1 = g_next_duel_card_id;
      sVar2 = strlen(g_ui_message_buffer);
      sprintf(g_ui_message_buffer + sVar2, pcVar6, iVar1);
      pEVar8 = g_worlds_extra_sprite_entries[2];
      iVar1 = ScaleUiCoordinate(0x25);
      iVar3 = ScaleUiCoordinate(0x1c);
      iVar4 = ScaleUiCoordinate(200);
      iVar5 = ScaleUiCoordinate(0x254);
      DrawEncodedImageResampled(PTR_DAT_005832b4, iVar5, iVar4, iVar3, iVar1, pEVar8);
    }
    strcat(g_ui_message_buffer, DAT_00590b5c);
  }
  if ((DAT_007894f4 == 0) && (g_current_quest_destination != -1))
  {
    if (g_current_quest_type == 0)
    {
      strcat(g_ui_message_buffer, gs_queststatus_0077e0a0[10]);
    }
    else if (g_town_slots[g_current_quest_destination].location_type == 1)
    {
      strcat(g_ui_message_buffer, gs_queststatus_0077e0a0[g_current_quest_color + 4]);
      strcat(g_ui_message_buffer, DAT_00590b60);
    }
    else
    {
      pcVar6 = FUN_004f2e17(g_current_quest_destination);
      strcat(g_ui_message_buffer, pcVar6);
      strcat(g_ui_message_buffer, DAT_00590b64);
    }
    if ((((g_current_quest_type == 0) || (g_current_quest_type == 2)) ||
         ((g_current_quest_type == 1 &&
           (iVar1 = FindDeckSlotForQuestColorAndType((uchar)g_current_quest_color,
                                                     (uchar)(1 << ((byte)g_current_quest_destination & 3))),
            iVar1 != 0)))) ||
        (g_current_quest_type < -100))
    {
      iVar1 = GetRelativeWorldQuadrant(g_town_slots[g_current_quest_destination].world_x,
                                       g_town_slots[g_current_quest_destination].world_y);
      if (iVar1 != g_current_quest_data)
      {
        g_current_quest_data = -1;
      }
      switch (g_current_quest_data)
      {
      case 0:
        pcVar7 = BuildTownDisplayName(g_current_quest_destination);
        pcVar6 = gs_queststatus_0077e0a0[0xe];
        sVar2 = strlen(g_ui_message_buffer);
        sprintf(g_ui_message_buffer + sVar2, pcVar6, pcVar7);
        break;
      case 1:
        pcVar7 = BuildTownDisplayName(g_current_quest_destination);
        pcVar6 = gs_queststatus_0077e0a0[0xf];
        sVar2 = strlen(g_ui_message_buffer);
        sprintf(g_ui_message_buffer + sVar2, pcVar6, pcVar7);
        break;
      case 2:
        pcVar7 = BuildTownDisplayName(g_current_quest_destination);
        pcVar6 = gs_queststatus_0077e0a0[0x10];
        sVar2 = strlen(g_ui_message_buffer);
        sprintf(g_ui_message_buffer + sVar2, pcVar6, pcVar7);
        break;
      case 3:
        pcVar7 = BuildTownDisplayName(g_current_quest_destination);
        pcVar6 = gs_queststatus_0077e0a0[0x11];
        sVar2 = strlen(g_ui_message_buffer);
        sprintf(g_ui_message_buffer + sVar2, pcVar6, pcVar7);
        break;
      case -1:
        if (g_current_quest_type < 0)
        {
          pcVar7 = BuildTownDisplayName(g_current_quest_destination);
          pcVar6 = gs_queststatus_0077e0a0[0xb];
          sVar2 = strlen(g_ui_message_buffer);
          sprintf(g_ui_message_buffer + sVar2, pcVar6, pcVar7);
        }
        else if ((g_current_quest_type == 0) || (g_current_quest_type == 2))
        {
          pcVar7 = BuildTownDisplayName(g_current_quest_destination);
          pcVar6 = gs_queststatus_0077e0a0[0xc];
          sVar2 = strlen(g_ui_message_buffer);
          sprintf(g_ui_message_buffer + sVar2, pcVar6, pcVar7);
        }
        else
        {
          pcVar7 = BuildTownDisplayName(g_current_quest_destination);
          pcVar6 = gs_queststatus_0077e0a0[0xd];
          sVar2 = strlen(g_ui_message_buffer);
          sprintf(g_ui_message_buffer + sVar2, pcVar6, pcVar7);
        }
      }
      strcat(g_ui_message_buffer, DAT_00590b68);
    }
    else
    {
      if (g_current_quest_type == 1)
      {
        GetQuestCardClassName(1 << ((byte)g_current_quest_destination & 3));
        FUN_00564e70(g_ui_message_buffer, 0x1000, gs_queststatus_0077e0a0[0x12]);
      }
      if (g_current_quest_type < 0)
      {
        pcVar7 = BuildCreatureNameWithArticle(-g_current_quest_type);
        pcVar6 = gs_queststatus_0077e0a0[0x18];
        sVar2 = strlen(g_ui_message_buffer);
        sprintf(g_ui_message_buffer + sVar2, pcVar6, pcVar7);
      }
    }
  }
  return 0;
}

// FUNCTION: SHANDALAR 0x004f2ec0
int FUN_004f2ec0(int slot_index, int monster_color)
{
  char cVar1;
  bool bVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  char *pcVar10;
  char *pcVar11;
  size_t sVar12;
  undefined3 extraout_var;
  EncodedImage *pEVar13;
  int local_908[7];
  int local_8ec;
  int local_8e8;
  int local_8e4;
  int local_8e0;
  int local_8dc;
  int local_8d8;
  int local_8d4;
  int local_8d0;
  int local_8cc;
  int local_8c8;
  char *local_8c4;
  int local_8c0;
  int local_8bc;
  int local_8b8;
  int local_8b4;
  EncodedImage *local_8b0;
  EncodedImage *local_8ac;
  char *local_8a8[5];
  EncodedImage *local_894;
  int local_890;
  EncodedImage *local_88c;
  EncodedImage *local_888;
  EncodedImage *local_884;
  EncodedImage *local_880;
  uint local_87c;
  int local_878;
  uint local_874;
  uint local_870;
  void *local_86c;
  uint auStack_868[10];
  int local_840;
  int local_83c;
  int local_838;
  uint local_834;
  uint local_830;
  int local_82c;
  uint local_828;
  int local_824;
  int local_820;
  uint local_81c;
  uint local_818;
  uint local_814;
  int local_808;
  int local_804;
  int local_800;
  int local_7fc;
  int local_7f8;
  int local_7f4;
  int local_7f0;
  undefined4 local_7ec;
  int local_7e8;
  uint local_7e4[500];
  uint local_14;
  uint local_10;
  int local_c;
  int local_8;

  local_86c = g_town_dialog_callback;
  FUN_00562893();
  if (g_lair_or_monster_slots[slot_index].entry_type == 0)
  {
    FUN_0052fae0(g_lair_or_monster_slots[slot_index].color);
    g_lairs_explored = g_lairs_explored + 1;
    EnsureAdvfac64Loaded(0);
  }
  else
  {
    FUN_004be570(s_dbox2_spr_0058afb8, 0xd5, 0xd2);
    local_14 = g_lair_or_monster_slots[slot_index].entry_type;
    DAT_008ce538 = monster_color;
    DAT_007a7874 = (int)gs_creature_names_00591a08[local_14].metadata[2];
    local_838 = DAT_007a7874 + (int)gs_creature_names_00591a08[local_14].metadata[1] / 2;
    if ('\n' < gs_creature_names_00591a08[local_14].metadata[3])
    {
      local_838 = 0;
    }
    PlayDuelMusic(0);
    if (gs_creature_names_00591a08[local_14].metadata[3] == '\v')
    {
      local_874 = FUN_004bb458(g_world_magic_offer_slot_index);
    }
    else
    {
      local_874 = FUN_0056c705(*(int *)(gs_creature_names_00591a08[local_14].metadata + 6));
      if (local_874 == 0xffffffff)
      {
        local_874 = monster_color - 1;
      }
    }
    memset(global_ante_cards, 0xff, 0x80);
    LoadCreatureDuelDeck(local_14, 0xffffffff, 0, -1);
    if (gs_creature_names_00591a08[local_14].metadata[3] != '\v')
    {
      do
      {
        do
        {
          g_duel_ante_card_ids[0] = DrawRandomCardFromInitialLibrary(DAT_0057a750);
        } while (g_duel_ante_card_ids[0] < 5);
      } while ((((global_cards_data[g_duel_ante_card_ids[0]].extra_ability & 0x100) != 0) ||
                (uVar3 = GetCardAvailabilityMask(g_duel_ante_card_ids[0]), uVar3 == 0)) ||
               (global_cards_data[g_duel_ante_card_ids[0]].id ==
                *(int *)(gs_creature_names_00591a08[local_14].metadata + 6)));
    }
    if (gs_creature_names_00591a08[local_14].metadata[3] == '\f')
    {
      local_7f8 = 3;
    }
    else
    {
      local_7f8 = 1;
    }
    ExitIfNoUsableDeckCards();
    for (local_82c = 0; local_82c < local_7f8; local_82c = local_82c + 1)
    {
      do
      {
        do
        {
          local_830 = FUN_00522508(500);
        } while (deck[local_830] == -1);
      } while (((*(byte *)((int)deck + local_830 * 4 + 1) & 0x40) != 0) ||
               ((deck[local_830] & 0xfffU) < 5));
      *(uint *)(global_ante_cards[0] + local_82c * 4) = deck[local_830] & 0xfff;
    }
    local_7ec = 2;
    local_8a8[0] = s_prdblk_pic_0058afd0;
    local_8a8[1] = s_prdblu_pic_0058afe8;
    local_8a8[2] = s_prdgrn_pic_0058b000;
    local_8a8[3] = s_prdrd_pic_0058b018;
    local_8a8[4] = s_prdwt_pic_0058b030;
    AnimatePaletteToColor(0, DAT_00589dec);
    LoadPcxResource(1, 0, 0, local_8a8[monster_color], (g_graphics_bpp == 8) ? g_palette_data_words.entry_data : 1);
    StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, 0, 0x280, 0x1e0, PTR_DAT_005832b4, 0, 0, global_screen_width,
                            global_screen_height);
    FadeInPaletteFromGray(0, DAT_00589dec);
    ApplyCardArtPaletteToDc(global_main_hdc);
    LoadPcxIntoPage(1, s_prdfrma_pic_0058b03c);
    BeginSpriteEncodeSession();
    local_88c = EncodeSpriteFromPage(1, 1, 1, 0x68, 0x2c);
    local_8ac = EncodeSpriteFromPage(1, 1, 0x2e, 0x67, 0x31);
    local_884 = EncodeSpriteFromPage(1, 1, 0x60, 0x66, 0x2c);
    local_894 = EncodeSpriteFromPage(1, 1, 0x8d, 0x79, 0x2c);
    local_880 = EncodeSpriteFromPage(1, 1, 0xba, 0x91, 0x72);
    pEVar13 = local_88c;
    iVar4 = ScaleUiCoordinate((int)local_88c->height);
    iVar5 = ScaleUiCoordinate((int)local_88c->width);
    iVar6 = ScaleUiCoordinate(0x173);
    iVar7 = ScaleUiCoordinate(0x14);
    DrawEncodedImageResampled(PTR_DAT_005832b4, iVar7, iVar6, iVar5, iVar4, pEVar13);
    pEVar13 = local_8ac;
    iVar4 = ScaleUiCoordinate((int)local_8ac->height);
    iVar5 = ScaleUiCoordinate((int)local_8ac->width);
    iVar6 = ScaleUiCoordinate(0x16d);
    iVar7 = ScaleUiCoordinate((int)local_88c->width);
    iVar8 = ScaleUiCoordinate(0x14);
    iVar9 = ScaleUiCoordinate(8);
    DrawEncodedImageResampled(PTR_DAT_005832b4, (iVar7 + iVar8) - iVar9, iVar6, iVar5, iVar4, pEVar13);
    iVar4 = ScaleUiCoordinate((int)local_8ac->width);
    iVar5 = ScaleUiCoordinate((int)local_88c->width);
    iVar6 = ScaleUiCoordinate(8);
    local_890 = ScaleUiCoordinate(0x14);
    local_890 = ((iVar4 + iVar5) - iVar6) / 2 + local_890;
    iVar4 = ScaleUiCoordinate((int)local_894->width);
    iVar5 = ScaleUiCoordinate((int)local_884->width);
    iVar6 = ScaleUiCoordinate(8);
    local_890 = local_890 - ((iVar4 + iVar5) - iVar6) / 2;
    pEVar13 = local_894;
    iVar4 = ScaleUiCoordinate((int)local_894->height);
    iVar5 = ScaleUiCoordinate((int)local_894->width);
    iVar6 = ScaleUiCoordinate((int)local_88c->height);
    iVar7 = ScaleUiCoordinate(0x173);
    DrawEncodedImageResampled(PTR_DAT_005832b4, local_890, iVar6 + iVar7, iVar5, iVar4, pEVar13);
    pEVar13 = local_884;
    iVar4 = ScaleUiCoordinate((int)local_884->height);
    iVar5 = ScaleUiCoordinate((int)local_884->width);
    iVar6 = ScaleUiCoordinate((int)local_88c->height);
    iVar7 = ScaleUiCoordinate(0x173);
    iVar6 = iVar6 + iVar7;
    iVar7 = ScaleUiCoordinate((int)local_894->width);
    iVar7 = local_890 + iVar7;
    iVar8 = ScaleUiCoordinate(8);
    DrawEncodedImageResampled(PTR_DAT_005832b4, iVar7 - iVar8, iVar6, iVar5, iVar4, pEVar13);
    PTR_DAT_005832b4->font_slot = 4;
    DrawTextAt(PTR_DAT_005832b4, 0xd2, 0x5b, 0x188, DAT_0058b048);
    DrawTextAt(PTR_DAT_005832b4, 0xd2, 0xbd, 0x188, DAT_0058b04c);
    DrawTextAt(PTR_DAT_005832b4, 0xd2, 0x5b, 0x1b5, s__d__d_0058b050);
    CountDuelPoolEligibleTowns();
    DrawTextAt(PTR_DAT_005832b4, 0xd2, 0xc3, 0x1b5, DAT_0058b058);
    PTR_DAT_005832b4->font_slot = 2;
    pEVar13 = local_880;
    iVar4 = ScaleUiCoordinate((int)local_880->height);
    iVar5 = ScaleUiCoordinate((int)local_880->width);
    iVar6 = ScaleUiCoordinate(0x15e);
    iVar7 = ScaleUiCoordinate(0x1d4);
    DrawEncodedImageResampled(PTR_DAT_005832b4, iVar7, iVar6, iVar5, iVar4, pEVar13);
    FUN_0054c6ba();
    DrawTextAt(PTR_DAT_005832b4, 0xd2, 0x21f, 0x199, g_ui_message_buffer);
    FinalizeSpriteEncodeSession();
    FreeSpriteBlob(local_88c);
    iVar7 = 0;
    iVar6 = 0;
    iVar4 = ScaleUiCoordinate(10);
    iVar5 = ScaleUiCoordinate(0x140);
    DrawCreaturePortrait(local_14, iVar5, iVar4, iVar6, iVar7);
    LoadPcxIntoPage(1, s_prdfrmb_pic_0058b05c);
    PTR_DAT_005832dc->font_slot = 4;
    PTR_DAT_005832b4->font_slot = 4;
    BeginSpriteEncodeSession();
    local_888 = EncodeSpriteFromPage(1, 1, 0x17e, 0x98, 0x23);
    local_8b0 = EncodeSpriteFromPage(1, 1, 0x1a2, 0x80, 0x23);
    FinalizeSpriteEncodeSession();
    pEVar13 = local_888;
    iVar4 = ScaleUiCoordinate((int)local_888->height);
    iVar5 = ScaleUiCoordinate((int)local_888->width);
    iVar6 = ScaleUiCoordinate(5);
    iVar7 = ScaleUiCoordinate(0x220 - (int)local_888->width / 2);
    DrawEncodedImageResampled(PTR_DAT_005832b4, iVar7, iVar6, iVar5, iVar4, pEVar13);
    iVar4 = ScaleUiCoordinate((int)local_888->height / 2 + 5);
    iVar5 = ScaleUiCoordinate(0x220);
    DrawFormattedTextNoShadowCentered(PTR_DAT_005832b4, 0xe6, iVar5, iVar4);
    pEVar13 = local_8b0;
    iVar4 = ScaleUiCoordinate((int)local_8b0->height);
    iVar5 = ScaleUiCoordinate((int)local_8b0->width);
    iVar6 = ScaleUiCoordinate(5);
    iVar7 = ScaleUiCoordinate(100 - (int)local_8b0->width / 2);
    DrawEncodedImageResampled(PTR_DAT_005832b4, iVar7, iVar6, iVar5, iVar4, pEVar13);
    iVar4 = ScaleUiCoordinate((int)local_8b0->height / 2 + 5);
    iVar5 = ScaleUiCoordinate(100);
    DrawFormattedTextNoShadowCentered(PTR_DAT_005832b4, 0xe6, iVar5, iVar4);
    PTR_DAT_005832b4->font_slot = 1;
    FreeSpriteBlob(local_888);
    pcVar10 = FUN_00561441(local_14);
    sprintf(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[2], pcVar10);
    if ((DAT_008bd200 == 0) && (gs_creature_names_00591a08[local_14].metadata[3] != '\v'))
    {
      FUN_004f2407(g_duel_ante_card_ids[0], 0xe8, 0x18, 1, DAT_0058b068);
    }
    local_7f4 = ClampIntToRange(local_838 * 10, 10, local_838 * 0x32);
    if (gs_creature_names_00591a08[local_14].metadata[3] != '\v')
    {
      local_824 = 0;
      local_7f0 = 0;
      local_8bc = (int)(g_lair_or_monster_slots[slot_index].world_x +
                        (g_lair_or_monster_slots[slot_index].world_x >> 0x1f & 0x1fU)) >>
                  5;
      local_8c0 = (int)(g_lair_or_monster_slots[slot_index].world_y +
                        (g_lair_or_monster_slots[slot_index].world_y >> 0x1f & 0x1fU)) >>
                  5;
      local_8b4 = g_castle_dungeon_slots[monster_color + -1].world_x;
      local_8b8 = g_castle_dungeon_slots[monster_color + -1].world_y;
      iVar4 = abs(local_8bc - local_8b4);
      if ((iVar4 <= g_shandalar_difficulty / 2 + 2) &&
          (iVar4 = abs(local_8c0 - local_8b8), iVar4 <= g_shandalar_difficulty / 2 + 2))
      {
        local_7f0 = 1;
      }
      if (slot_index == 7)
      {
        local_7f0 = 2;
      }
    LAB_004f3a77:
      for (local_82c = 0; local_82c < local_7f8; local_82c = local_82c + 1)
      {
        if (DAT_008bd200 == 0)
        {
          FUN_004f2407(*(int *)(global_ante_cards[0] + local_82c * 4), local_82c * 0x18 + 10,
                       local_82c * 0xc + 0x18, 1, DAT_0058b06c);
        }
      }
      local_808 = 0;
      for (local_82c = 0; (local_82c < 1000 && (g_duel_victory_log[local_82c] != '\0'));
           local_82c = local_82c + 1)
      {
        if ((gs_creature_names_00591a08[local_14].metadata[3] ==
             (g_duel_victory_log[local_82c] & 0xf)) &&
            ((int)(char)g_duel_victory_log[local_82c] >> 4 == monster_color))
        {
          local_808 = local_808 + 1;
        }
      }
      if (local_7f0 == 1)
      {
        iVar4 = IsWizardColorFeminine(monster_color);
        if (iVar4 == 0)
        {
          sprintf(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[4],
                  gs_wizardnames_0077ee70 + monster_color * 0x32);
        }
        else
        {
          sprintf(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[3],
                  gs_wizardnames_0077ee70 + monster_color * 0x32);
        }
        pcVar11 = FUN_00561441(local_14);
        pcVar10 = gs_encounter_preduel_0077f0d0[0xb];
        sVar12 = strlen(g_ui_message_buffer);
        sprintf(g_ui_message_buffer + sVar12, pcVar10, pcVar11);
      }
      else if (local_7f0 == 2)
      {
        iVar4 = FindNearestTownIndex((int)(g_lair_or_monster_slots[slot_index].world_x +
                                           (g_lair_or_monster_slots[slot_index].world_x >> 0x1f & 0x1fU)) >>
                                         5,
                                     (int)(g_lair_or_monster_slots[slot_index].world_y +
                                           (g_lair_or_monster_slots[slot_index].world_y >> 0x1f & 0x1fU)) >>
                                         5);
        local_8c4 = BuildTownDisplayName(iVar4);
        iVar4 = IsWizardColorFeminine(monster_color);
        if (iVar4 == 0)
        {
          FUN_00564e70(g_ui_message_buffer, 0x1000, gs_encounter_preduel_0077f0d0[6]);
        }
        else
        {
          FUN_00564e70(g_ui_message_buffer, 0x1000, gs_encounter_preduel_0077f0d0[5]);
        }
        pcVar11 = FUN_00561441(local_14);
        pcVar10 = gs_encounter_preduel_0077f0d0[0xb];
        sVar12 = strlen(g_ui_message_buffer);
        sprintf(g_ui_message_buffer + sVar12, pcVar10, pcVar11);
      }
      else if (local_808 < 5)
      {
        iVar4 = IsWizardColorFeminine(monster_color);
        if (iVar4 == 0)
        {
          sprintf(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[8],
                  gs_wizardnames_0077ee70 + monster_color * 0x32);
        }
        else
        {
          sprintf(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[7],
                  gs_wizardnames_0077ee70 + monster_color * 0x32);
        }
        pcVar11 = FUN_00561441(local_14);
        pcVar10 = gs_encounter_preduel_0077f0d0[0xb];
        sVar12 = strlen(g_ui_message_buffer);
        sprintf(g_ui_message_buffer + sVar12, pcVar10, pcVar11);
      }
      else
      {
        iVar4 = IsWizardColorFeminine(monster_color);
        if (iVar4 == 0)
        {
          FUN_00564e70(g_ui_message_buffer, 0x1000, gs_encounter_preduel_0077f0d0[10]);
        }
        else
        {
          FUN_00564e70(g_ui_message_buffer, 0x1000, gs_encounter_preduel_0077f0d0[9]);
        }
        strcat(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[0xc]);
      }
      local_87c = 1;
      if ((((local_830 == 0) || (Gold < local_7f4)) || (local_7f0 != 0)) || (4 < local_808))
      {
        local_81c = 0xffffffff;
      }
      else
      {
        pcVar10 = gs_encounter_preduel_0077f0d0[0xd];
        iVar4 = local_7f4;
        sVar12 = strlen(g_ui_message_buffer);
        sprintf(g_ui_message_buffer + sVar12, pcVar10, iVar4);
        local_81c = local_87c;
        local_87c = local_87c + 1;
      }
      if (((local_808 < g_shandalar_difficulty * 2 + 2) || (local_7f0 != 0)) || (4 < local_808))
      {
        local_818 = 0xffffffff;
      }
      else
      {
        strcat(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[0xf]);
        local_818 = local_87c;
        local_87c = local_87c + 1;
      }
      if ((((g_world_magic_bitmap & 1U) == 0) || (local_824 != 0)) || (local_7f8 != 1))
      {
        if (local_824 == 0)
        {
          local_814 = 0xffffffff;
        }
        else
        {
          strcat(g_ui_message_buffer, DAT_0058b070);
          local_814 = 0xffffffff;
        }
      }
      else
      {
        strcat(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[0x10]);
        local_814 = local_87c;
        local_87c = local_87c + 1;
      }
      if (DAT_008bd200 < 0)
      {
        local_828 = 0;
      }
      else
      {
        do
        {
          iVar4 = ScaleUiCoordinate(0x100);
          iVar5 = global_screen_width / 2;
          iVar6 = MeasureMultilineTextWidth(PTR_DAT_005832b4, g_ui_message_buffer);
          local_828 = RunTextMenuAt(g_ui_message_buffer, (iVar5 - iVar6 / 2) + -2, iVar4);
        } while (local_828 == 0xffffffff);
      }
      if (((((int)local_828 < 1) && (4 < local_808)) && (-local_14 != g_current_quest_type)) &&
          (local_7f0 == 0))
      {
        local_8d8 = FUN_00522508(3);
        local_8d4 = FUN_00522508(3);
        if (local_8d4 == 0)
        {
          cVar1 = gs_creature_names_00591a08[local_14].metadata[1];
          iVar4 = FUN_00522508(10 - g_shandalar_difficulty);
          local_8cc = (cVar1 + iVar4) * 10;
        }
        else if ((local_8d4 != 1) && (local_8d4 == 2))
        {
          local_8d0 = FUN_00522508(3);
          local_8d0 = local_8d0 + 1;
          local_8c8 = FUN_00522508(5);
          local_8c8 = local_8c8 + 1;
        }
        strcpy(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[0x11]);
        if (local_8d8 == 0)
        {
          strcat(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[0x12]);
        }
        else if (local_8d8 == 1)
        {
          strcat(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[0x13]);
        }
        else if (local_8d8 == 2)
        {
          iVar4 = monster_color * 100 + 0x77de00;
          pcVar10 = gs_encounter_preduel_0077f0d0[0x14];
          sVar12 = strlen(g_ui_message_buffer);
          sprintf(g_ui_message_buffer + sVar12, pcVar10, iVar4);
        }
        if (local_8d4 == 0)
        {
          pcVar10 = gs_encounter_preduel_0077f0d0[0x15];
          iVar4 = local_8cc;
          sVar12 = strlen(g_ui_message_buffer);
          sprintf(g_ui_message_buffer + sVar12, pcVar10, iVar4);
        }
        else if (local_8d4 == 1)
        {
          strcat(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[0x16]);
        }
        else if (local_8d4 == 2)
        {
          if (local_8d0 == 1)
          {
            pcVar10 = gs_encounter_preduel_0077f0d0[monster_color + 0x17];
            pcVar11 = gs_encounter_preduel_0077f0d0[0x17];
            iVar4 = local_8d0;
            sVar12 = strlen(g_ui_message_buffer);
            sprintf(g_ui_message_buffer + sVar12, pcVar11, iVar4, pcVar10);
          }
          else
          {
            pcVar10 = gs_encounter_preduel_0077f0d0[monster_color + 0x1c];
            pcVar11 = gs_encounter_preduel_0077f0d0[0x17];
            iVar4 = local_8d0;
            sVar12 = strlen(g_ui_message_buffer);
            sprintf(g_ui_message_buffer + sVar12, pcVar11, iVar4, pcVar10);
          }
        }
        strcat(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[0x22]);
        local_828 = RunTextMenuAt(g_ui_message_buffer, 0x80, 0xc0);
        if ((local_828 == 0) && (local_8d8 == 0))
        {
          LoadCreatureDuelDeck(local_14, 0xffffffff, 0, -1);
          for (slot_index = 0; slot_index < 500; slot_index = slot_index + 1)
          {
            iVar4 = DrawRandomCardFromInitialLibrary(DAT_0057a750);
            global_library[1][slot_index] = iVar4;
            if ((global_cards_data[global_library[1][slot_index]].extra_ability & 0x100) != 0)
            {
              global_library[1][slot_index] = -1;
            }
          }
          SelectPalette(global_main_hdc, g_realized_palette_handle, 0);
          AnimatePaletteToColor(0, DAT_00589dec);
          FUN_004be570(s_dbox_spr_0058b074, 0x71, 0xe3);
          local_874 = FUN_0056a515(unk_008b35ec, global_library[1], 500,
                                   gs_encounter_preduel_0077f0d0[0x23], 1, (int *)DAT_0058b080);
          if ((global_library[1][local_874] != -1) &&
              (local_830 = FUN_0056bd9d(global_library[1][local_874]), local_830 != 0xffffffff))
          {
            deck[local_830] = deck[local_830] | 0x4000;
          }
        }
        if ((local_828 == 0) && (local_8d8 == 1))
        {
          strcpy(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[0x24]);
          local_804 = 0;
          for (slot_index = 0; slot_index < 0xc; slot_index = slot_index + 1)
          {
            if ((g_world_magic_bitmap & 1 << ((byte)slot_index & 0x1f)) == 0)
            {
              strcat(g_ui_message_buffer, DAT_0058b084);
              strcat(g_ui_message_buffer, gs_worldmagic_names_00780660[slot_index]);
              strcat(g_ui_message_buffer, DAT_0058b088);
              local_7e4[local_804] = slot_index;
              local_804 = local_804 + 1;
            }
          }
          if (local_804 == 0)
          {
            strcpy(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[0x25]);
            local_828 = RunTextMenuAtScaled(g_ui_message_buffer, 0x50, 100);
          }
          else
          {
            local_828 = RunTextMenuAt(g_ui_message_buffer, 0xa0, 200);
            if (local_828 != 0xffffffff)
            {
              local_82c = Scards[local_7e4[local_828]].worldmagic_city;
              g_town_slots[local_82c].status_and_ruling_wizard =
                  g_town_slots[local_82c].status_and_ruling_wizard | 2;
              FUN_00431526(0x80, g_town_slots[local_82c].world_x, g_town_slots[local_82c].world_y);
              BuildTownDisplayName(local_82c);
              FUN_00564e70(g_ui_message_buffer, 0x1000, gs_encounter_preduel_0077f0d0[0x26]);
              RunTextMenuAt(g_ui_message_buffer, 0x5a, 0x6e);
              SelectPalette(global_main_hdc, g_realized_palette_handle, 0);
              AnimatePaletteToColor(0, DAT_00589dec);
              ShowWorldMapScreen(0);
              AnimatePaletteToColor(0, DAT_00589dec);
              ClearGraphicsPageWithPaletteColor(0, 0);
            }
          }
        }
        if ((local_828 == 0) && (local_8d8 == 2))
        {
          if (DAT_0073ea70[monster_color - 1] == 0)
          {
            if (((byte)g_castle_dungeon_slots[monster_color + -1].clues_bitmap & 7) == 7)
            {
              FUN_0050caa0(monster_color + -1);
            }
            else
            {
              SelectPalette(global_main_hdc, g_realized_palette_handle, 0);
              AnimatePaletteToColor(0, DAT_00589dec);
              RevealRandomCastleDungeonClue(monster_color + -1);
            }
          }
          else
          {
            sprintf(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[0x27],
                    monster_color * 100 + 0x77de00);
            RunTextMenuAtScaled(g_ui_message_buffer, 0x54, 0x74);
          }
        }
        if (local_828 == 1)
        {
          if (local_8d4 == 0)
          {
            Gold = Gold + local_8cc;
          }
          else if (local_8d4 == 1)
          {
            g_food = g_food + 0x28;
          }
          else if (local_8d4 == 2)
          {
            DAT_0078990c[local_8c8] = DAT_0078990c[local_8c8] + local_8d0;
          }
        }
        if (local_828 != 2)
          goto LAB_004f683f;
      }
      else
      {
        if (local_814 == local_828)
          goto LAB_004f483f;
        if (local_818 == local_828)
        {
          bVar2 = FUN_00534530();
          if (CONCAT31(extraout_var, bVar2) == 0)
          {
            PlayDuelMusic(2);
            strcpy(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[0x28]);
            LoadPcxIntoPage(1, s_losedul2_pic_0058b08c);
            StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, 0, 0x280, 0x1e0, PTR_DAT_005832b4, 0, 0, global_screen_width,
                                    global_screen_height);
            FUN_004f2407(ANTE_CARD(0, 0), 0x17, 0x50, 1, g_ui_message_buffer);
            ClearInputAndWaitForMouseRelease();
            WaitForInputEventUnlessBlocked();
            RemoveCardFromDeckById(ANTE_CARD(0, 0));
          }
          else
          {
            FUN_004ce992(0x1e);
          }
          LoadPcxIntoPageNoPalette(s_Prdblk_pic_0058b09c);
          SelectPalette(global_main_hdc, g_realized_palette_handle, 0);
          AnimatePaletteToColor(0, DAT_00589dec);
          ClearGraphicsPageWithPaletteColor(0, 0);
          LoadPcxIntoPageNoPalette(s_advfac64_pic_0058b0a8);
          FUN_004be570(s_dbox_spr_0058b0b8, 0x71, 0xe3);
          return 0;
        }
        if (local_81c == local_828)
        {
          Gold = Gold - local_7f4;
          SelectPalette(global_main_hdc, g_realized_palette_handle, 0);
          AnimatePaletteToColor(0, DAT_00589dec);
          LoadPcxIntoPageNoPalette(s_advfac64_pic_0058b0c4);
          FUN_004be570(s_dbox_spr_0058b0d4, 0x71, 0xe3);
          return 0;
        }
      }
      goto LAB_004f4bb2;
    }
    for (local_82c = 0; local_82c < local_7f8; local_82c = local_82c + 1)
    {
      if (DAT_008bd200 == 0)
      {
        FUN_004f2407(*(int *)(global_ante_cards[0] + local_82c * 4), local_82c * 0x18 + 10,
                     local_82c * 0xc + 0x18, 1, DAT_0058b0e0);
      }
    }
    iVar4 = IsWizardColorFeminine(monster_color);
    if (iVar4 == 0)
    {
      sprintf(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[0x2a],
              gs_wizardnames_0077ee70 + monster_color * 0x32);
    }
    else
    {
      sprintf(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[0x29],
              gs_wizardnames_0077ee70 + monster_color * 0x32);
    }
    pcVar11 = FUN_00561441(local_14);
    pcVar10 = gs_encounter_preduel_0077f0d0[0xb];
    sVar12 = strlen(g_ui_message_buffer);
    sprintf(g_ui_message_buffer + sVar12, pcVar10, pcVar11);
    ClearInputAndWaitForMouseRelease();
    iVar4 = ScaleUiCoordinate(0x100);
    iVar5 = global_screen_width / 2;
    iVar6 = MeasureMultilineTextWidth(PTR_DAT_005832b4, g_ui_message_buffer);
    RunTextMenuAt(g_ui_message_buffer, (iVar5 - iVar6 / 2) + -2, iVar4);
    FUN_00562893();
  LAB_004f4bb2:
    local_c = 0;
    for (local_82c = 0; local_82c < 1000; local_82c = local_82c + 1)
    {
      if (g_duel_victory_log[local_82c] != '\0')
      {
        local_c = local_c + 1;
      }
    }
    local_830 = DAT_008b3240[monster_color];
    if ('\n' < gs_creature_names_00591a08[local_14].metadata[3])
    {
      local_830 = g_deck_color_bitmap;
    }
    local_800 = DAT_008b3240[monster_color + 6];
    if ('\n' < gs_creature_names_00591a08[local_14].metadata[3])
    {
      local_800 = _DAT_007483e4;
    }
    iVar4 = ClampIntToRange(3 - local_c / 3, 0, 3);
    DAT_00742fd0 = -iVar4;
    if ((g_shandalar_difficulty == 3) || ('\n' < gs_creature_names_00591a08[local_14].metadata[3]))
    {
      DAT_00742fd0 = 0;
    }
    if (gs_creature_names_00591a08[local_14].metadata[3] == '\v')
    {
      DAT_00742fd0 = Scards[g_world_magic_offer_slot_index].worldmagic_price / 500 + -1;
    }
    if (-g_shandalar_difficulty < DAT_00742fd0)
    {
      local_830 = g_deck_color_bitmap;
    }
    local_870 = (uint)((int)(CONCAT44(g_monster_timer >> 0x1f, g_monster_timer >> 2) % 3) == 0);
    if (local_870 != 0)
    {
      PlaySoundEffectOnChannel(s_x_sound_dsummon_wav_0058b0e4, 0xf, 100, 100, 0);
      if (((gs_creature_names_00591a08[local_14].metadata[0x12] & 4U) != 0) &&
          (iVar4 = FUN_00522508(3), iVar4 == 0))
      {
        iVar4 = FUN_00522508(0x23);
        local_830 = iVar4 + 1;
        strcpy(g_ui_message_buffer, s_decks_0_0058b0f8);
        if ((*(int *)(gs_creature_names_00591a08[local_830].metadata + 6) < 100) &&
            (strcat(g_ui_message_buffer, DAT_0058b100),
             *(int *)(gs_creature_names_00591a08[local_830].metadata + 6) < 10))
        {
          strcat(g_ui_message_buffer, DAT_0058b104);
        }
        pcVar10 = _itoa(*(int *)(gs_creature_names_00591a08[local_830].metadata + 6), DAT_0093a870,
                        10);
        strcat(g_ui_message_buffer, pcVar10);
        strcat(g_ui_message_buffer, DAT_0058b108);
        ClearAndLoadInitialLibraryFromDeckFile(g_ui_message_buffer, 0, 0, -1);
        g_selected_wizard_color = 0;
        strcpy(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[0x2b]);
        RunTextMenuAt(g_ui_message_buffer, 0xa0, 0xa0);
      }
      if ((((*(uint *)(gs_creature_names_00591a08[local_14].metadata + 0x12) & 0x110) != 0) &&
           (iVar4 = FUN_00522508(3), iVar4 == 0)) &&
          (-local_14 != g_current_quest_type))
      {
        do
        {
          do
          {
            iVar4 = FUN_00522508(3);
            local_830 = local_14 + iVar4 + 1;
          } while (local_830 == 0x37);
        } while (((int)local_830 < 0x24) && ((int)local_830 % 7 == 0));
        iVar7 = 1;
        iVar6 = 0;
        iVar4 = ScaleUiCoordinate(10);
        iVar5 = ScaleUiCoordinate(0x140);
        DrawCreaturePortrait(local_830, iVar5, iVar4, iVar6, iVar7);
        BuildCreatureNameWithArticle(local_830);
        FUN_00561441(local_14);
        FUN_00564e70(g_ui_message_buffer, 0x1000, gs_encounter_preduel_0077f0d0[0x2c]);
        RunTextMenuAt(g_ui_message_buffer, 0xa0, 0x78);
        local_14 = local_830;
        local_838 = DAT_007a7874 + (int)gs_creature_names_00591a08[local_830].metadata[1] / 2;
      }
      if ((gs_creature_names_00591a08[local_14].metadata[0x12] & 0xc1U) != 0)
      {
        DAT_008cf6d0 = FUN_0056c705(*(int *)(gs_creature_names_00591a08[local_14].metadata + 0x1a));
      }
      if ((gs_creature_names_00591a08[local_14].metadata[0x12] & 0xcbU) != 0)
      {
        local_840 = 1;
        if ((gs_creature_names_00591a08[local_14].metadata[0x12] & 2U) != 0)
        {
          if (g_monster_timer % 3 == 0)
          {
            pcVar10 = FUN_00561441(local_14);
            sprintf(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[0x2d], pcVar10);
          }
          else
          {
            local_840 = 0;
          }
        }
        if ((gs_creature_names_00591a08[local_14].metadata[0x12] & 8U) != 0)
        {
          DAT_0073c7e4 = 1;
          pcVar10 = FUN_00561441(local_14);
          sprintf(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[0x2e], pcVar10);
        }
        if ((gs_creature_names_00591a08[local_14].metadata[0x12] & 0xc1U) != 0)
        {
          if (DAT_00742fd0 < 0)
          {
            DAT_008cf6d0 = -1;
            local_840 = 0;
          }
          else
          {
            FUN_00561441(local_14);
            FUN_00564e70(g_ui_message_buffer, 0x1000, gs_encounter_preduel_0077f0d0[0x2f]);
          }
        }
        if (local_840 != 0)
        {
          RunTextMenuAt(g_ui_message_buffer, 0xb4, 0x8c);
        }
      }
    }
    for (local_82c = 0; local_82c < 7; local_82c = local_82c + 1)
    {
      if (((slot_index != local_82c) && (g_lair_or_monster_slots[local_82c].entry_type != 0)) &&
          ((iVar4 = FUN_004ecf30(g_world_player_x - g_lair_or_monster_slots[local_82c].world_x,
                                 g_world_player_y - g_lair_or_monster_slots[local_82c].world_y),
            iVar4 < 0x40 && (g_lair_or_monster_slots[local_82c].entry_type != -1))))
      {
        FreeOpeningMenuSpriteWorkEntries(local_82c, local_82c + 8);
        g_lair_or_monster_slots[local_82c].entry_type = -1;
      }
    }
    LoadCreatureDuelDeck(local_14, local_874, local_830, local_800);
    DestroyCachedCardArt();
    AnimatePaletteToColor(0, DAT_00589dec);
    SelectPalette(global_main_hdc, g_realized_palette_handle, 0);
    FUN_004be570(s_dbox_spr_0058b110, 0x71, 0xe3);
    sound_stop(0);
    local_8e0 = -1;
    local_8dc = 0;
    while ((local_8e0 != 0 && (local_8dc != 4)))
    {
      local_8dc = FUN_0056d5c0(0, &local_8e0);
    }
    local_8 = RunDuelEngine(local_874, local_14);
    LoadPcxIntoPageNoPalette(s_advfac64_pic_0058b11c);
    if (local_8 == 1)
    {
      local_8e8 = 0xca;
      local_8ec = 0x8c;
      local_8e4 = 0x118;
      FUN_004bb400(gs_creature_names_00591a08[local_14].metadata[3], (byte)monster_color);
      FUN_004290e2(2, local_14 | 0x80);
      ShowStatsWindow(2, DAT_008ce538 << 0x10 | local_14);
      if (-local_14 == g_current_quest_type)
      {
        g_current_quest_type = g_current_quest_type + -100;
      }
      if (gs_creature_names_00591a08[local_14].metadata[3] < '\v')
      {
        local_7e8 = PickWisemanCastleDungeonSlotForHint();
        local_82c = local_838;
        local_87c = 0;
        do
        {
          do
          {
            do
            {
              uVar3 = 1;
              iVar4 = FUN_00522508(6);
              local_874 = FUN_0056bcf7(1 << ((byte)iVar4 & 0x1f), uVar3);
              iVar4 = FUN_0056c0e5(1 << ((byte)monster_color & 0x1f),
                                   (int)global_cards_data[local_874].color,
                                   (-(uint)((local_87c & 1) == 0) & 2) + 1);
            } while (iVar4 == 0);
            iVar4 = FUN_004bb1cf(local_874);
          } while (((iVar4 < 1) || ((global_cards_data[local_874].extra_ability & 0x900) != 0)) ||
                   (uVar3 = GetCardAvailabilityMask(local_874), uVar3 == 0));
          if (((int)local_87c < 3) && (g_duel_ante_card_ids[local_87c] != -1))
          {
            local_874 = g_duel_ante_card_ids[local_87c];
          }
          auStack_868[local_87c] = local_874;
          local_87c = local_87c + 1;
          uVar3 = global_cards_data[local_874].extra_ability;
          iVar4 = FUN_0056c5ea(local_874);
          local_82c = local_82c - (((uVar3 & 0x400) >> 10) + iVar4);
        } while (0 < local_82c);
        PlayDuelMusic(1);
        LoadPcxIntoPageOpaque(1, s_winbak01_pic_0058b12c);
        StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, 0, 0x280, 0x1e0, PTR_DAT_005832b4, 0, 0, global_screen_width,
                                global_screen_height);
        local_878 = (int)(300 / (longlong)(int)(local_87c + 1));
        for (slot_index = 0; slot_index < (int)local_87c; slot_index = slot_index + 1)
        {
          iVar4 = FUN_0056c5ea(auStack_868[slot_index]);
          local_908[6] = iVar4 + -1;
          strcpy(g_ui_message_buffer, gs_browse_0074da20[iVar4 + 0xc]);
          pcVar10 = g_ui_message_buffer;
          iVar5 = 1;
          iVar4 = FUN_00522508(10);
          FUN_004f2407(auStack_868[slot_index],
                       (local_878 * slot_index + 0x6f) - (int)((local_87c - 1) * local_878) / 2,
                       iVar4 + 0x10, iVar5, pcVar10);
        }
        if (local_7e8 == -1)
        {
          PTR_DAT_005832b4->font_slot = 5;
          DrawTextAt(PTR_DAT_005832b4, 0xff, 0x140, 0x15e, gs_encounter_postduel_0077f050[6]);
          WaitForInputEventUnlessBlocked();
          for (slot_index = 0; slot_index < (int)local_87c; slot_index = slot_index + 1)
          {
            local_830 = FUN_0056bd9d(auStack_868[slot_index]);
            if (local_830 != 0xffffffff)
            {
              deck[local_830] = deck[local_830] | 0x4000;
            }
          }
        }
        else
        {
          local_820 = 0xcb;
          LoadPcxIntoPage(1, s_endplak_pic_0058b13c);
          iVar4 = ScaleUiCoordinate(0x36);
          iVar5 = ScaleUiCoordinate(0x128);
          iVar6 = ScaleUiCoordinate(0x195);
          iVar7 = ScaleUiCoordinate(0x34);
          StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, 0, 0x128, 0x36, PTR_DAT_005832b4, iVar7, iVar6, iVar5, iVar4);
          pcVar10 = FUN_005081e0(local_7e8);
          strcpy(g_ui_message_buffer, pcVar10);
          DrawUiScaledCenteredText(g_ui_message_buffer, 100, local_820 + 3, 0xca);
          local_820 = local_820 + 9;
          strcpy(g_ui_message_buffer, DAT_0058b148);
          for (local_82c = 0; local_82c < 3; local_82c = local_82c + 1)
          {
            if ((&g_castle_dungeon_slots[local_7e8].card_slot_1)[local_82c] != -1)
            {
              if (local_82c != 0)
              {
                strcat(g_ui_message_buffer, DAT_0058b14c);
              }
              strcat(g_ui_message_buffer,
                     global_cards_data[(&g_castle_dungeon_slots[local_7e8].card_slot_1)[local_82c]].name);
            }
          }
          FUN_00430f69(g_ui_message_buffer, 100, local_820, 0xca);
          local_820 = local_820 + 7;
          local_830 = 0;
          for (local_82c = 0; local_82c < 4; local_82c = local_82c + 1)
          {
            if ((g_castle_dungeon_slots[local_7e8].clues_bitmap & 1 << ((byte)local_82c & 0x1f)) !=
                0)
            {
              local_830 = local_830 + 1;
            }
          }
          if (local_830 == 0)
          {
            strcpy(g_ui_message_buffer, gs_encounter_postduel_0077f050[0]);
          }
          else if (local_830 == 1)
          {
            strcpy(g_ui_message_buffer, gs_encounter_postduel_0077f050[1]);
          }
          else if (local_830 == 2)
          {
            strcpy(g_ui_message_buffer, gs_encounter_postduel_0077f050[2]);
          }
          else
          {
            strcpy(g_ui_message_buffer, DAT_0058b150);
          }
          FUN_00430f69(g_ui_message_buffer, 100, local_820, 200);
          iVar4 = IsCreatureTypeFeminine(local_14);
          if (iVar4 == 0)
          {
            pcVar10 = FUN_00561441(local_14);
            sprintf(g_ui_message_buffer, gs_encounter_postduel_0077f050[4], pcVar10);
          }
          else
          {
            pcVar10 = FUN_00561441(local_14);
            sprintf(g_ui_message_buffer, gs_encounter_postduel_0077f050[3], pcVar10);
          }
          strcat(g_ui_message_buffer, gs_encounter_postduel_0077f050[5]);
          FUN_004f2165();
          iVar4 = RunTextMenuAtScaled(g_ui_message_buffer, 0x40, 0x7c);
          if (iVar4 == 0)
          {
            if (local_7e8 != -1)
            {
              g_castle_dungeon_slots[local_7e8].rules_bitmap =
                  g_castle_dungeon_slots[local_7e8].rules_bitmap | 0x200;
            }
            for (slot_index = 0; slot_index < (int)local_87c; slot_index = slot_index + 1)
            {
              local_830 = FUN_0056bd9d(auStack_868[slot_index]);
              if (local_830 != 0xffffffff)
              {
                deck[local_830] = deck[local_830] | 0x4000;
              }
            }
          }
          else
          {
            RevealRandomCastleDungeonClue(local_7e8);
          }
        }
        ClearInputAndWaitForMouseRelease();
        RecountDeckCardTotals();
        if (0x1c1 < DAT_00789938)
        {
          sprintf(g_ui_message_buffer, gs_encounter_postduel_0077f050[0x1f], DAT_00789938);
          iVar4 = ScaleUiCoordinate(0x100);
          iVar5 = global_screen_width / 2;
          iVar6 = MeasureMultilineTextWidth(PTR_DAT_005832b4, g_ui_message_buffer);
          RunTextMenuAt(g_ui_message_buffer, (iVar5 - iVar6 / 2) + -2, iVar4);
        }
        local_10 = *(uint *)(gs_creature_names_00591a08[local_14].metadata + 0x16);
        if (local_870 == 0)
        {
          local_10 = 0;
        }
        iVar4 = FUN_00522508(0x28);
        if (iVar4 < local_838)
        {
          local_908[1] = 2;
          local_908[2] = 1;
          local_908[3] = 4;
          local_908[4] = 3;
          local_908[5] = 0;
          DAT_0078990c[monster_color] = DAT_0078990c[monster_color] + 1;
          LoadPcxIntoPage(1, s_winbak02_pic_0058b154);
          StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, 0, 0x280, 0x1e0, PTR_DAT_005832b4, 0, 0, global_screen_width,
                                  global_screen_height);
          ClearInputAndWaitForMouseRelease();
          strcpy(g_ui_message_buffer, gs_encounter_postduel_0077f050[7]);
          local_908[0] = MeasureMultilineTextWidth(PTR_DAT_005832b4, g_ui_message_buffer);
          iVar8 = 0;
          iVar4 = ScaleUiCoordinate(0x30);
          iVar4 = iVar4 + 0xe;
          iVar5 = local_908[0] + 0x10;
          iVar6 = ScaleUiCoordinate(0xa0);
          iVar7 = ScaleUiCoordinate(0xa0);
          DrawTiledDialogBoxFrame((iVar7 - local_908[0] / 2) + -8, iVar6, iVar5, iVar4, iVar8);
          iVar4 = ScaleUiCoordinate(0xae);
          iVar5 = ScaleUiCoordinate(0xa0);
          DrawFormattedTextShadowedCentered(PTR_DAT_005832b4, 0xff, iVar5, iVar4);
          pEVar13 = g_world_magic_avatar_sprites[local_908[monster_color]];
          iVar4 = ScaleUiCoordinate(0x22);
          iVar5 = ScaleUiCoordinate(0x1a);
          iVar6 = ScaleUiCoordinate(0xbd);
          iVar7 = ScaleUiCoordinate(0xa0);
          iVar8 = ScaleUiCoordinate(0xd);
          DrawEncodedImageResampled(PTR_DAT_005832b4, iVar7 - iVar8, iVar6, iVar5, iVar4, pEVar13);
          if (local_10 == 0)
          {
            ClearInputAndWaitForMouseRelease();
            WaitForInputEventUnlessBlocked();
          }
        }
        else if ((local_10 == 0) && (local_870 != 0))
        {
          iVar4 = FUN_00522508(0xc);
          local_10 = 1 << ((byte)iVar4 & 0x1f) & 0x1a19;
        }
        if (local_10 != 0)
        {
          PlaySoundEffectOnChannel(s_x_sound_treasure_wav_0058b164, 0xf, 100, 100, 0);
          LoadPcxIntoPage(1, s_winbak02_pic_0058b17c);
          StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, 0, 0x280, 0x1e0, PTR_DAT_005832b4, 0, 0, global_screen_width,
                                  global_screen_height);
          iVar7 = 0;
          iVar6 = 1;
          iVar4 = ScaleUiCoordinate(0x23);
          iVar5 = ScaleUiCoordinate(local_8ec);
          DrawCreaturePortrait(local_14, iVar5, iVar4, iVar6, iVar7);
          PTR_DAT_005832b4->font_slot = 4;
          if (g_shandalar_difficulty == 0)
          {
            iVar4 = FUN_00566238();
            if (iVar4 == 0)
            {
              pcVar10 = FUN_00561441(local_14);
              sprintf(g_ui_message_buffer, gs_encounter_postduel_0077f050[9], pcVar10);
            }
            else
            {
              pcVar10 = FUN_00561441(local_14);
              sprintf(g_ui_message_buffer, gs_encounter_postduel_0077f050[8], pcVar10);
            }
          }
          else if (g_shandalar_difficulty == 1)
          {
            iVar4 = FUN_00566238();
            if (iVar4 == 0)
            {
              pcVar10 = FUN_00561441(local_14);
              sprintf(g_ui_message_buffer, gs_encounter_postduel_0077f050[0xb], pcVar10);
            }
            else
            {
              pcVar10 = FUN_00561441(local_14);
              sprintf(g_ui_message_buffer, gs_encounter_postduel_0077f050[10], pcVar10);
            }
          }
          else if (g_shandalar_difficulty == 2)
          {
            iVar4 = FUN_00566238();
            if (iVar4 == 0)
            {
              pcVar10 = FUN_00561441(local_14);
              sprintf(g_ui_message_buffer, gs_encounter_postduel_0077f050[0xd], pcVar10);
            }
            else
            {
              pcVar10 = FUN_00561441(local_14);
              sprintf(g_ui_message_buffer, gs_encounter_postduel_0077f050[0xc], pcVar10);
            }
          }
          else if (g_shandalar_difficulty == 3)
          {
            iVar4 = FUN_00566238();
            if (iVar4 == 0)
            {
              pcVar10 = FUN_00561441(local_14);
              sprintf(g_ui_message_buffer, gs_encounter_postduel_0077f050[0xf], pcVar10);
            }
            else
            {
              pcVar10 = FUN_00561441(local_14);
              sprintf(g_ui_message_buffer, gs_encounter_postduel_0077f050[0xe], pcVar10);
            }
          }
          else
          {
            strcpy(g_ui_message_buffer, DAT_0058b18c);
          }
          DrawTextAt(PTR_DAT_005832b4, local_8e8, local_8ec, local_8e4, g_ui_message_buffer);
          local_8e4 = local_8e4 + 0x28;
        }
        if (((local_10 & 1) != 0) && (life[0] != DAT_00716024))
        {
          g_next_duel_life_delta = life[0] - DAT_00716024;
          sprintf(g_ui_message_buffer, gs_encounter_postduel_0077f050[0x10], life[0]);
          DrawTextAt(PTR_DAT_005832b4, local_8e8, local_8ec, local_8e4, g_ui_message_buffer);
        }
        if ((local_10 & 0x800) != 0)
        {
          iVar4 = FUN_00522508(4);
          g_next_duel_card_id = iVar4 + 1;
          sprintf(g_ui_message_buffer, gs_encounter_postduel_0077f050[0x11], g_next_duel_card_id);
          DrawTextAt(PTR_DAT_005832b4, local_8e8, local_8ec, local_8e4, g_ui_message_buffer);
        }
        if ((local_10 & 0x402) != 0)
        {
          do
          {
            local_7fc = FUN_00522508(0x40);
            local_820 = FUN_00522508(0x40);
            uVar3 = FUN_0043146b(local_7fc, local_820);
          } while (uVar3 == 0);
          g_world_player_x = local_7fc * 0x20 + 0x10;
          g_world_player_y = local_820 * 0x20 + 0x10;
          g_world_scroll_cache_ready = 0;
          DrawTextAt(PTR_DAT_005832b4, local_8e8, local_8ec, local_8e4,
                     gs_encounter_postduel_0077f050[0x12]);
        }
        if ((local_10 & 4) != 0)
        {
          g_next_duel_card_id =
              FUN_0056c705(*(int *)(gs_creature_names_00591a08[local_14].metadata + 0x1a));
          local_830 = FUN_0056bd9d(g_next_duel_card_id);
          if (local_830 != 0xffffffff)
          {
            deck[local_830] = deck[local_830] | 0x4000;
          }
          sprintf(g_ui_message_buffer, gs_encounter_postduel_0077f050[0x13],
                  g_next_duel_card_id * 0x48 + 0x594209);
          FUN_004f2407(g_next_duel_card_id, 0xa0, 0x70, 1, g_ui_message_buffer);
          g_next_duel_card_id = -1;
        }
        if ((local_10 & 0x10) != 0)
        {
          g_next_duel_card_id = 0;
          DrawTextAt(PTR_DAT_005832b4, local_8e8, local_8ec, local_8e4,
                     gs_encounter_postduel_0077f050[0x14]);
        }
        if ((local_10 & 0x20) != 0)
        {
          g_next_duel_card_id =
              FUN_0056c705(*(int *)(gs_creature_names_00591a08[local_14].metadata + 0x1a));
          sprintf(g_ui_message_buffer, gs_encounter_postduel_0077f050[0x15],
                  g_next_duel_card_id * 0x48 + 0x594209);
          FUN_004f2407(g_next_duel_card_id, 0xa0, 0x70, 1, g_ui_message_buffer);
        }
        if ((local_10 & 0x180) != 0)
        {
          do
          {
            do
            {
              local_830 = FUN_00522508(g_card_count - 0x39);
            } while ((global_cards_data[local_830].type & 0x42) != 0x40);
          } while (((int)local_830 < 5) || (uVar3 = GetCardAvailabilityMask(local_830), uVar3 == 0));
          g_next_duel_card_id = local_830;
          sprintf(g_ui_message_buffer, gs_encounter_postduel_0077f050[0x15],
                  local_830 * 0x48 + 0x594209);
          FUN_004f2407(g_next_duel_card_id, 0xa0, 0x70, 1, g_ui_message_buffer);
        }
        if ((local_10 & 0x200) != 0)
        {
          iVar4 = FUN_00522508(0x1e);
          g_food = g_food + iVar4 + 0x14;
          strcpy(g_ui_message_buffer, gs_encounter_postduel_0077f050[0x16]);
          DrawTextAt(PTR_DAT_005832b4, local_8e8, local_8ec, local_8e4, g_ui_message_buffer);
        }
        if ((local_10 & 0x40) != 0)
        {
          strcpy(g_ui_message_buffer, gs_encounter_postduel_0077f050[0x17]);
          DrawTextAt(PTR_DAT_005832b4, local_8e8, local_8ec, local_8e4, g_ui_message_buffer);
          WaitForInputEventUnlessBlocked();
          FUN_005616aa(1);
          local_874 = RunCardBrowser(gs_encounter_postduel_0077f050[0x19], 0, 0xffffffff, 1, 0);
          while (local_874 == 0xffffffff)
          {
            local_874 = RunCardBrowser(gs_encounter_postduel_0077f050[0x19], 0, 0xffffffff, 0, 0);
          }
          if (local_874 != 0xffffffff)
          {
            iVar4 = FUN_0056bd9d(local_874);
            deck[iVar4] = deck[iVar4] | 0x4000;
          }
          local_10 = 0;
        }
        if ((local_10 & 0x1000) != 0)
        {
          strcpy(g_ui_message_buffer, gs_encounter_postduel_0077f050[0x18]);
          DrawTextAt(PTR_DAT_005832b4, local_8e8, local_8ec, local_8e4, g_ui_message_buffer);
          WaitForInputEventUnlessBlocked();
          FUN_005616aa(1);
          for (slot_index = 0; slot_index < 500; slot_index = slot_index + 1)
          {
            local_7e4[slot_index] = deck[slot_index];
            if (local_7e4[slot_index] != 0xffffffff)
            {
              local_7e4[slot_index] = local_7e4[slot_index] & 0xfff;
            }
          }
          local_874 = FUN_0056a515(unk_008b35ec, (int *)local_7e4, 500,
                                   gs_encounter_postduel_0077f050[0x19], 1, (int *)DAT_0058b190);
          if (local_874 != 0xffffffff)
          {
            iVar4 = FUN_0056bd9d(local_7e4[local_874]);
            deck[iVar4] = deck[iVar4] | 0x4000;
          }
          local_10 = 0;
        }
        if ((local_10 & 8) != 0)
        {
          strcpy(g_ui_message_buffer, gs_encounter_postduel_0077f050[0x1a]);
          DrawTextAt(PTR_DAT_005832b4, local_8e8, local_8ec, local_8e4, g_ui_message_buffer);
          Gold = Gold + 100;
        }
        ClearInputAndWaitForMouseRelease();
        if (local_10 != 0)
        {
          WaitForInputEventUnlessBlocked();
        }
        iVar4 = FUN_00522508((int)(0x40 / (longlong)(g_shandalar_difficulty + 1)));
        if (iVar4 < local_838)
        {
          DAT_008b3240[monster_color] = g_deck_color_bitmap;
        }
        iVar4 = FUN_00522508((int)(0x80 / (longlong)(g_shandalar_difficulty + 1)));
        if (iVar4 < local_838)
        {
          DAT_008b3240[monster_color + 6] = _DAT_007483e4;
        }
      }
    }
    if (local_8 == 0)
    {
      PlayDuelMusic(2);
      LoadPcxIntoPage(1, s_losedul2_pic_0058b194);
      StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, 0, 0x280, 0x1e0, PTR_DAT_005832b4, 0, 0, global_screen_width,
                              global_screen_height);
      FUN_004290e2(2, local_14);
      for (local_83c = 0; local_83c < 3; local_83c = local_83c + 1)
      {
        local_834 = *(uint *)(global_ante_cards[0] + local_83c * 4);
        if (local_834 != 0xffffffff)
        {
          strcpy(g_ui_message_buffer, gs_encounter_postduel_0077f050[0x1b]);
          FUN_004f2407(local_834, 0x17, 0x50, 1, g_ui_message_buffer);
          ClearInputAndWaitForMouseRelease();
          WaitForInputEventUnlessBlocked();
          RemoveCardFromDeckById(local_834);
        }
      }
    }
    if (local_8 == -1)
    {
      LoadPcxIntoPage(1, s_losedul2_pic_0058b1a4);
      StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, 0, 0x280, 0x1e0, PTR_DAT_005832b4, 0, 0, global_screen_width,
                              global_screen_height);
      iVar6 = 0;
      iVar5 = 1;
      iVar4 = ScaleUiCoordinate(0x3c);
      DrawCreaturePortrait(local_14, (int)(global_screen_width + (global_screen_width >> 0x1f & 3U)) >> 2, iVar4, iVar5, iVar6);
      PTR_DAT_005832b4->font_slot = 4;
      iVar4 = IsCreatureTypeFeminine(local_14);
      if (iVar4 == 0)
      {
        pcVar10 = FUN_00561441(local_14);
        sprintf(g_ui_message_buffer, gs_encounter_postduel_0077f050[0x1d], pcVar10);
      }
      else
      {
        pcVar10 = FUN_00561441(local_14);
        sprintf(g_ui_message_buffer, gs_encounter_postduel_0077f050[0x1c], pcVar10);
      }
      iVar4 = ScaleUiCoordinate(300);
      DrawWorldUiFormattedText(PTR_DAT_005832b4, 0xca,
                               (int)(global_screen_width + (global_screen_width >> 0x1f & 3U)) >> 2, iVar4);
      iVar4 = GetFontLineHeight(PTR_DAT_005832b4->font_slot);
      iVar4 = ScaleUiCoordinate(iVar4 * 4 + 300);
      DrawWorldUiFormattedText(PTR_DAT_005832b4, 0xca,
                               (int)(global_screen_width + (global_screen_width >> 0x1f & 3U)) >> 2, iVar4);
      ClearInputAndWaitForMouseRelease();
      WaitForInputEventUnlessBlocked();
    }
  }
LAB_004f683f:
  FUN_004be570(s_dbox_spr_0058b1b4, 0x71, 0xe3);
  SelectPalette(global_main_hdc, g_realized_palette_handle, 0);
  LoadPcxIntoPageNoPalette(s_advfac64_pic_0058b1c0);
  ClearInputAndWaitForMouseRelease();
  return local_8;
LAB_004f483f:
  do
  {
    do
    {
      local_830 = FUN_00522508(500);
    } while (deck[local_830] == -1);
  } while (((*(byte *)((int)deck + local_830 * 4 + 1) & 0x40) != 0) ||
           ((deck[local_830] & 0xfffU) < 5));
  ANTE_CARD(0, 0) = deck[local_830] & 0xfff;
  local_824 = 1;
  local_808 = 0;
  goto LAB_004f3a77;
}
