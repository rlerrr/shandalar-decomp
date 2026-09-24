#ifndef MAGIC_SHELL_DIALOGS_H
#define MAGIC_SHELL_DIALOGS_H

#include <windows.h>

typedef struct
{
  COLORREF button_text_color;
  HBITMAP button_face;
  HPEN light_pen;
  HPEN dark_pen;
  COLORREF regular_button_color;
  COLORREF selected_button_color;
  HBRUSH light_brush;
  HBRUSH dark_brush;
  COLORREF dark_color;
  COLORREF panel_color;
  HBRUSH panel_brush;
  COLORREF background_color;
} shell_draw_resources_t;

extern shell_draw_resources_t shell_draw_resources;

typedef struct
{
  char name[200];
  char deck_name[300];
  char face_name[268];
  int difficulty;
  char reserved[4004];
} shell_rogue_profile_t;

extern shell_rogue_profile_t shell_rogue_profiles[200];
extern int shell_rogue_count;
extern char shell_gauntlet_player_deck_path[264];
int shell_load_rogue_profiles(void);
HBITMAP shell_load_bitmap_file(const char *path, void *copy_info, int unused);
void shell_draw_bitmap_button(DRAWITEMSTRUCT *item, HBITMAP background,
                              HPEN light_pen, HPEN dark_pen,
                              COLORREF color, int draw_focus);
void shell_enable_animation(int enabled);
void shell_prepare_save_game_dialog(int file_type);
int shell_parse_duel_deck(char *filename, void *library,
                          unsigned int flags, int mode);
int shell_load_gauntlet_decks(char *player_deck, char *opponent_deck);
int shell_run_gauntlet(int resume, int match_count, HWND opponent_deck_list);
void shell_show_gauntlet_matchup(HWND owner, const char *title,
                                const char *player_name,
                                const char *player_deck,
                                const char *player_face,
                                const char *opponent_name,
                                const char *opponent_deck,
                                const char *opponent_face, HANDLE match_thread);

BOOL CALLBACK shell_multi_duel_dialog_proc(HWND hwnd, UINT message,
                                          WPARAM wparam, LPARAM lparam);
BOOL CALLBACK shell_single_duel_dialog_proc(HWND hwnd, UINT message,
                                           WPARAM wparam, LPARAM lparam);
BOOL CALLBACK shell_gauntlet_dialog_proc(HWND hwnd, UINT message,
                                        WPARAM wparam, LPARAM lparam);
BOOL CALLBACK shell_blank_dialog_proc(HWND hwnd, UINT message,
                                     WPARAM wparam, LPARAM lparam);
BOOL CALLBACK shell_sealed_deck_dialog_proc(HWND hwnd, UINT message,
                                           WPARAM wparam, LPARAM lparam);
BOOL CALLBACK shell_screen_name_dialog_proc(HWND hwnd, UINT message,
                                           WPARAM wparam, LPARAM lparam);

int shell_play_movie(HWND hwnd, const char *movie_name,
                     const char *directory, RECT *rect);
BOOL shell_show_credits(HWND hwnd, void *text, RECT *rect);

#endif
