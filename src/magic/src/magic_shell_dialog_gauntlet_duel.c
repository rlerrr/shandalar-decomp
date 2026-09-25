#include "magic_shell_dialogs.h"
#include <commdlg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "duel_engine.h"
#include "game_support.h"
#include "global_other.h"
#include "global_state.h"
#include "global_strings.h"
#include "magic_shell.h"
#include "manalinkinterface/manalinkinterface.h"
#include "network.h"
#include "deckdll/src/magsnd.h"

extern HWND global_main_hwnd;
extern char g_default_screen_name[0x358];
extern void *g_duel_player_face_pic;
extern int g_duel_main_window_closing;
extern HANDLE global_mutex_GameInit;
unsigned int HasExpansion(unsigned char expansion_mask);
__declspec(dllimport) int __cdecl CheckDoPCD(int unused, int *tries,
                                             int drive_path);
__declspec(dllimport) int __cdecl CheckSotaCD(int unused, int *tries,
                                              int drive_path);
__declspec(dllimport) BOOL __cdecl IsCDDrive(int drive_path);
__declspec(dllimport) int Autoplay_ShutDown(void);
__declspec(dllimport) BOOL Autoplay_Restore(void);
WPARAM run_duel_engine_message_loop(void);
int end_the_game(int duel_result, char *match_progress, int match_finished,
                 char *sideboard_button, void (*sideboard_callback)(HWND),
                 char *save_button, void (*save_callback)(HWND));
void save_soloduel(char *path);
void save_gauntlet(char *path);
void save_sealeddeck(char *path);
void SetCardInDeck(int deck_index, int enabled);
WPARAM WINAPI DeckBuilderMain(HWND parent_hwnd, int db_flags_1,
                              int db_flags_2);
void prepare_duel_video_mode_transition(void);
void finish_duel_video_mode_transition(void);

typedef struct shell_sealed_deck_editor_state_t
{
  unsigned char reserved_0000[0x300];
  int generator_option_300;
  int generator_option_304;
  unsigned char reserved_0308[0x93c - 0x308];
  int card_pool[300];
  int card_pool_count;
  int selected_cards[300];
  int selected_count;
} shell_sealed_deck_editor_state_t;

// FUNCTION: MAGIC 0x0048fadd
void shell_save_match_screen_name_profile(void)
{
  struct
  {
    FILE *file;
    char filename[100];
  } s;

  if (strcmp(g_screen_name_profile.screen_name, g_default_screen_name) == 0)
    return;
  sprintf(s.filename, "ScreenNames\\%s.scn", g_screen_name_profile.screen_name);
  SetFileAttributesA(s.filename, FILE_ATTRIBUTE_NORMAL);
  s.file = fopen(s.filename, "wb");
  fwrite(&g_screen_name_profile, 0x748, 1, s.file);
  fclose(s.file);
  SetFileAttributesA("ScreenNames\\ActiveName.dat", FILE_ATTRIBUTE_NORMAL);
  s.file = fopen("ScreenNames\\ActiveName.dat", "wb");
  fwrite(g_screen_name_profile.screen_name, 0xe, 1, s.file);
  fclose(s.file);
}

// FUNCTION: MAGIC 0x0050aa80
void shell_show_solo_sideboard_notice(void)
{
  MessageBoxA(global_main_hwnd, "Solo Duel Sideboard", "", 0);
}

// FUNCTION: MAGIC 0x0050aaa3
void shell_show_gauntlet_sideboard_notice(void)
{
  MessageBoxA(global_main_hwnd, "Gauntlet Sideboard", "", 0);
}

// FUNCTION: MAGIC 0x004c5991
void shell_edit_sealed_deck(HWND owner, shell_sealed_deck_editor_state_t *state)
{
  struct
  {
    int selected_cards[300];
    int selected_index;
    int pool_index;
    int selected_count;
    int found;
  } s;

  for (s.pool_index = 0; s.pool_index < state->selected_count; ++s.pool_index)
    s.selected_cards[s.pool_index] = state->selected_cards[s.pool_index];
  s.selected_count = state->selected_count;
  for (s.pool_index = 0; s.pool_index < state->card_pool_count; ++s.pool_index)
  {
    deck[s.pool_index] = CardTypeFromID(state->card_pool[s.pool_index]);
    s.selected_index = 0;
    s.found = 0;
    for (; s.selected_index < s.selected_count && s.found == 0;
         ++s.selected_index)
    {
      if (state->card_pool[s.pool_index] == s.selected_cards[s.selected_index])
      {
        s.found = 1;
        s.selected_cards[s.selected_index] = -1;
      }
      SetCardInDeck(s.pool_index, 1);
      if (s.found != 0)
        deck[s.pool_index] |= 0x10000;
    }
  }
  deck[s.pool_index] = -1;
  _currentDeck = 0;
  DeckBuilderMain(owner, 0x40, 1);
  state->selected_count = 0;
  for (s.pool_index = 0; s.pool_index < state->card_pool_count; ++s.pool_index)
  {
    if ((deck[s.pool_index] & 0x10000) != 0)
    {
      state->selected_cards[state->selected_count] =
          CardIDFromType(deck[s.pool_index]);
      ++state->selected_count;
    }
  }
}

// FUNCTION: MAGIC 0x0050aac6
void shell_edit_sealed_sideboard(HWND owner)
{
  shell_sealed_deck_editor_state_t *state;
  int card_index;

  state = (shell_sealed_deck_editor_state_t *)g_savegame_data_pointer;
  prepare_duel_video_mode_transition();
  sound_close();
  shell_edit_sealed_deck(owner, state);
  init_sound_dll(g_duel_window_hwnd, 0, 0);
  finish_duel_video_mode_transition();
  g_selected_wizard_color = 0;
  for (card_index = 0; card_index < 200; ++card_index)
  {
    if (card_index < state->selected_count)
    {
      ((csvid_and_numcards *)g_opponent_deck_cards)[card_index].csvid =
          state->selected_cards[card_index];
      g_initial_library[g_selected_wizard_color][card_index].csvid =
          ((csvid_and_numcards *)g_opponent_deck_cards)[card_index].csvid;
      ((csvid_and_numcards *)g_opponent_deck_cards)[card_index].numcards = 1;
      g_initial_library[g_selected_wizard_color][card_index].numcards =
          ((csvid_and_numcards *)g_opponent_deck_cards)[card_index].numcards;
    }
    else
    {
      ((csvid_and_numcards *)g_opponent_deck_cards)[card_index].csvid = -1;
      g_initial_library[g_selected_wizard_color][card_index].csvid =
          ((csvid_and_numcards *)g_opponent_deck_cards)[card_index].csvid;
      ((csvid_and_numcards *)g_opponent_deck_cards)[card_index].numcards = 0;
      g_initial_library[g_selected_wizard_color][card_index].numcards =
          ((csvid_and_numcards *)g_opponent_deck_cards)[card_index].numcards;
    }
  }
}

// FUNCTION: MAGIC 0x0050ac1c
void shell_save_solo_duel(HWND owner)
{
  shell_prepare_save_game_dialog(1);
  g_duel_save_game_openfilename.hwndOwner = owner;
  g_duel_save_game_openfilename.lpstrTitle =
      gs_window_title_save_game_008951d0;
  g_duel_save_game_openfilename.Flags = 0x2a000c;
  if (GetSaveFileNameA(&g_duel_save_game_openfilename) != 0)
    save_soloduel(g_duel_save_game_openfilename.lpstrFile);
}

// FUNCTION: MAGIC 0x0050ac6d
void shell_save_gauntlet_duel(HWND owner)
{
  shell_prepare_save_game_dialog(2);
  g_duel_save_game_openfilename.hwndOwner = owner;
  g_duel_save_game_openfilename.lpstrTitle =
      gs_window_title_save_game_008951d0;
  g_duel_save_game_openfilename.Flags = 0x2a000c;
  if (GetSaveFileNameA(&g_duel_save_game_openfilename) != 0)
    save_gauntlet(g_duel_save_game_openfilename.lpstrFile);
}

// FUNCTION: MAGIC 0x0050acbe
void shell_save_sealed_duel(HWND owner)
{
  shell_prepare_save_game_dialog(3);
  g_duel_save_game_openfilename.hwndOwner = owner;
  g_duel_save_game_openfilename.lpstrTitle =
      gs_window_title_save_game_008951d0;
  g_duel_save_game_openfilename.Flags = 0x2a000c;
  if (GetSaveFileNameA(&g_duel_save_game_openfilename) != 0)
    save_sealeddeck(g_duel_save_game_openfilename.lpstrFile);
}

// FUNCTION: MAGIC 0x0050ade6
int shell_execute_gauntlet_match(int resume, int best_of,
                                int allow_sideboarding, int ante_enabled,
                                int match_limit, int minimum_deck_size,
                                void (*ante_callback)(void), int ranked)
{
  struct
  {
    int sealed_dialog_card_count;
    char *sealed_dialog_text;
    int sealed_dialog_state;
    int sealed_dialog_card_limit;
    HANDLE sealed_dialog_thread;
    int sealed_dialog_reserved;
    char sealed_editing_text[200];
    int sealed_editing_line_count;
    unsigned int sealed_thread_id;
    HANDLE sealed_thread;
    char warning_text[300];
    int ante_card_index;
    int ante_slot;
    unsigned char *ante_losing_deck;
    unsigned char *ante_winning_deck;
    int (*ante_card_list)[16];
    int ante_card_found;
    int cd_result_sota;
    int cd_result_dop;
    int cd_found;
    int cd_prompt_choice;
    int cd_retry_result;
    char drive_letter;
    char drive_padding[3];
    char cd_path[4];
    int cd_tries;
    int dialog_result;
    screen_name_file_t *profile;
    int opponent_deck_valid;
    int deck_count;
    int profile_state;
    char sideboard_button[32];
    unsigned int player_deck_valid;
    unsigned int has_dop;
    char save_button[32];
    int card_index;
    void (*save_callback)(HWND);
    int match_finished;
    int match_quit;
    int duel_result;
    char progress_text[300];
    void (*sideboard_callback)(HWND);
    unsigned int has_sota;
  } s;

  s.profile_state = 1;
  s.has_sota = HasExpansion(2);
  s.has_dop = HasExpansion(4);
  if (HasExpansion(8) == 0)
  {
    s.cd_found = 0;
    strcpy(s.cd_path, "c:");
    for (s.drive_letter = 'D';
         s.drive_letter < 'Z' && s.cd_found == 0;
         ++s.drive_letter)
    {
      s.cd_path[0] = s.drive_letter;
      if (IsCDDrive((int)s.cd_path) != 0)
        s.cd_found = 1;
    }
    if (s.has_dop != 0)
    {
      if (s.cd_found == 0)
        s.cd_result_dop = 0;
      else
      {
        s.cd_tries = 1;
        s.cd_result_dop = CheckDoPCD(0, &s.cd_tries, (int)s.cd_path);
        s.cd_retry_result = s.cd_result_dop;
        while (s.cd_retry_result == 0)
        {
          Autoplay_ShutDown();
          load_text(global_ui_strings_filename, "CHECKCD");
          s.cd_prompt_choice = MessageBoxA(GetFocus(), g_text_lines[1],
                                           gs_magic_the_gathering_title_00789460,
                                           0x2031);
          if (s.cd_prompt_choice == IDCANCEL)
            s.cd_retry_result = 1;
          else
          {
            s.cd_tries = 5;
            s.cd_result_dop = CheckDoPCD(0, &s.cd_tries, (int)s.cd_path);
            s.cd_retry_result = s.cd_result_dop;
          }
          Autoplay_Restore();
        }
      }
      if (s.cd_result_dop == 0)
      {
        if ((g_duel_network_flags & 2) != 0)
        {
          FamInterface_EndSession();
          FamInterface_Flush();
        }
        return 0;
      }
    }
    else if (s.has_sota != 0)
    {
      if (s.cd_found == 0)
        s.cd_result_sota = 0;
      else
      {
        s.cd_tries = 1;
        s.cd_result_sota = CheckSotaCD(0, &s.cd_tries, (int)s.cd_path);
        s.cd_retry_result = s.cd_result_sota;
        while (s.cd_retry_result == 0)
        {
          Autoplay_ShutDown();
          load_text(global_ui_strings_filename, "CHECKCD");
          s.cd_prompt_choice = MessageBoxA(GetFocus(), g_text_lines[0],
                                           gs_magic_the_gathering_title_00789460,
                                           0x2031);
          if (s.cd_prompt_choice == IDCANCEL)
            s.cd_retry_result = 1;
          else
          {
            s.cd_tries = 5;
            s.cd_result_sota = CheckSotaCD(0, &s.cd_tries, (int)s.cd_path);
            s.cd_retry_result = s.cd_result_sota;
          }
          Autoplay_Restore();
        }
      }
      if (s.cd_result_sota == 0)
      {
        if ((g_duel_network_flags & 2) != 0)
        {
          FamInterface_EndSession();
          FamInterface_Flush();
        }
        return 0;
      }
    }
  }

  g_duel_mode_flags = 4;
  g_main_window_hwnd = NULL;
  g_duel_player_face_pic = NULL;
  g_duel_creature_type = -1;
  g_duel_network_state = 0;
  if (best_of < 1)
  {
    if ((g_duel_network_flags & 2) == 0)
    {
      g_duel_player_face_pic = NULL;
      g_main_window_hwnd = NULL;
      g_duel_creature_type = -1;
      g_duel_mode_flags = 4;
      g_duel_network_state = 0;
    }
    /* TODO: decompile network match setup at MAGIC 0x0048c5a1. */
    return -2;
  }

  if (resume == 0)
  {
    g_opponent_starting_card_id_1 = -1;
    g_opponent_starting_card_id_2 = -1;
    g_duel_extra_turn_player = -1;
    g_duel_state_008cee6c = 0;
    g_ante_card_count = 0;
    g_creature_count_total = 0;
    g_opponent_deck_color_filter_count = -1;
  }
  load_active_screen_name_profile();

  s.profile = &g_screen_name_profile;
  s.sideboard_callback = NULL;
  if (allow_sideboarding != 0)
  {
    if (g_duel_run_mode == 1)
      s.sideboard_callback = (void (*)(HWND))shell_show_solo_sideboard_notice;
    else if (g_duel_run_mode == 2)
      s.sideboard_callback = (void (*)(HWND))shell_show_gauntlet_sideboard_notice;
    else if (g_duel_run_mode == 3)
      s.sideboard_callback = shell_edit_sealed_sideboard;
  }
  s.save_callback = NULL;
  if (g_duel_run_mode == 1 && (g_duel_network_flags & 2) == 0)
    s.save_callback = shell_save_solo_duel;
  else if (g_duel_run_mode == 2)
    s.save_callback = shell_save_gauntlet_duel;
  else if (g_duel_run_mode == 3)
    s.save_callback = shell_save_sealed_duel;

  s.match_finished = 0;
  s.match_quit = 0;
  do
  {
    if (s.match_finished != 0 || s.match_quit != 0)
    {
      if (s.duel_result == -2)
        return -2;
      if (s.match_quit != 0 && s.match_finished == 0)
        return 0;
      if (g_ante_card_count < g_duel_state_008cee6c)
        return 1;
      if (g_duel_state_008cee6c < g_ante_card_count)
        return 0;
      return -1;
    }

    if (resume == 0 || g_duel_active != 0)
    {
      if (resume == 0 || g_duel_active == 0)
      {
        g_life[1] = 20;
        g_life[0] = 20;
        deck[0] = -1;
        g_selected_wizard_color = 0;
        g_opponent_initial_library_index = 1;
        for (s.card_index = 0; s.card_index < 200; ++s.card_index)
        {
          g_initial_library[g_selected_wizard_color][s.card_index].csvid =
              ((csvid_and_numcards *)g_opponent_deck_cards)[s.card_index].csvid;
          g_initial_library[g_selected_wizard_color][s.card_index].numcards =
              ((csvid_and_numcards *)g_opponent_deck_cards)[s.card_index].numcards;
          g_initial_library[g_opponent_initial_library_index][s.card_index].csvid =
              ((csvid_and_numcards *)g_player_deck_cards)[s.card_index].csvid;
          g_initial_library[g_opponent_initial_library_index][s.card_index].numcards =
              ((csvid_and_numcards *)g_player_deck_cards)[s.card_index].numcards;
        }
        g_duel_ai_mode_state = 0;
        g_duel_use_previous_backdrop_colors = 0;
        g_duel_state_00896718 = g_opponent_deck_color_filter_count;
        s.deck_count = 0;
        for (s.card_index = 0; s.card_index < 200; ++s.card_index)
          if (((csvid_and_numcards *)g_opponent_deck_cards)[s.card_index].csvid != -1)
            s.deck_count +=
                ((csvid_and_numcards *)g_opponent_deck_cards)[s.card_index].numcards;
        s.player_deck_valid = minimum_deck_size <= s.deck_count;
        s.deck_count = 0;
        for (s.card_index = 0; s.card_index < 200; ++s.card_index)
          if (((csvid_and_numcards *)g_player_deck_cards)[s.card_index].csvid != -1)
            s.deck_count +=
                ((csvid_and_numcards *)g_player_deck_cards)[s.card_index].numcards;
        s.opponent_deck_valid = minimum_deck_size <= s.deck_count;
      }
      else
      {
        g_duel_ai_mode_state = -10;
        g_duel_use_previous_backdrop_colors = 1;
        resume = 0;
        s.player_deck_valid = 1;
        s.opponent_deck_valid = 1;
      }
      if (s.player_deck_valid != 0 && s.opponent_deck_valid != 0)
        s.duel_result = run_duel_engine_message_loop();
      if (s.duel_result == 3)
      {
        FamInterface_PostDuelResult(4);
        return 3;
      }
      if (s.player_deck_valid == 0)
      {
        if (s.opponent_deck_valid != 0)
          s.duel_result = 0;
        else
          s.duel_result = -1;
      }
      else if (s.opponent_deck_valid == 0)
        s.duel_result = 1;
      if (g_duel_main_window_closing == 0)
      {
        if (s.duel_result == 1)
          FamInterface_PostDuelResult(0);
        else if (s.duel_result == 0)
          FamInterface_PostDuelResult(1);
        else
          FamInterface_PostDuelResult(2);
      }
      else
        FamInterface_PostDuelResult(3);
      ++g_creature_count_total;
      if (s.duel_result == 1)
        ++g_duel_state_008cee6c;
      else if (s.duel_result == 0)
        ++g_ante_card_count;
      if (best_of == 99)
        s.match_finished = 0;
      else if ((best_of + 1) / 2 <= g_duel_state_008cee6c ||
               (best_of + 1) / 2 <= g_ante_card_count)
        s.match_finished = 1;
      if (match_limit != 0 && best_of <= g_creature_count_total)
        s.match_finished = 1;
      if (s.duel_result == 1)
        g_opponent_deck_color_filter_count = 1;
      else if (s.duel_result == 0)
        g_opponent_deck_color_filter_count = 0;
      if (s.profile != NULL)
      {
        /* TODO: compute network DCI rank at MAGIC 0x00497b9a. */
        if (s.duel_result == 1)
        {
          if (ranked != 0)
            ++s.profile->dci_wins;
          ++s.profile->mp_wins;
        }
        else if (s.duel_result == 0)
        {
          if (ranked != 0)
            ++s.profile->dci_losses;
          ++s.profile->mp_losses;
        }
        else
          ++s.profile->mp_draws;
        shell_save_match_screen_name_profile();
      }
    }
    else
    {
      strcpy(s.progress_text, "");
      s.duel_result = -2;
      resume = 0;
      s.player_deck_valid = 1;
      s.opponent_deck_valid = 1;
    }

    if (ante_enabled != 0 && g_duel_state_00789104 != 0)
    {
      if (s.duel_result == 0)
      {
        s.ante_card_list = &global_ante_cards[0];
        s.ante_winning_deck = g_player_deck_cards;
        s.ante_losing_deck = g_opponent_deck_cards;
      }
      else if (s.duel_result == 1)
      {
        s.ante_card_list = &global_ante_cards[1];
        s.ante_winning_deck = g_opponent_deck_cards;
        s.ante_losing_deck = g_player_deck_cards;
      }
      if (s.duel_result == 0 || s.duel_result == 1)
      {
        for (s.ante_slot = 0; s.ante_slot < 16; ++s.ante_slot)
        {
          if ((*s.ante_card_list)[s.ante_slot] != -1)
          {
            s.ante_card_index = 0;
            s.ante_card_found = 0;
            while (s.ante_card_index < 200 && s.ante_card_found == 0)
            {
              if (((csvid_and_numcards *)s.ante_winning_deck)[s.ante_card_index].csvid == -1 ||
                  ((csvid_and_numcards *)s.ante_winning_deck)[s.ante_card_index].numcards == 0)
              {
                ((csvid_and_numcards *)s.ante_winning_deck)[s.ante_card_index].csvid =
                    CardIDFromType((*s.ante_card_list)[s.ante_slot]);
                ((csvid_and_numcards *)s.ante_winning_deck)[s.ante_card_index].numcards = 1;
                s.ante_card_found = 1;
              }
              ++s.ante_card_index;
            }
            s.ante_card_index = 0;
            s.ante_card_found = 0;
            while (s.ante_card_index < 200 && s.ante_card_found == 0)
            {
              if (CardIDFromType((*s.ante_card_list)[s.ante_slot]) ==
                  ((csvid_and_numcards *)s.ante_losing_deck)[s.ante_card_index].csvid)
              {
                --((csvid_and_numcards *)s.ante_losing_deck)[s.ante_card_index].numcards;
                s.ante_card_found = 1;
              }
              ++s.ante_card_index;
            }
          }
        }
      }
      if (ante_callback != NULL)
        ante_callback();
    }
    strcpy(s.progress_text, "");
    LoadTextSectionLines(global_ui_strings_filename,
                         "DIALOG_ENDEXP1DUEL_MATCHPROGRESS");
    if (s.player_deck_valid == 0)
      strcat(s.progress_text, g_text_lines[0]);
    if (s.opponent_deck_valid == 0)
      strcat(s.progress_text, g_text_lines[1]);
    if (best_of == 1)
      strcat(s.progress_text, g_text_lines[2]);
    else if (best_of == 3)
      shell_format_gauntlet_match_text(s.progress_text + strlen(s.progress_text),
                                      300, g_text_lines[3]);
    else if (best_of == 5)
      shell_format_gauntlet_match_text(s.progress_text + strlen(s.progress_text),
                                      300, g_text_lines[4]);
    else if (best_of == 99)
    {
      LoadTextSectionLines("MP_UIStrings.txt",
                           "DIALOG_ENDEXP1DUEL_MATCHPROGRESS");
      shell_format_gauntlet_match_text(s.progress_text + strlen(s.progress_text),
                                      300, g_text_lines[0]);
    }
    LoadTextSectionLines(global_ui_strings_filename,
                         "DIALOG_ENDEXP1DUEL_MATCHPROGRESS");
    if (s.match_finished != 0)
    {
      if (g_ante_card_count < g_duel_state_008cee6c)
        strcat(s.progress_text, g_text_lines[5]);
      else if (g_duel_state_008cee6c < g_ante_card_count)
        strcat(s.progress_text, g_text_lines[6]);
      else
        strcat(s.progress_text, g_text_lines[7]);
    }
    LoadTextSectionLines(global_ui_strings_filename,
                         "DIALOG_ENDEXP1DUEL_BUTTONS");
    strcpy(s.sideboard_button, g_text_lines[g_duel_run_mode - 1]);
    strcpy(s.save_button, g_text_lines[g_duel_run_mode + 2]);
    do
    {
      if (s.duel_result == 0)
        play_sound_effect(0x2c);
      else if (s.duel_result == 1)
        play_sound_effect(0x2d);
      s.dialog_result = end_the_game(s.duel_result, s.progress_text,
                                     s.match_finished, s.sideboard_button,
                                     s.sideboard_callback, s.save_button,
                                     s.save_callback);
      if ((g_duel_network_flags & 2) != 0 && ranked != 0)
      {
        g_network_result_packet.packet_type = 10;
        g_network_result_packet.result = s.dialog_result;
        TENTATIVE_send_network_result(0, 10);
        while (WaitForSingleObject(global_mutex_GameInit, INFINITE) != 0)
        {
        }
        TENTATIVE_wait_for_network_result(1, 10);
        ReleaseMutex(global_mutex_GameInit);
        if (g_network_result_packet.result != s.dialog_result)
          s.dialog_result = 0;
      }
      if (s.dialog_result == 0)
        s.match_quit = 1;
      else
      {
        s.deck_count = 0;
        for (s.card_index = 0; s.card_index < 200; ++s.card_index)
          if (((csvid_and_numcards *)g_opponent_deck_cards)[s.card_index].csvid != -1)
            s.deck_count +=
                ((csvid_and_numcards *)g_opponent_deck_cards)[s.card_index].numcards;
        s.player_deck_valid = minimum_deck_size <= s.deck_count;
        if (s.player_deck_valid == 0 && s.match_finished == 0)
        {
          load_text(global_ui_strings_filename, "DIALOG_ENDEXP1DUEL");
          shell_format_gauntlet_match_text(s.warning_text, 300, g_text_lines[5]);
          MessageBoxA(g_duel_window_hwnd, s.warning_text,
                      gs_magic_the_gathering_title_00789460, MB_ICONERROR);
        }
      }
    } while (s.match_quit == 0 && s.match_finished == 0 &&
             s.player_deck_valid == 0);
    /* TODO: sealed opponent deck editing at MAGIC 0x0050cb6c. */
  } while (1);
}
