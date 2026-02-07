#include "catalog.h"
#include "haar.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "assert.h"

// GLOBAL: CARDARTLIB 0x10032adc
undefined4 DAT_10032adc = 0x00000000;

// GLOBAL: CARDARTLIB 0x100ea098
undefined4 DAT_100ea098 = 0x00000000;

// GLOBAL: CARDARTLIB 0x1001d108
undefined4 DAT_1001d108 = 0x00000000;

// GLOBAL: CARDARTLIB 0x1001e11c
undefined4 DAT_1001e11c = 0x00000001;

// GLOBAL: CARDARTLIB 0x1001e120
undefined4 DAT_1001e120 = 0x00000002;

// GLOBAL: CARDARTLIB 0x101221d0
// GLOBAL: DRAWCARDLIB 0x10124520
CRITICAL_SECTION global_critical_section_for_catalog;

// GLOBAL: CARDARTLIB 0x10032ae8
int DAT_10032ae8[0x80];

extern undefined1 DAT_100ad498[0x200];

// GLOBAL: CARDARTLIB 0x1001d12c
char s_D__Newmagic_sources_NedCard_Cata_1001d12c[] = "D:\\Newmagic\\sources\\NedCard\\Catalog.c";

// GLOBAL: CARDARTLIB 0x1001d1b4
char s_D__Newmagic_sources_NedCard_Cata_1001d1b4[] = "D:\\Newmagic\\sources\\NedCard\\Catalog.c";

// GLOBAL: CARDARTLIB 0x1001d10c
char s_Too_many_open_Catalogs__Max__d_1001d10c[] = "Too many open Catalogs: Max %d\n";

// GLOBAL: CARDARTLIB 0x1001d158
char s_Duplicate_short_name_found_in_ca_1001d158[] = "Duplicate short name found in catalogs\n%s entry %d and\n%s entry %d\nShortName value 0x%08lx";

// GLOBAL: CARDARTLIB 0x1001e12c
char s_SmallArt_cat_1001e12c[] = "SmallArt.cat";

// GLOBAL: CARDARTLIB 0x1001e13c
char s_MedArt_cat_1001e13c[] = "MedArt.cat";

// GLOBAL: CARDARTLIB 0x1001e148
char s__lf_1001e148[] = "\n";

// GLOBAL: CARDARTLIB 0x1001d154
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
Catalog DAT_10117290[5];

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
  s.catalog->file = fopen(catalog_path,&s_rb_1001d154);
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
  if ((catalog->cached_entry != (CatalogEntry *)0x0) && (catalog->cached_entry->key == key)) {
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
    uint ext_buf[4];
    char dir_buf[256];

    undefined4 key;
    int pad0;
    undefined4 filename_buf;
    int pad1;
    int pad2;
    uint multiplier;
    int ch;
    int acc_even;
  } s;

  s.multiplier = 3;
  s.pad0 = 0;
  s.acc_odd = 0;
  s.acc_even = 0;
  _splitpath(path,(char *)s.ext_buf,s.dir_buf,(char *)&s.filename_buf,(char *)s.ext_buf);
  path = (char *)&s.filename_buf;
  strcat((char *)&s.filename_buf,(char *)s.ext_buf);
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
int *Catalog_LoadWvlEntry(int param_1,uint *param_2,int param_3)
{
  size_t sVar1;
  undefined8 *puVar2;
  uint local_41c [66];
  uint local_314 [65];
  uint local_210 [64];
  uint local_110 [64];
  size_t local_10;
  int *local_c;
  int local_8;

  local_c = (int *)&DAT_10032ae8;
  EnterCriticalSection(&global_critical_section_for_catalog);
  _splitpath((char *)param_2,(char *)0x0,(char *)local_314,(char *)local_210,(char *)local_110);
  if (DAT_10032adc == 0) {
    strcpy((char *)local_41c,(char *)local_314);
    strcat((char *)local_41c,s_SmallArt_cat_1001e12c);
    DAT_1001e11c = Catalog_Open((char *)local_41c);
    strcpy((char *)local_41c,(char *)local_314);
    strcat((char *)local_41c,s_MedArt_cat_1001e13c);
    DAT_1001e120 = Catalog_Open((char *)local_41c);
    DAT_10032adc = 1;
  }
  sVar1 = strlen((char *)local_314);
  local_8 = (int)local_314 + sVar1;
  if (param_1 == 0) {
    DAT_100ea098 = DAT_1001e11c;
  }
  else {
    if (param_1 != 1) {
      return (int *)0x0;
    }
    DAT_100ea098 = DAT_1001e120;
  }
  strcpy((char *)local_314,(char *)local_210);
  strcat((char *)local_314,(char *)local_110);
  _strlwr((char *)local_314);
  if (local_c != (int *)0x0) {
    memset(local_c,0,0x1b0);
    strcpy((char *)(local_c + 0x27),(const char *)param_2);
    local_c[0x68] = (int)&DAT_100ad498;
    local_10 = Catalog_ReadEntry(DAT_100ea098,(char *)local_314,(void **)(local_c + 0x68));
    if (local_10 == 0xffffffff) {
      strcat((char *)param_2,s__lf_1001e148);
      OutputDebugStringA((LPCSTR)param_2);
      LeaveCriticalSection(&global_critical_section_for_catalog);
      local_c = (int *)0x0;
    }
    else {
      local_c[0x69] = local_10 - 0x9c;
      memcpy(local_c,(void *)local_c[0x68],0x9c);
      local_c[0x68] = local_c[0x68] + 0x9c;
      if (local_c[10] == 4) {
        local_c[7] = local_c[7] << 1;
        local_c[8] = local_c[8] << 1;
      }
      if (param_3 != 0) {
        puVar2 = Wvl_DecodeHaar(local_c,(undefined8 *)0x0);
        local_c[0x6b] = (int)puVar2;
        if (local_c[0x6b] == 0) {
          Catalog_Unlock(local_c);
          LeaveCriticalSection(&global_critical_section_for_catalog);
          local_c = (int *)0x0;
        }
        else {
          local_c[0x6a] = 1;
          LeaveCriticalSection(&global_critical_section_for_catalog);
        }
      }
    }
  }
  return local_c;
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10006bb5
// FUNCTION: DRAWCARDLIB 0x10008635
BOOL Catalog_Unlock(int unused)
{
  LeaveCriticalSection(&global_critical_section_for_catalog);

  if (unused == 0)
    return 0;
  return 0;
}
