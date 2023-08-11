// We don't want to collide with the Hex-Rays defs.h so let's use the same header guard.
#ifndef HEXRAYS_DEFS_H
#define HEXRAYS_DEFS_H

// Verify that the widberg extensions are present
#ifndef __has_feature
#define __has_feature(x) 0 // Compatibility with non-clang compilers.
#endif
#ifndef __has_extension
#define __has_extension __has_feature // Compatibility with pre-3.0 compilers.
#endif

#if !__has_extension(widberg)
#error "This file requires a compiler that implements the widberg extensions."
#endif

// Freestanding Headers Only
#include <stddef.h> // NULL size_t
#include <limits.h> // CHAR_BIT

////////
// Types
////////
// Unsigned Types
typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;
typedef unsigned long long ulonglong;

// long long Types
typedef long long ll;
typedef unsigned long long ull;
typedef signed long long sll;

// long long Macros
#define MAKELL(integer_literal) integer_literal##LL
#define FMT_64 "ll"

// Fixed Width Integer Types
typedef __int8 int8;
typedef unsigned __int8 uint8;
typedef signed __int8 sint8;
typedef __int16 int16;
typedef unsigned __int16 uint16;
typedef signed __int16 sint16;
typedef __int32 int32;
typedef unsigned __int32 uint32;
typedef signed __int32 sint32;
typedef __int64 int64;
typedef unsigned __int64 uint64;
typedef signed __int64 sint64;
typedef __int128 int128;
typedef unsigned __int128 uint128;
typedef signed __int128 sint128;

// Fixed Width Integer Type Aliases
typedef uint8 _BYTE;
typedef uint16 _WORD;
typedef uint32 _DWORD;
typedef uint64 _QWORD;
typedef uint128 _OWORD;
typedef uint128 _LONGLONG;

// 10 Byte Floating Point Type
typedef long double _TBYTE;

// Windows Types
// Check if the Windows header was already included before defining these
#ifndef _WINDOWS_
typedef int8 BYTE;
typedef int16 WORD;
typedef int32 DWORD;
typedef int32 LONG;
typedef int64 QWORD;
typedef int BOOL;
#endif

// C bool polyfill
// C doesn't have a bool type but Hex-Rays uses one
#ifndef __cplusplus
typedef int bool;
#endif

// Fixed Width Bool Types
typedef int8 _BOOL1;
typedef int16 _BOOL2;
typedef int32 _BOOL4;
typedef int64 _BOOL8;

// Unknown Type
#define _UNKNOWN char

////////////////
// Type Coercion
////////////////
// __builtin_bit_cast requires source and destination types to be the same size and trivially copyable
// This is probably for the best
#define COERCE_FLOAT(value) __builtin_bit_cast(float, value)
#define COERCE_DOUBLE(value) __builtin_bit_cast(double, value)
#define COERCE_LONG_DOUBLE(value) __builtin_bit_cast(long double, value)
#define COERCE_UNSIGNED_INT(value) __builtin_bit_cast(unsigned int, value)
#define COERCE_UNSIGNED_INT64(value) __builtin_bit_cast(uint64, value)

/////////////////////
// Checked Arithmetic
/////////////////////
// Addition
#define __OFADD__(a, b) __builtin_add_overflow_p(a, b, (__typeof__((a) + (b)))0)
#define __CFADD__ __OFADD__

// Subtraction
// https://stackoverflow.com/a/21371401/3997768
#define GET_MACRO(_0, _1, _2, _3, NAME, ...) NAME
#define __OFSUB__(...) GET_MACRO(_0, ##__VA_ARGS__, __OFSUB__3, __OFSUB__2, __OFSUB__1, __OFSUB__0)(__VA_ARGS__)

#define __OFSUB__2(a, b) __builtin_sub_overflow_p(a, b, (__typeof__((a) - (b)))0)
#define __OFSUB__3(a, b, c) (__OFADD__(y, c) ^ __OFSUB__(x, y + c))

// https://stackoverflow.com/a/21371401/3997768
#define GET_MACRO(_0, _1, _2, _3, NAME, ...) NAME
#define __CFSUB__(...) GET_MACRO(_0, ##__VA_ARGS__, __CFSUB__3, __CFSUB__2, __CFSUB__1, __CFSUB__0)(__VA_ARGS__)

#define __CFSUB__2(a, b) __OFSUB__2
#define __CFSUB__3(a, b, c) (__CFADD__(y, c) ^ __CFSUB__(x, y + c))

// Multiplication
#define is_mul_ok(a, b) __builtin_mul_overflow_p(a, b, (__typeof__((a) * (b)))0)
#define saturated_mul(a, b) (is_mul_ok(a, b) ? a * b : (__typeof__((a) * (b)))(-1))

// Absolute Value
inline uint8 abs8(int8 value) { return value < 0 ? -value : value; }
inline uint16 abs16(int16 value) { return value < 0 ? -value : value; }
inline uint32 abs32(int32 value) { return value < 0 ? -value : value; }
inline uint64 abs64(int64 value) { return value < 0 ? -value : value; }
inline uint128 abs128(int128 value) { return value < 0 ? -value : value; }

/////////////////
// Bit Operations
/////////////////
// Rotate
#define __ROL1__ __builtin_rotateleft8
#define __ROL2__ __builtin_rotateleft16
#define __ROL4__ __builtin_rotateleft32
#define __ROL8__ __builtin_rotateleft64
#define __ROR1__ __builtin_rotateright8
#define __ROR2__ __builtin_rotateright16
#define __ROR4__ __builtin_rotateright32
#define __ROR8__ __builtin_rotateright64

// Rotate through carry
// TODO: Implement these with compiler built-ins/intrinsics
#define __RCL__(...)
#define __RCR__(...)
#define __MKCRCL__(...)
#define __MKCRCR__(...)

// Shift
#define __MKCSHL__(value, count) (((value >> (count - (count % (sizeof(value) * CHAR_BIT)))) & 1) == 1)
#define __MKCSHR__(value, count) (((value >> (count - 1)) & 1) == 1)

// Sign bit
#define __SETS__(value) (sizeof(value) * CHAR_BIT != 0 ? ((value >> (sizeof(value) * CHAR_BIT - 1)) & 1) == 1 : false)

// Parity
#define __SETP__(a, b) __builtin_parity(a - b)

////////////////
// Memory Access
////////////////
// Indices
#define LAST_IND(value, type) (sizeof(value) / sizeof(type) - 1)
#ifdef __LITTLE_ENDIAN__
#define LOW_IND(value, type) 0
#define HIGH_IND(value, type) LAST_IND(value, type)
#else
#define LOW_IND(value, type) LAST_IND(value, type)
#define HIGH_IND(value, type) 0
#endif

// Unsigned Types
#define BYTEn(value, index) (((_BYTE *)&value)[index])
#define WORDn(value, index) (((_WORD *)&value)[index])
#define DWORDn(value, index) (((_DWORD *)&value)[index])

#define LOBYTE(value) BYTEn(value, LOW_IND(value, _BYTE))
#define LOWORD(value) WORDn(value, LOW_IND(value, _WORD))
#define LODWORD(value) DWORDn(value, LOW_IND(value, _DWORD))
#define HIBYTE(value) BYTEn(value, HIGH_IND(value, _BYTE))
#define HIWORD(value) WORDn(value, HIGH_IND(value, _WORD))
#define HIDWORD(value) DWORDn(value, HIGH_IND(value, _DWORD))
#define BYTE1(value) BYTEn(value, 1)
#define BYTE2(value) BYTEn(value, 2)
#define BYTE3(value) BYTEn(value, 3)
#define BYTE4(value) BYTEn(value, 4)
#define BYTE5(value) BYTEn(value, 5)
#define BYTE6(value) BYTEn(value, 6)
#define BYTE7(value) BYTEn(value, 7)
#define BYTE8(value) BYTEn(value, 8)
#define BYTE9(value) BYTEn(value, 9)
#define BYTE10(value) BYTEn(value, 10)
#define BYTE11(value) BYTEn(value, 11)
#define BYTE12(value) BYTEn(value, 12)
#define BYTE13(value) BYTEn(value, 13)
#define BYTE14(value) BYTEn(value, 14)
#define BYTE15(value) BYTEn(value, 15)
#define WORD1(value) WORDn(value, 1)
#define WORD2(value) WORDn(value, 2)
#define WORD3(value) WORDn(value, 3)
#define WORD4(value) WORDn(value, 4)
#define WORD5(value) WORDn(value, 5)
#define WORD6(value) WORDn(value, 6)
#define WORD7(value) WORDn(value, 7)

// Signed Types
#define SBYTEn(value, index) (((int8 *)&value)[index])
#define SWORDn(value, index) (((int16 *)&value)[index])
#define SDWORDn(value, index) (((int32 *)&value)[index])

#define SLOBYTE(value) SBYTEn(value, LOW_IND(value, int8))
#define SLOWORD(value) SWORDn(value, LOW_IND(value, int16))
#define SLODWORD(value) SDWORDn(value, LOW_IND(value, int32))
#define SHIBYTE(value) SBYTEn(value, HIGH_IND(value, int8))
#define SHIWORD(value) SWORDn(value, HIGH_IND(value, int16))
#define SHIDWORD(value) SDWORDn(value, HIGH_IND(value, int32))
#define SBYTE1(value) SBYTEn(value, 1)
#define SBYTE2(value) SBYTEn(value, 2)
#define SBYTE3(value) SBYTEn(value, 3)
#define SBYTE4(value) SBYTEn(value, 4)
#define SBYTE5(value) SBYTEn(value, 5)
#define SBYTE6(value) SBYTEn(value, 6)
#define SBYTE7(value) SBYTEn(value, 7)
#define SBYTE8(value) SBYTEn(value, 8)
#define SBYTE9(value) SBYTEn(value, 9)
#define SBYTE10(value) SBYTEn(value, 10)
#define SBYTE11(value) SBYTEn(value, 11)
#define SBYTE12(value) SBYTEn(value, 12)
#define SBYTE13(value) SBYTEn(value, 13)
#define SBYTE14(value) SBYTEn(value, 14)
#define SBYTE15(value) SBYTEn(value, 15)
#define SWORD1(value) SWORDn(value, 1)
#define SWORD2(value) SWORDn(value, 2)
#define SWORD3(value) SWORDn(value, 3)
#define SWORD4(value) SWORDn(value, 4)
#define SWORD5(value) SWORDn(value, 5)
#define SWORD6(value) SWORDn(value, 6)
#define SWORD7(value) SWORDn(value, 7)

#define __SPAIR16__(high, low) (((int16)(high) << 8) | (uint8)(low))
#define __SPAIR32__(high, low) (((int32)(high) << 16) | (uint16)(low))
#define __SPAIR64__(high, low) (((int64)(high) << 32) | (uint32)(low))
#define __SPAIR128__(high, low) (((int128)(high) << 64) | (uint64)(low))
#define __PAIR16__(high, low) (((uint16)(high) << 8) | (uint8)(low))
#define __PAIR32__(high, low) (((uint32)(high) << 16) | (uint16)(low))
#define __PAIR64__(high, low) (((uint64)(high) << 32) | (uint32)(low))
#define __PAIR128__(high, low) (((uint128)(high) << 64) | (uint64)(low))

//////////
// qmemcpy
//////////
inline void *qmemcpy(void *dest, const void *src, size_t count)
{
  char *dest_ = (char *)dest;
  const char *src_ = (const char *)src;

// No funny business. One byte at a time, beginning to end.
#pragma clang loop vectorize(disable)
#pragma clang loop interleave(disable)
#pragma clang loop unroll(disable)
  while (count--)
  {
    *dest_++ = *src_++;
  }
  return dest;
}

/////////////
// Attributes
/////////////
// Function Attributes
#define __pure __attribute__((pure))
#define __noreturn __attribute__((noreturn))
#define __high

// Parameter Attributes
#define __hidden
#define __return_ptr
#define __struct_ptr
#define __array_ptr
#define __unused

// Pointer Attributes
// These are implemented by the compiler
// #define __ptr32
// #define __ptr64
// #define __sptr
// #define __uptr

// Struct Attributes
#define __cppobj

///////////////////
// Shifted Pointers
///////////////////
// https://hex-rays.com/products/ida/support/idadoc/1695.shtml
// These are implemented by the compiler
// #define __shifted(parent, delta)
// #define __parentof(value)
// #define __deltaof(value)
// This is a predefined macro by the compiler
// #define ADJ(ptr) (__parentof(ptr) *)((char *)ptr - __deltaof(ptr))

//////////////////////
// Calling Conventions
//////////////////////
// These are already implemented in the compiler
// https://www.hex-rays.com/products/ida/support/idadoc/1361.shtml
// __usercall __spoils __userpurge

#endif // HEXRAYS_DEFS_H
