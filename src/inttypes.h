#ifndef SHANDALAR_INTTYPES_H
#define SHANDALAR_INTTYPES_H

#if defined(_MSC_VER)
/* MSVC provides __int8/__int16/__int32/__int64 as built-in types. */
typedef unsigned char _BYTE;
typedef unsigned long _DWORD;

typedef __int8              int8_t;
typedef __int16             int16_t;
typedef __int32             int32_t;
typedef __int64             int64_t;
typedef unsigned __int8     uint8_t;
typedef unsigned __int16    uint16_t;
typedef unsigned __int32    uint32_t;
typedef unsigned __int64    uint64_t;
typedef unsigned int		uint;

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
/* Avoid `long long` for MSVC 4.x. */
#if defined(_MSC_VER)
typedef unsigned __int64 undefined8;
#else
typedef unsigned long long undefined8;
#endif

/* Ghidra decompiler helper typedefs/macros */
#if defined(_MSC_VER)
typedef __int64 longlong;
typedef unsigned __int64 ulonglong;
#else
typedef long long longlong;
typedef unsigned long long ulonglong;
#endif

#define CONCAT31(x,y) ((undefined4)(((undefined4)(x) << 8) | ((undefined1)((undefined1)(y)))))
#define CONCAT11(x,y) ((undefined2)(((undefined1)(x)) | ((undefined2)((undefined1)(y)) << 8)))
#define CONCAT44(x,y) ((undefined8)(((ulonglong)(x) << 32) | (undefined4)(y)))

#endif
