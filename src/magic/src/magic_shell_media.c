#include "magic_shell_dialogs.h"
#include <stdio.h>
#include <string.h>
#include "deckdll/src/magsnd.h"
#include "global_strings.h"
#include "shared_startup.h"

extern HWND global_main_hwnd;
__declspec(dllimport) BOOL __cdecl IsCDDrive(void *drive_path);
void prepare_duel_video_mode_transition(void);

/* MagVid's movie interface is the next recursive decompilation boundary. */
// FUNCTION: MAGIC 0x004ea3e0
static int shell_movie_open(HWND hwnd, const char *path)
{
  (void)hwnd;
  (void)path;
  return 1;
}

// FUNCTION: MAGIC 0x004ea515
static int shell_movie_width(int *width)
{
  (void)width;
  return 1;
}

// FUNCTION: MAGIC 0x004ea555
static int shell_movie_height(int *height)
{
  (void)height;
  return 1;
}

// FUNCTION: MAGIC 0x004ea595
static int shell_movie_play(int position, unsigned char flags)
{
  (void)position;
  (void)flags;
  return 1;
}

// FUNCTION: MAGIC 0x004554b0
int shell_play_movie(HWND hwnd, const char *movie_name,
                     const char *directory, RECT *rect)
{
  struct
  {
    int movie_width;
    int movie_height;
    char movie_path[264];
    int result;
    char error_text[200];
    char current_directory[264];
    int found;
    FILE *file;
    char drive_letter;
    char drive_padding[3];
    char drive_path[8];
  } s;

  (void)rect;
  s.found = 0;
  if (directory != NULL)
  {
    sprintf(s.movie_path, "%s\\%s", directory, movie_name);
    s.file = fopen(s.movie_path, "rb");
    if (s.file != NULL)
    {
      fclose(s.file);
      s.found = 1;
    }
  }
  GetCurrentDirectoryA(0x104, s.current_directory);
  strcpy(s.drive_path, "c:");
  for (s.drive_letter = 'D'; s.drive_letter < 'Z' && s.found == 0;
       ++s.drive_letter)
  {
    s.drive_path[0] = s.drive_letter;
    if (IsCDDrive(s.drive_path) != 0)
    {
      sprintf(s.movie_path, "%s\\AutoPlay\\%s", s.drive_path, movie_name);
      s.file = fopen(s.movie_path, "rb");
      if (s.file != NULL)
      {
        fclose(s.file);
        s.found = 1;
      }
    }
  }
  SetCurrentDirectoryA(s.current_directory);
  if (s.found != 0)
  {
    prepare_duel_video_mode_transition();
    sound_close();
    if (g_display_color_depth == 8)
    {
      shell_movie_open(hwnd, s.movie_path);
      s.result = shell_movie_play(0, 3);
    }
    else
    {
      shell_movie_open(hwnd, s.movie_path);
      shell_movie_width(&s.movie_width);
      shell_movie_height(&s.movie_height);
      s.result = shell_movie_play(0, 3);
    }
    init_sound_dll(global_main_hwnd, 0, 0);
  }
  else
  {
    load_text(global_ui_strings_filename, "PLAYMOVIE_ERROR");
    sprintf(s.error_text, g_text_lines[0], movie_name);
    MessageBoxA(global_main_hwnd, s.error_text,
                gs_magic_the_gathering_title_00789460, MB_ICONERROR);
    s.result = 1;
  }
  SetFocus(hwnd);
  return s.result;
}
