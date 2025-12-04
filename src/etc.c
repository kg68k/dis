// ソースコードジェネレータ
// 雑用ルーチン
// Copyright (C) 1989,1990 K.Abe
// All rights reserved.
// Copyright (C) 2024 TcbnErik

#include "etc.h"

#include <ctype.h> /* isxdigit isdigit */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h> /* exit(), EXIT_FAILURE */

#include "estruct.h"
#include "global.h"
#include "label.h"
#include "offset.h"
#include "symbol.h"

#define ARRAYBUFFER_INITIAL_HALF 32
#define ARRAYBUFFER_INCREASE_RATIO 2

// ArrayBufferの内部バッファを解放する
//    ArrayBufferは引き続き使用可能
void freeArrayBuffer(ArrayBuffer* ab) { initArrayBuffer(ab, ab->bytes); }

// ArrayBufferを初期化する
void initArrayBuffer(ArrayBuffer* ab, size_t bytes) {
  ab->buffer = ab->write = NULL;
  ab->capacity = ab->count = 0;
  ab->bytes = bytes;
  ab->isFrozen = FALSE;
}

// ArrayBufferの内部バッファのサイズを変更する
static void reallocArrayBuffer(ArrayBuffer* ab, size_t newCapacity) {
  const size_t writtenSize = (char*)ab->write - (char*)ab->buffer;
  ab->buffer = Realloc(ab->buffer, newCapacity * ab->bytes);
  ab->write = (char*)ab->buffer + writtenSize;
}

// ArrayBufferの内部バッファを確保または拡大する
void secureArrayBufferCapacity(ArrayBuffer* ab) {
  size_t cap = ab->capacity;

  if (ab->isFrozen) internalError(__FILE__, __LINE__, "ArrayBuffer is frozen.");

  if (cap == 0) cap = ARRAYBUFFER_INITIAL_HALF;
  ab->capacity = cap = (cap * ARRAYBUFFER_INCREASE_RATIO);
  reallocArrayBuffer(ab, cap);
}

// ArrayBufferの内部バッファと要素数を取得する
//   内部バッファが確保されていない場合はNULLを返す。
//
// 呼び出し後は書き込み禁止になり、freeArrayBuffer()で解除される。
void* getArrayBufferRawPointer(ArrayBuffer* ab, size_t* countPtr) {
  freezeArrayBuffer(ab);

  *countPtr = ab->count;
  return ab->buffer;
}

// ArrayBufferを書き込み禁止にする
//   内部バッファは使用しているサイズに切り詰められる。
void freezeArrayBuffer(ArrayBuffer* ab) {
  if (!ab->isFrozen) return;

  ab->isFrozen = TRUE;
  ab->capacity = ab->count;
  reallocArrayBuffer(ab, ab->capacity);
  ab->write = NULL;
}

extern ULONG atox(const char* p) {
  ULONG val = 0;

  if (p[0] == '$') p++;
  /* 0x... なら頭の 0x を取り除く */
  else if (p[0] == '0' && p[1] == 'x' && p[2])
    p += 2;

  while (isxdigit(*p)) {
    char c = *p++;
    val <<= 4;
    val += isdigit(c) ? (c - '0') : (tolower(c) - 'a' + 10);
  }
  return val;
}

// safe strdup
char* Strdup(const char* s) {
  char* buf = Malloc(strlen(s) + 1);
  return strcpy(buf, s);
}

// safe strdup + strcat
char* StrdupCat(const char* s1, const char* s2) {
  char* buf = Malloc(strlen(s1) + strlen(s2) + 1);
  strcpy(strcpy2(buf, s1), s2);
  return buf;
}

// safe malloc
void* Malloc(size_t size) {
  void* p = malloc((size == 0) ? 1 : size);

  if (p == NULL) notEnoughMemory();
  return p;
}

// safe realloc
void* Realloc(void* ptr, size_t size) {
  void* rc = realloc(ptr, size);

  if (rc == NULL) notEnoughMemory();
  return rc;
}

// 内部エラーを表示してエラー終了する
void internalError(const char* file, int line, const char* s) {
  err("内部エラー: %s:%d: %s\n", file, line, s);
}

// メモリ不足によるエラー終了
void notEnoughMemory(void) { err("\nヒープメモリが不足しています。\n"); }

// フォーマット文字列を表示してエラー終了する
void err(const char* fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);

  errorExit();
}

// エラー終了する
void errorExit(void) { exit(EXIT_FAILURE); }

#ifndef __LIBC__
extern int eprintf(const char* fmt, ...) {
  int cnt;
  va_list ap;

  va_start(ap, fmt);
  cnt = vfprintf(stderr, fmt, ap);
  va_end(ap);
  return cnt;
}
#endif

int eputc(int c) { return fputc(c, stderr); }

// 指定したメモリ内の小文字を大文字化する
void toUpperMemory(size_t len, void* ptr) {
  char* p = ptr;
  char* end = p + len;

  while (p < end) {
    int c = toupper(*p);
    *p++ = c;
  }
}

/* EOF */
