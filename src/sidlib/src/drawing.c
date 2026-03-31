#include <WINDOWS.H>
#include "cardartlib/src/palette.h"

// FUNCTION: DRAWCARDLIB 0x10007296
// FUNCTION: DECKDLL 0x10015fe0
// FUNCTION: MAGIC 0x0055d9ed
int GetNextManaSymbol(char **param_1)
{
  struct {
    char result;
    char pad[3];
    char *chr;
  } s;
  
  s.chr = (char *)*param_1;
  s.result = '\0';
  if (*s.chr == '|') {
    if (s.chr[1] == 'X') {
      s.result = -0x10;
    }
    else if ((s.chr[1] == '1') && (s.chr[2] == '0')) {
      s.result = -0x11;
      s.chr = s.chr + 1;
    }
    else if (s.chr[1] == '0') {
      s.result = -0xf;
    }
    else if (s.chr[1] == '1') {
      s.result = -0xe;
    }
    else if (s.chr[1] == '2') {
      s.result = -0xd;
    }
    else if (s.chr[1] == '3') {
      s.result = -0xc;
    }
    else if (s.chr[1] == '4') {
      s.result = -0xb;
    }
    else if (s.chr[1] == '5') {
      s.result = -10;
    }
    else if (s.chr[1] == '6') {
      s.result = -9;
    }
    else if (s.chr[1] == '7') {
      s.result = -8;
    }
    else if (s.chr[1] == '8') {
      s.result = -7;
    }
    else if (s.chr[1] == '9') {
      s.result = -6;
    }
    else if (s.chr[1] == 'T') {
      s.result = -0x12;
    }
    else if (s.chr[1] == 'B') {
      s.result = -2;
    }
    else if (s.chr[1] == 'U') {
      s.result = -3;
    }
    else if (s.chr[1] == 'W') {
      s.result = -5;
    }
    else if (s.chr[1] == 'G') {
      s.result = -1;
    }
    else if (s.chr[1] == 'R') {
      s.result = -4;
    }
  }
  if (s.result != '\0') {
    s.chr++;
    s.chr++;
  }
  *param_1 = s.chr;
  return (int)s.result;
}

// FUNCTION: DRAWCARDLIB 0x1000aab9
// FUNCTION: DECKDLL 0x10023b0b
// FUNCTION: MAGIC 0x00494270
BOOL DrawMaskedBitmapToRect(HDC dc,RECT *rect,HANDLE param_3,int wSrc,int hSrc,int param_6,int param_7
            ,int param_8,int param_9)

{
  struct {
    int hDest;
    BITMAP bm;
    int wDest;
    int nSavedDC;
    int yDest;
    int xDest;
  } s;
  
  if (((dc == (HDC)0x0) || (rect == (RECT *)0x0)) || (param_3 == (HANDLE)0x0)) {
    return 0;
  }
  EnterCriticalSection(&global_critical_section_for_drawing);
  s.nSavedDC = SaveDC(dc);
  SelectObject(global_screen_dc,param_3);
  GetObjectA(param_3,0x18,&s.bm);
  s.xDest = rect->left;
  s.yDest = rect->top;
  if (rect->right >= rect->left) {
    s.wDest = rect->right - rect->left;
  }
  else {
    s.wDest = wSrc;
  }
  if (rect->top <= rect->bottom) {
    s.hDest = rect->bottom - rect->top;
  }
  else {
    s.hDest = hSrc;
  }
  ApplyCardArtPaletteToDc(global_screen_dc);
  StretchBlt(dc,s.xDest,s.yDest,s.wDest,s.hDest,global_screen_dc,param_8,param_9,wSrc,hSrc,SRCAND);
  ApplyCardArtPaletteToDc(global_screen_dc);
  StretchBlt(dc,s.xDest,s.yDest,s.wDest,s.hDest,global_screen_dc,param_6,param_7,wSrc,hSrc,SRCPAINT);
  RestoreDC(dc,s.nSavedDC);
  LeaveCriticalSection(&global_critical_section_for_drawing);
  return 1;
}
