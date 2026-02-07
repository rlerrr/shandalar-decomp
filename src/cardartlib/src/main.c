#include "CardArtLib.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "inttypes.h"
#include "mystdbool.h"
#include "defs.h"
#include "catalog.h"
#include "haar.h"
#include "assert.h"
#include "palette.h"

// GLOBAL: CARDARTLIB 0x1001d1e0
char s__CARDART_1001d1e0[] = "\\CARDART";

// GLOBAL: CARDARTLIB 0x1001d1ec
char s__CARDS_DAT_1001d1ec[] = "\\CARDS.DAT";

// GLOBAL: CARDARTLIB 0x1001d1f8
char s__rb_1001d1f8[] = "rb";

// GLOBAL: CARDARTLIB 0x1001d1fc
char s__s__04d_c_WVL_1001d1fc[] = "%s\\%04d%c.WVL";

// GLOBAL: CARDARTLIB 0x1001d20c
char s__s__04d_WVL_1001d20c[] = "%s\\%04d.WVL";

// GLOBAL: CARDARTLIB 0x1001d218
char s__s__04d_WVL_1001d218[] = "%s\\%04d.WVL";

// GLOBAL: CARDARTLIB 0x1001d224
char s__s__04d_c_WVL_1001d224[] = "%s\\%04d%c.WVL";

// GLOBAL: CARDARTLIB 0x1001d234
char s__s__04d_WVL_1001d234[] = "%s\\%04d.WVL";

// GLOBAL: CARDARTLIB 0x101177f4
int g_versionedBigArtCount = 0x00000000;

// GLOBAL: CARDARTLIB 0x10117910
char global_cartart_directory[0x105];

// GLOBAL: CARDARTLIB 0x10121fe0
int g_versionedSmallArtCount = 0x00000000;

// GLOBAL: CARDARTLIB 0x10121fe4
undefined4 global_hinstance = 0x00000000;

// GLOBAL: CARDARTLIB 0x10117a20
CRITICAL_SECTION global_critical_section_for_small_art;

// GLOBAL: CARDARTLIB 0x101221f0
CRITICAL_SECTION global_critical_section_for_big_art;

// GLOBAL: CARDARTLIB 0x101200a0
int g_cardPicCounts[2000];

// SIZE 0x10
typedef struct ArtCacheEntry {
  HBITMAP hbm;
  void *bits;
  int width;
  int height;
} ArtCacheEntry;
STATIC_ASSERT(sizeof(ArtCacheEntry) == 0x10, ArtCacheEntry_wrong_size);

// GLOBAL: CARDARTLIB 0x10117a40
ArtCacheEntry g_SmallArtCache[2000];

// SIZE 0x18
typedef struct VersionedArtCacheEntry {
  HBITMAP hbm;
  void *bits;
  int width;
  int height;
  int id;
  int version;
} VersionedArtCacheEntry;
STATIC_ASSERT(sizeof(VersionedArtCacheEntry) == 0x18, VersionedArtCacheEntry_wrong_size);

// GLOBAL: CARDARTLIB 0x1011f740
VersionedArtCacheEntry g_versionedSmallArtCache[100];

// GLOBAL: CARDARTLIB 0x10121ff0
VersionedArtCacheEntry g_versionedBigArtCache[0x14];

void CardArtLib_Shutdown(void);
VersionedArtCacheEntry * FindVersionedSmallArtCacheEntry(int id,int version);
void DestroyVersionedSmallArt(int id,int version);
void DestroyAllVersionedSmallArts(void);

// MATCHING
// FUNCTION: CARDARTLIB 0x100030f0
BOOL WINAPI DllMain(HINSTANCE instance,DWORD reason,LPVOID reserved)
{
  uint uVar1;
  
  (void)reserved;
  switch(reason) {
  case 1:
    uVar1 = 1;
    uVar1 = (uint)CardArtLib_Initialize(instance) & uVar1;
    return uVar1;
  case 2:
    return 1;
  case 3:
    return 1;
  case 0:
    CardArtLib_Shutdown();
    return 1;
  }
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10003175
static BOOL CardArtLib_Initialize(HINSTANCE instance)
{
  struct {
    char *last_slash;              /* -0x4a4a4 */
    char cards_dat_path[0x108];    /* -0x4a4a0 */
    int card_count;                /* -0x4a398 */
    int unk_4a394;                 /* -0x4a394 */
    int idx;                       /* -0x4a390 */
    FILE *cards_file;              /* -0x4a38c */
    card_ptr_t records[2000];      /* -0x4a388 */
    int result;                    /* -0x08 */
    int i;                         /* -0x04 */
  } s;

  s.result = 1;
  global_hinstance = (undefined4)instance;
  InitializeCriticalSection(&global_critical_section_for_small_art);
  InitializeCriticalSection(&global_critical_section_for_big_art);
  InitializeCriticalSection(&global_critical_section_for_catalog);
  GetModuleFileNameA((HMODULE)0x0,&global_base_directory,0x105);
  s.last_slash = strrchr(&global_base_directory,0x5c);
  *s.last_slash = '\0';
  strcpy(global_cartart_directory,global_base_directory);
  strcat(global_cartart_directory,s__CARDART_1001d1e0);
  InitCardArtGdiResources();
  strcpy(s.cards_dat_path,global_base_directory);
  strcat(s.cards_dat_path,s__CARDS_DAT_1001d1ec);
  s.cards_file = fopen(s.cards_dat_path,&s__rb_1001d1f8);

  if (s.cards_file != (FILE *)0x0) {
    fread(&s.card_count,4,1,s.cards_file);
    fread(&s.unk_4a394,4,1,s.cards_file);
    fread(s.records,0x98,(size_t)s.card_count,s.cards_file);

    for (s.idx = 0; s.idx < s.card_count; s.idx = s.idx + 1) {
      if (s.records[s.idx].num_pics > 0) {
        g_cardPicCounts[s.idx] = s.records[s.idx].num_pics;
      } else {
        g_cardPicCounts[s.idx] = 1;
      }
    }

    fclose(s.cards_file);
  } else {
    for (s.idx = 0; s.idx < s.card_count; s.idx = s.idx + 1) {
      g_cardPicCounts[s.idx] = 1;
    }
  }

  for (s.i = 0; s.i < 2000; s.i = s.i + 1) {
    g_SmallArtCache[s.i].hbm = 0;
  }

  for (s.i = 0; s.i < 100; s.i = s.i + 1) {
    g_versionedSmallArtCache[s.i].hbm = 0;
  }
  g_versionedSmallArtCount = 0;

  for (s.i = 0; s.i < 0x14; s.i = s.i + 1) {
    g_versionedBigArtCache[s.i].hbm = 0;
  }

  g_versionedBigArtCount = 0;
  global_dither_kernel_id = 3;
  return s.result;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x1000343e
void CardArtLib_Shutdown(void)
{
  DestroyAllSmallArts();
  DestroyAllBigArts();
  ShutdownCardArtGdiResources();
  DeleteCriticalSection(&global_critical_section_for_small_art);
  DeleteCriticalSection(&global_critical_section_for_big_art);
  DeleteCriticalSection(&global_critical_section_for_catalog);
}

// FUNCTION: CARDARTLIB 0x10003480
int LoadBigArt(int id,int version,int width,int height)
{
  struct {
    int align_bytes;          /* -0x158 */
    HDC desktop_hdc;          /* -0x154 */
    char wvl_path[0x108];     /* -0x150 */
    int existing;             /* -0x48 */
    HBITMAP bitmap;           /* -0x44 */
    HDC mem_dc;               /* -0x40 */
    int ok;                   /* -0x3c */
    int *wvl_entry;           /* -0x38 */
    void *dib_bits;           /* -0x34 */
    uint *decoded;            /* -0x30 */
    BITMAPINFO bmi;           /* -0x2c */
  } s;

  s.ok = 1;
  if (id == -1) {
    return 0;
  }

  EnterCriticalSection(&global_critical_section_for_big_art);

  s.existing = IsBigArtIn(id,version);
  if (s.existing != 0) {
    if ((*(int *)(s.existing + 8) == width) && (*(int *)(s.existing + 0xc) == height)) {
      LeaveCriticalSection(&global_critical_section_for_big_art);
      return 1;
    }
    else
      DestroyBigArt(id,version);
  }

  if ((g_cardPicCounts[id] > 1) && (version != 0)) {
    sprintf(s.wvl_path,s__s__04d_c_WVL_1001d1fc,&global_cartart_directory,id,(int)(char)(version + '`'));
  } else {
    sprintf(s.wvl_path,s__s__04d_WVL_1001d20c,&global_cartart_directory,id);
  }

  s.desktop_hdc = GetDC(GetDesktopWindow());
  global_color_depth = GetDeviceCaps(s.desktop_hdc,BITSPIXEL) * GetDeviceCaps(s.desktop_hdc,PLANES);
  ReleaseDC(GetDesktopWindow(),s.desktop_hdc);

  s.wvl_entry = Catalog_LoadWvlEntry(1,s.wvl_path,0);
  if (s.wvl_entry != (int *)0x0) {
    s.mem_dc = GetDC((HWND)0x0);
    ApplyCardArtPaletteToDc(s.mem_dc);
    InitBitmapInfo24bppTopDown(&s.bmi,width,height);
    s.bitmap = CreateDIBSection(s.mem_dc,&s.bmi,0,&s.dib_bits,(HANDLE)0x0,0);
    if (s.bitmap != (HBITMAP)0x0) {
      s.decoded = Wvl_DecodeToBgr24((uint *)0x0,s.wvl_entry,width,height);
      if (s.decoded != (uint *)0x0) {
        if ((-(width + width - width) & 3U) != 0) {
          s.align_bytes = 4 - (-(width + width - width) & 3U);
        }
        else
          s.align_bytes = 0;

        memcpy(s.dib_bits,s.decoded,(width * 3 + s.align_bytes) * height);
      } else {
        s.ok = 0;
        DeleteObject(s.bitmap);
      }
    } else {
      s.ok = 0;
    }
    ReleaseDC((HWND)0x0,s.mem_dc);
    Catalog_Unlock(s.wvl_entry);
  } else {
    s.ok = 0;
  }

  if (s.ok != 0) {
    if ((int)g_versionedBigArtCount >= 0x14) {
      DestroyBigArt(g_versionedBigArtCache[0].id, g_versionedBigArtCache[0].version);
    }

    g_versionedBigArtCache[g_versionedBigArtCount].hbm = s.bitmap;
    g_versionedBigArtCache[g_versionedBigArtCount].bits = s.dib_bits;
    g_versionedBigArtCache[g_versionedBigArtCount].width = width;
    g_versionedBigArtCache[g_versionedBigArtCount].height = height;
    g_versionedBigArtCache[g_versionedBigArtCount].id = id;
    g_versionedBigArtCache[g_versionedBigArtCount].version = version;
    g_versionedBigArtCount = g_versionedBigArtCount + 1;
  }

  LeaveCriticalSection(&global_critical_section_for_big_art);
  return s.ok;
}

// FUNCTION: CARDARTLIB 0x100037ba
int IsBigArtIn(int id,int version)
{
  struct {
    int result;
    int i;
  } s;

  s.result = 0;
  if (id == -1)
    return 0;

  EnterCriticalSection(&global_critical_section_for_big_art);

  for (s.i = 0; s.i < g_versionedBigArtCount && s.result == 0; s.i++) {
    if (g_versionedBigArtCache[s.i].id == id && g_versionedBigArtCache[s.i].version == version) {
      s.result = &g_versionedBigArtCache[s.i];
    }
  }

  LeaveCriticalSection(&global_critical_section_for_big_art);

  return s.result;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10003867
int IsBigArtRightSize(int id,int version,int width,int height)
{
  int local_8 = 0;
  if (id == -1) {
    return 0;
  }

  EnterCriticalSection(&global_critical_section_for_big_art);
  local_8 = IsBigArtIn(id,version);
  if ((local_8 != 0) && ((*(int *)(local_8 + 8) != width || (*(int *)(local_8 + 0xc) != height))))
  {
    local_8 = 0;
  }
  LeaveCriticalSection(&global_critical_section_for_big_art);

  return local_8;
}

// FUNCTION: CARDARTLIB 0x100038ed
int DrawBigArt(HDC hdc,RECT *rect,int id,int version)
{
  struct {
    int result;
    int found_index;
    int i;
    int found;
  } s;

  if (id == -1) {
    return 0;
  }

  EnterCriticalSection(&global_critical_section_for_big_art);
  s.i = 0;
  s.found = 0;
  for (; s.i < (int)g_versionedBigArtCount && (s.found == 0); s.i++) {
    if (g_versionedBigArtCache[s.i].id == id && g_versionedBigArtCache[s.i].version == version) {
      s.found = 1;
      s.found_index = s.i;
    }
  }

  if (s.found != 0) {
    s.result = DrawBitmapToRect(hdc,rect,g_versionedBigArtCache[s.found_index].hbm);
  }
  else {
    s.result = 0;
  }

  if (s.result == 0) {
    FillRect(hdc,rect,GetStockObject(2));
  }

  LeaveCriticalSection(&global_critical_section_for_big_art);
  return s.result;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x100039ef
int ReloadBigArtIfWrongSize(int id,int version,int width,int height)
{
  if (id == -1) {
    return 0;
  }

  if (IsBigArtRightSize(id,version,width,height) != 0) {
    return 1;
  } else {
    DestroyBigArt(id,version);
  }

  if (LoadBigArt(id,version,width,height) != 0) {
    return 1;
  } else {
    return 0;
  }
}

// FUNCTION: CARDARTLIB 0x10003a80
void DestroyBigArt(int id,int version)
{
  struct {
    int j;
    int i;
    int local_4;
  } s;

  if (id == -1) {
    return;
  }

  EnterCriticalSection(&global_critical_section_for_big_art);
  s.i = 0;
  s.local_4 = 0;

  for (; s.i < g_versionedBigArtCount && (s.local_4 == 0); s.i++) {
    if (g_versionedBigArtCache[s.i].id == id && g_versionedBigArtCache[s.i].version == version) {
      s.local_4 = 1;

      if (g_versionedBigArtCache[s.i].hbm != 0) {
        DeleteObject(g_versionedBigArtCache[s.i].hbm);
      }

      g_versionedBigArtCount--;

      s.j = s.i;
      for (; s.j < g_versionedBigArtCount; s.j++) {
        g_versionedBigArtCache[s.j].hbm = g_versionedBigArtCache[s.j+1].hbm;
        g_versionedBigArtCache[s.j].bits = g_versionedBigArtCache[s.j+1].bits;
        g_versionedBigArtCache[s.j].width = g_versionedBigArtCache[s.j+1].width;
        g_versionedBigArtCache[s.j].height = g_versionedBigArtCache[s.j+1].height;
        g_versionedBigArtCache[s.j].id = g_versionedBigArtCache[s.j+1].id;
        g_versionedBigArtCache[s.j].version = g_versionedBigArtCache[s.j+1].version;
      }
    }
  }

  LeaveCriticalSection(&global_critical_section_for_big_art);
}

// FUNCTION: CARDARTLIB 0x10003c0a
void DestroyAllBigArts(void)
{
  int i;
  EnterCriticalSection(&global_critical_section_for_big_art);
  for (i = 0; i < g_versionedBigArtCount; i = i + 1) {
    DeleteObject(g_versionedBigArtCache[i].hbm);
  }
  g_versionedBigArtCount = 0;
  LeaveCriticalSection(&global_critical_section_for_big_art);
}

// FUNCTION: CARDARTLIB 0x10003cf0
int LoadSmallArt(int id,int version,int width,int height)
{
  struct {
    int row_pad;
    HDC desktop_hdc;
    uint wvl_path[66];
    HBITMAP hbm;
    HDC hdc;
    int ok;
    int *wvl_entry;
    void *dib_bits;
    uint *decoded_bgr;
    BITMAPINFO bmi;
  } s;

  s.ok = 1;
  if (id == -1) {
    return 0;
  }

  if (g_cardPicCounts[id] > 1) {
    return LoadVersionedSmallArt(id,version,width,height);
  }

  EnterCriticalSection(&global_critical_section_for_small_art);

  if (g_SmallArtCache[id].hbm != (HBITMAP)0) {
    if (g_SmallArtCache[id].width == width && g_SmallArtCache[id].height == height) {
      LeaveCriticalSection(&global_critical_section_for_small_art);
      return 1;
    }
    else
      DestroySmallArt(id,0);
  }

  s.desktop_hdc = GetDC(GetDesktopWindow());
  global_color_depth = GetDeviceCaps(s.desktop_hdc,BITSPIXEL) * GetDeviceCaps(s.desktop_hdc,PLANES);
  ReleaseDC(GetDesktopWindow(),s.desktop_hdc);

  sprintf((char *)s.wvl_path,s__s__04d_WVL_1001d218,&global_cartart_directory,id);
  s.wvl_entry = Catalog_LoadWvlEntry(0,s.wvl_path,0);
  if (s.wvl_entry != (int *)0x0) {
    s.hdc = GetDC((HWND)0x0);
    ApplyCardArtPaletteToDc(s.hdc);
    InitBitmapInfo24bppTopDown(&s.bmi,width,height);
    s.hbm = CreateDIBSection(s.hdc,&s.bmi,0,&s.dib_bits,(HANDLE)0x0,0);
    if (s.hbm != (HBITMAP)0x0) {
      s.decoded_bgr = Wvl_DecodeToBgr24((uint *)0x0,s.wvl_entry,width,height);
      if (s.decoded_bgr != (uint *)0x0) {
        do {
          if ((-(width + width - width) & 3) != 0) {
            s.row_pad = 4 - (-(width + width - width) & 3);
            break;
          }
          s.row_pad = 0;
        } while (0);
        memcpy(s.dib_bits,s.decoded_bgr,(width * 3 + s.row_pad) * height);
      }
      else {
        s.ok = 0;
        DeleteObject(s.hbm);
      }
    }
    else {
      s.ok = 0;
    }
    ReleaseDC((HWND)0x0,s.hdc);
    Catalog_Unlock(s.wvl_entry);
  }
  else {
    s.ok = 0;
  }

  if (s.ok != 0) {
    g_SmallArtCache[id].hbm = s.hbm;
    g_SmallArtCache[id].bits = s.dib_bits;
    g_SmallArtCache[id].width = width;
    g_SmallArtCache[id].height = height;
  }

  LeaveCriticalSection(&global_critical_section_for_small_art);
  return s.ok;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10003fb4
int IsSmallArtIn(int id,int version)
{
  if (id == -1) {
    return 0;
  }

  if (g_cardPicCounts[id] > 1) {
    if (FindVersionedSmallArtCacheEntry(id,version) != (undefined *)0x0) {
      return 1;
    } else {
      return 0;
    }
  } else {  
    if (g_SmallArtCache[id].hbm != 0) {
      return 1;
    } else {
      return 0;
    }
  }
}

// MATCHING
// FUNCTION: CARDARTLIB 0x1000403d
int DrawSmallArt(HDC hdc,RECT *rect,int id,int version)
{
  int result;
  if ((hdc == (HDC)0x0) || (rect == (RECT *)0x0)) {
    return 0;
  }
  
  if (id == -1) {
    return 0;
  }
  
  if (g_cardPicCounts[id] > 1) {
    DrawVersionedSmallArt(hdc,rect,id,version);
  }
  else {
    EnterCriticalSection(&global_critical_section_for_small_art);
    if (IsSmallArtIn(id,version) != 0) {
      result = DrawBitmapToRect(hdc,rect,g_SmallArtCache[id].hbm);
    } else {
      result = 0;
    }
    if (result == 0) {
      FillRect(hdc,rect,GetStockObject(2));
    }
    LeaveCriticalSection(&global_critical_section_for_small_art);
    
    return result;
  }
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10004128
bool ReloadSmallArtIfWrongSize(int id,int version,int width,int height)
{
  struct {
    int result;
    HGDIOBJ old;
  } s;

  if (id == -1) {
    return 0;
  }

  if (g_cardPicCounts[id] > 1) {
    return ReloadVersionedSmallArtIfWrongSize(id,version,width,height);
  }

  EnterCriticalSection(&global_critical_section_for_small_art);

  if (g_SmallArtCache[id].hbm != (HGDIOBJ)0x0 && g_SmallArtCache[id].width == width && g_SmallArtCache[id].height == height) {    s.result = 1;
  }
  else {
    s.old = g_SmallArtCache[id].hbm;
    g_SmallArtCache[id].hbm = (HGDIOBJ)0x0;
    if (LoadSmallArt(id,version,width,height) != 0) {
      if (s.old != (HGDIOBJ)0x0) {
        DeleteObject(s.old);
      }
      s.result = 1;
    }
    else {
      g_SmallArtCache[id].hbm = s.old;
      s.result = 0;
    }
  }

  LeaveCriticalSection(&global_critical_section_for_small_art);
  return s.result;
}

// FUNCTION: CARDARTLIB 0x10004251
void DestroySmallArt(int id,int version)
{
  if (id == -1) 
    return;

  if ((g_cardPicCounts)[id] > 1) {
    DestroyVersionedSmallArt(id,version);
  } else {
    EnterCriticalSection(&global_critical_section_for_small_art);
    if (g_SmallArtCache[id].hbm != 0) {
      DeleteObject(g_SmallArtCache[id].hbm);
      g_SmallArtCache[id].hbm = 0;
    }
    LeaveCriticalSection(&global_critical_section_for_small_art);
  }
}

// FUNCTION: CARDARTLIB 0x100042dd
void DestroyAllSmallArts(void)
{
  int i;
  
  EnterCriticalSection(&global_critical_section_for_small_art);
  DestroyAllVersionedSmallArts();
  for (i = 0; i < 2000; i = i + 1) {
    if (g_cardPicCounts[i] <= 1) {
      DestroySmallArt(i, 0);
    }
  }
  LeaveCriticalSection(&global_critical_section_for_small_art);
}

// FUNCTION: CARDARTLIB 0x10004346
int LoadVersionedSmallArt(int id,int version,int width,int height)
{
  struct {
    int row_pad;
    uint wvl_path[66];
    HBITMAP hbm;
    HDC hdc;
    int ok;
    int *wvl_entry;
    void *dib_bits;
    uint *decoded_bgr;
    BITMAPINFO bmi;
    VersionedArtCacheEntry *cache_entry;
  } s;

  s.ok = 1;
  if (id == -1) {
    return 0;
  }

  EnterCriticalSection(&global_critical_section_for_small_art);

  s.cache_entry = (VersionedArtCacheEntry *)FindVersionedSmallArtCacheEntry(id,version);
  if (s.cache_entry != (VersionedArtCacheEntry *)0x0) {
    if ((s.cache_entry->width == width) && (s.cache_entry->height == height)) {
      LeaveCriticalSection(&global_critical_section_for_small_art);
      return 1;
    }
    else
      DestroyVersionedSmallArt(id,version);
  }

  if (version != 0) {
    sprintf((char *)s.wvl_path,s__s__04d_c_WVL_1001d224,&global_cartart_directory,id,(int)(char)(version + '`'));
  }
  else {
    sprintf((char *)s.wvl_path,s__s__04d_WVL_1001d234,&global_cartart_directory,id);
  }

  s.wvl_entry = Catalog_LoadWvlEntry(0,s.wvl_path,0);
  if (s.wvl_entry != (int *)0x0) {
    s.hdc = GetDC((HWND)0x0);
    ApplyCardArtPaletteToDc(s.hdc);
    InitBitmapInfo24bppTopDown(&s.bmi,width,height);
    s.hbm = CreateDIBSection(s.hdc,&s.bmi,0,&s.dib_bits,(HANDLE)0x0,0);
    if (s.hbm != (HBITMAP)0x0) {
      s.decoded_bgr = Wvl_DecodeToBgr24((uint *)0x0,s.wvl_entry,width,height);
      if (s.decoded_bgr != (uint *)0x0) {
        if ((-(width + width - width) & 3) != 0) {
          s.row_pad = 4 - (-(width + width - width) & 3);
        }
        else {
          s.row_pad = 0;
        }
        memcpy(s.dib_bits,s.decoded_bgr,(width * 3 + s.row_pad) * height);
      }
      else {
        s.ok = 0;
        DeleteObject(s.hbm);
      }
    }
    else {
      s.ok = 0;
    }
    ReleaseDC((HWND)0x0,s.hdc);
    Catalog_Unlock(s.wvl_entry);
  }
  else {
    s.ok = 0;
  }

  if (s.ok != 0) {
    g_versionedSmallArtCache[g_versionedSmallArtCount].hbm = s.hbm;
    g_versionedSmallArtCache[g_versionedSmallArtCount].bits = s.dib_bits;
    g_versionedSmallArtCache[g_versionedSmallArtCount].width = width;
    g_versionedSmallArtCache[g_versionedSmallArtCount].height = height;
    g_versionedSmallArtCache[g_versionedSmallArtCount].id = id;
    g_versionedSmallArtCache[g_versionedSmallArtCount].version = version;
    g_versionedSmallArtCount++;
  }

  LeaveCriticalSection(&global_critical_section_for_small_art);
  return s.ok;
}

// FUNCTION: CARDARTLIB 0x100045fe
VersionedArtCacheEntry *FindVersionedSmallArtCacheEntry(int id,int version)
{
  int i;
  VersionedArtCacheEntry *ptr;
  
  ptr = 0;
  if (id == -1)
    return 0;

  EnterCriticalSection(&global_critical_section_for_small_art);

  for (i = 0 ; i < (int)g_versionedSmallArtCount && ptr == 0; i++) {
    if (g_versionedSmallArtCache[i].id == id && g_versionedSmallArtCache[i].version == version) {
      ptr = &g_versionedSmallArtCache[i];
    }
  }

  LeaveCriticalSection(&global_critical_section_for_small_art);

  return ptr;
}

// FUNCTION: CARDARTLIB 0x100046aa
int DrawVersionedSmallArt(HDC hdc,RECT *rect,int id,int version)
{
  struct {
    int local_14;
    int local_10;
    int i;
    bool bVar1;
  } s;
  
  if (id == -1) 
    return 0;
   
  EnterCriticalSection(&global_critical_section_for_small_art);

  for (s.i = 0, s.bVar1 = false; s.i < g_versionedSmallArtCount && (!s.bVar1); s.i++) {
    if (g_versionedSmallArtCache[s.i].id == id && g_versionedSmallArtCache[s.i].version == version) {
      s.bVar1 = true;
      s.local_10 = s.i;
    }
  }
  
  if (s.bVar1) {
    s.local_14 = DrawBitmapToRect(hdc, rect, g_versionedSmallArtCache[s.local_10].hbm);
  }
  else {
    s.local_14 = 0;
  }
  if (s.local_14 == 0) {
    FillRect(hdc,rect,GetStockObject(2));
  }
  LeaveCriticalSection(&global_critical_section_for_small_art);

  return s.local_14;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x100047ab
bool ReloadVersionedSmallArtIfWrongSize(int id,int version,int width,int height)
{
  VersionedArtCacheEntry *cache_entry;

  if (id == -1) {
    return 0;
  }

  cache_entry = FindVersionedSmallArtCacheEntry(id,version);
  if (cache_entry != (undefined *)0x0) {
    if ((*(int *)(cache_entry + 8) == width) && (*(int *)(cache_entry + 0xc) == height)) {
      return 1;
    }
    else {
      DestroyVersionedSmallArt(id,version);
    }
  }

  if (LoadVersionedSmallArt(id,version,width,height) != 0) {
    return 1;
  }
  else {
    return 0;
  }
}

// FUNCTION: CARDARTLIB 0x1000485a
void DestroyVersionedSmallArt(int id,int version)
{
  struct {
    int j;    
    int i;
    bool found;
  } s;

  if (id == -1) {
    return;
  }

  EnterCriticalSection(&global_critical_section_for_small_art);

  for (s.i = 0, s.found = 0;s.i < g_versionedSmallArtCount && s.found == 0; s.i++) {
    if (g_versionedSmallArtCache[s.i].id == id && g_versionedSmallArtCache[s.i].version == version) {
      s.found = 1;

      if (g_versionedSmallArtCache[s.i].hbm != 0) {
        DeleteObject(g_versionedSmallArtCache[s.i].hbm);
      }

      g_versionedSmallArtCount = g_versionedSmallArtCount + -1;

      for (s.j = s.i; s.j < g_versionedSmallArtCount; s.j++) {
        g_versionedSmallArtCache[s.j].hbm = g_versionedSmallArtCache[s.j + 1].hbm;
        g_versionedSmallArtCache[s.j].bits = g_versionedSmallArtCache[s.j + 1].bits;
        g_versionedSmallArtCache[s.j].width = g_versionedSmallArtCache[s.j + 1].width;
        g_versionedSmallArtCache[s.j].height = g_versionedSmallArtCache[s.j + 1].height;
        g_versionedSmallArtCache[s.j].id = g_versionedSmallArtCache[s.j + 1].id;
        g_versionedSmallArtCache[s.j].version = g_versionedSmallArtCache[s.j + 1].version;
      }
    }
  }

  LeaveCriticalSection(&global_critical_section_for_small_art);
}

// FUNCTION: CARDARTLIB 0x100049e4
void DestroyAllVersionedSmallArts(void)
{
  int i;
  
  EnterCriticalSection(&global_critical_section_for_small_art);
  for (i = 0; i < g_versionedSmallArtCount; i = i + 1) {
    DeleteObject(g_versionedSmallArtCache[i].hbm);
  }
  g_versionedSmallArtCount = 0;
  LeaveCriticalSection(&global_critical_section_for_small_art);
}
