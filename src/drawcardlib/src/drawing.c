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
// FUNCTION: SHANDALAR 0x00464d0b
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

// FUNCTION: DECKDLL 0x10023972
// FUNCTION: MAGIC 0x004940d6
// FUNCTION: SHANDALAR 0x00464b72
BOOL TileBitmapIntoRect(HDC hdc, RECT *r, HBITMAP bmp)
{
  struct
  {
    BITMAP bm;
    RECT dst;
    int y;
    int saved;
    int x;
  } s;

  if (!hdc || !r || !bmp)
    return FALSE;

  s.saved = SaveDC(hdc);
  IntersectClipRect(hdc, r->left, r->top, r->right, r->bottom);

  GetObjectA(bmp, 0x18, &s.bm);

  for (s.x = r->left; s.x < r->right; s.x += s.bm.bmWidth)
  {
    for (s.y = r->top; s.y < r->bottom; s.y += s.bm.bmHeight)
    {
      SetRect(&s.dst, s.x, s.y, s.x - 1, s.y - 1);
      DrawBitmapSubrectToRect(hdc, &s.dst, bmp, 0, 0, s.bm.bmWidth, s.bm.bmHeight);
    }
  }

  RestoreDC(hdc, s.saved);
  return TRUE;
}

// FUNCTION: DECKDLL 0x10024ed5
// FUNCTION: MAGIC 0x0049563b
void draw_item(DRAWITEMSTRUCT *item, HBRUSH brush, HANDLE hbmp_bkgrd, HPEN pen1, HPEN pen2, COLORREF col, int do_focus, UINT format)
{
  struct
  {
    HDC hdc;       /* ebp - 0x64 */
    RECT focus;    /* ebp - 0x60 */
    HGDIOBJ hfont; /* ebp - 0x50 */
    RECT r;        /* ebp - 0x4c */
    char txt[52];  /* ebp - 0x3c */
    SIZE sz;       /* ebp - 0xc */
  } s;

  s.hdc = item->hDC;
  CopyRect(&s.r, &item->rcItem);
  GetWindowText(item->hwndItem, s.txt, 0x32);
  ApplyCardArtPaletteToDc(s.hdc);

  OffsetRect(&s.r, -item->rcItem.left, -item->rcItem.top);

  if (item->itemState & ODS_SELECTED)
  {
    if (brush)
      FillRect(s.hdc, &s.r, brush);
    else if (hbmp_bkgrd)
      TileBitmapIntoRect(s.hdc, &s.r, hbmp_bkgrd);

    SelectObject(s.hdc, GetStockObject(BLACK_PEN));
    MoveToEx(s.hdc, 0, 0, NULL);
    LineTo(s.hdc, s.r.right, 0);
    MoveToEx(s.hdc, 0, 0, NULL);
    LineTo(s.hdc, 0, s.r.bottom);

    SelectObject(s.hdc, pen2);
    MoveToEx(s.hdc, 1, 1, NULL);
    LineTo(s.hdc, s.r.right - 1, 1);
    MoveToEx(s.hdc, 1, 1, NULL);
    LineTo(s.hdc, 1, s.r.bottom - 1);

    SelectObject(s.hdc, pen1);
    MoveToEx(s.hdc, s.r.right - 1, 1, NULL);
    LineTo(s.hdc, s.r.right - 1, s.r.bottom);
    MoveToEx(s.hdc, 1, s.r.bottom - 1, NULL);
    LineTo(s.hdc, s.r.right, s.r.bottom - 1);

    OffsetRect(&s.r, 2, 2);
  }
  else
  {
    if (brush)
      FillRect(s.hdc, &s.r, brush);
    else if (hbmp_bkgrd)
      TileBitmapIntoRect(s.hdc, &s.r, hbmp_bkgrd);

    SelectObject(s.hdc, pen1);
    MoveToEx(s.hdc, 0, 0, NULL);
    LineTo(s.hdc, s.r.right, 0);
    MoveToEx(s.hdc, 0, 0, NULL);
    LineTo(s.hdc, 0, s.r.bottom);
    MoveToEx(s.hdc, 1, 1, NULL);
    LineTo(s.hdc, s.r.right - 1, 1);
    MoveToEx(s.hdc, 1, 1, NULL);
    LineTo(s.hdc, 1, s.r.bottom - 1);

    SelectObject(s.hdc, pen2);
    MoveToEx(s.hdc, s.r.right - 1, 1, NULL);
    LineTo(s.hdc, s.r.right - 1, s.r.bottom);
    MoveToEx(s.hdc, 1, s.r.bottom - 1, NULL);
    LineTo(s.hdc, s.r.right, s.r.bottom - 1);
    MoveToEx(s.hdc, s.r.right - 2, 2, NULL);
    LineTo(s.hdc, s.r.right - 2, s.r.bottom - 1);
    MoveToEx(s.hdc, 2, s.r.bottom - 2, NULL);
    LineTo(s.hdc, s.r.right - 1, s.r.bottom - 2);
  }

  SetBkMode(s.hdc, TRANSPARENT);
  SetTextColor(s.hdc, col);
  s.hfont = (HGDIOBJ)SendMessageA(item->hwndItem, WM_GETFONT, 0, 0);
  SelectObject(s.hdc, s.hfont);
  DrawTextA(s.hdc, s.txt, -1, &s.r, format);

  if (do_focus && (item->itemState & ODS_FOCUS))
  {
    GetTextExtentPoint32A(s.hdc, s.txt, strlen(s.txt), &s.sz);

    s.focus.left = (s.r.right - s.r.left) / 2 - s.sz.cx / 2 - 3;
    s.focus.right = s.sz.cx + s.focus.left + 6;
    s.focus.top = (s.r.bottom - s.r.top) / 2 - s.sz.cy / 2 - 3;
    s.focus.bottom = s.sz.cy + s.focus.top + 6;
    DrawFocusRect(s.hdc, &s.focus);
  }
}
