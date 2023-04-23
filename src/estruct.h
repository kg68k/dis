// ソースコードジェネレータ
// 構造体定義ファイル
// Copyright (C) 1989,1990 K.Abe, 1994 R.ShimiZu
// All rights reserved.
// Copyright (C) 1997-2023 TcbnErik

#ifndef ESTRUCT_H
#define ESTRUCT_H

#include <stddef.h>  // size_t

#ifdef _MSC_VER
#define NORETURN __declspec(noreturn)
#else
#define NORETURN  // [[noreturn]]
#endif

#ifdef __GNUC__
#define GCC_NORETURN __attribute__((noreturn))
#define GCC_PRINTF(a, b) __attribute__((format(printf, a, b)))
#else
#define GCC_NORETURN
#define GCC_PRINTF(a, b)
#endif

#if __GNUC__ >= 3
#define GCC_UNUSED __attribute__((unused))
#else
#define GCC_UNUSED
#endif

#if defined(__GNUC__) && defined(__mc68000__)
#define GCC_ALIGN(n) __attribute__((aligned(n)))
#else
#define GCC_ALIGN(n)
#endif

#ifdef __HUMAN68K__
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned int uintptr_t;

#define PRIx16 "hx"
#define PRIu32 "u"
#define PRIx32 "x"
#else
#include <inttypes.h>
#include <stdint.h>
#endif

// M680x0 data types
typedef int8_t BYTE;
typedef int16_t WORD;
typedef int32_t LONG;
typedef uint8_t UBYTE;
typedef uint16_t UWORD;
typedef uint32_t ULONG;
typedef uint32_t address;

#define PRI_OPCODE "$%04" PRIx16  // UWORD -> "$4e71"
#define PRI_ULONG "%" PRIu32      // ULONG -> "123"
#define PRI_ULHEX "$%08" PRIx32   // ULONG -> "$0089abcd"
#define PRI_ADRS "$%08" PRIx32    // address -> "$0089abcd"

// host data types
typedef uint8_t mputypes;
typedef uint8_t fputypes;
typedef uint8_t mmutypes;

typedef uint8_t* codeptr;

#if defined(TRUE) || defined(FALSE)
#error TRUE and FALSE are already defined somewhere.
#endif
typedef enum { FALSE, TRUE } boolean;

typedef enum {
  BYTESIZE,   // バイト
  WORDSIZE,   // ワード
  LONGSIZE,   // ロングワード
  QUADSIZE,   // クワッドワード
  SHORTSIZE,  // ショート(相対分岐命令)

  SINGLESIZE,  // 32bit実数型
  DOUBLESIZE,  // 64bit実数型
  EXTENDSIZE,  // 96bit実数型
  PACKEDSIZE,  // 96bit BCD実数型
  NOTHING,     // 無し

  STRING,    // 文字列
  RELTABLE,  // リロケータブルオフセットテーブル
  RELLONGTABLE,  // ロングワードなリロケータブルオフセットテーブル
  ZTABLE,  // 絶対番地テーブル

  WTABLE,  // OSK ワードテーブル

  EVENID,  // テーブルの識別子
  CRID,
  BYTEID,
  ASCIIID,
  ASCIIZID,
  LASCIIID,
  BREAKID,

  UNKNOWN = 128  // 不明
} opesize;

typedef struct {
  void* buffer;
  void* write;
  size_t capacity;
  size_t count;
  size_t bytes;
  boolean isFrozen;
} ArrayBuffer;

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

static inline int isEven(address adrs) { return (adrs & 1) ^ 1; }
static inline int isOdd(address adrs) { return (adrs & 1); }

#endif  // ESTRUCT_H

// EOF
