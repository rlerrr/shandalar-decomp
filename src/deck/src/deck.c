#include <windows.h>

extern WPARAM WINAPI DeckBuilderMain(HWND parent_hwnd, int db_flags_1, int db_flags_2);

// FUNCTION: DECK 0x00401010
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int nShowCmd)
{
  HWND hWnd;

  hWnd = FindWindowA((LPCSTR)0x0, "Magic: The Gathering");
  if (hWnd != (HWND)0x0 && _strnicmp(cmdLine, "/MTGshell", 9))
  {
    PostMessageA(hWnd, 0x400, 3, 0);
    return 0;
  }
  DeckBuilderMain(0, 2, 1);
  return 0;
}
