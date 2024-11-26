// ソースコードジェネレータ
// 16進変換, String 関数 ヘッダ
// Copyright (C) 1989,1990 K.Abe
// All rights reserved.
// Copyright (C) 2024 TcbnErik

#ifndef HEX_H
#define HEX_H

#include "global.h"

extern char* itod2(char*, ULONG);
extern char* itox(char*, ULONG, int);
extern char* itox8_without_0supress(char*, ULONG);
extern char* itox6_without_0supress(char*, ULONG);
extern char* itox4_without_0supress(char*, ULONG);

extern char HexCharTable[16];

static inline char* itox2(char* buf, ULONG n) {
  if (!Dis.Z || n >= 0x10) {
    // 10 の位
    *buf++ = HexCharTable[(n >> 4) & 0xf];
  }
  // 1 の位
  *buf++ = HexCharTable[n & 0xf];
  *buf = '\0';
  return buf;
}

static inline char* itox2_without_0supress(char* buf, ULONG n) {
  *buf++ = HexCharTable[(n >> 4) & 0xf];
  *buf++ = HexCharTable[n & 0xf];
  *buf = '\0';
  return buf;
}

static inline char* itoxd(char* buf, ULONG n, int width) {
  if (!Dis.suppressDollar || n >= 10) *buf++ = '$';
  return (width == 2) ? itox2(buf, n) : itox(buf, n, width);
}

static inline char* itox6(char* buf, ULONG n) {
  return itox(buf, n, (n >= 0x1000000) ? 8 : 6);
}

static inline char* itox6d(char* buf, ULONG n) {
  if (!Dis.suppressDollar || n >= 10) *buf++ = '$';
  return itox6(buf, n);
}

static inline char* itox8(char* buf, ULONG n) { return itox(buf, n, 8); }
static inline char* itox4(char* buf, ULONG n) { return itox(buf, n, 4); }

static inline char* itox8d(char* buf, ULONG n) { return itoxd(buf, n, 8); }
static inline char* itox4d(char* buf, ULONG n) { return itoxd(buf, n, 4); }
static inline char* itox2d(char* buf, ULONG n) { return itoxd(buf, n, 2); }

#endif
