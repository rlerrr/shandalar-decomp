#include <windows.h>

#include "magvid.h"
#include "mag_dib.h"
#include "avi_player.h"
extern "C"
{
#include "magsnd.h"
}

#define MAGVID_SLOT_COUNT 3

struct VideoPalette
{
  WORD version;
  WORD entry_count;
  PALETTEENTRY entries[256];
  DWORD reserved;
};

static LRESULT CALLBACK video_window_proc(HWND window, UINT message,
                                           WPARAM wparam, LPARAM lparam);
int __cdecl find_video_by_window(HWND window, int *video);
void __cdecl resize_video_window(MagVid *state);
void __cdecl center_video_surface(MagVid *state);
void __cdecl stop_video_state(MagVid *state);
void __cdecl unload_video_state(MagVid *state);
void __cdecl start_video_state(MagVid *state);
void __cdecl video_playback_thread(MagVid *state);
int __cdecl apply_video_background(MagVid *state);
int __cdecl load_video_file(MagVid *state, const char *path);
int __cdecl load_video_background(MagVid *state, char *path);
void __cdecl refresh_video_palette(MagVid *state);
void __cdecl realize_video_palette(MagVid *state, HWND excluded);

// GLOBAL: MAGVID 0x1000C020
static int g_init_count;
// GLOBAL: MAGVID 0x1000C024
static char *g_video_class_name = "VIDWINCLASS";
// GLOBAL: MAGVID 0x1000C02C
static HWND g_last_parent_window;
// GLOBAL: MAGVID 0x1000C034
static int g_active_play_count;
// GLOBAL: MAGVID 0x1000C03C
static HINSTANCE g_instance;
// GLOBAL: MAGVID 0x1000C040
static int g_restore_system_palette = 1;
// GLOBAL: MAGVID 0x1000C044
static UINT g_previous_system_palette_use;

// GLOBAL: MAGVID 0x1000C1B8
static MagVid *g_videos[MAGVID_SLOT_COUNT];

// GLOBAL: MAGVID 0x1000C1C8
static WNDCLASSA g_video_window_class;

// GLOBAL: MAGVID 0x1001F22C
int _delay;

// FUNCTION: MAGVID 0x1000109A
BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
  switch (reason)
  {
  case DLL_PROCESS_ATTACH:
    g_instance = instance;
    break;
  case DLL_PROCESS_DETACH:
    break;
  case DLL_THREAD_ATTACH:
    break;
  case DLL_THREAD_DETACH:
    break;
  }

  return TRUE;
}

// FUNCTION: MAGVID 0x10001104
int __cdecl InitVid(HWND owner, HINSTANCE instance, unsigned int flags)
{
  if (g_init_count == 0)
  {
    g_video_window_class.style = CS_HREDRAW | CS_VREDRAW;
    g_video_window_class.lpfnWndProc = video_window_proc;
    g_video_window_class.cbClsExtra = 0;
    g_video_window_class.cbWndExtra = 0;
    g_video_window_class.hInstance = g_instance;
    g_video_window_class.hIcon = LoadIconA(NULL, IDI_APPLICATION);
    g_video_window_class.hCursor = LoadCursorA(NULL, IDC_ARROW);
    g_video_window_class.hbrBackground = NULL;
    g_video_window_class.lpszMenuName = NULL;
    g_video_window_class.lpszClassName = g_video_class_name;
    RegisterClassA(&g_video_window_class);
  }
  init_sound_dll(NULL, 0, 3);
  g_init_count++;
  return MAGVID_OK;
}

// FUNCTION: MAGVID 0x100011B6
int __cdecl ReleaseVid(void)
{
  if (g_init_count == 0)
  {
    return MAGVID_OK;
  }
  g_init_count--;
  if (g_init_count == 0)
  {
    UnregisterClassA(g_video_class_name, g_video_window_class.hInstance);
    sound_close();
  }
  return MAGVID_OK;
}

// FUNCTION: MAGVID 0x100011FF
int __cdecl LoadAVI(const char *path, int *out_video,
                    MagVidPosition *position, unsigned int flags)
{
  struct
  {
    char *error_message;
    DWORD error_code;
    int height;
    int slot;
    int y;
    int x;
    int width;
    MagVid *state;
    AviPlayer *player;
  } local;

  local.slot = 0;
  local.width = 0;
  local.height = 0;
  local.x = 0;
  local.y = 0;
  while ((local.slot < MAGVID_SLOT_COUNT) &&
         (g_videos[local.slot] != NULL))
  {
    local.slot++;
  }
  if (local.slot == MAGVID_SLOT_COUNT)
  {
    return MAGVID_NO_FREE_SLOT;
  }
  g_videos[local.slot] = new MagVid;
  local.state = g_videos[local.slot];
  if (local.state == NULL)
  {
    return MAGVID_NO_FREE_SLOT;
  }
  memset(local.state, 0, sizeof(MagVid));
  g_videos[local.slot]->player = new AviPlayer;
  local.player = g_videos[local.slot]->player;
  if (local.player == NULL)
  {
    delete g_videos[local.slot];
    g_videos[local.slot] = NULL;
    return MAGVID_NO_FREE_SLOT;
  }
  if (flags != 0)
  {
    local.width = GetSystemMetrics(SM_CXSCREEN);
    local.height = GetSystemMetrics(SM_CYSCREEN);
    local.x = 0;
    local.state->x = local.x;
    local.y = 0;
    local.state->y = local.y;
  }
  else
  {
    local.width = CW_USEDEFAULT;
    local.height = CW_USEDEFAULT;
    if (position != NULL)
    {
      local.x = position->x;
      local.state->x = local.x;
      local.y = position->y;
      local.state->y = local.y;
    }
    else
    {
      local.x = 0;
      local.state->x = local.x;
      local.y = 0;
      local.state->x = local.y;
    }
  }
  if ((flags & MAGVID_USE_EXISTING_WINDOW) != 0)
  {
    if ((g_videos[*out_video] != NULL) &&
        (g_videos[*out_video]->window != NULL))
    {
      local.state->window = g_videos[*out_video]->window;
      local.state->owns_window = 0;
    }
  }
  else
  {
    g_last_parent_window = get_sound_hwnd();
    local.state->parent_window = g_last_parent_window;
    local.state->window = CreateWindowExA(
        0, g_video_class_name, NULL, WS_POPUP | WS_VISIBLE,
        local.x, local.y, local.width, local.height,
        g_last_parent_window, NULL, g_instance, NULL);
    if (local.state->window == NULL)
    {
      local.error_code = GetLastError();
      FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                         FORMAT_MESSAGE_FROM_SYSTEM,
                     NULL, local.error_code,
                     MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                     (char *)&local.error_message, 0, NULL);
      MessageBoxA(NULL, local.error_message, "GetLastError",
                  MB_ICONINFORMATION);
      LocalFree(local.error_message);
    }
    local.state->owns_window = 1;
  }
  local.state->index = local.slot;
  if ((flags & MAGVID_USE_EXISTING_WINDOW) == 0)
  {
    SetFocus(g_videos[local.slot]->window);
    SetForegroundWindow(g_videos[local.slot]->window);
    ShowWindow(g_videos[local.slot]->window, SW_SHOWNORMAL);
  }
  if (load_video_file(local.state, path) != MAGVID_OK)
  {
    if ((flags & MAGVID_USE_EXISTING_WINDOW) == 0)
    {
      DestroyWindow(g_videos[local.slot]->window);
    }
    delete g_videos[local.slot];
    g_videos[local.slot] = NULL;
    return MAGVID_ERROR;
  }
  *out_video = local.slot;
  return MAGVID_OK;
}

// FUNCTION: MAGVID 0x10002300
static LRESULT CALLBACK video_window_proc(HWND window, UINT message,
                                           WPARAM wparam, LPARAM lparam)
{
  struct
  {
    int video;
    LRESULT result;
    HDC dc;
    HANDLE process;
  } local;

  switch (message)
  {
    case WM_CREATE:
      break;
    case WM_DESTROY:
      g_restore_system_palette = 1;
      local.dc = GetDC(window);
      SetSystemPaletteUse(local.dc, g_previous_system_palette_use);
      ReleaseDC(window, local.dc);
      g_restore_system_palette = 1;
      break;
    case WM_SIZE:
      InvalidateRect(window, NULL, FALSE);
      break;
    case WM_PALETTECHANGED:
      InvalidateRect(window, NULL, FALSE);
      break;
    case WM_ERASEBKGND:
      if (g_restore_system_palette != 0)
      {
        local.result = DefWindowProcA(window, message, wparam, lparam);
        local.dc = GetDC(window);
        g_previous_system_palette_use =
            SetSystemPaletteUse(local.dc, SYSPAL_STATIC);
        ReleaseDC(window, local.dc);
        g_restore_system_palette = 0;
      }
      local.video = -1;
      if (find_video_by_window(window, &local.video) == MAGVID_OK)
      {
        DrawVidBackground(local.video);
      }
      break;
    case WM_KILLFOCUS:
      local.process = GetCurrentProcess();
      SetPriorityClass(local.process, NORMAL_PRIORITY_CLASS);
      break;
    case WM_SETFOCUS:
      local.process = GetCurrentProcess();
      SetPriorityClass(local.process, HIGH_PRIORITY_CLASS);
      break;
    case WM_PAINT:
      InvalidateRect(window, NULL, FALSE);
      break;
    case WM_LBUTTONUP:
      break;
    case WM_USER + 1:
      if ((g_videos[lparam] != NULL) &&
          (g_videos[lparam]->background == NULL))
      {
        resize_video_window(g_videos[lparam]);
      }
      break;
  }
  return DefWindowProcA(window, message, wparam, lparam);
}

// FUNCTION: MAGVID 0x1000190C
int __cdecl SetVidCallBack(int video, int callback)
{
  MagVid *state;

  if ((video < 0) || (2 < video))
  {
    return MAGVID_INVALID_ARGUMENT;
  }
  state = g_videos[video];
  if (state == NULL)
  {
    return MAGVID_NOT_LOADED;
  }

  state->callback = callback;
  return MAGVID_OK;
}

// FUNCTION: MAGVID 0x1000197B
int __cdecl SetVidBackground(int background, int video)
{
  struct
  {
    MagVid *state;
    AviPlayer *player;
  } local;

  if ((video < 0) || (2 < video))
  {
    return MAGVID_INVALID_ARGUMENT;
  }
  local.state = g_videos[video];
  if (local.state == NULL)
  {
    return MAGVID_OK;
  }
  local.player = local.state->player;
  load_video_background(local.state, (char *)background);
  refresh_video_palette(local.state);
  center_video_surface(local.state);
  return MAGVID_OK;
}

// FUNCTION: MAGVID 0x100019FD
int __cdecl SetVidBackgroundToBMP(BITMAPINFOHEADER *bitmap, void *bits,
                                  int video)
{
  struct
  {
    MagVid *state;
    AviPlayer *player;
  } local;

  if ((video < 0) || (2 < video))
  {
    return MAGVID_INVALID_ARGUMENT;
  }
  local.state = g_videos[video];
  if (local.state == NULL)
  {
    return MAGVID_OK;
  }
  local.player = local.state->player;
  if (bitmap->biHeight < 0)
  {
    bitmap->biHeight = abs(bitmap->biHeight);
  }
  local.state->background = new MagDib;
  local.state->background->attach_external_bits(bitmap, bits);
  apply_video_background(local.state);
  center_video_surface(local.state);
  return MAGVID_OK;
}

// FUNCTION: MAGVID 0x10001B1F
int __cdecl SetVidBackgroundToDIB(MagDib *dib, int video)
{
  struct
  {
    MagVid *state;
    RECT window_rect;
    RECT dib_rect;
    AviPlayer *player;
  } local;

  if ((video < 0) || (2 < video))
  {
    return MAGVID_INVALID_ARGUMENT;
  }
  local.state = g_videos[video];
  if (local.state == NULL)
  {
    return MAGVID_OK;
  }
  local.player = local.state->player;
  local.state->background = dib;
  local.state->owns_background = 0;
  apply_video_background(local.state);
  local.window_rect.left = 0;
  local.window_rect.top = 0;
  local.window_rect.right = 0;
  local.window_rect.bottom = 0;
  local.dib_rect.left = 0;
  local.dib_rect.top = 0;
  local.dib_rect.right = 0;
  local.dib_rect.bottom = 0;
  GetWindowRect(local.state->window, &local.window_rect);
  dib->get_bounds_rect(&local.dib_rect);
  if (local.dib_rect.right < local.window_rect.right)
  {
    local.state->x =
        (local.window_rect.right - local.dib_rect.right) / 2;
  }
  if (local.dib_rect.bottom < local.window_rect.bottom)
  {
    local.state->y =
        (local.window_rect.bottom - local.dib_rect.bottom) / 2;
  }
  return MAGVID_OK;
}

// FUNCTION: MAGVID 0x1000186E
int __cdecl StopAVI(int video)
{
  MagVid *state;
  AviPlayer *player;

  if ((video < 0) || (2 < video))
  {
    return MAGVID_INVALID_ARGUMENT;
  }
  state = g_videos[video];
  if (state == NULL)
  {
    return MAGVID_OK;
  }
  if (((state->flags >> 1) & 1) != 0)
  {
    return MAGVID_OK;
  }
  player = state->player;
  if (player != NULL)
  {
    sound_stop(video + 0x100);
    stop_video_state(state);
  }
  g_active_play_count--;
  return MAGVID_OK;
}

// FUNCTION: MAGVID 0x100015F7
int __cdecl UnloadAVI(int video)
{
  MagVid *state;
  AviPlayer *player;

  if ((video < 0) || (2 < video))
  {
    return MAGVID_INVALID_ARGUMENT;
  }
  state = g_videos[video];
  if (state == NULL)
  {
    return MAGVID_OK;
  }
  if (state->reserved_4c != 0)
  {
    StopAVI(video);
  }
  player = state->player;
  if ((((state->flags >> 3) & 1) != 0) && (state->is_linked != 0))
  {
    state->thread_active = 0;
  }
  if (((state->flags & 1) != 0) && (player != NULL))
  {
    unload_video_state(state);
    state->flags &= ~1U;
  }
  if (state->device_context != NULL)
  {
    ReleaseDC(state->window, state->device_context);
    if (state->linked_video != NULL)
    {
      state->linked_video->device_context = NULL;
    }
    state->linked_video = NULL;
  }
  if ((state->window != NULL) && (state->owns_window != 0))
  {
    DestroyWindow(state->window);
    state->window = NULL;
    if (state->linked_video != NULL)
    {
      state->linked_video->window = NULL;
    }
    state->owns_window = 0;
  }
  if (player != NULL)
  {
    delete player;
  }
  if ((state->owns_background != 0) && (state->background != NULL))
  {
    delete state->background;
  }
  g_last_parent_window = state->parent_window;
  delete state;
  g_videos[video] = NULL;
  return MAGVID_OK;
}

// FUNCTION: MAGVID 0x100017F8
int __cdecl PlayAVI(int video)
{
  struct
  {
    MagVid *state;
    AviPlayer *player;
  } local;

  if ((video < 0) || (2 < video))
  {
    return MAGVID_INVALID_ARGUMENT;
  }
  local.state = g_videos[video];
  if (local.state == NULL)
  {
    return MAGVID_OK;
  }
  local.player = local.state->player;
  if (local.player != NULL)
  {
    start_video_state(local.state);
  }
  g_active_play_count++;
  return MAGVID_OK;
}

// FUNCTION: MAGVID 0x1000204B
int __cdecl SetVidThreadPriority(int video, int priority)
{
  MagVid *state;
  int result;

  if ((video < 0) || (2 < video))
  {
    return MAGVID_INVALID_ARGUMENT;
  }
  state = g_videos[video];
  if (state == NULL)
  {
    return MAGVID_OK;
  }

  if (state->thread != NULL)
  {
    result = SetThreadPriority(state->thread, priority);
  }

  if (result == 0)
  {
    return MAGVID_THREAD_ERROR;
  }
  return MAGVID_OK;
}

// FUNCTION: MAGVID 0x100020D1
int __cdecl LinkVids(int video, int linked_video)
{
  MagVid *state;
  MagVid *linked;

  if ((video < 0) || (2 < video))
  {
    return MAGVID_INVALID_ARGUMENT;
  }
  if ((linked_video < 0) || (2 < linked_video))
  {
    return MAGVID_INVALID_ARGUMENT;
  }
  state = g_videos[video];
  if (state == NULL)
  {
    return MAGVID_NOT_LOADED;
  }

  linked = g_videos[linked_video];
  if (linked == NULL)
  {
    return MAGVID_NOT_LOADED;
  }

  state->linked_video = linked;
  linked->is_linked = 1;
  return MAGVID_OK;
}

// FUNCTION: MAGVID 0x100022AB
int __cdecl VidStatus(int video)
{
  MagVid *state;

  if ((video < 0) || (2 < video))
  {
    return MAGVID_INVALID_ARGUMENT;
  }
  state = g_videos[video];
  if (state == NULL)
  {
    return MAGVID_OK;
  }

  return state->playing;
}

// FUNCTION: MAGVID 0x10002B6E
int __cdecl SetVidTransparency(int video, int transparency)
{
  struct
  {
    MagVid *state;
    AviPlayer *player;
  } local;

  local.state = NULL;
  local.player = NULL;

  if ((video < 0) || (2 < video))
  {
    return MAGVID_INVALID_ARGUMENT;
  }
  local.state = g_videos[video];
  if (local.state == NULL)
  {
    return MAGVID_OK;
  }
  local.player = local.state->player;
  if (local.player == NULL)
  {
    return MAGVID_INVALID_ARGUMENT;
  }
  if (local.player->draw == NULL)
  {
    return MAGVID_INVALID_ARGUMENT;
  }
  local.player->draw->set_transparency(transparency);
  return MAGVID_OK;
}

// FUNCTION: MAGVID 0x10002AEA
int __cdecl advance_video_frame(int video)
{
  struct
  {
    MagVid *state;
    AviPlayer *player;
  } local;

  local.state = NULL;
  local.player = NULL;

  if ((video < 0) || (2 < video))
  {
    return MAGVID_INVALID_ARGUMENT;
  }
  local.state = g_videos[video];
  if (local.state == NULL)
  {
    return MAGVID_OK;
  }
  local.player = local.state->player;
  if (local.player == NULL)
  {
    return MAGVID_INVALID_ARGUMENT;
  }
  local.player->advance_frame();
  return MAGVID_OK;
}

// FUNCTION: MAGVID 0x10002A73
int __cdecl find_video_by_window(HWND window, int *video)
{
  struct
  {
    int previous_slot;
    int slot;
  } local;

  local.slot = 0;
  while ((g_videos[local.slot] != NULL) &&
         (g_videos[local.slot]->window != window))
  {
    local.previous_slot = local.slot;
    local.slot++;
    if (local.previous_slot >= MAGVID_SLOT_COUNT)
    {
      break;
    }
  }
  if (local.slot < MAGVID_SLOT_COUNT)
  {
    *video = local.slot;
    return MAGVID_OK;
  }
  return MAGVID_INVALID_ARGUMENT;
}

// FUNCTION: MAGVID 0x10006C95
void __cdecl move_video_destination(MagVid *state, int x, int y)
{
  AviPlayer *player;
  RECT rect;

  if ((state != NULL) && (state->player != NULL))
  {
    player = state->player;
    if (player->is_open() != 0)
    {
      player->draw->get_destination_rect(&rect);
      rect.top += y;
      rect.bottom += y;
      rect.right += x;
      rect.left += x;
      player->draw->set_destination_rect(&rect);
    }
  }
}

// FUNCTION: MAGVID 0x100068CF
void __cdecl resize_video_window(MagVid *state)
{
  struct
  {
    RECT rect;
    AviPlayer *player;
    RECT copy;
  } local;

  if ((state == NULL) || (state->player == NULL))
  {
    return;
  }
  local.player = state->player;
  if (local.player->draw != NULL)
  {
    local.player->draw->get_present_rect(&local.rect);
  }
  else
  {
    local.rect.top = 0;
    local.rect.left = local.rect.top;
    local.rect.right = 320;
    local.rect.bottom = 0;
  }
  local.copy.left = local.rect.left;
  local.copy.right = local.rect.right;
  local.copy.bottom = local.rect.bottom;
  local.copy.top = local.rect.top;
  SetWindowPos(state->window, NULL,
               local.copy.left + state->x,
               local.copy.top + state->y,
               local.copy.right - local.copy.left,
               local.copy.bottom - local.copy.top,
               SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
}

// FUNCTION: MAGVID 0x10002177
int __cdecl SetVidPos(int video, MagVidPosition *position)
{
  struct
  {
    MagVid *state;
    RECT rect;
    AviPlayer *player;
  } local;

  local.state = NULL;
  local.player = NULL;

  if ((video < 0) || (2 < video))
  {
    return MAGVID_INVALID_ARGUMENT;
  }
  local.state = g_videos[video];
  if (local.state == NULL)
  {
    return MAGVID_OK;
  }
  local.player = local.state->player;
  if (local.player == NULL)
  {
    return MAGVID_INVALID_ARGUMENT;
  }
  if (local.state->background == NULL)
  {
    return MAGVID_INVALID_ARGUMENT;
  }

  move_video_destination(local.state, position->x, position->y);
  local.rect.left = 0;
  local.rect.top = 0;
  local.rect.right = 0;
  local.rect.bottom = 0;
  local.player->draw->get_draw_rect(&local.rect);
  position->x += local.state->x;
  position->y += local.state->y;
  local.rect.left += position->x;
  local.rect.top += position->y;
  local.rect.right += position->x;
  local.rect.bottom += position->y;
  local.player->draw->set_draw_rect(&local.rect);
  return MAGVID_OK;
}

// FUNCTION: MAGVID 0x10001C2E
int __cdecl DrawVidBackground(int video)
{
  struct
  {
    BITMAPINFOHEADER *header;
    HDRAWDIB draw_dib;
    HDC dc;
    int y;
    int x;
    int height;
    int width;
    void *bits;
    MagVid *state;
    MagDib *dib;
  } local;

  if ((video < 0) || (2 < video))
  {
    return MAGVID_INVALID_ARGUMENT;
  }
  local.state = g_videos[video];
  if (local.state == NULL)
  {
    return MAGVID_OK;
  }
  local.dib = local.state->background;
  if (local.dib == NULL)
  {
    return MAGVID_INVALID_ARGUMENT;
  }
  if (local.state->window != NULL)
  {
    local.draw_dib = DrawDibOpen();
    local.dc = GetDC(local.state->window);
    local.header = local.dib->get_header();
    local.x = local.state->x;
    local.y = local.state->y;
    local.width = local.dib->get_width();
    local.height = local.dib->get_height();
    local.bits = local.dib->get_bits();
    DrawDibDraw(local.draw_dib, local.dc, local.x, local.y, local.width,
                local.height, local.header, local.bits, 0, 0, local.width,
                local.height, 0);
    ReleaseDC(local.state->window, local.dc);
    DrawDibClose(local.draw_dib);
  }
  return MAGVID_OK;
}

// FUNCTION: MAGVID 0x10001D52
int __cdecl PasteToVidBackground(MagDib *dib, MagVidPosition *position,
                                 RECT *source, int video)
{
  MagVid *state;

  if ((dib == NULL) || (position == NULL) || (source == NULL))
  {
    return MAGVID_INVALID_ARGUMENT;
  }
  if ((video < 0) || (2 < video))
  {
    return MAGVID_INVALID_ARGUMENT;
  }
  state = g_videos[video];
  if ((state == NULL) || (state->background == NULL))
  {
    return MAGVID_NOT_LOADED;
  }
  dib->blit_to(state->background, position->x, position->y,
               source->right - source->left, source->bottom - source->top,
               source->left, source->top);
  return MAGVID_OK;
}

// FUNCTION: MAGVID 0x10001E22
int __cdecl SetVidForeground(MagDib *dib, int video)
{
  MagVid *state;
  AviDraw *draw;

  if ((video < 0) || (2 < video))
  {
    return MAGVID_INVALID_ARGUMENT;
  }
  state = g_videos[video];
  if (state == NULL)
  {
    return MAGVID_OK;
  }
  draw = state->player->draw;
  draw->set_foreground(dib);
  return MAGVID_OK;
}

// FUNCTION: MAGVID 0x10001F93
int __cdecl PaintVid(int video)
{
  struct
  {
    MagVid *state;
    AviPlayer *player;
  } local;

  if ((video < 0) || (2 < video))
  {
    return MAGVID_INVALID_ARGUMENT;
  }
  local.state = g_videos[video];
  if (local.state == NULL)
  {
    return MAGVID_OK;
  }
  local.player = local.state->player;
  if (local.player->is_open() == 0)
  {
    return MAGVID_ERROR;
  }
  if (local.player != NULL)
  {
    if (local.state->playing != 0)
    {
      return MAGVID_OK;
    }
    else
    {
      local.player->start_video(local.state->device_context, -1);
      local.player->render_next_frame();
      local.player->stop_video();
    }
  }
  return MAGVID_OK;
}

// FUNCTION: MAGVID 0x10005FC0
void __cdecl video_playback_thread(MagVid *state)
{
  struct
  {
    MagVid *initial_state;
    MagVid *current;
    int frame_rendered;
    AviPlayer *player;
    int unused;
    int audio_started;
  } local;

  local.initial_state = state;
  local.player = local.initial_state->player;
  local.unused = 1;
  local.frame_rendered = 0;
  local.audio_started = 0;
  if (local.player == NULL)
  {
    return;
  }
  local.current = local.initial_state;
  local.current->reserved_4c = 1;
  while ((local.current != NULL) && (local.current->thread_active != 0))
  {
    EnterCriticalSection(&local.current->lock);
    if (local.current->playing == 0)
    {
      LeaveCriticalSection(&local.current->lock);
      Sleep(5);
      continue;
    }
    else if (local.current->player->is_playback_finished() != 0)
    {
      local.current->playing = 0;
      LeaveCriticalSection(&local.current->lock);
      PostMessageA(local.current->player->notification_window, WM_COMMAND,
                   0x9c44, local.current->index);
      if (local.current->linked_video != NULL)
      {
        PostMessageA(local.current->player->notification_window, WM_COMMAND,
                     0x9c67, local.current->linked_video->index);
        local.current = local.current->linked_video;
      }
      else
      {
        local.current = NULL;
      }
      continue;
    }
    else
    {
      if (local.current->player->render_next_frame() < 0)
      {
      }
      LeaveCriticalSection(&local.current->lock);
      if ((local.frame_rendered != 0) && (local.audio_started == 0) &&
          (_delay != 0))
      {
        local.current->player->adjust_playback_clock(1000);
        sound_play(local.initial_state->index + 0x100, NULL);
        local.audio_started = 1;
        _delay = 0;
      }
      else if (local.audio_started == 0)
      {
        sound_play(local.initial_state->index + 0x100, NULL);
        local.audio_started = 1;
      }
      local.frame_rendered = 1;
      Sleep(1);
    }
  }
  sound_stop(local.initial_state->index + 0x100);
  sound_unload(local.initial_state->index + 0x100);
  local.initial_state->reserved_4c = 0;
}

// FUNCTION: MAGVID 0x1000663C
void __cdecl start_video_state(MagVid *state)
{
  struct
  {
    HANDLE thread;
    HDC dc;
    AviPlayer *player;
  } local;

  if ((state == NULL) || (state->player == NULL))
  {
    return;
  }
  local.player = state->player;
  if (local.player == NULL)
  {
    return;
  }
  if (local.player->is_open() == 0)
  {
    return;
  }
  if (state->playing != 0)
  {
    return;
  }
  state->thread_active = 1;
  if ((state->stop_requested == 0) && (state->thread == NULL) &&
      (((state->flags >> 3) & 1) == 0))
  {
    local.thread = CreateThread(NULL, 0,
        (LPTHREAD_START_ROUTINE)video_playback_thread, state, 0,
        &state->thread_id);
    state->thread = local.thread;
  }
  if (state->linked_video != NULL)
  {
    state->linked_video->flags = state->linked_video->flags | 8;
    state->linked_video->thread = state->thread;
    state->linked_video->thread_id = state->thread_id;
    state->linked_video->thread_active = 1;
  }
  local.dc = GetDC(state->window);
  local.player->start_playback(local.dc);
  SetThreadPriority(state->thread, THREAD_PRIORITY_ABOVE_NORMAL);
  EnterCriticalSection(&state->lock);
  state->stop_requested = 0;
  state->playing = 1;
  LeaveCriticalSection(&state->lock);
}

// FUNCTION: MAGVID 0x10006F86
int __cdecl apply_video_background(MagVid *state)
{
  struct
  {
    AviPlayer *player;
    int was_playing;
    MagDib *background;
    HWND window;
  } local;

  local.window = state->window;
  local.was_playing = 0;
  if ((state == NULL) || (state->player == NULL))
  {
    return MAGVID_INVALID_ARGUMENT;
  }
  local.player = state->player;
  local.background = state->background;
  if (local.background == NULL)
  {
    return MAGVID_INVALID_ARGUMENT;
  }
  if (local.player->is_open() == 0)
  {
    return MAGVID_ERROR;
  }
  if (state->playing != 0)
  {
    local.was_playing = 1;
    stop_video_state(state);
  }
  local.player->set_background_dib(local.background);
  if (local.was_playing != 0)
  {
    start_video_state(state);
  }
  PostMessageA(state->window, WM_USER + 1, 0, state->index);
  return MAGVID_OK;
}

// FUNCTION: MAGVID 0x10006597
void __cdecl realize_video_palette(MagVid *state, HWND excluded)
{
  AviPlayer *player;
  HPALETTE old_palette;
  HPALETTE palette;

  player = state->player;
  if ((player != NULL) && (player->draw != NULL) &&
      (state->window != excluded))
  {
    palette = (HPALETTE)player->draw->reserved_194;
    if (palette != NULL)
    {
      old_palette = SelectPalette(player->playback_dc, palette, FALSE);
      RealizePalette(player->playback_dc);
      if (old_palette != NULL)
      {
        SelectPalette(player->playback_dc, old_palette, FALSE);
      }
    }
  }
}

// FUNCTION: MAGVID 0x10006D16
void __cdecl refresh_video_palette(MagVid *state)
{
  AviPlayer *player;
  VideoPalette *palette;
  RGBQUAD *colors;
  int was_playing;
  int entry;

  if ((state == NULL) || (state->player == NULL))
  {
    return;
  }
  player = state->player;
  if (!player->is_open() || (player->background_dib == NULL))
  {
    return;
  }
  palette = new VideoPalette;
  if (palette == NULL)
  {
    return;
  }
  colors = (RGBQUAD *)mag_dib_get_palette(player->background_dib);
  for (entry = 0; entry < 0x100; entry++)
  {
    palette->entries[entry].peRed = colors[entry].rgbRed;
    palette->entries[entry].peGreen = colors[entry].rgbGreen;
    palette->entries[entry].peBlue = colors[entry].rgbBlue;
    palette->entries[entry].peFlags = 4;
  }
  palette->version = 0x300;
  palette->entry_count = 0x100;
  was_playing = state->playing;
  if (was_playing != 0)
  {
    stop_video_state(state);
  }
  if (player->set_codec_palette((LOGPALETTE *)palette) == 0)
  {
    realize_video_palette(state, NULL);
  }
  if (was_playing != 0)
  {
    start_video_state(state);
  }
  PostMessageA(state->window, WM_USER + 1, 0, state->index);
  delete palette;
}

// FUNCTION: MAGVID 0x10006EAD
int __cdecl load_video_background(MagVid *state, char *path)
{
  struct
  {
    AviPlayer *player;
    int was_playing;
    HWND window;
  } local;

  local.window = state->window;
  local.was_playing = 0;

  if ((state == NULL) || (state->player == NULL))
  {
    return MAGVID_INVALID_ARGUMENT;
  }
  local.player = state->player;
  if (!local.player->is_open())
  {
    return MAGVID_ERROR;
  }
  if (path == NULL)
  {
    return MAGVID_ERROR;
  }
  if (state->playing != 0)
  {
    local.was_playing = 1;
    stop_video_state(state);
  }
  local.player->load_background(path);
  if (local.was_playing != 0)
  {
    start_video_state(state);
  }
  PostMessageA(state->window, WM_USER + 1, 0, state->index);
  return MAGVID_OK;
}

// FUNCTION: MAGVID 0x100072F2
void __cdecl center_video_surface(MagVid *state)
{
  struct
  {
    RECT window;
    RECT centered;
    RECT present;
  } local;

  GetWindowRect(state->window, &local.window);
  state->player->draw->get_present_rect(&local.present);
  local.centered.left =
      (local.window.right - local.window.left) / 2 -
      (local.present.right - local.present.left) / 2;
  local.centered.right =
      (local.present.right - local.present.left) + local.centered.left;
  local.centered.top =
      (local.window.bottom - local.window.top) / 2 -
      (local.present.bottom - local.present.top) / 2;
  local.centered.bottom =
      (local.present.bottom - local.present.top) + local.centered.top;
  state->player->draw->set_present_rect(&local.centered);
  PostMessageA(state->window, WM_USER + 1, 0, state->index);
}

// FUNCTION: MAGVID 0x100067A8
void __cdecl stop_video_state(MagVid *state)
{
  struct
  {
    HANDLE process;
    HDC dc;
    AviPlayer *player;
  } local;

  local.player = state->player;
  if (local.player != NULL)
  {
    if (local.player->is_open() != 0)
    {
      EnterCriticalSection(&state->lock);
      state->stop_requested = 0;
      state->playing = 0;
      local.dc = local.player->playback_dc;
    local.player->stop_playback();
      if (state->linked_video == NULL)
      {
        local.process = GetCurrentProcess();
        SetPriorityClass(local.process, NORMAL_PRIORITY_CLASS);
        if (state->thread_active != 0)
        {
          state->thread_active = 0;
        }
        LeaveCriticalSection(&state->lock);
        CloseHandle(state->thread);
      }
      else
      {
        LeaveCriticalSection(&state->lock);
      }
      local.player->clear_thread_state();
      if (local.dc != NULL)
      {
        ReleaseDC(state->window, local.dc);
      }
    }
  }
}

// FUNCTION: MAGVID 0x10006442
void __cdecl unload_video_state(MagVid *state)
{
  AviPlayer *player;

  if ((state != NULL) && (state->player != NULL))
  {
    player = state->player;
    if (player != NULL)
    {
      if (player->is_open() == 0)
      {
        return;
      }
      if (((state->flags >> 1) & 1) != 0)
      {
        stop_video_state(state);
      }
      if (state->thread != NULL)
      {
        state->thread_active = 0;
        state->thread = NULL;
      }
      EnterCriticalSection(&state->lock);
      player->close_streams();
      player->close_file();
      LeaveCriticalSection(&state->lock);
    }
    DeleteCriticalSection(&state->lock);
    PostMessageA(state->window, WM_USER + 1, 0, state->index);
  }
}

// FUNCTION: MAGVID 0x1000630B
int __cdecl load_video_file(MagVid *state, const char *path)
{
  struct
  {
    HFILE file;
    int result;
    AviPlayer *player;
  } local;

  if ((state == NULL) || (state->player == NULL))
  {
    return MAGVID_INVALID_ARGUMENT;
  }
  local.player = state->player;
  local.file = _lopen(path, OF_READ);
  if (local.file == HFILE_ERROR)
  {
    return MAGVID_ERROR;
  }
  state->reserved_60 = GetFileSize((HANDLE)local.file, NULL);
  state->reserved_60 /= 1024;
  _lclose(local.file);
  if (local.player->is_open() != 0)
  {
    unload_video_state(state);
  }
  local.result = local.player->open_file(path);
  if (local.result != 0)
  {
    return MAGVID_ERROR;
  }
  local.result = local.player->open_streams(state->window, state->index);
  if (local.result != 0)
  {
    return MAGVID_ERROR;
  }
  state->flags = state->flags | 1;
  PostMessageA(state->window, WM_USER + 1, 0, state->index);
  InitializeCriticalSection(&state->lock);
  return MAGVID_OK;
}
