#include <windows.h>
#include <string.h>
#include <stddef.h>

#include "defs.h"
#include "shandalar.h"
#include "magic/src/global_other.h"
#include "magic/src/global_state.h"
#include "magic/src/global_strings.h"
#include "magic/src/game_support.h"
#include "shandalar_global_strings.h"
#include "facemaker/src/facemaker_types.h"
#include "drawcardlib/src/pic.h"

extern int global_screen_width;
extern int global_screen_height;
extern int g_mouse_x_snapshot;
extern int g_mouse_y_snapshot;
extern int g_mouse_button_mask_snapshot;
extern char g_ui_message_buffer[0x1000];
extern HDC global_main_hdc;
extern HPALETTE g_palette_handle;
extern RpBitsPalettePacket g_palette_data_words;

extern FacemakerWindowBounds *PTR_DAT_005832b4;
extern FacemakerWindowBounds *PTR_DAT_005832dc;

int FUN_00522508(int param_1);
int FUN_005501dc(int value);

int ReadPalette(char *palette_text_path, char *palette_binary_path);
void ClearGraphicsPageWithPaletteColor(int page_number, int palette_color);
void LoadPcxResource(int page_number, int x, int y, char *path, void *opaque);
void LoadPcxIntoPageNoPalette(char *path);
void StretchBlitGraphicsRect(FacemakerWindowBounds *dst, int dst_x, int dst_y, int src_w, int src_h,
                             FacemakerWindowBounds *src, int src_x, int src_y, int copy_w, int copy_h);
void BlitGraphicsRect(FacemakerWindowBounds *dst, int dst_x, int dst_y, int width, int height,
                      FacemakerWindowBounds *src, int src_x, int src_y);

int ReadSpriteEntryPointers(EncodedImage **out_sprite_entries, char *sprite_path);
void DrawEncodedImageResampled(FacemakerWindowBounds *dst, int x, int y, int width, int height, EncodedImage *encoded_image);
void FreeSpriteBlob(void *sprite_blob);

void FUN_004f2407(int card_index, int x, int y, int full_card, char *banner_label);
void FUN_004f263b(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, char *param_7);
void DestroyCachedCardArt(void);

void DrawCenteredTextLineWithShadow(char *text, int center_x, int y, int color_index);
void DrawTextAt(FacemakerWindowBounds *window, int color, int x, int y, char *text);

void UpdateMouseSnapshot(void);
int HasQueuedKeyInput(void);
int PopNormalizedQueuedKeyInput(void);
void ClearInputAndWaitForMouseRelease(void);

typedef ptrdiff_t INT_PTR;

// FUNCTION: SHANDALAR 0x0056c91f
int TENTATIVE_wait_for_network_result(int player, signed int packet_type)
{
  (void)player;
  (void)packet_type;
  return 0;
}

// FUNCTION: SHANDALAR 0x0056c931
int TENTATIVE_send_network_result(int player, signed int packet_type)
{
  (void)player;
  (void)packet_type;
  return 0;
}

// FUNCTION: SHANDALAR 0x004c4181
void FUN_004c4181(void)
{
}

// FUNCTION: SHANDALAR 0x004c42f8
void FUN_004c42f8(void)
{
}

// FUNCTION: SHANDALAR 0x0055837e
INT_PTR __cdecl FUN_0055837e(int *param_1, int param_2, void *param_3, int param_4, char *param_5)
{
  if (DAT_008bd200 == 1)
  {
    return 1;
  }

  return show_cardlist(param_1, 0, 0, param_2, param_3, (unsigned int)param_4, param_5);
}

// FUNCTION: SHANDALAR 0x0056a515
int FUN_0056a515(int param_1, int *param_2, int param_3, char *param_4, int param_5, int *param_6)
{
  struct
  {
    int card_indices[500];
    int card_draw_x[500];
    int card_draw_y[500];
    EncodedImage *buy_button_sprites[13];

    int visible_count;
    int row_count;
    int y_step;
    int x;
    int y;
    int selected_index;
    int hover_index;
    int has_input;
    int key_code;
    int accept_key;
    char *accept_keys;
    int i;
    int j;
  } s;
  int result;

  if ((param_1 == active_player) &&
      ((gs_window_title_opponent_attack_009267a0[100] & 2) != 0) &&
      (unk_00742fc4 != 0))
  {
    TENTATIVE_wait_for_network_result(param_1, 0x19);
    return *(int *)&gs_phasebar_your_upkeep_007a7c20[0xec];
  }
  else if (((param_1 == active_player) &&
            ((gs_window_title_opponent_attack_009267a0[100] & 2) == 0)) ||
           (DAT_008bd200 == 1))
  {
    s.visible_count = 0;
    for (s.i = 0; s.i < param_3; s.i = s.i + 1)
    {
      if (param_2[s.i] != -1)
      {
        s.card_indices[s.visible_count] = s.i;
        s.visible_count = s.visible_count + 1;
      }
    }

    unk_00939340 = FUN_00522508(s.visible_count);
    if (param_1 != unk_008b35ec)
    {
      if (DAT_008bd200 == 1)
      {
        FUN_004c4181();
      }
      else
      {
        FUN_004c42f8();
      }
    }

    return s.card_indices[unk_00939340];
  }
  else if (unk_00742fc4 == 0)
  {
    ReadPalette("todpal.tr", (char *)0);
    ClearGraphicsPageWithPaletteColor(0, 0);
    SelectPalette(global_main_hdc, g_palette_handle, FALSE);

    LoadPcxIntoPageNoPalette("advfac64.pic");
    LoadPcxResource(1, 0, 0, "seedeck.pic", &g_palette_data_words);
    StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, 0, 0x280, 0x1e0, PTR_DAT_005832b4, 0, 0, global_screen_width, global_screen_height);

    s.row_count = 0;
    for (s.i = 0; s.i < param_3; s.i = s.i + 1)
    {
      if ((param_2[s.i] != -1) &&
          ((s.i == 0) || (param_2[s.i - 1] != param_2[s.i])))
      {
        s.row_count = s.row_count + 1;
      }
    }

    s.visible_count = (s.row_count - 1) / 5;
    if (s.visible_count == 0)
    {
      s.visible_count = 1;
    }
    s.y_step = 0x54 / s.visible_count;

    s.x = 0x60;
    s.y = 0x10;
    s.visible_count = 0;
    for (s.i = 0; s.i < param_3; s.i = s.i + 1)
    {
      if ((param_2[s.i] != -1) &&
          ((s.i == 0) || (param_2[s.i - 1] != param_2[s.i])))
      {
        s.card_draw_x[s.visible_count] = s.x + 4;
        s.card_draw_y[s.visible_count] = s.y;
        s.card_indices[s.visible_count] = s.i;
        s.visible_count = s.visible_count + 1;

        s.x = s.x + 0x38;
        if (0x13f < s.x)
        {
          s.x = 0x60;
          s.y = s.y + s.y_step;
        }
      }
    }

    DrawCenteredTextLineWithShadow(param_4, 0xa0, 2, 0xff);
    s.hover_index = -1;

    ReadSpriteEntryPointers(&s.buy_button_sprites[0], "BuyButtons.spr");
    for (s.i = 0; s.i < s.visible_count; s.i = s.i + 1)
    {
      FUN_004f2407(param_2[s.card_indices[s.i]] & 0xfff, s.card_draw_x[s.i], s.card_draw_y[s.i], 0, "");
    }

    while (1)
    {
      do
      {
        UpdateMouseSnapshot();
        result = -1;
        g_mouse_x_snapshot = (g_mouse_x_snapshot * 0x140) / global_screen_width;
        g_mouse_y_snapshot = (g_mouse_y_snapshot * 0xf0) / global_screen_height;

        for (s.i = 0; s.i < s.visible_count; s.i = s.i + 1)
        {
          if ((s.card_draw_x[s.i] <= g_mouse_x_snapshot) &&
              (g_mouse_x_snapshot < s.card_draw_x[s.i] + 0x30) &&
              (s.card_draw_y[s.i] <= g_mouse_y_snapshot) &&
              (g_mouse_y_snapshot < s.card_draw_y[s.i] + 0x30))
          {
            result = s.card_indices[s.i];
          }
        }

        if ((result != -1) &&
            (param_2[result] != param_2[s.hover_index]))
        {
          FUN_004f2407(param_2[result] & 0xfff, 8, 0x40, 1, "");
          s.hover_index = result;
        }

        if (param_5 == 0)
        {
          if ((g_mouse_button_mask_snapshot == 0) &&
              (HasQueuedKeyInput() == 0))
          {
            s.has_input = 0;
          }
          else
          {
            s.has_input = 1;
          }
        }
        else if (((g_mouse_button_mask_snapshot == 0) &&
                  (HasQueuedKeyInput() == 0)) ||
                 (result == -1))
        {
          s.has_input = 0;
        }
        else
        {
          s.has_input = 1;
        }
      } while (s.has_input == 0);

      if (param_5 == 0)
      {
        break;
      }

      BlitGraphicsRect(PTR_DAT_005832b4,
                       FUN_005501dc(0xdc) / 2,
                       FUN_005501dc(0x34) / 2,
                       FUN_005501dc(0xc5) / 2,
                       FUN_005501dc(0x111) / 2,
                       PTR_DAT_005832dc,
                       FUN_005501dc(0xdc) / 2,
                       FUN_005501dc(0x34) / 2);

      strcpy(g_ui_message_buffer, gs_showcard_name_0077e110);
      DrawEncodedImageResampled(PTR_DAT_005832b4,
                                FUN_005501dc(0xdc) / 2,
                                FUN_005501dc(0x34) / 2,
                                FUN_005501dc(0xc5) / 2,
                                FUN_005501dc(0x10f) / 2,
                                s.buy_button_sprites[0]);
      FUN_004f263b(param_2[result] & 0xfff, 0x7a, 0x29, 0x4b, 0x70, 1, "");

      PTR_DAT_005832b4->font_slot = 1;
      DrawTextAt(PTR_DAT_005832b4, 0x76, 0x20, 0x1b, g_ui_message_buffer);

      s.key_code = PopNormalizedQueuedKeyInput();
      s.accept_keys = gs_showcard_type_0077e142;
      s.accept_key = 0;
      while (*s.accept_keys != '\0')
      {
        if (*s.accept_keys == s.key_code)
        {
          s.accept_key = 1;
        }
        s.accept_keys = s.accept_keys + 1;
      }

      if (s.accept_key != 0)
      {
        break;
      }

      BlitGraphicsRect(PTR_DAT_005832dc,
                       FUN_005501dc(0xdc) / 2,
                       FUN_005501dc(0x34) / 2,
                       FUN_005501dc(0xc5) / 2,
                       FUN_005501dc(0x111) / 2,
                       PTR_DAT_005832b4,
                       FUN_005501dc(0xdc) / 2,
                       FUN_005501dc(0x34) / 2);
      ClearInputAndWaitForMouseRelease();
    }

    FreeSpriteBlob(s.buy_button_sprites[0]);
    DestroyCachedCardArt();
  }
  else
  {
    result = (int)FUN_0055837e(param_2, param_3, param_4, param_5, (char *)param_6);
    if ((param_1 == unk_008b35ec) &&
        ((gs_window_title_opponent_attack_009267a0[100] & 2) != 0))
    {
      gs_phasebar_your_upkeep_007a7c20[0xe8] = '\x19';
      *(int *)&gs_phasebar_your_upkeep_007a7c20[0xec] = result;
      TENTATIVE_send_network_result(param_1, 0x19);
    }
  }

  return result;
}
