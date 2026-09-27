#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "magic_sealed_build.h"
#include "deckdll/src/card_db.h"

// GLOBAL: MAGIC 0x0074b62c
FILE *g_sealed_deck_report;
// GLOBAL: MAGIC 0x0074b5e0
FILE *g_sealed_deck_build_log;
// GLOBAL: MAGIC 0x0074b5e4
FILE *g_sealed_deck_build_extra_log;
// GLOBAL: MAGIC 0x0057449c
int g_sealed_deck_build_report_enabled;
// GLOBAL: MAGIC 0x005744a0
int g_sealed_deck_build_report_extra_enabled;

// FUNCTION: MAGIC 0x004c4c50
int shell_choose_sealed_card_to_remove(int *deck, int count, int *deck_value)
{
  struct
  {
    int best_value;
    int duplicate_index;
    int card_index;
    int removed_card;
    int best_index;
    int value;
    int values[1000];
  } s;

  for (s.card_index = 0; s.card_index < 1000; ++s.card_index)
    s.values[s.card_index] = -20000;
  if (g_sealed_deck_build_log != NULL)
    fprintf(g_sealed_deck_build_log,
            "\nATTEMTPING TO TAKE OUT A CARD (current size: %d)\n", count);
  s.best_value = -10000;
  s.best_index = -1;
  for (s.card_index = 0; s.card_index < count; ++s.card_index)
  {
    if ((deck[s.card_index] < -5 || deck[s.card_index] > -1) &&
        s.values[s.card_index] == -20000)
    {
      s.removed_card = deck[s.card_index];
      deck[s.card_index] = deck[count - 1];
      if (g_sealed_deck_build_log != NULL)
        fprintf(g_sealed_deck_build_log, "\nCompute Deck Value without %s:\n",
                global_raw_cards_storage[s.removed_card].full_name);
      s.value = shell_evaluate_sealed_deck(deck, count - 1, NULL, NULL);
      for (s.duplicate_index = s.card_index + 1;
           s.duplicate_index < count; ++s.duplicate_index)
        if (deck[s.duplicate_index] == s.removed_card)
          s.values[s.duplicate_index] = s.value;
      if (s.value > s.best_value)
      {
        s.best_value = s.value;
        s.best_index = s.card_index;
      }
      deck[s.card_index] = s.removed_card;
    }
  }
  if (deck_value != NULL)
    *deck_value = s.best_value;
  return s.best_index;
}

// FUNCTION: MAGIC 0x004c41af
int shell_build_sealed_deck(int *card_pool, int pool_count, int free_land_count,
                           int difficulty, int unknown, int minimum_size,
                           int maximum_size, int *deck, int *deck_count)
{
  struct
  {
    int free_land_index;
    int best_land_value;
    int removed_card;
    int removed_land_color;
    int best_land_color;
    int report_index;
    int deck_value;
    unsigned int land_color_mask;
    DWORD start_ticks;
    int failed;
    int card_index;
    int card_count;
    int done;
    int previous_value;
    char debug_message[56];
  } s;

  s.start_ticks = GetTickCount();
  s.failed = 0;
  for (s.card_index = 0; s.card_index < pool_count; ++s.card_index)
  {
    deck[s.card_index] = card_pool[s.card_index];
  }
  s.card_count = pool_count;
  for (s.card_index = 0; s.card_index < free_land_count; ++s.card_index)
  {
    deck[s.card_count] = -(s.card_index % 5 + 1);
    s.card_count = s.card_count + 1;
  }
  s.previous_value = -10000;
  s.done = 0;
  s.land_color_mask = 0x3e;
  if (minimum_size + 10 < s.card_count)
  {
    s.land_color_mask = shell_reduce_sealed_deck_colors(deck, &s.card_count, minimum_size + 10);
  }
  if (g_sealed_deck_report != 0)
  {
    fprintf(g_sealed_deck_report, "\n\nWill build deck from these cards: (%d)\n", s.card_count);
    for (s.report_index = 0; s.report_index < s.card_count; ++s.report_index)
    {
      if (deck[s.report_index] < 0)
      {
        if (deck[s.report_index] == -1)
        {
          fprintf(g_sealed_deck_report, "FREE LAND (swamp)\n");
        }
        else if (deck[s.report_index] == -2)
        {
          fprintf(g_sealed_deck_report, "FREE LAND (island)\n");
        }
        else if (deck[s.report_index] == -3)
        {
          fprintf(g_sealed_deck_report, "FREE LAND (forest)\n");
        }
        else if (deck[s.report_index] == -4)
        {
          fprintf(g_sealed_deck_report, "FREE LAND (mountain)\n");
        }
        else if (deck[s.report_index] == -5)
        {
          fprintf(g_sealed_deck_report, "FREE LAND (plains)\n");
        }
        else
        {
          fprintf(g_sealed_deck_report, "FREE LAND %d\n", deck[s.report_index]);
        }
      }
      else
      {
        fprintf(g_sealed_deck_report, "%s\n",
                global_raw_cards_storage[deck[s.report_index]].full_name);
      }
    }
    fprintf(g_sealed_deck_report, "\n\n");
  }
  if (g_sealed_deck_build_report_enabled != 0)
  {
    g_sealed_deck_build_log = fopen("c:\\Sbuild.txt", "wt");
  }
  else
  {
    g_sealed_deck_build_log = NULL;
  }
  if (g_sealed_deck_build_report_extra_enabled != 0)
  {
    g_sealed_deck_build_extra_log = fopen("c:\\Sbuild2.txt", "wt");
  }
  else
  {
    g_sealed_deck_build_extra_log = NULL;
  }
  if (g_sealed_deck_build_log != 0)
  {
    fprintf(g_sealed_deck_build_log, "aa Building the deck from these %d cards: (min size:%d)\n\n", pool_count, minimum_size);
  }
  if (g_sealed_deck_report != 0)
  {
    g_sealed_deck_build_report_enabled = 1;
    g_sealed_deck_build_log = g_sealed_deck_report;
  }
  s.deck_value = shell_evaluate_sealed_deck(deck, s.card_count, NULL, NULL);
  if (g_sealed_deck_report != 0)
  {
    g_sealed_deck_build_report_enabled = 0;
    g_sealed_deck_build_log = 0;
  }
  if (g_sealed_deck_build_extra_log != 0)
  {
    fprintf(g_sealed_deck_build_extra_log, "Starting deck value: %d\n", s.deck_value);
  }
  while (s.done == 0 && minimum_size < s.card_count && s.failed == 0)
  {
    s.card_index = shell_choose_sealed_card_to_remove(deck, s.card_count, &s.deck_value);
    if (s.card_index == -1)
    {
      s.failed = 1;
    }
    else
    {
      if (s.card_count <= maximum_size && s.previous_value > s.deck_value)
      {
        s.done = 1;
      }
      else
      {
        if ((maximum_size < s.card_count) || ((s.card_count <= maximum_size && (s.previous_value <= s.deck_value))))
        {
          if (g_sealed_deck_build_log != 0)
          {
            fprintf(g_sealed_deck_build_log, "\nTOOK OUT: %s\n",
                    global_raw_cards_storage[deck[s.card_index]].full_name);
          }
          if (g_sealed_deck_build_log != 0)
          {
            fprintf(g_sealed_deck_build_log, "***********************************\n\n");
          }
          if (g_sealed_deck_build_extra_log != 0)
          {
            fprintf(g_sealed_deck_build_extra_log, "TOOK OUT: %s.  NEW DECK VALUE:%d\n",
                    global_raw_cards_storage[deck[s.card_index]].full_name, s.deck_value);
          }
          if (g_sealed_deck_report != 0)
          {
            fprintf(g_sealed_deck_report, "TOOK OUT: %s.  NEW DECK VALUE:%d\n",
                    global_raw_cards_storage[deck[s.card_index]].full_name, s.deck_value);
          }
          s.removed_card = deck[s.card_index];
          deck[s.card_index] = deck[s.card_count + -1];
          --s.card_count;
          s.previous_value = s.deck_value;
          if (s.removed_card == 0xef)
          {
            s.removed_land_color = 1;
            s.land_color_mask &= 0xfffffffd;
          }
          else if (s.removed_card == 0x7e)
          {
            s.removed_land_color = 2;
            s.land_color_mask &= 0xfffffffb;
          }
          else if (s.removed_card == 0x5b)
          {
            s.removed_land_color = 3;
            s.land_color_mask &= 0xfffffff7;
          }
          else if (s.removed_card == 0xa4)
          {
            s.removed_land_color = 4;
            s.land_color_mask &= 0xffffffef;
          }
          else if (s.removed_card == 0xbc)
          {
            s.removed_land_color = 5;
            s.land_color_mask &= 0xffffffdf;
          }
          else
          {
            s.removed_land_color = -1;
          }
          s.free_land_index = -1;
          if (s.removed_land_color != -1)
          {
            for (s.card_index = 0;
                 s.card_index < s.card_count && s.free_land_index == -1;
                 ++s.card_index)
            {
              if (deck[s.card_index] == -s.removed_land_color)
              {
                s.free_land_index = s.card_index;
              }
            }
          }
          if (s.free_land_index != -1)
          {
            s.best_land_value = -10000;
            for (s.card_index = 1; s.card_index <= 5; ++s.card_index)
            {
              if ((s.land_color_mask & (1 << (unsigned char)s.card_index)) != 0)
              {
                deck[s.free_land_index] = -s.card_index;
                s.deck_value = shell_evaluate_sealed_deck(deck, s.card_count, NULL, NULL);
                if (s.best_land_value < s.deck_value)
                {
                  s.best_land_value = s.deck_value;
                  s.best_land_color = s.card_index;
                }
              }
            }
            if (s.deck_value < s.best_land_value)
            {
              deck[s.free_land_index] = -s.best_land_color;
              s.previous_value = s.best_land_value;
              if (g_sealed_deck_build_extra_log != 0)
              {
                if (s.best_land_color == 1)
                {
                  fprintf(g_sealed_deck_build_extra_log, "  Changed a free land to %s. New deck value %d.\n",
                          "SWAMP", s.previous_value);
                }
                else if (s.best_land_color == 2)
                {
                  fprintf(g_sealed_deck_build_extra_log, "  Changed a free land to %s. New deck value %d.\n",
                          "ISLAND", s.previous_value);
                }
                else if (s.best_land_color == 3)
                {
                  fprintf(g_sealed_deck_build_extra_log, "  Changed a free land to %s. New deck value %d.\n",
                          "FOREST", s.previous_value);
                }
                else if (s.best_land_color == 4)
                {
                  fprintf(g_sealed_deck_build_extra_log, "  Changed a free land to %s. New deck value %d.\n",
                          "MOUNTAIN", s.previous_value);
                }
                else if (s.best_land_color == 5)
                {
                  fprintf(g_sealed_deck_build_extra_log, "  Changed a free land to %s. New deck value %d.\n",
                          "PLAINS", s.previous_value);
                }
                else
                {
                  fprintf(g_sealed_deck_build_extra_log, "  Changed a free land to %d. New deck value %d.\n", s.best_land_color,
                          s.previous_value);
                }
              }
              if (g_sealed_deck_report != 0)
              {
                if (s.best_land_color == 1)
                {
                  fprintf(g_sealed_deck_report, "  Changed a free land to %s. New deck value %d.\n",
                          "SWAMP", s.previous_value);
                }
                else if (s.best_land_color == 2)
                {
                  fprintf(g_sealed_deck_report, "  Changed a free land to %s. New deck value %d.\n",
                          "ISLAND", s.previous_value);
                }
                else if (s.best_land_color == 3)
                {
                  fprintf(g_sealed_deck_report, "  Changed a free land to %s. New deck value %d.\n",
                          "FOREST", s.previous_value);
                }
                else if (s.best_land_color == 4)
                {
                  fprintf(g_sealed_deck_report, "  Changed a free land to %s. New deck value %d.\n",
                          "MOUNTAIN", s.previous_value);
                }
                else if (s.best_land_color == 5)
                {
                  fprintf(g_sealed_deck_report, "  Changed a free land to %s. New deck value %d.\n",
                          "PLAINS", s.previous_value);
                }
                else
                {
                  fprintf(g_sealed_deck_report, "  Changed a free land to %d. New deck value %d.\n", s.best_land_color,
                          s.previous_value);
                }
              }
            }
          }
        }
      }
      if (s.card_count <= minimum_size)
      {
        s.done = 1;
      }
    }
  }
  if (g_sealed_deck_build_log != 0)
  {
    fclose(g_sealed_deck_build_log);
  }
  if (g_sealed_deck_build_extra_log != 0)
  {
    fprintf(g_sealed_deck_build_extra_log, "\n\nHere is the final deck (%d cards):\n", s.card_count);
  }
  if (g_sealed_deck_build_extra_log != 0)
  {
    for (s.card_index = 0; s.card_index < s.card_count; ++s.card_index)
    {
      if (deck[s.card_index] < 0)
      {
        if (deck[s.card_index] == -1)
        {
          fprintf(g_sealed_deck_build_extra_log, "FREE LAND (swamp)\n");
        }
        else if (deck[s.card_index] == -2)
        {
          fprintf(g_sealed_deck_build_extra_log, "FREE LAND (island)\n");
        }
        else if (deck[s.card_index] == -3)
        {
          fprintf(g_sealed_deck_build_extra_log, "FREE LAND (forest)\n");
        }
        else if (deck[s.card_index] == -4)
        {
          fprintf(g_sealed_deck_build_extra_log, "FREE LAND (mountain)\n");
        }
        else if (deck[s.card_index] == -5)
        {
          fprintf(g_sealed_deck_build_extra_log, "FREE LAND (plains)\n");
        }
        else
        {
          fprintf(g_sealed_deck_build_extra_log, "FREE LAND %d\n", deck[s.card_index]);
        }
      }
      else
      {
        fprintf(g_sealed_deck_build_extra_log, "%s\n",
                global_raw_cards_storage[deck[s.card_index]].full_name);
      }
    }
  }
  if (g_sealed_deck_build_extra_log != 0)
  {
    fclose(g_sealed_deck_build_extra_log);
  }
  sprintf(s.debug_message, "Built deck: %d sec\n", (GetTickCount() - s.start_ticks) / 1000);
  OutputDebugStringA(s.debug_message);
  if (deck_count != NULL)
  {
    *deck_count = s.card_count;
  }
  return s.failed;
}
