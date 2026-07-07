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
extern int g_world_magic_town_flags[5];
extern int g_world_location_music_active;

extern int DAT_00789938;
extern int DAT_0078df68;
extern int g_reveal_all_world_info;
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
extern int unk_008b35ec;
extern int unk_00742fc4;
extern int DAT_008bd200;
extern int DAT_008c7408[7];
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
extern HWND DAT_00896714;

char *BuildCreatureNameWithArticle(int creature_type);
char *BuildTownDisplayName(int town_index);
char *FUN_00428783(unsigned int mana_mask);
char *FUN_004f2e17(int town_index);
char *GetDungeonName(int dungeon_index);
char *FUN_00561441(int creature_type);
DWORD FormatMessageFromStringStripCarriageReturns(char *dst, DWORD max_length, LPCVOID format, ...);
EncodedImage *EncodeSpriteFromPage(int page_number, int x, int y, int width, int height);
int BuyAnyCardFromTown(int color, int type);
int ClampIntToRange(int value, int min_value, int max_value);
int CountDuelPoolEligibleTowns(void);
int DrawRandomCardFromInitialLibrary(int param_1);
int ExitIfNoUsableDeckCards(void);
int FindDeckSlotForQuestColorAndType(unsigned char color, unsigned char type);
int FindNearestTownIndex(int world_x, int world_y);
int GetFirstManaColorIndex(int mask);
int FUN_004bb1cf(unsigned int card_id);
unsigned int FUN_004bb458(int world_magic_slot_index);
unsigned int GetWorldTileType(int x, int y);
int FUN_00469099(void);
int FUN_004ecf30(int x, int y);
void ShowDungeonClueDetailScreen(int dungeon_index);
int FUN_00522508(int param_1);
int FUN_005501dc(int value);
int FUN_0056a515(int player, int *cards, int count, char *title, int allow_cancel, int *out_selection);
int FUN_0056bd9d(unsigned int card_id);
int PickRandomCardMatchingTypeAndColor(unsigned int type_mask, unsigned int color_mask);
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
int GetFontCharWidth(int font_slot, char character);
ShandalarEntryType PickRandomCreatureTypeForWizardTier(int wizard_color, int creature_tier);
int PickWisemanCastleDungeonSlotForHint(void);
int ReadSpriteEntryPointers(EncodedImage **out_sprite_entries, char *sprite_path);
void ReadCsvFieldByCsvid(char *out, int csvid, int field, char *csv_name);
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
int DrawTextFormatted(FacemakerWindowBounds *dst, int text_color, int draw_shadow, int scale_to_screen, int center_x, int center_y, int x,
                      int y, int *format_and_args);
void DrawFormattedTextNoShadowCentered(FacemakerWindowBounds *dst, int text_color, int x, int y, char *format, ...);
void DrawFormattedTextShadowedCentered(FacemakerWindowBounds *window, int color_index, int x, int y, char *format, ...);
void DrawTiledDialogBoxFrame(int x, int y, int width, int height, int frame_style);
void DrawTextAt(FacemakerWindowBounds *window, int color, int x, int y, char *text, ...);
void DrawTextLineNoShadow(char *text, int x, int y, int color_index);
void FillGraphicsRect(FacemakerWindowBounds *window, int x, int y, int w, int h, int color);
void DrawUiScaledCenteredText(char *text, int x, int y, int color_index);
void DrawVisitScreenOverlays(FacemakerWindowBounds *window);
void DrawWorldUiFormattedText(FacemakerWindowBounds *window, int color_index, int x, int y, char *format, ...);
void EnsureAdvfac64Loaded(int state);
void FadeInPaletteFromGray(int color_index, int palette_id);
void FinalizeSpriteEncodeSession(void);
void FreeSpriteBlob(EncodedImage *sprite_blob);
int FreeOpeningMenuSpriteWorkEntries(int work_entry_index_a, int work_entry_index_b);
void FUN_00431526(unsigned int mask, int x, int y);
void AddJournalEntry(int entry_type, int entry_arg);
void FUN_0046ed03(void);
void FUN_0046ed33(void);
void DrawLairMenuButtonText(char *text, int x, int y);
int DrawWrappedLairText(char *text, int width_chars, int x, int y, int color);
void TruncateLairTextToWidth(char *text, int width);
void RunRandomCreatureAnteDuel(int creature_tier, int ante_card_count);
undefined4 BuildQuestStatusMessage(void);
void FUN_004ce992(int param_1);
void DrawAdventureCard(unsigned int card_id, int x, int y, int full_card, char *banner_text);
void DrawAdventureCardSized(int card_id, int x, int y, int width, int height, int full_card, char *label);
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
static char DAT_00581a74[4] = "";
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
  if (g_world_location_music_active != 0)
  {
    sound_stop(0x10);
  }
  g_world_location_music_active = 0;
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

// FUNCTION: SHANDALAR 0x00417d4d
void TruncateLairTextToWidth(char *text, int width)
{
  size_t text_len;

  while (MeasureTextLineWidth(text) > width)
  {
    text_len = strlen(text);
    if (text[text_len - 3] != ' ')
    {
      text_len = strlen(text);
      text[text_len - 2] = '.';
    }
    text_len = strlen(text);
    text[text_len - 1] = '\0';
  }
}

// FUNCTION: SHANDALAR 0x00417aff
int DrawWrappedLairText(char *text, int width_chars, int x, int y, int color)
{
  struct
  {
    int normalized_char;
    int line_height;
    size_t text_len;
    int char_index;
    int saved_char;
    int line_start_index;
    int line_pixel_width;
    int last_break_index;
  } s;

#define last_break_index s.last_break_index
#define line_pixel_width s.line_pixel_width
#define line_start_index s.line_start_index
#define saved_char s.saved_char
#define char_index s.char_index
#define text_len s.text_len
#define line_height s.line_height
#define normalized_char s.normalized_char

  last_break_index = 0;
  line_start_index = last_break_index;
  line_pixel_width = line_start_index;
  line_height = GetFontLineHeight(PTR_DAT_005832b4->font_slot);
  text_len = strlen(text);
  for (char_index = 0; char_index < (int)text_len; char_index = char_index + 1)
  {
    if (text[char_index] >= '\0')
    {
      normalized_char = text[char_index];
    }
    else
    {
      normalized_char = text[char_index] + 0x80;
    }

    line_pixel_width += GetFontCharWidth(PTR_DAT_005832b4->font_slot, normalized_char);

    if (((text[char_index] == ' ') || (text[char_index] == '\n')) || (text[char_index] == '^'))
    {
      last_break_index = char_index;
    }

    if (((text[char_index] == '\n') || (text[char_index] == '^')) || (line_pixel_width > width_chars * 8))
    {
      saved_char = text[last_break_index];
      text[last_break_index] = '\0';
      if (10 < line_height)
      {
        strcpy(text + 0x200, text);
        TruncateLairTextToWidth(text + line_start_index, (global_screen_width - 2) - x);
      }
      DrawTextLineNoShadow(text + line_start_index, x, y, color);
      if (10 < line_height)
      {
        strcpy(text, text + 0x200);
      }
      text[last_break_index] = saved_char;
      y = y + line_height;
      line_pixel_width = 0;
      line_start_index = last_break_index + 1;
      if (text[char_index] == '^')
      {
        line_start_index++;
      }
      char_index = last_break_index;
      if (global_screen_height - 8 < y)
      {
        break;
      }
    }
  }

  if ((line_pixel_width <= 0) || (global_screen_height - 8 < y))
  {
    return 0;
  }
  else
  {
    DrawTextLineNoShadow(text + line_start_index, x, y, color);
    y = y + line_height;
  }

  return y;

#undef last_break_index
#undef line_pixel_width
#undef line_start_index
#undef saved_char
#undef char_index
#undef text_len
#undef line_height
#undef normalized_char
}

// FUNCTION: SHANDALAR 0x00426e30
void DrawLairMenuButtonText(char *text, int x, int y)
{
  int button_width;

  x <<= 1;
  y <<= 1;
  button_width = MeasureTextLineWidth(text) + 0x10;
  FillGraphicsRect(PTR_DAT_005832b4, (x - button_width / 2) - 1, y - 5, button_width + 1, 0x13, 0xff);
  FillGraphicsRect(PTR_DAT_005832b4, x - button_width / 2, y - 4, button_width + 1, 0x12, 0xf4);
  FillGraphicsRect(PTR_DAT_005832b4, x - button_width / 2, y - 4, button_width, 0x11, 0xf6);
  DrawCenteredTextLineClamped(text, x, y, 0xe3);
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
void AppendDuelVictoryLogEntry(int encounter_type, int monster_color)
{
  int log_index;

  for (log_index = 0; log_index < 1000; log_index = log_index + 1)
  {
    if ((char)g_duel_victory_log[log_index] == '\0')
    {
      g_duel_victory_log[log_index] = monster_color * '\x10' + encounter_type;
      return;
    }
  }
}

// FUNCTION: SHANDALAR 0x00430f69
void DrawScaledCenteredTextNoShadow(char *text, int center_x, int y, int color_index)
{
  center_x = (center_x * global_screen_width) / 0x140;
  y = (global_screen_height * y) / 0xf0;
  DrawCenteredTextLineClamped(text, center_x, y, color_index);
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
  undefined4 uVar1 = g_player_is_male;

  if (uVar1 == 0)
  {
    return 1;
  }
  else if (uVar1 == 1)
  {
    return 0;
  }
  else if (uVar1 == 2)
  {
    return 1;
  }
  else if (uVar1 == 3)
  {
    return 0;
  }
  else if (uVar1 == 4)
  {
    return 1;
  }
  else if (uVar1 == 5)
  {
    return 0;
  }
  else if (uVar1 == 6)
  {
    return 1;
  }
  else if (uVar1 == 7)
  {
    return 0;
  }
  else if (uVar1 == 8)
  {
    return 1;
  }
  else if (uVar1 == 9)
  {
    return 0;
  }
  else if (uVar1 == 10)
  {
    return 0;
  }
  else if (uVar1 == 0xb)
  {
    return 1;
  }
  else if (uVar1 == 0xc)
  {
    return 0;
  }
  else if (uVar1 == 0xd)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

// FUNCTION: SHANDALAR 0x0042811a
void ShowCardImageDialog(int card_index, int text_color, char *button_text, int unused)
{
  struct
  {
    char button_text_copy[100];
    int text_height;
    EncodedImage *button_sprites[100];
  } s;

  if (DAT_008bd200 == 1)
  {
  }
  else
  {
    FUN_0046ed33();
    strcpy(s.button_text_copy, button_text);
    ReadSpriteEntryPointers(s.button_sprites, s_dungbutt_spr_00581a64);
    DrawEncodedImageResampled(PTR_DAT_005832b4, FUN_005501dc(0xdc) / 2, FUN_005501dc(0x34) / 2,
                              FUN_005501dc(0xc5) / 2, FUN_005501dc(0x10f) / 2, s.button_sprites[0]);
    DrawAdventureCardSized(card_index, 0x7a, 0x29, 0x4b, 0x70, 1, DAT_00581a74);
    FreeSpriteBlob(s.button_sprites[0]);
    if (0)
    {

      if (unused != 0)
      {
        DrawLairMenuButtonText("OK", 0xc1, 0x88);
      }

      PTR_DAT_005832b4->font_slot = 2;
      if ((char)global_cards_data[card_index].cc[1] == -1)
      {
        strcpy(g_ui_message_buffer, "X is ");
        strcat(g_ui_message_buffer, _itoa(x_value, DAT_0093a870, 10));
        DrawTextLineNoShadow(g_ui_message_buffer, global_screen_width / 2 - 0x60, 0xb0, 0);
      }

      PTR_DAT_005832b4->font_slot = 3;
      switch (FUN_0056c5ea(card_index))
      {
      case 1:
        strcpy(g_ui_message_buffer, "Common");
        break;
      case 2:
        strcpy(g_ui_message_buffer, "Uncommon");
        break;
      case 3:
        strcpy(g_ui_message_buffer, "Rare");
        break;
      }
      DrawTextLineNoShadow(g_ui_message_buffer, global_screen_width / 2 + 0x35, 0x6e, 0);

      switch (global_cards_data[card_index].expansion)
      {
      case 1:
        strcpy(g_ui_message_buffer, "4th edition");
        break;
      case 2:
        strcpy(g_ui_message_buffer, "Unlimited");
        break;
      case 4:
        strcpy(g_ui_message_buffer, "Arabian Nights");
        break;
      case 8:
        strcpy(g_ui_message_buffer, "Antiquities");
        break;
      case 0x40:
        strcpy(g_ui_message_buffer, "Astral");
        break;
      }
      DrawTextLineNoShadow(g_ui_message_buffer, global_screen_width / 2 + 0x35, 0x6e, 0);

      strcpy(g_ui_message_buffer, (char *)GetQuestCardClassName(global_cards_data[card_index].type));
      DrawTextLineNoShadow(g_ui_message_buffer, global_screen_width / 2 + 0x35, 0x66, 0);

      PTR_DAT_005832b4->font_slot = 2;
      ReadCsvFieldByCsvid(g_ui_message_buffer, global_cards_data[card_index].id, 5, "master.csv");
      s.text_height = DrawWrappedLairText(g_ui_message_buffer, 0x18, global_screen_width / 2 - 0x62, 0xb8, 0);
      PTR_DAT_005832b4->font_slot = 3;
      ReadCsvFieldByCsvid(g_ui_message_buffer, global_cards_data[card_index].id, 6, "master.csv");
      DrawWrappedLairText(g_ui_message_buffer, 0x18, global_screen_width / 2 - 0x62, s.text_height + 4, 3);
    }

    PTR_DAT_005832b4->font_slot = 4;
    s.text_height = MeasureTextLineWidth(s.button_text_copy);
    DrawCenteredTextLineClamped(s.button_text_copy, global_screen_width / 2,
                                (ScaleUiCoordinate(0x52) - GetFontLineHeight(PTR_DAT_005832b4->font_slot)) + -2, 0);
    DrawCenteredTextLineClamped(s.button_text_copy, global_screen_width / 2,
                                (ScaleUiCoordinate(0x52) - GetFontLineHeight(PTR_DAT_005832b4->font_slot)) + -3, text_color);
    FUN_0046ed03();
  }
}

// FUNCTION: SHANDALAR 0x00450925
void FUN_00450925(uint param_1, int param_2, int param_3, uint param_4, undefined4 param_5, char *param_6)
{
  int local_c;
  int local_8;

  (void)param_4;
  (void)param_5;
  (void)param_6;

  local_c = param_2;
  local_8 = param_3;

  if (IsWindowVisible(DAT_00896714))
  {
    if ((param_2 != -1) && (param_3 != -1))
    {
      SendMessageA(DAT_00896714, 0x401, CardIDFromType(param_1), &local_c);
    }
    else
    {
      SendMessageA(DAT_00896714, 0x401, CardIDFromType(param_1), 0);
    }
  }
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
    ShowCardImageDialog(param_1, param_2, param_3, param_4);
  }
}

// FUNCTION: SHANDALAR 0x005307dd
void RunRandomCreatureAnteDuel(int creature_tier, int ante_card_count)
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

  for (local_c = 0; local_c < ante_card_count; local_c = local_c + 1)
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
    DrawAdventureCard(uVar1, (int)(0x96 / (longlong)ante_card_count) * local_c + 100,
                      (int)(10 / (longlong)ante_card_count) * local_c + 100, 1, DAT_0058f04c);
  }
  do
  {
    local_8 = PickRandomCreatureTypeForWizardTier(0, creature_tier);
  } while (0x36 < local_8);
  if (creature_tier == 0xd)
  {
    pcVar4 = BuildCreatureNameWithArticle(local_8);
    sprintf(g_ui_message_buffer, gs_monsterlair_0074cff0[0], pcVar4);
  }
  else if (creature_tier == 0x10)
  {
    pcVar4 = BuildCreatureNameWithArticle(local_8);
    sprintf(g_ui_message_buffer, gs_monsterlair_0074cff0[1], pcVar4);
  }
  else if (creature_tier == 0x12)
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
    uVar1 = FUN_0056c705(gs_creature_names_00591a08[local_8].deck_number);
    LoadCreatureDuelDeck(local_8, uVar1, 0, -1);
    DAT_008ce538 = GetFirstManaColorIndex((int)gs_creature_names_00591a08[local_8].color_mask);
    DAT_00742fd0 = 0;
    DAT_007a7874 = 3;
    ANTE_CARD(0, 0) = -1;
    for (local_c = 0; local_c < ante_card_count; local_c = local_c + 1)
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
      for (local_c = 0; local_c < ante_card_count; local_c = local_c + 1)
      {
        iVar2 = (int)(300 / (longlong)(ante_card_count + 1));
        iVar5 = FUN_0056c5ea(auStack_20[local_c]);
        strcpy(g_ui_message_buffer, gs_browse_0074da20[iVar5 + 0xc]);
        pcVar4 = g_ui_message_buffer;
        full_card = 1;
        iVar5 = FUN_00522508(10);
        DrawAdventureCard(auStack_20[local_c], (iVar2 * local_c + 0x6f) - ((ante_card_count + -1) * iVar2) / 2,
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
bool RunCardRiddleChallenge(void)
{
  struct
  {
    int landwalk_index;   // ebp - 0x50
    int attempts;         // ebp - 0x4c
    uint selected_card;   // ebp - 0x48
    bool candidate_ok;    // ebp - 0x44
    int riddle_type;      // ebp - 0x40
    int duplicate_scan;   // ebp - 0x3c
    int menu_selection;   // ebp - 0x38
    int clue_count;       // ebp - 0x34
    uint choice_cards[8]; // ebp - 0x30
    int choice_count;     // ebp - 0x10
    int numeric_answer;   // ebp - 0xc
    int correct_answer;   // ebp - 0x8
    uint decoy_card;      // ebp - 0x4
  } s;

  s.choice_count = 5;
  memset(s.choice_cards, 0xff, 0x20);

pick_riddle_type:
  s.attempts = 0;
  switch (FUN_00522508(10))
  {
  case 0:
    s.riddle_type = 0;
    break;
  case 1:
    s.riddle_type = 1;
    break;
  case 2:
    s.riddle_type = 2;
    break;
  case 3:
    s.riddle_type = 3;
    break;
  case 4:
    s.riddle_type = 4;
    break;
  case 5:
    s.riddle_type = 5;
    break;
  case 6:
    s.riddle_type = 6;
    break;
  case 7:
    s.riddle_type = 7;
    break;
  case 8:
    s.riddle_type = 8;
    break;
  case 9:
    s.riddle_type = 2;
    break;
  }

find_riddle_candidate:
  do
  {
    s.selected_card = FUN_00522508(g_card_count - 0x39);
  } while (((global_cards_data[s.selected_card].type & 2) == 0 && s.riddle_type != 2) ||
           (global_cards_data[s.selected_card].expansion & 0xc1) == 0);

  s.candidate_ok = false;
  switch (s.riddle_type)
  {
  case 0:
    if ((0 < global_cards_data[s.selected_card].power) && (global_cards_data[s.selected_card].power < 0x4000))
    {
      s.candidate_ok = true;
    }
    break;
  case 1:
    if ((0 < global_cards_data[s.selected_card].toughness) &&
        (global_cards_data[s.selected_card].toughness < 0x4000))
    {
      s.candidate_ok = true;
    }
    break;
  case 2:
    if (('\0' < (char)global_cards_data[s.selected_card].cc[0]) &&
        ((char)global_cards_data[s.selected_card].cc[1] >= 0))
    {
      s.candidate_ok = true;
    }
    break;
  case 3:
    if ((global_cards_data[s.selected_card].static_ability & KEYWORD_FLYING) != 0)
    {
      s.candidate_ok = true;
    }
    break;
  case 4:
    if ((global_cards_data[s.selected_card].static_ability & KEYWORD_BASIC_LANDWALK) != 0)
    {
      s.candidate_ok = true;
    }
    break;
  case 5:
    if ((global_cards_data[s.selected_card].static_ability & KEYWORD_REGENERATION) != 0)
    {
      s.candidate_ok = true;
    }
    break;
  case 6:
    if ((global_cards_data[s.selected_card].static_ability & KEYWORD_BANDING) != 0)
    {
      s.candidate_ok = true;
    }
    break;
  case 7:
    if ((global_cards_data[s.selected_card].static_ability & KEYWORD_FIRST_STRIKE) != 0)
    {
      s.candidate_ok = true;
    }
    break;
  case 8:
    if ((global_cards_data[s.selected_card].static_ability & KEYWORD_TRAMPLE) != 0)
    {
      s.candidate_ok = true;
    }
    break;
  }

  s.attempts = s.attempts + 1;
  if (s.candidate_ok == 0)
  {
    if (s.attempts <= 3)
    {
      goto find_riddle_candidate;
    }

    goto pick_riddle_type;
  }

  if (FUN_00522508(4) < g_shandalar_difficulty)
  {
    s.numeric_answer = -1;
    switch (s.riddle_type)
    {
    case 0:
      sprintf(g_ui_message_buffer, gs_riddle_0077cf20[0], global_cards_data[s.selected_card].name);
      s.numeric_answer = (int)global_cards_data[s.selected_card].power;
      break;
    case 1:
      sprintf(g_ui_message_buffer, gs_riddle_0077cf20[1], global_cards_data[s.selected_card].name);
      s.numeric_answer = (int)global_cards_data[s.selected_card].toughness;
      break;
    case 2:
      sprintf(g_ui_message_buffer, gs_riddle_0077cf20[2], global_cards_data[s.selected_card].name);
      s.numeric_answer = (int)(char)global_cards_data[s.selected_card].cc[0] +
                         (int)(char)global_cards_data[s.selected_card].cc[1];
      break;
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
      sprintf(g_ui_message_buffer, gs_riddle_0077cf20[3], global_cards_data[s.selected_card].name);
      strcat(g_ui_message_buffer, gs_riddle_0077cf20[4]);
      break;
    }

    if (s.numeric_answer != -1)
    {
      strcat(g_ui_message_buffer, s___0_1_2_3_4_5_6_7_8__0058f180);
      s.correct_answer = ClampIntToRange(s.numeric_answer, 0, 8);
      s.menu_selection = RunTextMenuAt(g_ui_message_buffer, 0x50, 100);
    }
    else
    {
      s.menu_selection = RunTextMenuAt(g_ui_message_buffer, 0x50, 100);
      if ((global_cards_data[s.selected_card].static_ability &
           (keyword_t)(1 << (byte)s.menu_selection)) != 0)
      {
        s.correct_answer = s.menu_selection;
      }
      else
      {
        s.correct_answer = 99;
      }
    }
    EnsureAdvfac64Loaded(1);
    if (s.correct_answer == s.menu_selection)
    {
      FUN_005584d5(s.selected_card, 0xce, gs_riddle_0077cf20[5], 1);
    }
    else
    {
      FUN_005584d5(s.selected_card, 0xbc, gs_riddle_0077cf20[6], 1);
    }
    WaitForInputEventUnlessBlocked();
    return (s.correct_answer == s.menu_selection);
  }

  for (s.clue_count = 0; s.clue_count < s.choice_count; s.clue_count++)
  {
  find_decoy_card:
    do
    {
      s.decoy_card = FUN_00522508(g_card_count - 0x39);
    } while (((global_cards_data[s.decoy_card].type & 2) == 0 && s.riddle_type != 2) ||
             (global_cards_data[s.decoy_card].expansion & 0xc1) == 0);

    s.candidate_ok = false;
    switch (s.riddle_type)
    {
    case 0:
      if (global_cards_data[s.decoy_card].power != global_cards_data[s.selected_card].power)
      {
        s.candidate_ok = 1;
      }
      break;
    case 1:
      if (global_cards_data[s.decoy_card].toughness != global_cards_data[s.selected_card].toughness)
      {
        s.candidate_ok = 1;
      }
      break;
    case 2:
      if (global_cards_data[s.decoy_card].color == global_cards_data[s.selected_card].color)
      {
        if (((global_cards_data[s.decoy_card].cc[0] != global_cards_data[s.selected_card].cc[0] ||
              (global_cards_data[s.decoy_card].color != global_cards_data[s.selected_card].color))))
        {
          s.candidate_ok = true;
        }
      }
      break;
    case 3:
      if ((global_cards_data[s.decoy_card].static_ability & KEYWORD_FLYING) == 0)
      {
        s.candidate_ok = 1;
      }
      break;
    case 4:
      if ((global_cards_data[s.decoy_card].static_ability &
           global_cards_data[s.selected_card].static_ability & KEYWORD_BASIC_LANDWALK) == 0)
      {
        s.candidate_ok = 1;
      }
      break;
    case 5:
      if ((global_cards_data[s.decoy_card].static_ability & KEYWORD_REGENERATION) == 0)
      {
        s.candidate_ok = 1;
      }
      break;
    case 6:
      if ((global_cards_data[s.decoy_card].static_ability & KEYWORD_BANDING) == 0)
      {
        s.candidate_ok = 1;
      }
      break;
    case 7:
      if ((global_cards_data[s.decoy_card].static_ability & KEYWORD_FIRST_STRIKE) == 0)
      {
        s.candidate_ok = 1;
      }
      break;
    case 8:
      if ((global_cards_data[s.decoy_card].static_ability & KEYWORD_TRAMPLE) == 0)
      {
        s.candidate_ok = 1;
      }
      break;
    }

    for (s.duplicate_scan = 0; s.duplicate_scan < s.clue_count; s.duplicate_scan = s.duplicate_scan + 1)
    {
      if (s.choice_cards[s.duplicate_scan] == s.decoy_card)
      {
        s.candidate_ok = false;
      }
    }

    if (s.candidate_ok == 0)
    {
      goto find_decoy_card;
    }

    s.choice_cards[s.clue_count] = s.decoy_card;
  }

  s.correct_answer = FUN_00522508(s.choice_count);
  s.choice_cards[s.correct_answer] = s.selected_card;
  switch (s.riddle_type)
  {
  case 0:
    sprintf(g_ui_message_buffer, gs_riddle_0077cf20[7], (int)global_cards_data[s.selected_card].power);
    break;
  case 1:
    sprintf(g_ui_message_buffer, gs_riddle_0077cf20[8], (int)global_cards_data[s.selected_card].toughness);
    break;
  case 2:
    FormatMessageFromStringStripCarriageReturns(g_ui_message_buffer, 0x1000, gs_riddle_0077cf20[9],
                                                (int)(char)global_cards_data[s.selected_card].cc[0],
                                                gs_mananames_0074d980[GetFirstManaColorIndex((int)global_cards_data[s.selected_card].color)],
                                                (int)(char)global_cards_data[s.selected_card].cc[1]);
    break;
  case 3:
    strcpy(g_ui_message_buffer, gs_riddle_0077cf20[10]);
    break;
  case 4:
    if ((global_cards_data[s.selected_card].static_ability & KEYWORD_SWAMPWALK) != 0)
    {
      s.landwalk_index = 1;
    }
    if ((global_cards_data[s.selected_card].static_ability & KEYWORD_MOUNTAINWALK) != 0)
    {
      s.landwalk_index = 4;
    }
    if ((global_cards_data[s.selected_card].static_ability & KEYWORD_ISLANDWALK) != 0)
    {
      s.landwalk_index = 2;
    }
    if ((global_cards_data[s.selected_card].static_ability & KEYWORD_FORESTWALK) != 0)
    {
      s.landwalk_index = 3;
    }
    if ((global_cards_data[s.selected_card].static_ability & KEYWORD_PLAINSWALK) != 0)
    {
      s.landwalk_index = 5;
    }
    sprintf(g_ui_message_buffer, gs_riddle_0077cf20[0xb], gs_landwalks_007806f0[s.landwalk_index]);
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
    break;
  }

  for (s.clue_count = 0; s.clue_count < s.choice_count; s.clue_count = s.clue_count + 1)
  {
    strcat(g_ui_message_buffer, DAT_0058f1a0);
    strcat(g_ui_message_buffer, global_cards_data[s.choice_cards[s.clue_count]].name);
    strcat(g_ui_message_buffer, DAT_0058f1a4);
  }
  s.menu_selection = RunTextMenuAt(g_ui_message_buffer, 0x50, 100);
  EnsureAdvfac64Loaded(1);
  if (s.correct_answer == s.menu_selection)
  {
    FUN_005584d5(s.choice_cards[s.correct_answer], 0xce, gs_riddle_0077cf20[5], 1);
  }
  else
  {
    FUN_005584d5(s.choice_cards[s.correct_answer], 0xbc, gs_riddle_0077cf20[0x10], 1);
  }
  WaitForInputEventUnlessBlocked();

  return s.menu_selection == s.correct_answer;
}

// FUNCTION: SHANDALAR 0x0052fae0
void RunLairExplorationEvent(int color)
{
  struct
  {
    int event_loop_state;
    int event_random_flag;
    int event_type;
    int card_or_deck_index;
    int menu_selection;
    int i;
    int reward_card;
    int reward_cards[4];
    int hint_dungeon_index;
    int copied_creature_type;
  } s;

  FUN_004ed7c8(PTR_DAT_0058edb8[color]);
  DrawVisitScreenOverlays(PTR_DAT_005832b4);

  for (s.reward_card = 1; s.reward_card <= 3; s.reward_card = s.reward_card + 1)
  {
    do
    {
      if (FUN_00522508(3) != 0)
      {
        s.card_or_deck_index = PickRandomCardMatchingTypeAndColor(0x3f, 1 << (byte)color);
      }
      else
      {
        s.card_or_deck_index = PickRandomCardMatchingTypeAndColor(0x40, 1);
      }
    } while ((FUN_0056c5ea(s.card_or_deck_index) != s.reward_card) ||
             (FUN_004bb1cf(s.card_or_deck_index) < 0) ||
             ((global_cards_data[s.card_or_deck_index].extra_ability & 0x900) != 0) ||
             (GetCardAvailabilityMask(s.card_or_deck_index) == 0));

    if ((s.reward_card == 1) && (FUN_00522508(2) != 0))
    {
      s.card_or_deck_index = color - 1;
    }
    s.reward_cards[s.reward_card] = s.card_or_deck_index;
  }

  do
  {
    do
    {
      s.card_or_deck_index = FUN_00522508(500);
    } while (deck[s.card_or_deck_index] == -1);
  } while ((FUN_0056c5ea(deck[s.card_or_deck_index] & 0xfff) <= 1) ||
           (GetCardAvailabilityMask(s.card_or_deck_index) == 0));

  s.reward_card = s.reward_cards[3];
  EnsureAdvfac64Loaded(1);

retry:
  if (((g_monster_timer >> 2) % 3) != 0)
  {
    s.event_type = color - 1;
    if (FUN_00522508(2) != 0)
    {
      s.reward_card = color - 1;
    }
    else
    {
      do
      {
        do
        {
          s.reward_card = FUN_00522508(g_card_count - 0x39);
        } while (FUN_0056c0e5((int)global_cards_data[s.reward_card].color, 1 << (byte)color, 1) == 0);
      } while (((global_cards_data[s.reward_card].extra_ability & 0x100) != 0) ||
               (GetCardAvailabilityMask(s.reward_card) == 0));
    }

    goto end;
  }
  else
  {
    s.event_type = FUN_00522508(0xe) + 5;
    AddJournalEntry(JOURNAL_ENTRY_RANDOM_EVENT, s.event_type);
  }

  s.event_random_flag = FUN_00522508(2);
  if ((s.event_type == 6) || (s.event_type == 0x11))
  {
    s.event_random_flag = 0;
  }

  if ((g_next_duel_card_id != -1) && ((s.event_type == 0xb) || (s.event_type == 0x11)))
  {
    goto retry;
  }

  sprintf(g_ui_message_buffer, gs_lair_0077e180[0], FUN_005307c3(s.event_type));
  ClearInputAndWaitForMouseRelease();
  RunTextMenuAt(g_ui_message_buffer, 0x5a, 100);

  s.menu_selection = 2;
  s.event_loop_state = s.menu_selection;
  for (; s.event_loop_state == 2;)
  {
    s.reward_card = -1;
    if (FUN_00522508(4) == 0)
    {
      s.reward_card = FUN_00530770();
    }

    switch (s.event_type)
    {
    case 5:
      s.hint_dungeon_index = PickWisemanCastleDungeonSlotForHint();
      if (s.hint_dungeon_index != -1)
      {
        RevealRandomCastleDungeonClue(s.hint_dungeon_index);
        break;
      }
    case 6:
      if (RunCardRiddleChallenge())
      {
        s.reward_card = s.reward_cards[3];
      }
      break;
    case 7:
      if ((FUN_00522508(2) != 0) || (g_siege_timer < 0x80))
      {
        PlaySoundEffectOnChannel(s_x_sound_treasure_wav_0058ee48, 0xf, 100, 100, 0);
        strcpy(g_ui_message_buffer, gs_lair_0077e180[1]);
        Gold = Gold + 500;
        s.reward_card = -1;
      }
      else
      {
        strcpy(g_ui_message_buffer, gs_lair_0077e180[2]);
        Gold = Gold / 2;
        s.reward_card = FUN_00530770();
      }
      RunTextMenuAt(g_ui_message_buffer, 0x5a, 100);
      break;
    case 8:
      do
      {
      } while (BuyAnyCardFromTown(-1, -1) != 0);
      break;
    case 9:
      PlaySoundEffectOnChannel(s_x_Duelsounds_aswanjag_wav_0058ee60, 0x97, 100, 100, 0);
      if (g_siege_timer < 0x100)
      {
        RunRandomCreatureAnteDuel(8, 2);
      }
      else
      {
        RunRandomCreatureAnteDuel(0xd, 3);
      }
      break;
    case 10:
      PlaySoundEffectOnChannel(s_x_Duelsounds_aswanjag_wav_0058ee7c, 0x97, 100, 100, 0);
      RunRandomCreatureAnteDuel(0x12, 4);
      break;
    case 0xb:
      g_next_duel_card_id = FUN_0056c705(0x1b4);
      PlaySoundEffectOnChannel(s_x_Duelsounds_aswanjag_wav_0058ee98, 0x97, 100, 100, 0);
      RunRandomCreatureAnteDuel(0xd, 2);
      break;
    case 0xc:
      PlaySoundEffectOnChannel(s_x_sound_treasure_wav_0058eeb4, 0xf, 100, 100, 0);
      do
      {
        strcpy(g_ui_message_buffer, gs_lair_0077e180[3]);
        strcat(g_ui_message_buffer, gs_lair_0077e180[4]);
        for (s.i = 0; s.i < 5; s.i = s.i + 1)
        {
          sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), gs_lair_0077e180[5],
                  gs_amuletnames_0077d090[s.i + 1], g_amulet_inventory[s.i]);
        }
        s.menu_selection = RunTextMenuAt(g_ui_message_buffer, 0x5a, 100);
        if ((s.menu_selection > 0) && (Gold >= 200))
        {
          Gold -= 200;
          g_amulet_inventory[s.menu_selection - 1]++;
        }
      } while (Gold >= 200);
      break;
    case 0xd:
      s.copied_creature_type = FUN_00522508(gs_creature_name_count_00593934 - 3) + 1;
      gs_creature_names_00591a08[gs_creature_name_count_00593934 - 1].tier = '\x10';
      gs_creature_names_00591a08[gs_creature_name_count_00593934 - 1].deck_number =
          gs_creature_names_00591a08[s.copied_creature_type].deck_number;
      strcpy(gs_creature_names_00591a08[gs_creature_name_count_00593934 - 1].name,
             gs_creature_names_00591a08[s.copied_creature_type].name);
      strcpy(gs_creature_names_00591a08[gs_creature_name_count_00593934 - 1].article,
             gs_creature_names_00591a08[s.copied_creature_type].article);
      strcpy(gs_creature_names_00591a08[gs_creature_name_count_00593934 - 1].plural,
             gs_creature_names_00591a08[s.copied_creature_type].plural);
      gs_creature_names_00591a08[gs_creature_name_count_00593934 - 1].gender =
          gs_creature_names_00591a08[s.copied_creature_type].gender;
      PlaySoundEffectOnChannel(s_x_Duelsounds_aswanjag_wav_0058eecc, 0x97, 100, 100, 0);
      RunRandomCreatureAnteDuel(0x10, 3);
      break;
    case 0xe:
      if (RunCardRiddleChallenge())
      {
        s.reward_card = FUN_00530770();
      }
      break;
    case 0xf:
      if ((FUN_00522508(2) != 0) || (g_siege_timer < 0x80))
      {
        PlaySoundEffectOnChannel(s_x_sound_treasure_wav_0058eee8, 0xf, 100, 100, 0);
        strcpy(g_ui_message_buffer, gs_lair_0077e180[6]);
        for (s.i = 0; s.i < 5; s.i = s.i + 1)
        {
          g_amulet_inventory[s.i] = g_amulet_inventory[s.i] + 1;
        }
        s.reward_card = -1;
      }
      else
      {
        strcpy(g_ui_message_buffer, gs_lair_0077e180[7]);
        for (s.i = 0; s.i < 5; s.i = s.i + 1)
        {
          g_amulet_inventory[s.i] = (g_amulet_inventory[s.i] + 1) / 2;
        }
        s.reward_card = FUN_00530770();
      }
      RunTextMenuAt(g_ui_message_buffer, 0x5a, 100);
      break;
    case 0x10:
      s.card_or_deck_index = 0;
      do
      {
        s.i = FUN_00522508(5);
      } while (g_amulet_inventory[s.i] == 0 && ++s.card_or_deck_index < 99);
      if (g_amulet_inventory[s.i] != 0)
      {
        FormatMessageFromStringStripCarriageReturns(g_ui_message_buffer, 0x1000, gs_lair_0077e180[8],
                                                    gs_amuletnames_0077d090[s.i + 1], gs_lair_0077e180[color + 8]);
        strcat(g_ui_message_buffer, gs_lair_0077e180[0xe]);
        s.menu_selection = RunTextMenuAt(g_ui_message_buffer, 0x5a, 100);
        if (s.menu_selection == 1)
        {
          g_amulet_inventory[s.i] = g_amulet_inventory[s.i] - 1;
          WisemanChooseLairCreatureAndSetupDuel((byte)color);
        }
      }
      break;
    case 0x11:
      sprintf(g_ui_message_buffer, gs_lair_0077e180[0xf],
              global_cards_data[deck[s.card_or_deck_index] & 0xfffU].name);
      strcat(g_ui_message_buffer, gs_lair_0077e180[0x10]);
      if (RunTextMenuAt(g_ui_message_buffer, 100, 100) == 1)
      {
        RemoveDeckSlot(s.card_or_deck_index);
        g_next_duel_card_id = 5;
      }
      break;
    case 0x12:
      do
      {
      } while (BuyAnyCardFromTown(0, -1) != 0);
      RecountDeckCardTotals();
      break;
    }
    break;
  }

end:
  if (s.reward_card != -1)
  {
    if ((global_cards_data[s.reward_card].type & 2) != 0)
    {
      PlaySoundEffectOnChannel(s_x_DuelSounds_summon_wav_0058ef00, 0xf, 100, 100, 0);
    }
    else if ((global_cards_data[s.reward_card].type & 0x40) != 0)
    {
      PlaySoundEffectOnChannel(s_x_DuelSounds_artifact_wav_0058ef18, 0xf, 100, 100, 0);
    }
    else if ((global_cards_data[s.reward_card].type & 4) != 0)
    {
      PlaySoundEffectOnChannel(s_x_DuelSounds_enchant_wav_0058ef34, 0xf, 100, 100, 0);
    }
    else if ((global_cards_data[s.reward_card].type & 0x10) != 0)
    {
      PlaySoundEffectOnChannel(s_x_DuelSounds_instant_wav_0058ef50, 0xf, 100, 100, 0);
    }
    else if ((global_cards_data[s.reward_card].type & 0x20) != 0)
    {
      PlaySoundEffectOnChannel(s_x_DuelSounds_interupt_wav_0058ef6c, 0xf, 100, 100, 0);
    }
    else if ((global_cards_data[s.reward_card].type & 8) != 0)
    {
      PlaySoundEffectOnChannel(s_x_DuelSounds_sorcery_wav_0058ef88, 0xf, 100, 100, 0);
    }
    else if ((global_cards_data[s.reward_card].type & 1) != 0)
    {
      switch ((int)global_cards_data[s.reward_card].color)
      {
      case 2:
        PlaySoundEffectOnChannel(s_x_DuelSounds_black_wav_0058efa4, 0xf, 100, 100, 0);
        break;
      case 4:
        PlaySoundEffectOnChannel(s_x_DuelSounds_blue_wav_0058efbc, 0xf, 100, 100, 0);
        break;
      case 8:
        PlaySoundEffectOnChannel(s_x_DuelSounds_green_wav_0058efd4, 0xf, 100, 100, 0);
        break;
      case 0x10:
        PlaySoundEffectOnChannel(s_x_DuelSounds_red_wav_0058efec, 0xf, 100, 100, 0);
        break;
      case 0x20:
        PlaySoundEffectOnChannel(s_x_DuelSounds_white_wav_0058f004, 0xf, 100, 100, 0);
        break;
      default:
        PlaySoundEffectOnChannel(s_x_DuelSounds_grey_wav_0058f01c, 0xf, 100, 100, 0);
        break;
      }
    }
    else
    {
      PlaySoundEffectOnChannel(s_x_sound_treasure_wav_0058f034, 0xf, 100, 100, 0);
    }

    ClearInputAndWaitForMouseRelease();
    FUN_005584d5(s.reward_card, 0xd0, gs_lair_0077e180[0x12], 1);
    s.card_or_deck_index = FUN_0056bd9d(s.reward_card);
    if (s.card_or_deck_index != -1)
    {
      deck[s.card_or_deck_index] = deck[s.card_or_deck_index] | 0x4000;
    }
    WaitForInputEventUnlessBlocked();
  }
}

// FUNCTION: SHANDALAR 0x0054c6ba
undefined4 BuildQuestStatusMessage(void)
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
  if (g_lair_or_monster_slots[7].entry_type != SHANDALAR_ENTRY_NONE)
  {
    iVar1 = FindNearestTownIndex((int)(g_lair_or_monster_slots[7].world_x +
                                       (g_lair_or_monster_slots[7].world_x >> 0x1f & 0x1fU)) >>
                                     5,
                                 (int)(g_lair_or_monster_slots[7].world_y +
                                       (g_lair_or_monster_slots[7].world_y >> 0x1f & 0x1fU)) >>
                                     5);
    BuildTownDisplayName(iVar1);
    FUN_00561441(g_lair_or_monster_slots[7].entry_type);
    FormatMessageFromStringStripCarriageReturns(g_ui_message_buffer, 0x1000, gs_queststatus_0077e0a0[0]);
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
  if ((g_reveal_all_world_info == 0) && (g_current_quest_destination != -1))
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
        FormatMessageFromStringStripCarriageReturns(g_ui_message_buffer, 0x1000, gs_queststatus_0077e0a0[0x12]);
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
int RunWorldLairMonsterEncounter(int slot_index, int monster_color)
{
  struct
  {
    int iVar7;             // ebp - 0x90c
    int iVar8;             // ebp - 0x908
    int scratch_values[7]; // ebp - 0x904
    int reward_text_x;
    int reward_text_color;
    int reward_text_y;
    int duel_prompt_done;
    int duel_prompt_choice;
    int bargain_offer_type;
    int bargain_payment_type;
    int requested_card_count;
    int requested_gold;
    int requested_card_tier;
    char *nearest_town_name;
    int slot_tile_y;
    int slot_tile_x;
    int wizard_home_y;
    int wizard_home_x;
    EncodedImage *mage_name_plaque_sprite;
    EncodedImage *preduel_card_frame_right;
    char *preduel_background_paths[5];
    EncodedImage *preduel_bottom_left_sprite;
    int preduel_bottom_x;
    EncodedImage *preduel_card_frame_left;
    EncodedImage *creature_name_plaque_sprite;
    EncodedImage *preduel_bottom_right_sprite;
    EncodedImage *duel_pool_panel_sprite;
    int menu_option_count;
    int reward_card_spacing;
    uint selected_card_id;
    uint allow_random_reward;
    void *saved_town_dialog_callback;
    uint reward_card_ids[10];
    int duel_setup_allowed;
    int ante_card_slot;
    int creature_strength;
    uint ante_card_id;
    uint deck_or_card_index;
    int loop_index;
    uint menu_choice;              // ebp - 0x824
    int world_magic_ante_used;     // ebp - 0x820
    int text_y;                    // ebp - 0x81c
    uint bribe_option_index;       // ebp - 0x818
    uint concede_option_index;     // ebp - 0x814
    uint world_magic_option_index; // ebp - 0x810
    int menu_alignment_padding_1;
    int menu_alignment_padding_2;
    int victory_count;
    int world_magic_list_count;
    int duel_special_rules;
    int random_tile_x;                // ebp - 0x7f8
    int ante_card_count;              // ebp - 0x7f4
    int bribe_gold_cost;              // ebp - 0x7f0
    int encounter_relation_type;      // ebp - 0x7ec
    undefined4 font_slot;             // ebp - 0x7e8
    int hint_dungeon_index;           // ebp - 0x7e4
    uint temp_deck[500];              // ebp - 0x7e0
    ShandalarEntryType creature_type; // ebp - 0x10
    uint reward_flags;                // ebp - 0xc
    int prompt_counter;               // ebp - 0x8
    int duel_result;                  // ebp - 0x4
  } s;

#ifdef MODERN_FIXES
  s.duel_result = 0;
#endif

  s.saved_town_dialog_callback = g_town_dialog_callback;
  FUN_00562893();
  if (g_lair_or_monster_slots[slot_index].entry_type == SHANDALAR_ENTRY_LAIR)
  {
    RunLairExplorationEvent(g_lair_or_monster_slots[slot_index].color);
    g_lairs_explored = g_lairs_explored + 1;
    EnsureAdvfac64Loaded(0);
    goto LAB_004f683f;
  }

  FUN_004be570(s_dbox2_spr_0058afb8, 0xd5, 0xd2);
  s.creature_type = g_lair_or_monster_slots[slot_index].entry_type;
  DAT_008ce538 = monster_color;
  DAT_007a7874 = (int)gs_creature_names_00591a08[s.creature_type].base_strength;
  s.creature_strength = DAT_007a7874 + (int)gs_creature_names_00591a08[s.creature_type].tier / 2;
  if (gs_creature_names_00591a08[s.creature_type].encounter_type >= '\v')
  {
    s.creature_strength = 0;
  }
  PlayDuelMusic(0);
  if (gs_creature_names_00591a08[s.creature_type].encounter_type == '\v')
  {
    s.selected_card_id = FUN_004bb458(g_world_magic_offer_slot_index);
  }
  else
  {
    s.selected_card_id = FUN_0056c705(gs_creature_names_00591a08[s.creature_type].deck_number);
    if (s.selected_card_id == 0xffffffff)
    {
      s.selected_card_id = monster_color - 1;
    }
  }
  memset(global_ante_cards, 0xff, 0x80);
  LoadCreatureDuelDeck(s.creature_type, 0xffffffff, 0, -1);
  if (gs_creature_names_00591a08[s.creature_type].encounter_type != '\v')
  {
    do
    {
      do
      {
        g_duel_ante_card_ids[0] = DrawRandomCardFromInitialLibrary(DAT_0057a750);
      } while (g_duel_ante_card_ids[0] <= 4);
    } while ((((global_cards_data[g_duel_ante_card_ids[0]].extra_ability & 0x100) != 0) ||
              (GetCardAvailabilityMask(g_duel_ante_card_ids[0]) == 0)) ||
             (global_cards_data[g_duel_ante_card_ids[0]].id ==
              gs_creature_names_00591a08[s.creature_type].deck_number));
  }
  if (gs_creature_names_00591a08[s.creature_type].encounter_type == '\f')
  {
    s.ante_card_count = 3;
  }
  else
  {
    s.ante_card_count = 1;
  }
  ExitIfNoUsableDeckCards();
  for (s.loop_index = 0; s.ante_card_count > s.loop_index; s.loop_index = s.loop_index + 1)
  {
    do
    {
      do
      {
        s.deck_or_card_index = FUN_00522508(500);
      } while (deck[s.deck_or_card_index] == -1);
    } while (((deck[s.deck_or_card_index] & 0x4000) != 0) ||
             (((int)deck[s.deck_or_card_index] & 0xfff) <= 4));
    *(uint *)(global_ante_cards[0] + s.loop_index * 4) = deck[s.deck_or_card_index] & 0xfff;
  }
  s.font_slot = 2;
  s.preduel_background_paths[0] = s_prdblk_pic_0058afd0;
  s.preduel_background_paths[1] = s_prdblu_pic_0058afe8;
  s.preduel_background_paths[2] = s_prdgrn_pic_0058b000;
  s.preduel_background_paths[3] = s_prdrd_pic_0058b018;
  s.preduel_background_paths[4] = s_prdwt_pic_0058b030;
  AnimatePaletteToColor(0, DAT_00589dec);
  LoadPcxResource(1, 0, 0, s.preduel_background_paths[monster_color - 1],
                  (g_graphics_bpp == 8) ? &g_palette_data_words : (RpBitsPalettePacket *)1);
  StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, 0, 0x280, 0x1e0, PTR_DAT_005832b4, 0, 0, global_screen_width,
                          global_screen_height);
  FadeInPaletteFromGray(0, DAT_00589dec);
  ApplyCardArtPaletteToDc(global_main_hdc);
  LoadPcxIntoPage(1, s_prdfrma_pic_0058b03c);
  BeginSpriteEncodeSession();
  s.preduel_card_frame_left = EncodeSpriteFromPage(1, 1, 1, 0x68, 0x2c);
  s.preduel_card_frame_right = EncodeSpriteFromPage(1, 1, 0x2e, 0x67, 0x31);
  s.preduel_bottom_right_sprite = EncodeSpriteFromPage(1, 1, 0x60, 0x66, 0x2c);
  s.preduel_bottom_left_sprite = EncodeSpriteFromPage(1, 1, 0x8d, 0x79, 0x2c);
  s.duel_pool_panel_sprite = EncodeSpriteFromPage(1, 1, 0xba, 0x91, 0x72);
  DrawEncodedImageResampled(PTR_DAT_005832b4, ScaleUiCoordinate(0x14), ScaleUiCoordinate(0x173),
                            ScaleUiCoordinate((int)s.preduel_card_frame_left->width),
                            ScaleUiCoordinate((int)s.preduel_card_frame_left->height),
                            s.preduel_card_frame_left);
  DrawEncodedImageResampled(PTR_DAT_005832b4,
                            (ScaleUiCoordinate((int)s.preduel_card_frame_left->width) +
                             ScaleUiCoordinate(0x14)) -
                                ScaleUiCoordinate(8),
                            ScaleUiCoordinate(0x16d),
                            ScaleUiCoordinate((int)s.preduel_card_frame_right->width),
                            ScaleUiCoordinate((int)s.preduel_card_frame_right->height),
                            s.preduel_card_frame_right);
  s.preduel_bottom_x =
      ((ScaleUiCoordinate((int)s.preduel_card_frame_left->width) +
        ScaleUiCoordinate((int)s.preduel_card_frame_right->width)) -
       ScaleUiCoordinate(8)) /
          2 +
      ScaleUiCoordinate(0x14);
  s.preduel_bottom_x =
      s.preduel_bottom_x -
      ((ScaleUiCoordinate((int)s.preduel_bottom_right_sprite->width) +
        ScaleUiCoordinate((int)s.preduel_bottom_left_sprite->width)) -
       ScaleUiCoordinate(8)) /
          2;
  DrawEncodedImageResampled(PTR_DAT_005832b4, s.preduel_bottom_x,
                            ScaleUiCoordinate((int)s.preduel_card_frame_left->height) +
                                ScaleUiCoordinate(0x173),
                            ScaleUiCoordinate((int)s.preduel_bottom_left_sprite->width),
                            ScaleUiCoordinate((int)s.preduel_bottom_left_sprite->height),
                            s.preduel_bottom_left_sprite);
  DrawEncodedImageResampled(PTR_DAT_005832b4,
                            (s.preduel_bottom_x +
                             ScaleUiCoordinate((int)s.preduel_bottom_left_sprite->width)) -
                                ScaleUiCoordinate(8),
                            ScaleUiCoordinate((int)s.preduel_card_frame_left->height) +
                                ScaleUiCoordinate(0x173),
                            ScaleUiCoordinate((int)s.preduel_bottom_right_sprite->width),
                            ScaleUiCoordinate((int)s.preduel_bottom_right_sprite->height),
                            s.preduel_bottom_right_sprite);
  PTR_DAT_005832b4->font_slot = 4;
  DrawTextAt(PTR_DAT_005832b4, 0xd2, 0x5b, 0x188, DAT_0058b048, Gold);
  DrawTextAt(PTR_DAT_005832b4, 0xd2, 0xbd, 0x188, DAT_0058b04c, g_food);
  DrawTextAt(PTR_DAT_005832b4, 0xd2, 0x5b, 0x1b5, s__d__d_0058b050, DAT_0078df68, DAT_00789938);
  DrawTextAt(PTR_DAT_005832b4, 0xd2, 0xc3, 0x1b5, DAT_0058b058, CountDuelPoolEligibleTowns());
  PTR_DAT_005832b4->font_slot = 2;
  DrawEncodedImageResampled(PTR_DAT_005832b4, ScaleUiCoordinate(0x1d4), ScaleUiCoordinate(0x15e),
                            ScaleUiCoordinate((int)s.duel_pool_panel_sprite->width),
                            ScaleUiCoordinate((int)s.duel_pool_panel_sprite->height),
                            s.duel_pool_panel_sprite);
  BuildQuestStatusMessage();
  DrawTextAt(PTR_DAT_005832b4, 0xd2, 0x21f, 0x199, g_ui_message_buffer);
  FinalizeSpriteEncodeSession();
  FreeSpriteBlob(s.preduel_card_frame_left);
  DrawCreaturePortrait(s.creature_type, ScaleUiCoordinate(0x140), ScaleUiCoordinate(10), 0, 0);
  LoadPcxIntoPage(1, s_prdfrmb_pic_0058b05c);
  PTR_DAT_005832dc->font_slot = 4;
  PTR_DAT_005832b4->font_slot = 4;
  BeginSpriteEncodeSession();
  s.creature_name_plaque_sprite = EncodeSpriteFromPage(1, 1, 0x17e, 0x98, 0x23);
  s.mage_name_plaque_sprite = EncodeSpriteFromPage(1, 1, 0x1a2, 0x80, 0x23);
  FinalizeSpriteEncodeSession();
  DrawEncodedImageResampled(PTR_DAT_005832b4, ScaleUiCoordinate(0x220 - (int)s.creature_name_plaque_sprite->width / 2),
                            ScaleUiCoordinate(5), ScaleUiCoordinate((int)s.creature_name_plaque_sprite->width),
                            ScaleUiCoordinate((int)s.creature_name_plaque_sprite->height), s.creature_name_plaque_sprite);
  DrawFormattedTextNoShadowCentered(PTR_DAT_005832b4, 0xe6, ScaleUiCoordinate(0x220),
                                    ScaleUiCoordinate((int)s.creature_name_plaque_sprite->height / 2 + 5),
                                    gs_encounter_preduel_0077f0d0[0]);
  DrawEncodedImageResampled(PTR_DAT_005832b4, ScaleUiCoordinate(100 - (int)s.mage_name_plaque_sprite->width / 2),
                            ScaleUiCoordinate(5), ScaleUiCoordinate((int)s.mage_name_plaque_sprite->width),
                            ScaleUiCoordinate((int)s.mage_name_plaque_sprite->height), s.mage_name_plaque_sprite);
  DrawFormattedTextNoShadowCentered(PTR_DAT_005832b4, 0xe6, ScaleUiCoordinate(100),
                                    ScaleUiCoordinate((int)s.mage_name_plaque_sprite->height / 2 + 5),
                                    gs_encounter_preduel_0077f0d0[1]);
  PTR_DAT_005832b4->font_slot = 1;
  FreeSpriteBlob(s.creature_name_plaque_sprite);
  sprintf(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[2], FUN_00561441(s.creature_type));
  if ((DAT_008bd200 == 0) && (gs_creature_names_00591a08[s.creature_type].encounter_type != '\v'))
  {
    DrawAdventureCard(g_duel_ante_card_ids[0], 0xe8, 0x18, 1, DAT_0058b068);
  }
  s.bribe_gold_cost = ClampIntToRange(s.creature_strength * 10, 10, s.creature_strength * 0x32);

  if (gs_creature_names_00591a08[s.creature_type].encounter_type != '\v')
  {
    s.world_magic_ante_used = 0;
    s.encounter_relation_type = 0;
    s.slot_tile_x = g_lair_or_monster_slots[slot_index].world_x / 32;
    s.slot_tile_y = g_lair_or_monster_slots[slot_index].world_y / 32;
    s.wizard_home_x = g_castle_dungeon_slots[monster_color + -1].world_x;
    s.wizard_home_y = g_castle_dungeon_slots[monster_color + -1].world_y;
    if (abs(s.slot_tile_x - s.wizard_home_x) <= g_shandalar_difficulty / 2 + 2)
    {
      if (abs(s.slot_tile_y - s.wizard_home_y) <= g_shandalar_difficulty / 2 + 2)
      {
        s.encounter_relation_type = 1;
      }
    }
    if (slot_index == 7)
    {
      s.encounter_relation_type = 2;
    }
  LAB_004f3a77:
    for (s.loop_index = 0; s.ante_card_count > s.loop_index; s.loop_index = s.loop_index + 1)
    {
      if (DAT_008bd200 == 0)
      {
        DrawAdventureCard(*(int *)(global_ante_cards[0] + s.loop_index * 4), s.loop_index * 0x18 + 10,
                          s.loop_index * 0xc + 0x18, 1, DAT_0058b06c);
      }
    }
    s.victory_count = 0;
    for (s.loop_index = 0; (s.loop_index < 1000 && ((char)g_duel_victory_log[s.loop_index] != '\0'));
         s.loop_index = s.loop_index + 1)
    {
      if ((gs_creature_names_00591a08[s.creature_type].encounter_type ==
           ((char)g_duel_victory_log[s.loop_index] & 0xf)) &&
          ((int)(char)g_duel_victory_log[s.loop_index] >> 4 == monster_color))
      {
        s.victory_count = s.victory_count + 1;
      }
    }
    if (s.encounter_relation_type == 1)
    {
      if (IsWizardColorFeminine(monster_color) != 0)
      {
        sprintf(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[3],
                gs_wizardnames_0077ee70[monster_color]);
      }
      else
      {
        sprintf(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[4],
                gs_wizardnames_0077ee70[monster_color]);
      }
      sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), gs_encounter_preduel_0077f0d0[0xb], FUN_00561441(s.creature_type));
    }
    else if (s.encounter_relation_type == 2)
    {
      s.nearest_town_name =
          BuildTownDisplayName(FindNearestTownIndex(g_lair_or_monster_slots[slot_index].world_x / 32,
                                                    g_lair_or_monster_slots[slot_index].world_y / 32));
      if (IsWizardColorFeminine(monster_color) != 0)
      {
        FormatMessageFromStringStripCarriageReturns(g_ui_message_buffer, 0x1000, gs_encounter_preduel_0077f0d0[5],
                                                    gs_wizardnames_0077ee70[monster_color], s.nearest_town_name);
      }
      else
      {
        FormatMessageFromStringStripCarriageReturns(g_ui_message_buffer, 0x1000, gs_encounter_preduel_0077f0d0[6],
                                                    gs_wizardnames_0077ee70[monster_color], s.nearest_town_name);
      }
      sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), gs_encounter_preduel_0077f0d0[0xb], FUN_00561441(s.creature_type));
    }
    else if (s.victory_count < 5)
    {
      if (IsWizardColorFeminine(monster_color) != 0)
      {
        sprintf(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[7],
                gs_wizardnames_0077ee70[monster_color]);
      }
      else
      {
        sprintf(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[8],
                gs_wizardnames_0077ee70[monster_color]);
      }
      sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), gs_encounter_preduel_0077f0d0[0xb], FUN_00561441(s.creature_type));
    }
    else
    {
      if (IsWizardColorFeminine(monster_color) != 0)
      {
        FormatMessageFromStringStripCarriageReturns(g_ui_message_buffer, 0x1000, gs_encounter_preduel_0077f0d0[9],
                                                    gs_creature_names_00591a08[s.creature_type].plural,
                                                    gs_wizardnames_0077ee70[monster_color]);
      }
      else
      {
        FormatMessageFromStringStripCarriageReturns(g_ui_message_buffer, 0x1000, gs_encounter_preduel_0077f0d0[10],
                                                    gs_creature_names_00591a08[s.creature_type].plural,
                                                    gs_wizardnames_0077ee70[monster_color]);
      }
      strcat(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[0xc]);
    }
    s.menu_option_count = 1;
    if ((((s.deck_or_card_index != 0) && (Gold >= s.bribe_gold_cost)) && (s.encounter_relation_type == 0)) && (s.victory_count < 5))
    {
      sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), gs_encounter_preduel_0077f0d0[0xd], s.bribe_gold_cost);
      s.bribe_option_index = s.menu_option_count;
      s.menu_option_count = s.menu_option_count + 1;
    }
    else
    {
      s.bribe_option_index = 0xffffffff;
    }
    if (((s.victory_count >= (g_shandalar_difficulty + 1) * 2) && (s.encounter_relation_type == 0)) && (s.victory_count < 5))
    {
      strcat(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[0xf]);
      s.concede_option_index = s.menu_option_count;
      s.menu_option_count = s.menu_option_count + 1;
    }
    else
    {
      s.concede_option_index = 0xffffffff;
    }
    if ((((g_world_magic_bitmap & 1U) != 0) && (s.world_magic_ante_used == 0)) && (s.ante_card_count == 1))
    {
      strcat(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[0x10]);
      s.world_magic_option_index = s.menu_option_count;
      s.menu_option_count = s.menu_option_count + 1;
    }
    else
    {
      if (s.world_magic_ante_used != 0)
      {
        strcat(g_ui_message_buffer, DAT_0058b070);

        s.world_magic_option_index = 0xffffffff;
      }
      else
      {
        s.world_magic_option_index = 0xffffffff;
      }
    }
    if (DAT_008bd200 >= 0)
    {
      do
      {
        s.menu_choice = RunTextMenuAt(g_ui_message_buffer,
                                      (global_screen_width / 2 -
                                       MeasureMultilineTextWidth(PTR_DAT_005832b4, g_ui_message_buffer) / 2) +
                                          -2,
                                      ScaleUiCoordinate(0x100));
      } while (s.menu_choice == 0xffffffff);
    }
    else
    {
      s.menu_choice = 0;
    }
    if (((((int)s.menu_choice <= 0) && (s.victory_count >= 5)) && (-g_current_quest_type != s.creature_type)) &&
        (s.encounter_relation_type == 0))
    {
      s.bargain_offer_type = FUN_00522508(3);
      s.bargain_payment_type = FUN_00522508(3);
      switch (s.bargain_payment_type)
      {
      case 0:
        s.requested_gold = (gs_creature_names_00591a08[s.creature_type].tier + FUN_00522508(10 - g_shandalar_difficulty)) * 10;
        break;
      case 1:
        break;
      case 2:
        s.requested_card_count = FUN_00522508(3) + 1;
        s.requested_card_tier = FUN_00522508(5) + 1;
        break;
      }

      strcpy(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[0x11]);
      switch (s.bargain_offer_type)
      {
      case 0:
        strcat(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[0x12]);
        break;
      case 1:
        strcat(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[0x13]);
        break;
      case 2:
        sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), gs_encounter_preduel_0077f0d0[0x14],
                gs_city_text_cluster_0077d610.cityname_manacastle_0077de00 + monster_color * 100);
        break;
      }
      switch (s.bargain_payment_type)
      {
      case 0:
        sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), gs_encounter_preduel_0077f0d0[0x15], s.requested_gold);
        break;
      case 1:
        strcat(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[0x16]);
        break;
      case 2:
        if (s.requested_card_count == 1)
        {
          sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), gs_encounter_preduel_0077f0d0[0x17], s.requested_card_count, gs_encounter_preduel_0077f0d0[monster_color + 0x17]);
        }
        else
        {
          sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), gs_encounter_preduel_0077f0d0[0x17], s.requested_card_count, gs_encounter_preduel_0077f0d0[monster_color + 0x1c]);
        }
        break;
      }
      strcat(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[0x22]);
      s.menu_choice = RunTextMenuAt(g_ui_message_buffer, 0x80, 0xc0);
      if ((s.menu_choice == 0) && (s.bargain_offer_type == 0))
      {
        LoadCreatureDuelDeck(s.creature_type, 0xffffffff, 0, -1);
        for (slot_index = 0; slot_index < 500; slot_index = slot_index + 1)
        {
          global_library[1][slot_index] = DrawRandomCardFromInitialLibrary(DAT_0057a750);
          if ((global_cards_data[global_library[1][slot_index]].extra_ability & 0x100) != 0)
          {
            global_library[1][slot_index] = -1;
          }
        }
        SelectPalette(global_main_hdc, g_realized_palette_handle, 0);
        AnimatePaletteToColor(0, DAT_00589dec);
        FUN_004be570(s_dbox_spr_0058b074, 0x71, 0xe3);
        s.selected_card_id = FUN_0056a515(unk_008b35ec, global_library[1], 500,
                                          gs_encounter_preduel_0077f0d0[0x23], 1, &DAT_0058b080);
        if (global_library[1][s.selected_card_id] != -1)
        {
          s.deck_or_card_index = FUN_0056bd9d(global_library[1][s.selected_card_id]);
          if (s.deck_or_card_index != 0xffffffff)
          {
            deck[s.deck_or_card_index] = deck[s.deck_or_card_index] | 0x4000;
          }
        }
      }
      if ((s.menu_choice == 0) && (s.bargain_offer_type == 1))
      {
        strcpy(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[0x24]);
        s.world_magic_list_count = 0;
        for (slot_index = 0; slot_index < 0xc; slot_index = slot_index + 1)
        {
          if ((g_world_magic_bitmap & 1 << (byte)slot_index) == 0)
          {
            strcat(g_ui_message_buffer, DAT_0058b084);
            strcat(g_ui_message_buffer, gs_worldmagic_names_00780660[slot_index]);
            strcat(g_ui_message_buffer, DAT_0058b088);
            s.temp_deck[s.world_magic_list_count] = slot_index;
            s.world_magic_list_count = s.world_magic_list_count + 1;
          }
        }
        if (s.world_magic_list_count == 0)
        {
          strcpy(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[0x25]);
          s.menu_choice = RunTextMenuAtScaled(g_ui_message_buffer, 0x50, 100);
        }
        else
        {
          s.menu_choice = RunTextMenuAt(g_ui_message_buffer, 0xa0, 200);
          if (s.menu_choice != 0xffffffff)
          {
            s.loop_index = Scards[s.temp_deck[s.menu_choice]].worldmagic_city;
            g_town_slots[s.loop_index].status_and_ruling_wizard =
                g_town_slots[s.loop_index].status_and_ruling_wizard | 2;
            FUN_00431526(0x80, g_town_slots[s.loop_index].world_x, g_town_slots[s.loop_index].world_y);
            FormatMessageFromStringStripCarriageReturns(g_ui_message_buffer, 0x1000, gs_encounter_preduel_0077f0d0[0x26],
                                                        gs_worldmagic_names_00780660[s.temp_deck[s.menu_choice]],
                                                        BuildTownDisplayName(s.loop_index));
            RunTextMenuAt(g_ui_message_buffer, 0x5a, 0x6e);
            SelectPalette(global_main_hdc, g_realized_palette_handle, 0);
            AnimatePaletteToColor(0, DAT_00589dec);
            ShowWorldMapScreen(0);
            AnimatePaletteToColor(0, DAT_00589dec);
            ClearGraphicsPageWithPaletteColor(0, 0);
          }
        }
      }
      if ((s.menu_choice == 0) && (s.bargain_offer_type == 2))
      {
        if (g_world_magic_town_flags[monster_color - 1] != 0)
        {
          sprintf(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[0x27],
                  gs_city_text_cluster_0077d610.cityname_manacastle_0077de00 + monster_color * 100);
          RunTextMenuAtScaled(g_ui_message_buffer, 0x54, 0x74);
        }
        else
        {
          if ((g_castle_dungeon_slots[monster_color + -1].clues_bitmap & 7) != 7)
          {
            SelectPalette(global_main_hdc, g_realized_palette_handle, 0);
            AnimatePaletteToColor(0, DAT_00589dec);
            RevealRandomCastleDungeonClue(monster_color + -1);
          }
          else
          {
            ShowDungeonClueDetailScreen(monster_color + -1);
          }
        }
      }
      if (s.menu_choice == 1)
      {
        switch (s.bargain_payment_type)
        {
        case 0:
          Gold = Gold + s.requested_gold;
          break;
        case 1:
          g_food = g_food + 0x28;
          break;
        case 2:
          g_amulet_inventory[s.requested_card_tier - 1] += s.requested_card_count;
          break;
        }
      }
      if (s.menu_choice == 2)
      {
        goto LAB_4F4BB2;
      }

      goto LAB_004f683f;
    }

    if (s.menu_choice == s.world_magic_option_index)
    {
      do
      {
        do
        {
          s.deck_or_card_index = FUN_00522508(500);
        } while (deck[s.deck_or_card_index] == -1);
      } while (((deck[s.deck_or_card_index] & 0x4000) != 0) ||
               (((int)deck[s.deck_or_card_index] & 0xfff) <= 4));
      ANTE_CARD(0, 0) = deck[s.deck_or_card_index] & 0xfff;
      s.world_magic_ante_used = 1;
      s.victory_count = 0;
      goto LAB_004f3a77;
    }

    if (s.concede_option_index == s.menu_choice)
    {
      if (RunCardRiddleChallenge() == 0)
      {
        PlayDuelMusic(2);
        strcpy(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[0x28]);
        LoadPcxIntoPage(1, s_losedul2_pic_0058b08c);
        StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, 0, 0x280, 0x1e0, PTR_DAT_005832b4, 0, 0, global_screen_width,
                                global_screen_height);
        DrawAdventureCard(ANTE_CARD(0, 0), 0x17, 0x50, 1, g_ui_message_buffer);
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
    if (s.bribe_option_index == s.menu_choice)
    {
      Gold = Gold - s.bribe_gold_cost;
      SelectPalette(global_main_hdc, g_realized_palette_handle, 0);
      AnimatePaletteToColor(0, DAT_00589dec);
      LoadPcxIntoPageNoPalette(s_advfac64_pic_0058b0c4);
      FUN_004be570(s_dbox_spr_0058b0d4, 0x71, 0xe3);
      return 0;
    }
  }
  else
  {
    for (s.loop_index = 0; s.ante_card_count > s.loop_index; s.loop_index = s.loop_index + 1)
    {
      if (DAT_008bd200 == 0)
      {
        DrawAdventureCard(*(int *)(global_ante_cards[0] + s.loop_index * 4), s.loop_index * 0x18 + 10,
                          s.loop_index * 0xc + 0x18, 1, DAT_0058b0e0);
      }
    }
    if (IsWizardColorFeminine(monster_color) != 0)
    {
      sprintf(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[0x29],
              gs_wizardnames_0077ee70[monster_color]);
    }
    else
    {
      sprintf(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[0x2a],
              gs_wizardnames_0077ee70[monster_color]);
    }
    sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), gs_encounter_preduel_0077f0d0[0xb], FUN_00561441(s.creature_type));
    ClearInputAndWaitForMouseRelease();
    RunTextMenuAt(g_ui_message_buffer,
                  (global_screen_width / 2 - MeasureMultilineTextWidth(PTR_DAT_005832b4, g_ui_message_buffer) / 2) +
                      -2,
                  ScaleUiCoordinate(0x100));
    FUN_00562893();
  }
LAB_4F4BB2:
  s.prompt_counter = 0;
  for (s.loop_index = 0; s.loop_index < 1000; s.loop_index = s.loop_index + 1)
  {
    if ((char)g_duel_victory_log[s.loop_index] != '\0')
    {
      s.prompt_counter = s.prompt_counter + 1;
    }
  }
  s.deck_or_card_index = DAT_008b3240[monster_color];
  if (gs_creature_names_00591a08[s.creature_type].encounter_type >= '\v')
  {
    s.deck_or_card_index = g_deck_color_bitmap;
  }
  s.duel_special_rules = DAT_008c7408[monster_color];
  if (gs_creature_names_00591a08[s.creature_type].encounter_type >= '\v')
  {
    s.duel_special_rules = _DAT_007483e4;
  }
  DAT_00742fd0 = -ClampIntToRange(3 - s.prompt_counter / 3, 0, 3);
  if ((g_shandalar_difficulty == 3) || (gs_creature_names_00591a08[s.creature_type].encounter_type >= '\v'))
  {
    DAT_00742fd0 = 0;
  }
  if (gs_creature_names_00591a08[s.creature_type].encounter_type == '\v')
  {
    DAT_00742fd0 = Scards[g_world_magic_offer_slot_index].worldmagic_price / 500 + -1;
  }
  if (-g_shandalar_difficulty < DAT_00742fd0)
  {
    s.deck_or_card_index = g_deck_color_bitmap;
  }
  s.allow_random_reward = (uint)(((g_monster_timer >> 2) % 3) == 0);
  if (s.allow_random_reward != 0)
  {
    PlaySoundEffectOnChannel(s_x_sound_dsummon_wav_0058b0e4, 0xf, 100, 100, 0);
    if ((gs_creature_names_00591a08[s.creature_type].preduel_flags & 4) != 0)
    {
      if (FUN_00522508(3) == 0)
      {
        s.deck_or_card_index = FUN_00522508(0x23) + 1;
        strcpy(g_ui_message_buffer, s_decks_0_0058b0f8);
        if (gs_creature_names_00591a08[s.deck_or_card_index].deck_number < 100)
        {
          strcat(g_ui_message_buffer, DAT_0058b100);
          if (gs_creature_names_00591a08[s.deck_or_card_index].deck_number < 10)
          {
            strcat(g_ui_message_buffer, DAT_0058b104);
          }
        }
        strcat(g_ui_message_buffer, _itoa(gs_creature_names_00591a08[s.deck_or_card_index].deck_number, DAT_0093a870,
                                          10));
        strcat(g_ui_message_buffer, DAT_0058b108);
        ClearAndLoadInitialLibraryFromDeckFile(g_ui_message_buffer, 0, 0, -1);
        g_selected_wizard_color = 0;
        strcpy(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[0x2b]);
        RunTextMenuAt(g_ui_message_buffer, 0xa0, 0xa0);
      }
    }
    if ((((gs_creature_names_00591a08[s.creature_type].preduel_flags & 0x110) != 0) &&
         (FUN_00522508(3) == 0)) &&
        (-g_current_quest_type != s.creature_type))
    {
      do
      {
        do
        {
          s.deck_or_card_index = s.creature_type + FUN_00522508(3) + 1;
        } while (s.deck_or_card_index == 0x37);
      } while (((int)s.deck_or_card_index < 0x24) && ((int)s.deck_or_card_index % 7 == 0));
      DrawCreaturePortrait(s.deck_or_card_index, ScaleUiCoordinate(0x140), ScaleUiCoordinate(10), 0, 1);
      FormatMessageFromStringStripCarriageReturns(g_ui_message_buffer, 0x1000, gs_encounter_preduel_0077f0d0[0x2c],
                                                  FUN_00561441(s.creature_type), BuildCreatureNameWithArticle(s.deck_or_card_index));
      RunTextMenuAt(g_ui_message_buffer, 0xa0, 0x78);
      s.creature_type = s.deck_or_card_index;
      s.creature_strength = DAT_007a7874 + (int)gs_creature_names_00591a08[s.deck_or_card_index].tier / 2;
    }
    if ((gs_creature_names_00591a08[s.creature_type].preduel_flags & 0xc1) != 0)
    {
      DAT_008cf6d0 = FUN_0056c705(gs_creature_names_00591a08[s.creature_type].reward_card_id);
    }
    if ((gs_creature_names_00591a08[s.creature_type].preduel_flags & 0xcb) != 0)
    {
      s.duel_setup_allowed = 1;
      if ((gs_creature_names_00591a08[s.creature_type].preduel_flags & 2) != 0)
      {
        if (g_monster_timer % 3)
        {
          s.duel_setup_allowed = 0;
        }
        else
        {
          sprintf(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[0x2d], FUN_00561441(s.creature_type));
        }
      }
      if ((gs_creature_names_00591a08[s.creature_type].preduel_flags & 8) != 0)
      {
        DAT_0073c7e4 = 1;
        sprintf(g_ui_message_buffer, gs_encounter_preduel_0077f0d0[0x2e], FUN_00561441(s.creature_type));
      }
      if ((gs_creature_names_00591a08[s.creature_type].preduel_flags & 0xc1) != 0)
      {
        if (DAT_00742fd0 < 0)
        {
          DAT_008cf6d0 = -1;
          s.duel_setup_allowed = 0;
        }
        else
        {
          FormatMessageFromStringStripCarriageReturns(g_ui_message_buffer, 0x1000, gs_encounter_preduel_0077f0d0[0x2f],
                                                      FUN_00561441(s.creature_type), global_cards_data[DAT_008cf6d0].name);
        }
      }
      if (s.duel_setup_allowed != 0)
      {
        RunTextMenuAt(g_ui_message_buffer, 0xb4, 0x8c);
      }
    }
  }
  for (s.loop_index = 0; s.loop_index < 7; s.loop_index = s.loop_index + 1)
  {
    if ((slot_index != s.loop_index) && (g_lair_or_monster_slots[s.loop_index].entry_type != SHANDALAR_ENTRY_LAIR))
    {
      if ((FUN_004ecf30(g_world_player_x - g_lair_or_monster_slots[s.loop_index].world_x,
                        g_world_player_y - g_lair_or_monster_slots[s.loop_index].world_y) < 0x40) &&
          (g_lair_or_monster_slots[s.loop_index].entry_type != SHANDALAR_ENTRY_NONE))
      {
        FreeOpeningMenuSpriteWorkEntries(s.loop_index, s.loop_index + 8);
        g_lair_or_monster_slots[s.loop_index].entry_type = SHANDALAR_ENTRY_NONE;
      }
    }
  }
  LoadCreatureDuelDeck(s.creature_type, s.selected_card_id, s.deck_or_card_index, s.duel_special_rules);
  DestroyCachedCardArt();
  AnimatePaletteToColor(0, DAT_00589dec);
  SelectPalette(global_main_hdc, g_realized_palette_handle, 0);
  FUN_004be570(s_dbox_spr_0058b110, 0x71, 0xe3);
  sound_stop(0);
  s.duel_prompt_done = -1;
  s.duel_prompt_choice = 0;
  while ((s.duel_prompt_done != 0 && (s.duel_prompt_choice != 4)))
  {
    s.duel_prompt_choice = FUN_0056d5c0(0, &s.duel_prompt_done);
  }
  s.duel_result = RunDuelEngine(s.selected_card_id, s.creature_type);
  LoadPcxIntoPageNoPalette(s_advfac64_pic_0058b11c);
  if (s.duel_result == 1)
  {
    s.reward_text_color = 0xca;
    s.reward_text_x = 0x8c;
    s.reward_text_y = 0x118;
    AppendDuelVictoryLogEntry(gs_creature_names_00591a08[s.creature_type].encounter_type, (byte)monster_color);
    AddJournalEntry(JOURNAL_ENTRY_CREATURE_DUEL, s.creature_type | 0x80);
    ShowStatsWindow(2, DAT_008ce538 << 0x10 | s.creature_type);
    if (-g_current_quest_type == s.creature_type)
    {
      g_current_quest_type = g_current_quest_type + -100;
    }
    if (gs_creature_names_00591a08[s.creature_type].encounter_type < '\v')
    {
      s.hint_dungeon_index = PickWisemanCastleDungeonSlotForHint();
      s.loop_index = s.creature_strength;
      s.menu_option_count = 0;
      do
      {
        do
        {
          do
          {
            s.selected_card_id = PickRandomCardMatchingTypeAndColor(1 << (byte)FUN_00522508(6), 1);
          } while (FUN_0056c0e5(1 << (byte)monster_color,
                                (int)global_cards_data[s.selected_card_id].color,
                                ((s.menu_option_count & 1) ? 1 : 3) == 0) == 0);
        } while (((FUN_004bb1cf(s.selected_card_id) <= 0) || ((global_cards_data[s.selected_card_id].extra_ability & 0x900) != 0)) ||
                 (GetCardAvailabilityMask(s.selected_card_id) == 0));
        if (((int)s.menu_option_count < 3) && (g_duel_ante_card_ids[s.menu_option_count] != -1))
        {
          s.selected_card_id = g_duel_ante_card_ids[s.menu_option_count];
        }
        s.reward_card_ids[s.menu_option_count] = s.selected_card_id;
        s.menu_option_count = s.menu_option_count + 1;
        s.loop_index = s.loop_index - (((global_cards_data[s.selected_card_id].extra_ability & 0x400) >> 10) + FUN_0056c5ea(s.selected_card_id));
      } while (0 < s.loop_index);
      PlayDuelMusic(1);
      LoadPcxIntoPageOpaque(1, s_winbak01_pic_0058b12c);
      StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, 0, 0x280, 0x1e0, PTR_DAT_005832b4, 0, 0, global_screen_width,
                              global_screen_height);
      s.reward_card_spacing = 300 / (int)(s.menu_option_count + 1);
      for (slot_index = 0; slot_index < (int)s.menu_option_count; slot_index = slot_index + 1)
      {
        s.scratch_values[6] = FUN_0056c5ea(s.reward_card_ids[slot_index]) + -1;
        strcpy(g_ui_message_buffer, gs_browse_0074da20[s.scratch_values[6] + 13]);
        DrawAdventureCard(s.reward_card_ids[slot_index],
                          (s.reward_card_spacing * slot_index + 0x6f) - (int)((s.menu_option_count - 1) * s.reward_card_spacing) / 2,
                          FUN_00522508(10) + 0x10, 1, g_ui_message_buffer);
      }
      if (s.hint_dungeon_index != -1)
      {
        s.text_y = 0xcb;
        LoadPcxIntoPage(1, s_endplak_pic_0058b13c);
        StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, 0, 0x128, 0x36, PTR_DAT_005832b4,
                                ScaleUiCoordinate(0x34), ScaleUiCoordinate(0x195),
                                ScaleUiCoordinate(0x128), ScaleUiCoordinate(0x36));
        strcpy(g_ui_message_buffer, GetDungeonName(s.hint_dungeon_index));
        DrawUiScaledCenteredText(g_ui_message_buffer, 100, s.text_y + 3, 0xca);
        s.text_y = s.text_y + 9;
        strcpy(g_ui_message_buffer, DAT_0058b148);
        for (s.loop_index = 0; s.loop_index < 3; s.loop_index = s.loop_index + 1)
        {
          if ((&g_castle_dungeon_slots[s.hint_dungeon_index].card_slot_1)[s.loop_index] == -1)
          {
            continue;
          }
          if (s.loop_index != 0)
          {
            strcat(g_ui_message_buffer, DAT_0058b14c);
          }
          strcat(g_ui_message_buffer,
                 global_cards_data[(&g_castle_dungeon_slots[s.hint_dungeon_index].card_slot_1)[s.loop_index]].name);
        }
        DrawScaledCenteredTextNoShadow(g_ui_message_buffer, 100, s.text_y, 0xca);
        s.text_y = s.text_y + 7;
        s.deck_or_card_index = 0;
        for (s.loop_index = 0; s.loop_index < 4; s.loop_index = s.loop_index + 1)
        {
          if ((g_castle_dungeon_slots[s.hint_dungeon_index].clues_bitmap & 1 << (byte)s.loop_index) != 0)
          {
            s.deck_or_card_index = s.deck_or_card_index + 1;
          }
        }
        if (s.deck_or_card_index == 0)
        {
          strcpy(g_ui_message_buffer, gs_encounter_postduel_0077f050[0]);
        }
        else if (s.deck_or_card_index == 1)
        {
          strcpy(g_ui_message_buffer, gs_encounter_postduel_0077f050[1]);
        }
        else if (s.deck_or_card_index == 2)
        {
          strcpy(g_ui_message_buffer, gs_encounter_postduel_0077f050[2]);
        }
        else
        {
          strcpy(g_ui_message_buffer, DAT_0058b150);
        }
        DrawScaledCenteredTextNoShadow(g_ui_message_buffer, 100, s.text_y, 200);
        if (IsCreatureTypeFeminine(s.creature_type) != 0)
        {
          sprintf(g_ui_message_buffer, gs_encounter_postduel_0077f050[3], FUN_00561441(s.creature_type));
        }
        else
        {
          sprintf(g_ui_message_buffer, gs_encounter_postduel_0077f050[4], FUN_00561441(s.creature_type));
        }
        strcat(g_ui_message_buffer, gs_encounter_postduel_0077f050[5]);
        FUN_004f2165();
        slot_index = RunTextMenuAtScaled(g_ui_message_buffer, 0x40, 0x7c);
        if (slot_index != 0)
        {
          RevealRandomCastleDungeonClue(s.hint_dungeon_index);
        }
        else
        {
          if (s.hint_dungeon_index != -1)
          {
            g_castle_dungeon_slots[s.hint_dungeon_index].rules_bitmap =
                g_castle_dungeon_slots[s.hint_dungeon_index].rules_bitmap | 0x200;
          }
          for (slot_index = 0; (int)s.menu_option_count > slot_index; slot_index = slot_index + 1)
          {
            s.deck_or_card_index = FUN_0056bd9d(s.reward_card_ids[slot_index]);
            if (s.deck_or_card_index != 0xffffffff)
            {
              deck[s.deck_or_card_index] = deck[s.deck_or_card_index] | 0x4000;
            }
          }
        }
      }
      else
      {
        PTR_DAT_005832b4->font_slot = 5;
        DrawTextAt(PTR_DAT_005832b4, 0xff, 0x140, 0x15e, gs_encounter_postduel_0077f050[6]);
        WaitForInputEventUnlessBlocked();
        for (slot_index = 0; (int)s.menu_option_count > slot_index; slot_index = slot_index + 1)
        {
          s.deck_or_card_index = FUN_0056bd9d(s.reward_card_ids[slot_index]);
          if (s.deck_or_card_index != 0xffffffff)
          {
            deck[s.deck_or_card_index] = deck[s.deck_or_card_index] | 0x4000;
          }
        }
      }
      ClearInputAndWaitForMouseRelease();
      RecountDeckCardTotals();
      if (DAT_00789938 >= 0x1c2)
      {
        sprintf(g_ui_message_buffer, gs_encounter_postduel_0077f050[0x1f], DAT_00789938);
        RunTextMenuAt(g_ui_message_buffer,
                      (global_screen_width / 2 - MeasureMultilineTextWidth(PTR_DAT_005832b4, g_ui_message_buffer) / 2) +
                          -2,
                      ScaleUiCoordinate(0x100));
      }
      s.reward_flags = gs_creature_names_00591a08[s.creature_type].reward_flags;
      if (s.allow_random_reward == 0)
      {
        s.reward_flags = 0;
      }
      if (FUN_00522508(0x28) < s.creature_strength)
      {
        s.scratch_values[1] = 2;
        s.scratch_values[2] = 1;
        s.scratch_values[3] = 4;
        s.scratch_values[4] = 3;
        s.scratch_values[5] = 0;
        g_amulet_inventory[monster_color - 1] += 1;
        LoadPcxIntoPage(1, s_winbak02_pic_0058b154);
        StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, 0, 0x280, 0x1e0, PTR_DAT_005832b4, 0, 0, global_screen_width,
                                global_screen_height);
        ClearInputAndWaitForMouseRelease();
        strcpy(g_ui_message_buffer, gs_encounter_postduel_0077f050[7]);
        s.scratch_values[0] = MeasureMultilineTextWidth(PTR_DAT_005832b4, g_ui_message_buffer);
        DrawTiledDialogBoxFrame((ScaleUiCoordinate(0xa0) - s.scratch_values[0] / 2) + -8,
                                ScaleUiCoordinate(0xa0), s.scratch_values[0] + 0x10,
                                ScaleUiCoordinate(0x30) + 0xe, 0);
        DrawFormattedTextShadowedCentered(PTR_DAT_005832b4, 0xff, ScaleUiCoordinate(0xa0),
                                          ScaleUiCoordinate(0xae), g_ui_message_buffer);
        DrawEncodedImageResampled(PTR_DAT_005832b4, ScaleUiCoordinate(0xa0) - ScaleUiCoordinate(0xd),
                                  ScaleUiCoordinate(0xbd), ScaleUiCoordinate(0x1a),
                                  ScaleUiCoordinate(0x22),
                                  g_world_magic_avatar_sprites[s.scratch_values[monster_color]]);
        if (s.reward_flags == 0)
        {
          ClearInputAndWaitForMouseRelease();
          WaitForInputEventUnlessBlocked();
        }
      }
      else if ((s.reward_flags == 0) && (s.allow_random_reward != 0))
      {
        s.reward_flags = 1 << (byte)FUN_00522508(0xc);
        s.reward_flags &= 0x1a19;
      }
      if (s.reward_flags != 0)
      {
        PlaySoundEffectOnChannel(s_x_sound_treasure_wav_0058b164, 0xf, 100, 100, 0);
        LoadPcxIntoPage(1, s_winbak02_pic_0058b17c);
        StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, 0, 0x280, 0x1e0, PTR_DAT_005832b4, 0, 0, global_screen_width,
                                global_screen_height);
        DrawCreaturePortrait(s.creature_type, ScaleUiCoordinate(s.reward_text_x), ScaleUiCoordinate(0x23), 1, 0);
        PTR_DAT_005832b4->font_slot = 4;
        if (g_shandalar_difficulty == 0)
        {
          if (FUN_00566238())
          {
            sprintf(g_ui_message_buffer, gs_encounter_postduel_0077f050[8], FUN_00561441(s.creature_type));
          }
          else
          {
            sprintf(g_ui_message_buffer, gs_encounter_postduel_0077f050[9], FUN_00561441(s.creature_type));
          }
        }
        else if (g_shandalar_difficulty == 1)
        {
          if (FUN_00566238())
          {
            sprintf(g_ui_message_buffer, gs_encounter_postduel_0077f050[10], FUN_00561441(s.creature_type));
          }
          else
          {
            sprintf(g_ui_message_buffer, gs_encounter_postduel_0077f050[11], FUN_00561441(s.creature_type));
          }
        }
        else if (g_shandalar_difficulty == 2)
        {
          if (FUN_00566238())
          {
            sprintf(g_ui_message_buffer, gs_encounter_postduel_0077f050[12], FUN_00561441(s.creature_type));
          }
          else
          {
            sprintf(g_ui_message_buffer, gs_encounter_postduel_0077f050[13], FUN_00561441(s.creature_type));
          }
        }
        else if (g_shandalar_difficulty == 3)
        {
          if (FUN_00566238())
          {
            sprintf(g_ui_message_buffer, gs_encounter_postduel_0077f050[14], FUN_00561441(s.creature_type));
          }
          else
          {
            sprintf(g_ui_message_buffer, gs_encounter_postduel_0077f050[15], FUN_00561441(s.creature_type));
          }
        }
        else
        {
          strcpy(g_ui_message_buffer, DAT_0058b18c);
        }
        DrawTextAt(PTR_DAT_005832b4, s.reward_text_color, s.reward_text_x, s.reward_text_y, g_ui_message_buffer);
        s.reward_text_y = s.reward_text_y + 0x28;
      }
      if (((s.reward_flags & 1) != 0) && (life[0] != DAT_00716024))
      {
        g_next_duel_life_delta = life[0] - DAT_00716024;
        sprintf(g_ui_message_buffer, gs_encounter_postduel_0077f050[0x10], life[0]);
        DrawTextAt(PTR_DAT_005832b4, s.reward_text_color, s.reward_text_x, s.reward_text_y, g_ui_message_buffer);
      }
      if ((s.reward_flags & 0x800) != 0)
      {
        g_next_duel_card_id = FUN_00522508(4) + 1;
        sprintf(g_ui_message_buffer, gs_encounter_postduel_0077f050[0x11], g_next_duel_card_id);
        DrawTextAt(PTR_DAT_005832b4, s.reward_text_color, s.reward_text_x, s.reward_text_y, g_ui_message_buffer);
      }
      if ((s.reward_flags & 0x402) != 0)
      {
        do
        {
          s.random_tile_x = FUN_00522508(0x40);
          s.text_y = FUN_00522508(0x40);
        } while (GetWorldTileType(s.random_tile_x, s.text_y) == 0);
        g_world_player_x = s.random_tile_x * 0x20 + 0x10;
        g_world_player_y = s.text_y * 0x20 + 0x10;
        g_world_scroll_cache_ready = 0;
        DrawTextAt(PTR_DAT_005832b4, s.reward_text_color, s.reward_text_x, s.reward_text_y,
                   gs_encounter_postduel_0077f050[0x12]);
      }
      if ((s.reward_flags & 4) != 0)
      {
        g_next_duel_card_id =
            FUN_0056c705(gs_creature_names_00591a08[s.creature_type].reward_card_id);
        s.deck_or_card_index = FUN_0056bd9d(g_next_duel_card_id);
        if (s.deck_or_card_index != 0xffffffff)
        {
          deck[s.deck_or_card_index] = deck[s.deck_or_card_index] | 0x4000;
        }
        sprintf(g_ui_message_buffer, gs_encounter_postduel_0077f050[0x13],
                global_cards_data[g_next_duel_card_id].name);
        DrawAdventureCard(g_next_duel_card_id, 0xa0, 0x70, 1, g_ui_message_buffer);
        g_next_duel_card_id = -1;
      }
      if ((s.reward_flags & 0x10) != 0)
      {
        g_next_duel_card_id = 0;
        DrawTextAt(PTR_DAT_005832b4, s.reward_text_color, s.reward_text_x, s.reward_text_y,
                   gs_encounter_postduel_0077f050[0x14]);
      }
      if ((s.reward_flags & 0x20) != 0)
      {
        g_next_duel_card_id =
            FUN_0056c705(gs_creature_names_00591a08[s.creature_type].reward_card_id);
        sprintf(g_ui_message_buffer, gs_encounter_postduel_0077f050[0x15],
                global_cards_data[g_next_duel_card_id].name);
        DrawAdventureCard(g_next_duel_card_id, 0xa0, 0x70, 1, g_ui_message_buffer);
      }
      if ((s.reward_flags & 0x180) != 0)
      {
        do
        {
          do
          {
            s.deck_or_card_index = FUN_00522508(g_card_count - 0x39);
          } while ((global_cards_data[s.deck_or_card_index].type & 0x42) != 0x40);
        } while (((int)s.deck_or_card_index <= 4) || (GetCardAvailabilityMask(s.deck_or_card_index) == 0));
        g_next_duel_card_id = s.deck_or_card_index;
        sprintf(g_ui_message_buffer, gs_encounter_postduel_0077f050[0x15], global_cards_data[g_next_duel_card_id].name);
        DrawAdventureCard(g_next_duel_card_id, 0xa0, 0x70, 1, g_ui_message_buffer);
      }
      if ((s.reward_flags & 0x200) != 0)
      {
        g_food = g_food + FUN_00522508(0x1e) + 0x14;
        strcpy(g_ui_message_buffer, gs_encounter_postduel_0077f050[0x16]);
        DrawTextAt(PTR_DAT_005832b4, s.reward_text_color, s.reward_text_x, s.reward_text_y, g_ui_message_buffer);
      }
      if ((s.reward_flags & 0x40) != 0)
      {
        strcpy(g_ui_message_buffer, gs_encounter_postduel_0077f050[0x17]);
        DrawTextAt(PTR_DAT_005832b4, s.reward_text_color, s.reward_text_x, s.reward_text_y, g_ui_message_buffer);
        WaitForInputEventUnlessBlocked();
        FUN_005616aa(1);
        s.selected_card_id = RunCardBrowser(gs_encounter_postduel_0077f050[0x19], 0, 0xffffffff, 1, 0);
        while (s.selected_card_id == 0xffffffff)
        {
          s.selected_card_id = RunCardBrowser(gs_encounter_postduel_0077f050[0x19], 0, 0xffffffff, 0, 0);
        }
        if (s.selected_card_id != 0xffffffff)
        {
          s.iVar8 = FUN_0056bd9d(s.selected_card_id);
          deck[s.iVar8] = deck[s.iVar8] | 0x4000;
        }
        s.reward_flags = 0;
      }
      if ((s.reward_flags & 0x1000) != 0)
      {
        strcpy(g_ui_message_buffer, gs_encounter_postduel_0077f050[0x18]);
        DrawTextAt(PTR_DAT_005832b4, s.reward_text_color, s.reward_text_x, s.reward_text_y, g_ui_message_buffer);
        WaitForInputEventUnlessBlocked();
        FUN_005616aa(1);
        for (slot_index = 0; slot_index < 500; slot_index = slot_index + 1)
        {
          s.temp_deck[slot_index] = deck[slot_index];
          if (s.temp_deck[slot_index] != 0xffffffff)
          {
            s.temp_deck[slot_index] = s.temp_deck[slot_index] & 0xfff;
          }
        }
        s.selected_card_id = FUN_0056a515(unk_008b35ec, (int *)s.temp_deck, 500,
                                          gs_encounter_postduel_0077f050[0x19], 1, &DAT_0058b190);
        if (s.selected_card_id != 0xffffffff)
        {
          s.iVar7 = FUN_0056bd9d(s.temp_deck[s.selected_card_id]);
          deck[s.iVar7] = deck[s.iVar7] | 0x4000;
        }
        s.reward_flags = 0;
      }
      if ((s.reward_flags & 8) != 0)
      {
        strcpy(g_ui_message_buffer, gs_encounter_postduel_0077f050[0x1a]);
        DrawTextAt(PTR_DAT_005832b4, s.reward_text_color, s.reward_text_x, s.reward_text_y, g_ui_message_buffer);
        Gold = Gold + 100;
      }
      ClearInputAndWaitForMouseRelease();
      if (s.reward_flags != 0)
      {
        WaitForInputEventUnlessBlocked();
      }
      if (FUN_00522508(0x40 / (g_shandalar_difficulty + 1)) < s.creature_strength)
      {
        DAT_008b3240[monster_color] = g_deck_color_bitmap;
      }
      if (FUN_00522508(0x80 / (g_shandalar_difficulty + 1)) < s.creature_strength)
      {
        DAT_008c7408[monster_color] = _DAT_007483e4;
      }
    }
  }
  if (s.duel_result == 0)
  {
    PlayDuelMusic(2);
    LoadPcxIntoPage(1, s_losedul2_pic_0058b194);
    StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, 0, 0x280, 0x1e0, PTR_DAT_005832b4, 0, 0, global_screen_width,
                            global_screen_height);
    AddJournalEntry(JOURNAL_ENTRY_CREATURE_DUEL, s.creature_type);
    for (s.ante_card_slot = 0; s.ante_card_slot < 3; s.ante_card_slot = s.ante_card_slot + 1)
    {
      s.ante_card_id = *(uint *)(global_ante_cards[0] + s.ante_card_slot * 4);
      if (s.ante_card_id == 0xffffffff)
      {
        continue;
      }
      strcpy(g_ui_message_buffer, gs_encounter_postduel_0077f050[0x1b]);
      DrawAdventureCard(s.ante_card_id, 0x17, 0x50, 1, g_ui_message_buffer);
      ClearInputAndWaitForMouseRelease();
      WaitForInputEventUnlessBlocked();
      RemoveCardFromDeckById(s.ante_card_id);
    }
  }
  if (s.duel_result == -1)
  {
    LoadPcxIntoPage(1, s_losedul2_pic_0058b1a4);
    StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, 0, 0x280, 0x1e0, PTR_DAT_005832b4, 0, 0, global_screen_width,
                            global_screen_height);
    DrawCreaturePortrait(s.creature_type, global_screen_width / 4, ScaleUiCoordinate(0x3c), 1, 0);
    PTR_DAT_005832b4->font_slot = 4;
    if (IsCreatureTypeFeminine(s.creature_type) != 0)
    {
      sprintf(g_ui_message_buffer, gs_encounter_postduel_0077f050[0x1c], FUN_00561441(s.creature_type));
    }
    else
    {
      sprintf(g_ui_message_buffer, gs_encounter_postduel_0077f050[0x1d], FUN_00561441(s.creature_type));
    }
    DrawWorldUiFormattedText(PTR_DAT_005832b4, 0xca, global_screen_width / 4, ScaleUiCoordinate(300),
                             g_ui_message_buffer);
    DrawWorldUiFormattedText(PTR_DAT_005832b4, 0xca, global_screen_width / 4,
                             ScaleUiCoordinate(GetFontLineHeight(PTR_DAT_005832b4->font_slot) * 4 + 300),
                             gs_encounter_postduel_0077f050[0x1e]);
    ClearInputAndWaitForMouseRelease();
    WaitForInputEventUnlessBlocked();
  }

LAB_004f683f:
  FUN_004be570(s_dbox_spr_0058b1b4, 0x71, 0xe3);
  SelectPalette(global_main_hdc, g_realized_palette_handle, 0);
  LoadPcxIntoPageNoPalette(s_advfac64_pic_0058b1c0);
  ClearInputAndWaitForMouseRelease();
  return s.duel_result;
}
