// ソースコードジェネレータ
// 雑用ルーチンヘッダファイル
// Copyright (C) 1989,1990 K.Abe
// All rights reserved.
// Copyright (C) 1997-2023 TcbnErik

#ifndef ETC_H
#define ETC_H

#include <stdio.h>   // for __byte_swap_*
#include <stdlib.h>  // free()
#include <string.h>

#include "estruct.h"
#include "global.h"

#if defined(__mc68000__) && !defined(__BIG_ENDIAN__)
#define __BIG_ENDIAN__ 1
#endif

#ifndef _countof  // stdlib.h
#define _countof(array) (sizeof(array) / sizeof(array[0]))
#endif

#if defined(_MSC_VER)
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif

// 可変長配列
extern void freeArrayBuffer(ArrayBuffer* ab);
extern void initArrayBuffer(ArrayBuffer* ab, size_t bytes);
extern void secureArrayBufferCapacity(ArrayBuffer* ab);
extern void* getArrayBufferRawPointer(ArrayBuffer* ab, size_t* countPtr);
extern void freezeArrayBuffer(ArrayBuffer* ab);

// ArrayBufferに新しい要素を書き込むためのバッファ上のアドレスを返す
static inline void* getArrayBufferNewPlace(ArrayBuffer* ab) {
  void* p;

  if (ab->capacity == ab->count || ab->isFrozen) secureArrayBufferCapacity(ab);

  p = ab->write;
  ab->count += 1;
  ab->write = (char*)p + ab->bytes;
  return p;
}

// エラー終了
extern NORETURN void internalError(const char* file, int line,
                                   const char* s) GCC_NORETURN;
extern NORETURN void notEnoughMemory(void) GCC_NORETURN;
extern NORETURN void err(const char*, ...) GCC_NORETURN GCC_PRINTF(1, 2);
extern NORETURN void errorExit(void) GCC_NORETURN;

// メモリ確保
extern void* Malloc(size_t size);
extern void* Realloc(void* ptr, size_t size);
static inline void Mfree(void* ptr) { free(ptr); }

// 標準エラー出力
#ifndef __LIBC__
extern int eprintf(const char*, ...) GCC_PRINTF(1, 2);
#endif
extern int eputc(int);

static inline void charout(int a) {
  if (!Dis.q) eputc(a);
}

// 文字列操作
extern ULONG atox(const char*);

// 文字列末尾の \n を \0 で埋める
static inline void removeTailLf(char* s) {
  if (*s) {
    s += strlen(s) - 1;
    if (*s == '\n') *s = '\0';
  }
}

// 書き込んだ文字列の末尾を返すstrcpy()
static inline char* strcpy2(char* dst, const char* src) {
  while ((*dst++ = *src++) != 0)
    ;
  return --dst;
}

// 文字コード(X680x0 Shift_JIS)
#ifdef HAVE_JCTYPE_H
#include <jctype.h>
#else

#define iskanji(c) \
  ((0x81 <= (c) && (c) <= 0x9f) || (0xe0 <= (c) && (c) <= 0xfc))
#define iskanji2(c) ((0x40 <= (c) && (c) <= 0xfc) && (c) != 0x7f)
#define isprkana(c) \
  ((0x20 <= (c) && (c) <= 0x7e) || (0xa1 <= (c) && (c) <= 0xdf))

#endif  // !HAVE_JCTYPE_H

// メモリ操作
extern void toUpperMemory(size_t len, void* ptr);

static inline UBYTE peekb(codeptr ptr) {
  uint8_t* p = ptr;
  return p[0];
}

static inline UWORD peekw(codeptr ptr) {
#ifdef __BIG_ENDIAN__
  return *(UWORD*)ptr;
#elif defined(__GNUC__)
  return __builtin_bswap16(*(uint16_t*)ptr);
#elif defined(__byte_swap_word)
  return __byte_swap_word(*(unsigned short*)ptr);
#else
  uint8_t* p = ptr;
  return (p[0] << 8) + p[1];
#endif
}

static inline ULONG peekl(codeptr ptr) {
#ifdef __BIG_ENDIAN__
  return *(ULONG*)ptr;
#elif defined(__GNUC__)
  return __builtin_bswap32(*(uint32_t*)ptr);
#elif defined(__byte_swap_long)
  return __byte_swap_long(*(unsigned long*)ptr);
#else
  uint8_t* p = ptr;
  return (p[0] << 24) + (p[1] << 16) + (p[2] << 8) + p[3];
#endif
}

static inline LONG extbl(UBYTE b) { return (LONG)(BYTE)b; }
static inline LONG extw(UBYTE b) { return (LONG)(UWORD)(BYTE)b; }
static inline LONG extl(UWORD w) { return (LONG)(WORD)w; }

#endif  // ETC_H
