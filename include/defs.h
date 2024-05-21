// We don't want to collide with the Hex-Rays defs.h so let's use the same header guard.
#ifndef HEXRAYS_DEFS_H
#define HEXRAYS_DEFS_H

// Freestanding Headers Only
#include <limits.h> // CHAR_BIT
#include <stddef.h> // NULL size_t offsetof
#include <stdint.h> // [u]intX_t

#ifndef __is_identifier
  #define __is_identifier(x) 0 // Compatibility with non-clang compilers.
#endif

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
typedef int8_t int8;
typedef uint8_t uint8;
typedef int8_t sint8;
typedef int16_t int16;
typedef uint16_t uint16;
typedef int16_t sint16;
typedef int32_t int32;
typedef uint32_t uint32;
typedef int32_t sint32;
typedef int64_t int64;
typedef uint64_t uint64;
typedef int64_t sint64;

// If the target doesn't support 128 bit integers, define one ourselves
#ifndef __SIZEOF_INT128__
#define __int128 _BitInt(128)
#endif
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
typedef int BOOL;
#endif
typedef int64 QWORD;

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
#define COERCE_FLOAT(value) __builtin_bit_cast(float, (value))
#define COERCE_DOUBLE(value) __builtin_bit_cast(double, (value))
#define COERCE_LONG_DOUBLE(value) __builtin_bit_cast(long double, (value))
#define COERCE_UNSIGNED_INT(value) __builtin_bit_cast(unsigned int, (value))
#define COERCE_UNSIGNED_INT64(value) __builtin_bit_cast(uint64, (value))

#ifndef CONTAINING_RECORD
#define CONTAINING_RECORD(address, type, field) ((type *)((_BYTE*)(address) - offsetof(type, field)))
#endif

/////////////////////
// Checked Arithmetic
/////////////////////
// Addition
// Clang does not support __builtin_add_overflow_p
#define __OFADD__(a, b) ({ __typeof__((a) + (b)) x; __builtin_add_overflow((a), (b), &x); })
#define __CFADD__(a, b) ((__typeof__((a) + (b)))(a) > ((a) + (b)))

// Subtraction
// https://stackoverflow.com/a/21371401/3997768
#define GET_MACRO(_0, _1, _2, _3, NAME, ...) NAME
#define __OFSUB__(...) GET_MACRO(_0, ##__VA_ARGS__, __OFSUB__3, __OFSUB__2, __OFSUB__1, __OFSUB__0)(__VA_ARGS__)

// Clang does not support __builtin_sub_overflow_p
#define __OFSUB__2(a, b) ({ __typeof__((a) - (b)) x; __builtin_sub_overflow((a), (b), &x); })
#define __OFSUB__3(a, b, c) (__OFADD__(y, c) ^ __OFSUB__(x, y + c))

// https://stackoverflow.com/a/21371401/3997768
#define GET_MACRO(_0, _1, _2, _3, NAME, ...) NAME
#define __CFSUB__(...) GET_MACRO(_0, ##__VA_ARGS__, __CFSUB__3, __CFSUB__2, __CFSUB__1, __CFSUB__0)(__VA_ARGS__)

#define __CFSUB__2(a, b) ((__typeof__((a) - (b)))(a) < (__typeof__((a) - (b)))(b))
#define __CFSUB__3(a, b, c) (__CFADD__((b), (c)) ^ __CFSUB__((a), (b) + c))

// Multiplication
// Clang does not support __builtin_mul_overflow_p
#define is_mul_ok(a, b) ({ __typeof__((a) * (b)) x; __builtin_mul_overflow((a), (b), &x); })
#define saturated_mul(a, b) (is_mul_ok((a), (b)) ? (a) * (b) : (__typeof__((a) * (b)))(-1))

// Absolute Value
inline uint8 abs8(int8 value) { return value < 0 ? -value : value; }
inline uint16 abs16(int16 value) { return value < 0 ? -value : value; }
inline uint32 abs32(int32 value) { return value < 0 ? -value : value; }
inline uint64 abs64(int64 value) { return value < 0 ? -value : value; }
inline uint128 abs128(int128 value) { return value < 0 ? -value : value; }

/////////////////
// Bit Operations
/////////////////
// Sign bit
#define __SETS__(value) (sizeof(value) * CHAR_BIT != 0 ? (((value) >> (sizeof(value) * CHAR_BIT - 1)) & 1) == 1 : 0)

// Parity
#define __SETP__(a, b) __builtin_parity((a) - (b))

// Shift
#define __MKCSHL__(value, count) ((((value) >> ((count) - ((count) % (sizeof(value) * CHAR_BIT)))) & 1) == 1)
#define __MKCSHR__(value, count) ((((value) >> ((count) - 1)) & 1) == 1)

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
// The signatures on these really should take in a carry bit
// https://gcc.gnu.org/bugzilla//show_bug.cgi?id=99582
#define __RCL__(value, count, carry) (((value) << (count)) | ((value) >> (sizeof(value) * CHAR_BIT - (count))) | ((carry) << ((count) - 1)))
#define __RCR__(value, count, carry) (((value) >> (count)) | ((value) << (sizeof(value) * CHAR_BIT - (count))) | ((carry) << (sizeof(value) * CHAR_BIT - (count) - 1)))
#define __MKCRCL__(value, count, carry) (__RCL__((value), (count), (carry)) & 1)
#define __MKCRCR__(value, count, carry) (__SETS__(__RCR__((value), (count), (carry))))

////////////////
// Memory Access
////////////////
// Indices
#define LAST_IND(value, type) (sizeof(value) / sizeof(type) - 1)
#ifdef __LITTLE_ENDIAN__
#define LOW_IND(value, type) 0
#define HIGH_IND(value, type) LAST_IND((value), type)
#else
#define LOW_IND(value, type) LAST_IND((value), type)
#define HIGH_IND(value, type) 0
#endif

// Unsigned Types
#define BYTEn(value, index) (((_BYTE *)&(value))[index])
#define WORDn(value, index) (((_WORD *)&(value))[index])
#define DWORDn(value, index) (((_DWORD *)&(value))[index])

#ifndef LOBYTE
  #define LOBYTE(value) BYTEn((value), LOW_IND((value), _BYTE))
#endif
#ifndef LOWORD
  #define LOWORD(value) WORDn((value), LOW_IND((value), _WORD))
#endif
#define LODWORD(value) DWORDn((value), LOW_IND((value), _DWORD))
#ifndef HIBYTE
  #define HIBYTE(value) BYTEn((value), HIGH_IND((value), _BYTE))
#endif
#ifndef HIWORD
  #define HIWORD(value) WORDn((value), HIGH_IND((value), _WORD))
#endif
#define HIDWORD(value) DWORDn((value), HIGH_IND((value), _DWORD))
#define BYTE1(value) BYTEn((value), 1)
#define BYTE2(value) BYTEn((value), 2)
#define BYTE3(value) BYTEn((value), 3)
#define BYTE4(value) BYTEn((value), 4)
#define BYTE5(value) BYTEn((value), 5)
#define BYTE6(value) BYTEn((value), 6)
#define BYTE7(value) BYTEn((value), 7)
#define BYTE8(value) BYTEn((value), 8)
#define BYTE9(value) BYTEn((value), 9)
#define BYTE10(value) BYTEn((value), 10)
#define BYTE11(value) BYTEn((value), 11)
#define BYTE12(value) BYTEn((value), 12)
#define BYTE13(value) BYTEn((value), 13)
#define BYTE14(value) BYTEn((value), 14)
#define BYTE15(value) BYTEn((value), 15)
#define WORD1(value) WORDn((value), 1)
#define WORD2(value) WORDn((value), 2)
#define WORD3(value) WORDn((value), 3)
#define WORD4(value) WORDn((value), 4)
#define WORD5(value) WORDn((value), 5)
#define WORD6(value) WORDn((value), 6)
#define WORD7(value) WORDn((value), 7)

// Signed Types
#define SBYTEn(value, index) (((int8 *)&(value))[index])
#define SWORDn(value, index) (((int16 *)&(value))[index])
#define SDWORDn(value, index) (((int32 *)&(value))[index])

#define SLOBYTE(value) SBYTEn((value), LOW_IND((value), int8))
#define SLOWORD(value) SWORDn((value), LOW_IND((value), int16))
#define SLODWORD(value) SDWORDn((value), LOW_IND((value), int32))
#define SHIBYTE(value) SBYTEn((value), HIGH_IND((value), int8))
#define SHIWORD(value) SWORDn((value), HIGH_IND((value), int16))
#define SHIDWORD(value) SDWORDn((value), HIGH_IND((value), int32))
#define SBYTE1(value) SBYTEn((value), 1)
#define SBYTE2(value) SBYTEn((value), 2)
#define SBYTE3(value) SBYTEn((value), 3)
#define SBYTE4(value) SBYTEn((value), 4)
#define SBYTE5(value) SBYTEn((value), 5)
#define SBYTE6(value) SBYTEn((value), 6)
#define SBYTE7(value) SBYTEn((value), 7)
#define SBYTE8(value) SBYTEn((value), 8)
#define SBYTE9(value) SBYTEn((value), 9)
#define SBYTE10(value) SBYTEn((value), 10)
#define SBYTE11(value) SBYTEn((value), 11)
#define SBYTE12(value) SBYTEn((value), 12)
#define SBYTE13(value) SBYTEn((value), 13)
#define SBYTE14(value) SBYTEn((value), 14)
#define SBYTE15(value) SBYTEn((value), 15)
#define SWORD1(value) SWORDn((value), 1)
#define SWORD2(value) SWORDn((value), 2)
#define SWORD3(value) SWORDn((value), 3)
#define SWORD4(value) SWORDn((value), 4)
#define SWORD5(value) SWORDn((value), 5)
#define SWORD6(value) SWORDn((value), 6)
#define SWORD7(value) SWORDn((value), 7)

// Packed Pairs
#define __SPAIR16__(high, low) (((int16)(high) << 8) | (uint8)(low))
#define __SPAIR32__(high, low) (((int32)(high) << 16) | (uint16)(low))
#define __SPAIR64__(high, low) (((int64)(high) << 32) | (uint32)(low))
#define __SPAIR128__(high, low) (((int128)(high) << 64) | (uint64)(low))
#define __PAIR16__(high, low) (((uint16)(high) << 8) | (uint8)(low))
#define __PAIR32__(high, low) (((uint32)(high) << 16) | (uint16)(low))
#define __PAIR64__(high, low) (((uint64)(high) << 32) | (uint32)(low))
#define __PAIR128__(high, low) (((uint128)(high) << 64) | (uint64)(low))

// Raw Memory Access
// https://hex-rays.com/blog/igors-tip-of-the-week-108-raw-memory-accesses-in-pseudocode/
#define MEMORY ((_BYTE *)NULL)

//////////
// qmemcpy
//////////
inline void *qmemcpy(void *dest, const void *src, size_t count)
{
  char *dest_ = (char *)dest;
  const char *src_ = (const char *)src;

// No funny business. One byte at a time, low to high.
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
// If these are not implemented by the compiler define them
#if __is_identifier(__ptr32)
  #define __ptr32
#endif
#if __is_identifier(__ptr64)
  #define __ptr64
#endif
#if __is_identifier(__sptr)
  #define __sptr
#endif
#if __is_identifier(__uptr)
  #define __uptr
#endif

// Struct Attributes
#define __cppobj

///////////////////
// Shifted Pointers
///////////////////
// https://hex-rays.com/products/ida/support/idadoc/1695.shtml
// If these are not implemented by the compiler define them
#if __is_identifier(__shifted)
  #define __shifted(parent, delta)
#endif
#if __is_identifier(__parentof)
  #define __parentof(value)
#endif
#if __is_identifier(__deltaof)
  #define __deltaof(value)
#endif
// If this macro is not predefined by the compiler define it
#ifndef ADJ
  #define ADJ(ptr) ((__parentof(value) *)((char *)(value) - __deltaof(value)))
#endif

//////////////////////
// Calling Conventions
//////////////////////
// https://www.hex-rays.com/products/ida/support/idadoc/1361.shtml
// If these are not implemented by the compiler define them
#if __is_identifier(__usercall)
  #define __usercall
#endif
#if __is_identifier(__userpurge)
  #define __userpurge
#endif
#if __is_identifier(__spoils)
  #define __spoils
#endif

///////////////
// Control Flow
///////////////

// JUMPOUT
typedef __noreturn void (*__jumpout_t)(void);
#define JUMPOUT(value) (((__jumpout_t)(value))())

#endif // HEXRAYS_DEFS_H
