#include "WINDOWS.H"

extern HPALETTE global_cart_art_hpalette;

// GLOBAL: DECKDLL 0x1003628c
const char s_Button_1003628c[] = "Button";

// GLOBAL: DECKDLL 0x1011279c
static WNDPROC global_wndproc_std_ButtonClass;

// FUNCTION: DECKDLL 0x10025d1b
// FUNCTION: MAGIC 0x00496489
static BOOL CALLBACK FUN_10025d1b(HWND child_hwnd, LPARAM lparam)
{
  int *args;

  args = (int *)lparam;
  if (GetParent(child_hwnd) == (HWND)*args)
    SendMessageA(child_hwnd, args[1], args[2], args[3]);
  return TRUE;
}

// FUNCTION: DECKDLL 0x10025b5e
// FUNCTION: MAGIC 0x004962cc
static int FUN_10025b5e(HWND hwnd, UINT msg, HWND wparam_hwnd, LPARAM lparam)
{
  struct
  {
    HDC hdc2; // ebp - 0x38
    struct
    {
      HWND hwnd;     // ebp - 0x34
      UINT msg;      // ebp - 0x30
      HWND wparam;   // ebp - 0x2c
      LPARAM lparam; // ebp - 0x28
      int pad1;
      int pad2;
      int pad3;
    } args;

    DWORD tid_dst;    // ebp - 0x18
    HDC hdc;          // ebp - 0x14
    DWORD tid_src;    // ebp - 0x10
    DWORD procid_dst; // ebp - 0xc
    HWND hwnd;        // ebp - 0x8
    int procid_src;   // ebp - 0x4
  } s;

  switch (msg)
  {
  case WM_PALETTEISCHANGING:
  case WM_PALETTECHANGED:
    s.hwnd = wparam_hwnd;
    if (s.hwnd != hwnd)
    {
      s.tid_src = GetWindowThreadProcessId(s.hwnd, &s.procid_src);
      s.tid_dst = GetWindowThreadProcessId(hwnd, &s.procid_dst);

      if (s.procid_src != s.procid_dst)
      {
        InvalidateRect(hwnd, NULL, TRUE);
      }
      else if (GetParent(hwnd) == NULL)
      {
        s.hdc = GetDC(hwnd);
        SelectPalette(s.hdc, global_cart_art_hpalette, TRUE);
        InvalidateRect(hwnd, NULL, TRUE);
        ReleaseDC(hwnd, s.hdc);
      }
      else if (!(GetWindowLongA(hwnd, GWL_STYLE) & WS_CHILD))
      {
        InvalidateRect(hwnd, NULL, TRUE);
      }
    }
    if (msg == WM_PALETTECHANGED)
    {
      s.args.hwnd = hwnd;
      s.args.msg = msg;
      s.args.wparam = wparam_hwnd;
      s.args.lparam = lparam;
      EnumChildWindows(hwnd, FUN_10025d1b, (LPARAM)&s.args);
    }
    return 0;

  case WM_QUERYNEWPALETTE:
    UnrealizeObject(global_cart_art_hpalette);
    s.hdc2 = GetDC(hwnd);
    SelectPalette(s.hdc2, global_cart_art_hpalette, FALSE);

    if (RealizePalette(s.hdc2))
      InvalidateRect(hwnd, NULL, TRUE);

    ReleaseDC(hwnd, s.hdc2);
    return 1;
  default:
    return 0;
  }
}

// FUNCTION: DECKDLL 0x100256c8
// FUNCTION: MAGIC 0x00495e36
static int is_buttonclass(HWND hwnd)
{
  char classname[0x64];

  if (hwnd == (HWND)0)
    return 0;

  GetClassNameA(hwnd, classname, 0x64);
  if (!_strcmpi(classname, s_Button_1003628c))
    return 1;
  else
    return 0;
}

// FUNCTION: DECKDLL 0x10025593
// FUNCTION: MAGIC 0x00495d01
LRESULT CALLBACK wndproc_ButtonClass(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  struct
  {
    HWND local_10;
    HWND local_c;
    LRESULT local_8;
  } s;

  if ((msg == WM_SETFOCUS) || (msg == WM_KILLFOCUS))
  {
    if (msg == WM_SETFOCUS)
    {
      s.local_c = hwnd;
      s.local_10 = (HWND)wparam;
    }
    else
    {
      s.local_10 = hwnd;
      s.local_c = (HWND)wparam;
    }
    if (is_buttonclass(s.local_c) == 0)
      s.local_c = (HWND)0;
    if (is_buttonclass(s.local_10) == 0)
      s.local_10 = (HWND)0;
    SendMessageA(GetParent(hwnd), WM_USER + 0xC8, (WPARAM)s.local_c, (LPARAM)s.local_10);
    s.local_8 = 0;
  }
  else if ((msg == WM_PALETTECHANGED) || (msg == WM_PALETTEISCHANGING) || (msg == WM_QUERYNEWPALETTE))
  {
    s.local_8 = CallWindowProcA(global_wndproc_std_ButtonClass, hwnd, msg, wparam, lparam);
    FUN_10025b5e(hwnd, msg, (HWND)wparam, lparam);
  }
  else
    s.local_8 = CallWindowProcA(global_wndproc_std_ButtonClass, hwnd, msg, wparam, lparam);

  return s.local_8;
}

// FUNCTION: DECKDLL 0x10025552
// FUNCTION: MAGIC 0x00495cc0
BOOL CALLBACK enumfunc_change_buttonclass_wndproc(HWND hwnd, LPARAM lparam)
{
  if (is_buttonclass(hwnd))
    global_wndproc_std_ButtonClass = SetWindowLong(hwnd, GWL_WNDPROC, wndproc_ButtonClass);
  return TRUE;
}

// FUNCTION: DECKDLL 0x10025536
// FUNCTION: MAGIC 0x00495ca4
void change_buttonclass_wndproc(HWND hwnd)
{
  EnumChildWindows(hwnd, enumfunc_change_buttonclass_wndproc, 0);
}
