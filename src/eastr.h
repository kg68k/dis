// ソースコードジェネレータ
// 実効アドレス文字列 ヘッダ
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

#ifndef EASTR_H
#define EASTR_H

#include <string.h>

#include "estruct.h"
#include "etc.h"

typedef struct {
  char dn[8][2];
  char an[8][2];
  char sp[2];
  char size[PACKEDSIZE + 1];
  char suppress_z;
  char pc[2];
  char ccr[4];
  char sr[2];
  char fpn[8][4];
  char fpcrsr[3][8];  // null-terminated
  char cr00x[9][8];   // null-terminated
  char cr80x[9][8];   // null-terminated
  char cache[4][2];
  char psr[4];
  char pcsr[4];
  char mmusr[6];
  char badn[8][4];
  char bacn[8][4];
  char mmureg[10][4];  // null-terminated
} EAStringTable;

extern EAStringTable EAString;

// 制御レジスタ(movec)
enum {
  CTRLREG_SFC = 0x000,
  CTRLREG_DFC = 0x001,
  CTRLREG_CACR = 0x002,
  CTRLREG_TC = 0x003,
  CTRLREG_ITT0 = 0x004,
  CTRLREG_ITT1 = 0x005,
  CTRLREG_DTT0 = 0x006,
  CTRLREG_DTT1 = 0x007,
  CTRLREG_BUSCR = 0x008,
  CTRLREG_USP = 0x800,
  CTRLREG_VBR = 0x801,
  CTRLREG_CAAR = 0x802,
  CTRLREG_MSP = 0x803,
  CTRLREG_ISP = 0x804,
  CTRLREG_MMUSR = 0x805,
  CTRLREG_URP = 0x806,
  CTRLREG_SRP = 0x807,
  CTRLREG_PCR = 0x808,
};

// MMUレジスタ(MC68851,MC68030 pmove)
//   EAString.mmureg[] 参照用の通し番号(オペコードとは異なる)
typedef enum {
  MMUREG_TT0 = 0,
  MMUREG_TT1,
  MMUREG_TC,
  MMUREG_DRP,
  MMUREG_SRP,
  MMUREG_CRP,
  MMUREG_CAL,
  MMUREG_VAL,
  MMUREG_SCC,
  MMUREG_AC,
} MmuReg;

extern void initEAString(void);

static inline char* write_size(char* p, opesize size) {
  if (size < sizeof(EAString.size)) {
    *p++ = '.';
    *p++ = EAString.size[size];
  }
  return p;
}

static inline char* write_dn(char* p, int regno) {
  const char* s = EAString.dn[regno];
  *p++ = *s++;  // 'd'
  *p++ = *s++;  // '0'
  return p;
}

static inline char* write_an(char* p, int regno) {
  const char* s = EAString.an[regno];
  *p++ = *s++;  // 'a'
  *p++ = *s++;  // '0'
  return p;
}

static inline char* write_fpn(char* p, int regno) {
  const char* s = EAString.fpn[regno];
  *p++ = *s++;  // 'f'
  *p++ = *s++;  // 'p'
  *p++ = *s++;  // '0'
  return p;
}

static inline char* write_rn(char* p, int regno) {
  return (regno & 8) ? write_an(p, regno & 7) : write_dn(p, regno);
}

static inline char* write_pc(char* p) {
  const char* s = EAString.pc;
  *p++ = *s++;
  *p++ = *s++;
  return p;
}

static inline char* write_suppress_z(char* p) {
  *p++ = EAString.suppress_z;
  return p;
}

static inline char* write_zan(char* p, int regno) {
  *p++ = EAString.suppress_z;
  return write_an(p, regno);
}

static inline char* write_zpc(char* p) {
  *p++ = EAString.suppress_z;
  return write_pc(p);
}

static inline void write0_ccr(char* p) {
  const char* s = EAString.ccr;
  *p++ = *s++;
  *p++ = *s++;
  *p++ = *s++;
  *p = '\0';
}

static inline void write0_sr(char* p) {
  const char* s = EAString.sr;
  *p++ = *s++;
  *p++ = *s++;
  *p = '\0';
}

static inline char* write0_fpcrsr(char* p, int regno) {
  char* s = EAString.fpcrsr[regno];

  while ((*p++ = *s++)) {
    // strcpy
  }
  return p - 1;
}

static inline void write0_ctrlreg(char* p, int regno) {
  int r = regno & 15;
  const char* s =
      (r == regno)
          ? ((r < (int)_countof(EAString.cr00x)) ? EAString.cr00x[r] : "")
          : ((r < (int)_countof(EAString.cr80x)) ? EAString.cr80x[r] : "");

  strcpy(p, s);
}

static inline void write0_cachekind(char* p, int caches) {
  const char* s = EAString.cache[caches];
  *p++ = *s++;
  *p++ = *s++;
  *p = '\0';
}

static inline void write0_psr(char* p) {
  const char* s = EAString.psr;
  *p++ = *s++;
  *p++ = *s++;
  *p++ = *s++;
  *p = '\0';
}

static inline void write0_pcsr(char* p) {
  const char* s = EAString.pcsr;
  *p++ = *s++;
  *p++ = *s++;
  *p++ = *s++;
  *p++ = *s++;
  *p = '\0';
}

static inline void write0_mmusr(char* p) {
  const char* s = EAString.mmusr;
  *p++ = *s++;
  *p++ = *s++;
  *p++ = *s++;
  *p++ = *s++;
  *p++ = *s++;
  *p = '\0';
}

static inline void write0_badn(char* p, int regno) {
  char* s = EAString.badn[regno];
  *p++ = *s++;
  *p++ = *s++;
  *p++ = *s++;
  *p++ = *s++;
  *p = '\0';
}

static inline void write0_bacn(char* p, int regno) {
  char* s = EAString.bacn[regno];
  *p++ = *s++;
  *p++ = *s++;
  *p++ = *s++;
  *p++ = *s++;
  *p = '\0';
}

static inline void write0_mmureg(char* p, MmuReg regno) {
  const char* s = EAString.mmureg[regno];
  strcpy(p, s);
}

#endif

// EOF
