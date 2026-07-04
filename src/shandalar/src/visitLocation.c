#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "defs.h"
#include "shandalar.h"
#include "magic/src/global_state.h"
#include "magic/src/global_strings.h"
#include "magic/src/shared_startup.h"
#include "shandalar_global_strings.h"
#include "deckdll/src/magsnd.h"
#include "facemaker/src/facemaker_types.h"
#include "drawcardlib/src/pic.h"

extern int global_screen_width;
extern int global_screen_height;
extern HWND g_main_window_hwnd;

extern int g_menu_render_guard;
extern int g_mouse_x;
extern int g_mouse_y;
extern int g_mouse_button_down_mask;
extern int g_mouse_x_snapshot;
extern int g_mouse_y_snapshot;

extern FacemakerWindowBounds *PTR_DAT_005832b4;
extern FacemakerWindowBounds *PTR_DAT_005832dc;

extern DIBSurface *g_graphics_pages[10];

extern char g_ui_message_buffer[0x1000];
extern int DAT_0078990c[10];
extern int g_text_menu_abort_requested;
extern card_data_t global_cards_data[];
extern int g_card_count;
extern int DAT_00589dec;
extern int DAT_009300f0;
extern char DAT_0097e340[0x110];

// GLOBAL: SHANDALAR 0x0073ea68
int DAT_0073ea68;

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

extern WorldMagicChoiceButtonSpriteBank g_world_magic_choice_button_sprite_bank;

void LoadPcxResource(int page_number, int x, int y, char *path, void *opaque);
void LoadPcxIntoPage(int page_number, char *path);
void LoadPcxIntoPageOpaque(int page_number, char *path);

void StretchBlitGraphicsRect(FacemakerWindowBounds *dst, int dst_x, int dst_y, int src_w, int src_h,
                             FacemakerWindowBounds *src, int src_x, int src_y, int copy_w, int copy_h);
void BlitGraphicsRect(FacemakerWindowBounds *dst, int dst_x, int dst_y, int width, int height,
                      FacemakerWindowBounds *src, int src_x, int src_y);

int ScaleUiCoordinate(int value);
int FUN_005501dc(int value);

unsigned int FUN_0043146b(int x, int y);
unsigned int FUN_005611c8(unsigned int tile_mask);
int FUN_0040dffd(int mask);

void PopNormalizedQueuedKeyInput(void);
int RunTextMenuAt(char *menu_text, int left_x, int top_y);
int RunTextMenuAtScaled(char *menu_text, int x_320_scale, unsigned int y_200_scale);
void ClearInputAndWaitForMouseRelease(void);
unsigned int WaitForInputEventUnlessBlocked(void);

void FUN_00562736(int param_1, int param_2, int param_3, int param_4);
unsigned int LoadSoundWithDriveFallback(char *filename, int channel, int flags);

void RefreshAdventureInterfaceLayout(void);
void *FUN_0055060c(int param_1);
void FUN_004290e2(int param_1, int param_2);
void FUN_004ce992(int delay);

void FUN_0050a5c1(int param_1);
void EnsureAdvfac64Loaded(int state);
WPARAM WINAPI DeckBuilderMain(HWND parent_hwnd, int db_flags_1, int db_flags_2);
void ShowWorldMapScreen(int mode);
void ShowDungeonCluesScreen(void);
int RestoreAdventureUiPaletteAndFocus(void);
void FUN_00549002(void);

void __cdecl FUN_004f2407(int card_index, int x, int y, int full_card, char *banner_label);
int FUN_00522508(int param_1);

void AnimatePaletteToColor(int color_index, int palette_id);
void SetFontStyleSize(int font_slot, unsigned int scaled_size);
int GetFontLineHeight(int font_slot);
void UnloadStatWinDllExports(void);
int FUN_00469099(void);
int FileExists(const char *filename);
void FreeOpeningMenuSpriteWorkEntries(int work_entry_index_a, int work_entry_index_b);
void PlaySoundEffectOnChannel(char *sound_path, int channel, int volume, int pitch_percent, int pan_percent);
char *BuildCreatureNameWithArticle(int creature_type);
void FUN_00431526(unsigned int param_1, int param_2, int param_3);
int FUN_00417e11(char *param_1, int param_2, unsigned int param_3, int param_4);

int ApplyPortraitPaletteMap(FacemakerWindowBounds *page, int src_x, int src_y, unsigned int width, int height, char *palette_source_path, char *portrait_path);
void BeginSpriteEncodeSession(void);
void FinalizeSpriteEncodeSession(void);
EncodedImage *EncodeSpriteFromPage(int page_number, int src_x, int src_y, int width, int height);
void FreeSpriteBlob(void *sprite_blob);

void __cdecl DrawFormattedTextNoShadowCentered(FacemakerWindowBounds *dst, int text_color, int x, int y, char *format, ...);
void __cdecl DrawFormattedTextShadowedCentered(FacemakerWindowBounds *window, int color_index, int x, int y, char *format, ...);
unsigned int BlitRectByRandomTileOrderInPlace(HDC dst, int dst_x, int dst_y, int width, int height, int tile_w, int tile_h, HDC src);
void FUN_0057d500(HDC dst_hdc, int x, int y, int w, int h, int unk_20, int unk_a, int unk_b, int unk_c, HDC src_hdc);

void DestroyAllCardBackgrounds(void);
void DestroyAllBigArts(void);
void DestroyAllSmallArts(void);
void InitializeNewGameState(void);

char *FUN_00550220(int town_index);
int FUN_004ecf30(int x, int y);
int GetRelativeWorldQuadrant(int world_x, int world_y);
int FUN_0056bd9d(unsigned int card_id);
int FUN_0056c705(int param_1);
int FindDeckSlotForQuestColorAndType(unsigned char quest_color, unsigned char quest_bitmap_mask);
char *FUN_00561441(int creature_type);
DWORD FUN_00564e70(char *dst, DWORD max_length, LPCVOID format, ...);
int FUN_004ef1ea(int town_index);
int FUN_004bb458(int param_1);
char *FUN_004f2e17(int town_index);
char *BuildTownDisplayName(int town_index);
extern int DAT_0058c620[];

int FUN_00568320(unsigned int param_1, int param_2);
int FUN_005636ab(void);
int FUN_0056139e(int wizard_color, int creature_tier);
void FUN_0050a5f1(int creature_type, unsigned int name_id, unsigned int param_3, int param_4);
void FUN_005222ae(char *param_1, int param_2, unsigned int param_3, int param_4);
int FUN_0052234e(int library_index);
void FUN_004f6886(int creature_type, int x_320, int y_200, int tinted, int mode);
void FUN_005629c3(int tune_index);
void FUN_004f6d32(unsigned int card_id);
unsigned int FUN_00532aeb(char *title, unsigned int color_mask, unsigned int type_mask, int param_4, int param_5);
int FUN_00530b7e(void);
void DrawTiledDialogBoxFrame(int x, int y, int width, int height, int frame_style);
int HandleMainMenuButtonControlEvent(void *control_ptr, int event_type);
int HandlePortraitMainMenuControlEvent(void *control_ptr, int event_type);

void __cdecl FUN_004f2aa1(int param_1, int param_2, int param_3, int param_4, char *param_5);
void FUN_005626b0(char *param_1, int param1, int param2, int param3, int param4);
int BeginMenuContext(void);
int ResetMenuContext(int context_index);
int AddMenuControlsToContext(AdvMenuControl *controls, int control_count, int context_index);
int EndMenuContext(void);
int UpdateMenuControlSelection(int mouse_x, int mouse_y, int allow_activate_on_click);
int QueuePendingMenuActionInput(void);
int FUN_005001e3(void);
void UpdateMouseSnapshot(void);

AdvMenuRect *PushGraphicsClipRect(AdvMenuRect *saved_clip_rect, FacemakerWindowBounds *page, int x, int y, int width, int height);

void DrawEncodedImageResampled(FacemakerWindowBounds *dst, int x, int y, int width, int height, EncodedImage *encoded_image);
void DrawEncodedImageUnscaled(FacemakerWindowBounds *dst, int x, int y, EncodedImage *encoded_image);
void DrawTextAt(FacemakerWindowBounds *window, int color, int x, int y, char *text);
void DrawWorldUiFormattedText(FacemakerWindowBounds *window, int color_index, int x, int y, char *format, ...);
void DrawCenteredTextLineWithShadow(char *text, int center_x, int y, int color_index);

extern EncodedImage *g_questnew_sprite_entries[4];
extern EncodedImage *g_tips_frame_sprite;
extern EncodedImage *g_tips_icon_sprite;
extern EncodedImage *g_main_menu_button_sprites_normal[4];
extern EncodedImage *g_main_menu_button_sprites_highlight[4];

// GLOBAL: SHANDALAR 0x00582d40
int DAT_00582d40[3] = { 0x40, 0x50, 0x66 };
// GLOBAL: SHANDALAR 0x00582d50
int DAT_00582d50[3] = { 0x48, 0x5a, 0x72 };
// GLOBAL: SHANDALAR 0x00582d60
int DAT_00582d60[3] = { 0xcb, 0xfd, 0x143 };
// GLOBAL: SHANDALAR 0x00582d6c
int DAT_00582d6c[3] = { 0x1af, 0x21a, 0x2b0 };
// GLOBAL: SHANDALAR 0x00582d78
int DAT_00582d78[3] = { 0x2f, 0x3c, 0x4d };

int sound_unload(int sound_id);
void FUN_00562835(char *filename, int sound_id);
void FUN_005627ee(int sound_id, int volume, int pan);

extern int DAT_00591220;
extern int DAT_0059126c;
extern int DAT_0059121c;

// GLOBAL: SHANDALAR 0x00591284
char s_x_sound_dueltune_wav_00591284[] = "x:sound\\dueltune.wav";
// GLOBAL: SHANDALAR 0x0059129c
char s_x_sound_winduel_wav_0059129c[] = "x:sound\\winduel.wav";
// GLOBAL: SHANDALAR 0x005912b0
char s_x_sound_loseduel_wav_005912b0[] = "x:sound\\loseduel.wav";
// GLOBAL: SHANDALAR 0x00591278
char *PTR_s_x_sound_dueltune_wav_00591278[3] = {
    s_x_sound_dueltune_wav_00591284,
    s_x_sound_winduel_wav_0059129c,
    s_x_sound_loseduel_wav_005912b0,
};

// GLOBAL: SHANDALAR 0x005b7e1c
int g_current_town_slot_index;

// GLOBAL: SHANDALAR 0x00580da4
void(__cdecl *DAT_00580da4)();

// GLOBAL: SHANDALAR 0x008e2e50
unsigned int DAT_008e2e50[16];

// GLOBAL: SHANDALAR 0x0058a888
int DAT_0058a888 = -1;
// GLOBAL: SHANDALAR 0x0058a88c
int DAT_0058a88c = -1;

// GLOBAL: SHANDALAR 0x005b7da0
int DAT_005b7da0;
// GLOBAL: SHANDALAR 0x005b7da4
int DAT_005b7da4;
// GLOBAL: SHANDALAR 0x005b7da8
int DAT_005b7da8[8];
// GLOBAL: SHANDALAR 0x005b7de8
int DAT_005b7de8[8];
// GLOBAL: SHANDALAR 0x005b7e34
int DAT_005b7e34;
// GLOBAL: SHANDALAR 0x005b7e38
int DAT_005b7e38;

// GLOBAL: SHANDALAR 0x007483ec
int _DAT_007483ec;
// GLOBAL: SHANDALAR 0x007481e0
int _DAT_007481e0;
// GLOBAL: SHANDALAR 0x007481d4
int _DAT_007481d4;
// GLOBAL: SHANDALAR 0x007481d0
int _DAT_007481d0;
// GLOBAL: SHANDALAR 0x007481cc
int _DAT_007481cc;
// GLOBAL: SHANDALAR 0x007481d8
int _DAT_007481d8;
// GLOBAL: SHANDALAR 0x007483e8
int _DAT_007483e8;

// GLOBAL: SHANDALAR 0x007481f0
char DAT_007481f0[0x64];
// GLOBAL: SHANDALAR 0x00748254
char DAT_00748254[0x64];
// GLOBAL: SHANDALAR 0x007482b8
char DAT_007482b8[0x64];
// GLOBAL: SHANDALAR 0x0074831c
char DAT_0074831c[0x64];
// GLOBAL: SHANDALAR 0x00748380
char DAT_00748380[0x64];

// GLOBAL: SHANDALAR 0x0058ab58
void(__cdecl *PTR_FUN_0058ab58)(void);
// GLOBAL: SHANDALAR 0x0058ab70
int _DAT_0058ab70;

// GLOBAL: SHANDALAR 0x0058abdc
char DAT_0058abdc[] = "\x1b \0\0\x1b \0\0\x1b \0\0\x1b \0\0";

// GLOBAL: SHANDALAR 0x0058ad7c
char s_x_sound_button2_wav_0058ad7c[] = "x:sound\\button2.wav";
// GLOBAL: SHANDALAR 0x0058ad90
char s_x_sound_button_wav_0058ad90[] = "x:sound\\button.wav";

// GLOBAL: SHANDALAR 0x0058adfc
char s_village_pic_0058adfc[] = "village.pic\0city.pic";

// GLOBAL: SHANDALAR 0x0058aef0
char s_x_sound_button2_wav_0058aef0[] = "x:sound\\button2.wav";
// GLOBAL: SHANDALAR 0x0058af1c
char s_x_sound_button2_wav_0058af1c[] = "x:sound\\button2.wav";
// GLOBAL: SHANDALAR 0x0058af48
char s_x_sound_button2_wav_0058af48[] = "x:sound\\button2.wav";
// GLOBAL: SHANDALAR 0x0058af8c
char s_x_sound_button2_wav_0058af8c[] = "x:sound\\button2.wav";

// GLOBAL: SHANDALAR 0x0058aed8
char s_village_pic_0058aed8[] = "village.pic\0city.pic";
// GLOBAL: SHANDALAR 0x0058af04
char s_village_pic_0058af04[] = "village.pic\0city.pic";
// GLOBAL: SHANDALAR 0x0058af30
char s_village_pic_0058af30[] = "village.pic\0city.pic";
// GLOBAL: SHANDALAR 0x0058af5c
char s_village_pic_0058af5c[] = "village.pic\0city.pic";
// GLOBAL: SHANDALAR 0x0058afa0
char s_village_pic_0058afa0[] = "village.pic\0city.pic";

int __cdecl FUN_004ef0a8(AdvMenuControl *control, int mode);
void FUN_004ef07e(void);
void FUN_004f042c(void);
void FUN_004f04c5(void);
void FUN_004f18c0(void);
void FUN_004f1951(void);
void FUN_004f19e9(void);
void FUN_004f1a67(void);
void FUN_004f1b4e(void);
void FUN_004f0d09(void);

// GLOBAL: SHANDALAR 0x0058a890
AdvMenuControl DAT_0058a890[4] = {
    {3, 0x47, 0x39, 0x36, 3, 0x47, 0x39, 0x36, 1, (AdvMenuRenderCallback)HandleMainMenuButtonControlEvent, (AdvMenuActivateCallback)FUN_004f1951, 0x70, 0, 0, 0, 0, 0, {0, 0, 0, 0}},
    {3, 0x7e, 0x39, 0x36, 3, 0x7e, 0x39, 0x36, 1, (AdvMenuRenderCallback)HandleMainMenuButtonControlEvent, (AdvMenuActivateCallback)FUN_004f19e9, 0x71, 0, 0, 0, 0, 0, {0, 0, 0, 0}},
    {3, 0xb5, 0x39, 0x36, 3, 0xb5, 0x39, 0x36, 1, (AdvMenuRenderCallback)HandleMainMenuButtonControlEvent, (AdvMenuActivateCallback)FUN_004f1a67, 0x73, 0, 0, 0, 0, 0, {0, 0, 0, 0}},
    {3, 0xec, 0x39, 0x36, 3, 0xec, 0x39, 0x36, 1, (AdvMenuRenderCallback)HandlePortraitMainMenuControlEvent, (AdvMenuActivateCallback)FUN_004f1b4e, 0x74, 0, 0, 0, 0, 0, {0, 0, 0, 0}},
};

// GLOBAL: SHANDALAR 0x0058a9e0
AdvMenuControl DAT_0058a9e0[5] = {
    {0x181, 0x30, 0xaa, 0x5d, 0x181, 0x30, 0xaa, 0x5d, 1, (AdvMenuRenderCallback)FUN_004ef0a8, (AdvMenuActivateCallback)FUN_004f1951, 5, 0, 0, 0, 0, 0, {0, 0, 0, 0}},
    {0x54, 0xea, 0xaa, 0x5d, 0x54, 0xea, 0xaa, 0x5d, 1, (AdvMenuRenderCallback)FUN_004ef0a8, (AdvMenuActivateCallback)FUN_004f042c, 1, 1, 0, 0, 0, 0, {0, 0, 0, 0}},
    {0x181, 0xea, 0xaa, 0x5d, 0x181, 0xea, 0xaa, 0x5d, 1, (AdvMenuRenderCallback)FUN_004ef0a8, (AdvMenuActivateCallback)FUN_004ef07e, 2, 2, DAT_0058abdc, DAT_0058abdc + 4, 0, 0, {0, 0, 0, 0}},
    {0x54, 0x30, 0xaa, 0x5d, 0x54, 0x30, 0xaa, 0x5d, 1, (AdvMenuRenderCallback)FUN_004ef0a8, (AdvMenuActivateCallback)FUN_004f0d09, 4, 3, 0, 0, 0, 0, {0, 0, 0, 0}},
    {0xea, 0x8d, 0xaa, 0x5d, 0xea, 0x8d, 0xaa, 0x5d, 1, (AdvMenuRenderCallback)FUN_004ef0a8, (AdvMenuActivateCallback)FUN_004f04c5, 3, 4, 0, 0, 0, 0, {0, 0, 0, 0}},
};

// GLOBAL: SHANDALAR 0x0058ac2c
char s_0246_pic_0058ac2c[] = "0246.pic";
// GLOBAL: SHANDALAR 0x0058ac44
char s_0364_pic_0058ac44[] = "0364.pic";
// GLOBAL: SHANDALAR 0x0058ac5c
char s_0335_pic_0058ac5c[] = "0335.pic";
// GLOBAL: SHANDALAR 0x0058ac74
char s_0737_pic_0058ac74[] = "0737.pic";
// GLOBAL: SHANDALAR 0x0058ac8c
char s_0028_pic_0058ac8c[] = "0028.pic";

// GLOBAL: SHANDALAR 0x0058aca4
char s_0246_pic_0058aca4[] = "0246.pic";
// GLOBAL: SHANDALAR 0x0058acbc
char s_0364_pic_0058acbc[] = "0364.pic";
// GLOBAL: SHANDALAR 0x0058acd4
char s_0335_pic_0058acd4[] = "0335.pic";
// GLOBAL: SHANDALAR 0x0058acec
char s_0737_pic_0058acec[] = "0737.pic";
// GLOBAL: SHANDALAR 0x0058ad04
char s_0028_pic_0058ad04[] = "0028.pic";

// GLOBAL: SHANDALAR 0x0058ad10
char s_village_pic_0058ad10[] = "village.pic\0city.pic";
// GLOBAL: SHANDALAR 0x0058abec
char s_village_pic_0058abec[] = "village.pic\0city.pic";
// GLOBAL: SHANDALAR 0x0058ad54
char s_village_pic_0058ad54[] = "village.pic\0city.pic";

// GLOBAL: SHANDALAR 0x0058ad28
char s_x_sound_manalink_wav_0058ad28[] = "x:sound\\manalink.wav";
// GLOBAL: SHANDALAR 0x0058ad40
char s_x_sound_reward_wav_0058ad40[] = "x:sound\\reward.wav";
// GLOBAL: SHANDALAR 0x0058ad6c
char s_worlbak1_pic_0058ad6c[] = "worlbak1.pic";

// GLOBAL: SHANDALAR 0x0058b1d0
char s_prdfrmc_pic_0058b1d0[] = "prdfrmc.pic\0prdfrmb.pic";
// GLOBAL: SHANDALAR 0x0058b1e8
char s_advfac64_pic_0058b1e8[] = "advfac64.pic";
// GLOBAL: SHANDALAR 0x0058b1f8
char s_prdblk_pic_0058b1f8[] = "prdblk.pic";
// GLOBAL: SHANDALAR 0x0058b204
char s_faces__03d_pic_0058b204[] = "faces\\%03d.pic";

// GLOBAL: SHANDALAR 0x00580e8c
char DAT_00580e8c[] = "rt\0";
// GLOBAL: SHANDALAR 0x00580e90
char s_______00580e90[] = "%[^\n]\0\0";
// GLOBAL: SHANDALAR 0x00580e98
char DAT_00580e98[] = "%[\n]\0\0\0";
// GLOBAL: SHANDALAR 0x00580ea0
char s_______00580ea0[] = "%[^\n]\0\0";
// GLOBAL: SHANDALAR 0x00580ea8
char s__d__d_00580ea8[] = "%d %d\0\0";
// GLOBAL: SHANDALAR 0x00580eb0
char s__vNONE_00580eb0[] = ".vNONE\0";
// GLOBAL: SHANDALAR 0x00580eb8
char s__vBLACK_00580eb8[] = ".vBLACK";
// GLOBAL: SHANDALAR 0x00580ec0
char s__vBLUE_00580ec0[] = ".vBLUE\0";
// GLOBAL: SHANDALAR 0x00580ec8
char s__vRED_00580ec8[] = ".vRED\0\0";
// GLOBAL: SHANDALAR 0x00580ed0
char s__vGREEN_00580ed0[] = ".vGREEN";
// GLOBAL: SHANDALAR 0x00580ed8
char s__vWHITE_00580ed8[] = ".vWHITE";
// GLOBAL: SHANDALAR 0x00580ee0
char s__vFAST_00580ee0[] = ".vFAST\0";
// GLOBAL: SHANDALAR 0x00580ee8
char s__vLARGE_00580ee8[] = ".vLARGE";
// GLOBAL: SHANDALAR 0x00580ef0
char s__vDIRECT_00580ef0[] = ".vDIRECT\0\0\0";
// GLOBAL: SHANDALAR 0x00580efc
char s__vARTIFACT_00580efc[] = ".vARTIFACT\0";
// GLOBAL: SHANDALAR 0x00580f08
char s_4th_Edition_00580f08[] = "4th Edition";
// GLOBAL: SHANDALAR 0x00580f14
char DAT_00580f14[] = "%[\n]\0\0\0";
// GLOBAL: SHANDALAR 0x0058b214
char s_advfac64_pic_0058b214[] = "advfac64.pic";
// GLOBAL: SHANDALAR 0x0058b224
char s_prdblk_pic_0058b224[] = "prdblk.pic";

// GLOBAL: SHANDALAR 0x0058cb70
char s_decks_0_0058cb70[] = "decks\\0";
// GLOBAL: SHANDALAR 0x0058cb78
char DAT_0058cb78[] = "0";
// GLOBAL: SHANDALAR 0x0058cb7c
char DAT_0058cb7c[] = "0";
// GLOBAL: SHANDALAR 0x0058cb80
char DAT_0058cb80[] = ".dck";
// GLOBAL: SHANDALAR 0x0058cb88
char s_decks_0179_dck_0058cb88[] = "decks\\0179.dck";

// GLOBAL: SHANDALAR 0x005916bc
char s_x_sound_newsflash_wav_005916bc[] = "x:sound\\newsflash.wav\0\0\0newsback.pic";
// GLOBAL: SHANDALAR 0x005916d4
char s_newsback_pic_005916d4[] = "newsback.pic";

// GLOBAL: SHANDALAR 0x005919f4
char s_uth_arz_pic_005919f4[] = "uth-arz.pic";

// GLOBAL: SHANDALAR 0x005a1870
int DAT_005a1870[0x14] = {5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79};

// FUNCTION: SHANDALAR 0x0056615f
int FUN_0056615f(int wizard_color)
{
  if (wizard_color == 1)
  {
    return 0;
  }
  else if (wizard_color == 2)
  {
    return 1;
  }
  else if (wizard_color == 3)
  {
    return 0;
  }
  else if (wizard_color == 4)
  {
    return 1;
  }
  else if (wizard_color == 5)
  {
    return 0;
  }
}

// FUNCTION: SHANDALAR 0x004ceae2
void DestroyCachedCardArt(void)
{
  DestroyAllCardBackgrounds();
  DestroyAllBigArts();
  DestroyAllSmallArts();
}

// FUNCTION: SHANDALAR 0x0042fe00
int FUN_0042fe00(FacemakerWindowBounds *page)
{
  struct
  {
    int screen_width;
    AdvMenuRect clip_restore;
    AdvMenuRect clip_temp;
    int resolution_index;
    EncodedImage *questnew_entry;
    AdvMenuRect saved_clip_rect;
  } s;

  s.questnew_entry = g_questnew_sprite_entries[0];
  s.screen_width = global_screen_width;

  switch (s.screen_width)
  {
  case 0x280:
    s.resolution_index = 0;
    break;
  case 0x320:
    s.resolution_index = 1;
    break;
  case 0x400:
    s.resolution_index = 2;
    break;
  }

  s.saved_clip_rect =
      *PushGraphicsClipRect(&s.clip_temp, PTR_DAT_005832b4, ScaleUiCoordinate(0x40), ScaleUiCoordinate(0x30), FUN_005501dc(0x100), FUN_005501dc(0x8c));

  DrawEncodedImageResampled(page, ScaleUiCoordinate(0x181), ScaleUiCoordinate(0x135), ScaleUiCoordinate((int)*(short *)((char *)s.questnew_entry + 4)),
                            ScaleUiCoordinate((int)*(short *)((char *)s.questnew_entry + 6)), g_questnew_sprite_entries[0]);

  DrawEncodedImageUnscaled(page, DAT_00582d40[s.resolution_index], DAT_00582d50[s.resolution_index], g_tips_frame_sprite);
  DrawEncodedImageUnscaled(page, DAT_00582d60[s.resolution_index], DAT_00582d78[s.resolution_index], g_tips_icon_sprite);
  DrawEncodedImageUnscaled(page, DAT_00582d6c[s.resolution_index], DAT_00582d78[s.resolution_index], g_tips_icon_sprite);

  PushGraphicsClipRect(&s.clip_restore, PTR_DAT_005832b4, s.saved_clip_rect.x, s.saved_clip_rect.y, s.saved_clip_rect.width, s.saved_clip_rect.height);
  return 0;
}

// FUNCTION: SHANDALAR 0x004ed44b
void LoadVisitBackdropAndCopyToPage0(char *pcx_path)
{
  int dst_x;
  int dst_y;
  int dst_w;
  int dst_h;

  dst_x = ScaleUiCoordinate(0x40);
  dst_y = ScaleUiCoordinate(0x30);
  dst_w = ScaleUiCoordinate(0x200);
  dst_h = ScaleUiCoordinate(0x118);

  LoadPcxResource(1, 0, global_screen_height - 0x118, pcx_path, (void *)0);
  StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, global_screen_height - 0x118, 0x200, 0x118, PTR_DAT_005832dc,
                          dst_x, dst_y, dst_w, dst_h);
  FUN_0042fe00(PTR_DAT_005832dc);
  BlitGraphicsRect(PTR_DAT_005832dc, dst_x, dst_y, dst_w, dst_h, PTR_DAT_005832b4, dst_x, dst_y);
}

// FUNCTION: SHANDALAR 0x004ed525
void AnimateVisitBackdropZoomIn(char *pcx_path)
{
  int dst_y;
  int copy_w;
  int copy_h;
  int center_y;
  int scale_w;
  int scale_h;
  int scale_step;

  dst_y = global_screen_height - 0x118;
  LoadPcxResource(1, 0, dst_y, pcx_path, (void *)0);

  copy_w = FUN_005501dc(0x100);
  copy_h = FUN_005501dc(0x8c);
  center_y = FUN_005501dc(0x5e);

  for (scale_step = 2; scale_step < 9; scale_step = scale_step + 1)
  {
    scale_w = (int)(copy_w * scale_step + ((copy_w * scale_step >> 0x1f) & 7U)) >> 3;
    scale_h = (int)(copy_h * scale_step + ((copy_h * scale_step >> 0x1f) & 7U)) >> 3;
    StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, dst_y, 0x200, 0x118, PTR_DAT_005832b4,
                            global_screen_width / 2 - scale_w / 2, center_y - scale_h / 2, scale_w, scale_h);
  }

  StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, dst_y, 0x200, 0x118, PTR_DAT_005832dc,
                          global_screen_width / 2 - copy_w / 2, center_y - copy_h / 2, copy_w, copy_h);
  FUN_0042fe00(PTR_DAT_005832b4);
}

// FUNCTION: SHANDALAR 0x004ed6b9
int ShowWizardVisitBackdrop(char *pcx_path)
{
  DIBSurface *src;
  DIBSurface *dst;
  int x;
  int y;
  int w;
  int h;

  x = ScaleUiCoordinate(0x40);
  y = ScaleUiCoordinate(0x30);
  w = ScaleUiCoordinate(0x200);
  h = ScaleUiCoordinate(0x118);

  src = g_graphics_pages[PTR_DAT_005832dc->page_number];
  dst = g_graphics_pages[PTR_DAT_005832b4->page_number];

  LoadPcxResource(1, 0, global_screen_height - 0x118, pcx_path, (void *)0);
  StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, global_screen_height - 0x118, 0x200, 0x118, PTR_DAT_005832dc,
                          x, y, w, h);
  FUN_0042fe00(PTR_DAT_005832dc);
  FUN_0057d500(dst->hTempDC, x, y, w, h, 0x20, 3, 8, 8, src->hTempDC);
  InitializeNewGameState();
  return 0;
}

// FUNCTION: SHANDALAR 0x00561bf4
void FUN_00561bf4(int param_1)
{
  int ruled_by_color_count[7];
  int lair_world_x_by_color[7];
  int lair_world_y_by_color[7];
  int scan_index;
  int town_index;
  int best_score;
  int best_town_index;
  int best_color;
  int nearest_color;
  int nearest_distance;
  int dist;
  int score;
  int creature_tier;
  int creature_type;
  size_t msg_len;

  if (g_siege_indicator != 0)
  {
    return;
  }

  PopNormalizedQueuedKeyInput();

  for (scan_index = 0; scan_index < 7; scan_index = scan_index + 1)
  {
    lair_world_x_by_color[scan_index] = -1;
    ruled_by_color_count[scan_index] = 0;
  }

  for (scan_index = 0; scan_index < 0x80; scan_index = scan_index + 1)
  {
    if (g_town_slots[scan_index].location_type == 4)
    {
      unsigned int tile_type;
      unsigned int tile_magic_mask;
      int tile_color_index;

      tile_type = FUN_0043146b(g_town_slots[scan_index].world_x, g_town_slots[scan_index].world_y);
      tile_magic_mask = FUN_005611c8(tile_type);
      tile_color_index = FUN_0040dffd((int)tile_magic_mask);
      if (tile_color_index == param_1)
      {
        lair_world_x_by_color[tile_color_index] = g_town_slots[scan_index].world_x;
        lair_world_y_by_color[tile_color_index] = g_town_slots[scan_index].world_y;
      }
    }

    if (*(char *)((char *)&g_town_slots[scan_index].status_and_ruling_wizard + 1) != '\0')
    {
      ++ruled_by_color_count[((int)(g_town_slots[scan_index].status_and_ruling_wizard & 0xffffff3fU) >> 6)];
    }
  }

  best_score = 0x7fff;
  best_town_index = -1;
  best_color = -1;

  for (town_index = 0; town_index < 0x80; town_index = town_index + 1)
  {
    if ((((*(char *)((char *)&g_town_slots[town_index].status_and_ruling_wizard + 1) == '\0') && (g_town_slots[town_index].location_type != 4)) &&
         (g_town_slots[town_index].location_type != 1)) &&
        (g_town_slots[town_index].location_type != 5))
    {
      nearest_distance = 0x7fff;
      nearest_color = -1;
      for (scan_index = 1; scan_index < 6; scan_index = scan_index + 1)
      {
        if (lair_world_x_by_color[scan_index] != -1)
        {
          dist = FUN_004ecf30(g_town_slots[town_index].world_x - lair_world_x_by_color[scan_index],
                              g_town_slots[town_index].world_y - lair_world_y_by_color[scan_index]);
          if (dist < nearest_distance)
          {
            nearest_distance = dist;
            nearest_color = scan_index;
          }
        }
      }

      score = FUN_00522508(0x80);
      score = score + ruled_by_color_count[nearest_color] * 0x20;
      if (score < best_score)
      {
        best_score = score;
        best_town_index = town_index;
        best_color = nearest_color;
      }
    }
  }

  if (best_color != -1)
  {
    switch ((int)(g_siege_timer + (g_siege_timer >> 0x1f & 0x7fU)) >> 7)
    {
    case 0:
      creature_tier = 4;
      break;
    case 1:
      creature_tier = 6;
      break;
    case 2:
      creature_tier = 8;
      break;
    case 3:
      creature_tier = 0xc;
      break;
    default:
      if ((g_town_slots[best_town_index].world_y & 1) == 0)
      {
        creature_tier = 0x10;
      }
      else
      {
        creature_tier = 0xc;
      }
      break;
    }

    FreeOpeningMenuSpriteWorkEntries(7, 0xf);
    creature_type = FUN_0056139e(best_color, creature_tier);
    g_lair_or_monster_slots[7].entry_type = creature_type;
    g_lair_or_monster_slots[7].world_x = g_town_slots[best_town_index].world_x * 0x20 + 0x10;
    g_lair_or_monster_slots[7].world_y = g_town_slots[best_town_index].world_y * 0x20 + 0x10;
    g_lair_or_monster_slots[7].color = best_color;
    DAT_0073ea68 = DAT_0073ea68 + 1;
    FUN_00431526(0x80, g_town_slots[best_town_index].world_x, g_town_slots[best_town_index].world_y);

    PlaySoundEffectOnChannel(s_x_sound_newsflash_wav_005916bc, 0x97, 100, 100, 0);
    AnimateVisitBackdropZoomIn(s_newsback_pic_005916d4);

    strcpy(g_ui_message_buffer, gs_newsflash_0077d140[0]);
    if (FUN_0056615f(best_color) == 0)
    {
      BuildTownDisplayName(best_town_index);
      BuildCreatureNameWithArticle(g_lair_or_monster_slots[7].entry_type);
      msg_len = strlen(g_ui_message_buffer);
      FUN_00564e70(g_ui_message_buffer + msg_len, 0x1000, gs_newsflash_0077d140[3]);
    }
    else
    {
      BuildTownDisplayName(best_town_index);
      BuildCreatureNameWithArticle(g_lair_or_monster_slots[7].entry_type);
      msg_len = strlen(g_ui_message_buffer);
      FUN_00564e70(g_ui_message_buffer + msg_len, 0x1000, gs_newsflash_0077d140[2]);
    }

    PTR_DAT_005832b4->font_slot = 5;
    DrawTextAt(PTR_DAT_005832b4, 0xbe, 0x140, 0xf7, g_ui_message_buffer);
    PTR_DAT_005832b4->font_slot = 1;
    PopNormalizedQueuedKeyInput();
    ClearInputAndWaitForMouseRelease();
    RefreshAdventureInterfaceLayout();
    g_siege_indicator = 1;
  }
}

// FUNCTION: SHANDALAR 0x00430fb5
void FUN_00430fb5(char *text, int center_x_320, int y_200, int color_index)
{
  DrawCenteredTextLineWithShadow(text, (center_x_320 * global_screen_width) / 0x140, (y_200 * global_screen_height) / 0xf0, color_index);
}

// FUNCTION: SHANDALAR 0x005661d9
int FUN_005661d9(int creature_type)
{
  if ((creature_type < 0) || ((int)DAT_00593934 - 1 < creature_type))
  {
    return 0;
  }
  if (gs_creature_names_00591a08[creature_type].plural[0x32] == 'f')
  {
    return 1;
  }
  return 0;
}

// FUNCTION: SHANDALAR 0x0056bfa4
void FUN_0056bfa4(int deck_slot_index)
{
  int i;

  for (i = deck_slot_index + 1; i < 500; i = i + 1)
  {
    deck[i - 1] = deck[i];
  }
  deck[499] = -1;
}

// FUNCTION: SHANDALAR 0x005626b0
void FUN_005626b0(char *param_1, int param1, int param2, int param3, int param4)
{
  Sound local_24;

  sound_unload(param1);
  (void)LoadSoundWithDriveFallback(param_1, param1, 0);
  memset(&local_24, 0, 0x20);
  local_24.volume = param2 << 2;
  local_24.sampleRate = (param3 * 0x5622) / 100;
  local_24.pan = param4 << 2;
  local_24.flags = local_24.flags & 0xffffffeeU;
  sound_play(param1, &local_24);
}

// FUNCTION: SHANDALAR 0x004f2aa1
void __cdecl FUN_004f2aa1(int param_1, int param_2, int param_3, int param_4, char *param_5)
{
  (void)param_1;
  (void)param_2;
  (void)param_3;
  (void)param_4;
  (void)param_5;
  // TODO(decomp): Renders an icon-button with optional tooltip bubble/label (used by city-screen controls).
}

// FUNCTION: SHANDALAR 0x004ef0a8
int __cdecl FUN_004ef0a8(AdvMenuControl *control, int mode)
{
  int in_bounds;

  if (g_menu_render_guard == 0)
  {
    if ((g_mouse_x < control->x) || ((control->x + control->width) < g_mouse_x))
    {
      in_bounds = 0;
    }
    else if ((g_mouse_y < control->y) || ((control->y + control->height) < g_mouse_y))
    {
      in_bounds = 0;
    }
    else
    {
      in_bounds = 1;
    }

    if (in_bounds == 0)
    {
      return 0;
    }
  }

  if (control->state == 3)
  {
    return 0;
  }

  FUN_004f2aa1(control->x + control->width / 2, control->y + control->height / 2, control->selection_value - 1, mode,
              DAT_007481f0 + control->unk_30 * 100);

  if ((mode == 2) && (control->on_activate != (AdvMenuActivateCallback)0))
  {
    FUN_00562736(0x12, 100, 100, 0);
    control->on_activate(control);
  }

  return 1;
}

// FUNCTION: SHANDALAR 0x004ef07e
void FUN_004ef07e(void)
{
  FUN_005626b0(s_x_sound_button2_wav_0058ad7c, 0xf, 100, 100, 0);
  _DAT_007481d8 = -1;
}

// FUNCTION: SHANDALAR 0x004f042c
void FUN_004f042c(void)
{
  if (_DAT_007481d0 <= Gold)
  {
    g_food = g_food + 10;
    Gold = Gold - _DAT_007481d0;
  }

  FUN_0055060c(1);
  PTR_DAT_005832b4->page_number = 1;
  LoadVisitBackdropAndCopyToPage0(s_village_pic_0058adfc + (((g_town_slots[g_current_town_slot_index].location_type == 1) - 1) & 0xc));
  PTR_DAT_005832b4->page_number = 0;

  // 0x0058aa34 == &DAT_0058a9e0[1]
  (void)FUN_004ef0a8(&DAT_0058a9e0[1], 1);
}

// FUNCTION: SHANDALAR 0x004f04c5
void FUN_004f04c5(void)
{
  // TODO(decomp): Wiseman/quest flow for city screen. Large (quest generation, UI prompts, journal, timers).
  _DAT_007481d8 = -2;
}

// FUNCTION: SHANDALAR 0x004f18c0
void FUN_004f18c0(void)
{
  // TODO(decomp): City trade flow. This likely calls into the card-chooser UI and modifies the town's trade timer.
  *(int *)(g_town_slots[_DAT_007483ec].data_18_to_63 + 0x40) = g_quest_restock_timer;
  RefreshAdventureInterfaceLayout();
  FUN_0055060c(1);
  AnimateVisitBackdropZoomIn(s_village_pic_0058aed8 + (((g_town_slots[_DAT_007483ec].location_type == 1) - 1) & 0xc));
  _DAT_007481d8 = -2;
}

// FUNCTION: SHANDALAR 0x004f1951
void FUN_004f1951(void)
{
  int town_index;

  town_index = _DAT_007483ec;
  FUN_005626b0(s_x_sound_button2_wav_0058aef0, 0xf, 100, 100, 0);
  AnimatePaletteToColor(0, DAT_00589dec);
  DeckBuilderMain(g_main_window_hwnd, 1, 3);
  RestoreAdventureUiPaletteAndFocus();
  RefreshAdventureInterfaceLayout();
  FUN_0055060c(1);
  AnimateVisitBackdropZoomIn(s_village_pic_0058af04 + (((g_town_slots[town_index].location_type == 1) - 1) & 0xc));
  _DAT_007481d8 = -2;
}

// FUNCTION: SHANDALAR 0x004f19e9
void FUN_004f19e9(void)
{
  int town_index;

  town_index = _DAT_007483ec;
  FUN_005626b0(s_x_sound_button2_wav_0058af1c, 0xf, 100, 100, 0);
  ShowWorldMapScreen(0);
  RefreshAdventureInterfaceLayout();
  FUN_0055060c(1);
  AnimateVisitBackdropZoomIn(s_village_pic_0058af30 + (((g_town_slots[town_index].location_type == 1) - 1) & 0xc));
  _DAT_007481d8 = -2;
}

// FUNCTION: SHANDALAR 0x004f1a67
void FUN_004f1a67(void)
{
  int town_index;

  town_index = _DAT_007483ec;
  FUN_005626b0(s_x_sound_button2_wav_0058af48, 0xf, 100, 100, 0);
  ShowDungeonCluesScreen();
  RefreshAdventureInterfaceLayout();
  FUN_0055060c(1);
  AnimateVisitBackdropZoomIn(s_village_pic_0058af5c + (((g_town_slots[town_index].location_type == 1) - 1) & 0xc));
  _DAT_007481d8 = -2;
}

// FUNCTION: SHANDALAR 0x004f1b4e
void FUN_004f1b4e(void)
{
  int town_index;

  town_index = _DAT_007483ec;
  FUN_005626b0(s_x_sound_button2_wav_0058af8c, 0xf, 100, 100, 0);
  PopNormalizedQueuedKeyInput();
  FUN_00549002();
  RefreshAdventureInterfaceLayout();
  FUN_0055060c(1);
  AnimateVisitBackdropZoomIn(s_village_pic_0058afa0 + (((g_town_slots[town_index].location_type == 1) - 1) & 0xc));
  _DAT_007481d8 = -2;
}

// FUNCTION: SHANDALAR 0x004f0d09
void FUN_004f0d09(void)
{
  // TODO(decomp): City "buy cards" UI. Large.
  _DAT_007481d8 = -2;
}

// FUNCTION: SHANDALAR 0x004ef1ea
int FUN_004ef1ea(int town_index)
{
  int menu_context;
  int i;

  // TODO(decomp): Full town-services / city screen loop. This minimal version wires up the 4 main buttons
  // (deckbuilder/map/clues/stats) so the caller can keep original control flow while we decompile the rest.
  _DAT_007483ec = town_index;
  _DAT_007481d8 = 0;

  // Fill sprite pointers (the render callback expects these).
  for (i = 0; i < 4; i = i + 1)
  {
    DAT_0058a890[i].mode_data[0] = (int)g_main_menu_button_sprites_normal[i];
    DAT_0058a890[i].mode_data[1] = (int)g_main_menu_button_sprites_highlight[i];
    DAT_0058a890[i].mode_data[2] = (int)g_main_menu_button_sprites_highlight[i];
    DAT_0058a890[i].mode_data[3] = (int)g_main_menu_button_sprites_normal[i];
  }

  AnimateVisitBackdropZoomIn(s_village_pic_0058adfc + (((g_town_slots[town_index].location_type == 1) - 1) & 0xc));
  EnsureAdvfac64Loaded(1);

  menu_context = BeginMenuContext();
  (void)AddMenuControlsToContext(DAT_0058a890, 4, menu_context);
  (void)FUN_005001e3();

  while (_DAT_007481d8 == 0)
  {
    UpdateMouseSnapshot();
    (void)UpdateMenuControlSelection(g_mouse_x_snapshot, g_mouse_y_snapshot, (g_mouse_button_down_mask != 0));
  }

  (void)EndMenuContext();
  return 0;
}

// FUNCTION: SHANDALAR 0x00568320
int FUN_00568320(unsigned int param_1, int param_2)
{
  (void)param_1;
  (void)param_2;
  // TODO(decomp): Enters the duel engine. Spawns a worker thread, runs the duel, then restores UI/palette/state.
  // This is a huge call tree; keep it stubbed until we explicitly decide to tackle duel code.
  return 0;
}

// FUNCTION: SHANDALAR 0x005636ab
int FUN_005636ab(void)
{
  int deck_slot;
  int usable_card_count;
  int font_size;
  int text_y;

  usable_card_count = 0;
  for (deck_slot = 0; deck_slot < 500; deck_slot = deck_slot + 1)
  {
    if (((deck[deck_slot] != -1) && ((((unsigned char *)&deck[deck_slot])[1] & 0x40) == 0)) && (4 < (deck[deck_slot] & 0xfffU)))
    {
      usable_card_count = usable_card_count + 1;
    }
  }

  if (usable_card_count == 0)
  {
    AnimatePaletteToColor(0, DAT_00589dec);
    LoadPcxIntoPageOpaque(1, s_uth_arz_pic_005919f4);

    PTR_DAT_005832dc->font_slot = 5;
    if ((global_screen_width == 0x280) || (global_screen_width == 0x320))
    {
      font_size = 0x14;
    }
    else
    {
      font_size = 0x10;
    }
    SetFontStyleSize(5, (unsigned int)ScaleUiCoordinate(font_size));

    strcpy(g_ui_message_buffer, gs_questfailed_0077c580[0]);
    strcat(g_ui_message_buffer, gs_questfailed_0077c580[1]);
    strcat(g_ui_message_buffer, gs_questfailed_0077c580[4]);
    strcat(g_ui_message_buffer, gs_questfailed_0077c580[5]);

    text_y = 0x1e0 - GetFontLineHeight(PTR_DAT_005832dc->font_slot) * 7;
    DrawFormattedTextShadowedCentered(PTR_DAT_005832dc, 0xea, 0x140, text_y, g_ui_message_buffer);

    StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, 0, 0x280, 0x1e0, PTR_DAT_005832b4, 0, 0, global_screen_width, global_screen_height);
    PopNormalizedQueuedKeyInput();
    ClearInputAndWaitForMouseRelease();
    AnimatePaletteToColor(0, DAT_00589dec);
    DAT_009300f0 = 1;
    UnloadStatWinDllExports();
    FUN_00469099();
    exit(0);
  }
  return 0;
}

// FUNCTION: SHANDALAR 0x0056139e
int FUN_0056139e(int wizard_color, int creature_tier)
{
  int creature_type;
  int tries;

  tries = 0;
  while (1)
  {
    creature_type = FUN_00522508((int)DAT_00593934 - 1) + 1;
    tries = tries + 1;
    if (999 <= tries)
    {
      break;
    }

    if ((int)(signed char)gs_creature_names_00591a08[creature_type].metadata[1] == creature_tier)
    {
      if (wizard_color == 0)
      {
        break;
      }
      if (((1 << (unsigned char)wizard_color) & (int)(signed char)gs_creature_names_00591a08[creature_type].metadata[4]) != 0)
      {
        break;
      }
    }
  }

  if (tries == 999)
  {
    return 0;
  }

  return creature_type;
}

// FUNCTION: SHANDALAR 0x0050a5f1
void FUN_0050a5f1(int creature_type, unsigned int name_id, unsigned int param_3, int param_4)
{
  char *num_str;
  int deck_number;

  (void)name_id;

  deck_number = *(int *)&gs_creature_names_00591a08[creature_type].metadata[6];

  strcpy(g_ui_message_buffer, s_decks_0_0058cb70);
  if (deck_number < 100)
  {
    strcat(g_ui_message_buffer, DAT_0058cb78);
    if (deck_number < 10)
    {
      strcat(g_ui_message_buffer, DAT_0058cb7c);
    }
  }

  num_str = _itoa(deck_number, unk_009266d0 + 0x70, 10);
  strcat(g_ui_message_buffer, num_str);
  strcat(g_ui_message_buffer, DAT_0058cb80);

  if (FileExists(g_ui_message_buffer) == 0)
  {
    FUN_005222ae(s_decks_0179_dck_0058cb88, 1, param_3, param_4);
  }
  else
  {
    FUN_005222ae(g_ui_message_buffer, 1, param_3, param_4);
  }

  DAT_0057a750 = 1;
}

// FUNCTION: SHANDALAR 0x005222ae
void FUN_005222ae(char *param_1, int param_2, unsigned int param_3, int param_4)
{
  int i;

  for (i = 0; i < 200; i = i + 1)
  {
    initial_library[param_2][i].numcards = 0;
    initial_library[param_2][i].csvid = initial_library[param_2][i].numcards;
  }
  FUN_00417e11(param_1, (int)(initial_library + param_2), param_3, param_4);
}

// FUNCTION: SHANDALAR 0x00417e11
int FUN_00417e11(char *param_1, int param_2, unsigned int param_3, int param_4)
{
  struct
  {
    unsigned int entry_index;
    unsigned int last_basic_entry_index;
    int csvid;
    int numcards;
    int header_line_index;
    char *space;
    int total_cards;
    int speed_filter;
    unsigned int color_filter;
    char line[0x200];
    int deck_type;
    FILE *f;
    int scan_ret;
    int card_index;
  } s;

  strcpy(DAT_0097e340, param_1);
  s.f = fopen(param_1, DAT_00580e8c);
  if (s.f == NULL)
  {
    s.deck_type = 0;
  }
  else
  {
    s.deck_type = 0;
    s.header_line_index = 0;
    s.entry_index = 0;

    s.scan_ret = fscanf(s.f, s_______00580e90, s.line);
    s.scan_ret = fscanf(s.f, DAT_00580e98, s.line);

    s.color_filter = 0;
    s.speed_filter = -1;
    s.last_basic_entry_index = 0xffffffff;
    s.total_cards = 0;
    s.numcards = -1;
    s.csvid = -1;

    do
    {
      s.scan_ret = fscanf(s.f, s_______00580ea0, s.line);
      if (s.line[0] == '.')
      {
        if (s.line[1] == 'v')
        {
          s.space = strchr(s.line, ' ');
          if (s.space != NULL)
          {
            *s.space = '\0';
          }

          if (_strcmpi(s.line, s__vNONE_00580eb0) == 0)
          {
            s.color_filter = 1;
          }
          if (_strcmpi(s.line, s__vBLACK_00580eb8) == 0)
          {
            s.color_filter = 2;
          }
          if (_strcmpi(s.line, s__vBLUE_00580ec0) == 0)
          {
            s.color_filter = 4;
          }
          if (_strcmpi(s.line, s__vRED_00580ec8) == 0)
          {
            s.color_filter = 0x10;
          }
          if (_strcmpi(s.line, s__vGREEN_00580ed0) == 0)
          {
            s.color_filter = 8;
          }
          if (_strcmpi(s.line, s__vWHITE_00580ed8) == 0)
          {
            s.color_filter = 0x20;
          }

          if (_strcmpi(s.line, s__vFAST_00580ee0) == 0)
          {
            s.speed_filter = 0;
          }
          if (_strcmpi(s.line, s__vLARGE_00580ee8) == 0)
          {
            s.speed_filter = 1;
          }
          if (_strcmpi(s.line, s__vDIRECT_00580ef0) == 0)
          {
            s.speed_filter = 2;
          }
          if (_strcmpi(s.line, s__vARTIFACT_00580efc) == 0)
          {
            s.speed_filter = 6;
          }
        }
        else
        {
          sscanf(s.line + 1, s__d__d_00580ea8, &s.csvid, &s.numcards);
          s.total_cards = s.total_cards + s.numcards;

          if (((s.color_filter == 0) || ((param_3 & s.color_filter) != 0)) && ((s.speed_filter == -1) || (s.speed_filter == param_4)))
          {
            *(int *)(param_2 + s.entry_index * 8) = s.csvid;
            *(int *)(param_2 + 4 + s.entry_index * 8) = s.numcards;

            s.card_index = FUN_0056c705(s.csvid);

            if (s.numcards == 0)
            {
              s.numcards = g_shandalar_difficulty;
            }

            if (s.card_index == -1)
            {
              if ((s.last_basic_entry_index != 0xffffffff) && (s.numcards < *(int *)(param_2 + 4 + s.last_basic_entry_index * 8)))
              {
                *(int *)(param_2 + 4 + s.last_basic_entry_index * 8) =
                    *(int *)(param_2 + 4 + s.last_basic_entry_index * 8) - (int)((s.entry_index & 1) + s.numcards) / 2;
              }
            }
            else if (s.card_index < 5)
            {
              s.last_basic_entry_index = s.entry_index;
            }
          }
        }
        s.entry_index = s.entry_index + 1;
      }
      else
      {
        s.header_line_index = s.header_line_index + 1;
        if (s.header_line_index == 5)
        {
          s.deck_type = atoi(s.line + 1);
        }
        else if ((s.header_line_index == 6) && (strcmp(s.line + 1, s_4th_Edition_00580f08) != 0))
        {
          s.deck_type = -2;
        }
      }
      s.scan_ret = fscanf(s.f, DAT_00580f14, s.line);
    } while ((((int)s.entry_index < 200) && (s.total_cards < 500) && (s.scan_ret != -1)) && ((s.csvid != 0) || (s.numcards != 0)));

    fclose(s.f);

    if (s.total_cards < 0x28)
    {
      s.deck_type = -3;
    }
    if (((499 < s.total_cards) || (199 < (int)s.entry_index)) && (s.scan_ret != -1))
    {
      s.deck_type = -4;
    }
    if (0x37 < s.deck_type)
    {
      s.deck_type = -1;
    }
  }

  return s.deck_type;
}

// FUNCTION: SHANDALAR 0x0052234e
int FUN_0052234e(int library_index)
{
  struct
  {
    int rand_pick;
    int chosen_csvid;
    int total_cards;
    int running_pick;
    int i;
  } s;

  if (library_index == -1)
  {
    return -1;
  }

  s.total_cards = 0;
  for (s.i = 0; s.i < 200; s.i = s.i + 1)
  {
    s.total_cards = s.total_cards + initial_library[library_index][s.i].numcards;
  }

  if (s.total_cards == 0)
  {
    return -1;
  }

  s.rand_pick = FUN_00522508(s.total_cards);
  s.running_pick = s.rand_pick;

  for (s.i = 0; s.i < 200; s.i = s.i + 1)
  {
    s.running_pick = s.running_pick - initial_library[library_index][s.i].numcards;
    if (s.running_pick < 0)
    {
      s.chosen_csvid = initial_library[library_index][s.i].csvid;
      if (DAT_008bd200 != 1)
      {
        initial_library[library_index][s.i].numcards = initial_library[library_index][s.i].numcards + -1;
      }
      break;
    }
  }

  for (s.i = 0; (s.i < g_card_count && ((int)global_cards_data[s.i].id != s.chosen_csvid)); s.i = s.i + 1)
  {
  }

  return s.i;
}

// FUNCTION: SHANDALAR 0x004f6886
void FUN_004f6886(int creature_type, int x_320, int y_200, int tinted, int mode)
{
  DIBSurface *src_page;
  DIBSurface *dst_page;
  int scaled_w;
  int scaled_h;
  int scaled_x;
  int scaled_y;
  int dst_right;
  int dst_bottom;
  int text_y;
  char *name_ptr;
  EncodedImage *portrait_frame_sprite;
  EncodedImage *face_sprite;
  unsigned int portrait_panel_x;
  char face_path[24];

  LoadPcxIntoPage(1, s_prdfrmc_pic_0058b1d0 + (((tinted != 0) - 1) & 0xc));
  PTR_DAT_005832dc->font_slot = 4;
  PTR_DAT_005832b4->font_slot = 4;

  if (tinted != 0)
  {
    ApplyPortraitPaletteMap(PTR_DAT_005832dc, 1, 0x15a, 0xb3, 0x23, s_prdblk_pic_0058b1f8, s_advfac64_pic_0058b1e8);
  }

  BeginSpriteEncodeSession();
  portrait_frame_sprite = EncodeSpriteFromPage(1, 1, 0x15a, 0xb3, 0x23);
  FinalizeSpriteEncodeSession();

  dst_right = global_screen_width - ScaleUiCoordinate(0xb4);
  dst_bottom = global_screen_height - ScaleUiCoordinate(0xb4);

  if (mode == 1)
  {
    PTR_DAT_005832b4->page_number = 1;
  }

  scaled_h = ScaleUiCoordinate((int)portrait_frame_sprite->height);
  scaled_w = ScaleUiCoordinate((int)portrait_frame_sprite->width);
  scaled_y = y_200;
  scaled_x = x_320 - ScaleUiCoordinate((int)portrait_frame_sprite->width / 2);
  DrawEncodedImageResampled(PTR_DAT_005832b4, scaled_x, scaled_y, scaled_w, scaled_h, portrait_frame_sprite);

  name_ptr = FUN_00561441(creature_type);
  strcpy(g_ui_message_buffer, name_ptr);
  text_y = y_200 + ScaleUiCoordinate((int)portrait_frame_sprite->height / 2);
  DrawFormattedTextNoShadowCentered(PTR_DAT_005832b4, (-(unsigned int)(tinted == 0) & 0x38) + 0xae, x_320, text_y,
                                    g_ui_message_buffer);

  PTR_DAT_005832b4->font_slot = 1;
  if (mode == 1)
  {
    PTR_DAT_005832b4->page_number = 0;
  }

  scaled_h = ScaleUiCoordinate(0xb4);
  scaled_w = ScaleUiCoordinate(0xb4);
  StretchBlitGraphicsRect(PTR_DAT_005832dc, 1, 0xa5, 0xb4, 0xb4, PTR_DAT_005832dc, dst_right, dst_bottom, scaled_w, scaled_h);

  sprintf(face_path, s_faces__03d_pic_0058b204, creature_type);
  LoadPcxResource(1, 0, global_screen_height - 0xf0, face_path, (void *)0);
  BeginSpriteEncodeSession();
  face_sprite = EncodeSpriteFromPage(1, 0, global_screen_height - 0xf0, 0x8a, 0xaa);
  FinalizeSpriteEncodeSession();

  scaled_h = ScaleUiCoordinate(0xaa);
  scaled_w = ScaleUiCoordinate(0x8a);
  scaled_y = dst_bottom + ScaleUiCoordinate(5);
  scaled_x = dst_right + ScaleUiCoordinate(0x15);
  DrawEncodedImageResampled(PTR_DAT_005832dc, scaled_x, scaled_y, scaled_w, scaled_h, face_sprite);

  if (tinted != 0)
  {
    ApplyPortraitPaletteMap(PTR_DAT_005832dc, dst_right, dst_bottom, (unsigned int)ScaleUiCoordinate(0xb4), ScaleUiCoordinate(0xb4), s_prdblk_pic_0058b224,
                            s_advfac64_pic_0058b214);
  }

  if (mode == 0)
  {
    scaled_y = y_200 + ScaleUiCoordinate(0x2d);
    scaled_x = x_320 - ScaleUiCoordinate(0x5a);
    BlitGraphicsRect(PTR_DAT_005832dc, dst_right, dst_bottom, ScaleUiCoordinate(0xb4), ScaleUiCoordinate(0xb4), PTR_DAT_005832b4, scaled_x, scaled_y);
  }
  else
  {
    src_page = g_graphics_pages[PTR_DAT_005832dc->page_number];
    dst_page = g_graphics_pages[PTR_DAT_005832b4->page_number];

    scaled_y = y_200 + ScaleUiCoordinate(0x2d);
    scaled_x = x_320 - ScaleUiCoordinate(0x5a);
    BlitGraphicsRect(PTR_DAT_005832dc, dst_right, dst_bottom, ScaleUiCoordinate(0xb4), ScaleUiCoordinate(0xb4), PTR_DAT_005832dc, scaled_x, scaled_y);
    BlitRectByRandomTileOrderInPlace(dst_page->hTempDC, x_320 - ScaleUiCoordinate(0x5a), scaled_y, ScaleUiCoordinate(0xb4),
                                     ScaleUiCoordinate(0xb4), 3, 3, src_page->hTempDC);

    if (mode == 1)
    {
      BlitRectByRandomTileOrderInPlace(dst_page->hTempDC,
                                       x_320 - ScaleUiCoordinate((int)portrait_frame_sprite->width / 2 + -0xb), y_200,
                                       ScaleUiCoordinate(portrait_frame_sprite->width + -0x16),
                                       ScaleUiCoordinate((int)portrait_frame_sprite->height), 2, 2, src_page->hTempDC);
    }
  }

  FreeSpriteBlob(face_sprite);
  FreeSpriteBlob(portrait_frame_sprite);
}

// FUNCTION: SHANDALAR 0x005629c3
void FUN_005629c3(int tune_index)
{
  if (DAT_00591220 != -1)
  {
    sound_unload(0x10);
  }
  DAT_00591220 = 0x15;
  DAT_0059126c = -1;
  FUN_00562835(PTR_s_x_sound_dueltune_wav_00591278[tune_index], 0x10);
  FUN_005627ee(0x10, 0x80, 0);
  DAT_0059121c = 1;
}

// FUNCTION: SHANDALAR 0x004f6d32
void FUN_004f6d32(unsigned int card_id)
{
  int deck_slot;

  deck_slot = 0;
  while (1)
  {
    deck_slot = deck_slot + 1;
    if (deck_slot >= 0x1f4)
    {
      break;
    }
    if (((unsigned int)deck[deck_slot] & 0xfff) == card_id)
    {
      FUN_0056bfa4(deck_slot);
      break;
    }
  }
}

// FUNCTION: SHANDALAR 0x00532aeb
unsigned int FUN_00532aeb(char *title, unsigned int color_mask, unsigned int type_mask, int param_4, int param_5)
{
  (void)title;
  (void)color_mask;
  (void)type_mask;
  (void)param_4;
  (void)param_5;
  // TODO(decomp): Large "choose a card" UI (trade-like list). Returns selected card id or 0xffffffff on cancel.
  return 0xffffffff;
}

// FUNCTION: SHANDALAR 0x00530b7e
int FUN_00530b7e(void)
{
  // TODO(decomp): Callback used by the card-chooser UI while a "quest card marking" flow is active.
  return 0;
}

// FUNCTION: SHANDALAR 0x004eda50
int VisitTownSlot(int town_index)
{
  struct
  {
    unsigned int tile_type;
    unsigned int tile_magic_mask;
    int tile_color_index;
    int menu_result;
    size_t msg_len;
    char *amulet_name;
    char *format;
    char *wizard_name;
    char *pics[6];
    int quest_handled;
    unsigned int journal_param;

    int deck_slot_plus_one;
    int deck_card_index;

    int world_magic_slot_index;
    int nearest_distance;
    int scan_index;
    int distance;

    int text_x;
    int text_y;

    EncodedImage *sprite;
    // used for matching-only raw offset reads
    int sprite_ptr_as_int;
    int sprite_draw_w;
    int sprite_draw_h;
    int menu_x_320;
    int local_dx;
    int local_dy;

    int found_deck_slot;

    int duel_wizard_color;
    int duel_creature_type;
    int duel_ante_slot;
    int duel_deck_seed;
    /* Matching-only: keep original stack size (MSVC switch temp lives below this). */
    int duel_menu_result;
  } s;

  g_current_town_slot_index = town_index;
  ClearInputAndWaitForMouseRelease();

  // NOTE: byte 1 of status_and_ruling_wizard is treated as the "ruling wizard" byte by the original code.
  if (*(char *)((char *)&g_town_slots[town_index].status_and_ruling_wizard + 1) != 0)
  {
    // Duel path: wizard-controlled towns/dungeons.
    AnimateVisitBackdropZoomIn(s_village_pic_0058abec + (((g_town_slots[town_index].location_type == 1) - 1) & 0xc));
    EnsureAdvfac64Loaded(1);

    for (s.duel_ante_slot = 0; s.duel_ante_slot < 0x10; s.duel_ante_slot = s.duel_ante_slot + 1)
    {
      DAT_008e2e50[s.duel_ante_slot] = 0xffffffff;
      *(unsigned int *)(global_ante_cards[0] + s.duel_ante_slot * 4) = DAT_008e2e50[s.duel_ante_slot];
    }

    (void)FUN_005636ab();
    s.duel_deck_seed = g_town_slots[town_index].world_y;
    for (s.duel_ante_slot = 0; s.duel_ante_slot < g_town_slots[town_index].world_x % 3 + 1; s.duel_ante_slot = s.duel_ante_slot + 1)
    {
      do
      {
        do
        {
          s.found_deck_slot = s.duel_deck_seed % 500;
          s.duel_deck_seed = s.duel_deck_seed + 7;
        } while (deck[s.found_deck_slot] == -1);
      } while ((deck[s.found_deck_slot] & 0x4000) != 0 || ((deck[s.found_deck_slot] & 0xfff) <= 4));

      *(unsigned int *)(global_ante_cards[0] + s.duel_ante_slot * 4) = deck[s.found_deck_slot] & 0xfff;
      FUN_004f2407(deck[s.found_deck_slot] & 0xfff, s.duel_ante_slot * 0x28 + 0x60, s.duel_ante_slot * 3 + 0x80, 1,
                   gs_visit_0077c4f0[0]);
    }

    s.duel_wizard_color = g_town_slots[town_index].status_and_ruling_wizard >> 8;
    switch (g_siege_timer / 0x80)
    {
    case 0:
      s.duel_creature_type = 4;
      break;
    case 1:
      s.duel_creature_type = 6;
      break;
    case 2:
      s.duel_creature_type = 8;
      break;
    case 3:
      s.duel_creature_type = 0xc;
      break;
    case 4:
      s.duel_creature_type = 0x10;
      break;
    default:
      if ((g_town_slots[town_index].world_y & 1) != 0)
      {
        s.duel_creature_type = 0x12;
      }
      else
      {
        s.duel_creature_type = 0xe;
      }
      break;
    }

    s.duel_creature_type = FUN_0056139e(s.duel_wizard_color, s.duel_creature_type);
    FUN_0050a5f1(s.duel_creature_type, 0xffffffff, 0, -1);

    do
    {
      do
      {
        DAT_008e2e50[0] = FUN_0052234e(DAT_0057a750);
      } while (DAT_008e2e50[0] <= 4);
    } while ((global_cards_data[DAT_008e2e50[0]].extra_ability & 0x100) != 0);

    FUN_004f2407((int)DAT_008e2e50[0], 0xe0, 0x40, 1, gs_visit_0077c4f0[1]);
    FUN_004f6886(s.duel_creature_type, 0xa0, 0x20, 1, 2);

    if (FUN_0056615f(s.duel_wizard_color) != 0)
    {
      sprintf(g_ui_message_buffer, gs_visit_0077c4f0[3], gs_wizardnames_0077ee70 + s.duel_wizard_color * 0x32);
    }
    else
    {
      sprintf(g_ui_message_buffer, gs_visit_0077c4f0[2], gs_wizardnames_0077ee70 + s.duel_wizard_color * 0x32);
    }

    if (FUN_005661d9(s.duel_creature_type) != 0)
    {
      s.msg_len = strlen(g_ui_message_buffer);
      sprintf(g_ui_message_buffer + s.msg_len, gs_visit_0077c4f0[4], FUN_00561441(s.duel_creature_type));
      strcat(g_ui_message_buffer, gs_visit_0077c4f0[6]);
    }
    else
    {
      s.msg_len = strlen(g_ui_message_buffer);
      sprintf(g_ui_message_buffer + s.msg_len, gs_visit_0077c4f0[5], FUN_00561441(s.duel_creature_type));
      strcat(g_ui_message_buffer, gs_visit_0077c4f0[7]);
    }

    if (RunTextMenuAtScaled(g_ui_message_buffer, 0x78, 0x38) == 1)
    {
      unk_00789308 = FUN_0056c705(DAT_0058c620[FUN_00522508(3) + (s.duel_wizard_color * 3 - 3) + 20]);
      g_lair_or_monster_slots[6].color = s.duel_wizard_color;
      g_lair_or_monster_slots[6].entry_type = s.duel_creature_type;

      s.deck_card_index = FUN_0056c705(*(int *)&gs_creature_names_00591a08[s.duel_creature_type].metadata[6]);
      FUN_0050a5f1(s.duel_creature_type, (unsigned int)s.deck_card_index, 0, -1);

      DAT_008ce538 = s.duel_wizard_color;
      DAT_00742fd0 = 0;
      DAT_007a7874 = 3;

      s.duel_menu_result = FUN_00568320((unsigned int)s.deck_card_index, s.duel_creature_type);
      if (s.duel_menu_result == 1)
      {
        FUN_005629c3(1);
        *(volatile int *)&g_town_slots[town_index].status_and_ruling_wizard &= 0xffff00ff;

        LoadPcxIntoPageOpaque(1, "celeb.pic");
        StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, 0, 0x280, 0x1e0, PTR_DAT_005832b4, 0, 0, global_screen_width, global_screen_height);

        sprintf(g_ui_message_buffer, gs_visit_0077c4f0[8], BuildTownDisplayName(town_index));
        FUN_0056bd9d((unsigned int)DAT_008e2e50[0]);
        RunTextMenuAtScaled(g_ui_message_buffer, 0x14, 0x14);
        FUN_004290e2(7, town_index);
      }
      else
      {
        FUN_005629c3(2);
        for (s.duel_ante_slot = 0; s.duel_ante_slot < 3; s.duel_ante_slot = s.duel_ante_slot + 1)
        {
          s.tile_magic_mask = *(unsigned int *)(global_ante_cards[0] + s.duel_ante_slot * 4);
          if (s.tile_magic_mask != 0xffffffff)
          {
            LoadPcxIntoPageOpaque(1, "losedul2.pic");
            StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, 0, 0x280, 0x1e0, PTR_DAT_005832b4, 0, 0, global_screen_width, global_screen_height);
            FUN_004f2407((int)s.tile_magic_mask, 0x17, 0x50, 1, gs_visit_0077c4f0[9]);
            ClearInputAndWaitForMouseRelease();
            WaitForInputEventUnlessBlocked();
            FUN_004f6d32(s.tile_magic_mask);
          }
        }
      }
    }

    EnsureAdvfac64Loaded(0);
    sound_stop(0x10);
    DestroyCachedCardArt();
    return 0;
  }

  // non-duel
  if (g_town_slots[town_index].location_type == 4)
  {
    s.pics[0] = (char *)0;
    s.pics[1] = s_0246_pic_0058ac2c;
    s.pics[2] = s_0364_pic_0058ac44;
    s.pics[3] = s_0335_pic_0058ac5c;
    s.pics[4] = s_0737_pic_0058ac74;
    s.pics[5] = s_0028_pic_0058ac8c;

    s.tile_type = FUN_0043146b(g_town_slots[town_index].world_x, g_town_slots[town_index].world_y);
    s.tile_magic_mask = FUN_005611c8(s.tile_type);
    s.tile_color_index = (int)FUN_0040dffd((int)s.tile_magic_mask);
    DAT_008ce538 = s.tile_color_index;

    ShowWizardVisitBackdrop(s.pics[s.tile_color_index]);
    PTR_DAT_005832b4->font_slot = 1;

    s.wizard_name = gs_wizardnames_0077ee70 + s.tile_color_index * 0x32;
    if (FUN_0056615f(s.tile_color_index) == 0)
    {
      sprintf(g_ui_message_buffer, gs_visit_0077c4f0[0xb], s.wizard_name);
    }
    else
    {
      sprintf(g_ui_message_buffer, gs_visit_0077c4f0[10], s.wizard_name);
    }

    strcat(g_ui_message_buffer, gs_visit_0077c4f0[0xc]);
    s.menu_result = RunTextMenuAtScaled(g_ui_message_buffer, 0x2a, 0x1a);
    if (s.menu_result == 1)
    {
      sound_stop(0x10);
      FUN_0050a5c1(s.tile_color_index - 1);
      if (((g_defeated_wizards_bitmap & (1 << ((unsigned char)s.tile_color_index & 0x1f))) == 0) && (g_shandalar_difficulty == 3))
      {
        FUN_00561bf4(s.tile_color_index);
      }
    }

    DestroyCachedCardArt();
    return 0;
  }

  if (g_town_slots[town_index].location_type == 5)
  {
    s.pics[0] = (char *)0;
    s.pics[1] = s_0246_pic_0058aca4;
    s.pics[2] = s_0364_pic_0058acbc;
    s.pics[3] = s_0335_pic_0058acd4;
    s.pics[4] = s_0737_pic_0058acec;
    s.pics[5] = s_0028_pic_0058ad04;

    s.tile_type = FUN_0043146b(g_town_slots[town_index].world_x, g_town_slots[town_index].world_y);
    s.tile_magic_mask = FUN_005611c8(s.tile_type);
    s.tile_color_index = (int)FUN_0040dffd((int)s.tile_magic_mask);
    DAT_008ce538 = s.tile_color_index;

    ShowWizardVisitBackdrop(s.pics[s.tile_color_index]);

    s.wizard_name = gs_wizardnames_0077ee70 + s.tile_color_index * 0x32;
    if (FUN_0056615f(s.tile_color_index) == 0)
    {
      sprintf(g_ui_message_buffer, gs_visit_0077c4f0[0xe], s.wizard_name);
    }
    else
    {
      sprintf(g_ui_message_buffer, gs_visit_0077c4f0[0xd], s.wizard_name);
    }
    strcat(g_ui_message_buffer, gs_visit_0077c4f0[0xf]);
    RunTextMenuAtScaled(g_ui_message_buffer, 0x2a, 0x1a);

    sound_stop(0x10);
    DestroyCachedCardArt();
    return 0;
  }

  if ((g_current_quest_destination == town_index) && ((-1 < g_current_quest_type) || (g_current_quest_type < -100)))
  {
    AnimateVisitBackdropZoomIn(s_village_pic_0058ad10 + (((g_town_slots[town_index].location_type == 1) - 1) & 0xc));
    s.quest_handled = 0;
    FUN_004290e2(0x10, g_current_quest_type);

    if ((g_current_quest_type == 0) || (g_current_quest_type == 2))
    {
      strcpy(g_ui_message_buffer, gs_visit_0077c4f0[0x10]);
      if (g_current_quest_type == 0)
      {
        strcat(g_ui_message_buffer, gs_visit_0077c4f0[0x11]);
        sound_unload(0xf);
        LoadSoundWithDriveFallback(s_x_sound_manalink_wav_0058ad28, 0xf, 0);
        FUN_00562736(0xf, 100, 100, 0);
        g_town_slots[g_current_quest_destination].status_and_ruling_wizard =
            g_town_slots[g_current_quest_destination].status_and_ruling_wizard | 1;
      }
      else if (g_current_quest_type == 2)
      {
        s.amulet_name = gs_amuletnames_0077d090 + g_current_quest_color * 0x19;
        s.format = gs_visit_0077c4f0[0x12];
        s.msg_len = strlen(g_ui_message_buffer);
        sprintf(g_ui_message_buffer + s.msg_len, s.format, s.amulet_name);
        sound_unload(0xf);
        LoadSoundWithDriveFallback(s_x_sound_reward_wav_0058ad40, 0xf, 0);
        FUN_00562736(0xf, 100, 100, 0);
        DAT_0078990c[g_current_quest_color] = DAT_0078990c[g_current_quest_color] + 1;
      }

      PTR_DAT_005832b4->font_slot = 1;
      RunTextMenuAtScaled(g_ui_message_buffer, 0x50, 0x50);
      s.quest_handled = 1;
      g_current_quest_destination = -1;
      RefreshAdventureInterfaceLayout();
      FUN_0055060c(1);
    }

    if ((g_current_quest_type == 1) &&
        ((s.menu_result = FindDeckSlotForQuestColorAndType((unsigned char)g_current_quest_color,
                                                          (unsigned char)(1 << ((unsigned char)g_current_quest_destination & 3)))) != 0))
    {
      s.deck_slot_plus_one = FindDeckSlotForQuestColorAndType((unsigned char)g_current_quest_color,
                                                              (unsigned char)(1 << ((unsigned char)g_current_quest_destination & 3)));
      DAT_0078990c[g_current_quest_color] = DAT_0078990c[g_current_quest_color] + 1;

      sprintf(g_ui_message_buffer, gs_visit_0077c4f0[0x13],
              (char *)(((((int *)((char *)deck - 4))[s.deck_slot_plus_one] & 0xfff) * 0x48) + 0x594209));

      s.amulet_name = gs_amuletnames_0077d090 + g_current_quest_color * 0x19;
      s.format = gs_visit_0077c4f0[0x14];
      s.msg_len = strlen(g_ui_message_buffer);
      sprintf(g_ui_message_buffer + s.msg_len, s.format, s.amulet_name);

      PTR_DAT_005832b4->font_slot = 1;
      g_text_menu_abort_requested = 1;
      RunTextMenuAtScaled(g_ui_message_buffer, 0x50, 0x50);

      g_town_slots[g_current_quest_destination].status_and_ruling_wizard =
          g_town_slots[g_current_quest_destination].status_and_ruling_wizard | 1;
      s.quest_handled = 1;
      FUN_0056bfa4(s.deck_slot_plus_one - 1);
      ClearInputAndWaitForMouseRelease();
      g_current_quest_destination = -1;
      RefreshAdventureInterfaceLayout();
      FUN_0055060c(1);
    }

    if (g_current_quest_type < -100)
    {
      if (g_town_slots[g_current_quest_destination].location_type < 2)
      {
        g_current_quest_type = g_current_quest_type + 100;
        s.deck_card_index = (int)gs_creature_names_00591a08[-g_current_quest_type].plural[0x33] / 7 + 1;
        if (FUN_005661d9(-g_current_quest_type) == 0)
        {
          sprintf(g_ui_message_buffer, gs_visit_0077c4f0[0x1b], FUN_00561441(-g_current_quest_type));
        }
        else
        {
          sprintf(g_ui_message_buffer, gs_visit_0077c4f0[0x1a], FUN_00561441(-g_current_quest_type));
        }

        if (s.deck_card_index == 1)
        {
          s.amulet_name = gs_amuletnames_0077d090 + g_current_quest_color * 0x19;
          s.format = gs_visit_0077c4f0[0x12];
          s.msg_len = strlen(g_ui_message_buffer);
          sprintf(g_ui_message_buffer + s.msg_len, s.format, s.amulet_name);
        }
        else
        {
          s.format = gs_visit_0077c4f0[0x1c];
          s.msg_len = strlen(g_ui_message_buffer);
          FUN_00564e70(g_ui_message_buffer + s.msg_len, 0x1000, s.format);
        }

        RunTextMenuAtScaled(g_ui_message_buffer, 0x50, 0x50);
        DAT_0078990c[g_current_quest_color] = DAT_0078990c[g_current_quest_color] + s.deck_card_index;
        g_town_slots[g_current_quest_destination].status_and_ruling_wizard =
            g_town_slots[g_current_quest_destination].status_and_ruling_wizard | 1;
      }
      else
      {
        g_current_quest_type = g_current_quest_type + 100;
        s.deck_card_index = (int)gs_creature_names_00591a08[-g_current_quest_type].plural[0x33] / 7 + 1;
        if (FUN_005661d9(-g_current_quest_type) == 0)
        {
          sprintf(g_ui_message_buffer, gs_visit_0077c4f0[0x16], FUN_00561441(-g_current_quest_type));
        }
        else
        {
          sprintf(g_ui_message_buffer, gs_visit_0077c4f0[0x15], FUN_00561441(-g_current_quest_type));
        }

        s.format = gs_visit_0077c4f0[0x17];
        s.msg_len = strlen(g_ui_message_buffer);
        sprintf(g_ui_message_buffer + s.msg_len, s.format, FUN_004f2e17(g_current_quest_destination));
        RunTextMenuAtScaled(g_ui_message_buffer, 0x50, 0x50);

        // TODO(decomp): This branch continues into the large "choose a card" UI (FUN_00532aeb) and then
        // marks the selected card slot with 0x4000.
      }

      s.quest_handled = 1;
      if (DAT_0058a88c == g_current_quest_destination)
      {
        DAT_0058a88c = -1;
      }
      g_current_quest_destination = -1;
      RefreshAdventureInterfaceLayout();
      FUN_0055060c(1);
    }

    if (s.quest_handled == 0)
    {
      s.journal_param = (unsigned int)town_index;
    }
    else
    {
      s.journal_param = (unsigned int)town_index | 0x80;
    }
    FUN_004290e2(1, (int)s.journal_param);
  }
  else
  {
    FUN_004290e2(1, town_index);
  }

  g_town_slots[town_index].status_and_ruling_wizard = g_town_slots[town_index].status_and_ruling_wizard | 2;
  *(int *)(g_town_slots[town_index].data_18_to_63 + 0x44) = *(int *)(g_town_slots[town_index].data_18_to_63 + 0x44) + 1;

  AnimateVisitBackdropZoomIn(s_village_pic_0058ad54 + (((g_town_slots[town_index].location_type == 1) - 1) & 0xc));
  // TODO(decomp): Town services UI (shops/healer/clues/map/stats/etc.).
  // This is currently not decompiled in-tree; keep the call to preserve control flow.
  FUN_004ef1ea(town_index);

  *(int *)(g_town_slots[town_index].data_18_to_63 + 0x48) = g_quest_restock_timer;

  if ((g_town_slots[town_index].location_type == 3) || (g_town_slots[town_index].location_type == 2))
  {
    s.world_magic_slot_index = -1;
    s.nearest_distance = 999;
    for (s.scan_index = 0; s.scan_index < 0xc; s.scan_index = s.scan_index + 1)
    {
      if (*(int *)(Scards + s.scan_index * 0x10 + 8) != 0)
      {
        s.distance = FUN_004ecf30(g_town_slots[town_index].world_x - g_town_slots[*(int *)(Scards + s.scan_index * 0x10 + 8)].world_x,
                                 g_town_slots[town_index].world_y - g_town_slots[*(int *)(Scards + s.scan_index * 0x10 + 8)].world_y);
        if (s.distance < s.nearest_distance)
        {
          s.world_magic_slot_index = s.scan_index;
          s.nearest_distance = s.distance;
        }
      }
    }

    if ((s.world_magic_slot_index != -1) && ((g_world_magic_bitmap & (1 << ((unsigned char)s.world_magic_slot_index & 0x1f))) == 0))
    {
      LoadPcxIntoPage(1, s_worlbak1_pic_0058ad6c);
      StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, 0, 0x280, 0x1e0, PTR_DAT_005832b4, 0, 0, global_screen_width, global_screen_height);

      (void)FUN_004bb458(s.world_magic_slot_index);

      s.sprite = ((EncodedImage **)&g_world_magic_choice_button_sprite_bank)[s.world_magic_slot_index];
      s.sprite_ptr_as_int = (int)s.sprite;

      s.text_y = (0x4e - *(short *)(s.sprite_ptr_as_int + 6)) / 2 + 0x4c;
      s.text_x = (0x4f - *(short *)(s.sprite_ptr_as_int + 4)) / 2 + 0x14f;

      s.sprite_draw_w = ScaleUiCoordinate((int)*(short *)(s.sprite_ptr_as_int + 4));
      s.sprite_draw_h = ScaleUiCoordinate((int)*(short *)(s.sprite_ptr_as_int + 6));
      DrawEncodedImageResampled(PTR_DAT_005832b4, ScaleUiCoordinate(s.text_x), ScaleUiCoordinate(s.text_y), s.sprite_draw_w, s.sprite_draw_h,
                                s.sprite);

      s.text_y = (s.text_y + *(short *)(s.sprite_ptr_as_int + 6) + 0x10) / 2;
      s.text_x = (s.text_x + (int)*(short *)(s.sprite_ptr_as_int + 4) / 2) / 2;

      PTR_DAT_005832b4->font_slot = 4;
      DrawTextAt(PTR_DAT_005832b4, 0x7b, 0x176, 0x4b, gs_visit_0077c4f0[0x1d]);

      s.text_y = s.text_y - 5;
      strcpy(g_ui_message_buffer, gs_worldmagic_names_00780660[s.world_magic_slot_index]);
      FUN_00430fb5(g_ui_message_buffer, s.text_x, s.text_y, 0x40);

      s.text_y = s.text_y + 8;
      sprintf(g_ui_message_buffer, gs_visit_0077c4f0[0x1e], *(int *)(Scards + s.world_magic_slot_index * 0x10 + 4) / 2);
      FUN_00430fb5(g_ui_message_buffer, s.text_x, s.text_y, 0x7b);

      s.text_y = s.text_y + 8;
      strcpy(g_ui_message_buffer, gs_worldmagic_explains_0074b8f0[s.world_magic_slot_index]);
      DrawWorldUiFormattedText(PTR_DAT_005832b4, 0x7b, FUN_005501dc(s.text_x), FUN_005501dc(s.text_y), g_ui_message_buffer);

      /* Matching: the original early-outs for "this town doesn't sell it" (distance != 0) via a jump. */
      do
      {
        if (s.nearest_distance != 0)
        {
          /* Message: where to find it (direction + town name). */
          s.local_dx =
              g_town_slots[*(int *)(Scards + s.world_magic_slot_index * 0x10 + 8)].world_x - g_town_slots[town_index].world_x;
          s.local_dy =
              g_town_slots[*(int *)(Scards + s.world_magic_slot_index * 0x10 + 8)].world_y - g_town_slots[town_index].world_y;

          s.text_y = s.text_y + 0x10;
          s.tile_color_index = GetRelativeWorldQuadrant(g_town_slots[*(int *)(Scards + s.world_magic_slot_index * 0x10 + 8)].world_x,
                                                       g_town_slots[*(int *)(Scards + s.world_magic_slot_index * 0x10 + 8)].world_y);
          s.wizard_name = BuildTownDisplayName(*(int *)(Scards + s.world_magic_slot_index * 0x10 + 8));
          s.format = gs_directions_00765d50 + s.tile_color_index * 0x19;
          FUN_00564e70(g_ui_message_buffer, 0x1000, gs_visit_0077c4f0[0x1f], s.format, s.wizard_name);

          s.text_y = s.text_y + 8;
          FUN_00430fb5(g_ui_message_buffer, s.text_x, s.text_y, 0x8d);
          s.text_y = s.text_y + 8;
          DAT_0058a888 = s.world_magic_slot_index;
          ClearInputAndWaitForMouseRelease();
          WaitForInputEventUnlessBlocked();
          break;
        }

        if ((*(int *)(Scards + s.world_magic_slot_index * 0x10 + 4) / 2) <= Gold)
        {
          /* Purchase prompt. */
          s.text_y = s.text_y + 0x10;
          FUN_00564e70(g_ui_message_buffer, 0x1000, gs_visit_0077c4f0[0x20], gs_worldmagic_names_00780660[s.world_magic_slot_index],
                       BuildTownDisplayName(*(int *)(Scards + s.world_magic_slot_index * 0x10 + 8)),
                       *(int *)(Scards + s.world_magic_slot_index * 0x10 + 4) / 2);
          strcat(g_ui_message_buffer, gs_visit_0077c4f0[0x21]);

          ClearInputAndWaitForMouseRelease();
          s.menu_result = RunTextMenuAtScaled(
              g_ui_message_buffer,
              ((((unsigned int)(global_screen_width - 0x280) < 1) ? 0xffffffce : 0) + 0xbe),
              0x88);

          DAT_0058a888 = s.world_magic_slot_index;
          if (s.menu_result == 1)
          {
            Gold = Gold - *(int *)(Scards + s.world_magic_slot_index * 0x10 + 4) / 2;
            g_world_magic_bitmap = g_world_magic_bitmap | (1 << (unsigned char)DAT_0058a888);
            *(int *)(Scards + DAT_0058a888 * 0x10 + 8) = 0;
            FUN_004290e2(6, DAT_0058a888);
          }
          DAT_0058a888 = -1;
        }
        else
        {
          /* Not enough gold. */
          s.text_y = s.text_y + 0x18;
          strcpy(g_ui_message_buffer, gs_visit_0077c4f0[0x22]);
          FUN_00430fb5(g_ui_message_buffer, s.text_x, s.text_y, 0xbe);
          s.text_y = s.text_y + 8;
          DAT_0058a888 = s.world_magic_slot_index;
          ClearInputAndWaitForMouseRelease();
          WaitForInputEventUnlessBlocked();
        }
      } while (0);
    }
  }

  DestroyCachedCardArt();
  if (g_current_quest_deadline < g_quest_restock_timer)
  {
    g_current_quest_destination = -1;
  }

  return 0;
}
