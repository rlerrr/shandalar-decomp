#ifndef MAGIC_SEALED_PLAYER_H
#define MAGIC_SEALED_PLAYER_H

#include <windows.h>

typedef struct sealed_player_dialog_context_t
{
  char *name;
  char *description;
  HBITMAP face;
  char *face_path;
  HANDLE build_thread;
  int keep_open_under_cursor;
} sealed_player_dialog_context_t;

BOOL CALLBACK shell_sealed_player_dialog_proc(HWND hwnd, UINT message,
    WPARAM wparam, LPARAM lparam);

#endif
