// ソースコードジェネレータ
// 浮動小数点実数値文字列変換モジュール
// Copyright (C) 1997-2023 TcbnErik

// This file is part of dis (source code generator).
//
// This program is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program. If not, see <https://www.gnu.org/licenses/>.

#include "fpconv.h"

#include <float.h>
#include <stdio.h>  // sprintf()

#include "estruct.h"
#include "etc.h"  // strcpy2()
#include "hex.h"  // itox8_without_0supress()

/* External functions */
#ifdef HAVE_STRTOX
extern long double strtox(const char* ptr);
#endif

#if !defined(NO_PRINTF_LDBL) && ((LDBL_MANT_DIG != 64) || (LDBL_DIG != 18))
#define NO_PRINTF_LDBL
#endif

static inline ULONG peeklv(ULONG ul) { return peekl((codeptr)&ul); }

static inline int isodd(codeptr ptr) { return (uintptr_t)ptr & 1; }

static SingleReal* alignSingleReal(codeptr valp, SingleReal* buf) {
  return isodd(valp) ? memcpy(buf, valp, sizeof(*buf)) : (SingleReal*)valp;
}

static float peekFloat(SingleReal* sr) {
#ifdef __BIG_ENDIAN__
  return *(float*)sr;
#else
  SingleReal le = {.ul = peeklv(sr->ul)};
  return *(float*)&le;
#endif
}

// Single-Precision Real 文字列化
char* fpconv_s(char* buf, codeptr valp) {
  SingleReal srBuf;
  SingleReal* sr = alignSingleReal(valp, &srBuf);

  ULONG ul = peeklv(sr->ul);
  UWORD e = (ul >> 16) & 0x7f80;
  ULONG m = ul & 0x007fffff;

  // ±0.0を先に処理
  if (!Dis.inreal && e == 0 && m == 0) {
    return strcpy2(buf, ((LONG)ul < 0) ? "0f-0" : "0f0");
  }

  if (Dis.inreal || e == 0 || e == 0x7f80) {  // 非正規化数, 無限大、非数
    // 特殊な値なら内部表現で出力
    *buf++ = '!';
    return itox8_without_0supress(buf, ul);
  }

  return buf + sprintf(buf, "0f%.30g", peekFloat(sr));
}

static DoubleReal* alignDoubleReal(codeptr valp, DoubleReal* buf) {
  return isodd(valp) ? memcpy(buf, valp, sizeof(*buf)) : (DoubleReal*)valp;
}

static double peekDouble(DoubleReal* dr) {
#ifdef __BIG_ENDIAN__
  return *(double*)dr;
#else
  DoubleReal le = {.ul = {peeklv(dr->ul[1]), peeklv(dr->ul[0])}};
  return *(double*)&le;
#endif
}

// Double-Precision Real 文字列化
char* fpconv_d(char* buf, codeptr valp) {
  DoubleReal drBuf;
  DoubleReal* dr = alignDoubleReal(valp, &drBuf);

  ULONG ul0 = peeklv(dr->ul[0]);
  UWORD e = (ul0 >> 16) & 0x7ff0;
  ULONG mh = ul0 & 0x000fffff;

  // ±0.0を先に処理
  if (!Dis.inreal && e == 0 && mh == 0 && dr->ul[1] == 0) {
    return strcpy2(buf, ((LONG)ul0 < 0) ? "0f-0" : "0f0");
  }

  if (Dis.inreal || e == 0 || e == 0x7ff0) {  // 非正規化数, 無限大、非数
    // 特殊な値なら内部表現で出力
    *buf++ = '!';
    buf = itox8_without_0supress(buf, ul0);
    *buf++ = '_';
    return itox8_without_0supress(buf, peeklv(dr->ul[1]));
  }

  return buf + sprintf(buf, "0f%.30g", peekDouble(dr));
}

static ExtendedReal* alignExtendedReal(codeptr valp, ExtendedReal* buf) {
  return isodd(valp) ? memcpy(buf, valp, sizeof(*buf)) : (ExtendedReal*)valp;
}

static long double peekExtended(ExtendedReal* xr) {
#ifdef __BIG_ENDIAN__
  return *(long double*)xr;
#else
  ExtendedReal le = {
      .ul = {peeklv(xr->ul[2]), peeklv(xr->ul[1]), peeklv(xr->ul[0]) >> 16, 0}};
  return *(long double*)&le;
#endif
}

static boolean is_normalized_x(ExtendedReal* xr) {
  ULONG ul0 = peeklv(xr->ul[0]);
  UWORD e = (ul0 >> 16) & 0x7fff;

  if (e == 0 || e == 0x7fff) return FALSE;  // 非正規化数, 無限大、非数
  if ((ul0 & 0x0000ffff) != 0) return FALSE;  // 未使用ビットがセット
  if ((int8_t)xr->uc[4] >= 0) return FALSE;   // 整数ビットが0

#ifndef HAVE_STRTOX
  if (e <= 64) return FALSE;  // ライブラリに依存
#endif

  return TRUE;
}

static char* fpconv_x_inreal(char* buf, ExtendedReal* xr) {
  *buf++ = '!';
  buf = itox8_without_0supress(buf, peeklv(xr->ul[0]));
  *buf++ = '_';
  buf = itox8_without_0supress(buf, peeklv(xr->ul[1]));
  *buf++ = '_';
  return itox8_without_0supress(buf, peeklv(xr->ul[2]));
}

static inline char* fpconv_x_printf(char* buf, ExtendedReal* xr) {
  char* p = buf + sprintf(buf, "0f%.30Lg", peekExtended(xr));

#ifdef HAVE_STRTOX
  ULONG ul0 = peeklv(xr->ul[0]);

  //  指数が正なら多分正しく変換できているはず
  if (((ul0 >> 16) & 0x7fff) >= 0x3fff) return p;

  // 正しく変換できなかったら、内部表現で出力しなおす
  if (strtox(buf + 2) != peekExtended(xr)) return fpconv_x_inreal(buf, xr);
#endif

  return p;
}

// Extended-Precision Real 文字列化
char* fpconv_x(char* buf, codeptr valp) {
  ExtendedReal xrBuf;
  ExtendedReal* xr = alignExtendedReal(valp, &xrBuf);

  ULONG ul0 = peeklv(xr->ul[0]);

  // ±0.0を先に処理
  if (!Dis.inreal && (ul0 << 1 | xr->ul[1] | xr->ul[2]) == 0) {
    return strcpy2(buf, ((LONG)ul0 < 0) ? "0f-0" : "0f0");
  }

  if (Dis.inreal || !is_normalized_x(xr)) {
    return fpconv_x_inreal(buf, xr);  // 特殊な値なら内部表現で出力
  }

#ifdef NO_PRINTF_LDBL
  return fpconv_x_inreal(buf, xr);  // 常に内部表現で出力
#else
  return fpconv_x_printf(buf, xr);
#endif
}

static boolean is_normalized_p(PackedDecimal* pd) {
  ULONG ul0 = peeklv(pd->ul[0]);

  if ((ul0 << 1 | pd->ul[1] | pd->ul[2]) == 0) return TRUE;  // ±0.0

  if (((ul0 & 0x7fff0000) == 0x7fff0000)  // 非数,無限大
      || ((ul0 & 0x3000fff0) != 0)        // 未使用ビットがセット
#if 0
      || ((ul0 & 0x4fff0000) == 0x4fff0000)  // 非正規化数
#endif
      || ((ul0 & 0x0000000f) > 9))  // BCD(整数桁)の値が異常
    return FALSE;

  {
    int i;
    uint8_t* ptr = (uint8_t*)&pd->uc[4];
    for (i = 0; i < 8; i++) {
      uint8_t c = *ptr++;
      if ((c > 0x99) || ((c & 0x0f) > 0x09))
        return FALSE;  // BCD(小数桁)の値が異常
    }
  }

  return TRUE;
}

static PackedDecimal* alignPackedDecimal(codeptr valp, PackedDecimal* buf) {
  return isodd(valp) ? memcpy(buf, valp, sizeof(*buf)) : (PackedDecimal*)valp;
}

// Packed Decimal Real 文字列化
char* fpconv_p(char* buf, codeptr valp) {
  PackedDecimal pdBuf;
  PackedDecimal* pd = alignPackedDecimal(valp, &pdBuf);

  if (Dis.inreal || !is_normalized_p(pd)) {
    // 特殊な値なら内部表現で出力
    *buf++ = '!';
    buf = itox8_without_0supress(buf, peeklv(pd->ul[0]));
    *buf++ = '_';
    buf = itox8_without_0supress(buf, peeklv(pd->ul[1]));
    *buf++ = '_';
    return itox8_without_0supress(buf, peeklv(pd->ul[2]));
  }

  *buf++ = '0';
  *buf++ = 'f';
  if ((int8_t)pd->uc[0] < 0) *buf++ = '-';

  // 仮数の整数部
  *buf++ = '0' + (pd->uc[3] & 0x0f);
  *buf++ = '.';

  // 仮数の小数部
  {
    int i;
    uint8_t* ptr = &pd->uc[4];

    for (i = 0; i < 8; i++) {
      uint8_t c = *ptr++;
      *buf++ = '0' + (c >> 4);
      *buf++ = '0' + (c & 0x0f);
    }
  }

  // 末尾の'0'を削除
  while (*--buf == '0')
    ;
  if (*buf != '.') buf++;

  // 指数
  {
    int expo =
        (pd->uc[0] & 0x0f) * 100 + (pd->uc[1] >> 4) * 10 + (pd->uc[1] & 0x0f);

    if ((expo == 0) && !(pd->uc[0] & 0x40)) {
      *buf = '\0';  // "e+0"は省略する
    } else {
      char* p;

      *buf++ = 'e';
      *buf++ = (pd->uc[0] & 0x40) ? '-' : '+';
      buf += (expo >= 100) + (expo >= 10) + 1;
      *buf = '\0';

      p = buf;
      do {
        *--p = '0' + (expo % 10);
        expo /= 10;
      } while (expo);
    }
  }
  return buf;
}

// EOF
