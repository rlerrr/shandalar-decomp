#include <windows.h>
#include <string.h>
#include <stddef.h>

#include "defs.h"
#include "shandalar.h"
#include "magic/src/global_other.h"
#include "magic/src/global_state.h"
#include "magic/src/global_strings.h"
#include "magic/src/game_support.h"
#include "magic/src/network.h"
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
extern HPALETTE g_realized_palette_handle;
extern RpBitsPalettePacket g_palette_data_words;

extern FacemakerWindowBounds *PTR_DAT_005832b4;
extern FacemakerWindowBounds *PTR_DAT_005832dc;

int RandomIntLessThan(int param_1);
int ScaleUiCoordinateFrom320(int value);

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

void DrawAdventureCard(int card_index, int x, int y, int full_card, char *banner_label);
void DrawAdventureCardSized(int card_index, int x, int y, int width, int height, int full_card, char *banner_label);
void DestroyCachedCardArt(void);

void DrawCenteredTextLineWithShadow(char *text, int center_x, int y, int color_index);
void DrawTextAt(FacemakerWindowBounds *window, int color, int x, int y, char *text, ...);
void DrawScaledTextNoShadow(char *text, int x, int y, int color_index);

void UpdateMouseSnapshot(void);
int HasQueuedKeyInput(void);
int PopNormalizedQueuedKeyInput(void);
void ClearInputAndWaitForMouseRelease(void);

typedef ptrdiff_t INT_PTR;

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
  if (g_duel_ai_mode_state == 1)
  {
    return 1;
  }

  return show_cardlist(param_1, 0, 0, param_2, param_3, (unsigned int)param_4, param_5);
}

// FUNCTION: SHANDALAR 0x0056a515
int SelectAdventureListCardIndex(int player, int *card_ids, int card_count, char *title, int require_card_click, int *out_selection)
{
  struct
  {
    char *saved_accept_keys;
    int accept_key;
    int key_code;
    char *accept_keys;
    int sprite_copy_index;
    EncodedImage *sprite_load_buffer[13];
    EncodedImage *unused_sprite_row_3[3];
    EncodedImage *unused_sprite_row_2[3];
    EncodedImage *unused_sprite_2;
    EncodedImage *unused_sprite_row_1[3];
    EncodedImage *buy_button_sprite;
    int hover_index;
    EncodedImage *unused_sprite_1;
    int has_input;
    int y_step;
    int selected_index;
    int row_count;
    int card_draw_y[500];
    int card_draw_x[500];
    int y;
    int x;
    int visible_count;
    int card_indices[500];
    int i;
  } s;
  int result;

  if ((player == active_player) &&
      ((g_duel_network_flags & 2) != 0) &&
      (unk_00742fc4 != 0))
  {
    ReportUnexpectedNetworkPacketType(player, 0x19);
    return g_network_result_value;
  }

  if (((player == active_player) &&
       ((g_duel_network_flags & 2) == 0)) ||
      (g_duel_ai_mode_state == 1))
  {
    s.visible_count = 0;
    for (s.i = 0; card_count > s.i; s.i = s.i + 1)
    {
      if (card_ids[s.i] != -1)
      {
        s.card_indices[s.visible_count] = s.i;
        s.visible_count = s.visible_count + 1;
      }
    }

    unk_00939340 = RandomIntLessThan(s.visible_count);
    if (player != unk_008b35ec)
    {
      if (g_duel_ai_mode_state == 1)
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

  if (unk_00742fc4 == 0)
  {
    ReadPalette("todpal.tr", (char *)0);
    ClearGraphicsPageWithPaletteColor(0, 0);
    SelectPalette(global_main_hdc, g_realized_palette_handle, FALSE);

    LoadPcxIntoPageNoPalette("advfac64.pic");
    LoadPcxResource(1, 0, 0, "seedeck.pic", &g_palette_data_words);
    StretchBlitGraphicsRect(PTR_DAT_005832dc, 0, 0, 0x280, 0x1e0, PTR_DAT_005832b4, 0, 0, global_screen_width, global_screen_height);

    s.x = 0;
    s.row_count = 0;
    for (s.i = 0; card_count > s.i; s.i = s.i + 1)
    {
      if ((card_ids[s.i] != -1) &&
          ((s.i == 0) || (card_ids[s.i - 1] != card_ids[s.i])))
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
    s.visible_count = 0;
    s.y = 0x10;
    for (s.i = 0; card_count > s.i; s.i = s.i + 1)
    {
      if ((card_ids[s.i] != -1) &&
          ((s.i == 0) || (card_ids[s.i - 1] != card_ids[s.i])))
      {
        s.card_draw_x[s.visible_count] = s.x + 4;
        s.card_draw_y[s.visible_count] = s.y;
        s.card_indices[s.visible_count] = s.i;
        s.visible_count = s.visible_count + 1;

        s.x = s.x + 0x38;
        if (s.x >= 0x140)
        {
          s.x = 0x60;
          s.y = s.y + s.y_step;
        }
      }
    }

    DrawCenteredTextLineWithShadow(title, 0xa0, 2, 0xff);
    s.hover_index = -1;
    s.has_input = 0;

    ReadSpriteEntryPointers(&s.sprite_load_buffer[0], "BuyButtons.spr");
    s.buy_button_sprite = s.sprite_load_buffer[0];
    s.unused_sprite_1 = s.sprite_load_buffer[1];
    s.unused_sprite_2 = s.sprite_load_buffer[2];
    for (s.sprite_copy_index = 0; s.sprite_copy_index < 3; s.sprite_copy_index = s.sprite_copy_index + 1)
    {
      s.unused_sprite_row_1[s.sprite_copy_index] = s.sprite_load_buffer[s.sprite_copy_index + 3];
    }
    for (s.sprite_copy_index = 0; s.sprite_copy_index < 3; s.sprite_copy_index = s.sprite_copy_index + 1)
    {
      s.unused_sprite_row_2[s.sprite_copy_index] = s.sprite_load_buffer[s.sprite_copy_index + 6];
    }
    for (s.sprite_copy_index = 0; s.sprite_copy_index < 3; s.sprite_copy_index = s.sprite_copy_index + 1)
    {
      s.unused_sprite_row_3[s.sprite_copy_index] = s.sprite_load_buffer[s.sprite_copy_index + 9];
    }
    for (s.i = 0; s.visible_count > s.i; s.i = s.i + 1)
    {
      DrawAdventureCard(card_ids[s.card_indices[s.i]] & 0xfff, s.card_draw_x[s.i], s.card_draw_y[s.i], 0, "");
    }

  get_input:
    do
    {
      UpdateMouseSnapshot();
      s.selected_index = -1;
      g_mouse_x_snapshot = (g_mouse_x_snapshot * 0x140) / global_screen_width;
      g_mouse_y_snapshot = (g_mouse_y_snapshot * 0xf0) / global_screen_height;

      for (s.i = 0; s.visible_count > s.i; s.i = s.i + 1)
      {
        if (s.card_draw_x[s.i] > g_mouse_x_snapshot || g_mouse_x_snapshot >= s.card_draw_x[s.i] + 0x30)
        {
          continue;
        }

        if (s.card_draw_y[s.i] > g_mouse_y_snapshot || g_mouse_y_snapshot >= s.card_draw_y[s.i] + 0x30)
        {
          continue;
        }

        s.selected_index = s.card_indices[s.i];
      }

      if ((s.selected_index != -1) &&
          (card_ids[s.selected_index] != card_ids[s.hover_index]))
      {
        DrawAdventureCard(card_ids[s.selected_index] & 0xfff, 8, 0x40, 1, "");
        s.hover_index = s.selected_index;
      }

      if (require_card_click != 0)
      {
        if (((g_mouse_button_mask_snapshot != 0) ||
             (HasQueuedKeyInput() != 0)) &&
            (s.selected_index != -1))
        {
          s.has_input = 1;
        }
        else
        {
          s.has_input = 0;
        }
      }
      else if ((g_mouse_button_mask_snapshot != 0) ||
               (HasQueuedKeyInput() != 0))
      {
        s.has_input = 1;
      }
      else
      {
        s.has_input = 0;
      }
    } while (s.has_input == 0);

    if (require_card_click != 0)
    {

      BlitGraphicsRect(PTR_DAT_005832b4,
                       ScaleUiCoordinateFrom320(0xdc) / 2,
                       ScaleUiCoordinateFrom320(0x34) / 2,
                       ScaleUiCoordinateFrom320(0xc5) / 2,
                       ScaleUiCoordinateFrom320(0x111) / 2,
                       PTR_DAT_005832dc,
                       ScaleUiCoordinateFrom320(0xdc) / 2,
                       ScaleUiCoordinateFrom320(0x34) / 2);

      strcpy(g_ui_message_buffer, gs_showcard_text_0077e110.title);
      DrawEncodedImageResampled(PTR_DAT_005832b4,
                                ScaleUiCoordinateFrom320(0xdc) / 2,
                                ScaleUiCoordinateFrom320(0x34) / 2,
                                ScaleUiCoordinateFrom320(0xc5) / 2,
                                ScaleUiCoordinateFrom320(0x10f) / 2,
                                s.buy_button_sprite);
      DrawAdventureCardSized(card_ids[s.selected_index] & 0xfff, 0x7a, 0x29, 0x4b, 0x70, 1, "");

      PTR_DAT_005832b4->font_slot = 1;
      DrawScaledTextNoShadow(g_ui_message_buffer, 0x76, 0x20, 0x1b);

      s.key_code = PopNormalizedQueuedKeyInput();
      s.accept_keys = gs_showcard_text_0077e110.accept_keys;
      s.accept_key = 0;
      while (*s.accept_keys != '\0')
      {
        s.saved_accept_keys = s.accept_keys;
        s.accept_keys = s.accept_keys + 1;
        if (*s.saved_accept_keys == s.key_code)
        {
          s.accept_key = 1;
        }
      }

      if (s.accept_key == 0)
      {
        BlitGraphicsRect(PTR_DAT_005832dc,
                         ScaleUiCoordinateFrom320(0xdc) / 2,
                         ScaleUiCoordinateFrom320(0x34) / 2,
                         ScaleUiCoordinateFrom320(0xc5) / 2,
                         ScaleUiCoordinateFrom320(0x111) / 2,
                         PTR_DAT_005832b4,
                         ScaleUiCoordinateFrom320(0xdc) / 2,
                         ScaleUiCoordinateFrom320(0x34) / 2);
        ClearInputAndWaitForMouseRelease();
        goto get_input;
      }
    }

    FreeSpriteBlob(s.buy_button_sprite);
    DestroyCachedCardArt();
    return s.selected_index;
  }
  else
  {
    result = (int)FUN_0055837e(card_ids, card_count, title, require_card_click, (char *)out_selection);
  }

  if ((player == unk_008b35ec) &&
      ((g_duel_network_flags & 2) != 0))
  {
    g_network_result_value = result;
    g_network_result_packet_type = 0x19;
    TENTATIVE_send_network_result(player, 0x19);
  }

  return result;
}
