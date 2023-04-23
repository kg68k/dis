// ソースコードジェネレータ
// MMU命令逆アセンブル
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

#include "mmu.h"

#include "disasm.h"
#include "ea.h"
#include "global.h"
#include "hex.h"
#include "opstr.h"

char* stringifyQuadWord(char* buf, quadword* valp) {
  *buf++ = '!';
  buf = itox8_without_0supress(buf, valp->hi);
  *buf++ = '_';
  return itox8_without_0supress(buf, valp->lo);
}

// 68851/68030 PLOAD
static boolean pload(codeptr ptr, disasm* code, UWORD word2) {
  code->mputypes = M020 | M030;

  OPECODE(pload[(word2 >> 9) & 1]);
  code->bytes = 4;
  if (!setMMUfc(code, &code->op1, word2)) return FALSE;
  return setEA(code, &code->op2, ptr, CTRLCHG);
}

// 68851 PVALID
static boolean pvalid(codeptr ptr, disasm* code, UWORD word2) {
  if (reject_no_pmmu(code)) return FALSE;

  OPECODE(pvalid);
  code->bytes = 4;
  code->size = code->size2 = code->default_size = LONGSIZE;
  if (word2 & 0x0400)
    setMMUregVal(&code->op1);
  else
    setAn(&code->op1, word2);
  return setEA(code, &code->op2, ptr, CTRLCHG);
}

// 68851/68030 PFLUSHA
static boolean pflusha(disasm* code, UWORD word1) {
  if (extract_ea_modreg(word1) != 0 && Dis.undefReg) return FALSE;

  code->mputypes = M020 | M030;

  OPECODE(pflusha);
  code->bytes = 4;

  return TRUE;
}

// 68851/68030 PFLUSH
static boolean pflush(codeptr ptr, disasm* code, UWORD word1, UWORD word2) {
  int mode = (word2 >> 10) & 7;

  if ((mode & 2) == 0 && extract_ea_modreg(word1) != 0 && Dis.undefReg)
    return FALSE;

  if (mode & 1) {
    if (reject_no_pmmu(code)) return FALSE;  // fflushsは68851のみ
    OPECODE(pflushs);
  } else {
    code->mputypes = M020 | M030;
    OPECODE(pflush[PFOPMODE_PFLUSH]);
  }

  code->bytes = 4;
  if (!setMMUfc(code, &code->op1, word2) ||                // fc
      !setMMUfc(code, &code->op2, (word2 >> 5) | 0x10)) {  // #xx
    return FALSE;
  }
  if (mode & 2) return setEA(code, &code->op3, ptr, CTRLCHG);  // <ea>

  return TRUE;
}

static boolean op000cls001(codeptr ptr, disasm* code, UWORD word1) {
  UWORD word2 = peekw(ptr + 2);

  if ((word2 & 0xfde0) == 0x2000) return pload(ptr, code, word2);
  if ((word2 & 0xfbf8) == 0x2800) return pvalid(ptr, code, word2);
  if (word2 == 0x2400) return pflusha(code, word1);
  if ((word2 & 0xf200) == 0x3000) return pflush(ptr, code, word1, word2);

  return FALSE;
}

// 68851/68030 PMOVE
static boolean pmove(codeptr ptr, disasm* code, UWORD word1) {
  UWORD word2 = peekw(ptr + 2);
  int rw_fd = (word2 >> 8) & 3;
  int rw = rw_fd >> 1;
  int fd = rw_fd & 1;

  // pmovefd ea,mmureg は68030専用
  // pmovefd mmureg,ea は存在しない
  if ((word2 & 0x00ff) != 0x0000 || rw_fd == 3 ||
      (rw_fd == 1 && (Dis.mmu & MMU030) == 0)) {
    return FALSE;
  }
  code->mputypes = fd ? M030 : M020 | M030;

  OPECODE2(fd, pmovefd, pmove);
  {
    operand* mmureg = rw ? &code->op1 : &code->op2;
    operand* eamem = rw ? &code->op2 : &code->op1;

    int mode = setMMUreg(code, mmureg, word2, extract_ea_basic(word1));
    if (mode == 0) return FALSE;
    return setEA(code, eamem, ptr, mode);
  }
}

// 68851 PMOVE to/from PSR/PCSR
// 68030 PMOVE to/from MMUSR
static boolean cls011pmove(codeptr ptr, disasm* code) {
  UWORD word2 = peekw(ptr + 2);
  int rw = (word2 >> 9) & 1;

  code->mputypes = M020 | M030;

  OPECODE(pmove);
  {
    operand* mmureg = rw ? &code->op1 : &code->op2;
    operand* eamem = rw ? &code->op2 : &code->op1;

    if (!setPMMUreg(code, mmureg, word2)) return FALSE;
    return setEA(code, eamem, ptr, CTRLCHG);
  }
}

// 68851/68030 PTEST
static boolean ptest(codeptr ptr, disasm* code) {
  UWORD word2 = peekw(ptr + 2);
  int level = (word2 >> 10) & 7;
  int a = (word2 >> 8) & 1;
  int regno = (word2 >> 5) & 7;

  if (level == 0 && (a != 0 || regno != 0)) {
    // level==0のとき、A-REGISTERフィールドは0でないとFライン例外になる
    return FALSE;
  }
  if (a == 0 && regno != 0 && Dis.undefReg) return FALSE;

  code->mputypes = M020 | M030;

  OPECODE(ptest[(word2 >> 9) & 1]);
  code->bytes = 4;
  if (!setMMUfc(code, &code->op1, word2)) return FALSE;
  if (!setEA(code, &code->op2, ptr, CTRLCHG)) return FALSE;
  setImmEmbed(&code->op3, level);
  if (a) setAn(&code->op4, regno);

  return TRUE;
}

// 68851 PFLUSHR
static boolean pflushr(codeptr ptr, disasm* code) {
  UWORD word2 = peekw(ptr + 2);

  if (word2 != 0xa000) return FALSE;
  if (reject_no_pmmu(code)) return FALSE;

  OPECODE(pflushr);
  code->bytes = 4;
  return setEA(code, &code->op1, ptr, MEMORY);
}

static boolean op000(codeptr ptr, disasm* code) {
  UWORD word1 = peekw(ptr);
  int opclass = peekb(ptr + 2) >> 5;

  switch (opclass) {
    default:
      break;

    case 0:
    case 2:
      return pmove(ptr, code, word1);
    case 1:
      return op000cls001(ptr, code, word1);
    case 3:
      return cls011pmove(ptr, code);
    case 4:
      return ptest(ptr, code);
    case 5:
      return pflushr(ptr, code);
  }

  return FALSE;
}

// 68851 PDBcc
static boolean pdbcc(codeptr ptr, disasm* code, UWORD word1) {
  UWORD word2 = peekw(ptr + 2);

  if ((word2 & 0xfff0) != 0) return FALSE;

  OPECODE(pdbcc[word2 & 15]);
  setDn(&code->op1, word1);
  setrelative4(code, &code->op2, extl(peekw(ptr + 4)));
  code->jmp = code->op2.opval;
  code->jmpea = code->op2.ea = PCDISP;
  code->op2.labelchange1 = LABELCHANGE_LABEL_ONLY;
  code->opeType = BCCOP;
  code->bytes = 6;
  code->size = code->size2 = WORDSIZE;

  return TRUE;
}

// 68851 PTRAPcc
static boolean ptrapcc(codeptr ptr, disasm* code, int opmode) {
  UWORD word2 = peekw(ptr + 2);

  if ((word2 & 0xfff0) != 0) return FALSE;

  OPECODE(ptrapcc[word2 & 15]);
  code->bytes = 4;
  if (opmode != CPTRAP_UNSIZED) {
    code->size = (opmode == CPTRAP_WORD) ? WORDSIZE : LONGSIZE;
    return setImm(code, &code->op1, ptr, code->size);
  }
  return TRUE;
}

// 68851 PScc
static boolean pscc(codeptr ptr, disasm* code) {
  UWORD word2 = peekw(ptr + 2);

  if ((word2 & 0xfff0) != 0) return FALSE;

  OPECODE(pscc[word2 & 15]);
  code->bytes = 4;
  code->size = code->size2 = code->default_size = BYTESIZE;
  return setEA(code, &code->op1, ptr, DATA & CHANGE);
}

static boolean op001(codeptr ptr, disasm* code) {
  UWORD word1 = peekw(ptr);
  int eamode, opmode;

  if (reject_no_pmmu(code)) return FALSE;

  // PScc で未使用の<ea>にPDBccとPTRAPccが割り当てられているので先に調べる
  eamode = (word1 >> 3) & 7;
  if (eamode == 1) return pdbcc(ptr, code, word1);

  opmode = word1 & 7;
  if (eamode == 7 && CPTRAP_WORD <= opmode && opmode <= CPTRAP_UNSIZED) {
    return ptrapcc(ptr, code, opmode);
  }

  return pscc(ptr, code);
}

// 68851 PBcc
static boolean pbcc(codeptr ptr, disasm* code, UWORD word1, opesize size) {
  if (reject_no_pmmu(code)) return FALSE;
  if (word1 & 0x0030) return FALSE;

  OPECODE(pbcc[word1 & 15]);
  code->default_size = NOTHING;
  code->size = code->size2 = size;
  if (size == WORDSIZE) {
    code->bytes = 4;
    setrelative(code, &code->op1, extl(peekw(ptr + 2)));
  } else {
    LONG d32 = peekl(ptr + 2);
    code->bytes = 6;
    setrelative(code, &code->op1, d32);
    if (-32768 <= d32 && d32 <= 32767) code->codeflags += CODEFLAG_NEED_OPESIZE;
  }
  code->jmp = code->op1.opval;
  code->jmpea = code->op1.ea = PCDISP;
  code->opeType = BCCOP;
  code->op1.labelchange1 = LABELCHANGE_LABEL_ONLY;

  return TRUE;
}

// 68851 PSAVE
static boolean psave(codeptr ptr, disasm* code) {
  if (reject_no_pmmu(code)) return FALSE;

  OPECODE(psave);
  return setEA(code, &code->op1, ptr, CTRLCHG | PREDEC);
}

// 68851 PRESTORE
static boolean prestore(codeptr ptr, disasm* code) {
  if (reject_no_pmmu(code)) return FALSE;

  OPECODE(prestore);
  return setEA(code, &code->op1, ptr, CONTROL | POSTINC);
}

// 68851/68030の拡張命令(コプロセッサID=0)
boolean cp000(codeptr ptr, disasm* code) {
  UWORD word1 = peekw(ptr);
  int optype = (word1 >> 6) & 7;

  switch (optype) {
    default:
      break;

    case 0:
      return op000(ptr, code);
    case 1:
      return op001(ptr, code);
    case 2:
      return pbcc(ptr, code, word1, WORDSIZE);
    case 3:
      return pbcc(ptr, code, word1, LONGSIZE);
    case 4:
      return psave(ptr, code);
    case 5:
      return prestore(ptr, code);
  }

  return FALSE;
}

// EOF
