#ifndef CATALOG_H
#define CATALOG_H

#include <windows.h>
#include "inttypes.h"
#include "mystdbool.h"

int *Catalog_LoadWvlEntry(int catalog_id, char *wvl_path, int decode_haar);
BOOL Catalog_Unlock(int unused);

extern CRITICAL_SECTION global_critical_section_for_catalog;

#endif
