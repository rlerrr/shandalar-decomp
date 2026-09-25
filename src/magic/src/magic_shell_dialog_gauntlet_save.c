#include "magic_shell_dialogs.h"
#include <stdio.h>
#include <string.h>
#include <commdlg.h>
#include "duel_engine.h"
#include "global_other.h"
#include "global_state.h"
#include "global_strings.h"
#include "shared_startup.h"

extern char g_duel_run_save_extensions[4][10];
extern char g_duel_save_extension[0x10];

// GLOBAL: MAGIC 0x0069c388
static char shell_save_game_file_title[0x104];
// GLOBAL: MAGIC 0x0069c4a0
static char shell_save_game_filter[0x68];
// GLOBAL: MAGIC 0x0069c508
static char shell_save_game_filename[0x104];

// FUNCTION: MAGIC 0x004d2630
void shell_prepare_save_game_dialog(int file_type)
{
  g_save_format_version = 0xf;
  strcpy(g_duel_run_save_extensions[0], "SVG");
  strcpy(g_duel_run_save_extensions[1], "E1M");
  strcpy(g_duel_run_save_extensions[2], "E1G");
  strcpy(g_duel_run_save_extensions[3], "E1T");
  strcpy(g_duel_save_extension,
         g_duel_run_save_extensions[file_type]);
  load_text(global_ui_strings_filename, "SAVEGAMEFILETYPE");
  strcpy(shell_save_game_filter, g_text_lines[file_type]);
  strcat(shell_save_game_filter + strlen(shell_save_game_filter) + 1, "*.");
  strcat(shell_save_game_filter + strlen(shell_save_game_filter) + 1,
         g_duel_save_extension);
  strcat(shell_save_game_filter + strlen(shell_save_game_filter) + 1, "");
  sprintf(shell_save_game_filename, "*.%s", g_duel_save_extension);
  g_duel_save_game_openfilename.lStructSize = sizeof(OPENFILENAMEA);
  g_duel_save_game_openfilename.hwndOwner = g_duel_window_hwnd;
  g_duel_save_game_openfilename.hInstance = NULL;
  g_duel_save_game_openfilename.lpstrFilter = shell_save_game_filter;
  g_duel_save_game_openfilename.lpstrCustomFilter = NULL;
  g_duel_save_game_openfilename.nMaxCustFilter = 0;
  g_duel_save_game_openfilename.nFilterIndex = 0;
  g_duel_save_game_openfilename.lpstrFile = shell_save_game_filename;
  g_duel_save_game_openfilename.nMaxFile = 0x104;
  g_duel_save_game_openfilename.lpstrFileTitle = shell_save_game_file_title;
  g_duel_save_game_openfilename.nMaxFileTitle = 0x104;
  g_duel_save_game_openfilename.lpstrInitialDir = global_savegame_path;
  load_text(global_ui_strings_filename, "WINDOWTITLES");
  g_duel_save_game_openfilename.lpstrTitle = g_text_lines[5];
  g_duel_save_game_openfilename.Flags = 0x2a000c;
  g_duel_save_game_openfilename.nFileOffset = 0;
  g_duel_save_game_openfilename.nFileExtension = 0;
  g_duel_save_game_openfilename.lpstrDefExt = g_duel_save_extension;
  g_duel_save_game_openfilename.lCustData = 0;
  g_duel_save_game_openfilename.lpfnHook = NULL;
  g_duel_save_game_openfilename.lpTemplateName = NULL;
}
