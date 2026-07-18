#include <stdio.h>
#include <string.h>
#include "game_support.h"
#include "global_strings.h"

char *FUN_0044a3bf(int player, int card);

// FUNCTION: MAGIC 0x00443898
unsigned int FUN_00443898(void)
{
  if (stack_size == 0)
  {
    return 0xffffffffU;
  }

  return (unsigned int)DAT_0093917c[stack_size];
}

// FUNCTION: MAGIC 0x00444c43
char *FUN_00444c43(char *out, int msg, int player, int card)
{
  char tmp[300];
  char *prefix;

  if (out == (char *)0)
  {
    return (char *)0;
  }

  if (msg == 0x71)
  {
    prefix = gs_casting_008b4850;
  }
  else if (msg == 0x72)
  {
    prefix = gs_activating_007912e0;
  }
  else if (msg == 0x7e)
  {
    prefix = gs_processing_008ce290;
  }
  else
  {
    prefix = "";
  }

  if (player < 0 || card < 0)
  {
    strcpy(tmp, prefix);
  }
  else
  {
    sprintf(tmp, "%s: %s", prefix, FUN_0044a3bf(player, card));
  }

  strcpy(out, tmp);
  return out;
}

