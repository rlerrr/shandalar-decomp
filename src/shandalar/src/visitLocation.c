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
extern int g_current_quest_giver_town_index;
extern int g_done_text_table_entry;

extern int card_dummy(int player, int card, event_t event);

typedef struct
{
  int first;
  int second;
} HintPair;

extern int DAT_0097df40[0x100];
extern HintPair DAT_0097e450[0x100];

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

typedef struct
{
  EncodedImage *frame[4][9];
  EncodedImage *icon_rows[4][4];
} DialogBoxSpriteBank;

extern DialogBoxSpriteBank g_dialog_box_sprite_bank;
extern EncodedImage *g_icons_sprite_entries[0x20];

typedef struct
{
  int unk_00;
  int timer;
  int unk_08;
  int unk_0c;
} WorldMagicSlotTimer;

extern WorldMagicSlotTimer g_world_magic_slot_timers[0xc];

typedef struct
{
  int x0;
  int y0;
  int x1;
  int y1;
} BuyCardsHitRect;

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
int ClampIntToRange(int value, int min_value, int max_value);
int MeasureTextLineWidth(char *text);

int PopNormalizedQueuedKeyInput(void);
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
int ReadSpriteEntryPointers(EncodedImage **out_sprite_entries, char *sprite_path);
void __cdecl FUN_004f263b(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, char *param_7);
void FUN_0041905e(int param_1);
void FUN_0057b590(FacemakerWindowBounds *window, int color_index, int x, int y, char *text);
int RecountDeckCardTotals(void);

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
void DrawFormattedTextShadowed(FacemakerWindowBounds *window, int color_index, int x, int y, char *format, ...);
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
int FUN_0056c0e5(int param_1, int param_2, int param_3);
int FUN_0056c5ea(int param_1);
int FUN_0056c705(int param_1);
int FindDeckSlotForQuestColorAndType(unsigned char quest_color, unsigned char quest_bitmap_mask);
char *FUN_00561441(int creature_type);
DWORD FUN_00564e70(char *dst, DWORD max_length, LPCVOID format, ...);
char *GetQuestCardClassName(int quest_bitmap_mask);
int FUN_004ef1ea(int town_index);
int __cdecl FUN_00418a2d(int param_1);
int __cdecl FUN_0041879d(int param_1);
int __cdecl FUN_004f1cce(int param_1);
int FUN_004bb458(int param_1);
int FUN_00428b05(int param_1);
char *FUN_004f2e17(int town_index);
char *BuildTownDisplayName(int town_index);
extern int DAT_0058c620[];
void FUN_004f23bc(char *param_1, DWORD param_2);
unsigned int FUN_005318f5(int param_1, int param_2, int param_3);
void FUN_00530b90(int param_1, int param_2);

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
int BeginMenuContext(void);
int ResetMenuContext(int context_index);
int AddMenuControlsToContext(AdvMenuControl *controls, int control_count, int context_index);
int EndMenuContext(void);
int UpdateMenuControlSelection(int mouse_x, int mouse_y, int allow_activate_on_click);
int QueuePendingMenuActionInput(void);
int FUN_005001e3(void);
void FUN_0046ed03(void);
void FUN_0046ed33(void);
void UpdateMouseSnapshot(void);

AdvMenuRect *PushGraphicsClipRect(AdvMenuRect *saved_clip_rect, FacemakerWindowBounds *page, int x, int y, int width, int height);

void DrawEncodedImageResampled(FacemakerWindowBounds *dst, int x, int y, int width, int height, EncodedImage *encoded_image);
void DrawEncodedImageUnscaled(FacemakerWindowBounds *dst, int x, int y, EncodedImage *encoded_image);
void DrawGraphicsLine(FacemakerWindowBounds *window_bounds, int x1, int y1, int x2, int y2, int color_index);
void DrawTextAt(FacemakerWindowBounds *window, int color, int x, int y, char *text);
void DrawWorldUiFormattedText(FacemakerWindowBounds *window, int color_index, int x, int y, char *format, ...);
void DrawCenteredTextLineWithShadow(char *text, int center_x, int y, int color_index);

extern EncodedImage *g_questnew_sprite_entries[4];
extern EncodedImage *g_tips_frame_sprite;
extern EncodedImage *g_tips_icon_sprite;
extern EncodedImage *g_main_menu_button_sprites_normal[4];
extern EncodedImage *g_main_menu_button_sprites_highlight[4];

// GLOBAL: SHANDALAR 0x00582d40
int DAT_00582d40[3] = {0x40, 0x50, 0x66};
// GLOBAL: SHANDALAR 0x00582d50
int DAT_00582d50[3] = {0x48, 0x5a, 0x72};
// GLOBAL: SHANDALAR 0x00582d60
int DAT_00582d60[3] = {0xcb, 0xfd, 0x143};
// GLOBAL: SHANDALAR 0x00582d6c
int DAT_00582d6c[3] = {0x1af, 0x21a, 0x2b0};
// GLOBAL: SHANDALAR 0x00582d78
int DAT_00582d78[3] = {0x2f, 0x3c, 0x4d};

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
void(__cdecl *DAT_00580da4)(void);

// GLOBAL: SHANDALAR 0x008e2e50
unsigned int DAT_008e2e50[16];

// GLOBAL: SHANDALAR 0x0058f0b0
int _DAT_0058f0b0 = 1;
// GLOBAL: SHANDALAR 0x0058f0b4
int DAT_0058f0b4 = 1;
// GLOBAL: SHANDALAR 0x0058f0b8
unsigned int DAT_0058f0b8 = 0xffffffff;
// GLOBAL: SHANDALAR 0x0058f0bc
unsigned int DAT_0058f0bc;
// GLOBAL: SHANDALAR 0x0058f0c0
int DAT_0058f0c0 = 0x110;
// GLOBAL: SHANDALAR 0x0058f0c4
int DAT_0058f0c4 = 5;
// GLOBAL: SHANDALAR 0x0058f0c8
int DAT_0058f0c8 = 0x61;
// GLOBAL: SHANDALAR 0x0058f0cc
int DAT_0058f0cc = 0x1b;
// GLOBAL: SHANDALAR 0x0058f0d0
int DAT_0058f0d0 = 0x110;
// GLOBAL: SHANDALAR 0x0058f0d4
int DAT_0058f0d4 = 5;
// GLOBAL: SHANDALAR 0x0058f0d8
int DAT_0058f0d8 = 0x61;
// GLOBAL: SHANDALAR 0x0058f0dc
int DAT_0058f0dc = 0x1b;
// GLOBAL: SHANDALAR 0x0058f0e0
int DAT_0058f0e0 = 1;

// GLOBAL: SHANDALAR 0x00746b10
EncodedImage *DAT_00746b10[3];
// GLOBAL: SHANDALAR 0x00746b18
EncodedImage *pERam00746b18;

// GLOBAL: SHANDALAR 0x00650268
int DAT_00650268;

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
// GLOBAL: SHANDALAR 0x005b7e08
EncodedImage *DAT_005b7e08;
// GLOBAL: SHANDALAR 0x005b7e10
EncodedImage *DAT_005b7e10[3];
// GLOBAL: SHANDALAR 0x005b7e20
EncodedImage *DAT_005b7e20;
// GLOBAL: SHANDALAR 0x005b7e28
EncodedImage *DAT_005b7e28[3];
// GLOBAL: SHANDALAR 0x005b7e34
int DAT_005b7e34;
// GLOBAL: SHANDALAR 0x005b7e38
int DAT_005b7e38;
// GLOBAL: SHANDALAR 0x005b7e40
EncodedImage *DAT_005b7e40[3];
// GLOBAL: SHANDALAR 0x005b7e4c
EncodedImage *DAT_005b7e4c;

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
// GLOBAL: SHANDALAR 0x007481dc
int _DAT_007481dc;
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

void FUN_004f04c5(void);

// GLOBAL: SHANDALAR 0x0058ab58
void(__cdecl *PTR_FUN_0058ab58)(void) = FUN_004f04c5;
// GLOBAL: SHANDALAR 0x0058ab70
int _DAT_0058ab70;

int __cdecl FUN_004f0b50(AdvMenuControl *control, int mode);
extern char DAT_0058abdc[];

// GLOBAL: SHANDALAR 0x0058ab88
AdvMenuControl DAT_0058ab88 = {
    0x114,
    0x112,
    0x58,
    0x22,
    0x114,
    0x112,
    0x58,
    0x22,
    1,
    (AdvMenuRenderCallback)FUN_004f0b50,
    (AdvMenuActivateCallback)0,
    0,
    0,
    DAT_0058abdc + 8,
    DAT_0058abdc + 0xc,
    0,
    0,
    {0, 0, 0, 0},
};

// GLOBAL: SHANDALAR 0x0058abdc
char DAT_0058abdc[] = "\x1b \0\0\x1b \0\0\x1b \0\0\x1b \0\0";

// GLOBAL: SHANDALAR 0x0058ad7c
char s_x_sound_button2_wav_0058ad7c[] = "x:sound\\button2.wav";
// GLOBAL: SHANDALAR 0x0058ad90
char s_x_sound_button_wav_0058ad90[] = "x:sound\\button.wav";

// GLOBAL: SHANDALAR 0x0058adfc
char s_village_pic_0058adfc[] = "village.pic\0city.pic";

// GLOBAL: SHANDALAR 0x0058f124
char s_x_sound_button2_wav_0058f124[] = "x:sound\\button2.wav\0tradscrn.pic\0\0\0\0tradbut1.pic\0\0\0\0";

#define s_tradscrn_pic_0058f138 (s_x_sound_button2_wav_0058f124 + 0x14)
#define s_tradbut1_pic_0058f148 (s_x_sound_button2_wav_0058f124 + 0x24)

// GLOBAL: SHANDALAR 0x0058f164
char DAT_0058f164[4] = "-";

// GLOBAL: SHANDALAR 0x0058ae14
char s_wiseman3_pic_0058ae14[0x10] = "wiseman3.pic";
// GLOBAL: SHANDALAR 0x0058ae24
char s_village_pic_0058ae24[0x18] = "village.pic\0city.pic";
// GLOBAL: SHANDALAR 0x0058ae3c
char s_wiseman3_pic_0058ae3c[0x10] = "wiseman3.pic";
// GLOBAL: SHANDALAR 0x0058ae4c
char s_wiseman3_pic_0058ae4c[0x10] = "wiseman3.pic";
// GLOBAL: SHANDALAR 0x0058ae5c
char s_village_pic_0058ae5c[0x18] = "village.pic\0city.pic";

// GLOBAL: SHANDALAR 0x0058ae74
char s_BuyButtons_spr_0058ae74[] = "BuyButtons.spr";
// GLOBAL: SHANDALAR 0x0058ae84
char s_smBuybttn_pic_0058ae84[] = "smBuybttn.pic";
// GLOBAL: SHANDALAR 0x0058ae98
char s_buycards_pic_0058ae98[] = "buycards.pic";
// GLOBAL: SHANDALAR 0x0058aea8
char s_buycards_pic_0058aea8[] = "buycards.pic";
// GLOBAL: SHANDALAR 0x0058aeb8
int DAT_0058aeb8;
// GLOBAL: SHANDALAR 0x0058aebc
int DAT_0058aebc;
// GLOBAL: SHANDALAR 0x0058aec0
char s_village_pic_0058aec0[] = "village.pic\0city.pic";

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

// FUNCTION: SHANDALAR 0x004313cf
void __cdecl FUN_004313cf(FacemakerWindowBounds *dst, int x_320, int y_200, int w_320, int h_200, EncodedImage *sprite)
{
  DrawEncodedImageResampled(dst, (x_320 * global_screen_width) / 0x140, (y_200 * global_screen_height) / 0xf0,
                            (w_320 * global_screen_width) / 0x140, (h_200 * global_screen_height) / 0xf0, sprite);
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
  else
  {
    return 0;
  }
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

// FUNCTION: SHANDALAR 0x004f2aa1
void __cdecl FUN_004f2aa1(int param_1, int param_2, int param_3, int param_4, char *param_5)
{
  struct
  {
    EncodedImage *bubble_right; /* -0x24 */
    int line_height;            /* -0x20 */
    EncodedImage *bubble_left;  /* -0x1c */
    EncodedImage *bubble_mid;   /* -0x18 */
    EncodedImage *tmp_sprite;   /* -0x14 */
    int local_x;                /* -0x10 */
    int tile_index;             /* -0x0c */
    int bubble_width;           /* -0x08 */
    int is_resampled_mode;      /* -0x04 */
  } s;

  s.is_resampled_mode = (param_4 == 2);
  s.local_x = param_1 - 0x2b;
  param_2 = param_2 - 0x26;

  switch (param_4)
  {
  case 0:
    param_4 = 0;
    break;
  case 1:
    param_4 = 1;
    break;
  case 2:
    param_4 = 1;
    break;
  case 3:
    return;
  }

  if (s.is_resampled_mode == 0)
  {
    DrawEncodedImageUnscaled(PTR_DAT_005832b4, s.local_x + 8, param_2 + 6, g_icons_sprite_entries[param_3]);
    DrawEncodedImageUnscaled(PTR_DAT_005832b4, s.local_x, param_2, g_dialog_box_sprite_bank.icon_rows[param_4][0]);
  }
  else
  {
    s.tmp_sprite = g_icons_sprite_entries[param_3];
    DrawEncodedImageResampled(PTR_DAT_005832dc, s.local_x + 0xa, param_2 + 8, (int)s.tmp_sprite->width - 4, (int)s.tmp_sprite->height - 2,
                              g_icons_sprite_entries[param_3]);

    s.tmp_sprite = g_dialog_box_sprite_bank.icon_rows[param_4][0];
    DrawEncodedImageResampled(PTR_DAT_005832dc, s.local_x + 2, param_2 + 2, (int)s.tmp_sprite->width - 4, (int)s.tmp_sprite->height - 2,
                              g_dialog_box_sprite_bank.icon_rows[param_4][0]);

    BlitGraphicsRect(PTR_DAT_005832dc, s.local_x, param_2, (int)s.tmp_sprite->width, (int)s.tmp_sprite->height, PTR_DAT_005832b4, s.local_x,
                     param_2);
  }

  if (_DAT_007481cc != 0)
  {
    s.line_height = GetFontLineHeight(PTR_DAT_005832b4->font_slot) + ScaleUiCoordinate(8);

    s.bubble_left = g_dialog_box_sprite_bank.icon_rows[param_4][2];
    s.bubble_mid = g_dialog_box_sprite_bank.icon_rows[param_4][1];
    s.bubble_right = g_dialog_box_sprite_bank.icon_rows[param_4][3];

    s.bubble_width = ClampIntToRange(MeasureTextLineWidth(param_5) + 0x20, 0x60, 0x3e7);

    for (s.tile_index = 0; s.tile_index < s.bubble_width / 0x40; s.tile_index = s.tile_index + 1)
    {
      DrawEncodedImageResampled(PTR_DAT_005832b4, (param_1 - s.bubble_width / 2) + s.tile_index * 0x40, param_2 + 0x38, 0x40, s.line_height,
                                s.bubble_mid);
    }

    DrawEncodedImageResampled(PTR_DAT_005832b4, param_1 + s.bubble_width / 2 - 0x40, param_2 + 0x38, 0x40, s.line_height,
                              g_dialog_box_sprite_bank.icon_rows[param_4][1]);
    DrawEncodedImageResampled(PTR_DAT_005832b4, param_1 - s.bubble_width / 2 - 8, param_2 + 0x38, (int)s.bubble_left->width, s.line_height,
                              g_dialog_box_sprite_bank.icon_rows[param_4][2]);
    DrawEncodedImageResampled(PTR_DAT_005832b4, param_1 + s.bubble_width / 2, param_2 + 0x38, (int)s.bubble_right->width, s.line_height,
                              g_dialog_box_sprite_bank.icon_rows[param_4][3]);

    DrawFormattedTextNoShadowCentered(PTR_DAT_005832b4, 0xff, param_1, param_2 + s.line_height / 2 + 0x38, param_5);
  }
}

// FUNCTION: SHANDALAR 0x004f23bc
void FUN_004f23bc(char *param_1, DWORD param_2)
{
  FUN_00564e70(param_1, param_2, gs_spellname_primary_0077e6e0, gs_spellnames_0077e220 + (g_current_quest_color * 5) * 5,
               GetQuestCardClassName(1 << (g_current_quest_destination & 3)));
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
  PlaySoundEffectOnChannel(s_x_sound_button2_wav_0058ad7c, 0xf, 100, 100, 0);
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
  int town_index;
  void(__cdecl * saved_callback)(void);
  int quest_time_units;
  char local_quest_text[100];

  town_index = _DAT_007483ec;
  saved_callback = DAT_00580da4;

  if ((g_town_slots[town_index].status_and_ruling_wizard & 4) != 0)
  {
    AnimateVisitBackdropZoomIn(s_wiseman3_pic_0058ae14);
    strcpy(g_ui_message_buffer, gs_citywiseman_0074d800[0]);
    PTR_DAT_005832b4->font_slot = 1;
    DrawFormattedTextShadowed(PTR_DAT_005832b4, 0xc1, ScaleUiCoordinate(0x50),
                              ScaleUiCoordinate(0x13c) - GetFontLineHeight(PTR_DAT_005832b4->font_slot) * 4, g_ui_message_buffer);
    ClearInputAndWaitForMouseRelease();
    (void)WaitForInputEventUnlessBlocked();
    AnimateVisitBackdropZoomIn(s_village_pic_0058ae24 + (((g_town_slots[town_index].location_type == 1) - 1) & 0xc));
    _DAT_007481d8 = -2;
    return;
  }

  if (((g_current_quest_destination == -1) && (DAT_005b7e38 != -1)) &&
      (((g_shandalar_difficulty + 3) * 0x10) <
       (g_quest_restock_timer - *(int *)(g_town_slots[town_index].data_18_to_63 + 0x3c))))
  {
    AnimateVisitBackdropZoomIn(s_wiseman3_pic_0058ae3c);
    g_current_quest_destination = DAT_005b7e38;
    g_current_quest_color = DAT_005b7da0;
    g_current_quest_type = DAT_005b7da4;

    if ((g_current_quest_type == 0) || (g_current_quest_type == 2))
    {
      FUN_00431526(0x80, g_town_slots[DAT_005b7e38].world_x, g_town_slots[DAT_005b7e38].world_y);
      g_current_quest_data = GetRelativeWorldQuadrant(g_town_slots[g_current_quest_destination].world_x, g_town_slots[g_current_quest_destination].world_y);
      (void)BuildTownDisplayName(g_current_quest_destination);

      FUN_00564e70(g_ui_message_buffer, 0x1000, gs_citywiseman_0074d800[1],
                   gs_directions_00765d50 + g_current_quest_data * 0x19, BuildTownDisplayName(g_current_quest_destination));

      if (g_current_quest_type == 0)
      {
        strcat(g_ui_message_buffer, gs_citywiseman_0074d800[2]);
      }
      else
      {
        sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), gs_citywiseman_0074d800[3], gs_amuletnames_0077d090 + g_current_quest_color * 0x19);
      }
      quest_time_units = 0x20;
    }
    else if (g_current_quest_type == 1)
    {
      FUN_00431526(0x80, g_town_slots[DAT_005b7e38].world_x, g_town_slots[DAT_005b7e38].world_y);
      g_current_quest_data = GetRelativeWorldQuadrant(g_town_slots[g_current_quest_destination].world_x, g_town_slots[g_current_quest_destination].world_y);

      FUN_004f23bc(local_quest_text, 100);
      (void)BuildTownDisplayName(g_current_quest_destination);

      FUN_00564e70(g_ui_message_buffer, 0x1000, gs_citywiseman_0074d800[4],
                   gs_directions_00765d50 + g_current_quest_data * 0x19, BuildTownDisplayName(g_current_quest_destination), local_quest_text);
      sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), gs_citywiseman_0074d800[5], gs_amuletnames_0077d090 + g_current_quest_color * 0x19);
      quest_time_units = 0x28;
    }
    else
    {
      if (g_current_quest_type < 0)
      {
        if (gs_creature_names_00591a08[-g_current_quest_type].plural[0x33] == '\x12')
        {
          sprintf(g_ui_message_buffer, gs_citywiseman_0074d800[6], FUN_00561441(-g_current_quest_type));
        }
        else
        {
          sprintf(g_ui_message_buffer, gs_citywiseman_0074d800[7], FUN_00561441(-g_current_quest_type));
        }

        if (g_town_slots[town_index].location_type == 1)
        {
          if (((int)gs_creature_names_00591a08[-g_current_quest_type].plural[0x33] / 7) == 0)
          {
            sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), gs_citywiseman_0074d800[8],
                    gs_amuletnames_0077d090 + g_current_quest_color * 0x19);
          }
          else
          {
            FUN_00564e70(g_ui_message_buffer + strlen(g_ui_message_buffer), 0x1000, gs_citywiseman_0074d800[9]);
          }
        }
        else
        {
          sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), gs_citywiseman_0074d800[10], FUN_004f2e17(town_index));
        }
      }
      quest_time_units = 0x18;
    }

    strcat(g_ui_message_buffer, gs_citywiseman_0074d800[0xb]);
    DAT_00580da4 = (void(__cdecl *)(void))FUN_00530b7e;

    if (RunTextMenuAtScaled(g_ui_message_buffer, 0x2d, 0x24) == 0)
    {
      LoadVisitBackdropAndCopyToPage0(s_wiseman3_pic_0058ae4c);
      g_current_quest_giver_town_index = town_index;
      g_current_quest_deadline = quest_time_units * 2 + g_quest_restock_timer - 1;
      *(int *)(g_town_slots[town_index].data_18_to_63 + 0x3c) = g_quest_restock_timer;

      sprintf(g_ui_message_buffer, gs_citywiseman_0074d800[0xc], ((int)(quest_time_units + ((quest_time_units >> 0x1f) & 7U)) >> 3));
      (void)RunTextMenuAtScaled(g_ui_message_buffer, 0x48, 0x48);
      FUN_00431526(0x80, g_town_slots[town_index].world_x, g_town_slots[town_index].world_y);
      FUN_004290e2(0xf, g_current_quest_type);
      DAT_00580da4 = saved_callback;
    }
    else
    {
      g_current_quest_destination = -1;
      g_current_quest_type = 0;
      DAT_00580da4 = saved_callback;
    }
  }
  else
  {
    (void)FUN_005318f5(_DAT_007481e0, (DAT_0058a88c != _DAT_007483ec), _DAT_007483ec);
  }

  DAT_0058a88c = town_index;
  (void)FUN_0055060c(1);
  AnimateVisitBackdropZoomIn(s_village_pic_0058ae5c + (((g_town_slots[town_index].location_type == 1) - 1) & 0xc));
  _DAT_007481d8 = -2;
}

// FUNCTION: SHANDALAR 0x004f18c0
void FUN_004f18c0(void)
{
  int town_index;
  int preferred_color;

  town_index = _DAT_007483ec;
  preferred_color = _DAT_007481e0;
  FUN_00530b90(preferred_color, town_index);

  *(int *)(g_town_slots[town_index].data_18_to_63 + 0x40) = g_quest_restock_timer;
  RefreshAdventureInterfaceLayout();
  RecountDeckCardTotals();
  FUN_0055060c(1);

  /* Branchless selection of which town backdrop we use (0 or +0xc). */
  AnimateVisitBackdropZoomIn(s_village_pic_0058aed8 +
                             (((g_town_slots[town_index].location_type == 1) - 1) &
                              (int)((s_village_pic_0058aed8 + 0xc) - s_village_pic_0058aed8)));
  _DAT_007481d8 = -2;
}

// FUNCTION: SHANDALAR 0x004f1951
void FUN_004f1951(void)
{
  int town_index;

  town_index = _DAT_007483ec;
  PlaySoundEffectOnChannel(s_x_sound_button2_wav_0058aef0, 0xf, 100, 100, 0);
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
  PlaySoundEffectOnChannel(s_x_sound_button2_wav_0058af1c, 0xf, 100, 100, 0);
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
  PlaySoundEffectOnChannel(s_x_sound_button2_wav_0058af48, 0xf, 100, 100, 0);
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
  PlaySoundEffectOnChannel(s_x_sound_button2_wav_0058af8c, 0xf, 100, 100, 0);
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
  struct
  {
    char *accept_scan;      /* -0x134 */
    int deck_slot;          /* -0x130 */
    int accepted;           /* -0x12c */
    char *accept_cursor;    /* -0x128 */
    int mouse_y;            /* -0x124 */
    int mouse_x;            /* -0x120 */
    int title_y_200;        /* -0x11c */
    int title_line_h;       /* -0x118 */
    int center_x_320;       /* -0x114 */
    int title_text_w;       /* -0x110 */
    char *title;            /* -0x10c */
    int loop_i;             /* -0x108 */
    int colors[3];          /* -0x104 */
    EncodedImage *sprites[13]; /* -0xf8 */
    int menu_context;       /* -0xc4 */
    int tile_w;             /* -0xc0 */
    int selected_idx;       /* -0xbc */
    int mana_mask;          /* -0xb8 */
    int key;                /* -0xb4 */
    int idx;                /* -0xb0 */
    int y_base;             /* -0xac */
    int item_count;         /* -0xa8 */
    int town_index;         /* -0xa4 */
    BuyCardsHitRect rects[10]; /* -0xa0 */
  } s;

  s.town_index = _DAT_007483ec;
  s.item_count = 0;

  ClearInputAndWaitForMouseRelease();

  s.mana_mask = (int)FUN_005611c8(FUN_0043146b(g_town_slots[s.town_index].world_x, g_town_slots[s.town_index].world_y));

  s.item_count = g_town_slots[s.town_index].location_type + 3;
  if (g_world_magic_slot_timers[1].unk_00 == 0)
  {
    s.item_count = s.item_count + 1;
  }

  if (s.item_count == 0)
  {
    DAT_005b7da8[s.item_count] = FUN_0040dffd(s.mana_mask) - 1;
    DAT_005b7de8[s.item_count] = 0x28;
    s.item_count = s.item_count + 1;
  }

  s.colors[0] = 0x1b;
  s.colors[1] = 0x1a;
  s.colors[2] = 0x1a;

  ReadSpriteEntryPointers(&s.sprites[0], s_BuyButtons_spr_0058ae74);
  DAT_005b7e20 = s.sprites[0];
  DAT_005b7e4c = s.sprites[1];
  DAT_005b7e08 = s.sprites[2];

  for (s.loop_i = 0; s.loop_i < 3; s.loop_i = s.loop_i + 1)
  {
    DAT_005b7e10[s.loop_i] = (&s.sprites[6])[s.loop_i];
  }

  for (s.loop_i = 0; s.loop_i < 3; s.loop_i = s.loop_i + 1)
  {
    DAT_005b7e28[s.loop_i] = (&s.sprites[9])[s.loop_i];
  }

  LoadPcxIntoPage(1, s_smBuybttn_pic_0058ae84);
  PTR_DAT_005832dc->font_slot = 7;
  BeginSpriteEncodeSession();
  for (s.loop_i = 0; s.loop_i < 3; s.loop_i = s.loop_i + 1)
  {
    SetFontStyleSize(7, (unsigned int)(((-(unsigned int)(s.loop_i == 2)) & 0xfffffffeU) + 0xb));
    DrawFormattedTextNoShadowCentered(PTR_DAT_005832dc, s.colors[s.loop_i], s.loop_i * 0x5a + 0x2e, 0x10, "%s", (char *)g_done_text_table_entry);
    DAT_005b7e40[s.loop_i] = EncodeSpriteFromPage(1, s.loop_i * 0x5a + 1, 1, 0x59, 0x23);
  }
  FinalizeSpriteEncodeSession();

  LoadPcxResource(1, 0, global_screen_height - 0x118, s_buycards_pic_0058ae98, (void *)0);
  StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, global_screen_height - 0x118, 0x200, 0x118, PTR_DAT_005832b4,
                          FUN_005501dc(0x20), FUN_005501dc(0x18), FUN_005501dc(0x100), FUN_005501dc(0x8c));
  StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, global_screen_height - 0x118, 0x200, 0x118, PTR_DAT_005832dc,
                          FUN_005501dc(0x20), FUN_005501dc(0x18), FUN_005501dc(0x100), FUN_005501dc(0x8c));
  FUN_0042fe00(PTR_DAT_005832b4);

  if (_DAT_007481d4 != -1)
  {
    FUN_0041905e(_DAT_007481d4);
  }

  s.menu_context = BeginMenuContext();
  ResetMenuContext(s.menu_context);
  if (DAT_0058ab88.x == DAT_0058ab88.base_x)
  {
    DAT_0058ab88.x = ScaleUiCoordinate(DAT_0058ab88.x);
    DAT_0058ab88.y = ScaleUiCoordinate(DAT_0058ab88.y);
    DAT_0058ab88.width = ScaleUiCoordinate(DAT_0058ab88.width);
    DAT_0058ab88.height = ScaleUiCoordinate(DAT_0058ab88.height);
  }
  AddMenuControlsToContext(&DAT_0058ab88, 1, s.menu_context);

  while (1)
  {
    LoadPcxIntoPage(1, s_buycards_pic_0058aea8);
    StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, 0, 0x200, 0x118, PTR_DAT_005832b4, FUN_005501dc(0x20), FUN_005501dc(0x18), FUN_005501dc(0x100),
                            FUN_005501dc(0x8c));
    FUN_0042fe00(PTR_DAT_005832b4);

    FUN_005001e3();
    (void)UpdateMenuControlSelection(g_mouse_x_snapshot, g_mouse_y_snapshot, g_mouse_button_down_mask);

    s.center_x_320 = 0xa0;
    s.title_y_200 = 0x1f;
    s.title = gs_visit_citybuy_0077f1d0[0];
    PTR_DAT_005832b4->font_slot = 4;
    s.title_line_h = GetFontLineHeight(PTR_DAT_005832b4->font_slot);
    s.title_text_w = MeasureTextLineWidth(s.title);
    DrawEncodedImageResampled(PTR_DAT_005832b4, global_screen_width / 2 - s.title_text_w / 2 - 0x14,
                              (s.title_y_200 * global_screen_height) / 0xf0 - s.title_line_h, s.title_text_w + 0x28, s.title_line_h * 3, DAT_005b7e4c);
    FUN_00430fb5(s.title, s.center_x_320, s.title_y_200, 0x1b);

    s.y_base = 0x40;
    s.tile_w = 0xf4 / s.item_count;
    memset(s.rects, 0xff, 0xa0);

    for (s.idx = s.item_count - 1; s.idx >= 0; s.idx = s.idx - 1)
    {
      if (DAT_005b7da8[s.idx] != -1)
      {
        FUN_004313cf(PTR_DAT_005832b4, s.tile_w * s.idx + 0x2c, (DAT_005b7da8[s.idx] & 7) + s.y_base - 0xc, s.tile_w - 4, 0xc, DAT_005b7e08);

        sprintf(g_ui_message_buffer, gs_visit_citybuy_0077f1d0[1], DAT_005b7de8[s.idx]);
        PTR_DAT_005832b4->font_slot = 1;
        FUN_00430fb5(g_ui_message_buffer, s.tile_w * s.idx + s.tile_w / 2 + 0x29, (DAT_005b7da8[s.idx] & 7) + s.y_base - 8, 0x1b);

        FUN_004f2407(DAT_005b7da8[s.idx],
                     ClampIntToRange(s.tile_w * s.idx + s.tile_w / 2 + 0x12, 0, global_screen_width - 0x62),
                     (DAT_005b7da8[s.idx] & 7) + s.y_base + 4, 0, (char *)&DAT_0058aeb8);

        s.rects[s.idx].x0 = FUN_005501dc(ClampIntToRange(s.tile_w * s.idx + s.tile_w / 2 + 0x12, 0, global_screen_width - 0x62));
        s.rects[s.idx].y0 = FUN_005501dc((DAT_005b7da8[s.idx] & 7) + s.y_base + 4);
        s.rects[s.idx].x1 = s.rects[s.idx].x0 + FUN_005501dc(0x30);
        s.rects[s.idx].y1 = s.rects[s.idx].y0 + FUN_005501dc(0x30);
      }
    }

    _DAT_007481dc = 0;
    while (_DAT_007481dc == 0)
    {
      s.selected_idx = -1;
      UpdateMouseSnapshot();
      s.mouse_x = g_mouse_x_snapshot;
      s.mouse_y = g_mouse_y_snapshot;

      if (g_mouse_button_down_mask == 0)
      {
        (void)UpdateMenuControlSelection(g_mouse_x_snapshot, g_mouse_y_snapshot, 0);
      }
      else
      {
        s.idx = 0;
        while (s.idx < s.item_count)
        {
          if ((s.rects[s.idx].x0 <= s.mouse_x) && (s.mouse_x <= s.rects[s.idx].x1) &&
              (s.rects[s.idx].y0 <= s.mouse_y) && (s.mouse_y <= s.rects[s.idx].y1))
          {
            s.selected_idx = s.idx;
            break;
          }
          s.idx = s.idx + 1;
        }

        if (s.selected_idx == -1)
        {
          (void)UpdateMenuControlSelection(s.mouse_x, s.mouse_y, g_mouse_button_down_mask);
        }
      }

      if (s.selected_idx != -1)
      {
        break;
      }
    }

    if (_DAT_007481dc != 0)
    {
      break;
    }

    sprintf(g_ui_message_buffer, gs_visit_citybuy_0077f1d0[2], DAT_005b7de8[s.selected_idx]);
    DrawEncodedImageResampled(PTR_DAT_005832b4, FUN_005501dc(0xdc) / 2, FUN_005501dc(0x34) / 2, FUN_005501dc(0xc5) / 2, FUN_005501dc(0x10f) / 2, DAT_005b7e20);
    FUN_004f263b(DAT_005b7da8[s.selected_idx], 0x7a, 0x29, 0x4b, 0x70, 1, (char *)&DAT_0058aebc);
    FUN_0057b590(PTR_DAT_005832b4, 0x1b, 0x140, 0x47, g_ui_message_buffer);
    ClearInputAndWaitForMouseRelease();
    s.key = PopNormalizedQueuedKeyInput();

    s.accept_cursor = gs_visit_citybuy_0077f1d0[3];
    s.accepted = 0;
    while (*s.accept_cursor != '\0')
    {
      s.accept_scan = s.accept_cursor;
      s.accept_cursor = s.accept_cursor + 1;
      if ((int)(signed char)*s.accept_scan == s.key)
      {
        s.accepted = 1;
      }
    }

    if ((s.accepted != 0) && (DAT_005b7de8[s.selected_idx] <= Gold))
    {
      Gold = Gold - DAT_005b7de8[s.selected_idx];
      s.deck_slot = FUN_0056bd9d((unsigned int)DAT_005b7da8[s.selected_idx]);
      deck[s.deck_slot] = deck[s.deck_slot] | 0x4000;
      DAT_005b7da8[s.selected_idx] = -1;
      DAT_005b7de8[s.selected_idx] = 0;

      if (_DAT_007483e8 == s.selected_idx)
      {
        _DAT_007481d4 = -1;
      }

      *(int *)(g_town_slots[s.town_index].data_18_to_63 + 0x28 + s.selected_idx * 4) =
          FUN_00522508(5) * (g_shandalar_difficulty + 2) + g_quest_restock_timer;
      RecountDeckCardTotals();
      (void)FUN_0055060c(1);
    }
  }

  (void)EndMenuContext();
  RefreshAdventureInterfaceLayout();
  (void)FUN_0055060c(1);
  AnimateVisitBackdropZoomIn(s_village_pic_0058aec0 +
                             (-(g_town_slots[s.town_index].location_type != 1) & ((s_village_pic_0058aec0 + 0xc) - s_village_pic_0058aec0)));
  _DAT_007481d8 = -2;
  FreeSpriteBlob(DAT_005b7e20);
  FreeSpriteBlob(DAT_005b7e40[0]);
}

// FUNCTION: SHANDALAR 0x004f0b50
int __cdecl FUN_004f0b50(AdvMenuControl *control, int mode)
{
  int w_scaled;
  int h_scaled;
  int x_left;
  int y_top;
  FacemakerWindowBounds *dst;

  if (g_menu_render_guard == 0)
  {
    if ((g_mouse_x < control->x) || ((control->x + control->width) < g_mouse_x))
    {
      return 0;
    }
    if ((g_mouse_y < control->y) || ((control->y + control->height) < g_mouse_y))
    {
      return 0;
    }
  }

  if (control->state == 3)
  {
    return 0;
  }

  dst = (mode == 2) ? PTR_DAT_005832dc : PTR_DAT_005832b4;
  w_scaled = FUN_005501dc((int)DAT_005b7e40[0]->width) / 2;
  h_scaled = FUN_005501dc((int)DAT_005b7e40[0]->height) / 2;
  x_left = global_screen_width / 2 - w_scaled / 2;
  y_top = FUN_005501dc(0x112) / 2;

  DrawEncodedImageResampled(dst, x_left, y_top, w_scaled, h_scaled, DAT_005b7e40[mode]);

  if (mode == 2)
  {
    FUN_00562736(0x12, 100, 100, 0);
    BlitGraphicsRect(PTR_DAT_005832dc, x_left, y_top, w_scaled, h_scaled, PTR_DAT_005832b4, x_left, y_top);
    _DAT_007481dc = 1;
  }
  return 1;
}

// FUNCTION: SHANDALAR 0x00418a2d
int __cdecl FUN_00418a2d(int param_1)
{
  int local_8;

  local_8 = 0;
  while (1)
  {
    if (499 < local_8)
    {
      return 0;
    }
    if (global_cards_data[deck[local_8] & 0xfff].id == param_1)
    {
      break;
    }
    local_8 = local_8 + 1;
  }
  return 1;
}

// FUNCTION: SHANDALAR 0x0041879d
int __cdecl FUN_0041879d(int param_1)
{
  int iVar1;
  int iVar2;
  int local_420;
  int local_41c;
  int local_418;
  int deck_card_id;
  int aiStack_404[256];

  local_420 = 0;
  deck_card_id = -1;
  for (local_418 = 0; local_418 < 0x100; local_418 = local_418 + 1)
  {
    if (DAT_0097e450[local_418].second == -1)
    {
      if (((global_cards_data[param_1].id == DAT_0097e450[local_418].first) &&
           (iVar1 = FUN_00418a2d(DAT_0097e450[local_418].first), iVar1 == 0)) &&
          ((DAT_0097df40[local_418] & (1 << ((unsigned char)g_shandalar_difficulty & 0x1f))) != 0))
      {
        local_41c = 0;
        while (local_41c < 500)
        {
          deck_card_id = deck[local_41c] & 0xfff;
          if (((unsigned char)global_cards_data[deck_card_id].type & 1) != 0)
          {
            if (((unsigned char)(global_cards_data[deck_card_id].color & global_cards_data[param_1].color)) != 0)
            {
              break;
            }
          }
          local_41c = local_41c + 1;
        }
      }
    }
    else
    {
      iVar1 = FUN_00418a2d(DAT_0097e450[local_418].first);
      iVar2 = FUN_00418a2d(DAT_0097e450[local_418].second);
      if (((iVar1 == 0) || (iVar2 == 0)) &&
          ((DAT_0097df40[local_418] & (1 << ((unsigned char)g_shandalar_difficulty & 0x1f))) != 0))
      {
        if ((global_cards_data[param_1].id == DAT_0097e450[local_418].first) && (iVar2 != 0))
        {
          aiStack_404[local_420] = local_418;
          local_420 = local_420 + 1;
        }
        if ((DAT_0097e450[local_418].second == global_cards_data[param_1].id) && (iVar1 != 0))
        {
          aiStack_404[local_420] = local_418;
          local_420 = local_420 + 1;
        }
      }
    }
  }

  if (local_420 == 0)
  {
    iVar1 = -1;
  }
  else
  {
    iVar1 = FUN_00522508(local_420);
    iVar1 = aiStack_404[iVar1];
  }
  return iVar1;
}

// FUNCTION: SHANDALAR 0x004f1cce
int __cdecl FUN_004f1cce(int param_1)
{
  int iVar1;
  int local_10;
  int local_c;
  int local_8;

  local_8 = 0x28;
  switch (global_cards_data[param_1].type)
  {
  case 2:
  case 'B':
    if ((global_cards_data[param_1].power & 0xbfffU) == 0xfffe)
    {
      local_10 = (int)global_cards_data[param_1].power;
    }
    else
    {
      local_10 = ((int)global_cards_data[param_1].power & 0xffffbfffU) + 3;
    }

    if ((global_cards_data[param_1].toughness & 0xbfffU) == 0xfffe)
    {
      local_c = (int)global_cards_data[param_1].toughness;
    }
    else
    {
      local_c = ((int)global_cards_data[param_1].toughness & 0xffffbfffU) + 3;
    }

    local_c = (*(int *)global_cards_data[param_1].reserved4 + local_10) * local_c;
    local_8 = local_c * 5;
    if ((global_cards_data[param_1].static_ability & 0x1f) != 0)
    {
      local_8 = (local_c * 0xf) / 2;
    }
    if ((global_cards_data[param_1].static_ability & 0x200) != 0)
    {
      local_8 = (local_8 * 3) / 2;
    }
    if (global_cards_data[param_1].code_pointer != card_dummy)
    {
      local_8 = (local_8 * 3) / 2;
    }
    if ((global_cards_data[param_1].static_ability & 0x1c0U) != 0)
    {
      local_8 = (local_8 * 3) / 2;
    }
    if ((global_cards_data[param_1].extra_ability & 8) != 0)
    {
      local_8 = local_8 * 3;
    }
    if ((global_cards_data[param_1].extra_ability & 0x10) != 0)
    {
      local_8 = local_8 * 3;
    }

    iVar1 = abs((int)(char)global_cards_data[param_1].cc[1]);
    local_8 = local_8 / ((char)global_cards_data[param_1].cc[0] + iVar1 + 1);
    break;

  case 4:
    iVar1 = abs((int)(char)global_cards_data[param_1].cc[1]);
    iVar1 = (iVar1 + (char)global_cards_data[param_1].cc[0] + *(int *)global_cards_data[param_1].reserved4) * 5 + 5;
    local_8 = iVar1 * 5;
    if ((global_cards_data[param_1].extra_ability & 3) != 0)
    {
      local_8 = iVar1 * 10;
    }
    break;

  case 0xb:
    iVar1 = abs((int)(char)global_cards_data[param_1].cc[1]);
    local_8 = ((iVar1 + (char)global_cards_data[param_1].cc[0] + *(int *)global_cards_data[param_1].reserved4) * 4 + 4) * 5;
    break;

  case 0x10:
  case ' ':
    iVar1 = abs((int)(char)global_cards_data[param_1].cc[1]);
    local_8 = *(int *)global_cards_data[param_1].reserved4 * 0x14 + ((iVar1 + (char)global_cards_data[param_1].cc[0]) * 5 + 5) * 8;
    if (global_cards_data[param_1].cc[1] == 0xff)
    {
      local_8 = (local_8 * 3) / 2;
    }
    break;

  case '@':
    iVar1 = abs((int)(char)global_cards_data[param_1].cc[1]);
    local_8 = 0xfa / (*(int *)global_cards_data[param_1].reserved4 + iVar1 + 2);
    break;
  }

  iVar1 = FUN_0056c5ea(param_1);
  if (iVar1 == 2)
  {
    local_8 = ClampIntToRange(local_8 * 2, 100, 9999);
  }
  else if (iVar1 == 3)
  {
    local_8 = ClampIntToRange(local_8 << 2, 200, 9999);
  }
  else if (iVar1 == 4)
  {
    local_8 = ClampIntToRange(local_8 << 3, 200, 9999);
  }

  if ((global_cards_data[param_1].extra_ability & 0x200) != 0)
  {
    local_8 = local_8 << 1;
  }
  if ((global_cards_data[param_1].extra_ability & 0x400) != 0)
  {
    local_8 = (local_8 * 3) / 2;
  }

  return local_8;
}

// FUNCTION: SHANDALAR 0x004ef1ea
int FUN_004ef1ea(int town_index)
{
  struct
  {
    int hint_pair_count;
    struct
    {
      int hint_pair_index;
      int slot_index;
    } hint_pairs[8];

    int tmp_cost;
    unsigned int tile_magic_mask;
    int slot_count;
    int rand_slot;
    int rand_mask;
    int slot_i;
    int card_id;
    int ok;
    int scan_color;
    int menu_context;
  } s;

  ClearInputAndWaitForMouseRelease();

  _DAT_007483ec = town_index;

  s.tile_magic_mask = FUN_005611c8(FUN_0043146b(g_town_slots[town_index].world_x, g_town_slots[town_index].world_y));

  s.slot_count = g_town_slots[_DAT_007483ec].location_type + 3;
  if (g_world_magic_slot_timers[1].unk_00 == 0)
  {
    s.slot_count = g_town_slots[_DAT_007483ec].location_type + 4;
  }
  if (7 < s.slot_count)
  {
    s.slot_count = 8;
  }

  /* Pick a "preferred" color (stored in _DAT_007481e0) that the player can actually cast. */
  s.card_id = 0xffffffff;
  for (s.slot_i = 0; s.slot_i < 199; s.slot_i = s.slot_i + 1)
  {
    s.scan_color = FUN_00522508(5) + 1;
    if (((DAT_0078990c[s.scan_color] != 0) || (0x50 < s.slot_i)) &&
        ((s.tile_magic_mask & (1U << ((unsigned char)s.scan_color & 0x1f))) != 0))
    {
      s.card_id = s.scan_color;
    }
  }

  _DAT_007481e0 = s.card_id;
  _DAT_007481d4 = -1;
  DAT_005b7e34 = town_index;

  /* Build shop offer cache if we haven't visited this town yet (DAT_0058a88c). */
  if (DAT_0058a88c != town_index)
  {
    if ((g_town_slots[town_index].status_and_ruling_wizard & 8) == 0)
    {
      for (s.slot_i = 0; s.slot_i < 8; s.slot_i = s.slot_i + 1)
      {
        DAT_005b7da8[s.slot_i] = 0xffffffff;
      }
    }
    else
    {
      for (s.slot_i = 0; s.slot_i < s.slot_count; s.slot_i = s.slot_i + 1)
      {
        if (g_quest_restock_timer - *(int *)(g_town_slots[town_index].data_18_to_63 + 0x1c + s.slot_i * 4) <
            g_shandalar_difficulty * 5 + 0xf)
        {
          DAT_005b7da8[s.slot_i] = 0xffffffff;
        }
        else
        {
          DAT_005b7da8[s.slot_i] = *(int *)((char *)&g_town_slots[town_index].card_slot_1 + s.slot_i * 4);
        }
      }
    }

    s.rand_slot = FUN_00522508(s.slot_count);
    for (s.slot_i = 0; s.slot_i < s.slot_count; s.slot_i = s.slot_i + 1)
    {
      if ((DAT_005b7da8[s.slot_i] == -1) &&
          (g_shandalar_difficulty * 5 + 0xf <=
           g_quest_restock_timer - *(int *)(g_town_slots[town_index].data_18_to_63 + 0x1c + s.slot_i * 4)))
      {
        s.rand_mask = 1 << ((unsigned char)FUN_00522508(7) & 0x1f);

        while (1)
        {
          if (s.slot_i == s.rand_slot)
          {
            s.card_id = FUN_00522508(5);
          }
          else
          {
            s.card_id = FUN_00522508(g_card_count - 0x39);
          }
          DAT_005b7da8[s.slot_i] = s.card_id;

          s.ok = 0;
          for (s.scan_color = 1; s.scan_color < 6; s.scan_color = s.scan_color + 1)
          {
            if ((s.tile_magic_mask & (1U << ((unsigned char)s.scan_color & 0x1f))) != 0)
            {
              if (FUN_0056c0e5(1 << ((unsigned char)s.scan_color & 0x1f), (int)(signed char)global_cards_data[s.card_id].color,
                               ((s.scan_color & 1) == 1) ? 1 : 3) != 0)
              {
                s.ok = 1;
              }
            }
          }

          if ((s.slot_i & 1) != 0)
          {
            if ((((unsigned char)global_cards_data[s.card_id].type & 0x40) != 0) ||
                ((global_cards_data[s.card_id].extra_ability & 0x900) != 0) ||
                (((unsigned char)global_cards_data[s.card_id].expansion & 0xc1) != 0) ||
                (FUN_0056c5ea(s.card_id) > ((s.slot_i % 3) + 1)))
            {
              s.ok = 0;
            }
          }

          if (FUN_004bb1cf(s.card_id) <= 0)
          {
            continue;
          }
          if (s.ok == 0)
          {
            continue;
          }
          if ((global_cards_data[s.card_id].extra_ability & 0x180) != 0)
          {
            continue;
          }
          break;
        }
      }
    }

    /* Pick an optional "hint pair" tied to the available cards. */
    s.hint_pair_count = 0;
    for (s.slot_i = 0; s.slot_i < s.slot_count; s.slot_i = s.slot_i + 1)
    {
      s.card_id = FUN_0041879d(DAT_005b7da8[s.slot_i]);
      if (s.card_id != -1)
      {
        s.hint_pairs[s.hint_pair_count].hint_pair_index = s.card_id;
        s.hint_pairs[s.hint_pair_count].slot_index = s.slot_i;
        s.hint_pair_count = s.hint_pair_count + 1;
      }
    }
    if (s.hint_pair_count != 0)
    {
      s.slot_i = FUN_00522508(s.hint_pair_count);
      _DAT_007481d4 = s.hint_pairs[s.slot_i].hint_pair_index;
      _DAT_007483e8 = s.hint_pairs[s.slot_i].hint_pair_index;
    }

    /* Price each offer. */
    for (s.slot_i = 0; s.slot_i < 8; s.slot_i = s.slot_i + 1)
    {
      s.card_id = DAT_005b7da8[s.slot_i];
      if (s.card_id == -1)
      {
        continue;
      }

      s.tmp_cost = FUN_004f1cce(s.card_id);
      s.tmp_cost = (g_town_slots[town_index].location_type + 2) * s.tmp_cost;

      if ((s.tile_magic_mask & (unsigned int)(signed char)global_cards_data[s.card_id].color) == 0)
      {
        if (((signed char)global_cards_data[s.card_id].color != 0) &&
            (FUN_0056c0e5((int)s.tile_magic_mask, (int)(signed char)global_cards_data[s.card_id].color, 3) != 0))
        {
          s.tmp_cost = (s.tmp_cost * 3) / 2;
        }
        else
        {
          s.tmp_cost = s.tmp_cost << 1;
        }
      }

      DAT_005b7de8[s.slot_i] = ClampIntToRange(((s.tmp_cost / 0x32) * 5), 5, 1000);
    }

    g_town_slots[town_index].status_and_ruling_wizard |= 8;
  }

  /* Food cost(?) depends on location_type. */
  _DAT_007481d0 = g_town_slots[town_index].location_type * 5 + 10;

  sound_unload(0x12);
  (void)LoadSoundWithDriveFallback(s_x_sound_button_wav_0058ad90, 0x12, 0);

  if (s.slot_count == 0)
  {
    DAT_005b7da8[s.slot_count] = FUN_0040dffd((int)s.tile_magic_mask) - 1;
    DAT_005b7de8[s.slot_count] = 0x28;
    s.slot_count = s.slot_count + 1;
  }

  _DAT_007481d8 = 0;

  /* Fill sprite pointers (the render callback expects these). */
  for (s.slot_i = 0; s.slot_i < 4; s.slot_i = s.slot_i + 1)
  {
    DAT_0058a890[s.slot_i].mode_data[0] = (int)g_main_menu_button_sprites_normal[s.slot_i];
    DAT_0058a890[s.slot_i].mode_data[1] = (int)g_main_menu_button_sprites_highlight[s.slot_i];
    DAT_0058a890[s.slot_i].mode_data[2] = (int)g_main_menu_button_sprites_highlight[s.slot_i];
    DAT_0058a890[s.slot_i].mode_data[3] = (int)g_main_menu_button_sprites_normal[s.slot_i];
  }

  AnimateVisitBackdropZoomIn(s_village_pic_0058adfc + (((g_town_slots[town_index].location_type == 1) - 1) & 0xc));
  EnsureAdvfac64Loaded(1);

  s.menu_context = BeginMenuContext();
  (void)AddMenuControlsToContext(DAT_0058a890, 4, s.menu_context);
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

  return;
}

// FUNCTION: SHANDALAR 0x00417e11
int FUN_00417e11(char *param_1, int param_2, unsigned int param_3, int param_4)
{
  struct
  {
    unsigned int entry_index;            /* local_230 */
    unsigned int last_basic_entry_index; /* local_22c */
    int csvid;                           /* local_220 */
    int numcards;                        /* local_21c */
    int header_line_index;               /* local_218 */
    char *space;                         /* local_214 */
    int total_cards;                     /* local_210 */
    int speed_filter;                    /* local_20c */
    unsigned int color_filter;           /* local_208 */
    char line[500];                      /* local_204/local_203 */
    int deck_type;                       /* local_10 */
    FILE *f;                             /* local_c */
    int scan_ret;                        /* local_8 */
    int card_index;
    int *tmp_entry_ptr;
  } s;

  strcpy(DAT_0097e340, param_1);
  s.f = fopen(param_1, DAT_00580e8c);
  if (s.f == NULL)
  {
    return 0;
  }

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
              s.tmp_entry_ptr = (int *)(param_2 + 4 + s.last_basic_entry_index * 8);
              *s.tmp_entry_ptr = *s.tmp_entry_ptr - (int)((s.entry_index & 1) + s.numcards) / 2;
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

// FUNCTION: SHANDALAR 0x005318f5
unsigned int FUN_005318f5(int param_1, int param_2, int param_3)
{
  (void)param_1;
  (void)param_2;
  (void)param_3;
  // TODO(decomp): Updates "wiseman/shop" state when entering a town (quest offers, restock, tips).
  return 0;
}

// FUNCTION: SHANDALAR 0x005328e4
int __cdecl FUN_005328e4(AdvMenuControl *control, int mode)
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

  if (mode == 2)
  {
    DrawEncodedImageResampled(PTR_DAT_005832dc, control->x, control->y, control->width, control->height, pERam00746b18);
    BlitGraphicsRect(PTR_DAT_005832dc, control->x, control->y, control->width, control->height, PTR_DAT_005832b4, control->x, control->y);

    if (control->on_activate != (AdvMenuActivateCallback)0)
    {
      (void)control->on_activate(control);
    }
  }
  else
  {
    DrawEncodedImageResampled(PTR_DAT_005832b4, control->x, control->y, control->width, control->height, DAT_00746b10[mode]);
  }

  return 1;
}

// FUNCTION: SHANDALAR 0x00532a7c
int __cdecl FUN_00532a7c(AdvMenuControl *control)
{
  PlaySoundEffectOnChannel(s_x_sound_button2_wav_0058f124, 0xf, 100, 100, 0);
  DAT_00650268 = control->selection_value;
  return 0;
}

// FUNCTION: SHANDALAR 0x00532aae
int __cdecl FUN_00532aae(const void *param_1, const void *param_2)
{
  const unsigned int *a;
  const unsigned int *b;

  a = (const unsigned int *)param_1;
  b = (const unsigned int *)param_2;
  return strcoll(global_cards_data[*a].name, global_cards_data[*b].name);
}

// FUNCTION: SHANDALAR 0x00532aeb
unsigned int FUN_00532aeb(char *title, unsigned int color_mask, unsigned int type_mask, int param_4, int param_5)
{
  struct
  {
    int needs_redraw;
    int i;
    int j;
    int line_h;
    int line_top;
    int col_left;
    int max_lines_per_col;
    int draw_count;
    unsigned int selected_card_id;
    unsigned int card_list[300];
    unsigned int draw_index_to_card[64];
    unsigned int card_list_count;
    int button_colors[3];
    int menu_context;
  } s;

  s.needs_redraw = 1;
  s.selected_card_id = 0xffffffff;

  ClearInputAndWaitForMouseRelease();

  if (param_4 != 0)
  {
    LoadPcxResource(1, 0, global_screen_height - 0x1e0, s_tradscrn_pic_0058f138, (void *)0);
    StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, global_screen_height - 0x1e0, 0x280, 0x1e0, PTR_DAT_005832dc, 0, 0, global_screen_width,
                            global_screen_height);
    CopyGraphicsRect(PTR_DAT_005832dc, 0, 0, global_screen_width, global_screen_height, PTR_DAT_005832b4, 0, 0);
  }

  if (param_5 != 0)
  {
    s.button_colors[0] = 0x1a;
    s.button_colors[1] = 0x19;
    s.button_colors[2] = 0x19;

    LoadPcxIntoPage(1, s_tradbut1_pic_0058f148);
    BeginSpriteEncodeSession();
    PTR_DAT_005832dc->font_slot = 7;

    for (s.i = 0; s.i < 3; s.i = s.i + 1)
    {
      SetFontStyleSize(7, (unsigned int)(((-(unsigned int)(s.i == 2)) & 0xfffffffeU) + 0xb));
      DrawFormattedTextNoShadowCentered(PTR_DAT_005832dc, s.button_colors[s.i], s.i * 0x62 + 0x32, 0xd, "%s", (char *)g_done_text_table_entry);
      DAT_00746b10[s.i] = EncodeSpriteFromPage(1, s.i * 0x62 + 1, 1, 0x61, 0x1b);
    }

    FinalizeSpriteEncodeSession();

    if (DAT_0058f0d0 == DAT_0058f0c0)
    {
      DAT_0058f0d0 = ScaleUiCoordinate(DAT_0058f0d0);
      DAT_0058f0d4 = ScaleUiCoordinate(DAT_0058f0d4);
      DAT_0058f0d8 = ScaleUiCoordinate(DAT_0058f0d8);
      DAT_0058f0dc = ScaleUiCoordinate(DAT_0058f0dc);
    }

    s.menu_context = BeginMenuContext();
    (void)ResetMenuContext(s.menu_context);
    EndMenuContext();
    (void)FUN_005001e3();
  }

  if (type_mask == 0xffffffff)
  {
    if (param_4 != 0)
    {
      type_mask = 0;
      DAT_0058f0b4 = 1;
    }
  }
  else
  {
    DAT_0058f0b4 = (int)type_mask;
    type_mask = 1U << ((unsigned char)type_mask & 0x1f);
    if (type_mask == 0x10)
    {
      type_mask = 0x30;
    }
  }

  if ((color_mask != 0) && (param_4 != 0))
  {
    _DAT_0058f0b0 = FUN_0040dffd((int)color_mask);
  }

  if (param_4 != 0)
  {
    DAT_0058f0b8 = 0xffffffff;
  }

  while (1)
  {
    if (s.needs_redraw != 0)
    {
      s.card_list_count = 0;
      for (s.i = 0; s.i < g_card_count; s.i = s.i + 1)
      {
        if ((param_4 != 0) || (FUN_004bb1cf(s.i) > 0))
        {
          s.card_list[s.card_list_count] = (unsigned int)s.i;
          s.card_list_count = s.card_list_count + 1;
          if (s.card_list_count >= 300)
          {
            break;
          }
        }
      }

      qsort(s.card_list, s.card_list_count, 4, FUN_00532aae);

      s.draw_count = 0;
      s.col_left = FUN_005501dc(0x219) / 2;
      s.line_top = FUN_005501dc(0x99) / 2;
      s.line_h = FUN_005501dc(0x12);
      s.max_lines_per_col = 0x20;

      for (s.i = 0; s.i < (int)s.card_list_count && s.draw_count < 64; s.i = s.i + 1)
      {
        s.draw_index_to_card[s.draw_count] = s.card_list[s.i];
        strcpy(g_ui_message_buffer, global_cards_data[s.card_list[s.i]].name);
        DrawCenteredTextLineWithShadow(g_ui_message_buffer, s.col_left, s.line_top, 0x54);

        s.draw_count = s.draw_count + 1;
        s.line_top = s.line_top + s.line_h;
        if (s.draw_count == s.max_lines_per_col)
        {
          s.col_left = FUN_005501dc(0xd) + FUN_005501dc(0x1c0);
          s.line_top = FUN_005501dc(0x99) / 2;
        }
      }

      s.needs_redraw = 0;
      FUN_0046ed03();
      DAT_00650268 = -1;
    }

    while ((UpdateMouseSnapshot(), (param_5 == 0) || (g_mouse_button_down_mask == 0)))
    {
      if (g_mouse_button_down_mask != 0)
      {
        break;
      }
      if (param_5 != 0)
      {
        (void)UpdateMenuControlSelection(g_mouse_x_snapshot, g_mouse_y_snapshot, 0);
      }
    }

    (void)UpdateMenuControlSelection(g_mouse_x_snapshot, g_mouse_y_snapshot, g_mouse_button_down_mask);
    if (0 < DAT_00650268)
    {
      (void)EndMenuContext();
      FreeSpriteBlob(DAT_00746b10);
      ClearInputAndWaitForMouseRelease();
      DestroyCachedCardArt();
      return 0xffffffff;
    }

    if (g_mouse_button_down_mask == 0)
    {
      if (param_5 != 0)
      {
        (void)EndMenuContext();
      }
      FreeSpriteBlob(DAT_00746b10);
      DestroyCachedCardArt();
      return 0xffffffff;
    }

    /* Click in list area selects a card. */
    s.j = (g_mouse_y_snapshot - FUN_005501dc(0x99) / 2) / s.line_h;
    if ((s.j >= 0) && (s.j < s.draw_count))
    {
      DAT_0058f0b8 = s.draw_index_to_card[s.j];
      s.needs_redraw = 1;
    }
  }
}

// FUNCTION: SHANDALAR 0x00530b7e
int FUN_00530b7e(void)
{
  // TODO(decomp): Callback used by the card-chooser UI while a "quest card marking" flow is active.
  return 0;
}

// FUNCTION: SHANDALAR 0x00530b90
void FUN_00530b90(int param_1, int param_2)
{
  (void)param_1;
  (void)param_2;
  // TODO(decomp): Called by FUN_004f18c0 before refreshing the city UI; likely updates trade/quest state.
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
