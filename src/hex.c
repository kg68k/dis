// ソースコードジェネレータ
// 16進変換 , String 関数
// Copyright (C) 1989,1990 K.Abe, 1994 R.ShimiZu
// All rights reserved.
// Copyright (C) 1997-2023 TcbnErik

#include "hex.h"

#include <stdio.h>

#include "estruct.h"
#include "global.h"

char Hex[16] = "0123456789abcdef";

static char* ulongToDecimal(char* buf, ULONG n);

char* itod2(char* buf, ULONG n) {
  char x = n;

  // 19以下の数を高速に変換する細工(trap #15用)
  if (n >= 10) {
    if (n >= 20) return ulongToDecimal(buf, n);
    *buf++ = '1';
    x -= 10;
  }
  *buf++ = x + '0';
  *buf = '\0';
  return buf;
}

static char* ulongToDecimal(char* buf, ULONG n) {
  return buf + sprintf(buf, PRI_ULONG, n);
}

extern char* itox(char* buf, ULONG n, int width) {
  if (Dis.Z) {
    if (n == 0)
      *buf++ = '0';
    else {
      char tmp[8];
      char* p = tmp + sizeof tmp;
      int i = 0;

      do {
        // 下位桁からテンポラリに変換
        i += 1;
        *--p = Hex[n & 0xf];
        n >>= 4;
      } while (n);
      do {
        // 上位桁からバッファに転送
        *buf++ = *p++;
      } while (--i > 0);
    }
  }

  else {
    char* p = buf += width;
    int i;

    for (i = width; --i >= 0;) {
      /* 下位桁から変換 */
      *--p = Hex[n & 0xf];
      n >>= 4;
    }
  }

  *buf = '\0';
  return buf;
}

static char* itox_without_0supress(char* buf, ULONG n, int width) {
  char* p = buf += width;
  int i;

  for (i = width; --i >= 0;) {
    *--p = Hex[n & 0xf];
    n >>= 4;
  }
  *buf = '\0';
  return buf;
}

char* itox8_without_0supress(char* buf, ULONG n) {
  return itox_without_0supress(buf, n, 8);
}

char* itox6_without_0supress(char* buf, ULONG n) {
  return itox_without_0supress(buf, n, 6);
}

char* itox4_without_0supress(char* buf, ULONG n) {
  return itox_without_0supress(buf, n, 4);
}

// EOF
