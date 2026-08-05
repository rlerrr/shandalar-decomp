#ifndef __CDTOOLS_H__
#define __CDTOOLS_H__

#include <windows.h>
#include "inttypes.h"

#ifdef __cplusplus
extern "C" {
#endif

	undefined4 __cdecl CheckOriginalCD(undefined4 unused,int *maxTries,undefined4 drivePath);
	undefined4 __cdecl CheckDoPCD(undefined4 unused,int *maxTries,undefined4 drivePath);
	undefined4 __cdecl CheckSotaCD(undefined4 unused,int *maxTries,undefined4 drivePath);
	BOOL __cdecl IsCDDrive(undefined4 drivePath);
	undefined4 Autoplay_ShutDown(void);
	BOOL Autoplay_Restore(void);
	int WINAPI DllMain(HINSTANCE hDllHandle, DWORD nReason, LPVOID reserved);

#ifdef __cplusplus
}
#endif

#endif
