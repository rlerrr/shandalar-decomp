#ifndef MAGIC_SHELL_H
#define MAGIC_SHELL_H

#include <windows.h>

#define STARTUP_DIALOG_PAGE_CAPACITY 5
#define STARTUP_DIALOG_CHOICE_CAPACITY 10

typedef HWND (*startup_dialog_open_callback_t)(LPARAM rect);
typedef int (*startup_dialog_close_callback_t)(void);
typedef void (*startup_dialog_action_callback_t)(RECT *rect);

typedef struct
{
  char label[100];
  char description[100];
  int is_modeless;
  startup_dialog_open_callback_t open;
  startup_dialog_close_callback_t close;
  startup_dialog_action_callback_t action;
} startup_dialog_choice_t;

typedef struct
{
  RECT page_rect;
  char title[0x34];
  startup_dialog_choice_t choices[STARTUP_DIALOG_CHOICE_CAPACITY];
  int choice_count;
} startup_dialog_page_t;

typedef struct
{
  char playface_name[0x105];
  char screen_name[0x0e];
  char real_name[0x100];
  char personal_quote[0x400];
  char email[0x100];
  char date_text[0x15];
  unsigned short unk_728;
  unsigned short unk_72a;
  unsigned short unk_72c;
  unsigned short dci_rank_display;
  unsigned short dci_wins;
  unsigned short dci_losses;
  unsigned short dci_draws_or_unused;
  unsigned short dci_rank_pending;
  unsigned short mp_wins;
  unsigned short mp_losses;
  unsigned short mp_draws;
  unsigned short concede_count;
  unsigned short disconnect_count;
  unsigned char has_profile_stats;
  unsigned char unk_743;
  unsigned int unk_744;
} screen_name_file_t;

extern startup_dialog_page_t
    g_startup_dialog_choices[STARTUP_DIALOG_PAGE_CAPACITY];
extern screen_name_file_t g_screen_name_profile;
void load_active_screen_name_profile(void);

#endif
