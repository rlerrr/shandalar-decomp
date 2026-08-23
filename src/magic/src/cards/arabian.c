#include "defs.h"
#include "../game_support.h"

// FUNCTION: MAGIC 0x00483ac6
// FUNCTION: SHANDALAR 0x004875d7
int card_repentant_blacksmith(int player, int card, event_t event)
{
  int saved_event_result;
  int opponent;
  int current_card;
  int ai_bonus;

  if (event == EVENT_SHOULD_AI_PLAY &&
      g_current_phase == 0x22 &&
      g_affected_card == card &&
      g_affected_card_controller == player)
  {
    opponent = 1 - player;
    ai_bonus = 0;
    for (current_card = 0; current_card < g_active_cards_count[opponent]; ++current_card)
    {
      if (((1 << (byte)get_sleighted_color(player, card, COLOR_RED)) &
           (int)(char)PLAYER_CARD_INSTANCE(opponent, current_card).color) != 0 &&
          is_in_play(opponent, current_card))
      {
        ai_bonus += 0x18;
      }
    }
    if (ai_bonus != 0)
    {
      g_ai_modifier += ai_bonus + 0x18;
    }
  }

  if (event == EVENT_ABILITIES && g_affected_card == card && g_affected_card_controller == player)
  {
    g_event_result |= KEYWORD_PROT_BLACK << (get_sleighted_color(player, card, COLOR_RED) - 1U);
    saved_event_result = g_event_result;
    destroy_attached_auras_of_color(player, card, COLOR_RED);
    g_event_result = saved_event_result;
  }

  return 0;
}
