#include "magic_shell_dialogs.h"
#include <mmsystem.h>
#include <stdio.h>
#include <string.h>
#include "deckdll/src/magsnd.h"
#include "global_strings.h"
#include "shared_startup.h"

extern HWND global_main_hwnd;
extern HINSTANCE g_app_instance;
__declspec(dllimport) BOOL __cdecl IsCDDrive(void *drive_path);
void prepare_duel_video_mode_transition(void);

// GLOBAL: MAGIC 0x0057ade0
static HWND g_shell_movie_original_window;
// GLOBAL: MAGIC 0x0057ade4
static HWND g_shell_movie_window;
// GLOBAL: MAGIC 0x0057ade8
static HWND g_shell_movie_backdrop;
// GLOBAL: MAGIC 0x0057adf0
static int g_shell_movie_playing;
// GLOBAL: MAGIC 0x0057adf8
static MCIDEVICEID g_shell_movie_device;
// GLOBAL: MAGIC 0x0057adfc
static unsigned int g_shell_movie_width;
// GLOBAL: MAGIC 0x0057ae00
static unsigned int g_shell_movie_height;

// FUNCTION: MAGIC 0x004ea92e
static void shell_movie_close_device(void)
{
  DWORD callback;

  if (g_shell_movie_playing != 0)
  {
    callback = 0;
    mciSendCommandA(g_shell_movie_device, MCI_STOP, MCI_WAIT,
                    (DWORD)&callback);
  }
  if (g_shell_movie_device != 0)
  {
    mciSendCommandA(g_shell_movie_device, MCI_CLOSE, MCI_WAIT,
                    (DWORD)&callback);
    g_shell_movie_device = 0;
  }
  g_shell_movie_playing = 0;
}

// FUNCTION: MAGIC 0x004ea99f
static void shell_movie_cleanup(void)
{
  shell_movie_close_device();
  if (g_shell_movie_window != NULL)
    DestroyWindow(g_shell_movie_window);
  if (g_shell_movie_backdrop != NULL)
    DestroyWindow(g_shell_movie_backdrop);
  UnregisterClassA("RJC_MovieClass", g_app_instance);
  if (g_shell_movie_original_window != NULL)
    ShowWindow(g_shell_movie_original_window, SW_SHOW);
}

// FUNCTION: MAGIC 0x004eaa6a
static LRESULT CALLBACK shell_movie_window_proc(HWND hwnd, UINT message,
                                                WPARAM wparam, LPARAM lparam)
{
  switch (message)
  {
  case WM_LBUTTONUP:
  case WM_RBUTTONUP:
    shell_movie_cleanup();
    break;
  }
  return DefWindowProcA(hwnd, message, wparam, lparam);
}

// FUNCTION: MAGIC 0x004ea820
static int shell_movie_create_windows(HWND parent)
{
  struct
  {
    DWORD style;
    DWORD extended_style;
    WNDCLASSA window_class;
  } s;

  memset(&s.window_class, 0, sizeof(s.window_class));
  s.window_class.style = 0x1000;
  s.window_class.lpfnWndProc = shell_movie_window_proc;
  s.window_class.hInstance = g_app_instance;
  s.window_class.hIcon = NULL;
  s.window_class.hCursor = NULL;
  s.window_class.hbrBackground = GetStockObject(BLACK_BRUSH);
  s.window_class.lpszClassName = "RJC_MovieClass";
  RegisterClassA(&s.window_class);
  s.extended_style = 0;
  s.style = WS_POPUP;
  g_shell_movie_backdrop =
      CreateWindowExA(s.extended_style, "RJC_MovieClass", "Movie Backdrop", s.style, 0, 0,
                      GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
                      parent, NULL, g_app_instance, NULL);
  g_shell_movie_window =
      CreateWindowExA(s.extended_style, "RJC_MovieClass", "Magical Night at the Movies",
                      s.style, 0, 0, 0, 0, g_shell_movie_backdrop, NULL,
                      g_app_instance, NULL);
  ShowWindow(g_shell_movie_window, SW_SHOW);

  return (g_shell_movie_window != NULL) ? 0 : 1;
}

// FUNCTION: MAGIC 0x004eaa0d
static void shell_movie_show_error(const char *message, MCIERROR error)
{
  struct
  {
    char formatted[1024];
    char system_error[132];
  } s;

  mciGetErrorStringA(error, s.system_error, 0x81);
  sprintf(s.formatted, "%s\n%s", message, s.system_error);
  MessageBoxA(NULL, s.formatted, "PlayMovie() Error:", 0);
}

// FUNCTION: MAGIC 0x004ea3e0
static int shell_movie_open(HWND hwnd, const char *path)
{
  struct
  {
    DWORD callback;
    MCIDEVICEID device;
    const char *device_type;
    const char *element_name;
    const char *alias;
    DWORD style;
    HWND parent;
    MCIERROR error;
    DWORD where_callback;
    RECT where_rect;
    DWORD flags;
  } s;

  g_shell_movie_original_window = hwnd;
  if (shell_movie_create_windows(hwnd) != 0)
  {
    shell_movie_cleanup();
    shell_movie_show_error("Could not create the movie's windows", 0);
    return 1;
  }
  s.callback = 0;
  s.device = 0;
  s.device_type = NULL;
  s.element_name = path;
  s.alias = NULL;
  s.style = WS_CHILD;
  s.parent = g_shell_movie_window;
  s.flags = 0x170202;
  s.error = mciSendCommandA(0, MCI_OPEN, s.flags, (DWORD)&s.callback);
  if (s.error != 0)
  {
    shell_movie_cleanup();
    shell_movie_show_error(
        "MCI_OPEN: An error occurred while opening the MCI device.", s.error);
    return 1;
  }
  g_shell_movie_device = s.device;
  s.flags = 0x40002;
  s.where_callback = 0;
  s.error = mciSendCommandA(g_shell_movie_device, MCI_WHERE, s.flags,
                            (DWORD)&s.where_callback);
  if (s.error != 0)
  {
    shell_movie_cleanup();
    shell_movie_show_error(
        "MCI_WHERE: An error occurred when requesting the movie's size", s.error);
    return 1;
  }
  g_shell_movie_width = s.where_rect.right;
  g_shell_movie_height = s.where_rect.bottom;
  return 0;
}

// FUNCTION: MAGIC 0x004ea515
static int shell_movie_width(int *width)
{
  if (g_shell_movie_width == 0 || g_shell_movie_height == 0)
    return 1;
  *width = g_shell_movie_width;
  return 0;
}

// FUNCTION: MAGIC 0x004ea555
static int shell_movie_height(int *height)
{
  if (g_shell_movie_width == 0 || g_shell_movie_height == 0)
    return 1;
  *height = g_shell_movie_height;
  return 0;
}

// FUNCTION: MAGIC 0x004ea595
static int shell_movie_play(int position, int flags)
{
  struct
  {
    int screen_height_half;
    int screen_width_half;
    HWND previous_capture;
    DWORD play_params[3];
    MCIERROR error;
    DWORD break_callback;
    int break_key;
    HWND break_window;
    DWORD status_callback;
    DWORD status_result;
    DWORD status_item;
    DWORD status_track;
    DWORD command_flags;
  } s;

  SetFocus(g_shell_movie_window);
  SetForegroundWindow(g_shell_movie_window);
  if ((flags & 1) != 0)
    ShowWindow(g_shell_movie_backdrop, SW_SHOW);
  if (position == 0 && (flags & 1) == 0)
  {
    s.screen_width_half = GetSystemMetrics(SM_CXSCREEN) >> 1;
    s.screen_height_half = GetSystemMetrics(SM_CYSCREEN) >> 1;
    MoveWindow(g_shell_movie_window,
               s.screen_width_half - (g_shell_movie_width >> 1),
               s.screen_height_half - (g_shell_movie_height >> 1),
               g_shell_movie_width, g_shell_movie_height, FALSE);
  }
  if ((flags & 2) != 0)
  {
    s.break_key = VK_ESCAPE;
    s.break_window = g_shell_movie_window;
    s.command_flags = 0x302;
    s.error = mciSendCommandA(g_shell_movie_device, 0x811, s.command_flags,
                              (DWORD)&s.break_callback);
    if (s.error != 0)
    {
      shell_movie_cleanup();
      shell_movie_show_error(
          "MCI_BREAK: An error occurred while setting the \"break\" key.",
          s.error);
      return 1;
    }
  }
  s.previous_capture = SetCapture(g_shell_movie_window);
  s.command_flags = 0;
  if ((flags & 1) != 0)
    s.command_flags |= 0x2000000;
  s.error = mciSendCommandA(g_shell_movie_device, MCI_PLAY, s.command_flags,
                            (DWORD)s.play_params);
  if (s.error != 0)
  {
    shell_movie_cleanup();
    shell_movie_show_error(
        "MCI_PLAY: An error occurred while playing the movie.", s.error);
    return 1;
  }
  memset(&s.status_callback, 0, 0x10);
  s.status_item = MCI_STATUS_MODE;
  s.command_flags = 0x102;
  do
  {
    Sleep(350);
    s.error = mciSendCommandA(g_shell_movie_device, MCI_STATUS, s.command_flags,
                              (DWORD)&s.status_callback);
    if (s.error != 0)
    {
      shell_movie_cleanup();
      shell_movie_show_error(
          "MCI_STATUS: An error occurred while querying the movie's status.",
          s.error);
      return 1;
    }
    if ((GetAsyncKeyState(VK_ESCAPE) & 1) != 0 ||
        (GetAsyncKeyState(VK_LBUTTON) & 1) != 0 ||
        (GetAsyncKeyState(VK_RBUTTON) & 1) != 0)
      s.status_result = MCI_MODE_STOP;
  } while (s.status_result == MCI_MODE_PLAY);
  if (s.previous_capture != NULL)
    SetCapture(s.previous_capture);
  else
    ReleaseCapture();
  shell_movie_cleanup();
  return 0;
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
