// ソースコードジェネレータ
// 浮動小数点実数値文字列変換モジュールヘッダ
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

#ifndef FPCONV_H
#define FPCONV_H

#include "estruct.h"

typedef union {
  uint8_t uc[4];
  ULONG ul;
} SingleReal;

typedef union {
  uint8_t uc[8];
  ULONG ul[2];
} DoubleReal;

typedef union {
  uint8_t uc[12];
  ULONG ul[4];
} ExtendedReal;

typedef union {
  uint8_t uc[12];
  ULONG ul[3];
} PackedDecimal;

extern char* fpconv_s(char* buf, codeptr valp);
extern char* fpconv_d(char* buf, codeptr valp);
extern char* fpconv_x(char* buf, codeptr valp);
extern char* fpconv_p(char* buf, codeptr valp);

#endif  // FPCONV_H

// EOF
