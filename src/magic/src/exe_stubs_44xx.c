#include <stdio.h>
#include <string.h>
#include "game_support.h"
#include "global_strings.h"

char *get_displayed_card_name(int player, int card);

// FUNCTION: MAGIC 0x00443898
unsigned int get_current_stack_action(void)
{
  if (stack_size == 0)
  {
    return 0xffffffffU;
  }

  return (unsigned int)DAT_0093917c[stack_size];
}

// FUNCTION: MAGIC 0x00444c43
char *format_stack_action_text(char *out, int msg, int player, int card)
{
  struct
  {
    char tmp[300];
    char *prefix;
  } s;

  if (out == (char *)0)
  {
    return (char *)0;
  }

  if (msg == 0x71)
  {
    s.prefix = gs_casting_008b4850;
  }
  else if (msg == 0x72)
  {
    s.prefix = gs_activating_007912e0;
  }
  else if (msg == 0x7e)
  {
    s.prefix = gs_processing_008ce290;
  }
  else
  {
    s.prefix = "";
  }

  if (player >= 0 && card >= 0)
  {
    sprintf(s.tmp, "%s: %s", s.prefix, get_displayed_card_name(player, card));
  }
  else
  {
    strcpy(s.tmp, s.prefix);
  }

  strcpy(out, s.tmp);
  return out;
}
