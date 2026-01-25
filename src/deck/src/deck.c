#include <windows.h>

// GLOBAL: DECK 0x00404410
char s_Magic__The_Gathering_00404410[] = "Magic: The Gathering";

// GLOBAL: DECK 0x00404428
char s__MTGshell_00404428[] = "/MTGshell";

extern WPARAM WINAPI DeckBuilderMain(HWND parent_hwnd, int db_flags_1, int db_flags_2);

// FUNCTION: DECK 0x00401010
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE prevInstance,PSTR cmdLine,int nShowCmd)
{
  HWND hWnd;
  
  hWnd = FindWindowA((LPCSTR)0x0,s_Magic__The_Gathering_00404410);
  if (hWnd != (HWND)0x0 && _strnicmp(cmdLine,s__MTGshell_00404428,9)) {
    PostMessageA(hWnd,0x400,3,0);
    return 0;
  }
  DeckBuilderMain(0,2,1);
  return 0;
}
