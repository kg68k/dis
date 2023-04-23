// ソースコードジェネレータ
// Human68k ヘッダ
// Copyright (C) 2023 TcbnErik

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

#ifndef HUMAN_H
#define HUMAN_H

#include <stdio.h>

#include "estruct.h"

// 実行ファイルのヘッダID
enum {
  XHEAD_ID_VALUE = (('H' << 8) + 'U'),
  ZHEAD_ID_VALUE = 0x601a,
};

extern void loadHeaderHuman(FILE* fp, ULONG fileSize);

#endif

// EOF
