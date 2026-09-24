#include "magic_shell_dialogs.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "global_other.h"
#include "global_state.h"
#include "global_strings.h"
#include "shared_startup.h"

extern HWND global_main_hwnd;
extern char g_exp1_art_path[0x110];
int initialize_duel_engine_window(void);
void shutdown_duel_engine_window(void);

// GLOBAL: MAGIC 0x008ce910
static char shell_gauntlet_opponent_deck_path[264];

// FUNCTION: MAGIC 0x00497b23
static DWORD shell_format_gauntlet_match_text(char *buffer, DWORD capacity,
                                              const char *format, ...)
{
  struct
  {
    DWORD length;
    va_list arguments;
    char *carriage_return;
  } s;

  va_start(s.arguments, format);
  s.length = FormatMessageA(FORMAT_MESSAGE_FROM_STRING, format, 0, 0,
                            buffer, capacity, &s.arguments);
  while ((s.carriage_return = strchr(buffer, '\r')) != NULL)
    strcpy(s.carriage_return, s.carriage_return + 1);
  va_end(s.arguments);
  return s.length;
}

// FUNCTION: MAGIC 0x0050ade6
static int shell_execute_gauntlet_match(int resume, int best_of,
                                         int allow_sideboarding, int arg4,
                                         int arg5, int arg6, void *arg7,
                                         int arg8)
{
  (void)resume;
  (void)best_of;
  (void)allow_sideboarding;
  (void)arg4;
  (void)arg5;
  (void)arg6;
  (void)arg7;
  (void)arg8;
  return -2;
}

// FUNCTION: MAGIC 0x004647fe
static int shell_read_duel_deck(char *filename, int player,
                                unsigned int flags, int unknown)
{
  int card_index;

  for (card_index = 0; card_index < 200; ++card_index)
  {
    g_initial_library[player][card_index].numcards = 0;
    g_initial_library[player][card_index].csvid =
        g_initial_library[player][card_index].numcards;
  }
  return shell_parse_duel_deck(filename, g_initial_library[player],
                               flags, unknown);
}

// FUNCTION: MAGIC 0x0050d4aa
int shell_load_gauntlet_decks(char *player_deck, char *opponent_deck)
{
  struct
  {
    int opponent_error_line;
    int player_error_line;
    int result;
    int card_index;
    char message[500];
  } s;

  s.result = 1;
  if (player_deck != NULL)
  {
    g_selected_wizard_color = 0;
    _PlayerFace = shell_read_duel_deck(player_deck,
                                       g_selected_wizard_color, 1, -1);
    for (s.card_index = 0; s.card_index < 200; ++s.card_index)
    {
      ((csvid_and_numcards *)g_opponent_deck_cards)[s.card_index].csvid =
          g_initial_library[g_selected_wizard_color][s.card_index].csvid;
      ((csvid_and_numcards *)g_opponent_deck_cards)[s.card_index].numcards =
          g_initial_library[g_selected_wizard_color][s.card_index].numcards;
    }
    if (_PlayerFace <= 0)
    {
      s.result = 0;
      LoadTextSectionLines(global_ui_strings_filename, "GAUNTLETERRORS");
      if (_PlayerFace == 0)
        s.player_error_line = 1;
      else if (_PlayerFace == -1)
        s.player_error_line = 2;
      else if (_PlayerFace == -2)
        s.player_error_line = 3;
      else if (_PlayerFace == -3)
        s.player_error_line = 4;
      else if (_PlayerFace == -4)
        s.player_error_line = 5;
      else
        s.player_error_line = 0;
      if (s.player_error_line < 2)
        strcpy(s.message, g_text_lines[s.player_error_line]);
      else
        sprintf(s.message, g_text_lines[s.player_error_line], player_deck);
      MessageBoxA(global_main_hwnd, s.message, g_text_lines[0], MB_ICONERROR);
    }
  }
  if ((g_duel_network_flags & 2) == 0 && opponent_deck != NULL)
  {
    g_opponent_initial_library_index = 1;
    _OpponFace = shell_read_duel_deck(opponent_deck, 1, 1, -1);
    for (s.card_index = 0; s.card_index < 200; ++s.card_index)
    {
      ((csvid_and_numcards *)g_player_deck_cards)[s.card_index].csvid =
          g_initial_library[g_opponent_initial_library_index]
                           [s.card_index].csvid;
      ((csvid_and_numcards *)g_player_deck_cards)[s.card_index].numcards =
          g_initial_library[g_opponent_initial_library_index]
                           [s.card_index].numcards;
    }
    if (_OpponFace <= 0)
    {
      s.result = 0;
      LoadTextSectionLines(global_ui_strings_filename, "GAUNTLETERRORS");
      if (_OpponFace == 0)
        s.opponent_error_line = 6;
      else if (_OpponFace == -1)
        s.opponent_error_line = 7;
      else if (_OpponFace == -2)
        s.opponent_error_line = 8;
      else if (_OpponFace == -3)
        s.opponent_error_line = 9;
      else if (_OpponFace == -4)
        s.opponent_error_line = 10;
      else
        s.opponent_error_line = 0;
      if (s.opponent_error_line == 0)
        strcpy(s.message, g_text_lines[0]);
      else
        sprintf(s.message, g_text_lines[s.opponent_error_line],
                opponent_deck);
      MessageBoxA(global_main_hwnd, s.message, g_text_lines[0], MB_ICONERROR);
    }
  }
  return s.result;
}

// FUNCTION: MAGIC 0x0050c2fc
int shell_run_gauntlet(int resume, int match_count, HWND opponent_deck_list)
{
  struct
  {
    char matchup_text[100];
    WPARAM deck_selection;
    int resume_match;
    int current_match;
    int stop;
    int duel_result;
  } s;

  if (resume != 0)
  {
    g_duel_run_mode = 2;
    if (initialize_duel_engine_window() == 0)
      return -2;
  }
  if (resume == 0)
  {
    g_duel_options_state = 0;
    g_current_spell_internal_card_id = 0;
    g_duel_state_00925bf8 = 0;
    g_duel_state_00777c10 = 0;
    g_duel_state_00777c08 = 0;
  }
  shell_load_rogue_profiles();
#ifdef MODERN_FIXES
  if (shell_rogue_count <= 0)
    return -2;
#endif
  s.stop = 0;
  for (s.current_match = g_duel_state_00925bf8;
       s.current_match < match_count && s.stop == 0;
       ++s.current_match)
  {
    if (resume != 0 && g_duel_state_00777c08 != 0)
    {
      s.resume_match = 1;
      resume = 0;
    }
    else
    {
      s.resume_match = 0;
      resume = 0;
      s.deck_selection = (WPARAM)-1;
      if (opponent_deck_list != NULL)
      {
        s.deck_selection = SendMessageA(opponent_deck_list, CB_GETCOUNT,
                                        0, 0);
        if (s.deck_selection != 0)
        {
          s.deck_selection = rand() % (int)s.deck_selection;
          SendMessageA(opponent_deck_list, CB_SETCURSEL,
                       s.deck_selection, 0);
        }
        else
          s.deck_selection = (WPARAM)-1;
      }
      if (s.deck_selection == (WPARAM)-1)
        strcpy(shell_gauntlet_opponent_deck_path,
               shell_gauntlet_player_deck_path);
      else
      {
        SendMessageA(opponent_deck_list, CB_GETLBTEXT, s.deck_selection,
                     (LPARAM)g_saved_player_name);
        sprintf(shell_gauntlet_opponent_deck_path, "%s\\%s.dck",
                global_playdeck_path, g_saved_player_name);
      }
      shell_load_gauntlet_decks(NULL, shell_gauntlet_opponent_deck_path);
      shell_load_rogue_profiles();
      s.deck_selection = rand() % shell_rogue_count;
      sprintf((char *)g_duel_state_block_008ce570, "%s\\Rogues\\%s",
              g_exp1_art_path,
              shell_rogue_profiles[s.deck_selection].face_name);
      strcpy(g_saved_player_name,
             shell_rogue_profiles[s.deck_selection].name);
      g_shandalar_difficulty =
          shell_rogue_profiles[s.deck_selection].difficulty;
    }
    if (s.resume_match == 0)
    {
      load_text(global_ui_strings_filename,
                "DIALOG_STARTEXP1MATCH_GAUNTLET");
      if (g_duel_state_00925bf8 == 0)
        strcpy(s.matchup_text, g_text_lines[0]);
      else if (match_count - 1 == g_duel_state_00925bf8)
        strcpy(s.matchup_text, g_text_lines[2]);
      else
        shell_format_gauntlet_match_text(s.matchup_text, 100,
                                        g_text_lines[1],
                                        s.current_match + 1, match_count);
      if (g_duel_state_00925bf8 == 0)
      {
        shell_show_gauntlet_matchup(
            global_main_hwnd, s.matchup_text, g_player_name,
            shell_gauntlet_player_deck_path,
            (char *)g_duel_state_block_008b3fc0, g_saved_player_name,
            shell_gauntlet_opponent_deck_path,
            (char *)g_duel_state_block_008ce570, 0);
        PostMessageA(global_main_hwnd, 0x496, 0, 0);
        g_duel_run_mode = 2;
        if (initialize_duel_engine_window() == 0)
          return -2;
      }
      else
        shell_show_gauntlet_matchup(
            g_duel_window_hwnd, s.matchup_text, g_player_name,
            shell_gauntlet_player_deck_path,
            (char *)g_duel_state_block_008b3fc0, g_saved_player_name,
            shell_gauntlet_opponent_deck_path,
            (char *)g_duel_state_block_008ce570, 0);
    }
    g_duel_state_00777c08 = 1;
    s.duel_result = shell_execute_gauntlet_match(
        s.resume_match, g_gauntlet_options.best_of,
        g_gauntlet_options.allow_sideboarding, 0, 0, 0, NULL, 0);
    g_duel_state_00777c08 = 0;
    ++g_duel_state_00925bf8;
    if (s.duel_result == -1)
      g_duel_state_00777c10 += g_shandalar_difficulty * 5;
    else if (s.duel_result == 0)
    {
      g_duel_state_00777c10 -= g_shandalar_difficulty * 5;
      ++g_current_spell_internal_card_id;
    }
    else if (s.duel_result == 1)
    {
      g_duel_state_00777c10 += g_shandalar_difficulty * 0xf;
      ++g_duel_options_state;
    }
    if (s.duel_result == 0)
      s.stop = 1;
    if (s.duel_result == -2)
      s.stop = 1;
  }
  shutdown_duel_engine_window();
  return g_duel_state_00777c10;
}
