#ifndef MAGIC_SHELL_NETWORK_MATCH_H
#define MAGIC_SHELL_NETWORK_MATCH_H

#include <windows.h>

typedef struct
{
  int minimum_deck_size;
  int deck_type;
  int ante;
  int free_play;
  int best_of;
  int allow_sideboarding;
  char player_deck[52];
  int player_random;
} shell_multiplayer_options_t;

extern shell_multiplayer_options_t g_shell_multiplayer_options;
void shell_save_multiplayer_options(void);
void shell_load_multiplayer_options(void);
void shell_layout_multiplayer_controls(HWND hwnd);
void shell_apply_multiplayer_options(HWND hwnd);
extern int g_multiplayer_no_valid_decks;
extern char g_multiplayer_status_text[6][300];
extern int g_multiplayer_resume_requested;
extern int g_multiplayer_dialog_state;
extern int g_multiplayer_duel_ready;
void shell_update_multiplayer_status(HWND hwnd, int state);
void shell_update_multiplayer_dialog_state(HWND hwnd, int state);
void __cdecl shell_wait_for_multiplayer_parameters(void *context);
void __cdecl shell_wait_for_multiplayer_response(void *context);
void __cdecl shell_wait_for_multiplayer_start(void *context);
void shell_begin_duel_trace(char *message);
LRESULT shell_populate_multiplayer_decks(HWND hwnd, int control_id, int deck_type,
                                        int ante, int minimum_size);
int shell_classify_multiplayer_deck(char *filename, int requested_type,
                                    int ante, int minimum_size);
extern char g_multiplayer_button_text[6][300];
int shell_update_dci_rank(int rank, int opponent_rank, int duel_result);

#endif
