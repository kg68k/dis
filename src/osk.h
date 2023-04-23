// ソースコードジェネレータ
// OS-9/X680x0(OSK) ヘッダ
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

#ifndef OSK_H
#define OSK_H

#include <stdio.h>

#include "disasm.h"
#include "estruct.h"

// 実行ファイルのヘッダID
enum {
  OSKHEAD_ID_VALUE = 0x4afc,
};

#ifdef OSKDIS

// Dis.oskModType
enum {
  OSKTYPE_PROGRAM = 0x01,
  OSKTYPE_SUBROUTINE = 0x02,
  OSKTYPE_TRAP = 0x0b,
  OSKTYPE_SYSTEM = 0x0c,
  OSKTYPE_FILEMAN = 0x0d,
  OSKTYPE_DRIVER = 0x0e,
  // OSKTYPE_DEVDESC = 0x0f,
};

extern void loadHeaderOsk(FILE* fp, ULONG fileSize);

#endif
#endif

// EOF
