#ifndef __CARDARTLIB_H__
#define __CARDARTLIB_H__

#include <windows.h>
#ifdef __cplusplus
extern "C" {
#endif
	__declspec(dllimport) int LoadBigArt(int id, int version, int width, int height);
	__declspec(dllimport) int LoadSmallArt(int id, int version, int width, int height);
	__declspec(dllimport) void DestroyBigArt(int id, int version);
	__declspec(dllimport) void DestroySmallArt(int id, int version);
	__declspec(dllimport) int IsBigArtRightSize(int id, int version, int width, int height);
	__declspec(dllimport) int IsSmallArtRightSize(int id, int version, int width, int height);
	__declspec(dllimport) void DestroyAllBigArts(void);
	__declspec(dllimport) void DestroyAllSmallArts(void);
	__declspec(dllimport) int IsBigArtIn(int id, int version);
	__declspec(dllimport) int IsSmallArtIn(int id, int version);
	__declspec(dllimport) int ReloadBigArtIfWrongSize(int id, int version, int width, int height);
	__declspec(dllimport) int ReloadSmallArtIfWrongSize(int id, int version, int width, int height);
	__declspec(dllimport) int DrawBigArt(HDC hdc, const RECT* rect, int id, int version);
	__declspec(dllimport) int DrawSmallArt(HDC hdc, const RECT* rect, int id, int version);
	int WINAPI DllMain(HINSTANCE hDllHandle, DWORD nReason, LPVOID reserved);
#ifdef __cplusplus
}
#endif

#endif
