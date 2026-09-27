#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "magic_sealed_tournament.h"
#include "magic_sealed_build.h"
#include "magic_sealed_report.h"
#include "deckdll/src/card_db.h"
#include "global_state.h"
#include "shared_startup.h"

int ParseRarityCsvIntFieldClamped(char **field, int maximum);

// FUNCTION: MAGIC 0x004cce74
int shell_write_sealed_deck_build_report(void)
{
  struct
  {
    char card_filename[264];
    HANDLE card_file;
    DWORD bytes_read;
    DWORD file_size;
    char *file_buffer;
    char *file_end;
    char *cursor;
    sealed_deck_tournament_t tournament;
    sealed_deck_pack_t packs[10];
    int free_lands;
    DWORD build_time;
    int card_pool_count;
    int pack_index;
    int card_index;
    int deck[500];
    int card_pool[2000];
    int minimum_size;
    int generate_packs;
    FILE *report;
    int deck_count;
  } s;

  g_sealed_deck_extra_card_limit = 10;
  s.generate_packs = 1;
  s.report = fopen("c:\\Spack.txt", "wt");
  g_sealed_deck_report = s.report;
  if (s.generate_packs != 0)
  {
    fprintf(s.report, "These are the packs (from which the deck will be built)\n\n");
    s.tournament.player_count = 1 << (byte)g_sealed_deck_options.rounds;
    s.tournament.rounds = g_sealed_deck_options.rounds;
    s.tournament.ante = g_sealed_deck_options.ante;
    s.tournament.best_of = g_sealed_deck_options.best_of;
    s.tournament.starter_count = g_sealed_deck_options.starter_count;
    for (s.card_index = 0; s.card_index < g_sealed_deck_options.starter_count; ++s.card_index)
      s.tournament.starter_types[s.card_index] = g_sealed_deck_options.starter_packs[s.card_index];
    s.tournament.booster_count = g_sealed_deck_options.booster_count;
    for (s.card_index = 0; s.card_index < g_sealed_deck_options.booster_count; ++s.card_index)
      s.tournament.booster_types[s.card_index] = g_sealed_deck_options.booster_packs[s.card_index];
    s.tournament.free_lands = g_sealed_deck_options.free_lands;
    s.tournament.minimum_deck_size = g_sealed_deck_options.minimum_deck_size;
    shell_generate_sealed_packs(s.packs, s.card_pool, &s.card_pool_count,
        s.tournament.starter_count, s.tournament.starter_types,
        s.tournament.booster_count, s.tournament.booster_types);
    s.card_pool_count = 0;
    for (s.pack_index = 0;
        s.pack_index < s.tournament.starter_count + s.tournament.booster_count; ++s.pack_index)
    {
      fprintf(s.report, "\n\n%s %s PACK:\n\n", g_sealed_pack_definitions[s.packs[s.pack_index].pack_type].name,
          s.packs[s.pack_index].is_starter != 0 ? "STARTER" : "BOOSTER");
      for (s.card_index = 0; s.card_index < s.packs[s.pack_index].card_count; ++s.card_index)
      {
        fprintf(s.report, "%3d, %s\n", s.packs[s.pack_index].cards[s.card_index],
            global_raw_cards_storage[s.packs[s.pack_index].cards[s.card_index]].full_name);
        s.card_pool[s.card_pool_count] = s.packs[s.pack_index].cards[s.card_index];
        ++s.card_pool_count;
      }
    }
    s.free_lands = s.tournament.free_lands;
    fprintf(s.report, "\nFree Lands:%d\n", s.free_lands);
    s.minimum_size = s.tournament.minimum_deck_size;
  }
  else
  {
    fprintf(s.report, "These are the cards read from SCARDS.TXT (from which the deck will be built)\n\n");
    strcpy(s.card_filename, "c:/Scards.txt");
    s.card_file = CreateFileA(s.card_filename, GENERIC_READ, FILE_SHARE_READ, NULL,
        OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN | FILE_ATTRIBUTE_NORMAL, NULL);
    if (s.card_file != INVALID_HANDLE_VALUE)
    {
      s.file_size = GetFileSize(s.card_file, NULL);
      s.file_buffer = malloc(s.file_size + 1);
      if (s.file_buffer != NULL)
      {
        ReadFile(s.card_file, s.file_buffer, s.file_size, &s.bytes_read, NULL);
        s.cursor = s.file_buffer;
        s.file_end = s.cursor + s.bytes_read;
        s.card_pool_count = 0;
        while (s.cursor < s.file_end)
        {
          if (*s.cursor >= '0' && *s.cursor <= '9')
          {
            s.card_pool[s.card_pool_count] = ParseRarityCsvIntFieldClamped(&s.cursor, 9999);
            ++s.card_pool_count;
          }
          if (*s.cursor != '\n')
            s.cursor = strchr(s.cursor, '\n') + 1;
        }
        free(s.file_buffer);
      }
      CloseHandle(s.card_file);
    }
    s.free_lands = 5;
    /* The original uses %s for the integer free-land count in this branch. */
    fprintf(s.report, "\nFree Lands:%s\n", s.free_lands);
    s.minimum_size = 60;
  }
  s.build_time = GetTickCount();
  shell_build_sealed_deck(s.card_pool, s.card_pool_count, s.free_lands, 0, 0,
      s.minimum_size, s.minimum_size + g_sealed_deck_extra_card_limit, s.deck, &s.deck_count);
  s.build_time = GetTickCount() - s.build_time;
  fprintf(s.report, "\n\n-------------------------------\n");
  fprintf(s.report, "AI BUILT THIS DECK (%d cards):\n", s.deck_count);
  fprintf(s.report, "<<minimum size:%d  max:%d>>\n", s.minimum_size,
      s.minimum_size + g_sealed_deck_extra_card_limit);
  fprintf(s.report, "time to build deck: %d seconds\n", s.build_time / 1000);
  fprintf(s.report, "-------------------------------\n\n");
  for (s.card_index = 0; s.card_index < s.deck_count; ++s.card_index)
  {
    if (s.deck[s.card_index] < 0)
    {
      if (s.deck[s.card_index] == -1)
        fprintf(s.report, "    FREE LAND (swamp)\n");
      else if (s.deck[s.card_index] == -2)
        fprintf(s.report, "    FREE LAND (island)\n");
      else if (s.deck[s.card_index] == -3)
        fprintf(s.report, "    FREE LAND (forest)\n");
      else if (s.deck[s.card_index] == -4)
        fprintf(s.report, "    FREE LAND (mountain)\n");
      else if (s.deck[s.card_index] == -5)
        fprintf(s.report, "    FREE LAND (plains)\n");
      else
        fprintf(s.report, "    FREE LAND %d\n", s.deck[s.card_index]);
    }
    else
      fprintf(s.report, "%3d, %s\n", s.deck[s.card_index],
          global_raw_cards_storage[s.deck[s.card_index]].full_name);
  }
  fclose(s.report);
  WinExec("Write c:/spack.txt", SW_SHOW);
  if (g_sealed_deck_build_report_extra_enabled != 0)
    WinExec("Write c:/sbuild2.txt", SW_SHOW);
  if (g_sealed_deck_build_report_enabled != 0)
    WinExec("Write c:/sbuild.txt", SW_SHOW);
  return 0;
}
