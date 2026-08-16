#include "defs.h"
#include "../game_support.h"

// FUNCTION: MAGIC 0x00483ac6
// FUNCTION: SHANDALAR 0x004875d7
int card_repentant_blacksmith(int player, int card, event_t event)
{
  int saved_event_result;

  if (event == EVENT_ABILITIES && g_affected_card == card && g_affected_card_controller == player)
  {
    g_event_result |= KEYWORD_PROT_BLACK << (get_sleighted_color(player, card, COLOR_RED) - 1U);
    saved_event_result = g_event_result;
    destroy_attached_auras_of_color(player, card, COLOR_RED);
    g_event_result = saved_event_result;
  }

  return 0;
}
