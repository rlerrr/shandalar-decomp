#include "defs.h"
#include <WINDOWS.H>
#include "mystdbool.h"

#pragma intrinsic(_rotl)
#pragma intrinsic(_rotr)

// GLOBAL: DECKDLL 0x10035b88
// GLOBAL: MAGIC 0x00571d68
uint8_t card_coded[3][128] = {
    {255, 229, 239, 119, 127, 181, 191, 206, 127, 59, 254, 255, 126, 247, 63, 250, 235, 217, 239, 255, 255, 235, 255, 223, 219, 55, 235, 246, 251, 255, 253, 175, 121, 250, 255, 255, 237, 144, 7, 68, 66, 0, 155, 12, 0, 49, 8, 193, 16, 158, 1, 178, 74, 74, 194, 48, 65, 84, 132, 200, 251, 213, 208, 120, 133, 32, 241, 13, 202, 0, 9, 128, 208, 33, 0, 81, 0, 85, 20, 0, 36, 2, 96, 0, 128, 0, 0, 12, 64, 128, 0, 0, 65, 34, 17, 40, 1, 96, 0, 12, 8, 36, 2, 132, 40, 2, 216, 248, 255, 0, 32, 12, 16, 64, 0, 0, 140, 202, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 26, 16, 0, 128, 74, 64, 49, 128, 132, 1, 0, 129, 8, 192, 4, 20, 38, 16, 0, 0, 20, 0, 32, 36, 200, 20, 9, 4, 0, 2, 80, 134, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 97, 250, 77, 181, 181, 61, 205, 190, 137, 123, 55, 4, 42, 39, 135, 90, 223, 14, 242, 53, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 208, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 75, 16, 26, 25, 148, 4, 66, 10, 0, 196, 16, 42, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 18, 32, 9, 66, 16, 168, 146, 160, 128, 8, 129, 4, 20, 40, 37, 72, 16, 225, 1, 10, 192, 5, 128, 0, 40, 128, 76, 0, 5, 82, 32, 0, 1, 8, 128, 60, 5, 0, 0, 0, 128, 0, 0, 0, 0, 0, 0, 49, 153, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

// GLOBAL: DECKDLL 0x10033008
char DAT_10033008[] = {0x12, 0x0e, 0x07, 0x15, 0x16, 0x00, 0x13, 0x04, 0x1d, 0x0c, 0x28, 0x22, 0x33, 0x2e, 0x32, 0x2e, 0x27, 0x35, 0x1d, 0x16, 0x28, 0x2f, 0x25, 0x2e, 0x36, 0x32, 0x1d, 0x02, 0x34, 0x33, 0x33, 0x24, 0x2f, 0x35, 0x17, 0x24, 0x33, 0x32, 0x28, 0x2e, 0x2f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// GLOBAL: DECKDLL 0x10033038
char DAT_10033038[40] = {0x12, 0x0e, 0x07, 0x15, 0x16, 0x00, 0x13, 0x04, 0x1d, 0x0c, 0x28, 0x22, 0x33, 0x2e, 0x11, 0x33, 0x2e, 0x32, 0x24, 0x1d, 0x0c, 0x20, 0x26, 0x28, 0x22, 0x7b, 0x61, 0x15, 0x29, 0x24, 0x61, 0x06, 0x20, 0x35, 0x29, 0x24, 0x33, 0x28, 0x2f, 0x26};

// GLOBAL: DECKDLL 0x10033060
char DAT_10033060[] = {0x07, 0x28, 0x33, 0x32, 0x35, 0x08, 0x2f, 0x32, 0x35, 0x20, 0x2d, 0x2d, 0x05, 0x20, 0x35, 0x24, 0x15, 0x28, 0x2c, 0x24, 0x00, 0x00, 0x00, 0x00};

// GLOBAL: DECKDLL 0x10033078
char DAT_10033078[] = {0x11, 0x33, 0x2e, 0x25, 0x34, 0x22, 0x35, 0x08, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// GLOBAL: DECKDLL 0x10033088
char s__10033088[] = {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00};

// GLOBAL: DECKDLL 0x100330b8
char s__100330b8[] = {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00};

// GLOBAL: DECKDLL 0x100330c8
char s__100330c8[] = {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00};

// GLOBAL: DECKDLL 0x100330f8
char s__100330f8[] = {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00};

// GLOBAL: DECKDLL 0x101bc86c
uint32_t DAT_101bc86c;

// GLOBAL: DECKDLL 0x101bc870
int DAT_101bc870;

// GLOBAL: DECKDLL 0x101bc874
int global_Windows_FirstInstallDateTime;

// GLOBAL: DECKDLL 0x101bc878
int DAT_101bc878;

// GLOBAL: DECKDLL 0x101bc87c
int DAT_101bc87c;

// FUNCTION: DECKDLL 0x1000de30
// FUNCTION: MAGIC 0x0043d210
static void ScrubString(char *param_1)
{
  memset(param_1, 0x20, strlen(param_1));
}

// FUNCTION: DECKDLL 0x1000de56
static void ReverseBits32(uint *param_1)
{
  struct
  {
    int i;        // ebp - 0x10
    uint result;  // ebp - 0x0c
    uint local_c; // ebp - 0x08
    uint local_8; // ebp - 0x04
  } s;

  s.result = 0;
  s.local_8 = 1;
  s.local_c = 0x80000000;
  for (s.i = 0; s.i < 0x20; s.i++)
  {
    if (s.local_8 & *param_1)
      s.result |= s.local_c;
    s.local_8 <<= 1;
    s.local_c >>= 1;
  }
  *param_1 = s.result;
}

// FUNCTION: DECKDLL 0x1000deb9
// FUNCTION: MAGIC 0x0043d29b
int InitLicenseSecretsFromRegistry(void)
{
#ifndef MODERN_FIXES
  struct
  {
    int sProductIdLength;
    size_t sFirstInstallDateTimeLength;
    int sWindowsCurrentVersionKeyLength;
    int local_420;
    DWORD lpcbData;
    size_t winProductIdLength;
    uint i;
    HKEY hKey;
    BYTE winProductId[1024];
    char *prodKeyPtr;
    DWORD lpType;
  } s;

  s.prodKeyPtr = NULL;
  // This generates "SOFTWARE\Microsoft\Windows\CurrentVersion"
  for (s.i = 0; strlen(s__100330c8) > s.i; s.i++)
  {
    s__100330c8[s.i] = DAT_10033008[s.i] ^ 0x41;
  }
  s.sWindowsCurrentVersionKeyLength = strlen(s__100330c8);
  s__100330c8[s.sWindowsCurrentVersionKeyLength] = '\0';

  if (RegOpenKeyA((HKEY)0x80000002, s__100330c8, &s.hKey) != 0)
  {
    ScrubString(s__100330c8);
    return 1;
  }

  ScrubString(s__100330c8);
  s.lpType = 4;
  s.lpcbData = 4;

  // This generates "FirstInstallDateTime" which is a Windows 95/98/ME thing
  for (s.i = 0; strlen(s__100330f8) > s.i; s.i++)
  {
    s__100330f8[s.i] = DAT_10033060[s.i] ^ 0x41;
  }

  s.sFirstInstallDateTimeLength = strlen(s__100330f8);
  s__100330f8[s.sFirstInstallDateTimeLength] = '\0';

  if (RegQueryValueExA(s.hKey, s__100330f8, (LPDWORD)0x0, &s.lpType, (LPBYTE)&global_Windows_FirstInstallDateTime, &s.lpcbData) != 0)
  {
    ScrubString(s__100330f8);
    RegCloseKey(s.hKey);
    return 1;
  }

  ScrubString(s__100330f8);
  s.lpType = 1;
  s.lpcbData = 0x400;

  // This generates "ProductId"
  for (s.i = 0; strlen(s__100330b8) > s.i; s.i++)
  {
    s__100330b8[s.i] = DAT_10033078[s.i] ^ 0x41;
  }

  s.sProductIdLength = strlen(s__100330b8);
  s__100330b8[s.sProductIdLength] = '\0';

  if (RegQueryValueExA(s.hKey, s__100330b8, (LPDWORD)0x0, &s.lpType, s.winProductId, &s.lpcbData) != 0)
  {
    ScrubString(s__100330b8);
    RegCloseKey(s.hKey);
    return 1;
  }

  ScrubString(s__100330b8);
  s.winProductIdLength = strlen((char *)s.winProductId);

  // Repeats product key into itself up to 16 chars
  for (s.local_420 = 0; strlen((char *)s.winProductId) < 0x10;)
  {
    s.winProductId[s.winProductIdLength] = s.winProductId[s.local_420];
    s.local_420 = s.local_420 + 1;
    s.winProductIdLength++;
    s.winProductId[s.winProductIdLength] = '\0';
  }

  // Using product key as entropy in a weird way (just the last 8 chars)
  s.prodKeyPtr = &(s.winProductId - 8)[strlen((char *)s.winProductId)];
  memcpy(&DAT_101bc878, s.prodKeyPtr, 4);
  s.prodKeyPtr += 4;
  memcpy(&DAT_101bc87c, s.prodKeyPtr, 4);
  s.prodKeyPtr = NULL;
  RegCloseKey(s.hKey);
#endif
  return 0;
}

// DRM
// Return codes:
// 0 = success (valid serial)
// 1 = registry / allocation failure
// 2 = invalid serial
// FUNCTION: DECKDLL 0x1000e20a
// FUNCTION: MAGIC 0x0043d5ec
static int ValidateRegistrySerial(uint32_t *out_value)
{
  struct
  {
    int regPathLength; // ebp - 0x40
    uint8_t *buf;      // ebp - 0x3c
    uint32_t i;        // ebp - 0x38
    int sumpt8;        // ebp - 0x34
    int sumpt7;        // ebp - 0x30
    char *pad6;        // ebp - 0x2c
    uint32_t checksum; // ebp - 0x28
    int sumpt6;        // ebp - 0x24
    uint32_t size;     // ebp - 0x20
    int sumpt5;        // ebp - 0x1c
    HKEY hKey;
    int sumpt4;      // ebp - 0x14
    int sumpt3;      // ebp - 0x10
    int sumpt2;      // ebp - 0xc
    uint32_t type;   // ebp - 0x8
    uint32_t sumpt1; // ebp - 0x4
  } s;

  *out_value = 0;

  for (s.i = 0; s.i < strlen(s__10033088); s.i++)
  {
    s__10033088[s.i] = DAT_10033038[s.i] ^ 0x41;
  }
  s.regPathLength = strlen(s__10033088);
  s__10033088[s.regPathLength] = '\0';

  // ------------------------------------------------------------
  // Open HKLM\<decoded key>
  // ------------------------------------------------------------
  if (RegOpenKeyA(HKEY_LOCAL_MACHINE, s__10033088, &s.hKey) != ERROR_SUCCESS)
  {
    ScrubString(s__10033088); // wipe decoded string
    return 1;
  }

  ScrubString(s__10033088); // wipe decoded string again

  // ------------------------------------------------------------
  // Allocate buffer for serial (12 bytes)
  // ------------------------------------------------------------
  s.size = 12;
  s.type = REG_BINARY;
  s.buf = malloc(s.size);
  if (!s.buf)
  {
    RegCloseKey(s.hKey);
    return 1;
  }

  // ------------------------------------------------------------
  // Read "SerialNumber" value
  // ------------------------------------------------------------
  if (RegQueryValueExA(
          s.hKey,
          "SerialNumber",
          NULL,
          &s.type,
          s.buf,
          &s.size) != ERROR_SUCCESS)
  {
    RegCloseKey(s.hKey);
    return 1;
  }

  RegCloseKey(s.hKey);

  // ------------------------------------------------------------
  // Split serial fields
  // ------------------------------------------------------------
  s.pad6 = s.buf;
  memcpy(&DAT_101bc86c, s.pad6, 4); // part A
  s.pad6 += 4;
  memcpy(&DAT_101bc870, s.pad6, 4); // part B
  s.pad6 += 4;
  memcpy(&s.checksum, s.pad6, 4); // checksum
  s.pad6 = NULL;
  s.checksum ^= global_Windows_FirstInstallDateTime;

  // ------------------------------------------------------------
  // Nibble checksum verification
  // ------------------------------------------------------------
  *(byte *)&s.sumpt1 = ((DAT_101bc86c >> 24) & 0xF);
  *(byte *)&s.sumpt2 = ((DAT_101bc86c >> 16) & 0xF);
  *(byte *)&s.sumpt3 = ((DAT_101bc86c >> 8) & 0xF);
  *(byte *)&s.sumpt4 = ((DAT_101bc86c >> 0) & 0xF);
  *(byte *)&s.sumpt5 = ((DAT_101bc870 >> 24) & 0xF);
  *(byte *)&s.sumpt6 = ((DAT_101bc870 >> 16) & 0xF);
  *(byte *)&s.sumpt7 = ((DAT_101bc870 >> 8) & 0xF);
  *(byte *)&s.sumpt8 = ((DAT_101bc870 >> 0) & 0xF);

  //*(byte*)&s.sum =

  if ((
          *(byte *)&s.sumpt1 +
          *(byte *)&s.sumpt2 +
          *(byte *)&s.sumpt3 +
          *(byte *)&s.sumpt4 +
          *(byte *)&s.sumpt5 +
          *(byte *)&s.sumpt6 +
          *(byte *)&s.sumpt7 +
          *(byte *)&s.sumpt8) != s.checksum)
    return 2;

  // ------------------------------------------------------------
  // Serial decryption / mixing
  // ------------------------------------------------------------

  DAT_101bc86c ^= DAT_101bc878;
  DAT_101bc870 ^= DAT_101bc87c;

  // TODO: this is almost possible using _rotl/_rotr
  // the push/pop make no sense even for an inlined function
  __asm {
    push eax
    rol dword ptr [global_Windows_FirstInstallDateTime], 6
    mov eax, dword ptr [DAT_101bc86c]
    xor eax, dword ptr [global_Windows_FirstInstallDateTime]
    mov dword ptr [DAT_101bc86c], eax
    ror dword ptr [global_Windows_FirstInstallDateTime], 0xc
    mov eax, dword ptr [DAT_101bc870]
    xor eax, dword ptr [global_Windows_FirstInstallDateTime]
    mov dword ptr [DAT_101bc870], eax
    rol dword ptr [global_Windows_FirstInstallDateTime], 6
    pop eax
  }

  ReverseBits32(&DAT_101bc870);

  if (DAT_101bc86c != DAT_101bc870)
    return 2;

  *out_value = DAT_101bc86c;
  return 0;
}

// FUNCTION: DECKDLL 0x100271e3
// FUNCTION: MAGIC 0x00497a09
unsigned int HasExpansion(unsigned char param_1)
{
#ifndef MODERN_FIXES
  unsigned int serial;
  unsigned int result;

  result = 0;
  if ((param_1 & 1) == 1)
    return 1;

  serial = 0;
  if (ValidateRegistrySerial(&serial) != 0)
    return 0;

  if ((param_1 & 1) == 1 && (serial & 0x400000))
    result = 1;
  else if ((param_1 & 2) == 2 && (serial & 0x100))
    result = 1;
  else if ((param_1 & 4) == 4 && (serial & 0x800))
    result = 1;
  else if ((param_1 & 8) == 8 && (serial & 0x20000))
    result = 1;
  else if ((param_1 & 0x10) == 0x10 && (serial & 0x10000000))
    result = 1;
  else
    result = 0;

  return result;
#else
  return 1;
#endif
}

// FUNCTION: DECKDLL 0x1002709f
// FUNCTION: MAGIC 0x00497811
int IsCardAvailable(csvid_t csvid, int expansion)
{
  int result;

  result = 1;
  if (expansion == 1)
  {
    if (HasExpansion(2) == 0)
      result = 0;
  }
  if (expansion == 2)
  {
    if (HasExpansion(4) == 0)
      result = 0;
  }
  if ((((csvid < 0) || (0x3FF < csvid)) || (expansion < 0)) || (2 < expansion))
    result = 0;

  if ((*(int *)&card_coded[expansion][(csvid >> 5) << 2] & (1 << (csvid & 0x1f))) == 0)
    result = 0;

  return result;
}
