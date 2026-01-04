#ifndef SHANDALAR_INTTYPES_H
#define SHANDALAR_INTTYPES_H

#if defined(_MSC_VER)
/* MSVC provides __int8/__int16/__int32/__int64 as built-in types. */
typedef unsigned char _BYTE;
typedef unsigned long _DWORD;
#if _MSC_VER >= 1600
	#include <cstdint.h>
#else
	typedef __int8              int8_t;
	typedef __int16             int16_t;
	typedef __int32             int32_t;
	typedef __int64             int64_t;
	typedef unsigned __int8     uint8_t;
	typedef unsigned __int16    uint16_t;
	typedef unsigned __int32    uint32_t;
	typedef unsigned __int64    uint64_t;
	typedef unsigned int		uint;
#endif
#else
/* Fallbacks for non-MSVC compilers and parsers. */
#ifndef __int8
typedef signed char __int8;
#endif
#ifndef __int16
typedef short __int16;
#endif
#ifndef __int32
typedef long __int32;
#endif
#ifndef __int64
typedef long long __int64;
#endif

typedef unsigned char _BYTE;
typedef unsigned long _DWORD;
#endif

/* Optional: enable this only for the Ghidra parser if needed. */
#ifdef SHANDALAR_GHIDRA_PARSER
#define __int8 unsigned__int8_alias
typedef unsigned char unsigned__int8_alias;
#endif

/* Ghidra compat */
typedef unsigned char undefined;
typedef unsigned char undefined1;
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned long long undefined8;

#endif
