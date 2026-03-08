#include "defs.h"
#include <WINDOWS.H>
#include "mystdbool.h"

#pragma intrinsic(_rotl)
#pragma intrinsic(_rotr)

// GLOBAL: DECKDLL 0x10035b88
// GLOBAL: MAGIC 0x56fd68
// GLOBAL: SHANDALAR 0x57f970
uint8_t card_coded[3][128];

// GLOBAL: DECKDLL 0x10033008
char DAT_10033008[] = {0x12, 0x0e, 0x07, 0x15, 0x16, 0x00, 0x13, 0x04, 0x1d, 0x0c, 0x28, 0x22, 0x33, 0x2e, 0x32, 0x2e, 0x27, 0x35, 0x1d, 0x16, 0x28, 0x2f, 0x25, 0x2e, 0x36, 0x32, 0x1d, 0x02, 0x34, 0x33, 0x33, 0x24, 0x2f, 0x35, 0x17, 0x24, 0x33, 0x32, 0x28, 0x2e, 0x2f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// GLOBAL: DECKDLL 0x10033038
char DAT_10033038[] = {0x12, 0x0e, 0x07, 0x15, 0x16, 0x00, 0x13, 0x04, 0x1d, 0x0c, 0x28, 0x22, 0x33, 0x2e, 0x32, 0x2e, 0x27, 0x35, 0x1d, 0x16, 0x28, 0x2f, 0x25, 0x2e, 0x36, 0x32, 0x1d, 0x02, 0x34, 0x33, 0x33, 0x24, 0x2f, 0x35, 0x17, 0x24, 0x33, 0x32, 0x28, 0x2e, 0x2f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

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
int DAT_101bc874;

// GLOBAL: DECKDLL 0x101bc878
int DAT_101bc878;

// GLOBAL: DECKDLL 0x101bc87c
int DAT_101bc87c;

// FUNCTION: DECKDLL 0x1000de30
void ScrubString(char *param_1)
{
  memset(param_1, 0x20, strlen(param_1));
}

// FUNCTION: DECKDLL 0x1000de56
void __cdecl ReverseBits32(uint *param_1)
{
  int i;
  uint result = 0;
  uint local_8 = 1;
  uint local_c = 0x80000000;
  for (i = 0; i < 0x20; i++)
  {
    if ((local_8 & *param_1) != 0)
    {
      result = result | local_c;
    }
    local_8 <<= 1;
    local_c >>= 1;
  }
  *param_1 = result;
}

// FUNCTION: DECKDLL 0x1000deb9
int InitLicenseSecretsFromRegistry(void)
{
  struct {
    int pad2;
    size_t sVar1;
    int pad;
    int local_420;
    DWORD local_41c;
    size_t local_418;
    uint i;
    HKEY local_410;
    BYTE local_40c[1024];
    char *local_c;
    DWORD local_8;
  } s;

  s.local_c = NULL;
  for (s.i = 0; strlen(s__100330c8) > s.i; s.i++)
  {
    s__100330c8[s.i] = DAT_10033008[s.i] ^ 0x41;
  }
  s.pad = strlen(s__100330c8);
  s__100330c8[s.pad] = '\0';

  if (RegOpenKeyA((HKEY)0x80000002, s__100330c8, &s.local_410) != 0)
  {
    ScrubString(s__100330c8);
    return 1;
  }

  ScrubString(s__100330c8);
  s.local_8 = 4;
  s.local_41c = 4;

  for (s.i = 0; strlen(s__100330f8) > s.i; s.i++)
  {
    s__100330f8[s.i] = DAT_10033060[s.i] ^ 0x41;
  }

  s.sVar1 = strlen(s__100330f8);
  s__100330f8[s.sVar1] = '\0';

  if (RegQueryValueExA(s.local_410, s__100330f8, (LPDWORD)0x0, &s.local_8, (LPBYTE)&DAT_101bc874,
                       &s.local_41c) != 0)
  {
    ScrubString(s__100330f8);
    RegCloseKey(s.local_410);
    return 1;
  }

  ScrubString(s__100330f8);
  s.local_8 = 1;
  s.local_41c = 0x400;

  for (s.i = 0; strlen(s__100330b8) > s.i; s.i++)
  {
    s__100330b8[s.i] = DAT_10033078[s.i] ^ 0x41;
  }

  s.pad2 = strlen(s__100330b8);
  s__100330b8[s.pad2] = '\0';

  if (RegQueryValueExA(s.local_410, s__100330b8, (LPDWORD)0x0, &s.local_8, s.local_40c, &s.local_41c) != 0)
  {
    ScrubString(s__100330b8);
    RegCloseKey(s.local_410);
    return 1;
  }

  ScrubString(s__100330b8);
  s.local_418 = strlen((char *)s.local_40c);

  for (s.local_420 = 0; strlen((char *)s.local_40c) < 0x10; )
  {
    s.local_40c[s.local_418] = s.local_40c[s.local_420];
    s.local_420 = s.local_420 + 1;
    s.local_418++;
    s.local_40c[s.local_418] = '\0';
  }
  
  //NOTE: this DEPENDS on stack layout, reading out of bounds intentionally
  s.local_c = (void *)((int)&s.i + strlen((char *)s.local_40c));
  memcpy(&DAT_101bc878, s.local_c, 4);
  s.local_c +=4;
  memcpy(&DAT_101bc87c, s.local_c, 4);
  s.local_c = NULL;
  RegCloseKey(s.local_410);
  return 0;
}

// DRM
// Return codes:
// 0 = success (valid serial)
// 1 = registry / allocation failure
// 2 = invalid serial
// FUNCTION: DECKDLL 0x1000e20a
static int ValidateRegistrySerial(uint32_t *out_value)
{
  struct {
    int regPathLength; // ebp - 0x40
    uint8_t *buf; // ebp - 0x3c
    uint32_t i; // ebp - 0x38
    int sumpt8; // ebp - 0x34
    int sumpt7; // ebp - 0x30
    char *pad6; // ebp - 0x2c
    uint32_t checksum; // ebp - 0x28
    int sumpt6; // ebp - 0x24
    uint32_t size; // ebp - 0x20
    int sumpt5; // ebp - 0x1c
    HKEY hKey; 
    int sumpt4; // ebp - 0x14
    int sumpt3; // ebp - 0x10
    int sumpt2; // ebp - 0xc
    uint32_t type; // ebp - 0x8
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
  memcpy(&s.checksum, s.pad6, 4);     // checksum
  s.pad6 = NULL;
  s.checksum ^= DAT_101bc874;

  // ------------------------------------------------------------
  // Nibble checksum verification
  // ------------------------------------------------------------
  *(byte*)&s.sumpt1 = ((DAT_101bc86c >> 24) & 0xF);
  *(byte*)&s.sumpt2 = ((DAT_101bc86c >> 16) & 0xF);
  *(byte*)&s.sumpt3 = ((DAT_101bc86c >> 8) & 0xF);
  *(byte*)&s.sumpt4 = ((DAT_101bc86c >> 0) & 0xF);
  *(byte*)&s.sumpt5 = ((DAT_101bc870 >> 24) & 0xF);
  *(byte*)&s.sumpt6 = ((DAT_101bc870 >> 16) & 0xF);
  *(byte*)&s.sumpt7 = ((DAT_101bc870 >> 8) & 0xF);
  *(byte*)&s.sumpt8 = ((DAT_101bc870 >> 0) & 0xF);

  //*(byte*)&s.sum = 

  if ((
    *(byte*)&s.sumpt1 +
    *(byte*)&s.sumpt2 +
    *(byte*)&s.sumpt3 +
    *(byte*)&s.sumpt4 +
    *(byte*)&s.sumpt5 +
    *(byte*)&s.sumpt6 +
    *(byte*)&s.sumpt7 +
    *(byte*)&s.sumpt8
  ) != s.checksum)
    return 2;
 
  // ------------------------------------------------------------
  // Serial decryption / mixing
  // ------------------------------------------------------------ 
  
  DAT_101bc86c ^= DAT_101bc878;
  DAT_101bc870 ^= DAT_101bc87c;

  //TODO: this is almost possible using _rotl/_rotr
  //the push/pop make no sense even for an inlined function
  __asm {
    push eax
    rol dword ptr [DAT_101bc874], 6
    mov eax, dword ptr [DAT_101bc86c]
    xor eax, dword ptr [DAT_101bc874]
    mov dword ptr [DAT_101bc86c], eax
    ror dword ptr [DAT_101bc874], 0xc
    mov eax, dword ptr [DAT_101bc870]
    xor eax, dword ptr [DAT_101bc874]
    mov dword ptr [DAT_101bc870], eax
    rol dword ptr [DAT_101bc874], 6
    pop eax
  }

  ReverseBits32(&DAT_101bc870);

  if (DAT_101bc86c != DAT_101bc870)
    return 2;

  *out_value = DAT_101bc86c;
  return 0;
}

// FUNCTION: DECKDLL 0x100271e3
unsigned int HasExpansion(unsigned char param_1)
{
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
}

// FUNCTION: DECKDLL 0x1002709f
// FUNCTION: MAGIC 0x496251
// FUNCTION: SHANDALAR 0x4669e0
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
