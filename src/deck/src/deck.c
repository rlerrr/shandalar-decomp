#include <windows.h>

typedef unsigned int undefined4;
typedef unsigned char undefined1;
typedef void * pointer;

// GLOBAL: DECK 0x00404410
char s_Magic__The_Gathering_00404410[] = "Magic: The Gathering";

// GLOBAL: DECK 0x00404428
char s__MTGshell_00404428[] = "/MTGshell";

// GLOBAL: DECK 0x00404440
undefined4 DAT_00404440 = 0x00000000;

// GLOBAL: DECK 0x00404444
undefined4 DAT_00404444 = 0x00000000;

// GLOBAL: DECK 0x00404448
undefined4 DAT_00404448 = 0x00000001;

// GLOBAL: DECK 0x0040444C
undefined4 DAT_0040444c = 0x00000000;

// GLOBAL: DECK 0x00404450
undefined4 DAT_00404450 = 0x00000000;

// GLOBAL: DECK 0x00404564
undefined4 DAT_00404564 = 0x00000000;

// LIBRARY: DECK 0x00401000 SYMBOL
// thunk_FUN_00401010

// FUNCTION: DECK 0x00401010
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE prevInstance,PSTR cmdLine)
{
  HWND hWnd;
  int iVar1;
  
  hWnd = FindWindowA((LPCSTR)0x0,s_Magic__The_Gathering_00404410);
  if ((hWnd != (HWND)0x0) && (iVar1 = _strnicmp(cmdLine,s__MTGshell_00404428,9), iVar1 != 0)) {
    PostMessageA(hWnd,0x400,3,0);
    return 0;
  }
  DeckBuilderMain(0,2,1);
  return 0;
}

// LIBRARY: DECK 0x004010AA SYMBOL
// DeckBuilderMain


// LIBRARY: DECK 0x00401266 SYMBOL
// _XcptFilter

// LIBRARY: DECK 0x00401278 SYMBOL
// initterm

// LIBRARY: DECK 0x00401290 SYMBOL
// __setdefaultprecision

// FUNCTION: DECK 0x004012C0
void FUN_004012c0(void)

{
  return;
}

// LIBRARY: DECK 0x00401390 SYMBOL
// _controlfp

