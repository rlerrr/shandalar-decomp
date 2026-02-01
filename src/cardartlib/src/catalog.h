#ifndef CATALOG_H
#define CATALOG_H

#include "CardArtLib.h"
#include "inttypes.h"
#include "mystdbool.h"

int *Catalog_LoadWvlEntry(int param_1, uint *param_2, int param_3);
BOOL Catalog_Unlock(int unused);

extern CRITICAL_SECTION global_critical_section_for_catalog;

#endif
