#include "catalog.h"
#include "haar.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "assert.h"

// GLOBAL: CARDARTLIB 0x10032adc
// GLOBAL: DRAWCARDLIB 0x1003a9ac
undefined4 DAT_10032adc;

// GLOBAL: CARDARTLIB 0x100ea098
// GLOBAL: DRAWCARDLIB 0x100f1f68
undefined4 DAT_100ea098;

// GLOBAL: CARDARTLIB 0x1001d108
// GLOBAL: DRAWCARDLIB 0x10026974
undefined4 DAT_1001d108 = 0x00000000;

// GLOBAL: CARDARTLIB 0x1001e11c
// GLOBAL: DRAWCARDLIB 0x100223fc
undefined4 DAT_1001e11c = 0x00000001;

// GLOBAL: CARDARTLIB 0x1001e120
// GLOBAL: DRAWCARDLIB 0x10022400
undefined4 DAT_1001e120 = 0x00000002;

// GLOBAL: CARDARTLIB 0x101221d0
// GLOBAL: DRAWCARDLIB 0x10124520
CRITICAL_SECTION global_critical_section_for_catalog;

// GLOBAL: CARDARTLIB 0x10032ae8
// GLOBAL: DRAWCARDLIB 0x1003a9b8
int DAT_10032ae8[0x80];

extern unsigned char g_defaultPalette256[0x200];

// GLOBAL: CARDARTLIB 0x1001d12c
// GLOBAL: DRAWCARDLIB 0x10026998
char s_D__Newmagic_sources_NedCard_Cata_1001d12c[] = "D:\\Newmagic\\sources\\NedCard\\Catalog.c";

// GLOBAL: CARDARTLIB 0x1001d1b4
// GLOBAL: DRAWCARDLIB 0x10026a20
char s_D__Newmagic_sources_NedCard_Cata_1001d1b4[] = "D:\\Newmagic\\sources\\NedCard\\Catalog.c";

// GLOBAL: CARDARTLIB 0x1001d10c
// GLOBAL: DRAWCARDLIB 0x10026978
char s_Too_many_open_Catalogs__Max__d_1001d10c[] = "Too many open Catalogs: Max %d\n";

// GLOBAL: CARDARTLIB 0x1001d158
// GLOBAL: DRAWCARDLIB 0x100269c4
char s_Duplicate_short_name_found_in_ca_1001d158[] = "Duplicate short name found in catalogs\n%s entry %d and\n%s entry %d\nShortName value 0x%08lx";

// GLOBAL: CARDARTLIB 0x1001e12c
// GLOBAL: DRAWCARDLIB 0x1002240c
char s_SmallArt_cat_1001e12c[] = "SmallArt.cat";

// GLOBAL: CARDARTLIB 0x1001e13c
// GLOBAL: DRAWCARDLIB 0x1002241c
char s_MedArt_cat_1001e13c[] = "MedArt.cat";

// GLOBAL: CARDARTLIB 0x1001e148
// GLOBAL: DRAWCARDLIB 0x10022428
char s__lf_1001e148[] = "\n";

// GLOBAL: CARDARTLIB 0x1001d154
// GLOBAL: DRAWCARDLIB 0x100269c0
char s_rb_1001d154[] = "rb";

// SIZE 0xc
typedef struct CatalogEntry {
  int key;
  uint offset;
  uint size;
} CatalogEntry;
STATIC_ASSERT(sizeof(CatalogEntry) == 0xc, CatalogEntry_wrong_size);

// SIZE 0x114
typedef struct Catalog {
  FILE *file;
  int entry_count;
  CatalogEntry *entries;
  CatalogEntry *cached_entry;
  char path[0x104];
} Catalog;
STATIC_ASSERT(sizeof(Catalog) == 0x114, Catalog_wrong_size);

// GLOBAL: CARDARTLIB 0x10117290
// GLOBAL: DRAWCARDLIB 0x100f2e30
Catalog DAT_10117290[5];

uint Catalog_MakeKeyFromPath(const char *path);

// MATCHING
// FUNCTION: CARDARTLIB 0x100019d0
// FUNCTION: DRAWCARDLIB 0x1000b820
static int Catalog_Open(const char *catalog_path)
{
  struct {
    int iVar1;
    int entry_index;
    Catalog *other_catalog;
    Catalog *catalog;
    int other_slot;
    int slot_index;
    FILE *catalog_file;
  } s;

  s.slot_index = -1;

  for (s.other_slot = 0; s.other_slot < 5; s.other_slot++) {
    if (DAT_10117290[s.other_slot].file == 0) {
      s.slot_index = s.other_slot;
      break;
    }
  }

  assert((uint)(s.slot_index != -1),s_D__Newmagic_sources_NedCard_Cata_1001d12c,0x43,
            s_Too_many_open_Catalogs__Max__d_1001d10c,5);

  s.catalog = &DAT_10117290[s.slot_index];
  s.catalog->cached_entry = (CatalogEntry *)0x0;
  strcpy(s.catalog->path,catalog_path);
  s.catalog->file = fopen(catalog_path,s_rb_1001d154);
  s.catalog_file = s.catalog->file;
  if (s.catalog_file == (FILE *)0x0)
    return 0;

  fread(&s.catalog->entry_count,4,1,s.catalog_file);
  s.catalog->entries = (CatalogEntry *)malloc(s.catalog->entry_count * sizeof(CatalogEntry));
  fread(s.catalog->entries,sizeof(CatalogEntry),(size_t)s.catalog->entry_count,s.catalog_file);
  if (DAT_1001d108 != 0) {
    for (s.other_slot = 0; s.other_slot < 5; s.other_slot = s.other_slot + 1) {
      if (s.slot_index == s.other_slot)
        continue;

      if (DAT_10117290[s.other_slot].file == 0)
        continue;

      s.other_catalog = (Catalog *)((char *)&DAT_10117290 + s.other_slot * sizeof(Catalog));

      for (s.entry_index = 0; 0 != s.catalog->entry_count; s.entry_index++) {
        for (s.iVar1 = 0; s.iVar1 < s.other_catalog->entry_count; s.iVar1 = s.iVar1 + 1) {
          assert(s.catalog->entries[s.entry_index].key - s.other_catalog->entries[s.entry_index].key != 0,
                  s_D__Newmagic_sources_NedCard_Cata_1001d1b4,0x69,
                  s_Duplicate_short_name_found_in_ca_1001d158,s.catalog->path,s.entry_index,
                  s.other_catalog->path,s.iVar1,s.catalog->entries[s.entry_index].key);
        }
      }
    }
  }

  return s.slot_index + 1;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10001c1a
// FUNCTION: DRAWCARDLIB 0x1000ba6a
bool Catalog_Close(int handle)
{
  if (DAT_10117290[--handle].file == 0)
    return 0;
  free(DAT_10117290[handle].entries);
  fclose(DAT_10117290[handle].file);
  DAT_10117290[handle].entries = (CatalogEntry *)0x0;
  DAT_10117290[handle].file = (FILE *)0x0;
  DAT_10117290[handle].entry_count = 0;
  return 1;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10001cd6
// FUNCTION: DRAWCARDLIB 0x1000bb26
static int CatalogEntry_CompareKey(const int *key, const CatalogEntry *entry)
{
  if (entry->key < *key) {
    return 1;
  }
  else if (*key < entry->key) {
    return -1;
  } else {
    return 0;
  }
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10001d26
// FUNCTION: DRAWCARDLIB 0x1000bb76
CatalogEntry *Catalog_FindEntryCached(Catalog *catalog,const char *name)
{
  CatalogEntry *entry;
  uint key;

  key = Catalog_MakeKeyFromPath(name);
  if ((catalog->cached_entry != (CatalogEntry *)0x0) && ((uint)catalog->cached_entry->key == key)) {
    return catalog->cached_entry;
  }

  entry = (CatalogEntry *)bsearch(&key,catalog->entries,(size_t)catalog->entry_count,sizeof(CatalogEntry),
                                  CatalogEntry_CompareKey);
  catalog->cached_entry = entry;
  return entry;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10001da1
// FUNCTION: DRAWCARDLIB 0x1000bbf1
size_t Catalog_ReadEntry(int catalog_handle,const char *name,void **buffer)

{
  Catalog *catalog;
  CatalogEntry *entry;

  catalog = (Catalog *)((char *)&DAT_10117290 + (catalog_handle + -1) * sizeof(Catalog));
  entry = Catalog_FindEntryCached(catalog,name);
  if (entry == (CatalogEntry *)0x0) {
    return 0xffffffff;
  }

  if (*buffer == (void *)0x0) {
    *buffer = malloc(entry->size + 0x10);
  }
  fseek(catalog->file,(long)entry->offset,0);
  return fread(*buffer,1,entry->size,catalog->file);
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10001e4a
// FUNCTION: DRAWCARDLIB 0x1000bc9a
uint Catalog_MakeKeyFromPath(const char *path)
{
  struct {
    int acc_odd;
    char ext_buf[16];
    char dir_buf[256];

    uint key;
    int pad0;
    char filename_buf[12];
    uint multiplier;
    int ch;
    int acc_even;
  } s;

  s.multiplier = 3;
  s.pad0 = 0;
  s.acc_odd = 0;
  s.acc_even = 0;
  _splitpath(path,s.ext_buf,s.dir_buf,s.filename_buf,s.ext_buf);
  path = s.filename_buf;
  strcat(s.filename_buf,s.ext_buf);
  s.key = (undefined4)(((int)(signed char)path[1] ^ (int)(signed char)path[0]) << 0x18);

  while ((s.ch = (int)(signed char)*(path++)) != 0) {
    if ((s.multiplier & 1) != 0) {
      s.acc_odd = s.multiplier * s.ch + s.acc_odd;
    }
    else {
      s.acc_even = s.multiplier * s.ch + s.acc_even;
    }
    s.multiplier = s.multiplier + 1;
  }

  s.key = (undefined4)((uint)s.key | (s.acc_even * s.acc_odd & 0xffffffU));
  return (uint)s.key;
}

// FUNCTION: CARDARTLIB 0x100068f0
// FUNCTION: DRAWCARDLIB 0x10008370
int *Catalog_LoadWvlEntry(int catalog_id, char *wvl_path, int decode_haar)
{
  struct {
    char fullpath[0x108]; /* [ebp-0x418] */
    char dir[0x104];      /* [ebp-0x310] */
    char fname[0x100];    /* [ebp-0x20c] */
    char ext[0x100];      /* [ebp-0x10c] */
    size_t entry_size;    /* [ebp-0x0c] */
    int *entry;           /* [ebp-0x08] */
    char *dir_end;        /* [ebp-0x04] */
  } s;

  s.entry = (int *)&DAT_10032ae8;
  EnterCriticalSection(&global_critical_section_for_catalog);
  _splitpath(wvl_path, (char *)0x0, s.dir, s.fname, s.ext);
  if (DAT_10032adc == 0) {
    strcpy(s.fullpath, s.dir);
    strcat(s.fullpath, s_SmallArt_cat_1001e12c);
    DAT_1001e11c = Catalog_Open(s.fullpath);
    strcpy(s.fullpath, s.dir);
    strcat(s.fullpath, s_MedArt_cat_1001e13c);
    DAT_1001e120 = Catalog_Open(s.fullpath);
    DAT_10032adc = 1;
  }
  s.dir_end = s.dir + strlen(s.dir);

  if (catalog_id == 0) {
    DAT_100ea098 = DAT_1001e11c;
  } else if (catalog_id == 1) {
    DAT_100ea098 = DAT_1001e120;
  } else {
    /* NOTE: Original does not leave the critical section on this path. */
    return (int *)0x0;
  }

  strcpy(s.dir, s.fname);
  strcat(s.dir, s.ext);
  _strlwr(s.dir);

  if (s.entry != (int *)0x0) {
    memset(s.entry, 0, 0x1b0);
    strcpy((char *)(s.entry + 0x27), wvl_path);
    s.entry[0x68] = (int)&g_defaultPalette256;
    s.entry_size = Catalog_ReadEntry(DAT_100ea098, s.dir, (void **)(s.entry + 0x68));
    if (s.entry_size == (size_t)-1) {
      strcat(wvl_path, s__lf_1001e148);
      OutputDebugStringA((LPCSTR)wvl_path);
      LeaveCriticalSection(&global_critical_section_for_catalog);
      return (int *)0x0;
    }

    s.entry[0x69] = s.entry_size - 0x9c;
    memcpy(s.entry, (void *)s.entry[0x68], 0x9c);
    s.entry[0x68] = s.entry[0x68] + 0x9c;
    if (s.entry[10] == 4) {
      s.entry[7] = s.entry[7] << 1;
      s.entry[8] = s.entry[8] << 1;
    }

    if (decode_haar != 0) {
      s.entry[0x6b] = (int)Wvl_DecodeHaar(s.entry, (byte *)0x0);
      if (s.entry[0x6b] != 0) {
        s.entry[0x6a] = 1;
        LeaveCriticalSection(&global_critical_section_for_catalog);
      } else {
        Catalog_Unlock(s.entry);
        LeaveCriticalSection(&global_critical_section_for_catalog);
        return (int *)0x0;
      }
    }
  }

  /* NOTE: Original does not leave the critical section on this path. */
  return s.entry;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10006bb5
// FUNCTION: DRAWCARDLIB 0x10008635
BOOL Catalog_Unlock(void *unused)
{
  LeaveCriticalSection(&global_critical_section_for_catalog);

  if (unused == 0)
    return 0;
  return 0;
}
