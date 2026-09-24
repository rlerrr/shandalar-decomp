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

extern startup_dialog_page_t
    g_startup_dialog_choices[STARTUP_DIALOG_PAGE_CAPACITY];

#endif
