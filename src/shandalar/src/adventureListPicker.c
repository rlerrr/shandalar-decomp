#include <windows.h>
#include <string.h>
#include <stddef.h>

#include "defs.h"
#include "shandalar.h"
#include "shandalar_internal.h"
#include "magic/src/global_other.h"
#include "magic/src/global_state.h"
#include "magic/src/global_strings.h"
#include "magic/src/game_support.h"
#include "magic/src/network.h"
#include "shandalar_global_strings.h"
#include "facemaker/src/facemaker_types.h"
#include "drawcardlib/src/pic.h"

extern HPALETTE g_realized_palette_handle;


int internal_rand(int max_exclusive);
int ScaleUiCoordinateFrom320(int value);

void LoadPcxResource(int page_number, int x, int y, char *path, void *opaque);


void DrawAdventureCard(int card_index, int x, int y, int full_card, char *banner_label);
void DrawAdventureCardSized(int card_index, int x, int y, int width, int height, int full_card, char *banner_label);
void DestroyCachedCardArt(void);

void DrawCenteredTextLineWithShadow(char *text, int center_x, int y, int color_index);
void DrawScaledTextNoShadow(char *text, int x, int y, int color_index);


typedef ptrdiff_t INT_PTR;

// FUNCTION: SHANDALAR 0x0055837e
INT_PTR __cdecl ShowAdventureListCardList(int *card_ids, int card_count, char *title, int require_selection, char *prompt)
{
  if (g_duel_ai_mode_state == 1)
  {
    return 1;
  }

  return show_cardlist(card_ids, 0, 0, card_count, title, (unsigned int)require_selection, prompt);
}

// FUNCTION: SHANDALAR 0x0056a515
int show_deck(int player, int *cards, int count, char *title, int require_selection, char *prompt)
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

  if ((player == g_other_player) &&
      ((g_duel_network_flags & 2) != 0) &&
      (g_duel_active != 0))
  {
    TENTATIVE_wait_for_network_result(player, 0x19);
    return g_network_result_packet.result;
  }

  if (((player == g_other_player) &&
       ((g_duel_network_flags & 2) == 0)) ||
      (g_duel_ai_mode_state == 1))
  {
    s.visible_count = 0;
    for (s.i = 0; count > s.i; s.i = s.i + 1)
    {
      if (cards[s.i] != -1)
      {
        s.card_indices[s.visible_count] = s.i;
        s.visible_count = s.visible_count + 1;
      }
    }

    g_ai_recorded_choice = internal_rand(s.visible_count);
    if (player != g_active_player)
    {
      if (g_duel_ai_mode_state == 1)
      {
        record_ai_action_selection();
      }
      else
      {
        replay_ai_action_selection();
      }
    }

    return s.card_indices[g_ai_recorded_choice];
  }

  if (g_duel_active == 0)
  {
    ReadPalette("todpal.tr", (char *)0);
    ClearGraphicsPageWithPaletteColor(0, 0);
    SelectPalette(global_main_hdc, g_realized_palette_handle, FALSE);

    LoadPcxIntoPageNoPalette("advfac64.pic");
    LoadPcxResource(1, 0, 0, "seedeck.pic", &g_palette_data_words);
    StretchBlitGraphicsRect(g_page1_window_bounds, 0, 0, 0x280, 0x1e0, g_page0_window_bounds, 0, 0, global_screen_width, global_screen_height);

    s.x = 0;
    s.row_count = 0;
    for (s.i = 0; count > s.i; s.i = s.i + 1)
    {
      if ((cards[s.i] != -1) &&
          ((s.i == 0) || (cards[s.i - 1] != cards[s.i])))
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
    for (s.i = 0; count > s.i; s.i = s.i + 1)
    {
      if ((cards[s.i] != -1) &&
          ((s.i == 0) || (cards[s.i - 1] != cards[s.i])))
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
      DrawAdventureCard(cards[s.card_indices[s.i]] & 0xfff, s.card_draw_x[s.i], s.card_draw_y[s.i], 0, "");
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
          (cards[s.selected_index] != cards[s.hover_index]))
      {
        DrawAdventureCard(cards[s.selected_index] & 0xfff, 8, 0x40, 1, "");
        s.hover_index = s.selected_index;
      }

      if (require_selection != 0)
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

    if (require_selection != 0)
    {

      BlitGraphicsRect(g_page0_window_bounds,
                       ScaleUiCoordinateFrom320(0xdc) / 2,
                       ScaleUiCoordinateFrom320(0x34) / 2,
                       ScaleUiCoordinateFrom320(0xc5) / 2,
                       ScaleUiCoordinateFrom320(0x111) / 2,
                       g_page1_window_bounds,
                       ScaleUiCoordinateFrom320(0xdc) / 2,
                       ScaleUiCoordinateFrom320(0x34) / 2);

      strcpy(g_ui_message_buffer, gs_showcard_text_0077e110.title);
      DrawEncodedImageResampled(g_page0_window_bounds,
                                ScaleUiCoordinateFrom320(0xdc) / 2,
                                ScaleUiCoordinateFrom320(0x34) / 2,
                                ScaleUiCoordinateFrom320(0xc5) / 2,
                                ScaleUiCoordinateFrom320(0x10f) / 2,
                                s.buy_button_sprite);
      DrawAdventureCardSized(cards[s.selected_index] & 0xfff, 0x7a, 0x29, 0x4b, 0x70, 1, "");

      g_page0_window_bounds->font_slot = 1;
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
        BlitGraphicsRect(g_page1_window_bounds,
                         ScaleUiCoordinateFrom320(0xdc) / 2,
                         ScaleUiCoordinateFrom320(0x34) / 2,
                         ScaleUiCoordinateFrom320(0xc5) / 2,
                         ScaleUiCoordinateFrom320(0x111) / 2,
                         g_page0_window_bounds,
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
    result = (int)ShowAdventureListCardList(cards, count, title, require_selection, prompt);
  }

  if ((player == g_active_player) &&
      ((g_duel_network_flags & 2) != 0))
  {
    g_network_result_packet.result = result;
    g_network_result_packet.packet_type = 0x19;
    TENTATIVE_send_network_result(player, 0x19);
  }

  return result;
}
