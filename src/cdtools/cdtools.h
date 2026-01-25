#ifndef __CDTOOLS_H__
#define __CDTOOLS_H__

#include <windows.h>
#include "inttypes.h"

#ifdef __cplusplus
extern "C" {
#endif

	undefined4 __cdecl CheckOriginalCD(undefined4 param_1,int *param_2,undefined4 param_3);
	undefined4 __cdecl CheckDoPCD(undefined4 param_1,int *param_2,undefined4 param_3);
	undefined4 __cdecl CheckSotaCD(undefined4 param_1,int *param_2,undefined4 param_3);
	BOOL __cdecl IsCDDrive(undefined4 param_1);
	undefined4 Autoplay_ShutDown(void);
	undefined4 Autoplay_Restore(void);
	int WINAPI DllMain(HINSTANCE hDllHandle, DWORD nReason, LPVOID reserved);

#ifdef __cplusplus
}
#endif

#endif
