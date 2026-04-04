#ifndef CATALOG_H
#define CATALOG_H

#include <windows.h>
#include "inttypes.h"
#include "mystdbool.h"

int Catalog_Open(const char *catalog_path);
BOOL Catalog_Unlock(void *unused);

extern CRITICAL_SECTION global_critical_section_for_catalog;

#endif
