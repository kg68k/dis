// ソースコードジェネレータ
// 実効アドレス解釈 ヘッダ
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

#ifndef EA_H
#define EA_H

#include "disasm.h"
#include "eastr.h"
#include "estruct.h"
#include "etc.h"
#include "global.h"
#include "hex.h"

// アドレッシングモード (bitmaped)
//  Dn ----------------------------------------+
//  An ---------------------------------------+|
//  (An) ------------------------------------+||
//  (An)+ ----------------------------------+|||
//  -(An) ---------------------------------+||||
//  (d16,An) -----------------------------+|||||
//  (d8,An,ix) --------------------------+||||||
//  N/A --------------------------------+|||||||
//  (abs).w --------------------------+ ||||||||
//  (abs).l -------------------------+| ||||||||
//  (d16,pc) -----------------------+|| ||||||||
//  (d8,pc,ix) --------------------+||| ||||||||
//  #imm -------------------------+|||| ||||||||
enum {                  //        ||||| ||||||||
  DATAREG = 0x00001,    // ------------ -------@
  ADRREG = 0x00002,     // ------------ ------@-
  ADRIND = 0x00004,     // ------------ -----@--
  POSTINC = 0x00008,    // ------------ ----@---
  PREDEC = 0x00010,     // ------------ ---@----
  DISPAREG = 0x00020,   // ------------ --@-----
  IDXAREG = 0x00040,    // ------------ -@------
  ABSW = 0x00100,       // -----------@ --------
  ABSL = 0x00200,       // ----------@- --------
  DISPPC = 0x00400,     // ---------@-- --------
  IDXPC = 0x00800,      // --------@--- --------
  IMMEDIATE = 0x01000,  // -------@---- --------
  SRCCR = 0x10000,      // ---@-------- --------
  MOVEOPT = 0x80000,    // @----------- --------
  CHANGE = 0x0037f,     // ----------@@ -@@@@@@@
  CTRLCHG = 0x00364,    // ----------@@ -@@--@--
  CONTROL = 0x00f64,    // --------@@@@ -@@--@--
  MEMORY = 0x01f7c,     // -------@@@@@ -@@@@@--
  DATA = 0x01f7d,       // -------@@@@@ -@@@@@-@
  ALL = 0x01f7f,        // -------@@@@@ -@@@@@@@
};

extern void setRn(operand* op, int regno);
extern void setReglist(operand* op, UWORD reglist, boolean predec);
extern void setPairDn(operand* op, int reg1, int reg2);
extern void setPairID(operand* op, int reg1, int reg2);
extern void setPairFPn(operand* op, int reg1, int reg2);
extern void setFPCRSRlist(operand* op, int reglist);
extern void setFPreglist(operand* op, UBYTE list, boolean predec);
extern void setDynamicKFactor(operand* op, int regno);
extern void setStaticKFactor(operand* op, int kfactor);
extern void setBitField(operand* op, UWORD bf);
extern void setUsp(operand* op);
extern boolean setCtrlReg(disasm* code, operand* op, int regno);
extern void setCacheKind(operand* op, int caches);
extern boolean setMMUfc(disasm* code, operand* op, int fc);
extern void setMMUregVal(operand* op);
extern int setMMUreg(disasm* code, operand* op, UWORD word2, adrmode ea);
extern boolean setPMMUreg(disasm* code, operand* op, UWORD word2);

extern void setAnIndirect(operand* op, int regno);
extern void setAnPredec(operand* op, int regno);
extern void setAnPostinc(operand* op, int regno);
extern void setAnDisp(disasm* code, operand* op, int regno, WORD d16,
                      boolean nosize);
extern boolean setAbsLong(disasm* code, operand* op, codeptr ptr,
                          boolean move16);
extern void setImmSignedWord(disasm* code, operand* op, codeptr ptr);
extern void setImmSignedLong(disasm* code, operand* op, codeptr ptr);
extern boolean setImm(disasm* code, operand* op, codeptr ptr, opesize size);
extern void setCcr(operand* op);
extern void setSr(operand* op);
extern boolean setEA(disasm* code, operand* op, codeptr ptr, int mode);

// 指定MPUだけが対象なら未定義命令
static inline boolean reject(disasm* code, mputypes m) {
  if ((Dis.mpu & ~m) == 0) return TRUE;

  code->mputypes &= ~m;
  return FALSE;
}

// 68040/68060でFPSP無しなら未定義命令(6888xなら常に有効)
static inline boolean reject_no_fpsp(disasm* code) {
  fputypes fpu = Dis.fpu;
  if ((fpu & (F4SP | F6SP)) == 0) {
    if ((fpu & ~(F040 | F060)) == 0) return TRUE;

    code->mputypes &= ~(M040 | M060);
  }
  return FALSE;
}

// 68851無しなら未定義命令
static inline boolean reject_no_pmmu(disasm* code) {
  if ((Dis.mmu & MMU851) == 0) return TRUE;

  code->mputypes = M020;
  return FALSE;
}

// brief extension word format / full extension word format 共通
//   他は ec.c にある。

// インデックスレジスタの種類(Dn/An)と番号(0-7)
static inline int ex_ix_typenum(UWORD w) { return (w >> 12) & 15; }
// インデックスレジスタのサイズ 0=.w 1=.l
static inline int ex_ix_size(UWORD w) { return (w >> 11) & 1; }
// インデックスレジスタのスケールファクタ 0=*1 1=*2 2=*4 3=*8
static inline int ex_ix_scale(UWORD w) { return (w >> 9) & 3; }
// インデックスレジスタのサイズとスケールファクタ
static inline int ex_ix_sizescale(UWORD w) { return (w >> 9) & 7; }

// brief extension word format のみ

// 8ビットディスプレースメント
static inline BYTE ex_brief_d8(UWORD w) { return (BYTE)(w & 0x00ff); }

// アドレッシングモードがアドレスレジスタ直接か
//   0bxxxx_xxxx_xx00_1rrr  x=命令依存 rrr=レジスタ番号
static inline boolean is_areg_direct(UWORD word1) {
  return ((word1 & 0x0038) == 0x0008) ? TRUE : FALSE;
}

// アドレッシングモードがプリデクリメントか
//   0bxxxx_xxxx_xx10_0rrr  x=命令依存 rrr=レジスタ番号
static inline boolean is_predecrement(UWORD word1) {
  return ((word1 & 0x0038) == 0x0020) ? TRUE : FALSE;
}

// 命令の第1ワードから<ea>をそのまま取り出す
static inline int extract_ea_modreg(UWORD word1) { return word1 & 0x003f; }

// 命令の第1ワードから<ea>を取り出す
//   mode=7のときregisterを解釈する。拡張ワードは解釈しない
//   戻り値: DregD...AregIDX, AbShort...IMMED
static inline adrmode extract_ea_basic(UWORD word1) {
  int ea = (word1 >> 3) & 7;
  return (ea == 7) ? AbShort + (word1 & 7) : ea;
}

static inline void setDn(operand* op, int regno) {
  op->ea = DregD;
  op->baseReg = regno & 7;

  if (Dis.needString) {
    *write_dn(op->operand, op->baseReg) = '\0';
  }
}

static inline void setAn(operand* op, int regno) {
  op->ea = AregD;
  op->baseReg = regno & 7;

  if (Dis.needString) {
    *write_an(op->operand, op->baseReg) = '\0';
  }
}

static inline void setFPn(operand* op, int regno) {
  op->ea = FPregD;
  if (Dis.needString) {
    *write_fpn(op->operand, regno & 7) = '\0';
  }
}

// 最適化防止用に即値オペランドにサイズを付ける
static inline void addsize(operand* op, opesize size) {
  char* p = op->operand;
  *write_size(p + strlen(p), size) = '\0';
}

// 即値が 1～8 なら最適化防止用にオペランドにサイズを付ける
static inline void addsize_if_1to8(operand* op, opesize size) {
  if (((ULONG)op->opval - 1) <= 7) {
    addsize(op, size);
  }
}

// 命令固有の方法で即値が埋め込まれている場合用(10進数表記)
static inline void setImmEmbed(operand* op, ULONG imm) {
  op->ea = IMMED;
  op->opval = (address)imm;

  if (Dis.needString) {
    char* p = op->operand;
    *p++ = '#';
    itod2(p, imm);
  }
}

// 命令固有の方法で即値が埋め込まれている場合用(16進数表記)
static inline void setImmEmbedHex(operand* op, ULONG imm) {
  op->ea = IMMED;
  op->opval = (address)imm;

  if (Dis.needString) {
    char* p = op->operand;
    *p++ = '#';
    itox2d(p, imm);
  }
}

// 即値 #1～#8 addq/subq/ビット演算
static inline void setImm18(operand* op, ULONG imm) {
  ULONG n = imm & 7;
  op->ea = IMMED;
  op->opval = (address)n;

  if (Dis.needString) {
    char* p = op->operand;
    *p++ = '#';
    *p++ = (n == 0) ? '8' : '0' + n;
    *p = '\0';
  }
}

// 相対分岐
//   disp: ロングワードに符号拡張したディスプレースメント
static inline void setrelative(disasm* code, operand* op, LONG disp) {
  op->opval = code->pc + 2 + disp;
  if (Dis.needString) {
    itox8d(op->operand, (ULONG)op->opval);
  }
}

// 相対分岐(4バイトコード・コプロセッサ命令用)
//   disp: ロングワードに符号拡張したディスプレースメント
static inline void setrelative4(disasm* code, operand* op, LONG disp) {
  op->opval = code->pc + 4 + disp;
  if (Dis.needString) {
    itox8d(op->operand, (ULONG)op->opval);
  }
}

#endif

// EOF
