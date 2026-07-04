#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <setjmp.h>

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
extern int g_amulet_inventory[5];
extern int g_text_menu_abort_requested;
extern card_data_t global_cards_data[];
extern int g_card_count;
extern int DAT_00589dec;
extern int DAT_009300f0;
extern char DAT_0097e340[0x110];
extern int DAT_007894f4;
extern int g_current_quest_giver_town_index;
extern int g_done_text_table_entry;
extern int g_world_player_tile_x;
extern int g_world_player_tile_y;
extern long DAT_005a6198[0x4e2];
extern char text_lines[249][300];
extern int g_loadsave_skip_esc;
extern jmp_buf DAT_0073e990;
extern jmp_buf DAT_0073e9e0;

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
void ShowDungeonCluesScreen(int unused);
int RestoreAdventureUiPaletteAndFocus(void);
void FUN_00549002(void);

void FUN_004f2407(int card_index, int x, int y, int full_card, char *banner_label);
int FUN_00522508(int param_1);
int ReadSpriteEntryPointers(EncodedImage **out_sprite_entries, char *sprite_path);
void FUN_004f263b(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, char *param_7);
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

void DrawFormattedTextNoShadowCentered(FacemakerWindowBounds *dst, int text_color, int x, int y, char *format, ...);
void DrawFormattedTextShadowedCentered(FacemakerWindowBounds *window, int color_index, int x, int y, char *format, ...);
void DrawFormattedTextShadowed(FacemakerWindowBounds *window, int color_index, int x, int y, char *format, ...);
unsigned int BlitRectByRandomTileOrderInPlace(HDC dst, int dst_x, int dst_y, int width, int height, int tile_w, int tile_h, HDC src);
void FUN_0057d500(HDC dst_hdc, int x, int y, int w, int h, int unk_20, int unk_a, int unk_b, int unk_c, HDC src_hdc);

void DestroyAllCardBackgrounds(void);
void DestroyAllBigArts(void);
void DestroyAllSmallArts(void);
void InitializeNewGameState(void);

int FUN_004ecf30(int x, int y);
int GetRelativeWorldQuadrant(int world_x, int world_y);
int FUN_0056bd9d(unsigned int card_id);
int FUN_004bb1cf(int param_1);
int FUN_0056c0e5(int param_1, int param_2, int param_3);
int FUN_0056c5ea(int param_1);
int FUN_0056c705(int param_1);
int FindDeckSlotForQuestColorAndType(unsigned char quest_color, unsigned char quest_bitmap_mask);
char *FUN_00561441(int creature_type);
DWORD FUN_00564e70(char *dst, DWORD max_length, LPCVOID format, ...);
char *GetQuestCardClassName(int quest_bitmap_mask);
int FUN_004ef1ea(int town_index);
int FUN_00418a2d(int param_1);
int FindHintPairIndexForOfferCard(int card_internal_id);
int FUN_004f1cce(int param_1);
int FUN_004bb458(int param_1);
char *FUN_004f2e17(int town_index);
char *BuildTownDisplayName(int town_index);
extern int DAT_0058c620[];
void FUN_004f23bc(char *param_1, DWORD param_2);
unsigned int FUN_005318f5(int param_1, int param_2, int param_3);
int FUN_00530b90(int param_1, int param_2);
char *FUN_0057e826(char *dst, char *src);

int FUN_00568320(unsigned int param_1, int param_2);
int FUN_005636ab(void);
int FUN_0056139e(int wizard_color, int creature_tier);
void FUN_0050a5f1(int creature_type, unsigned int name_id, unsigned int param_3, int param_4);
void FUN_005222ae(char *param_1, int param_2, unsigned int param_3, int param_4);
int FUN_0052234e(int library_index);
int FUN_0056a515(int param_1, int *param_2, int param_3, char *param_4, int param_5, int *param_6);
void FUN_004f6886(int creature_type, int x_320, int y_200, int tinted, int mode);
void FUN_005629c3(int tune_index);
void FUN_004f6d32(unsigned int card_id);
unsigned int FUN_00532aeb(char *title, unsigned int color_mask, unsigned int type_mask, int param_4, int param_5);
int FUN_00530b7e(void);
void DrawTiledDialogBoxFrame(int x, int y, int width, int height, int frame_style);
int HandleMainMenuButtonControlEvent(void *control_ptr, int event_type);
int HandlePortraitMainMenuControlEvent(void *control_ptr, int event_type);
void CopyGraphicsRect(FacemakerWindowBounds *src_page, int src_x, int src_y, int width, int height, FacemakerWindowBounds *dst_page,
                      int dst_x, int dst_y);
void LoadPcxIntoPageNoPalette(char *path);
int RunSaveMenuAndSelectSlot(void);
int FUN_005031a8(void);
void SaveGameToSlot(int save_slot_index);
int LoadGameFromSlot(int save_slot_index);
int RestoreAdventureUiPaletteAndFocus(void);
void ShowWorldMapScreen(int mode);
void ShowCityInfoScreen(int param_1);
void ShowDungeonCluesScreen(int unused);
void ShowStatsWindow(int mode, int highlight);

void FUN_004f2aa1(int param_1, int param_2, int param_3, int param_4, char *param_5);
int BeginMenuContext(void);
int ResetMenuContext(int context_index);
int AddMenuControlsToContext(AdvMenuControl *controls, int control_count, int context_index);
int EndMenuContext(void);
int UpdateMenuControlSelection(int mouse_x, int mouse_y, int allow_activate_on_click);
int QueuePendingMenuActionInput(void);
int FUN_005001e3(void);
int LoadTextSectionLines(char *filename, char *section);
int RunTextMenuAtScaled(char *menu_text, int x_320_scale, unsigned int y_200_scale);
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
int(__cdecl *DAT_00580da4)(void);

// GLOBAL: SHANDALAR 0x008e2e50
int DAT_008e2e50[16];

// GLOBAL: SHANDALAR 0x0058edd8
int DAT_0058edd8[10] = {0x24d, 0x1b3, 0xc1, 0x1b8, 0x106, 0x1d0, 0x87, 0x1dd, 0xd3, 0xd};
// GLOBAL: SHANDALAR 0x0058ee00
int DAT_0058ee00 = -1;
// GLOBAL: SHANDALAR 0x0058ee04
int DAT_0058ee04 = -1;

// GLOBAL: SHANDALAR 0x0058f068
char s_wiseman3_pic_0058f068[0x10] = "wiseman3.pic";
// GLOBAL: SHANDALAR 0x0058f078
char s_wiseman3_pic_0058f078[0x10] = "wiseman3.pic";
// GLOBAL: SHANDALAR 0x0058f088
char DAT_0058f088[4] = "";
// GLOBAL: SHANDALAR 0x0058f08c
char s_wiseman3_pic_0058f08c[0x10] = "wiseman3.pic";
// GLOBAL: SHANDALAR 0x0058f09c
char s_wiseman3_pic_0058f09c[0x10] = "wiseman3.pic";

// GLOBAL: SHANDALAR 0x0058f0ac
int DAT_0058f0ac;

// GLOBAL: SHANDALAR 0x0058e048
int g_showlibrary_menu_selection;
// GLOBAL: SHANDALAR 0x0058edd0
int g_wiseman_city_block_active_town_count;

// GLOBAL: SHANDALAR 0x0058f0b0
int _DAT_0058f0b0 = 1;
// GLOBAL: SHANDALAR 0x0058f0b4
int DAT_0058f0b4 = 1;
// GLOBAL: SHANDALAR 0x0058f0b8
unsigned int DAT_0058f0b8 = 0xffffffff;
// GLOBAL: SHANDALAR 0x0058f0bc
unsigned int DAT_0058f0bc;

int FUN_005328e4(AdvMenuControl *control, int mode);
int FUN_00532a7c(AdvMenuControl *control);
extern int DAT_0058f114[4];

// GLOBAL: SHANDALAR 0x0058f0c0
AdvMenuControl DAT_0058f0c0 = {
    0x110, 5, 0x61, 0x1b, 0x110, 5, 0x61, 0x1b, 1, (AdvMenuRenderCallback)FUN_005328e4, (AdvMenuActivateCallback)FUN_00532a7c, 1, 0, (char *)DAT_0058f114, (char *)(&DAT_0058f114[2]), 0, 0, {0, 0, 0, 0}};

// GLOBAL: SHANDALAR 0x0058f114
int DAT_0058f114[4] = {0x0d644420, 0x1b, 0x0d644420, 0x1b};

// GLOBAL: SHANDALAR 0x00746b10
EncodedImage *DAT_00746b10[3];

// GLOBAL: SHANDALAR 0x00650048
int g_wiseman_city_block_town_choice_index_a;
// GLOBAL: SHANDALAR 0x0065004c
int g_wiseman_city_block_town_choice_index_b;

// GLOBAL: SHANDALAR 0x00650050
int g_wiseman_city_block_color_index;
// GLOBAL: SHANDALAR 0x00650058
char g_wiseman_city_block_subst_a[0x100];
// GLOBAL: SHANDALAR 0x00650158
int DAT_00650158;
// GLOBAL: SHANDALAR 0x00650160
char g_wiseman_city_block_subst_b[0x100];
// GLOBAL: SHANDALAR 0x00650260
int g_wiseman_city_block_world_magic_slot_index;

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
char DAT_007481f0[5][0x64];

void FUN_004f04c5(void);

// GLOBAL: SHANDALAR 0x0058ab58
void(__cdecl *PTR_FUN_0058ab58)(void) = FUN_004f04c5;
// GLOBAL: SHANDALAR 0x0058ab70
int _DAT_0058ab70;

int FUN_004f0b50(AdvMenuControl *control, int mode);
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
char DAT_0058abdc[] = "\x1b \0\0\x1b \0\0\x1b \0\0\x1b \0";

// GLOBAL: SHANDALAR 0x0058ad7c
char s_x_sound_button2_wav_0058ad7c[] = "x:sound\\button2.wav";
// GLOBAL: SHANDALAR 0x0058ad90
char s_x_sound_button_wav_0058ad90[] = "x:sound\\button.wav";

// GLOBAL: SHANDALAR 0x0058f124
char s_x_sound_button2_wav_0058f124[] = "x:sound\\button2.wav\0tradscrn.pic\0\0\0\0tradbut1.pic\0\0\0\0";

#define s_tradscrn_pic_0058f138 (s_x_sound_button2_wav_0058f124 + 0x14)
#define s_tradbut1_pic_0058f148 (s_x_sound_button2_wav_0058f124 + 0x24)

// GLOBAL: SHANDALAR 0x0058f164
char DAT_0058f164[4] = "-";
// GLOBAL: SHANDALAR 0x0058f168
char DAT_0058f168[4] = "";

// GLOBAL: SHANDALAR 0x0058ae14
char s_wiseman3_pic_0058ae14[0x10] = "wiseman3.pic";
// GLOBAL: SHANDALAR 0x0058ae3c
char s_wiseman3_pic_0058ae3c[0x10] = "wiseman3.pic";
// GLOBAL: SHANDALAR 0x0058ae4c
char s_wiseman3_pic_0058ae4c[0x10] = "wiseman3.pic";
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
// GLOBAL: SHANDALAR 0x0058aef0
char s_x_sound_button2_wav_0058aef0[] = "x:sound\\button2.wav";
// GLOBAL: SHANDALAR 0x0058af1c
char s_x_sound_button2_wav_0058af1c[] = "x:sound\\button2.wav";
// GLOBAL: SHANDALAR 0x0058af48
char s_x_sound_button2_wav_0058af48[] = "x:sound\\button2.wav";
// GLOBAL: SHANDALAR 0x0058af8c
char s_x_sound_button2_wav_0058af8c[] = "x:sound\\button2.wav";

int FUN_004ef0a8(AdvMenuControl *control, int mode);
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

// GLOBAL: SHANDALAR 0x0058ada4
char DAT_0058ada4[] = "";
// GLOBAL: SHANDALAR 0x0058ada8
char s_advfac64_pic_0058ada8[] = "advfac64.pic";
// GLOBAL: SHANDALAR 0x0058adb8
char s_advfac64_pic_0058adb8[] = "advfac64.pic";
// GLOBAL: SHANDALAR 0x0058adc8
char s_SHUTDOWN_0058adc8[] = "SHUTDOWN";
// GLOBAL: SHANDALAR 0x0058add4
char s_ADVstrings_txt_0058add4[] = "ADVstrings.txt";

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

// GLOBAL: SHANDALAR 0x00580e64
char DAT_00580e64[] = "rb\0";
// GLOBAL: SHANDALAR 0x00580e68
char s_master_csv_00580e68[] = "master.csv";
// GLOBAL: SHANDALAR 0x00580e74
char s______________00580e74[] = "%[^,\n] %[,\n]";
// GLOBAL: SHANDALAR 0x00580e84
char DAT_00580e84[] = ",";

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

#define GET_TOWN_PIC(_i) ((g_town_slots[_i].location_type == 1) ? "village.pic" : "city.pic")

int IsCardAvailable(csvid_t csvid, int expansion);

// FUNCTION: SHANDALAR 0x00428b05
unsigned int FUN_00428b05(int card_index)
{
  unsigned int mask = IsCardAvailable(global_cards_data[card_index].id, 0);

  if (HasExpansion(2) != 0)
  {
    mask |= IsCardAvailable(global_cards_data[card_index].id, 1);
  }

  if (HasExpansion(4) != 0)
  {
    mask |= IsCardAvailable(global_cards_data[card_index].id, 2);
  }

  return mask;
}

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
    AdvMenuRect clip_restore;
    AdvMenuRect clip_temp;
    int resolution_index;
    EncodedImage *questnew_entry;
    AdvMenuRect saved_clip_rect;
  } s;

  s.questnew_entry = g_questnew_sprite_entries[0];

  switch (global_screen_width)
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
  struct
  {
    int scale_h;
    int scale_w;
    int unused_local;
    int scale_step;
    int dst_y;
    int center_y;
    int copy_h;
    int copy_w;
  } s;

  s.dst_y = global_screen_height - 0x118;
  LoadPcxResource(1, 0, s.dst_y, pcx_path, (void *)0);

  s.copy_w = FUN_005501dc(0x100);
  s.copy_h = FUN_005501dc(0x8c);
  s.center_y = FUN_005501dc(0x5e);

  for (s.scale_step = 2; s.scale_step <= 8; s.scale_step = s.scale_step + 1)
  {
    s.scale_w = s.copy_w * s.scale_step / 8;
    s.scale_h = s.copy_h * s.scale_step / 8;
    s.unused_local = ((s.copy_h / 2) * (s.scale_step - 2) + (8 - s.scale_step) * (global_screen_height / 3)) / 6;
    StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, s.dst_y, 0x200, 0x118, PTR_DAT_005832b4,
                            global_screen_width / 2 - s.scale_w / 2, s.center_y - s.scale_h / 2, s.scale_w, s.scale_h);
  }

  s.scale_w = s.copy_w;
  s.scale_h = s.copy_h;
  StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, s.dst_y, 0x200, 0x118, PTR_DAT_005832dc,
                          global_screen_width / 2 - s.scale_w / 2, s.center_y - s.scale_h / 2, s.scale_w, s.scale_h);
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
  FUN_004ce992(0x2d);
  return 0;
}

// FUNCTION: SHANDALAR 0x00561bf4
void FUN_00561bf4(int param_1)
{
  struct
  {
    int unused_local;             /* -0x80 */
    int ruled_by_color_count[7];  /* -0x7c */
    int town_index;               /* -0x60 */
    int color;                    /* -0x5c */
    int best_score;               /* -0x58 */
    int nearest_distance;         /* -0x54 */
    int best_color;               /* -0x50 */
    int lair_world_y_by_color[7]; /* -0x4c */
    int lair_world_x_by_color[7]; /* -0x30 */
    int scan_index;               /* -0x14 */
    int creature_tier;            /* -0x10 */
    int dist;                     /* -0x0c */
    int best_town_index;          /* -0x08 */
    int score;                    /* -0x04 */
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
      if (s.color == param_1)
      {
        s.lair_world_x_by_color[s.color] = g_town_slots[s.scan_index].world_x;
        s.lair_world_y_by_color[s.color] = g_town_slots[s.scan_index].world_y;
      }
    }

    if (*(char *)((char *)&g_town_slots[s.scan_index].status_and_ruling_wizard + 1) != 0)
    {
      *(int *)((char *)s.ruled_by_color_count + (((int)(g_town_slots[s.scan_index].status_and_ruling_wizard & 0xffffff3f)) >> 6)) += 1;
    }
  }

  s.unused_local = 0;
  s.best_score = 0x7fff;

  for (s.town_index = 0; s.town_index < 0x80; s.town_index++)
  {
    if ((((*(char *)((char *)&g_town_slots[s.town_index].status_and_ruling_wizard + 1) == 0) && (g_town_slots[s.town_index].location_type != 4)) &&
         (g_town_slots[s.town_index].location_type != 1)) &&
        (g_town_slots[s.town_index].location_type != 5))
    {
      s.nearest_distance = 0x7fff;

      for (s.scan_index = 1; s.scan_index < 6; s.scan_index++)
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

      s.score = FUN_00522508(0x80) + (s.ruled_by_color_count[s.color] << 5);
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
    switch ((int)(g_siege_timer + ((g_siege_timer >> 0x1f) & 0x7f)) >> 7)
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

    FreeOpeningMenuSpriteWorkEntries(s.scan_index, s.scan_index + 8);
    g_lair_or_monster_slots[s.scan_index].entry_type = FUN_0056139e(s.color, s.creature_tier);
    g_lair_or_monster_slots[s.scan_index].world_x = g_town_slots[s.town_index].world_x * 0x20 + 0x10;
    g_lair_or_monster_slots[s.scan_index].world_y = g_town_slots[s.town_index].world_y * 0x20 + 0x10;
    g_lair_or_monster_slots[s.scan_index].color = s.color;
    DAT_0073ea68 = DAT_0073ea68 + 1;
    FUN_00431526(0x80, g_town_slots[s.town_index].world_x, g_town_slots[s.town_index].world_y);

    PlaySoundEffectOnChannel(s_x_sound_newsflash_wav_005916bc, 0x97, 100, 100, 0);
    AnimateVisitBackdropZoomIn(s_newsback_pic_005916d4);

    strcpy(g_ui_message_buffer, gs_newsflash_0077d140[0]);
    if (FUN_0056615f(s.color) != 0)
    {
      FUN_00564e70(g_ui_message_buffer + strlen(g_ui_message_buffer), 0x1000, gs_newsflash_0077d140[2], gs_wizardnames_0077ee70 + s.color * 0x32,
                   BuildCreatureNameWithArticle(g_lair_or_monster_slots[s.scan_index].entry_type), BuildTownDisplayName(s.town_index));
    }
    else
    {
      FUN_00564e70(g_ui_message_buffer + strlen(g_ui_message_buffer), 0x1000, gs_newsflash_0077d140[3], gs_wizardnames_0077ee70 + s.color * 0x32,
                   BuildCreatureNameWithArticle(g_lair_or_monster_slots[s.scan_index].entry_type), BuildTownDisplayName(s.town_index));
    }

    PTR_DAT_005832b4->font_slot = 5;
    DrawTextAt(PTR_DAT_005832b4, 0xbe, 0x140, 0xf7, g_ui_message_buffer);
    PTR_DAT_005832b4->font_slot = 1;
    ClearInputAndWaitForMouseRelease();
    (void)WaitForInputEventUnlessBlocked();
    RefreshAdventureInterfaceLayout();
    g_siege_indicator = 1;
  }
}

// FUNCTION: SHANDALAR 0x00430fb5
void FUN_00430fb5(char *text, int center_x_320, int y_200, int color_index)
{
  center_x_320 = (center_x_320 * global_screen_width) / 0x140;
  y_200 = (y_200 * global_screen_height) / 0xf0;
  DrawCenteredTextLineWithShadow(text, center_x_320, y_200, color_index);
}

// FUNCTION: SHANDALAR 0x004313cf
void FUN_004313cf(FacemakerWindowBounds *dst, int x_320, int y_200, int w_320, int h_200, EncodedImage *sprite)
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
void FUN_004f2aa1(int param_1, int param_2, int param_3, int param_4, char *param_5)
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
int FUN_004ef0a8(AdvMenuControl *control, int mode)
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
               DAT_007481f0[control->unk_30]);

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
  LoadVisitBackdropAndCopyToPage0(GET_TOWN_PIC(g_current_town_slot_index));
  PTR_DAT_005832b4->page_number = 0;

  // 0x0058aa34 == &DAT_0058a9e0[1]
  (void)FUN_004ef0a8(&DAT_0058a9e0[1], 1);
}

// FUNCTION: SHANDALAR 0x004f04c5
void FUN_004f04c5(void)
{
  int town_index;
  int(__cdecl * saved_callback)(void);
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
    AnimateVisitBackdropZoomIn(GET_TOWN_PIC(town_index));
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
    DAT_00580da4 = FUN_00530b7e;

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
  AnimateVisitBackdropZoomIn(GET_TOWN_PIC(town_index));
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
  AnimateVisitBackdropZoomIn(GET_TOWN_PIC(town_index));
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
  AnimateVisitBackdropZoomIn(GET_TOWN_PIC(town_index));
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
  AnimateVisitBackdropZoomIn(GET_TOWN_PIC(town_index));
  _DAT_007481d8 = -2;
}

// FUNCTION: SHANDALAR 0x004f1a67
void FUN_004f1a67(void)
{
  int town_index;

  town_index = _DAT_007483ec;
  PlaySoundEffectOnChannel(s_x_sound_button2_wav_0058af48, 0xf, 100, 100, 0);
  ShowDungeonCluesScreen(1);
  RefreshAdventureInterfaceLayout();
  FUN_0055060c(1);
  AnimateVisitBackdropZoomIn(GET_TOWN_PIC(town_index));
  _DAT_007481d8 = -2;
}

// FUNCTION: SHANDALAR 0x004f1b4e
void FUN_004f1b4e(void)
{
  int town_index;

  town_index = _DAT_007483ec;
  PlaySoundEffectOnChannel(s_x_sound_button2_wav_0058af8c, 0xf, 100, 100, 0);
  ClearInputAndWaitForMouseRelease();
  FUN_00549002();
  RefreshAdventureInterfaceLayout();
  FUN_0055060c(1);
  AnimateVisitBackdropZoomIn(GET_TOWN_PIC(town_index));
  _DAT_007481d8 = -2;
}

// FUNCTION: SHANDALAR 0x004f0d09
void FUN_004f0d09(void)
{
  struct
  {
    char *accept_scan;         /* -0x134 */
    int deck_slot;             /* -0x130 */
    int accepted;              /* -0x12c */
    char *accept_cursor;       /* -0x128 */
    int mouse_y;               /* -0x124 */
    int mouse_x;               /* -0x120 */
    int title_y_200;           /* -0x11c */
    int title_line_h;          /* -0x118 */
    int center_x_320;          /* -0x114 */
    int title_text_w;          /* -0x110 */
    char *title;               /* -0x10c */
    int loop_i;                /* -0x108 */
    int colors[3];             /* -0x104 */
    EncodedImage *sprites[13]; /* -0xf8 */
    int menu_context;          /* -0xc4 */
    int tile_w;                /* -0xc0 */
    int selected_idx;          /* -0xbc */
    int mana_mask;             /* -0xb8 */
    int key;                   /* -0xb4 */
    int idx;                   /* -0xb0 */
    int y_base;                /* -0xac */
    int item_count;            /* -0xa8 */
    int town_index;            /* -0xa4 */
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
  AnimateVisitBackdropZoomIn(GET_TOWN_PIC(s.town_index));
  _DAT_007481d8 = -2;
  FreeSpriteBlob(DAT_005b7e20);
  FreeSpriteBlob(DAT_005b7e40[0]);
}

// FUNCTION: SHANDALAR 0x004f0b50
int FUN_004f0b50(AdvMenuControl *control, int mode)
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
int FUN_00418a2d(int param_1)
{
  int i;

  for (i = 0; i < 500; i++)
  {
    if (global_cards_data[deck[i] & 0xfff].id == param_1)
    {
      return 1;
    }
  }
  return 0;
}

// FUNCTION: SHANDALAR 0x0041879d
int FindHintPairIndexForOfferCard(int card_internal_id)
{
  struct
  {
    int candidate_hint_pair_count;
    int deck_scan_index;
    int hint_pair_index;
    int is_hint_second_in_deck;
    int result_hint_pair_index;
    int deck_card_internal_id;
    int is_hint_first_in_deck;
    int candidate_hint_pair_indices[256];
  } s;

  s.result_hint_pair_index = -1;
  s.candidate_hint_pair_count = 0;
  s.hint_pair_index = 0;

  for (; s.hint_pair_index < 0x100; s.hint_pair_index = s.hint_pair_index + 1)
  {
    if (DAT_0097e450[s.hint_pair_index].second != -1)
    {
      s.is_hint_first_in_deck = FUN_00418a2d(DAT_0097e450[s.hint_pair_index].first);
      s.is_hint_second_in_deck = FUN_00418a2d(DAT_0097e450[s.hint_pair_index].second);

      if ((s.is_hint_first_in_deck == 0) || (s.is_hint_second_in_deck == 0))
      {
        if ((DAT_0097df40[s.hint_pair_index] & (1 << (unsigned char)g_shandalar_difficulty)) != 0)
        {
          if ((global_cards_data[card_internal_id].id == DAT_0097e450[s.hint_pair_index].first) && (s.is_hint_second_in_deck != 0))
          {
            s.candidate_hint_pair_indices[s.candidate_hint_pair_count] = s.hint_pair_index;
            s.candidate_hint_pair_count = s.candidate_hint_pair_count + 1;
          }

          if ((DAT_0097e450[s.hint_pair_index].second == global_cards_data[card_internal_id].id) && (s.is_hint_first_in_deck != 0))
          {
            s.candidate_hint_pair_indices[s.candidate_hint_pair_count] = s.hint_pair_index;
            s.candidate_hint_pair_count = s.candidate_hint_pair_count + 1;
          }
        }
      }
    }
    else
    {
      if (global_cards_data[card_internal_id].id == DAT_0097e450[s.hint_pair_index].first)
      {
        if ((FUN_00418a2d(DAT_0097e450[s.hint_pair_index].first) == 0) &&
            ((DAT_0097df40[s.hint_pair_index] & (1 << (unsigned char)g_shandalar_difficulty)) != 0))
        {
          s.deck_scan_index = 0;
          while (s.deck_scan_index < 500)
          {
            s.deck_card_internal_id = deck[s.deck_scan_index] & 0xfff;

            if (((unsigned char)global_cards_data[s.deck_card_internal_id].type & 1) != 0)
            {
              if (((signed char)global_cards_data[s.deck_card_internal_id].color &
                   (signed char)global_cards_data[card_internal_id].color) != 0)
              {
                break;
              }
            }

            s.deck_scan_index = s.deck_scan_index + 1;
          }
        }
      }
    }
  }

  if (s.candidate_hint_pair_count != 0)
  {
    s.result_hint_pair_index = FUN_00522508(s.candidate_hint_pair_count);
    s.result_hint_pair_index = s.candidate_hint_pair_indices[s.result_hint_pair_index];
  }

  return s.result_hint_pair_index;
}

// FUNCTION: SHANDALAR 0x004f1cce
int FUN_004f1cce(int param_1)
{
  int local_10;
  int local_c;
  int local_8;

  local_8 = 0x28;
  switch (global_cards_data[param_1].type)
  {
  case 2:
  case 'B':
    local_10 = (int)global_cards_data[param_1].power & 0xffffbfff;
    if (local_10 + 2 == 0)
    {
      local_10 = (int)global_cards_data[param_1].power;
    }
    else
    {
      local_10 = local_10 + 3;
    }

    local_c = (int)global_cards_data[param_1].toughness & 0xffffbfff;
    if (local_c + 2 == 0)
    {
      local_c = (int)global_cards_data[param_1].toughness;
    }
    else
    {
      local_c = local_c + 3;
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

    local_8 = local_8 / ((char)global_cards_data[param_1].cc[0] + abs((int)(char)global_cards_data[param_1].cc[1]) + 1);
    break;

  case 4:
    local_10 = abs((int)(char)global_cards_data[param_1].cc[1]) + (char)global_cards_data[param_1].cc[0] + *(int *)global_cards_data[param_1].reserved4;
    local_10 = local_10 * 5 + 5;
    local_8 = local_10 * 5;
    if ((global_cards_data[param_1].extra_ability & 3) != 0)
    {
      local_8 = local_10 * 10;
    }
    break;

  case '\b':
    local_8 = ((abs((int)(char)global_cards_data[param_1].cc[1]) + (char)global_cards_data[param_1].cc[0] + *(int *)global_cards_data[param_1].reserved4) * 4 + 4) * 5;
    break;

  case '@':
    local_8 = 0xfa / (*(int *)global_cards_data[param_1].reserved4 + abs((int)(char)global_cards_data[param_1].cc[1]) + 2);
    break;

  case 0x10:
  case ' ':
    local_8 = *(int *)global_cards_data[param_1].reserved4 * 0x14 +
              ((abs((int)(char)global_cards_data[param_1].cc[1]) + (char)global_cards_data[param_1].cc[0]) * 5 + 5) * 8;
    if ((char)global_cards_data[param_1].cc[1] == -1)
    {
      local_8 = (local_8 * 3) / 2;
    }
    break;
  }

  switch (FUN_0056c5ea(param_1))
  {
  case 2:
    local_8 = ClampIntToRange(local_8 * 2, 100, 9999);
    break;
  case 3:
    local_8 = ClampIntToRange(local_8 << 2, 200, 9999);
    break;
  case 4:
    local_8 = ClampIntToRange(local_8 << 3, 200, 9999);
    break;
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
    /* Misc temps that live below the hint-pair locals in the original stack frame. */
    unsigned int tmp_mask;        // ebp - 0x94
    unsigned int save_slot_index; // ebp - 0x90
    int menu_context;             // ebp - 0x8c
    int rand_slot;                // ebp - 0x88
    int hint_pair_count;          // ebp - 0x84
    struct
    {
      int hint_pair_index;
      int slot_index;
    } hint_pairs[8]; // ebp - 0x80

    /* Additional locals used by the full town-services loop (matching stack size). */
    int retry_count;              // ebp - 0x40
    int distance;                 // ebp - 0x3c
    unsigned int tile_magic_mask; // ebp - 0x38
    int ok;                       // ebp - 0x34
    int right_click_action;       // ebp - 0x30
    int scan_color;               // ebp - 0x2c
    int slot_i;                   // ebp - 0x28
    int rand_mask;                // ebp - 0x24
    int tmp_cost;                 // ebp - 0x20
    int tmp_j;                    // ebp - 0x1c
    int slot_count;               // ebp - 0x18
    int quest_button_enabled;     // ebp - 0x14
    int tmp_i;                    // ebp - 0x10
    int card_id;                  // ebp - 0xc
    int tmp_k;                    // ebp - 0x8
    int tmp_l;                    // ebp - 0x4
  } s;

  ClearInputAndWaitForMouseRelease();

  _DAT_007483ec = town_index;

  s.tile_magic_mask = FUN_005611c8(FUN_0043146b(g_town_slots[town_index].world_x, g_town_slots[town_index].world_y));

  s.slot_count = g_town_slots[_DAT_007483ec].location_type + 3;
  if (g_world_magic_slot_timers[1].unk_00 == 0)
  {
    s.slot_count = s.slot_count + 1;
  }
  s.slot_count = MIN(s.slot_count, 8);

  /* Pick a "preferred" color (stored in _DAT_007481e0) that the player can actually cast. */
  s.card_id = 0xffffffff;
  for (s.slot_i = 0; s.slot_i < 199; s.slot_i = s.slot_i + 1)
  {
    s.scan_color = FUN_00522508(5) + 1;
    if (((DAT_0078990c[s.scan_color] != 0) || (0x50 < s.slot_i)) &&
        ((s.tile_magic_mask & (1U << (unsigned char)s.scan_color)) != 0))
    {
      s.card_id = s.scan_color;
    }
  }

  _DAT_007481e0 = s.card_id;
  _DAT_007481d4 = -1;
  DAT_005b7e34 = town_index;

  /* Town quest/encounter seed setup + build shop offer cache if we haven't visited this town yet (DAT_0058a88c). */
  if (town_index != DAT_0058a88c)
  {
    /* Choose a quest/encounter target for this visit. */
    if ((FUN_00522508(3) != 0 || (g_town_slots[town_index].status_and_ruling_wizard & 1) != 0) &&
        g_town_slots[town_index].location_type == 1)
    {
      s.quest_button_enabled = 0;
      do
      {
        DAT_005b7e38 = FUN_00522508(0x80);
        s.tmp_i = FUN_004ecf30(g_town_slots[town_index].world_x - g_town_slots[DAT_005b7e38].world_x,
                               g_town_slots[town_index].world_y - g_town_slots[DAT_005b7e38].world_y);
        s.quest_button_enabled = s.quest_button_enabled + 1;
      } while ((s.quest_button_enabled < 1000) && (((g_town_slots[DAT_005b7e38].location_type <= 1) || (g_town_slots[DAT_005b7e38].location_type == 4)) ||
                                                   (g_town_slots[DAT_005b7e38].location_type == 5) ||
                                                   (s.tmp_i < 8) ||
                                                   ((s.quest_button_enabled / 0x10 + 0x10) < s.tmp_i) ||
                                                   ((g_town_slots[DAT_005b7e38].status_and_ruling_wizard & 0xff01U) != 0)));

      if (FUN_00522508(2) != 0)
      {
        DAT_005b7da4 = 0;
      }
      else
      {
        DAT_005b7da4 = 2;
      }

      if (s.quest_button_enabled >= 1000)
      {
        if ((g_town_slots[town_index].status_and_ruling_wizard & 1) != 0)
        {
          DAT_005b7e38 = -1;
        }
        else
        {
          goto choose_lair_or_monster;
        }
      }

      s.scan_color = FUN_00522508(5) + 1;
      if ((FUN_0056c0e5(1 << (unsigned char)s.scan_color, 1 << (unsigned char)s.card_id, 3) == 0) &&
          (FUN_00522508(2) != 0))
      {
        DAT_005b7da4 = 1;
        DAT_005b7da0 = s.scan_color;
      }
    }
    else
    {
    choose_lair_or_monster:
      s.scan_color = (unsigned int)FUN_00522508(6);
      if (g_lair_or_monster_slots[s.scan_color].entry_type > 0)
      {
        DAT_005b7e38 = town_index;
        DAT_005b7da4 = -g_lair_or_monster_slots[s.scan_color].entry_type;
      }
      else
      {
        DAT_005b7e38 = -1;
      }

      if ((FUN_00522508(4) == 0) || (DAT_005b7e38 == -1))
      {
        DAT_005b7e38 = town_index;
        DAT_005b7da4 = -FUN_0056139e(s.card_id, (FUN_00522508(8) + 2) * 2);
        FreeOpeningMenuSpriteWorkEntries(0, 8);
        g_lair_or_monster_slots[0].entry_type = -1;
      }
    }

    if (DAT_005b7da4 != 1)
    {
      s.scan_color = FUN_005611c8(FUN_0043146b(g_town_slots[DAT_005b7e38].world_x, g_town_slots[DAT_005b7e38].world_y));
      do
      {
        DAT_005b7da0 = FUN_00522508(5) + 1;
      } while ((s.scan_color & (1U << (unsigned char)DAT_005b7da0)) == 0);
    }

    if ((DAT_005b7da4 == 0) && (DAT_0078990c[DAT_005b7da0] == 0))
    {
      DAT_005b7da4 = 2;
    }

    if ((g_town_slots[town_index].status_and_ruling_wizard & 8))
    {
      for (s.slot_i = 0; s.slot_i < s.slot_count; s.slot_i = s.slot_i + 1)
      {
        if (g_quest_restock_timer - *(int *)(g_town_slots[town_index].data_18_to_63 + 0x1c + s.slot_i * 4) >=
            (g_shandalar_difficulty + 3) * 5)
        {
          DAT_005b7da8[s.slot_i] = ((int *)&g_town_slots[town_index].card_slot_1)[s.slot_i];
        }
        else
        {
          DAT_005b7da8[s.slot_i] = 0xffffffff;
        }
      }
    }
    else
    {
      for (s.slot_i = 0; s.slot_i < 8; s.slot_i = s.slot_i + 1)
      {
        DAT_005b7da8[s.slot_i] = 0xffffffff;
      }
    }

    s.scan_color = FUN_00522508(s.slot_count);
    for (s.slot_i = 0; s.slot_i < s.slot_count; s.slot_i = s.slot_i + 1)
    {
      if (DAT_005b7da8[s.slot_i] != -1 ||
          (g_shandalar_difficulty + 3) * 5 >
              g_quest_restock_timer - *(int *)(g_town_slots[town_index].data_18_to_63 + 0x1c + s.slot_i * 4))
      {
        continue;
      }

      s.rand_mask = 1 << (unsigned char)FUN_00522508(7);
      do
      {
        if (s.slot_i == s.scan_color)
        {
          DAT_005b7da8[s.slot_i] = s.distance = FUN_00522508(5);
        }
        else
        {
          DAT_005b7da8[s.slot_i] = s.distance = FUN_00522508(g_card_count - 0x39);
        }

        s.ok = 0;
        for (s.right_click_action = 1; s.right_click_action < 6; s.right_click_action = s.right_click_action + 1)
        {
          if ((s.tile_magic_mask & (1U << (unsigned char)s.right_click_action)) != 0)
          {
            if (FUN_0056c0e5(1 << (unsigned char)s.right_click_action, (int)(signed char)global_cards_data[s.distance].color,
                             (s.right_click_action & 1) ? 1 : 3) != 0)
            {
              s.ok = 1;
            }
          }
        }

        if ((s.slot_i & 1) != 0)
        {
          if (((unsigned char)global_cards_data[s.distance].type & 0x40) != 0)
          {
            s.ok = 0;
          }
        }
        if ((global_cards_data[s.distance].extra_ability & 0x900) != 0)
        {
          s.ok = 0;
        }
        if (((unsigned char)global_cards_data[s.distance].expansion & 0xc1) == 0)
        {
          s.ok = 0;
        }
        if (FUN_0056c5ea(s.distance) > ((s.slot_i % 3) + 1))
        {
          s.ok = 0;
        }

      } while (FUN_004bb1cf(s.distance) <= 0 || s.ok == 0 || (global_cards_data[s.distance].extra_ability & 0x180) != 0);
    }

    /* Pick an optional "hint pair" tied to the available cards. */
    s.hint_pair_count = 0;
    for (s.slot_i = 0; s.slot_i < s.slot_count; s.slot_i = s.slot_i + 1)
    {
      s.tmp_j = FindHintPairIndexForOfferCard(DAT_005b7da8[s.slot_i]);
      if (s.tmp_j != -1)
      {
        s.hint_pairs[s.hint_pair_count].hint_pair_index = s.tmp_j;
        s.hint_pairs[s.hint_pair_count].slot_index = s.slot_i;
        s.hint_pair_count = s.hint_pair_count + 1;
      }
    }
    if (s.hint_pair_count != 0)
    {
      s.rand_slot = FUN_00522508(s.hint_pair_count);
      _DAT_007481d4 = s.hint_pairs[s.rand_slot].hint_pair_index;
      _DAT_007483e8 = s.hint_pairs[s.rand_slot].hint_pair_index;
    }

    /* Price each offer. */
    for (s.slot_i = 0; s.slot_i < 8; s.slot_i = s.slot_i + 1)
    {
      s.distance = DAT_005b7da8[s.slot_i];
      if (s.distance == -1)
      {
        continue;
      }

      s.tmp_cost = FUN_004f1cce(s.distance);
      s.tmp_cost = (g_town_slots[town_index].location_type + 2) * s.tmp_cost;

      if ((s.tile_magic_mask & (unsigned int)(signed char)global_cards_data[s.distance].color) == 0)
      {
        if (((signed char)global_cards_data[s.distance].color != 0) &&
            (FUN_0056c0e5((int)s.tile_magic_mask, (int)(signed char)global_cards_data[s.distance].color, 3) != 0))
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
  _DAT_007481d0 = s.tmp_l = (g_town_slots[town_index].location_type + 2) * 5;

  sound_unload(0x12);
  (void)LoadSoundWithDriveFallback(s_x_sound_button_wav_0058ad90, 0x12, 0);

loop:
  if (s.slot_count == 0)
  {
    DAT_005b7da8[s.slot_count] = FUN_0040dffd((int)s.tile_magic_mask) - 1;
    DAT_005b7de8[s.slot_count] = 0x28;
    s.slot_count = s.slot_count + 1;
  }

  PTR_DAT_005832b4->font_slot = 4;
  strcpy(g_ui_message_buffer, BuildTownDisplayName(town_index));
  FUN_00430fb5(g_ui_message_buffer, 0xa0, 0x1c,
               (g_town_slots[town_index].location_type == 1 ? 0xfe : 0xe0));
  PTR_DAT_005832b4->font_slot = 1;

  /* Fill sprite pointers (the render callback expects these). */
  for (s.slot_i = 0; s.slot_i < 4; s.slot_i = s.slot_i + 1)
  {
    DAT_0058a890[s.slot_i].mode_data[0] = (int)g_main_menu_button_sprites_normal[s.slot_i];
    DAT_0058a890[s.slot_i].mode_data[1] = (int)g_main_menu_button_sprites_highlight[s.slot_i];
    DAT_0058a890[s.slot_i].mode_data[2] = (int)g_main_menu_button_sprites_highlight[s.slot_i];
    DAT_0058a890[s.slot_i].mode_data[3] = (int)g_main_menu_button_sprites_normal[s.slot_i];
  }

  if (DAT_0058a9e0[0].x == DAT_0058a9e0[0].base_x)
  {
    for (s.slot_i = 0; s.slot_i < 4; s.slot_i = s.slot_i + 1)
    {
      DAT_0058a890[s.slot_i].x = ScaleUiCoordinate(DAT_0058a890[s.slot_i].base_x);
      DAT_0058a890[s.slot_i].y = ScaleUiCoordinate(DAT_0058a890[s.slot_i].base_y);
      DAT_0058a890[s.slot_i].width = ScaleUiCoordinate(DAT_0058a890[s.slot_i].base_width);
      DAT_0058a890[s.slot_i].height = ScaleUiCoordinate(DAT_0058a890[s.slot_i].base_height);
    }

    for (s.slot_i = 0; s.slot_i < 5; s.slot_i = s.slot_i + 1)
    {
      DAT_0058a9e0[s.slot_i].x = ScaleUiCoordinate(DAT_0058a9e0[s.slot_i].base_x);
      DAT_0058a9e0[s.slot_i].y = ScaleUiCoordinate(DAT_0058a9e0[s.slot_i].base_y);
      DAT_0058a9e0[s.slot_i].width = ScaleUiCoordinate(DAT_0058a9e0[s.slot_i].base_width);
      DAT_0058a9e0[s.slot_i].height = ScaleUiCoordinate(DAT_0058a9e0[s.slot_i].base_height);
    }
  }

  s.retry_count = BeginMenuContext();
  (void)ResetMenuContext(s.retry_count);
  (void)AddMenuControlsToContext(DAT_0058a890, 4, s.retry_count);
  (void)AddMenuControlsToContext(DAT_0058a9e0, 5, s.retry_count);

  /* Build button labels. */
  strcpy(DAT_007481f0[0], gs_cityscreen_buttons_0077f5e0[0]);
  sprintf(DAT_007481f0[1], gs_cityscreen_buttons_0077f5e0[1], _DAT_007481d0);
  strcpy(DAT_007481f0[2], gs_cityscreen_buttons_0077f5e0[2]);
  if (_DAT_007481d4 != -1)
  {
    strcpy(DAT_007481f0[3], gs_cityscreen_buttons_0077f5e0[3]);
  }
  else
  {
    strcpy(DAT_007481f0[3], gs_cityscreen_buttons_0077f5e0[4]);
  }
  DAT_007481f0[4][0] = '\0';
  s.tmp_k = 0;
  PTR_FUN_0058ab58 = FUN_004f04c5;

  if (g_town_slots[town_index].location_type != 1)
  {
    if (((s.card_id != -1) && (DAT_0078990c[s.card_id] != 0)) && (g_town_slots[town_index].trade_color_and_type != 0) &&
        ((g_quest_restock_timer - *(int *)(g_town_slots[town_index].data_18_to_63 + 0x40)) > ((g_shandalar_difficulty + 3) * 2 * 9)))
    {
      strcpy(g_ui_message_buffer, DAT_0058ada4);
      FUN_00564e70(DAT_007481f0[4], 0x64, gs_cityscreen_buttons_0077f5e0[5], gs_amuletnames_0077d090 + (s.card_id * 5) * 5,
                   FUN_004f2e17(town_index));
      PTR_FUN_0058ab58 = FUN_004f18c0;
    }
    else if (((g_current_quest_destination == -1) && (DAT_005b7e38 != -1)) &&
             (((g_shandalar_difficulty + 3) * 0x10) <
              (g_quest_restock_timer - *(int *)(g_town_slots[town_index].data_18_to_63 + 0x3c))))
    {
      strcpy(DAT_007481f0[4], gs_cityscreen_buttons_0077f5e0[6]);
      s.tmp_k = 1;
    }
    else
    {
      strcpy(DAT_007481f0[4], gs_cityscreen_buttons_0077f5e0[7]);
    }
  }
  else
  {
    if (((g_current_quest_destination == -1) && (DAT_005b7e38 != -1)) &&
        (((g_shandalar_difficulty + 3) * 0x10) <
         (g_quest_restock_timer - *(int *)(g_town_slots[town_index].data_18_to_63 + 0x3c))))
    {
      strcpy(DAT_007481f0[4], gs_cityscreen_buttons_0077f5e0[6]);
    }
    else
    {
      strcpy(DAT_007481f0[4], gs_cityscreen_buttons_0077f5e0[7]);
    }
  }

  if (DAT_007481f0[4][0])
  {
    _DAT_0058ab70 = 1;
  }
  else
  {
    _DAT_0058ab70 = 3;
  }

  _DAT_007481cc = 1;
  (void)FUN_005001e3();
  _DAT_007481cc = 0;

  _DAT_007481d8 = 0;

  while (_DAT_007481d8 == 0)
  {
    UpdateMouseSnapshot();
    (void)UpdateMenuControlSelection(g_mouse_x_snapshot, g_mouse_y_snapshot, g_mouse_button_down_mask);
    if ((g_mouse_button_down_mask & 2U) != 0)
    {
      s.menu_context = RunStartupMenuAndQueueInput();

      switch (s.menu_context)
      {
      case 4:
        AnimatePaletteToColor(0, DAT_00589dec);
        DeckBuilderMain(g_main_window_hwnd, 1, 3);
        (void)RestoreAdventureUiPaletteAndFocus();
        RefreshAdventureInterfaceLayout();
        break;
      case 5:
        ClearInputAndWaitForMouseRelease();
        ShowWorldMapScreen(0);
        RefreshAdventureInterfaceLayout();
        break;
      case 6:
        ClearInputAndWaitForMouseRelease();
        ShowCityInfoScreen(1);
        RefreshAdventureInterfaceLayout();
        break;
      case 7:
        ClearInputAndWaitForMouseRelease();
        ShowDungeonCluesScreen(1);
        RefreshAdventureInterfaceLayout();
        break;
      case 8:
        FUN_00549002();
        RefreshAdventureInterfaceLayout();
        break;
      case 9:
        ClearInputAndWaitForMouseRelease();
        ShowStatsWindow(0, -1);
        RefreshAdventureInterfaceLayout();
        break;
      case 2:
        g_loadsave_skip_esc = 1;
        s.save_slot_index = FUN_005031a8();
        if (s.save_slot_index != -1)
        {
          (void)LoadGameFromSlot(s.save_slot_index);
        }
        LoadPcxIntoPageNoPalette(s_advfac64_pic_0058ada8);
        RefreshAdventureInterfaceLayout();
        (void)FUN_0055060c(1);
        g_loadsave_skip_esc = 0;
        (void)EndMenuContext();
        longjmp(DAT_0073e990, 0);
        break;
      case 1:
        s.tmp_mask = (unsigned int)RunSaveMenuAndSelectSlot();
        if ((int)s.tmp_mask != -1)
        {
          SaveGameToSlot((int)s.tmp_mask);
        }
        LoadPcxIntoPageNoPalette(s_advfac64_pic_0058adb8);
        RefreshAdventureInterfaceLayout();
        break;
      case 3:
        PTR_DAT_005832b4->font_slot = 4;
        (void)LoadTextSectionLines(s_ADVstrings_txt_0058add4, s_SHUTDOWN_0058adc8);
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
        if (DAT_009300f0 != 0)
        {
          (void)EndMenuContext();
          longjmp(DAT_0073e9e0, 0);
        }
        break;
      }

      (void)FUN_0055060c(1);
      (void)EndMenuContext();
      AnimateVisitBackdropZoomIn(GET_TOWN_PIC(town_index));
      // This can't be nested loop because the "inner" loop overlaps the "outer"
      goto loop;
    }
  }

  (void)EndMenuContext();
  if (_DAT_007481d8 == -2)
  {
    goto loop;
  }

  /* Persist cached offers back into the town slot. */
  for (s.slot_i = 0; s.slot_i < 8; s.slot_i = s.slot_i + 1)
  {
    ((int *)&g_town_slots[town_index].card_slot_1)[s.slot_i] = DAT_005b7da8[s.slot_i];
  }
  DAT_0058a88c = town_index;
  (void)sound_stop(0xf);
  sound_unload(0xf);
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

  usable_card_count = 0;
  for (deck_slot = 0; deck_slot < 500; deck_slot = deck_slot + 1)
  {
    if ((deck[deck_slot] != -1) && ((deck[deck_slot] & 0x4000) == 0) && (4 < (deck[deck_slot] & 0xfff)))
    {
      usable_card_count = usable_card_count + 1;
    }
  }

  if (usable_card_count == 0)
  {
    AnimatePaletteToColor(0, DAT_00589dec);
    LoadPcxIntoPageOpaque(1, s_uth_arz_pic_005919f4);
    strcpy(g_ui_message_buffer, "");
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
  return 0;
}

// FUNCTION: SHANDALAR 0x0056139e
int FUN_0056139e(int wizard_color, int creature_tier)
{
  int creature_type;
  int tries;

  tries = 0;
  do
  {
    creature_type = FUN_00522508((int)DAT_00593934 - 1) + 1;
    tries++;
  } while (tries < 999 && (gs_creature_names_00591a08[creature_type].metadata[1] != creature_tier ||
                           (wizard_color != 0 && ((1 << (unsigned char)wizard_color) & (int)(signed char)gs_creature_names_00591a08[creature_type].metadata[4]) == 0)));

  return (tries == 999) ? 0 : creature_type;
}

// FUNCTION: SHANDALAR 0x0050a5f1
void FUN_0050a5f1(int creature_type, unsigned int name_id, unsigned int param_3, int param_4)
{
  (void)name_id;

  strcpy(g_ui_message_buffer, s_decks_0_0058cb70);
  if (*(int *)&gs_creature_names_00591a08[creature_type].metadata[6] < 100)
  {
    strcat(g_ui_message_buffer, DAT_0058cb78);
    if (*(int *)&gs_creature_names_00591a08[creature_type].metadata[6] < 10)
    {
      strcat(g_ui_message_buffer, DAT_0058cb7c);
    }
  }

  strcat(g_ui_message_buffer,
         _itoa(*(int *)&gs_creature_names_00591a08[creature_type].metadata[6], &unk_009266d0[0x70], 10));
  strcat(g_ui_message_buffer, DAT_0058cb80);

  if (FileExists(g_ui_message_buffer) != 0)
  {
    FUN_005222ae(g_ui_message_buffer, 1, param_3, param_4);
    DAT_0057a750 = 1;
  }
  else
  {
    FUN_005222ae(s_decks_0179_dck_0058cb88, 1, param_3, param_4);
    DAT_0057a750 = 1;
  }
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

// FUNCTION: SHANDALAR 0x00417946
void FUN_00417946(char *out, int csvid, int field, char *csv_name)
{
  struct
  {
    char in_quotes;
    char pad_01_to_03[3];
    int current_id;
    char delim[11];
    char pad_13;
    char token[512];
    char field_idx;
    char pad_215_to_217[3];
    FILE *fp;
    int fscanf_ret;
  } locals;

  locals.fp = fopen(csv_name, DAT_00580e64);
  locals.in_quotes = 0;
  locals.field_idx = locals.in_quotes;
  *out = '\0';

  if ((DAT_005a6198[csvid] != -1) && (strcmp(csv_name, s_master_csv_00580e68) == 0))
  {
    fseek(locals.fp, DAT_005a6198[csvid], 0);
  }

  do
  {
    locals.fscanf_ret = fscanf(locals.fp, s______________00580e74, locals.token, locals.delim);
    if (locals.fscanf_ret == 0)
    {
      break;
    }

    if (locals.token[0] == '0')
    {
      locals.current_id = atoi(locals.token);
    }

    if (locals.current_id == csvid)
    {
      ++locals.field_idx;
      if ((locals.field_idx == field) && locals.in_quotes)
      {
        strcat(out, DAT_00580e84);
      }
      if (locals.token[0] == '\"')
      {
        locals.in_quotes = 1;
      }
      if (locals.field_idx == field)
      {
        strcat(out, locals.token);
      }
      if (locals.token[strlen(locals.token) - 1] == '\"')
      {
        locals.in_quotes = 0;
      }
      if (locals.in_quotes)
      {
        --locals.field_idx;
      }
    }

  } while ((locals.fscanf_ret != -1) && ((locals.field_idx == '\0') || (locals.current_id == csvid)));

  fclose(locals.fp);
}

// FUNCTION: SHANDALAR 0x00417e11
int FUN_00417e11(char *param_1, int param_2, unsigned int param_3, int param_4)
{
  struct
  {
    unsigned int entry_index;            /* ebp-0x22c */
    unsigned int last_basic_entry_index; /* ebp-0x228 */
    int unused_224;                      /* ebp-0x224 */
    int card_index;                      /* ebp-0x220 */
    int csvid;                           /* ebp-0x21c */
    int numcards;                        /* ebp-0x218 */
    int header_line_index;               /* ebp-0x214 */
    char *space;                         /* ebp-0x210 */
    int total_cards;                     /* ebp-0x20c */
    int speed_filter;                    /* ebp-0x208 */
    unsigned int color_filter;           /* ebp-0x204 */
    char line[500];                      /* ebp-0x200 */
    int deck_type;                       /* ebp-0xc */
    FILE *f;                             /* ebp-0x8 */
    int scan_ret;                        /* ebp-0x4 */
  } s;

  strcpy(DAT_0097e340, param_1);
  s.f = fopen(param_1, DAT_00580e8c);
  if (s.f == (FILE *)0)
  {
    return 0;
  }

  s.deck_type = 0;
  s.header_line_index = s.deck_type;
  s.entry_index = (unsigned int)s.header_line_index;

  s.scan_ret = fscanf(s.f, s_______00580e90, s.line);
  s.scan_ret = fscanf(s.f, DAT_00580e98, s.line);

  s.color_filter = 0;
  s.speed_filter = -1;
  s.last_basic_entry_index = 0xffffffff;
  s.unused_224 = 0;
  s.total_cards = 0;
  s.numcards = -1;
  s.csvid = s.numcards;

  do
  {
    s.scan_ret = fscanf(s.f, s_______00580ea0, s.line);
    if (s.line[0] == '.')
    {
      if (s.line[1] != 'v')
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

          if (s.card_index != -1)
          {
            if (s.card_index <= 4)
            {
              s.last_basic_entry_index = s.entry_index;
            }

            if (((unsigned char)global_cards_data[s.card_index].type & 1) != 0)
            {
              s.unused_224 = s.unused_224 + (int)s.entry_index;
            }
          }
          else
          {
            if ((s.last_basic_entry_index != 0xffffffff) &&
                (*(int *)(param_2 + 4 + s.last_basic_entry_index * 8) > s.numcards))
            {
              *(int *)(param_2 + 4 + s.last_basic_entry_index * 8) =
                  *(int *)(param_2 + 4 + s.last_basic_entry_index * 8) -
                  (int)((s.entry_index & 1) + s.numcards) / 2;
            }
          }
        }
      }
      else
      {
        s.space = strchr(s.line, ' ');
        if (s.space != (char *)0)
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
  } while ((((int)s.entry_index < 200) && (s.total_cards < 500) && (s.scan_ret != -1)) &&
           ((s.csvid != 0) || (s.numcards != 0)));

  fclose(s.f);

  if (s.total_cards < 0x28)
  {
    s.deck_type = -3;
  }
  if (((s.total_cards >= 500) || ((int)s.entry_index >= 200)) && (s.scan_ret != -1))
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

  for (s.i = 0; s.i < g_card_count; s.i = s.i + 1)
  {
    if (((int)global_cards_data[s.i].id == s.chosen_csvid))
    {
      return s.i;
    }
  }
}

// FUNCTION: SHANDALAR 0x004f6886
void FUN_004f6886(int creature_type, int x_320, int y_200, int tinted, int mode)
{
  DIBSurface *dst_page;
  DIBSurface *src_page;
  int dst_right;
  int dst_bottom;
  EncodedImage *portrait_frame_sprite;
  char face_path[24];
  EncodedImage *face_sprite;

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

  DrawEncodedImageResampled(PTR_DAT_005832b4, x_320 - ScaleUiCoordinate((int)portrait_frame_sprite->width / 2), y_200,
                            ScaleUiCoordinate((int)portrait_frame_sprite->width),
                            ScaleUiCoordinate((int)portrait_frame_sprite->height), portrait_frame_sprite);

  strcpy(g_ui_message_buffer, FUN_00561441(creature_type));
  DrawFormattedTextNoShadowCentered(PTR_DAT_005832b4, (-(unsigned int)(tinted == 0) & 0x38) + 0xae, x_320,
                                    y_200 + ScaleUiCoordinate((int)portrait_frame_sprite->height / 2), g_ui_message_buffer);

  PTR_DAT_005832b4->font_slot = 1;
  if (mode == 1)
  {
    PTR_DAT_005832b4->page_number = 0;
  }

  StretchBlitGraphicsRect(PTR_DAT_005832dc, 1, 0xa5, 0xb4, 0xb4, PTR_DAT_005832dc, dst_right, dst_bottom, ScaleUiCoordinate(0xb4),
                          ScaleUiCoordinate(0xb4));

  sprintf(face_path, s_faces__03d_pic_0058b204, creature_type);
  LoadPcxResource(1, 0, global_screen_height - 0xf0, face_path, (void *)0);
  BeginSpriteEncodeSession();
  face_sprite = EncodeSpriteFromPage(1, 0, global_screen_height - 0xf0, 0x8a, 0xaa);
  FinalizeSpriteEncodeSession();

  DrawEncodedImageResampled(PTR_DAT_005832dc, dst_right + ScaleUiCoordinate(0x15), dst_bottom + ScaleUiCoordinate(5), ScaleUiCoordinate(0x8a),
                            ScaleUiCoordinate(0xaa), face_sprite);

  if (tinted != 0)
  {
    ApplyPortraitPaletteMap(PTR_DAT_005832dc, dst_right, dst_bottom, (unsigned int)ScaleUiCoordinate(0xb4), ScaleUiCoordinate(0xb4), s_prdblk_pic_0058b224,
                            s_advfac64_pic_0058b214);
  }

  if (mode == 0)
  {
    BlitGraphicsRect(PTR_DAT_005832dc, dst_right, dst_bottom, ScaleUiCoordinate(0xb4), ScaleUiCoordinate(0xb4), PTR_DAT_005832b4,
                     x_320 - ScaleUiCoordinate(0x5a), y_200 + ScaleUiCoordinate(0x2d));
  }
  else
  {
    src_page = g_graphics_pages[PTR_DAT_005832dc->page_number];
    dst_page = g_graphics_pages[PTR_DAT_005832b4->page_number];

    BlitGraphicsRect(PTR_DAT_005832dc, dst_right, dst_bottom, ScaleUiCoordinate(0xb4), ScaleUiCoordinate(0xb4), PTR_DAT_005832dc,
                     x_320 - ScaleUiCoordinate(0x5a), y_200 + ScaleUiCoordinate(0x2d));
    BlitRectByRandomTileOrderInPlace(dst_page->hTempDC, x_320 - ScaleUiCoordinate(0x5a), y_200 + ScaleUiCoordinate(0x2d), ScaleUiCoordinate(0xb4),
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

  for (deck_slot = 0; deck_slot < 0x1f4; deck_slot = deck_slot + 1)
  {
    if (((unsigned int)deck[deck_slot] & 0xfff) == card_id)
    {
      FUN_0056bfa4(deck_slot);
      break;
    }
  }
}

// FUNCTION: SHANDALAR 0x005224a0
void ShowPlayer1LibraryMenu(int unused)
{
  (void)unused;
  FUN_0056a515(unk_008b35ec, global_library[1], 500, gs_showlibrary_empty_0074bcf2, 0, &g_showlibrary_menu_selection);
}

// FUNCTION: SHANDALAR 0x0053114a
void WisemanChooseLairCreatureAndSetupDuel(unsigned char amulet_color)
{
  struct
  {
    int aiStack_50[16];
    int chosen_creature_type;
    int i;
    int count;
  } s;

  /* The original overwrites the param stack slot with a precomputed bitmask. */
  *(int *)&amulet_color = 1 << amulet_color;

  strcpy(g_ui_message_buffer, gs_lair_0077e180[0x11]);
  s.count = 0;

  for (s.i = 1; s.i < (int)DAT_00593934; s.i = s.i + 1)
  {
    if ((*(int *)&amulet_color & (int)(signed char)gs_creature_names_00591a08[s.i].plural[0x36]) != 0)
    {
      s.aiStack_50[s.count] = s.i;
      s.count = s.count + 1;
      strcat(g_ui_message_buffer, " ");
      strcat(g_ui_message_buffer, FUN_00561441(s.i));
      strcat(g_ui_message_buffer, ".\n");
    }
  }

  s.chosen_creature_type = RunTextMenuAt(g_ui_message_buffer, 100, 0x46);
  if (s.chosen_creature_type != -1)
  {
    s.chosen_creature_type = s.aiStack_50[s.chosen_creature_type];
    FUN_0050a5f1(s.chosen_creature_type, -1, 0, -1);
    for (s.i = 0; s.i < 500; s.i = s.i + 1)
    {
      global_library[1][s.i] = FUN_0052234e(DAT_0057a750);
    }
    ShowPlayer1LibraryMenu(DAT_0057a750);
  }
}

// FUNCTION: SHANDALAR 0x0053128c
int FUN_0053128c(char *text)
{
  struct
  {
    char *saved_text;
    int lines;
  } s;

  s.lines = 0;

  while ((int)(signed char)*text != 0)
  {
    s.saved_text = text;
    text = text + 1;
    if ((int)(signed char)*s.saved_text == 10)
    {
      s.lines = s.lines + 1;
    }
  }

  return s.lines;
}

// FUNCTION: SHANDALAR 0x005312d7
int FUN_005312d7(char *text_block, char **out_line_ptrs)
{
  int count;

  count = 1;
  out_line_ptrs[0] = text_block;

  while (*text_block != '\0')
  {
    if (*(unsigned int *)text_block == 0x0A0A0A0AU)
    {
      out_line_ptrs[count] = text_block + 4;
      count = count + 1;
    }
    text_block = text_block + 1;
  }

  out_line_ptrs[count] = text_block;
  return count;
}

// FUNCTION: SHANDALAR 0x0053133f
int FUN_0053133f(void)
{
  struct
  {
    int selection;       /* ebp-0xc */
    int wizard_index;    /* ebp-0x8 */
    int available_count; /* ebp-0x4 */
  } s;

  s.available_count = 0;

  for (s.wizard_index = 1; s.wizard_index <= 5; s.wizard_index = s.wizard_index + 1)
  {
    if ((g_defeated_wizards_bitmap & (1U << (unsigned char)s.wizard_index)) == 0)
    {
      s.available_count = s.available_count + 1;
    }
  }

  s.selection = FUN_00522508(s.available_count);

  for (s.wizard_index = 1; (s.wizard_index <= 5) && (s.selection != 0); s.wizard_index = s.wizard_index + 1)
  {
    if ((g_defeated_wizards_bitmap & (1U << (unsigned char)s.wizard_index)) == 0)
    {
      s.selection = s.selection - 1;
    }
  }

  return s.wizard_index;
}

// FUNCTION: SHANDALAR 0x005313e3
int ComputeWisemanCityBlockValue(int color_index)
{
  struct
  {
    int score;            /* ebp-0x1c */
    int color_bit;        /* ebp-0x18 */
    int duel_win_count;   /* ebp-0x14 */
    int tmp;              /* ebp-0x10: param copy / min clamp */
    int duel_log_index;   /* ebp-0x0c */
    int creature_group;   /* ebp-0x08 */
    int town_match_count; /* ebp-0x04 */
  } s;

  s.creature_group = color_index * 7;
  s.color_bit = 1 << (unsigned char)color_index;
  s.score = (int)(signed char)gs_creature_names_00591a08[s.creature_group].metadata[1];

  if (s.creature_group <= 0x24)
  {
    if ((s.creature_group % 7) != 0)
    {
      s.score = s.score + g_shandalar_difficulty * 2;
      goto after_difficulty_adjust;
    }
  }

  if (s.creature_group <= 0x24)
  {
    if ((s.creature_group % 7) == 0)
    {
      s.score = s.score + g_shandalar_difficulty * 5;
      goto after_difficulty_adjust;
    }
  }

  if (s.creature_group < 0x37)
  {
    s.score = s.score + g_shandalar_difficulty * 2;
    goto after_difficulty_adjust;
  }

  if (s.creature_group >= 0x37)
  {
    s.score = s.score + g_shandalar_difficulty * 50;
  }

after_difficulty_adjust:

  if ((int)(signed char)gs_creature_names_00591a08[s.creature_group].metadata[3] == 0xb)
  {
    for (s.duel_log_index = 0; s.duel_log_index < 10; s.duel_log_index = s.duel_log_index + 1)
    {
      if ((g_world_magic_bitmap & (1 << (unsigned char)s.duel_log_index)) != 0)
      {
        s.score = s.score + 1;
      }
    }
  }

  if ((int)(signed char)gs_creature_names_00591a08[s.creature_group].metadata[3] == 0xc)
  {
    s.score = s.score + 10;
    s.duel_win_count = 0;
    s.town_match_count = 0;

    for (s.duel_log_index = 0; s.duel_log_index < 1000; s.duel_log_index = s.duel_log_index + 1)
    {
      if ((int)(signed char)g_duel_victory_log[s.duel_log_index] == 0)
      {
        break;
      }
      if (((int)(signed char)g_duel_victory_log[s.duel_log_index] >> 4) == s.color_bit)
      {
        s.duel_win_count = s.duel_win_count + 1;
      }
    }

    s.score = s.score - s.duel_win_count;

    for (s.tmp = 0; s.tmp < 0x80; s.tmp = s.tmp + 1)
    {
      if (g_town_slots[s.tmp].status_and_ruling_wizard & 0xff00)
      {
        if (((g_town_slots[s.tmp].status_and_ruling_wizard >> 8) - 1) == s.duel_log_index)
        {
          s.town_match_count = s.town_match_count + 1;
        }
      }
    }

    s.score = s.score + s.town_match_count * g_shandalar_difficulty;

    s.score = MAX(s.score, g_shandalar_difficulty * 5 + 0x14);
  }

  return s.score;
}

// FUNCTION: SHANDALAR 0x00531616
int FUN_00531616(int param_1)
{
  struct
  {
    int town_index;       /* ebp-0x8 */
    int ruled_town_count; /* ebp-0x4 */
  } s;

  s.ruled_town_count = 0;
  param_1 = 1 << (unsigned char)param_1;

  for (s.town_index = 0; s.town_index < 0x80; s.town_index = s.town_index + 1)
  {
    if (g_town_slots[s.town_index].status_and_ruling_wizard & 0xff00)
    {
      if (((g_town_slots[s.town_index].status_and_ruling_wizard >> 8) - 1) == param_1)
      {
        s.ruled_town_count = s.ruled_town_count + 1;
      }
    }
  }

  return s.ruled_town_count;
}

// FUNCTION: SHANDALAR 0x00531698
int FillWisemanCityBlockTownNameSubstitutions(char *out_a, char *out_b, int choose_new_towns)
{
  int town_index;
  int active_town_index;

  if (choose_new_towns != 0)
  {
    g_wiseman_city_block_active_town_count = 0;
    for (town_index = 0; town_index < 0x80; town_index = town_index + 1)
    {
      if ((g_town_slots[town_index].status_and_ruling_wizard & 1) != 0)
      {
        g_wiseman_city_block_active_town_count = g_wiseman_city_block_active_town_count + 1;
      }
    }

    if (g_wiseman_city_block_active_town_count >= 2)
    {
      g_wiseman_city_block_town_choice_index_a = FUN_00522508(g_wiseman_city_block_active_town_count);
      g_wiseman_city_block_town_choice_index_b = FUN_00522508(g_wiseman_city_block_active_town_count);
      while (g_wiseman_city_block_town_choice_index_b == g_wiseman_city_block_town_choice_index_a)
      {
        g_wiseman_city_block_town_choice_index_b = FUN_00522508(g_wiseman_city_block_active_town_count);
      }
    }
    else
    {
      return -1;
    }
  }

  if (g_wiseman_city_block_active_town_count >= 2)
  {
    active_town_index = 0;
    for (town_index = 0; town_index < 0x80; town_index = town_index + 1)
    {
      if ((g_town_slots[town_index].status_and_ruling_wizard & 1) != 0)
      {
        if (active_town_index == g_wiseman_city_block_town_choice_index_a)
        {
          strcpy(out_a, BuildTownDisplayName(town_index));
          active_town_index = active_town_index + 1;
        }
        else if (active_town_index == g_wiseman_city_block_town_choice_index_b)
        {
          strcpy(out_b, BuildTownDisplayName(town_index));
          active_town_index = active_town_index + 1;
        }
        else
        {
          active_town_index = active_town_index + 1;
        }
      }
    }
    return 2;
  }
  else
  {
    return -1;
  }
}

// FUNCTION: SHANDALAR 0x0053182b
int FUN_0053182b(void)
{
  struct
  {
    int selection;       /* ebp-0xc */
    int slot_index;      /* ebp-0x8 */
    int open_slot_count; /* ebp-0x4 */
  } s;

  s.open_slot_count = 0;
  for (s.slot_index = 0; s.slot_index < 0xc; s.slot_index = s.slot_index + 1)
  {
    if ((g_world_magic_bitmap & (1U << (unsigned char)s.slot_index)) == 0)
    {
      s.open_slot_count = s.open_slot_count + 1;
    }
  }

  if (s.open_slot_count == 0)
  {
    return -1;
  }

  s.selection = FUN_00522508(s.open_slot_count);

  for (s.slot_index = 0; (s.slot_index < 0xc) && (s.selection != 0); s.slot_index = s.slot_index + 1)
  {
    if ((g_world_magic_bitmap & (1U << (unsigned char)s.slot_index)) == 0)
    {
      s.selection = s.selection - 1;
    }
  }

  return s.slot_index;
}

// FUNCTION: SHANDALAR 0x004f2232
int PickWisemanCastleDungeonSlotForHint(void)
{
  struct
  {
    int dungeon_index;      /* ebp-0x10 */
    int best_score;         /* ebp-0xc */
    int best_dungeon_index; /* ebp-0x8 */
    int score;              /* ebp-0x4: reused for distance and score */
  } s;

  s.best_score = 0x7fff;
  s.best_dungeon_index = -1;
  s.dungeon_index = 0;

  for (; s.dungeon_index < 0xf; s.dungeon_index = s.dungeon_index + 1)
  {
    if ((g_castle_dungeon_slots[s.dungeon_index].card_slot_1 == -1) ||
        (g_castle_dungeon_slots[s.dungeon_index].clues_bitmap == 7))
    {
      continue;
    }

    s.score = FUN_004ecf30(g_world_player_tile_x - g_castle_dungeon_slots[s.dungeon_index].world_x,
                           g_world_player_tile_y - g_castle_dungeon_slots[s.dungeon_index].world_y);

    s.score = FUN_00522508(s.score) + s.score / 2;

    if ((g_castle_dungeon_slots[s.dungeon_index].rules_bitmap & 0x200) != 0)
    {
      s.score = s.score * 3;
    }

    if (s.best_score > s.score)
    {
      s.best_dungeon_index = s.dungeon_index;
      s.best_score = s.score;
    }
  }

  return s.best_dungeon_index;
}

// FUNCTION: SHANDALAR 0x00508970
void FUN_00508970(int param_1)
{
  int clue_index;

  do
  {
    clue_index = FUN_00522508(3);
  } while ((g_castle_dungeon_slots[param_1].clues_bitmap & (1U << (unsigned char)clue_index)) != 0);

  g_castle_dungeon_slots[param_1].clues_bitmap |= 1 << (unsigned char)clue_index;
  FUN_0050caa0(param_1);
}

// FUNCTION: SHANDALAR 0x005318f5
unsigned int FUN_005318f5(int param_1, int param_2, int param_3)
{
  struct
  {
    char local_8c0[100];

    int local_85c;
    int local_858;
    int local_854;
    char *local_850[10];

    int local_828;
    char *local_824;
    int local_820;
    int local_81c;
    char *local_818[10];

    int local_7f0;
    char *local_7ec;
    int local_7e8;
    int local_7e4;
    int local_7e0;
    int local_7dc;
    int local_7d8;
    unsigned int local_7d4[500];
  } s;

  s.local_7e8 = 0;
  AnimateVisitBackdropZoomIn(s_wiseman3_pic_0058f068);

  if (param_2 == 0)
  {
    s.local_7e8 = 1;
    switch (DAT_00650158)
    {
    case 0:
      break;
    case 1:
      goto LAB_00531afb;
    case 2:
      goto LAB_00531ee7;
    }
  }

LAB_0053197c:
  while (1)
  {
  if (((FUN_00522508(5) == 0) || (s.local_7e8 != 0)) || (param_2 == 0))
  {
    if ((DAT_0058ee00 == -1) || (param_2 != 0))
    {
      DAT_0058ee00 = rand() % 0xc;
    }

    s.local_7ec = gs_advblock_general_0077c9e0[DAT_0058ee00];
    s.local_820 = FUN_005312d7(s.local_7ec, s.local_818) - 1;

    for (s.local_81c = 0; s.local_81c < s.local_820; s.local_81c = s.local_81c + 1)
    {
      s.local_7ec = s.local_818[s.local_81c];
      s.local_818[s.local_81c + 1][-3] = '\0';

      s.local_7f0 = FUN_0053128c(s.local_7ec);
      PTR_DAT_005832b4->font_slot = 1;

      DrawFormattedTextShadowed(PTR_DAT_005832b4, 0xc1, ScaleUiCoordinate(0x50),
                                ScaleUiCoordinate(0x13c) - GetFontLineHeight(PTR_DAT_005832b4->font_slot) * s.local_7f0, s.local_7ec);

      ClearInputAndWaitForMouseRelease();
      WaitForInputEventUnlessBlocked();
      s.local_818[s.local_81c + 1][-3] = '\n';
      LoadVisitBackdropAndCopyToPage0(s_wiseman3_pic_0058f078);
    }

    DAT_00650158 = 0;
    return 1;
  }

LAB_00531afb:
  if ((FUN_00522508(5) == 0) || (s.local_7e8 != 0))
  {
    s.local_85c = DAT_0058ee04;
    if ((DAT_0058ee04 == -1) || (param_2 != 0))
    {
      DAT_0058ee04 = rand() % 3;
      g_wiseman_city_block_color_index = FUN_0053133f();
      g_wiseman_city_block_world_magic_slot_index = FUN_0053182b();
      FillWisemanCityBlockTownNameSubstitutions(g_wiseman_city_block_subst_a, g_wiseman_city_block_subst_b, 1);
    }

    s.local_824 = gs_advblock_city_0074c930[DAT_0058ee04];
    if (DAT_0058ee04 == 0)
    {
      if (FUN_0056615f(g_wiseman_city_block_color_index))
      {
        s.local_824 = gs_advblock_city_0074c930[3];
      }
    }

    s.local_858 = FUN_005312d7(s.local_824, s.local_850) - 1;
    PTR_DAT_005832b4->font_slot = 1;

    for (s.local_854 = 0; s.local_854 < s.local_858; s.local_854 = s.local_854 + 1)
    {
      s.local_824 = s.local_850[s.local_854];
      s.local_850[s.local_854 + 1][-3] = '\0';

      s.local_828 = FUN_0053128c(s.local_824);

      if (s.local_854 != (s.local_858 - 1))
      {
        DrawFormattedTextShadowed(PTR_DAT_005832b4, 0xc1, ScaleUiCoordinate(0x50),
                                  ScaleUiCoordinate(0x13c) - GetFontLineHeight(PTR_DAT_005832b4->font_slot) * s.local_828, s.local_824);
      }
      else if (DAT_0058ee04 == 0)
      {
        FUN_00564e70(g_ui_message_buffer, 0x1000, s.local_824,
                     gs_wizardnames_0077ee70 + g_wiseman_city_block_color_index * 0x32,
                     ComputeWisemanCityBlockValue(g_wiseman_city_block_color_index),
                     FUN_00531616(g_wiseman_city_block_color_index));
        DrawFormattedTextShadowed(PTR_DAT_005832b4, 0xc1, ScaleUiCoordinate(0x50),
                                  ScaleUiCoordinate(0x13c) - GetFontLineHeight(PTR_DAT_005832b4->font_slot) * s.local_828, g_ui_message_buffer);
      }
      else if (DAT_0058ee04 == 1)
      {
        DrawFormattedTextShadowed(PTR_DAT_005832b4, 0xc1, ScaleUiCoordinate(0x50),
                                  ScaleUiCoordinate(0x13c) - GetFontLineHeight(PTR_DAT_005832b4->font_slot) * s.local_828, s.local_824, g_wiseman_city_block_subst_a,
                                  g_wiseman_city_block_subst_b);
      }
      else if (DAT_0058ee04 == 2)
      {
        if (g_wiseman_city_block_world_magic_slot_index == -1)
        {
          strcpy(g_wiseman_city_block_subst_a, DAT_0058f088);
        }
        else
        {
          strcpy(g_wiseman_city_block_subst_a, gs_worldmagic_names_00780660[g_wiseman_city_block_world_magic_slot_index]);
        }
        strcpy(g_wiseman_city_block_subst_b,
               BuildTownDisplayName(g_world_magic_slot_timers[g_wiseman_city_block_world_magic_slot_index].unk_00));
        FUN_00564e70(g_ui_message_buffer, 0x1000, s.local_824, g_wiseman_city_block_subst_a, g_wiseman_city_block_subst_b);
        DrawFormattedTextShadowed(PTR_DAT_005832b4, 0xc1, ScaleUiCoordinate(0x50),
                                  ScaleUiCoordinate(0x13c) - GetFontLineHeight(PTR_DAT_005832b4->font_slot) * s.local_828, g_ui_message_buffer);
      }

      ClearInputAndWaitForMouseRelease();
      WaitForInputEventUnlessBlocked();

      s.local_850[s.local_854 + 1][-3] = '\n';
      LoadVisitBackdropAndCopyToPage0(s_wiseman3_pic_0058f08c);
    }

    DAT_00650158 = 1;
    return 1;
  }

  DAT_00650158 = 2;

  /* Quest/status block + special actions. */
LAB_00531ee7:
  if ((FUN_00522508(5) < 2) || (s.local_7e8 != 0))
  {
    if (g_current_quest_destination != -1)
    {
      if ((g_current_quest_type == 0) || (g_current_quest_type == 2) ||
          ((g_current_quest_type == 1) &&
           (FindDeckSlotForQuestColorAndType((unsigned char)g_current_quest_color,
                                             (unsigned char)(1 << (g_current_quest_destination & 3))) != 0)))
      {
        g_current_quest_data =
            GetRelativeWorldQuadrant(g_town_slots[g_current_quest_destination].world_x,
                                     g_town_slots[g_current_quest_destination].world_y);

        if (g_current_quest_type != 0)
        {
          FUN_00564e70(g_ui_message_buffer, 0x1000, gs_wiseman_0074d840[0],
                       gs_amuletnames_0077d090 + g_current_quest_color * 0x19,
                       gs_directions_00765d50 + g_current_quest_data * 0x19,
                       BuildTownDisplayName(g_current_quest_destination));
        }
        else
        {
          FUN_00564e70(g_ui_message_buffer, 0x1000, gs_wiseman_0074d840[1],
                       gs_directions_00765d50 + g_current_quest_data * 0x19,
                       BuildTownDisplayName(g_current_quest_destination));
        }

        PTR_DAT_005832b4->font_slot = 1;
        DrawFormattedTextShadowed(PTR_DAT_005832b4, 0xc1, ScaleUiCoordinate(0x50),
                                  ScaleUiCoordinate(0x13c) - GetFontLineHeight(PTR_DAT_005832b4->font_slot) * 2, g_ui_message_buffer);
        ClearInputAndWaitForMouseRelease();
        WaitForInputEventUnlessBlocked();
        return 0;
      }

      if (g_current_quest_type == 1)
      {
        FUN_004f23bc(s.local_8c0, 100);
        FUN_00564e70(g_ui_message_buffer, 0x1000, gs_wiseman_0074d840[2],
                     BuildTownDisplayName(g_current_quest_destination), s.local_8c0);

        PTR_DAT_005832b4->font_slot = 1;
        DrawFormattedTextShadowed(PTR_DAT_005832b4, 0xc1, ScaleUiCoordinate(0x50),
                                  ScaleUiCoordinate(0x13c) - GetFontLineHeight(PTR_DAT_005832b4->font_slot) * 3, g_ui_message_buffer);
        ClearInputAndWaitForMouseRelease();
        WaitForInputEventUnlessBlocked();
        return 0;
      }

      if (g_current_quest_type < 0)
      {
        if (g_current_quest_type <= -100)
        {
          g_current_quest_data =
              GetRelativeWorldQuadrant(g_town_slots[g_current_quest_destination].world_x,
                                       g_town_slots[g_current_quest_destination].world_y);
          FUN_00564e70(g_ui_message_buffer, 0x1000, gs_wiseman_0074d840[3],
                       gs_directions_00765d50 + g_current_quest_data * 0x19,
                       BuildTownDisplayName(g_current_quest_destination));
        }
        else
        {
          s.local_824 = BuildCreatureNameWithArticle(-g_current_quest_type);
          FUN_00564e70(g_ui_message_buffer, 0x1000, gs_wiseman_0074d840[4],
                       s.local_824,
                       BuildTownDisplayName(g_current_quest_destination));
        }

        PTR_DAT_005832b4->font_slot = 1;
        DrawFormattedTextShadowed(PTR_DAT_005832b4, 0xc1, ScaleUiCoordinate(0x50),
                                  ScaleUiCoordinate(0x13c) - GetFontLineHeight(PTR_DAT_005832b4->font_slot) * 3, g_ui_message_buffer);
        ClearInputAndWaitForMouseRelease();
        WaitForInputEventUnlessBlocked();
        return 0;
      }
    }
    param_2 = 0;
  }

  DAT_00650158 = 0;
  strcpy(g_ui_message_buffer, gs_wiseman_0074d840[5]);

  switch ((g_town_slots[param_3].world_x % 2) + (g_town_slots[param_3].world_y % 3))
  {
  case 0:
    strcat(g_ui_message_buffer, gs_wiseman_0074d840[6]);
    PTR_DAT_005832b4->font_slot = 1;
    DrawFormattedTextShadowed(PTR_DAT_005832b4, 0xc1, ScaleUiCoordinate(0x50),
                              ScaleUiCoordinate(0x13c) - GetFontLineHeight(PTR_DAT_005832b4->font_slot) * 7, g_ui_message_buffer);
    ClearInputAndWaitForMouseRelease();
    WaitForInputEventUnlessBlocked();
    s.local_7e8 = 1;
    LoadVisitBackdropAndCopyToPage0(s_wiseman3_pic_0058f09c);
    continue;
    break;

  case 1:
    s.local_7d8 = PickWisemanCastleDungeonSlotForHint();
    if (s.local_7d8 != -1)
    {
      strcat(g_ui_message_buffer, gs_wiseman_0074d840[7]);
      PTR_DAT_005832b4->font_slot = 1;
      DrawFormattedTextShadowed(PTR_DAT_005832b4, 0xc1, ScaleUiCoordinate(0x50),
                                ScaleUiCoordinate(0x13c) - GetFontLineHeight(PTR_DAT_005832b4->font_slot) * 7, g_ui_message_buffer);
      ClearInputAndWaitForMouseRelease();
      WaitForInputEventUnlessBlocked();
      FUN_00508970(s.local_7d8);
      RefreshAdventureInterfaceLayout();
      goto LAB_005328e3;
    }
  case 2:
    if (g_next_duel_card_id == -1)
    {
      strcat(g_ui_message_buffer, gs_wiseman_0074d840[8]);
      g_next_duel_card_id = 2;
    }
    else
    {
      strcat(g_ui_message_buffer, gs_wiseman_0074d840[9]);
      g_food = g_food + 0x19;
    }
    PTR_DAT_005832b4->font_slot = 1;
    DrawFormattedTextShadowed(PTR_DAT_005832b4, 0xc1, ScaleUiCoordinate(0x50),
                              ScaleUiCoordinate(0x13c) - GetFontLineHeight(PTR_DAT_005832b4->font_slot) * 7, g_ui_message_buffer);
    ClearInputAndWaitForMouseRelease();
    WaitForInputEventUnlessBlocked();
    goto LAB_005328e3;

LAB_005324c3:
  case 3:
    sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), gs_wiseman_0074d840[10], gs_wiseman_0074d840[param_1 + 10]);
    PTR_DAT_005832b4->font_slot = 1;
    DrawFormattedTextShadowed(PTR_DAT_005832b4, 0xc1, ScaleUiCoordinate(0x50),
                              ScaleUiCoordinate(0x13c) - GetFontLineHeight(PTR_DAT_005832b4->font_slot) * 7, g_ui_message_buffer);
    ClearInputAndWaitForMouseRelease();
    WaitForInputEventUnlessBlocked();
    WisemanChooseLairCreatureAndSetupDuel((unsigned char)param_1);
    RefreshAdventureInterfaceLayout();
    goto LAB_005328e3;

  case 4:
    if (g_next_duel_card_id == -1)
    {
      g_next_duel_card_id = FUN_0056c705(DAT_0058edd8[(FUN_00522508(2) - 2) + param_1 * 2]);
      sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), gs_wiseman_0074d840[0x10], global_cards_data[g_next_duel_card_id].name);
    }
    else
    {
      goto LAB_005324c3;
    }
    PTR_DAT_005832b4->font_slot = 1;
    DrawFormattedTextShadowed(PTR_DAT_005832b4, 0xc1, ScaleUiCoordinate(0x50),
                              ScaleUiCoordinate(0x13c) - GetFontLineHeight(PTR_DAT_005832b4->font_slot) * 7, g_ui_message_buffer);
    ClearInputAndWaitForMouseRelease();
    WaitForInputEventUnlessBlocked();
    goto LAB_005328e3;
  default:
    sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), gs_wiseman_0074d840[0x11], gs_amuletnames_plural_0077edd0 + param_1 * 0x19);
    PTR_DAT_005832b4->font_slot = 1;
    DrawFormattedTextShadowed(PTR_DAT_005832b4, 0xc1, ScaleUiCoordinate(0x50),
                              ScaleUiCoordinate(0x13c) - GetFontLineHeight(PTR_DAT_005832b4->font_slot) * 7, g_ui_message_buffer);

    ClearInputAndWaitForMouseRelease();
    WaitForInputEventUnlessBlocked();

    DAT_0078990c[param_1] = DAT_0078990c[param_1] - 3;
    for (s.local_7dc = 0; s.local_7dc < 500; s.local_7dc = s.local_7dc + 1)
    {
      s.local_7d4[s.local_7dc] = deck[s.local_7dc];
      if (s.local_7d4[s.local_7dc] != 0xffffffff)
      {
        s.local_7d4[s.local_7dc] = s.local_7d4[s.local_7dc] & 0xfff;
      }
    }

    s.local_7e4 = FUN_0056a515(unk_008b35ec, (int *)s.local_7d4, 500, gs_wiseman_0074d840[0x12], 1, &DAT_0058f0ac);
    if ((s.local_7e4 != -1) &&
        ((s.local_7e0 = FUN_0056bd9d(s.local_7d4[s.local_7e4])) != -1))
    {
      deck[s.local_7e0] = deck[s.local_7e0] | 0x4000;
    }
    RefreshAdventureInterfaceLayout();
    goto LAB_005328e3;
  }
  }

LAB_005328e3:
  ;
}

// FUNCTION: SHANDALAR 0x005328e4
int FUN_005328e4(AdvMenuControl *control, int mode)
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
    DrawEncodedImageResampled(PTR_DAT_005832dc, control->x, control->y, control->width, control->height, DAT_00746b10[mode]);
    BlitGraphicsRect(PTR_DAT_005832dc, control->x, control->y, control->width, control->height, PTR_DAT_005832b4, control->x, control->y);
  }
  else
  {
    DrawEncodedImageResampled(PTR_DAT_005832b4, control->x, control->y, control->width, control->height, DAT_00746b10[mode]);
  }

  if (mode == 2 && control->on_activate != (AdvMenuActivateCallback)0)
  {
    (void)control->on_activate(control);
  }

  return 1;
}

// FUNCTION: SHANDALAR 0x00532a7c
int FUN_00532a7c(AdvMenuControl *control)
{
  PlaySoundEffectOnChannel(s_x_sound_button2_wav_0058f124, 0xf, 100, 100, 0);
  DAT_00650268 = control->selection_value;
  return 0;
}

// FUNCTION: SHANDALAR 0x00532aae
int FUN_00532aae(const int *a, const int *b)
{
  return strcoll(global_cards_data[*a].name, global_cards_data[*b].name);
}

// FUNCTION: SHANDALAR 0x00532aeb
unsigned int FUN_00532aeb(char *title, unsigned int color_mask, unsigned int type_mask, int param_4, int param_5)
{
  struct
  {
    int tmp_620;                         /* [ebp-0x620] */
    int tmp_61c;                         /* [ebp-0x61c] */
    int tmp_618;                         /* [ebp-0x618] */
    int tmp_614;                         /* [ebp-0x614] */
    int tmp_610;                         /* [ebp-0x610] */
    int tmp_60c;                         /* [ebp-0x60c] */
    unsigned int filtered_count;         /* [ebp-0x608] */
    int sorted_i;                        /* [ebp-0x604] */
    unsigned int filtered_card_ids[300]; /* [ebp-0x600] */

    /* [0..2] button colors, [3] menu context */
    int button_colors[4]; /* [ebp-0x154] */

    int owned_copies;        /* [ebp-0x144] */
    int special_clamp;       /* [ebp-0x140] */
    int type_filter_index;   /* [ebp-0x13c] */
    int tmp_i;               /* [ebp-0x138] */
    int loop_i;              /* [ebp-0x134] */
    int y;                   /* [ebp-0x130] */
    unsigned int card_index; /* [ebp-0x12c] */
    int x;                   /* [ebp-0x128] */

    unsigned int draw_index_to_card[64]; /* [ebp-0x124] */

    int draw_count;               /* [ebp-0x24] */
    int max_lines_per_col;        /* [ebp-0x20] */
    int filter_top_y;             /* [ebp-0x1c] */
    int filter_line_h;            /* [ebp-0x18] */
    unsigned int card_color_mask; /* [ebp-0x14] */
    int needs_redraw;             /* [ebp-0xc] */
    int list_top_y;               /* [ebp-0x8] */
    int list_line_h;              /* [ebp-0x4] */
  } s;

  s.needs_redraw = 1;

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

    for (s.loop_i = 0; s.loop_i < 3; s.loop_i = s.loop_i + 1)
    {
      SetFontStyleSize(7, (unsigned int)(((-(unsigned int)(s.loop_i == 2)) & 0xfffffffeU) + 0xb));
      DrawFormattedTextNoShadowCentered(PTR_DAT_005832dc, s.button_colors[s.loop_i], s.loop_i * 0x62 + 0x32, 0xd, "%s",
                                        (char *)g_done_text_table_entry);
      DAT_00746b10[s.loop_i] = EncodeSpriteFromPage(1, s.loop_i * 0x62 + 1, 1, 0x61, 0x1b);
    }

    FinalizeSpriteEncodeSession();

    if (DAT_0058f0c0.x == DAT_0058f0c0.base_x)
    {
      DAT_0058f0c0.x = ScaleUiCoordinate(DAT_0058f0c0.x);
      DAT_0058f0c0.y = ScaleUiCoordinate(DAT_0058f0c0.y);
      DAT_0058f0c0.width = ScaleUiCoordinate(DAT_0058f0c0.width);
      DAT_0058f0c0.height = ScaleUiCoordinate(DAT_0058f0c0.height);
    }

    s.button_colors[3] = BeginMenuContext();
    (void)ResetMenuContext(s.button_colors[3]);
    (void)AddMenuControlsToContext(&DAT_0058f0c0, 1, s.button_colors[3]);
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
    type_mask = 1U << (unsigned char)type_mask;
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
      CopyGraphicsRect(PTR_DAT_005832dc, ScaleUiCoordinate(0xd), ScaleUiCoordinate(0x8c), ScaleUiCoordinate(0xb3), ScaleUiCoordinate(0x132),
                       PTR_DAT_005832b4, ScaleUiCoordinate(0xd), ScaleUiCoordinate(0x8c));
      CopyGraphicsRect(PTR_DAT_005832dc, ScaleUiCoordinate(0xd), ScaleUiCoordinate(0x8c), ScaleUiCoordinate(0xb3), ScaleUiCoordinate(0x132),
                       PTR_DAT_005832b4, ScaleUiCoordinate(0x1c0), ScaleUiCoordinate(0x8c));
      CopyGraphicsRect(PTR_DAT_005832dc, ScaleUiCoordinate(0xb4), ScaleUiCoordinate(0x1bf), ScaleUiCoordinate(0x118), ScaleUiCoordinate(0x1d),
                       PTR_DAT_005832b4, ScaleUiCoordinate(0xb4), ScaleUiCoordinate(0x1bf));
      CopyGraphicsRect(PTR_DAT_005832dc, ScaleUiCoordinate(0xe9), ScaleUiCoordinate(0x158), ScaleUiCoordinate(0xd4), ScaleUiCoordinate(0x68),
                       PTR_DAT_005832b4, ScaleUiCoordinate(0xe9), ScaleUiCoordinate(0x158));
      CopyGraphicsRect(PTR_DAT_005832dc, ScaleUiCoordinate(0x1db), ScaleUiCoordinate(0x10), ScaleUiCoordinate(0x7c), ScaleUiCoordinate(0x5e),
                       PTR_DAT_005832b4, ScaleUiCoordinate(0x1db), ScaleUiCoordinate(0x10));
      CopyGraphicsRect(PTR_DAT_005832dc, ScaleUiCoordinate(0xe4), ScaleUiCoordinate(0x27), ScaleUiCoordinate(0xb8), ScaleUiCoordinate(0x19),
                       PTR_DAT_005832b4, ScaleUiCoordinate(0xe4), ScaleUiCoordinate(0x27));
    }
    s.needs_redraw = 0;
    CopyGraphicsRect(PTR_DAT_005832dc, ScaleUiCoordinate(0xe8), ScaleUiCoordinate(0x157), ScaleUiCoordinate(0xd2), ScaleUiCoordinate(0x66),
                     PTR_DAT_005832b4, ScaleUiCoordinate(0xe8), ScaleUiCoordinate(0x157));
    PTR_DAT_005832b4->font_slot = 4;
    DrawTextAt(PTR_DAT_005832b4, 0x16, 0x140, 0x34, title);
    PTR_DAT_005832b4->font_slot = 4;

      /* Draw color filter list (left). */
      s.x = FUN_005501dc(0x6b) / 2;
      s.y = FUN_005501dc(0x45) / 2;
      s.y = s.y - ((GetFontLineHeight(PTR_DAT_005832b4->font_slot) * 6) / 2);

      for (s.card_index = 0; (int)s.card_index < 6; s.card_index = s.card_index + 1)
      {
        if (s.card_index == 0)
        {
          if (_DAT_0058f0b0 == (int)s.card_index)
          {
            s.tmp_60c = 0x1b;
          }
          else if (color_mask != 0)
          {
            s.tmp_60c = 0;
          }
          else
          {
            s.tmp_60c = 0x62;
          }

          DrawCenteredTextLineWithShadow(gs_browse_0074da20[0], s.x, s.y, s.tmp_60c);
        }
        else
        {
          if (_DAT_0058f0b0 == (int)s.card_index)
          {
            s.tmp_610 = 0x1b;
          }
          else if (color_mask != 0)
          {
            s.tmp_610 = 0;
          }
          else
          {
            s.tmp_610 = 0x62;
          }

          DrawCenteredTextLineWithShadow(gs_browse_0074da20[(int)s.card_index], s.x, s.y, s.tmp_610);
        }
        s.y = s.y + GetFontLineHeight(PTR_DAT_005832b4->font_slot);
      }

      /* Draw type/rarity filter list (right). */
      s.x = FUN_005501dc(0x215) / 2;
      s.y = FUN_005501dc(0x45) / 2;
      s.y = s.y - ((GetFontLineHeight(PTR_DAT_005832b4->font_slot) * 6) / 2);
      s.filter_top_y = s.y;
      s.filter_line_h = GetFontLineHeight(PTR_DAT_005832b4->font_slot);

      for (s.card_index = 0; (int)s.card_index < 6; s.card_index = s.card_index + 1)
      {
        if ((_DAT_0058f0b0 == 0) && ((int)s.card_index >= 2))
        {
          if (DAT_0058f0b4 == (int)s.card_index)
          {
            s.tmp_61c = 0x1b;
          }
          else
          {
            s.tmp_61c = 0x62;
          }

          DrawCenteredTextLineWithShadow(gs_browse_0074da20[11 + (int)s.card_index], s.x, s.y, s.tmp_61c);
        }
        else
        {
          if (DAT_0058f0b4 == (int)s.card_index)
          {
            s.tmp_618 = 0x1b;
          }
          else
          {
            s.tmp_618 = 0x62;
          }

          DrawCenteredTextLineWithShadow(gs_browse_0074da20[6 + (int)s.card_index], s.x, s.y, s.tmp_618);
        }

        s.y = s.y + GetFontLineHeight(PTR_DAT_005832b4->font_slot);
      }

      /* Type filter index used for type-bit selection. */
      s.type_filter_index = DAT_0058f0b4;
      if ((_DAT_0058f0b0 == 0) && (DAT_0058f0b4 >= 2))
      {
        s.special_clamp = DAT_0058f0b4 - 1;
        s.type_filter_index = 6;
      }

      PTR_DAT_005832b4->font_slot = 1;
      ((void(__cdecl *)(FacemakerWindowBounds *, int, int, int, char *, ...))DrawTextAt)(PTR_DAT_005832b4, 0xff, 0xfe, 0x1ce, "%d", Gold);
      for (s.x = 0; s.x < 5; s.x = s.x + 1)
      {
        ((void(__cdecl *)(FacemakerWindowBounds *, int, int, int, char *, ...))DrawTextAt)(
            PTR_DAT_005832b4, 0xff, ((((s.x << 3) - s.x) * 5) + 0x131), 0x1ce, "%d", g_amulet_inventory[s.x]);
      }

      /* Filter list hit-test geometry is stored in s.filter_top_y / s.filter_line_h. */

      /* Card list geometry. */
      s.max_lines_per_col = (FUN_005501dc(0x11e) / 2) / GetFontLineHeight(PTR_DAT_005832b4->font_slot);
      s.draw_count = 0;
      s.x = FUN_005501dc(0x66) / 2;
      s.y = FUN_005501dc(0x99) / 2;
      s.list_top_y = s.y;
      s.list_line_h = GetFontLineHeight(PTR_DAT_005832b4->font_slot);

      /* Build filtered list. */
      s.filtered_count = 0;
      for (s.card_index = 0; s.card_index < (unsigned int)(g_card_count - 0x39); s.card_index = s.card_index + 1)
      {
        if ((_DAT_0058f0b0 == 0) && (s.type_filter_index == 6))
        {
          if (ClampIntToRange((int)(signed char)global_cards_data[s.card_index].rarity, 1, 3) != s.special_clamp)
          {
            continue;
          }
        }

        s.card_color_mask = (unsigned int)(signed char)global_cards_data[s.card_index].color;
        if (s.card_color_mask == 0)
        {
          s.card_color_mask = 1;
        }

        if ((s.card_color_mask & (1U << (unsigned char)_DAT_0058f0b0)) == 0)
        {
          continue;
        }

        if (((unsigned int)global_cards_data[s.card_index].type & (1U << (unsigned char)s.type_filter_index)) == 0)
        {
          continue;
        }

        if (((global_cards_data[s.card_index].extra_ability & 0x900) != 0) && (DAT_007894f4 == 0))
        {
          continue;
        }

        if (FUN_00428b05(s.card_index) == 0)
        {
          continue;
        }

        if ((color_mask != 0) && ((s.card_color_mask & color_mask) == 0))
        {
          continue;
        }

        if ((type_mask != 0) && (((unsigned int)global_cards_data[s.card_index].type & type_mask) == 0))
        {
          continue;
        }

        if (((unsigned int)global_cards_data[s.card_index].type == 'B') && (s.type_filter_index == 6))
        {
          continue;
        }

        s.filtered_card_ids[s.filtered_count] = (unsigned int)s.card_index;
        s.filtered_count = s.filtered_count + 1;
      }

      qsort(s.filtered_card_ids, s.filtered_count, 4, FUN_00532aae);

      for (s.sorted_i = 0; s.sorted_i < (int)s.filtered_count; s.sorted_i = s.sorted_i + 1)
      {
        s.card_index = s.filtered_card_ids[s.sorted_i];

        /* Count copies owned in the 500-slot deck array. */
        s.owned_copies = 0;
        for (s.loop_i = 0; s.loop_i < 500; s.loop_i = s.loop_i + 1)
        {
          if (((unsigned int)deck[s.loop_i] & 0xfffU) == (unsigned int)s.card_index)
          {
            s.owned_copies = s.owned_copies + 1;
          }
        }

        strcpy(g_ui_message_buffer, global_cards_data[s.card_index].name);
        if (s.owned_copies != 0)
        {
          strcat(g_ui_message_buffer, DAT_0058f164);
          strcat(g_ui_message_buffer, _itoa(s.owned_copies, unk_009266d0 + 0x70, 10));
        }

        if (DAT_0058f0b8 == 0xffffffff)
        {
          DAT_0058f0b8 = (unsigned int)s.card_index;
        }

        if (DAT_0058f0b8 == (unsigned int)s.card_index)
        {
          s.tmp_620 = 0x1b;
        }
        else
        {
          if (FUN_004bb1cf((unsigned int)s.card_index) > 0)
          {
            s.tmp_620 = 0x54;
          }
          else
          {
            s.tmp_620 = 3;
          }
        }

        DrawCenteredTextLineWithShadow(g_ui_message_buffer, s.x, s.y, s.tmp_620);
        s.draw_index_to_card[s.draw_count] = (unsigned int)s.card_index;
        s.draw_count = s.draw_count + 1;
        s.y = s.y + GetFontLineHeight(PTR_DAT_005832b4->font_slot);

        if (s.draw_count == s.max_lines_per_col)
        {
          s.x = FUN_005501dc(0x219) / 2;
          s.y = FUN_005501dc(0x99) / 2;
        }
      }

      if (DAT_0058f0b8 != 0xffffffff)
      {
        DAT_0058f0bc = DAT_0058f0b8;
        FUN_004f263b((int)DAT_0058f0bc, 0x7b, 0x33, 0x4b, 0x70, 1, DAT_0058f168);
      }

      FUN_0046ed03();
      DAT_00650268 = -1;

    while (1)
    {
      UpdateMouseSnapshot();
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

    s.x = g_mouse_x_snapshot;
    s.y = g_mouse_y_snapshot;
    s.tmp_i = g_mouse_button_down_mask;
    ClearInputAndWaitForMouseRelease();
    FUN_0046ed33();

    /* OK click region returns the current selected card. */
    if (((FUN_005501dc(0x66) / 2 < s.y) && (s.y < FUN_005501dc(0x146) / 2)) &&
        ((FUN_005501dc(0xf6) / 2 < s.x) && (s.x < FUN_005501dc(0x18c) / 2)))
    {
      if (param_5 != 0)
      {
        (void)EndMenuContext();
        FreeSpriteBlob(DAT_00746b10);
      }
      DestroyCachedCardArt();
      return DAT_0058f0bc;
    }

    if (s.tmp_i == 0)
    {
      if (param_5 != 0)
      {
        (void)EndMenuContext();
      }
      FreeSpriteBlob(DAT_00746b10);
      DestroyCachedCardArt();
      return 0xffffffff;
    }

    if (s.y < FUN_005501dc(0x6d) / 2)
    {
      /* Filter list selection: index = (mouse_y - filter_top_y) / line_h. */
      s.tmp_i = (s.y - s.filter_top_y) / s.filter_line_h;

      if (s.x < ScaleUiCoordinate(0xac))
      {
        if ((s.tmp_i >= 0) && (s.tmp_i < 6) && ((color_mask == 0) || ((1U << (unsigned char)s.tmp_i) == color_mask)))
        {
          _DAT_0058f0b0 = s.tmp_i;
          DAT_0058f0b8 = 0xffffffff;
          s.needs_redraw = 1;
        }
      }
      else
      {
        if ((ScaleUiCoordinate(0x1d6) < s.x) && (s.tmp_i >= 0) && (s.tmp_i < 6))
        {
          DAT_0058f0b4 = s.tmp_i;
          DAT_0058f0b8 = 0xffffffff;
          s.needs_redraw = 1;
        }
      }
    }
    else
    {
      if ((FUN_005501dc(0x96) / 2) < s.y)
      {
        /* Card list selection. */
        s.tmp_i = (s.y - s.list_top_y) / s.list_line_h;
        s.tmp_i = s.tmp_i + ((s.x / (global_screen_width / 2)) * s.max_lines_per_col);
        if (s.tmp_i < s.draw_count)
        {
          DAT_0058f0b8 = s.draw_index_to_card[s.tmp_i];
        }
      }
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
int FUN_00530b90(int param_1, int param_2)
{
  struct
  {
    int tile_magic_index;               // ebp - 0x30
    unsigned int card_id;               // ebp - 0x2c
    int(__cdecl *saved_callback)(void); // ebp - 0x28
    int required;                       // ebp - 0x24
    int total_mana;                     // ebp - 0x20
    int can_afford;                     // ebp - 0x1c
    int menu_result;                    // ebp - 0x18
    int random_color;                   // ebp - 0x14
    int first_time;                     // ebp - 0x10
    int i;                              // ebp - 0x0c
    int copies;                         // ebp - 0x08
    int price;                          // ebp - 0x04
  } s;

  s.first_time = 1;
  s.saved_callback = DAT_00580da4;
  DAT_00580da4 = FUN_00530b7e;

  while (1)
  {
    if (param_2 != -1)
    {
      s.card_id = FUN_00532aeb(gs_buyanycard_0074ccd0[5], g_town_slots[param_2].trade_color_and_type & 0xff,
                               (g_town_slots[param_2].trade_color_and_type >> 8) - 1, s.first_time, 1);
    }
    else
    {
      s.card_id = FUN_00532aeb(gs_buyanycard_0074ccd0[5], 0, 0xffffffff, s.first_time, 1);
    }
    s.first_time = 0;

    if (param_1 == 0)
    {
      s.total_mana = 0;
      for (s.i = 0; s.i < 5; s.i = s.i + 1)
      {
        s.total_mana = s.total_mana + g_amulet_inventory[s.i];
      }
    }

    if (s.card_id == 0xffffffff)
    {
      goto cleanup;
    }

    if (param_1 == -2)
    {
      s.can_afford = 1;
      if ((int)s.card_id > 4)
      {
        s.copies = 1;
      }
      else
      {
        s.copies = 4;
      }
    }
    else
    {
      s.required = FUN_0056c5ea(s.card_id);

      switch (s.required)
      {
      case 1:
        if ((int)s.card_id > 4)
        {
          s.copies = 1;
        }
        else
        {
          s.copies = 4;
        }
        break;
      case 2:
      default:
        s.copies = 1;
        break;
      }

      if ((s.required < 2) && ((global_cards_data[s.card_id].extra_ability & 0x400) != 0))
      {
        s.required = s.required + 1;
      }

      s.price = FUN_004f1cce(s.card_id);
      s.price = s.price << 2;

      s.tile_magic_index = FUN_0040dffd((int)FUN_005611c8(FUN_0043146b(g_world_player_x / 32, g_world_player_y / 32)));

      if (((s.tile_magic_index & (int)(signed char)global_cards_data[s.card_id].color) == 0) &&
          ((signed char)global_cards_data[s.card_id].color != 0))
      {
        if (FUN_0056c0e5(s.tile_magic_index, (int)(signed char)global_cards_data[s.card_id].color, 3) != 0)
        {
          s.price = (s.price * 3) / 2;
        }
        else
        {
          s.price = s.price << 1;
        }
      }

      /* numerator = price * 0x4e2 (1250) via *5 chain, preserved across the RNG call */
      s.tile_magic_index = s.price;
      s.tile_magic_index = s.tile_magic_index + s.tile_magic_index * 4;
      s.tile_magic_index = s.tile_magic_index + s.tile_magic_index * 4;
      s.tile_magic_index = s.tile_magic_index + s.tile_magic_index * 4;
      s.tile_magic_index = s.tile_magic_index + s.tile_magic_index * 4;
      s.tile_magic_index = s.tile_magic_index + s.tile_magic_index;

      s.random_color = FUN_00522508(((g_world_player_y & 0x1f) * (g_world_player_x & 0x1f)) / 10);
      s.price = s.tile_magic_index / (s.random_color + 0x2ee);

      s.price = ClampIntToRange((s.price / 0x32) * 5, 5, 1000);

      s.price = s.copies * s.price;

      /* required*0x32, then max(required*0x32, copies*price). */
      s.tile_magic_index = s.required;
      s.tile_magic_index = s.tile_magic_index + s.tile_magic_index * 4;
      s.tile_magic_index = s.tile_magic_index + s.tile_magic_index * 4;
      s.tile_magic_index = s.tile_magic_index + s.tile_magic_index;
      if (s.tile_magic_index > s.price)
      {
        s.price = s.tile_magic_index;
      }

      if (param_1 >= 0)
      {
        if (s.required == 1)
        {
          FUN_00564e70(g_ui_message_buffer, 0x1000, gs_buyanycard_0074ccd0[0], s.copies, global_cards_data[s.card_id].name,
                       gs_amuletnames_0077d090 + param_1 * 0x19);
        }
        else
        {
          FUN_00564e70(g_ui_message_buffer, 0x1000, gs_buyanycard_0074ccd0[1], s.copies, global_cards_data[s.card_id].name, s.required,
                       gs_amuletnames_plural_0077edd0 + param_1 * 0x19);
        }
      }
      else
      {
        FUN_00564e70(g_ui_message_buffer, 0x1000, gs_buyanycard_0074ccd0[2], s.copies, global_cards_data[s.card_id].name, s.price);
      }

      s.can_afford = 1;
      if ((0 < param_1) && (DAT_0078990c[param_1] < s.required))
      {
        s.can_afford = 0;
      }
      if ((param_1 == 0) && (s.total_mana < s.required))
      {
        s.can_afford = 0;
      }
      if (param_1 == -1)
      {
        if (Gold >= s.price)
        {
        }
        else
        {
          s.can_afford = 0;
        }
      }

      if (s.can_afford != 0)
      {
        (void)FUN_0057e826(g_ui_message_buffer, gs_buyanycard_0074ccd0[3]);
      }
      else
      {
        (void)FUN_0057e826(g_ui_message_buffer, gs_buyanycard_0074ccd0[4]);
      }

      s.menu_result = RunTextMenuAt(g_ui_message_buffer, ScaleUiCoordinate(0xea), ScaleUiCoordinate(0x160));
      if ((s.menu_result != 0) || (s.can_afford == 0))
      {
        continue;
      }

      s.can_afford = 0;
      if ((0 < param_1) && (s.required <= DAT_0078990c[param_1]))
      {
        s.can_afford = 1;
        DAT_0078990c[param_1] = DAT_0078990c[param_1] - s.required;
      }
      if ((param_1 == 0) && (s.required <= s.total_mana))
      {
        s.can_afford = 1;
        do
        {
          do
          {
            s.random_color = FUN_00522508(5);
          } while (g_amulet_inventory[s.random_color] == 0);
          s.required = s.required - 1;
          g_amulet_inventory[s.random_color] = g_amulet_inventory[s.random_color] - 1;
        } while (s.required != 0);
      }
      if (param_1 == -1)
      {
        if (Gold >= s.price)
        {
          s.can_afford = 1;
          Gold = Gold - s.price;
        }
      }

      if (s.can_afford == 0)
      {
        goto cleanup;
      }
    }

    s.i = 0;
    while (1)
    {
      if (s.copies <= s.i)
      {
        break;
      }
      s.random_color = FUN_0056bd9d(s.card_id);
      if (s.random_color != -1)
      {
        deck[s.random_color] = deck[s.random_color] | 0x4000;
      }
      s.i = s.i + 1;
    }

    (void)RecountDeckCardTotals();
    DestroyCachedCardArt();
  }

cleanup:
  DestroyCachedCardArt();
  DAT_00580da4 = s.saved_callback;
  return 0;
}

// FUNCTION: SHANDALAR 0x004eda50
int VisitTownSlot(int town_index)
{
  struct
  {
    unsigned int journal_param; // ebp - 0x94
    int world_quadrant;         // ebp - 0x90
    int tile_color_index;       // ebp - 0x8c
    int menu_result;            // ebp - 0x88
    size_t msg_len;             // ebp - 0x84
    int amulet_name;            // ebp - 0x80
    EncodedImage *format;       // ebp - 0x7c

    int unk1; // ebp - 0x78
    int unk2; // ebp - 0x74

    char *pics2[5];        // ebp - 0x70
    char *pics[5];         // ebp - 0x58
    int world_y_something; // ebp - 0x48
    int world_x_something; // ebp - 0x44

    int duel_deck_seed;     // ebp - 0x40
    int text_x;             // ebp - 0x3c
    int duel_wizard_color;  // ebp - 0x38
    EncodedImage *sprite;   // ebp - 0x34
    int temp_dword;         // ebp - 0x30
    int found_deck_slot;    // ebp - 0x2c
    int duel_ante_slot;     // ebp - 0x28
    int menu_x_320;         // ebp - 0x24
    int local_dx;           // ebp - 0x20
    int deck_card_index;    // ebp - 0x1c
    int duel_creature_type; // ebp - 0x18
    int sprite_draw_w;      // ebp - 0x14
    int sprite_draw_h;      // ebp - 0x10
    int distance;           // ebp - 0xc
    int quest_handled;      // ebp - 0x8
    int duel_menu_result;   // ebp - 0x4
  } s;

  g_current_town_slot_index = town_index;
  ClearInputAndWaitForMouseRelease();

  // NOTE: byte 1 of status_and_ruling_wizard is treated as the "ruling wizard" byte by the original code.
  if (g_town_slots[town_index].status_and_ruling_wizard & 0xff00)
  {
    // Duel path: wizard-controlled towns/dungeons.
    AnimateVisitBackdropZoomIn(GET_TOWN_PIC(town_index));
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
      s.sprite_draw_w = 4;
      break;
    case 1:
      s.sprite_draw_w = 6;
      break;
    case 2:
      s.sprite_draw_w = 8;
      break;
    case 3:
      s.sprite_draw_w = 0xc;
      break;
    case 4:
      s.sprite_draw_w = 0x10;
      break;
    default:
      if ((g_town_slots[town_index].world_y & 1) != 0)
      {
        s.sprite_draw_w = 0x12;
      }
      else
      {
        s.sprite_draw_w = 0xe;
      }
      break;
    }

    s.sprite_draw_w = FUN_0056139e(s.duel_wizard_color, s.sprite_draw_w);
    FUN_0050a5f1(s.sprite_draw_w, 0xffffffff, 0, -1);

    do
    {
      do
      {
        DAT_008e2e50[0] = FUN_0052234e(DAT_0057a750);
      } while (DAT_008e2e50[0] <= 4);
    } while ((global_cards_data[DAT_008e2e50[0]].extra_ability & 0x100) != 0);

    FUN_004f2407((int)DAT_008e2e50[0], 0xe0, 0x40, 1, gs_visit_0077c4f0[1]);
    FUN_004f6886(s.sprite_draw_w, 0xa0, 0x20, 1, 2);

    if (FUN_0056615f(s.duel_wizard_color) != 0)
    {
      sprintf(g_ui_message_buffer, gs_visit_0077c4f0[2], gs_wizardnames_0077ee70 + s.duel_wizard_color * 0x32);
    }
    else
    {
      sprintf(g_ui_message_buffer, gs_visit_0077c4f0[3], gs_wizardnames_0077ee70 + s.duel_wizard_color * 0x32);
    }

    if (FUN_005661d9(s.sprite_draw_w) != 0)
    {
      sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), gs_visit_0077c4f0[4], FUN_00561441(s.sprite_draw_w));
      strcat(g_ui_message_buffer, gs_visit_0077c4f0[6]);
    }
    else
    {
      sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), gs_visit_0077c4f0[5], FUN_00561441(s.sprite_draw_w));
      strcat(g_ui_message_buffer, gs_visit_0077c4f0[7]);
    }

    if (RunTextMenuAtScaled(g_ui_message_buffer, 0x78, 0x38) == 1)
    {
      unk_00789308 = FUN_0056c705(
          *(int *)((char *)&DAT_0058c620[20 + FUN_00522508(3)] + (((s.duel_wizard_color - 1) * 3) << 2)));
      g_lair_or_monster_slots[6].color = s.duel_wizard_color;
      g_lair_or_monster_slots[6].entry_type = s.sprite_draw_w;

      s.text_x = FUN_0056c705(*(int *)&gs_creature_names_00591a08[s.sprite_draw_w].metadata[6]);
      FUN_0050a5f1(s.sprite_draw_w, (unsigned int)s.text_x, 0, -1);

      DAT_008ce538 = s.duel_wizard_color;
      DAT_00742fd0 = 0;
      DAT_007a7874 = 3;

      s.duel_menu_result = FUN_00568320((unsigned int)s.text_x, s.sprite_draw_w);
      if (s.duel_menu_result == 1)
      {
        FUN_005629c3(1);
        g_town_slots[town_index].status_and_ruling_wizard &= ~0xff00;

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
          s.temp_dword = *(unsigned int *)(global_ante_cards[0] + s.duel_ante_slot * 4);
          if (s.temp_dword == -1)
          {
            continue;
          }

          LoadPcxIntoPageOpaque(1, "losedul2.pic");
          StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, 0, 0x280, 0x1e0, PTR_DAT_005832b4, 0, 0, global_screen_width, global_screen_height);
          FUN_004f2407(s.temp_dword, 0x17, 0x50, 1, gs_visit_0077c4f0[9]);
          ClearInputAndWaitForMouseRelease();
          WaitForInputEventUnlessBlocked();
          FUN_004f6d32((unsigned int)s.temp_dword);
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
    s.pics[0] = s_0246_pic_0058ac2c;
    s.pics[1] = s_0364_pic_0058ac44;
    s.pics[2] = s_0335_pic_0058ac5c;
    s.pics[3] = s_0737_pic_0058ac74;
    s.pics[4] = s_0028_pic_0058ac8c;

    DAT_008ce538 = (int)FUN_0040dffd(FUN_005611c8(FUN_0043146b(g_town_slots[town_index].world_x, g_town_slots[town_index].world_y)));

    ShowWizardVisitBackdrop(s.pics[DAT_008ce538 - 1]);
    PTR_DAT_005832b4->font_slot = 1;

    if (FUN_0056615f(DAT_008ce538))
    {
      sprintf(g_ui_message_buffer, gs_visit_0077c4f0[10], gs_wizardnames_0077ee70 + DAT_008ce538 * 0x32);
    }
    else
    {
      sprintf(g_ui_message_buffer, gs_visit_0077c4f0[0xb], gs_wizardnames_0077ee70 + DAT_008ce538 * 0x32);
    }

    strcat(g_ui_message_buffer, gs_visit_0077c4f0[0xc]);

    if (RunTextMenuAtScaled(g_ui_message_buffer, 0x2a, 0x1a) == 1)
    {
      sound_stop(0x10);
      FUN_0050a5c1(DAT_008ce538 - 1);
      if (((g_defeated_wizards_bitmap & (1 << ((unsigned char)DAT_008ce538))) == 0) && (g_shandalar_difficulty == 3))
      {
        FUN_00561bf4(DAT_008ce538);
      }
    }

    DestroyCachedCardArt();
    return 0;
  }
  else if (g_town_slots[town_index].location_type == 5)
  {
    s.pics2[0] = s_0246_pic_0058aca4;
    s.pics2[1] = s_0364_pic_0058acbc;
    s.pics2[2] = s_0335_pic_0058acd4;
    s.pics2[3] = s_0737_pic_0058acec;
    s.pics2[4] = s_0028_pic_0058ad04;

    DAT_008ce538 = FUN_0040dffd(FUN_005611c8(FUN_0043146b(g_town_slots[town_index].world_x, g_town_slots[town_index].world_y)));

    ShowWizardVisitBackdrop(s.pics2[DAT_008ce538 - 1]);
    if (FUN_0056615f(DAT_008ce538))
    {
      sprintf(g_ui_message_buffer, gs_visit_0077c4f0[13], gs_wizardnames_0077ee70 + DAT_008ce538 * 0x32);
    }
    else
    {
      sprintf(g_ui_message_buffer, gs_visit_0077c4f0[14], gs_wizardnames_0077ee70 + DAT_008ce538 * 0x32);
    }
    strcat(g_ui_message_buffer, gs_visit_0077c4f0[0xf]);
    RunTextMenuAtScaled(g_ui_message_buffer, 0x2a, 0x1a);

    sound_stop(0x10);
    DestroyCachedCardArt();
    return 0;
  }

  if ((g_current_quest_destination == town_index) && ((g_current_quest_type >= 0) || (g_current_quest_type < -100)))
  {
    AnimateVisitBackdropZoomIn(GET_TOWN_PIC(town_index));
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
        sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), gs_visit_0077c4f0[0x12], gs_amuletnames_0077d090 + g_current_quest_color * 0x19);
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
        FindDeckSlotForQuestColorAndType((unsigned char)g_current_quest_color,
                                         (unsigned char)(1 << (g_current_quest_destination & 3))))
    {
      s.found_deck_slot = FindDeckSlotForQuestColorAndType((unsigned char)g_current_quest_color,
                                                           (unsigned char)(1 << (g_current_quest_destination & 3)));
      DAT_0078990c[g_current_quest_color] = DAT_0078990c[g_current_quest_color] + 1;

      sprintf(g_ui_message_buffer, gs_visit_0077c4f0[0x13], global_cards_data[deck[s.found_deck_slot - 1] & 0xfff].name);

      sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), gs_visit_0077c4f0[0x14], gs_amuletnames_0077d090 + g_current_quest_color * 0x19);

      PTR_DAT_005832b4->font_slot = 1;
      g_text_menu_abort_requested = 1;
      RunTextMenuAtScaled(g_ui_message_buffer, 0x50, 0x50);

      g_town_slots[g_current_quest_destination].status_and_ruling_wizard =
          g_town_slots[g_current_quest_destination].status_and_ruling_wizard | 1;
      s.quest_handled = 1;
      FUN_0056bfa4(s.found_deck_slot - 1);
      WaitForInputEventUnlessBlocked();
      g_current_quest_destination = -1;
      RefreshAdventureInterfaceLayout();
      FUN_0055060c(1);
    }

    if (g_current_quest_type < -100)
    {
      if (g_town_slots[g_current_quest_destination].location_type > 1)
      {
        s.unk2 = 1;
        g_current_quest_type = g_current_quest_type + 100;
        s.deck_card_index = (int)gs_creature_names_00591a08[-g_current_quest_type].plural[0x33] / 7 + 1;
        if (FUN_005661d9(-g_current_quest_type))
        {
          sprintf(g_ui_message_buffer, gs_visit_0077c4f0[0x15], FUN_00561441(-g_current_quest_type));
        }
        else
        {
          sprintf(g_ui_message_buffer, gs_visit_0077c4f0[0x16], FUN_00561441(-g_current_quest_type));
        }

        sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), gs_visit_0077c4f0[0x17], FUN_004f2e17(g_current_quest_destination));
        RunTextMenuAtScaled(g_ui_message_buffer, 0x50, 0x50);

        // TODO(decomp): This branch continues into the large "choose a card" UI (FUN_00532aeb) and then
        // marks the selected card slot with 0x4000.
        DAT_00580da4 = FUN_00530b7e;
        s.text_x = -1;
        while (s.text_x == -1)
        {
          s.text_x = FUN_00532aeb(gs_visit_0077c4f0[24], g_town_slots[town_index].trade_color_and_type & 0xff, (g_town_slots[town_index].trade_color_and_type >> 8) - 1, s.unk2, 0);
          s.unk2 = 0;
          if (s.text_x != -1)
          {
            strcpy(g_ui_message_buffer, gs_visit_0077c4f0[25]);
          retry:
            s.unk1 = RunTextMenuAt(g_ui_message_buffer, ScaleUiCoordinate(0xf4) + 10, ScaleUiCoordinate(0x15c) + 10);
            if (s.unk1 < 0)
            {
              goto retry;
            }

            if (s.unk1 == 0)
            {
              s.found_deck_slot = FUN_0056bd9d(s.text_x);
              if (s.found_deck_slot != -1)
              {
                deck[s.found_deck_slot] |= 0x4000;
              }
            }
            else
            {
              s.text_x = -1;
            }
            FUN_004ce992(0xf);
          }
        }
        DAT_00580da4 = DrawTiledDialogBoxFrame;
      }
      else
      {
        g_current_quest_type = g_current_quest_type + 100;
        s.deck_card_index = (int)gs_creature_names_00591a08[-g_current_quest_type].plural[0x33] / 7 + 1;
        if (FUN_005661d9(-g_current_quest_type))
        {
          sprintf(g_ui_message_buffer, gs_visit_0077c4f0[26], FUN_00561441(-g_current_quest_type));
        }
        else
        {
          sprintf(g_ui_message_buffer, gs_visit_0077c4f0[27], FUN_00561441(-g_current_quest_type));
        }

        if (s.deck_card_index == 1)
        {
          sprintf(g_ui_message_buffer + strlen(g_ui_message_buffer), gs_visit_0077c4f0[0x12], gs_amuletnames_0077d090 + g_current_quest_color * 0x19);
        }
        else
        {
          FUN_00564e70(g_ui_message_buffer + strlen(g_ui_message_buffer), 0x1000, gs_visit_0077c4f0[0x1c], s.deck_card_index, gs_amuletnames_plural_0077edd0 + g_current_quest_color * 0x19);
        }

        RunTextMenuAtScaled(g_ui_message_buffer, 0x50, 0x50);
        DAT_0078990c[g_current_quest_color] = DAT_0078990c[g_current_quest_color] + s.deck_card_index;
        g_town_slots[g_current_quest_destination].status_and_ruling_wizard =
            g_town_slots[g_current_quest_destination].status_and_ruling_wizard | 1;
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

    if (s.quest_handled != 0)
    {
      s.journal_param = (unsigned int)town_index | 0x80;
    }
    else
    {
      s.journal_param = (unsigned int)town_index;
    }
    FUN_004290e2(1, (int)s.journal_param);
  }
  else
  {
    FUN_004290e2(1, town_index);
  }

  g_town_slots[town_index].status_and_ruling_wizard = g_town_slots[town_index].status_and_ruling_wizard | 2;
  *(int *)(g_town_slots[town_index].data_18_to_63 + 0x44) = *(int *)(g_town_slots[town_index].data_18_to_63 + 0x44) + 1;

  AnimateVisitBackdropZoomIn(GET_TOWN_PIC(town_index));
  // TODO(decomp): Town services UI (shops/healer/clues/map/stats/etc.).
  // This is currently not decompiled in-tree; keep the call to preserve control flow.
  FUN_004ef1ea(town_index);

  *(int *)(g_town_slots[town_index].data_18_to_63 + 0x48) = g_quest_restock_timer;

  if ((g_town_slots[town_index].location_type == 3) || (g_town_slots[town_index].location_type == 2))
  {
    s.distance = -1;
    s.sprite = (EncodedImage *)999;
    for (s.menu_x_320 = 0; s.menu_x_320 < 0xc; s.menu_x_320 = s.menu_x_320 + 1)
    {
      if (g_world_magic_slot_timers[s.menu_x_320].unk_00 == 0)
      {
        continue;
      }

      s.sprite_draw_h = FUN_004ecf30(g_town_slots[town_index].world_x - g_town_slots[g_world_magic_slot_timers[s.menu_x_320].unk_00].world_x,
                                     g_town_slots[town_index].world_y - g_town_slots[g_world_magic_slot_timers[s.menu_x_320].unk_00].world_y);
      if (s.sprite_draw_h < (int)s.sprite)
      {
        s.sprite = (EncodedImage *)s.sprite_draw_h;
        s.distance = s.menu_x_320;
      }
    }

    if ((s.distance != -1) && ((g_world_magic_bitmap & (1 << ((unsigned char)s.distance & 0xff))) == 0))
    {
      s.amulet_name = 0x14f;
      s.msg_len = 0x19e;
      s.tile_color_index = 0x4c;
      s.menu_result = 0x9a;

      LoadPcxIntoPage(1, s_worlbak1_pic_0058ad6c);
      StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, 0, 0x280, 0x1e0, PTR_DAT_005832b4, 0, 0, global_screen_width, global_screen_height);

      s.found_deck_slot = FUN_004bb458(s.distance);
      s.local_dx = 0x28;
      s.local_dx += 0x10;

      s.format = g_world_magic_choice_button_sprite_bank.named.normal[s.distance];

      s.local_dx = ((s.menu_result - s.tile_color_index) - s.format->height) / 2 + s.tile_color_index;
      s.duel_creature_type =
          (((int)s.msg_len - s.amulet_name) - s.format->width) / 2 + s.amulet_name;

      DrawEncodedImageResampled(PTR_DAT_005832b4, ScaleUiCoordinate(s.duel_creature_type),
                                ScaleUiCoordinate(s.local_dx), ScaleUiCoordinate(s.format->width),
                                ScaleUiCoordinate(s.format->height), g_world_magic_choice_button_sprite_bank.named.normal[s.distance]);

      s.local_dx += s.format->height + 0x10;
      s.local_dx /= 2;
      s.duel_creature_type += s.format->width / 2;
      s.duel_creature_type /= 2;

      PTR_DAT_005832b4->font_slot = 4;
      DrawTextAt(PTR_DAT_005832b4, 0x7b, 0x176, 0x4b, gs_visit_0077c4f0[0x1d]);

      s.local_dx = s.local_dx - 5;
      strcpy(g_ui_message_buffer, gs_worldmagic_names_00780660[s.distance]);
      FUN_00430fb5(g_ui_message_buffer, s.duel_creature_type, s.local_dx, 0x40);

      s.local_dx = s.local_dx + 8;
      sprintf(g_ui_message_buffer, gs_visit_0077c4f0[0x1e], Scards[s.distance].worldmagic_price / 2);
      FUN_00430fb5(g_ui_message_buffer, s.duel_creature_type, s.local_dx, 0x7b);

      s.local_dx = s.local_dx + 8;
      strcpy(g_ui_message_buffer, gs_worldmagic_explains_0074b8f0[s.distance]);
      DrawWorldUiFormattedText(PTR_DAT_005832b4, 0x7b, FUN_005501dc(s.duel_creature_type), FUN_005501dc(s.local_dx), g_ui_message_buffer);

      s.local_dx += 0x10;

      /* Matching: the original early-outs for "this town doesn't sell it" (distance != 0) via a jump. */
      if (s.sprite)
      {
        // These are calcualted but unused
        s.world_x_something = g_town_slots[g_world_magic_slot_timers[s.distance].unk_00].world_x - g_town_slots[town_index].world_x;
        s.world_y_something = g_town_slots[g_world_magic_slot_timers[s.distance].unk_00].world_y - g_town_slots[town_index].world_y;

        /* Message: where to find it (direction + town name). */
        s.world_quadrant = GetRelativeWorldQuadrant(g_town_slots[g_world_magic_slot_timers[s.distance].unk_00].world_x,
                                                    g_town_slots[g_world_magic_slot_timers[s.distance].unk_00].world_y);

        FUN_00564e70(g_ui_message_buffer, 0x1000, gs_visit_0077c4f0[0x1f], gs_directions_00765d50 + s.world_quadrant * 0x19, BuildTownDisplayName(g_world_magic_slot_timers[s.distance].unk_00));

        s.local_dx = s.local_dx + 8;
        FUN_00430fb5(g_ui_message_buffer, s.duel_creature_type, s.local_dx, 0x8d);
        s.local_dx = s.local_dx + 8;
        DAT_0058a888 = s.distance;
        ClearInputAndWaitForMouseRelease();
        WaitForInputEventUnlessBlocked();
      }
      else if ((Scards[s.distance].worldmagic_price / 2) <= Gold)
      {
        /* Purchase prompt. */
        FUN_00564e70(g_ui_message_buffer, 0x1000, gs_visit_0077c4f0[0x20], gs_worldmagic_names_00780660[s.distance],
                     BuildTownDisplayName(g_world_magic_slot_timers[s.distance].unk_00), Scards[s.distance].worldmagic_price / 2);
        strcat(g_ui_message_buffer, gs_visit_0077c4f0[0x21]);

        ClearInputAndWaitForMouseRelease();
        s.found_deck_slot = RunTextMenuAtScaled(
            g_ui_message_buffer,
            ((((unsigned int)(global_screen_width - 0x280) < 1) ? 0xffffffce : 0) + 0xbe),
            0x88);
        DAT_0058a888 = s.distance;

        if (s.found_deck_slot == 1)
        {
          Gold = Gold - Scards[s.distance].worldmagic_price / 2;
          g_world_magic_bitmap = g_world_magic_bitmap | (1 << (unsigned char)DAT_0058a888);
          g_world_magic_slot_timers[DAT_0058a888].unk_00 = 0;
          FUN_004290e2(6, s.distance);
        }
        DAT_0058a888 = -1;
      }
      else
      {
        /* Not enough gold. */
        s.local_dx = s.local_dx + 8;
        strcpy(g_ui_message_buffer, gs_visit_0077c4f0[0x22]);
        FUN_00430fb5(g_ui_message_buffer, s.duel_creature_type, s.local_dx, 0xbe);
        s.local_dx = s.local_dx + 8;
        ClearInputAndWaitForMouseRelease();
        WaitForInputEventUnlessBlocked();
      }
    }
  }

  DestroyCachedCardArt();

  if (g_current_quest_deadline < g_quest_restock_timer)
  {
    g_current_quest_destination = -1;
  }
}
