#ifndef MAGIC_SHELL_SCREEN_NAME_H
#define MAGIC_SHELL_SCREEN_NAME_H

#include "magic_shell.h"

typedef struct
{
  char default_name[52];
  COLORREF selected_button_color;
  HBITMAP playfaces[200];
} screen_name_playface_resources_t;

typedef struct
{
  char date_format[100];
  HFONT font;
} screen_name_text_resources_t;

typedef struct
{
  char unknown_date[56];
  HFONT font;
} screen_name_unknown_resources_t;

extern screen_name_playface_resources_t g_screen_name_playface_resources;
extern screen_name_text_resources_t g_screen_name_text_resources;
extern screen_name_unknown_resources_t g_screen_name_unknown_resources;
extern int g_screen_name_profile_changed;
extern int g_screen_name_worker_running;
extern char g_screen_name_skill_format[100];
extern char g_screen_name_skill_names[10][300];
extern char g_screen_name_record_label[100];
extern char g_screen_name_rank_format[100];
extern char g_screen_name_disconnect_format[100];
extern char g_screen_name_concede_format[100];
extern char g_screen_name_dci_record_label[100];
extern char g_screen_name_multiplayer_record_label[100];
extern HBRUSH g_screen_name_entry_brush;
extern COLORREF g_screen_name_entry_text_color;
extern HPEN g_screen_name_entry_dark_pen;
extern HPEN g_screen_name_entry_light_pen;
extern HBITMAP g_screen_name_entry_background;
extern COLORREF g_screen_name_entry_button_color;

void shell_populate_screen_names(HWND hwnd);
void shell_save_match_screen_name_profile(void);
void shell_save_active_screen_name(void);
void shell_get_playface_rect(HWND hwnd, RECT *rect);
void __cdecl shell_screen_name_network_worker(void *context);
void shell_layout_screen_name_dialog(HWND hwnd);
void shell_apply_screen_name_profile(HWND hwnd);
BOOL CALLBACK shell_enter_screen_name_dialog_proc(HWND hwnd, UINT message,
                                                  WPARAM wparam, LPARAM lparam);
void initialize_screen_name_profile(screen_name_file_t *profile, int use_current_time);

#endif
