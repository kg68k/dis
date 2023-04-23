// ソースコードジェネレータ
// 実効アドレス文字列
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

#include "eastr.h"

#include "etc.h"
#include "global.h"

EAStringTable EAString = {
    {"d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7"},
    {"a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7"},
    "sp",
    "bwlqssdxp",
    'z',
    "pc",
    "ccr",
    "sr",
    {"fp0", "fp1", "fp2", "fp3", "fp4", "fp5", "fp6", "fp7"},
    {"fpcr", "fpsr", "fpiar"},
    {"sfc", "dfc", "cacr", "tc", "itt0", "itt1", "dtt0", "dtt1", "buscr"},
    {"usp", "vbr", "caar", "msp", "isp", "mmusr", "urp", "srp", "pcr"},
    {"nc", "dc", "ic", "bc"},
    "psr",
    "pcsr",
    "mmusr",
    {"bad0", "bad1", "bad2", "bad3", "bad4", "bad5", "bad6", "bad7"},
    {"bac0", "bac1", "bac2", "bac3", "bac4", "bac5", "bac6", "bac7"},
    {"tt0", "tt1", "tc", "drp", "srp", "crp", "cal", "val", "scc", "ac"}  //
};

void initEAString(void) {
  EAStringTable* eas = &EAString;

  if (Dis.U) toUpperMemory(sizeof(EAStringTable), eas);

  if (Dis.a7ToSp) {
    char* a7 = eas->an[7];
    char* sp = eas->sp;
    *a7++ = *sp++;
    *a7++ = *sp++;
  }
}

// EOF
