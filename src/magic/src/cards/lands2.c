#include <string.h>
#include "../game_support.h"
#include "../global_strings.h"

// FUNCTION: MAGIC 0x005022a0
// FUNCTION: SHANDALAR 0x00552320
int card_arena(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00503044
// FUNCTION: SHANDALAR 0x005530c4
int card_bazaar_of_baghdad(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00503331
// FUNCTION: SHANDALAR 0x005533b2
int card_city_of_brass(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00503987
// FUNCTION: SHANDALAR 0x00553a0e
int card_desert(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00503ef5
// FUNCTION: SHANDALAR 0x00553f7d
int card_diamond_valley(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00504340
// FUNCTION: SHANDALAR 0x005543ce
int card_elephant_graveyard(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00504ec7
// FUNCTION: SHANDALAR 0x00554f53
int card_island_of_wak_wak(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x0050535f
// FUNCTION: SHANDALAR 0x005553ee
int card_urza_s_mine(int player, int card, event_t event)
{
}

// FUNCTION: MAGIC 0x00505608
// FUNCTION: SHANDALAR 0x00555699
int card_urza_s_tower(int player, int card, event_t event)
{
}

// FUNCTION: MOK 0x004a9310
// FUNCTION: MAGIC 0x005058b1
int helper_dual_land(int player, int card, event_t event, color_test_t available_colors)
{
  int color;
  int first_available_color;
  int num_available_colors;
  color_test_t colors_to_choose_from;

  if (event == EVENT_TAP_CARD && unk_008a9000 != 1)
  {
    play_sound_effect(PLAYER_CARD_INSTANCE(player, card).internal_card_id + 0x16);
  }

  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0 && ((PLAYER_CARD_INSTANCE(player, card).state & (STATE_SUMMONSICK_NOATTACK | STATE_SUMMONSICK_NOTAP)) == 0 || (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0))
    {
      return 1;
    }
    return 0;
  }

  if (event == EVENT_ACTIVATE)
  {
    if (unk_00938e2c == 1 || unk_00938e2c == 0x40 || unk_00938e2c == 0x41)
    {
      color = -1;
      first_available_color = 0;
      while (first_available_color < 7 && color == -1)
      {
        if ((available_colors & (1 << ((unsigned char)first_available_color & 0x1f))) != 0)
        {
          color = first_available_color;
        }
        ++first_available_color;
      }
    }
    else
    {
      if (unk_00938e2c == 0)
      {
        colors_to_choose_from = available_colors;
      }
      else
      {
        colors_to_choose_from = available_colors & unk_00938e2c;
      }

      first_available_color = -1;
      num_available_colors = 0;
      for (color = 0; color < 7; ++color)
      {
        if ((colors_to_choose_from & (1 << ((unsigned char)color & 0x1f))) != COLOR_TEST_0)
        {
          ++num_available_colors;
          if (first_available_color == -1)
          {
            first_available_color = color;
          }
        }
      }

      if (player == active_player && (unk_00926804 & 2) == 0)
      {
        color = first_available_color;
      }
      else if (num_available_colors == 1)
      {
        color = choose_a_color(player, text_lines[0], 1, -1, colors_to_choose_from);
      }
      else
      {
        color = choose_a_color(player, text_lines[0], 1, -1, available_colors);
      }
    }

    if (color == -1)
    {
      spell_fizzled = 1;
    }
    else
    {
      produce_mana(player, color, 1);
      undeclare_mana_available_hex(player, color, 1);
      PLAYER_CARD_INSTANCE(player, card).state |= STATE_TAPPED;
      unk_0092664c[6] = color;

      if (player == active_player && (unk_00926804 & 2) == 0)
      {
        load_text("promptsX1.txt", "MULTI_LANDS");
        switch (color)
        {
        case COLOR_BLACK:
          do_dialog(player, player, card, -1, -1, text_lines[0], 0);
          break;

        case COLOR_BLUE:
          do_dialog(player, player, card, -1, -1, text_lines[1], 0);
          break;

        case COLOR_GREEN:
          do_dialog(player, player, card, -1, -1, text_lines[2], 0);
          break;

        case COLOR_RED:
          do_dialog(player, player, card, -1, -1, text_lines[3], 0);
          break;

        case COLOR_WHITE:
          do_dialog(player, player, card, -1, -1, text_lines[4], 0);
          break;
        }
      }
    }
  }

  if (event == EVENT_COUNT_MANA && card == affected_card && player == affected_card_controller && ((PLAYER_CARD_INSTANCE(player, card).state & (STATE_SUMMONSICK_NOATTACK | STATE_SUMMONSICK_NOTAP)) == 0 || (global_cards_data[PLAYER_CARD_INSTANCE(player, card).internal_card_id].type & TYPE_CREATURE) == 0) && (PLAYER_CARD_INSTANCE(player, card).state & STATE_TAPPED) == 0)
  {
    declare_mana_available_hex(player, available_colors, 1);
  }

  return 0;
}

// FUNCTION: MAGIC 0x00505dd2
// FUNCTION: SHANDALAR 0x00555e68
int card_badlands(int player, int card, event_t event)
{
  if (event == EVENT_ACTIVATE)
  {
    load_text("promptsX1.txt", "BADLANDS");
  }

  return helper_dual_land(player, card, event, PLAYER_CARD_INSTANCE(player, card).mana_color);
}

// FUNCTION: MAGIC 0x00505e42
// FUNCTION: SHANDALAR 0x00555ed8
int card_bayou(int player, int card, event_t event)
{
  if (event == EVENT_ACTIVATE)
  {
    load_text("promptsX1.txt", "BAYOU");
  }

  return helper_dual_land(player, card, event, PLAYER_CARD_INSTANCE(player, card).mana_color);
}

// FUNCTION: MAGIC 0x00505eb2
// FUNCTION: SHANDALAR 0x00555f48
int card_plateau(int player, int card, event_t event)
{
  if (event == EVENT_ACTIVATE)
  {
    load_text("promptsX1.txt", "PLATEAU");
  }

  return helper_dual_land(player, card, event, PLAYER_CARD_INSTANCE(player, card).mana_color);
}

// FUNCTION: MAGIC 0x00505f22
// FUNCTION: SHANDALAR 0x00555fb8
int card_savannah(int player, int card, event_t event)
{
  if (event == EVENT_ACTIVATE)
  {
    load_text("promptsX1.txt", "SAVANNAH");
  }

  return helper_dual_land(player, card, event, PLAYER_CARD_INSTANCE(player, card).mana_color);
}

// FUNCTION: MAGIC 0x00505f92
// FUNCTION: SHANDALAR 0x00556028
int card_scrubland(int player, int card, event_t event)
{
  if (event == EVENT_ACTIVATE)
  {
    load_text("promptsX1.txt", "SCRUBLAND");
  }

  return helper_dual_land(player, card, event, PLAYER_CARD_INSTANCE(player, card).mana_color);
}

// FUNCTION: MAGIC 0x00506002
// FUNCTION: SHANDALAR 0x00556098
int card_taiga(int player, int card, event_t event)
{
  if (event == EVENT_ACTIVATE)
  {
    load_text("promptsX1.txt", "TAIGA");
  }

  return helper_dual_land(player, card, event, PLAYER_CARD_INSTANCE(player, card).mana_color);
}

// FUNCTION: MAGIC 0x00506072
// FUNCTION: SHANDALAR 0x00556108
int card_tropical_island(int player, int card, event_t event)
{
  if (event == EVENT_ACTIVATE)
  {
    load_text("promptsX1.txt", "TROPICAL_ISLAND");
  }

  return helper_dual_land(player, card, event, PLAYER_CARD_INSTANCE(player, card).mana_color);
}

// FUNCTION: MAGIC 0x005060e2
// FUNCTION: SHANDALAR 0x00556178
int card_tundra(int player, int card, event_t event)
{
  if (event == EVENT_ACTIVATE)
  {
    load_text("promptsX1.txt", "TUNDRA");
  }

  return helper_dual_land(player, card, event, PLAYER_CARD_INSTANCE(player, card).mana_color);
}

// FUNCTION: MAGIC 0x00506152
// FUNCTION: SHANDALAR 0x005561e8
int card_underground_sea(int player, int card, event_t event)
{
  if (event == EVENT_ACTIVATE)
  {
    load_text("promptsX1.txt", "UNDERGROUND_SEA");
  }

  return helper_dual_land(player, card, event, PLAYER_CARD_INSTANCE(player, card).mana_color);
}

// FUNCTION: MAGIC 0x005061c2
// FUNCTION: SHANDALAR 0x00556258
int card_volcanic_island(int player, int card, event_t event)
{
  if (event == EVENT_ACTIVATE)
  {
    load_text("promptsX1.txt", "VOLCANIC_ISLAND");
  }

  return helper_dual_land(player, card, event, PLAYER_CARD_INSTANCE(player, card).mana_color);
}
