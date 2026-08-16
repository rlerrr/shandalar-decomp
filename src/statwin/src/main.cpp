#include <windows.h>

typedef unsigned char byte;
typedef unsigned int uint;

typedef struct StatWinData_t
{
  int progress_by_color[5];
  int duel_wins_by_color[5];
  unsigned char town_count_by_color[5];
  unsigned char highlighted_wizard_color;
  unsigned char highlighted_creature_type;
  unsigned char highlighted_lair_color;
} StatWinData;

typedef struct AviPosition_t
{
  short x;
  short y;
} AviPosition;

typedef struct StatWinState_t
{
  int field_00;
  int field_04;
  int enabled;
  int field_0c;
  int field_10;
  int field_14;
  int field_18;
  int field_1c;
  int has_window;
  int field_24;
  int apply_progress(StatWinData *data, int flags);
  int prepare_resources(void);
  int choose_mode(int *mode, StatWinData *data, int *selection);
  void update(StatWinData *data, int mode);
  void write_cache(void);
  void release_cache(void);
} StatWinState;

extern "C" int init_sound_dll(HWND hwnd, int unused, int flags);
extern "C" void clear_sound_imports_table(void);

static void __cdecl reset_status_assets(void);
static void __cdecl show_status_window(void);
static void __cdecl show_world_magic_detail(StatWinData *data);
static void __cdecl show_color_missing_message(uint color);
extern "C" HWND __cdecl get_sound_hwnd(void);
static int __cdecl init_video_dll(int hwnd, HINSTANCE instance, int flags);
static int __cdecl release_video_dll(void);
static void __cdecl clear_video_imports_table(void);
static int __cdecl load_avi(char *path, int *out_avi, AviPosition *position, uint flags);
static int __cdecl play_avi(int avi);
static int __cdecl stop_avi(int avi);
static int __cdecl unload_avi(int avi);
static int __cdecl set_vid_transparency(int avi, int transparency);
static int __cdecl vid_status(int avi);
static int __cdecl poll_statwin_messages(void);
static BOOL CALLBACK statwin_help_dialog_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
static LRESULT CALLBACK status_video_wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

// GLOBAL: STATWIN 0x1000d02c
static int g_detail_window_enabled;
// GLOBAL: STATWIN 0x1000d26c
static int g_sound_available;
// GLOBAL: STATWIN 0x1000d270
static int g_video_available;
// GLOBAL: STATWIN 0x1000d274
static int g_status_initialized;
// GLOBAL: STATWIN 0x1000d280
static char *g_statwin_class_name_ptr;
// GLOBAL: STATWIN 0x1000d284
static StatWinState *g_status_state;
// GLOBAL: STATWIN 0x1000d684
static char g_statwin_class_name[] = "STATWINCLASS";
// GLOBAL: STATWIN 0x1000e514
static HINSTANCE g_statwin_instance;
// GLOBAL: STATWIN 0x1000e524
static char g_status_flag_byte;
// GLOBAL: STATWIN 0x1000e52c
static int g_help_dialog_enabled;
// GLOBAL: STATWIN 0x1000d020
static char *g_status_flag_ptr;

typedef int (__cdecl *MagVidFn)(void);
// GLOBAL: STATWIN 0x10017d8c
static HMODULE g_magvid_module;
// GLOBAL: STATWIN 0x1000d718
static int g_video_dll_status;
// GLOBAL: STATWIN 0x1000d71c
static char *PTR_s_magvid_1000d71c = "magvid";
// GLOBAL: STATWIN 0x10017d40
static MagVidFn g_magvid_fns[19];
// GLOBAL: STATWIN 0x10017d90
static MagVidFn g_magvid_delay_fn;

// FUNCTION: STATWIN 0x10001000
extern "C" BOOL WINAPI statwin_dll_main(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
  WNDCLASSA window_class;
  int result;

  (void)reserved;
  switch (reason)
  {
  case DLL_PROCESS_DETACH:
    if (g_status_state != 0)
    {
      g_status_state->release_cache();
      g_status_state = 0;
    }
    if (g_sound_available != 0)
    {
      /* Existing shared sound wrapper owns the exact STATWIN 0x10002da0 body. */
    }
    if (g_video_available != 0)
    {
      release_video_dll();
    }
    UnregisterClassA(g_statwin_class_name, g_statwin_instance);
    break;

  case DLL_PROCESS_ATTACH:
    g_statwin_instance = instance;
    g_status_state = (StatWinState *)operator new(sizeof(StatWinState));
    if (g_status_state == 0)
    {
      return FALSE;
    }
    ZeroMemory(g_status_state, sizeof(StatWinState));
    result = init_video_dll(0, g_statwin_instance, 0);
    g_video_available = (result == 0);
    result = init_sound_dll(0, 0, 3);
    g_sound_available = (result == 0);

    ZeroMemory(&window_class, sizeof(window_class));
    window_class.style = CS_VREDRAW | CS_HREDRAW;
    window_class.lpfnWndProc = status_video_wndproc;
    window_class.hInstance = g_statwin_instance;
    window_class.hIcon = LoadIconA(0, IDI_APPLICATION);
    window_class.hCursor = LoadCursorA(0, IDC_ARROW);
    window_class.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    window_class.lpszClassName = g_statwin_class_name;
    RegisterClassA(&window_class);
    break;

  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
    break;
  }

  return TRUE;
}

// FUNCTION: STATWIN 0x10001273
extern "C" int __cdecl status_init(StatWinData *data)
{
  int result;

  result = g_status_state->apply_progress(data, 0);
  if (result != 0)
  {
    g_status_initialized = 0;
    return result;
  }
  else
  {
    g_status_initialized = 1;
    reset_status_assets();
    *g_status_flag_ptr = g_status_flag_byte;
    return 0;
  }
}

// FUNCTION: STATWIN 0x100012d9
extern "C" void __cdecl status_show(StatWinData *data, int mode)
{
  int selection;
  int result;

  if (g_video_available == 0)
  {
    return;
  }
  if (g_status_state->has_window == 0)
  {
    return;
  }

  result = g_status_state->prepare_resources();
  if (result != 0)
  {
    g_status_state->has_window = 0;
    return;
  }

  result = g_status_state->choose_mode(&mode, data, &selection);
  if (result == 0)
  {
    mode = 0;
  }

  switch (mode)
  {
  case 0:
    g_status_state->update(data, mode);
    show_status_window();
    return;

  case 1:
    if (*(int *)((char *)data + data->highlighted_wizard_color * 4) == 0)
    {
      show_color_missing_message(data->highlighted_wizard_color);
      if (data->highlighted_wizard_color < 5)
      {
        g_status_state->update(data, mode);
      }
    }
    else if (g_detail_window_enabled == 0)
    {
      show_world_magic_detail(data);
    }
    else
    {
      g_status_state->update(data, mode);
      show_status_window();
    }
    break;

  case 2:
    if (g_detail_window_enabled == 0)
    {
      show_world_magic_detail(data);
    }
    else
    {
      g_status_state->update(data, mode);
      show_status_window();
    }
    break;

  case 3:
    g_status_state->update(data, 3);
    show_status_window();
    break;

  default:
    return;
  }

  g_status_state->write_cache();
  g_status_state->release_cache();
}

// FUNCTION: STATWIN 0x10002958
extern "C" int __cdecl play_video(char *path, int x, int y, int flags)
{
  struct
  {
    int result;
    AviPosition position;
    int avi;
    int screen_height;
    uint load_flags;
    int screen_width;
    HWND parent;
    HWND video_window;
  } s;

  s.load_flags = 0;
  s.avi = 0;
  if (g_sound_available != 0)
  {
    s.parent = get_sound_hwnd();
  }
  else
  {
    s.parent = 0;
  }

  if ((flags & 1) != 0)
  {
    s.screen_width = GetSystemMetrics(SM_CXSCREEN);
    s.screen_height = GetSystemMetrics(SM_CYSCREEN);
    s.video_window = CreateWindowExA(0, g_statwin_class_name_ptr, 0, WS_POPUP, 0, 0, s.screen_width, s.screen_height, s.parent, 0, g_statwin_instance, 0);
    if (s.video_window == 0)
    {
      return 8;
    }
    if (SetWindowLongA(s.video_window, GWL_WNDPROC, (LONG)status_video_wndproc) == 0)
    {
      DestroyWindow(s.video_window);
      return 8;
    }
    ShowWindow(s.video_window, SW_SHOW);
  }

  if ((flags & 2) != 0)
  {
    s.load_flags |= 2;
    s.position.x = 0;
    s.position.y = 0;
  }
  else
  {
    s.position.x = x;
    s.position.y = y;
  }

  s.result = load_avi(path, &s.avi, &s.position, s.load_flags);
  if (s.result != 0)
  {
    DestroyWindow(s.video_window);
    return s.result;
  }

  set_vid_transparency(s.avi, 0);
  play_avi(s.avi);
  while (vid_status(s.avi) != 0)
  {
    if (poll_statwin_messages() != 0)
    {
      stop_avi(s.avi);
      break;
    }
    Sleep(50);
  }
  unload_avi(s.avi);
  s.avi = 0;
  DestroyWindow(s.video_window);
  return 0;
}

// FUNCTION: STATWIN 0x10003610
static int __cdecl init_video_dll(int hwnd, HINSTANCE instance, int flags)
{
  int i;
  int result;

  result = 0;
  if (g_video_dll_status != 0)
  {
    return 6;
  }

  g_magvid_module = LoadLibraryA(PTR_s_magvid_1000d71c);
  if (g_magvid_module != 0)
  {
    for (i = 0; i < 19; i++)
    {
      g_magvid_fns[i] = (MagVidFn)GetProcAddress(g_magvid_module, (LPCSTR)((i + 1U) & 0xffff));
      if (g_magvid_fns[i] == 0)
      {
        FreeLibrary(g_magvid_module);
        clear_video_imports_table();
        return 7;
      }
    }

    g_magvid_delay_fn = (MagVidFn)GetProcAddress(g_magvid_module, (LPCSTR)((i + 1U) & 0xffff));
  }
  else
  {
    return 7;
  }

  result = ((int (__cdecl *)(int, HINSTANCE, int))g_magvid_fns[0])(hwnd, instance, flags);
  if (result != 0)
  {
    FreeLibrary(g_magvid_module);
    clear_video_imports_table();
    return result;
  }

  g_video_dll_status = 1;
  return 0;
}

// FUNCTION: STATWIN 0x1000373a
static int __cdecl release_video_dll(void)
{
  if (g_video_dll_status == 0)
  {
    return 7;
  }

  g_video_dll_status = 0;
  ((void (__cdecl *)(void))g_magvid_fns[1])();
  FreeLibrary(g_magvid_module);
  clear_video_imports_table();
  return 0;
}

// FUNCTION: STATWIN 0x10003bae
static void __cdecl clear_video_imports_table(void)
{
  int i;

  for (i = 0; i < 19; i++)
  {
    g_magvid_fns[i] = 0;
  }
}

// FUNCTION: STATWIN 0x10003784
static int __cdecl load_avi(char *path, int *out_avi, AviPosition *position, uint flags)
{
  if (g_video_dll_status == 0)
  {
    return 7;
  }
  return ((int (__cdecl *)(char *, int *, AviPosition *, uint))g_magvid_fns[2])(path, out_avi, position, flags);
}

// FUNCTION: STATWIN 0x100037c4
static int __cdecl unload_avi(int avi)
{
  if (g_video_dll_status == 0)
  {
    return 7;
  }
  return ((int (__cdecl *)(int))g_magvid_fns[3])(avi);
}

// FUNCTION: STATWIN 0x100037f8
static int __cdecl play_avi(int avi)
{
  if ((g_video_dll_status == 0) || (g_video_dll_status == 2))
  {
    return 7;
  }
  return ((int (__cdecl *)(int))g_magvid_fns[4])(avi);
}

// FUNCTION: STATWIN 0x10003839
static int __cdecl stop_avi(int avi)
{
  if ((g_video_dll_status == 0) || (g_video_dll_status == 2))
  {
    return 7;
  }
  return ((int (__cdecl *)(int))g_magvid_fns[5])(avi);
}

// FUNCTION: STATWIN 0x10003a5d
static int __cdecl set_vid_transparency(int avi, int transparency)
{
  if ((g_video_dll_status == 0) || (g_video_dll_status == 2))
  {
    return 7;
  }
  return ((int (__cdecl *)(int, int))g_magvid_fns[18])(avi, transparency);
}

// FUNCTION: STATWIN 0x10003ae3
static int __cdecl vid_status(int avi)
{
  if ((g_video_dll_status == 0) || (g_video_dll_status == 2))
  {
    return 7;
  }
  return ((int (__cdecl *)(int))g_magvid_fns[15])(avi);
}

int StatWinState_t::apply_progress(StatWinData *data, int flags)
{
  (void)data;
  (void)flags;
  return 0;
}

static void __cdecl reset_status_assets(void)
{
}

int StatWinState_t::prepare_resources(void)
{
  return 0;
}

int StatWinState_t::choose_mode(int *mode, StatWinData *data, int *selection)
{
  (void)mode;
  (void)data;
  (void)selection;
  return 1;
}

void StatWinState_t::update(StatWinData *data, int mode)
{
  (void)data;
  (void)mode;
}

static void __cdecl show_status_window(void)
{
}

static void __cdecl show_world_magic_detail(StatWinData *data)
{
  (void)data;
}

static void __cdecl show_color_missing_message(uint color)
{
  (void)color;
}

void StatWinState_t::write_cache(void)
{
}

void StatWinState_t::release_cache(void)
{
  operator delete(this);
}

// FUNCTION: STATWIN 0x1000152c
static int __cdecl poll_statwin_messages(void)
{
  MSG msg;
  int result;

  result = 0;
  if (PeekMessageA(&msg, 0, 0, 0, PM_REMOVE) != 0)
  {
    TranslateMessage(&msg);
    if ((msg.message == WM_RBUTTONUP) && (g_help_dialog_enabled != 0))
    {
      DialogBoxParamA(g_statwin_instance, (LPCSTR)0x65, get_sound_hwnd(), (DLGPROC)statwin_help_dialog_proc, 0);
    }
    if ((msg.message == WM_LBUTTONUP) || (msg.message == WM_KEYDOWN))
    {
      result = 1;
    }
    DispatchMessageA(&msg);
  }
  return result;
}

// FUNCTION: STATWIN 0x10002713
static BOOL CALLBACK statwin_help_dialog_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  (void)hwnd;
  (void)msg;
  (void)wparam;
  (void)lparam;
  return FALSE;
}

// FUNCTION: STATWIN 0x10002b15
static LRESULT CALLBACK status_video_wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  return DefWindowProcA(hwnd, msg, wparam, lparam);
}
