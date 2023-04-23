// ソースコードジェネレータ
// 実効アドレス解釈
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

#include "ea.h"

#include <string.h>

#include "disasm.h"
#include "eastr.h"
#include "estruct.h"
#include "etc.h"
#include "fpconv.h"
#include "hex.h"
#include "mmu.h"

// IS-I/IS 変換テーブル(未定義の項目は DregD にしている)
// 0 000: No Memory Indirect Action
// 0 001: Indirect Preindexed with Null OD
// 0 010: Indirect Preindexed with Word OD
// 0 011: Indirect Preindexed with Long OD
// 0 100:
// 0 101: Indirect Postindexed with Null OD
// 0 110: Indirect Postindexed with Word OD
// 0 111: Indirect Postindexed with Long OD
// 1 000: No Memory Indirect Action
// 1 001: Memory Indirect with Null OD
// 1 010: Memory Indirect with Word OD
// 1 011: Memory Indirect with Long OD
// 1 100:
// 1 101:
// 1 110:
// 1 111:
static const uint8_t an_i_iis_to_ea_table[16] = {
    AregIDXB,    AregPREIDX,  AregPREIDX, AregPREIDX, DregD,      AregPOSTIDX,
    AregPOSTIDX, AregPOSTIDX, AregIDXB,   AregPREIDX, AregPREIDX, AregPREIDX,
    DregD,       DregD,       DregD,      DregD,
};
static const uint8_t pc_i_iis_to_ea_table[16] = {
    PCIDXB,    PCPREIDX,  PCPREIDX, PCPREIDX, DregD,    PCPOSTIDX,
    PCPOSTIDX, PCPOSTIDX, PCIDXB,   PCPREIDX, PCPREIDX, PCPREIDX,
    DregD,     DregD,     DregD,    DregD,
};

typedef enum {
  BASEREG_A0 = 0,  // a0-a7 = 0-7
  BASEREG_PC = -1
} BaseReg;

static int is_basereg_pc(BaseReg regno) { return regno < BASEREG_A0; }

// brief extension word format / full extension word format 共通
//   ex_ix_typenum()、ex_ix_size()、ex_ix_scale() は ea.h にある。

// インデックスレジスタが d0 かどうか
static int ex_is_ix_d0(UWORD w) { return (w & 0xf000) == 0x0000; }
// サプレスされたインデックスレジスタが自然な指定(zd0.w*1)か
static int ex_is_well_zix(UWORD w) { return (w & 0xfe00) == 0x0000; }
// 拡張ワードの種類 0=brief 1=full
static int ex_is_full_format(UWORD w) { return (w >> 8) & 1; }

// full のみ
// ベースレジスタ省略 0=省略しない 1=省略する
static int ex_is_br_suppress(UWORD w) { return (w >> 7) & 1; }
// インデックスレジスタ省略 0=省略しない 1=省略する
static int ex_is_ix_suppress(UWORD w) { return (w >> 6) & 1; }
// ベースディスプレースメントのサイズ 0=予約(未定義) 1=なし 2=ワード 3=ロング
static int ex_bd_size(UWORD w) { return (w >> 4) & 3; }
// Index/Indirect Selection
static int ex_iis(UWORD w) { return w & 7; }
// 未定義ビット
static int ex_reserved_bit(UWORD w) { return w & 0x0008; }

// an, zan, pc, zpc
static char* write_basereg(char* p, BaseReg regno, UWORD exword) {
  if (ex_is_br_suppress(exword)) p = write_suppress_z(p);
  return is_basereg_pc(regno) ? write_pc(p) : write_an(p, regno);
}

static char* write_ix(char* p, UWORD exword) {
  int scale;

  p = write_rn(p, ex_ix_typenum(exword));
  p = write_size(p, ex_ix_size(exword) ? LONGSIZE : WORDSIZE);

  scale = ex_ix_scale(exword);
  if (scale) {
    *p++ = '*';
    *p++ = '0' + (1 << scale);
  }
  return p;
}

static char* write_ix_or_zix(char* p, UWORD exword) {
  if (ex_is_ix_suppress(exword)) p = write_suppress_z(p);
  return write_ix(p, exword);
}

// 8ビットディスプレースメントを文字列化
static char* write0_d8(char* p, BYTE d8) {
  ULONG ul = d8;

  if (d8 < 0) {
    *p++ = '-';
    ul = -d8;
  }
  return itox2d(p, ul);
}

// ワードディスプレースメントを文字列化
static char* write0_d16(char* p, WORD d16) {
  ULONG ul = d16;

  if (d16 < 0) {
    *p++ = '-';
    ul = -d16;
  }
  return itox4d(p, ul);
}

// ワードディスプレースメントを文字列化
//   (最適化抑止) 0 の場合はサイズ .w をつける
static char* write_d16_avoid_null(char* p, WORD d16) {
  p = write0_d16(p, d16);
  return (d16 == 0) ? write_size(p, WORDSIZE) : p;
}

// ワードディスプレースメントを文字列化
//   (最適化抑止) d8 で表現できる値はサイズ .w をつける
static char* write_d16_avoid_d8(char* p, WORD d16) {
  p = write0_d16(p, d16);
  return (-128 <= d16 && d16 <= 128) ? write_size(p, WORDSIZE) : p;
}

// d32 の値について、d16 で表現できる範囲ならサイズ .l をつける
static char* write_size_l_if_d16(char* p, LONG d32) {
  return (-32768 <= d32 && d32 <= 32767) ? write_size(p, LONGSIZE) : p;
}

// ロングディスプレースメントを文字列化
//   (最適化抑止) d16 で表現できる値はサイズ .l をつける
//   $ffff8000-$ffffffff は負数として扱う
static char* write_d32_avoid_d16(char* p, LONG d32) {
  if ((ULONG)d32 >= 0xffff8000) {
    *p++ = '-';
    p = itox8d(p, (ULONG)-d32);
  } else {
    p = itox8d(p, (ULONG)d32);
  }
  return write_size_l_if_d16(p, d32);
}

static char* write_bd(char* p, operand* op, boolean avoidD8) {
  LONG disp = (LONG)op->opval;

  if (op->flags & OPFLAG_PC_RELATIVE) {
    // PC相対の場合はアドレスではなくディスプレースメント値で扱う
    disp -= (LONG)op->eaadrs - 2;

    if (op->exbd == 2) {
      if (avoidD8)
        return write_d16_avoid_d8(p, (WORD)disp);
      else
        return write_d16_avoid_null(p, (WORD)disp);
    } else {
      if ((ULONG)disp >= 0xffff8000) {
        *p++ = '-';
        disp = (ULONG)-disp;
      }
      p = itox8d(p, (ULONG)disp);
      return (-32768 <= disp && disp <= 32768) ? write_size(p, LONGSIZE) : p;
    }
  }

  return (op->exbd == 2) ? write_d16_avoid_null(p, (WORD)disp)
                         : write_d32_avoid_d16(p, disp);
}

static char* write_bd_comma(char* p, operand* op) {
  if (op->exbd == 0) return p;

  p = write_bd(p, op, FALSE);
  *p++ = ',';
  return p;
}

static char* write_od(char* p, operand* op) {
  return (op->exod == 2) ? write_d16_avoid_null(p, (WORD)op->opval2)
                         : write_d32_avoid_d16(p, (LONG)op->opval2);
}

static char* write_comma_od(char* p, operand* op) {
  if (op->exod == 0) return p;

  *p++ = ',';
  return write_od(p, op);
}

// データレジスタまたはアドレスレジスタ Dn or An
void setRn(operand* op, int regno) {
  if (regno & 8)
    setAn(op, regno);
  else
    setDn(op, regno);
}

static unsigned int bit_reverse(UBYTE n) {
  n = (n >> 4) | (n << 4);
  n = ((n & 0xcc) >> 2) | ((n & 0x33) << 2);
  n = ((n & 0xaa) >> 1) | ((n & 0x55) << 1);
  return n;
}

static char* stringifyRegList(char* p, unsigned int list, adrmode regtype);

void setReglist(operand* op, UWORD reglist, boolean predec) {
  op->ea = RegList;

  if (Dis.needString) {
    char* p = op->operand;

    // -(An) のみ |D0|D1|...|D6|D7|A0|A1|...|A6|A7| の並び
    unsigned int dreg = predec ? bit_reverse(reglist >> 8) : reglist & 0xff;
    unsigned int areg = predec ? bit_reverse(reglist & 0xff) : reglist >> 8;

    p = stringifyRegList(p, dreg, DregD);
    if (dreg && areg) *p++ = '/';
    stringifyRegList(p, areg, AregD);
  }
}

// データレジスタのペア Dm:Dn
void setPairDn(operand* op, int reg1, int reg2) {
  op->ea = RegPairD;

  if (Dis.needString) {
    char* p = op->operand;
    p = write_dn(p, reg1);
    *p++ = ':';
    *write_dn(p, reg2) = '\0';
  }
}

// レジスタ間接ペア (Rm):(Rn)
//   reg1, reg2 = 0...7
void setPairID(operand* op, int reg1, int reg2) {
  op->ea = RegPairID;

  if (Dis.needString) {
    char* p = op->operand;
    *p++ = '(';
    p = write_rn(p, reg1);
    *p++ = ')';
    *p++ = ':';
    *p++ = '(';
    p = write_rn(p, reg2);
    *p++ = ')';
    *p++ = '\0';
  }
}

// 浮動小数点レジスタのペア FPm:FPn
void setPairFPn(operand* op, int reg1, int reg2) {
  op->ea = FPPairD;

  if (Dis.needString) {
    char* p = op->operand;
    p = write_fpn(p, reg1);
    *p++ = ':';
    *write_fpn(p, reg2) = '\0';
  }
}

void setFPCRSRlist(operand* op, int reglist) {
  op->ea = FPCRSR;

  if (Dis.needString) {
    char* p = op->operand;

    if (reglist & 0x04) {
      p = write0_fpcrsr(p, 0);
      if (reglist & 0x03) *p++ = '/';
    }
    if (reglist & 0x02) {
      p = write0_fpcrsr(p, 1);
      if (reglist & 0x01) *p++ = '/';
    }
    if (reglist & 0x01) {
      p = write0_fpcrsr(p, 2);
    }
  }
}

void setFPreglist(operand* op, UBYTE reglist, boolean predec) {
  op->ea = FPregList;

  if (Dis.needString) {
    // -(An) 以外は |FP0|FP1|...|FP6|FP7| の並びなので
    // |FP7|FP6|...|FP1|FP0| に置き換える
    unsigned int list = predec ? reglist : bit_reverse(reglist);

    stringifyRegList(op->operand, list, FPregD);
  }
}

static int count_trailing_one(unsigned int n) {
  int c;
  for (c = 0; n & 1; n >>= 1) c += 1;
  return c;
}

// レジスタリストを文字列化
//   regtype: DregD, AregD, FPregD
static char* stringifyRegList(char* p, unsigned int list, adrmode regtype) {
  int regno;

  for (regno = 0; list != 0;) {
    int len;

    if ((list & 1) == 0) {
      regno += 1;
      list >>= 1;
      continue;
    }
    p = (regtype == DregD)   ? write_dn(p, regno)
        : (regtype == AregD) ? write_an(p, regno)
                             : write_fpn(p, regno);
    len = count_trailing_one(list);
    if (len >= 2) {
      int r = regno + len - 1;
      *p++ = '-';
      p = (regtype == DregD)   ? write_dn(p, r)
          : (regtype == AregD) ? write_an(p, r)
                               : write_fpn(p, r);
    }
    regno += (len + 1);
    list >>= (len + 1);
    if (list) *p++ = '/';
  }

  *p = '\0';
  return p;
}

// dynamic k-factor {Dn}
void setDynamicKFactor(operand* op, int regno) {
  op->ea = KFactor;
  op->flags |= OPFLAG_COMBINE;

  if (Dis.needString) {
    char* p = op->operand;
    *p++ = '{';
    p = write_dn(p, regno);
    *p++ = '}';
    *p = '\0';
  }
}

// static k-factor {#Imm}
//   kfactor: 7ビット、2の補数表現(0x40...0x7f -> -64...-1)
void setStaticKFactor(operand* op, int kfactor) {
  op->ea = KFactor;
  op->flags |= OPFLAG_COMBINE;

  if (Dis.needString) {
    char* p = op->operand;
    *p++ = '{';
    *p++ = '#';
    if (kfactor >= 0x40) {
      *p++ = '-';
      kfactor = 0x80 - kfactor;
    }
    p = itod2(p, kfactor);
    *p++ = '}';
    *p = '\0';
  }
}

// ビットフィールド {offset:width}
void setBitField(operand* op, UWORD bf) {
  op->ea = BitField;
  op->flags |= OPFLAG_COMBINE;

  if (Dis.needString) {
    int offset = (bf >> 6) & 0x1f;
    int width = bf & 0x1f;

    char* p = op->operand;
    *p++ = '{';

    p = (bf & 0x0800) ? write_dn(p, offset & 7) : itod2(p, offset);
    *p++ = ':';
    p = (bf & 0x0020) ? write_dn(p, width & 7) : itod2(p, width ? width : 32);

    *p++ = '}';
    *p = '\0';
  }
}

void setUsp(operand* op) {
  op->ea = CtrlReg;

  if (Dis.needString) {
    write0_ctrlreg(op->operand, CTRLREG_USP);
  }
}

static mputypes get_ctrlreg_mputypes(int regno) {
  static const mputypes types00x[9] = {
      M010 | M020 | M030 | M040 | M060,  // 0x000 sfc
      M010 | M020 | M030 | M040 | M060,  // 0x001 dfc
      M020 | M030 | M040 | M060,         // 0x002 cacr
      M040 | M060,                       // 0x003 tc
      M040,                              // 0x004 itt0
      M040,                              // 0x005 itt1
      M040,                              // 0x006 dtt0
      M040,                              // 0x007 dtt1
      M060                               // 0x008 buscr
  };
  static const mputypes types80x[9] = {
      M010 | M020 | M030 | M040 | M060,  // 0x800 usp
      M010 | M020 | M030 | M040 | M060,  // 0x801 vbr
      M020 | M030,                       // 0x802 caar
      M020 | M030 | M040,                // 0x803 msp
      M020 | M030 | M040,                // 0x804 isp
      M040,                              // 0x805 mmusr
      M040 | M060,                       // 0x806 urp
      M040 | M060,                       // 0x807 srp
      M060                               // 0x808 pcr
  };

  if (CTRLREG_SFC <= regno && regno <= CTRLREG_BUSCR) {
    return types00x[regno - CTRLREG_SFC];
  }
  if (CTRLREG_USP <= regno && regno <= CTRLREG_PCR) {
    return types80x[regno - CTRLREG_USP];
  }
  return 0;
}

boolean setCtrlReg(disasm* code, operand* op, int regno) {
  mputypes types = get_ctrlreg_mputypes(regno);

  if (types == 0 || reject(code, ~types)) return FALSE;
  op->ea = CtrlReg;

  if (Dis.needString) {
    write0_ctrlreg(op->operand, regno);
  }

  return TRUE;
}

void setCacheKind(operand* op, int caches) {
  op->ea = CtrlReg;

  if (Dis.needString) {
    write0_cachekind(op->operand, caches);
  }
}

boolean setMMUfc(disasm* code, operand* op, int fc) {
  fc &= 0x1f;

  if (fc == 0 || fc == 1) {
    op->ea = MMUreg;
    if (Dis.needString) {
      int regno = (fc == 0) ? CTRLREG_SFC : CTRLREG_DFC;
      write0_ctrlreg(op->operand, regno);
    }
    return TRUE;
  }
  if ((fc >> 3) == 1) {  // 01RRR
    setDn(op, fc);
    return TRUE;
  }
  if (fc & 0x10) {  // 1DDDD or 10DDD
    if (fc & 0x08) {
      // 68020+68851なら0-15を指定できる(1DDDD)が、68030は0-7のみ(10DDD)
      if ((Dis.mmu & MMU851) == 0) return FALSE;

      code->mputypes = M020;
    }
    op->ea = IMMED;

    if (Dis.needString) {
      char* p = op->operand;
      *p++ = '#';
      itod2(p, fc & 0x0f);
    }
    return TRUE;
  }

  return FALSE;
}

void setMMUregVal(operand* op) {
  op->ea = MMUreg;
  if (Dis.needString) {
    write0_mmureg(op->operand, MMUREG_VAL);
  }
}

static MmuReg get_mmu_regno(UWORD word2) {
  int type = word2 >> 13;
  int regno = (word2 >> 10) & 7;

  if (type == 2) {
    return MMUREG_TC + regno;
  } else if (type == 0) {
    if (regno == 2) return MMUREG_TT0;
    if (regno == 3) return MMUREG_TT1;
  }

  return -1;
}

// MC68851,MC68030 の PMOVE 命令(0b000,0b010)
//   ea: 命令の第1ワードに含まれる <ea>
//   <ea> として受け付けるモードを返す(0 なら不正な命令または対象外 MPU)
int setMMUreg(disasm* code, operand* op, UWORD word2, adrmode ea) {
  static const struct {
    mputypes neg_mpu;
    uint8_t size;
  } spec[] = {
      {~M030, LONGSIZE},                  // tt0
      {~M030, LONGSIZE},                  // tt1
      {~(M020 | M030 | M040), LONGSIZE},  // tc
      {~M020, QUADSIZE},                  // drp
      {~(M020 | M030 | M040), QUADSIZE},  // srp
      {~(M020 | M030), QUADSIZE},         // crp
      {~M020, BYTESIZE},                  // cal
      {~M020, BYTESIZE},                  // val
      {~M020, BYTESIZE},                  // scc
      {~M020, WORDSIZE}                   // ac
  };
  int mode = 0;
  int pmmu_mode;
  UWORD mmu_to_mem = word2 & 0x0200;
  MmuReg regno = get_mmu_regno(word2);

  if ((int)regno < 0 || reject(code, spec[regno].neg_mpu)) return 0;

  // <ea> が Dn,An,-(An),(An)+,#Imm,PC相対 なら MC68851 のみ
  pmmu_mode = mmu_to_mem ? (CHANGE ^ CTRLCHG) : (ALL ^ CTRLCHG);
  if (((1 << ea) & pmmu_mode) && reject_no_pmmu(code)) return 0;

  op->ea = MMUreg;
  code->size = code->size2 = code->default_size = spec[regno].size;
  code->bytes = 4;

  if (Dis.mmu & MMU851) mode |= (mmu_to_mem ? CHANGE : ALL);
  if (Dis.mmu & MMU030) mode |= CTRLCHG;
  // crp,drp,srp <--> dn,an は不可
  if (code->size == QUADSIZE) mode &= ~(DATAREG | ADRREG);

  if (Dis.needString) {
    write0_mmureg(op->operand, regno);
  }

  return mode;
}

// MC68851,MC68030 の PMOVE 命令(0b011)
boolean setPMMUreg(disasm* code, operand* op, UWORD word2) {
  int mputypes = 0;
  UWORD preg = word2 & 0x1dff;

  if (preg == 0x0000) {
    // 0b0110_00d0_0000_0000 ... MC68851 PSR (==MC68030 MMUSR)

    // -m68020,68030 なら 68030 を優先
    if (Dis.mmu & MMU030) {
      mputypes = M030;
      if (Dis.needString) {
        write0_mmusr(op->operand);
      }
    } else {
      mputypes = M020;
      if (Dis.needString) {
        write0_psr(op->operand);
      }
    }
  } else if (Dis.mmu & MMU851) {
    UWORD preg2 = word2 & 0x1de3;

    if (preg == 0x0400) {
      // 0b0110_01d0_0000_0000 ... MC68851 PCSR
      mputypes = M020;
      if (Dis.needString) {
        write0_pcsr(op->operand);
      }
    } else if (preg2 == 0x1000) {
      // 0b0110_00d0_000n_nn00 ... MC68851 BADx
      mputypes = M020;
      if (Dis.needString) {
        write0_badn(op->operand, (word2 >> 2) & 7);
      }
    } else if (preg2 == 0x1400) {
      // 0b0110_01d0_000n_nn00 ... MC68851 BACx
      mputypes = M020;
      if (Dis.needString) {
        write0_bacn(op->operand, (word2 >> 2) & 7);
      }
    }
  }
  if (mputypes == 0) return FALSE;

  code->mputypes = mputypes;
  op->ea = MMUreg;
  code->size = code->size2 = WORDSIZE;
  code->bytes = 4;
  return TRUE;
}

// Mode=0b010 Reg=An
void setAnIndirect(operand* op, int regno) {
  op->ea = AregID;

  if (Dis.needString) {
    char* p = op->operand;
    *p++ = '(';
    p = write_an(p, regno);
    *p++ = ')';
    *p = '\0';
  }
}

// Mode=0b011 Reg=An
void setAnPostinc(operand* op, int regno) {
  op->ea = AregIDPI;

  if (Dis.needString) {
    char* p = op->operand;
    *p++ = '(';
    p = write_an(p, regno);
    *p++ = ')';
    *p++ = '+';
    *p = '\0';
  }
}

// Mode=0b100 Reg=An
void setAnPredec(operand* op, int regno) {
  op->ea = AregIDPD;

  if (Dis.needString) {
    char* p = op->operand;
    *p++ = '-';
    *p++ = '(';
    p = write_an(p, regno);
    *p++ = ')';
    *p = '\0';
  }
}

// Mode=0b101 Reg=An
//   regno = 0...7
void setAnDisp(disasm* code, operand* op, int regno, WORD d16, boolean size) {
  op->ea = AregDISP;
  op->eaadrs = code->pc + code->bytes;
  op->opval = (address)extl(d16);
  op->baseReg = regno;

  code->bytes += 2;

  if (Dis.needString) {
    char* p = op->operand;
    *p++ = '(';
    p = size ? write_d16_avoid_null(p, d16) : write0_d16(p, d16);
    *p++ = ',';
    p = write_an(p, regno);
    *p++ = ')';
    *p = '\0';
  }
}

// Mode=0b110 Reg=An ... brief extension word format
static boolean setAregIDX(disasm* code, operand* op, int regno, UWORD exword) {
  BYTE d8 = ex_brief_d8(exword);

  // 68000-68010 はスケールファクタ 未対応
  int ix_scale = ex_ix_scale(exword);
  if (ix_scale != 0 && reject(code, M000 | M010)) return FALSE;

  op->ea = AregIDX;
  op->eaadrs = code->pc + code->bytes;
  op->opval = (address)extbl(d8);
  op->baseReg = regno;
  op->ixReg = ex_ix_typenum(exword);
  op->ixSizeScale = ex_ix_sizescale(exword);
  code->bytes += 2;

  if (Dis.needString) {
    char* p = op->operand;

    *p++ = '(';
    if (d8) {
      p = write0_d8(p, d8);
      *p++ = ',';
    }
    p = write_an(p, regno);
    *p++ = ',';
    p = write_ix(p, exword);
    *p++ = ')';
    *p = '\0';
  }

  return TRUE;
}

// (bd,an|pc,ix)
//   (an|pc,ix) のとき、HASによる (d8,an|pc,ix) への最適化を抑制できない
static void AnPcIDXB_an_ix(operand* op, BaseReg regno, UWORD exword) {
  char* p = op->operand;
  *p++ = '(';

  if (op->exbd) {
    p = write_bd(p, op, TRUE);
    *p++ = ',';
  }
  p = write_basereg(p, regno, exword);
  *p++ = ',';
  p = write_ix_or_zix(p, exword);

  *p++ = ')';
  *p = '\0';
}

// (bd,ill-zan,ix)  (bd,an|ill-zan|pc|zpc,ill-zix)
// (bd,zpc,ix) ... (d8,zpc,ix) は存在しないので考慮不要
static void AnPcIDXB_an_ix_ill(operand* op, BaseReg regno, UWORD exword) {
  char* p = op->operand;
  *p++ = '(';

  p = write_bd_comma(p, op);
  p = write_basereg(p, regno, exword);
  *p++ = ',';
  p = write_ix_or_zix(p, exword);

  *p++ = ')';
  *p = '\0';
}

// (bd,ix)  (bd,ill-zix)
static void AnIDXB_zan_ix(operand* op, UWORD exword) {
  char* p = op->operand;
  *p++ = '(';

  p = write_bd_comma(p, op);
  p = write_ix(p, exword);

  *p++ = ')';
  *p = '\0';
}

// (bd,an|pc)
static void AnPcIDXB_an_zix(operand* op, BaseReg regno, UWORD exword) {
  char* p = op->operand;
  *p++ = '(';

  p = write_bd_comma(p, op);
  p = write_basereg(p, regno, exword);

  // 最適化抑制のため、省略されたインデックスレジスタをわざと明示する
  //   (an) --> (an,zix)  (d16,an) --> (d16,an,zix)
  //   (d16,pc) --> (d16,pc,zix)  (pc)は最適化不可なので不要
  if (is_basereg_pc(regno) ? op->exbd == 2 : op->exbd != 4) {
    *p++ = ',';
    p = write_ix_or_zix(p, exword);
  }
  *p++ = ')';
  *p = '\0';
}

// (bd,zan|zpc)
static void AnPcIDXB_zan_zix(operand* op, BaseReg regno, UWORD exword) {
  char* p = op->operand;
  *p++ = '(';

  p = write_bd_comma(p, op);
  p = write_basereg(p, regno, exword);

  *p++ = ')';
  *p = '\0';
}

// ベースレジスタ 有効, 省略(za0/zpc), 不自然な省略(za1-za7)
enum {
  BRTYPE_AN = 0,
  BRTYPE_ZAN = 1,
  BRTYPE_ILL_ZAN = 2,
};
// インデックスレジスタ 有効, 省略(zd0.w), 不自然な省略(zd0.l/zd1-zd7.[wl])
enum {
  IXTYPE_IX = 0,
  IXTYPE_ZIX = 3,
  IXTYPE_ILL_ZIX = 6,
};

static void stringifyAregIDXB(operand* op, BaseReg regno, UWORD exword) {
  int brtype = ex_is_br_suppress(exword)
                   ? (regno == BASEREG_A0) ? BRTYPE_ZAN : BRTYPE_ILL_ZAN
                   : BRTYPE_AN;
  int ixtype = ex_is_ix_suppress(exword)
                   ? ex_is_well_zix(exword) ? IXTYPE_ZIX : IXTYPE_ILL_ZIX
                   : IXTYPE_IX;

  switch (ixtype + brtype) {
    default:
      break;

    case IXTYPE_IX + BRTYPE_AN:
      AnPcIDXB_an_ix(op, regno, exword);
      break;

    case IXTYPE_IX + BRTYPE_ILL_ZAN:
    case IXTYPE_ILL_ZIX + BRTYPE_AN:
    case IXTYPE_ILL_ZIX + BRTYPE_ILL_ZAN:
      AnPcIDXB_an_ix_ill(op, regno, exword);
      break;

    case IXTYPE_IX + BRTYPE_ZAN:
    case IXTYPE_ILL_ZIX + BRTYPE_ZAN:
      AnIDXB_zan_ix(op, exword);
      break;

    case IXTYPE_ZIX + BRTYPE_AN:
      AnPcIDXB_an_zix(op, regno, exword);
      break;

    case IXTYPE_ZIX + BRTYPE_ZAN:
    case IXTYPE_ZIX + BRTYPE_ILL_ZAN:
      AnPcIDXB_zan_zix(op, regno, exword);
      break;
  }
}

static void stringifyPCIDXB(operand* op, BaseReg regno, UWORD exword) {
  int brtype = ex_is_br_suppress(exword) ? BRTYPE_ZAN : BRTYPE_AN;
  int ixtype = ex_is_ix_suppress(exword)
                   ? ex_is_well_zix(exword) ? IXTYPE_ZIX : IXTYPE_ILL_ZIX
                   : IXTYPE_IX;

  switch (ixtype + brtype) {
    default:
      break;

    case IXTYPE_IX + BRTYPE_AN:
      AnPcIDXB_an_ix(op, regno, exword);
      break;

    case IXTYPE_ZIX + BRTYPE_AN:
      AnPcIDXB_an_zix(op, regno, exword);
      break;

    case IXTYPE_ILL_ZIX + BRTYPE_AN:
      AnPcIDXB_an_ix_ill(op, regno, exword);
      break;

    case IXTYPE_IX + BRTYPE_ZAN:
    case IXTYPE_ILL_ZIX + BRTYPE_ZAN:
      AnPcIDXB_an_ix_ill(op, regno, exword);
      break;

    case IXTYPE_ZIX + BRTYPE_ZAN:
      AnPcIDXB_zan_zix(op, regno, exword);
      break;
  }
}

// ([bd,an|zan],ix,od)  ([bd,pc|zpc],ix,od)
static void AnPcPOSTIDX(operand* op, BaseReg regno, UWORD exword, int brtype) {
  char c = 0;
  char* p = op->operand;
  *p++ = '(';

  *p++ = '[';
  if (op->exbd) {
    p = write_bd(p, op, FALSE);
    c = ',';
  }
  if (brtype == BRTYPE_AN) {
    if (c) *p++ = c;
    p = write_basereg(p, regno, exword);
  }
  *p++ = ']';
  *p++ = ',';
  p = write_ix_or_zix(p, exword);
  p = write_comma_od(p, op);

  *p++ = ')';
  *p = '\0';
}

static void stringifyAregPOSTIDX(operand* op, BaseReg regno, UWORD exword) {
  int brtype = (regno == BASEREG_A0 && ex_is_br_suppress(exword)) ? BRTYPE_ZAN
                                                                  : BRTYPE_AN;

  // ベースディスプレースメントなしの場合、za0 は省略できない
  if (op->exbd == 0) brtype = BRTYPE_AN;

  AnPcPOSTIDX(op, regno, exword, brtype);
}

static void stringifyPCPOSTIDX(operand* op, BaseReg regno, UWORD exword) {
  int brtype = BRTYPE_AN;  // pc/zpc は省略できない

  AnPcPOSTIDX(op, regno, exword, brtype);
}

// ([bd,an|zan,ix],od)  ([bd,pc|zpc,ix],od)
static void AnPcPREIDX(operand* op, BaseReg regno, UWORD exword, int brtype,
                       int ixtype) {
  char c = 0;
  char* p = op->operand;
  *p++ = '(';

  *p++ = '[';
  if (op->exbd) {
    p = write_bd(p, op, FALSE);
    c = ',';
  }
  if (brtype == BRTYPE_AN) {
    if (c) *p++ = c;
    p = write_basereg(p, regno, exword);
    c = ',';
  }
  if (ixtype == IXTYPE_IX) {
    if (c) *p++ = c;
    p = write_ix(p, exword);
  }
  *p++ = ']';
  p = write_comma_od(p, op);

  *p++ = ')';
  *p = '\0';
}

static void stringfyAregPREIDX(operand* op, BaseReg regno, UWORD exword) {
  int brtype = (regno == BASEREG_A0 && ex_is_br_suppress(exword)) ? BRTYPE_ZAN
                                                                  : BRTYPE_AN;
  int ixtype = (ex_is_ix_suppress(exword) && ex_is_well_zix(exword))
                   ? IXTYPE_ZIX
                   : IXTYPE_IX;

  // ベースディスプレースメントなし、インデックスレジスタなしの場合、
  // za0 は省略できない
  if (op->exbd == 0 && ixtype != IXTYPE_IX) brtype = BRTYPE_AN;

  AnPcPREIDX(op, regno, exword, brtype, ixtype);
}

static void stringfyPCPREIDX(operand* op, BaseReg regno, UWORD exword) {
  int brtype = BRTYPE_AN;  // pc/zpc は省略できない
  int ixtype = (ex_is_ix_suppress(exword) && ex_is_well_zix(exword))
                   ? IXTYPE_ZIX
                   : IXTYPE_IX;

  AnPcPREIDX(op, regno, exword, brtype, ixtype);
}

// サプレスされたインデックスレジスタが自然な指定(zd0.w)か
static boolean is_well_formed_ix(UWORD exword) {
  if (!ex_is_ix_d0(exword) && Dis.undefExReg) return FALSE;
  if (ex_ix_scale(exword) != 0 && Dis.undefExScale) return FALSE;
  if (ex_ix_size(exword) != 0 && Dis.undefExSize) return FALSE;

  return TRUE;
}

// Mode=0b110 Reg=An ... full extension word format
// Mode=0b111 Reg=0b011  (PC indirect)
static boolean setFullExWordFormat(disasm* code, operand* op, codeptr ptr,
                                   BaseReg regno, UWORD exword) {
  int br_suppress, ix_suppless, bd_size, iis;

  // 拡張ワードへのオフセット (+PC で実行時アドレス、+ptr で格納メモリ)
  int code_bytes;

  if (reject(code, M000 | M010)) return FALSE;

  bd_size = ex_bd_size(exword);  // 1=null 2=word 3=long
  if (bd_size == 0 || ex_reserved_bit(exword)) return FALSE;

  br_suppress = ex_is_br_suppress(exword);
  if (br_suppress && !is_basereg_pc(regno)) {
    // サプレスされたベースレジスタが za1-za7 か
    if (regno != BASEREG_A0 && Dis.undefExReg) return FALSE;
  }

  ix_suppless = ex_is_ix_suppress(exword);
  if (ix_suppless && !is_well_formed_ix(exword)) return FALSE;

  iis = ex_iis(exword);
  {
    const uint8_t* table =
        is_basereg_pc(regno) ? pc_i_iis_to_ea_table : an_i_iis_to_ea_table;
    uint8_t ea = table[(ix_suppless << 3) + iis];

    if (ea == DregD) return FALSE;
    op->ea = (adrmode)ea;
  }

  code_bytes = code->bytes + 2;
  op->exbd = 0;
  op->exod = 0;

  switch (bd_size) {
    default:
      break;

    case 2:  // Word Displacement
      op->eaadrs = code->pc + code_bytes;
      op->opval = (address)extl(peekw(ptr + code_bytes));
      code_bytes += (op->exbd = 2);

      if (is_basereg_pc(regno) && !br_suppress) {
        // (bd.w,pc) ならラベル化する
        op->opval += (ULONG)(code->pc + code->bytes);
        op->labelchange1 = LABELCHANGE_ALLOW;
        op->flags |= OPFLAG_PC_RELATIVE;
      }
      break;

    case 3:  // Long Displacement
      op->eaadrs = code->pc + code_bytes;
      op->opval = (address)peekl(ptr + code_bytes);
      code_bytes += (op->exbd = 4);

      // (bd.l,an) (bd.l,zan) (bd.l,zpc) はリロケート情報があるときだけラベル化
      op->labelchange1 = LABELCHANGE_DEPEND;

      if (is_basereg_pc(regno)) {
        if (!br_suppress) {
          // (bd.l,pc)
          op->opval += (ULONG)(code->pc + code->bytes);
          op->labelchange1 = LABELCHANGE_ALLOW;
          op->flags |= OPFLAG_PC_RELATIVE;
        }
      }
      break;
  }

  switch (iis) {
    default:
      break;

    case 2:  // Word Displacement
    case 6:
    case 10:
      op->eaadrs2 = code->pc + code_bytes;
      op->opval2 = (address)extl(peekw(ptr + code_bytes));
      code_bytes += (op->exod = 2);
      break;

    case 3:  // Long Displacement
    case 7:
    case 11:
      op->eaadrs2 = code->pc + code_bytes;
      op->opval2 = (address)peekl(ptr + code_bytes);
      code_bytes += (op->exod = 4);
      break;
  }

  code->bytes = code_bytes;

  if (!Dis.needString) return TRUE;
  // 以下は文字列化処理

  switch (op->ea) {
    default:
      break;

    case AregIDXB:
      stringifyAregIDXB(op, regno, exword);
      break;
    case AregPOSTIDX:
      stringifyAregPOSTIDX(op, regno, exword);
      break;
    case AregPREIDX:
      stringfyAregPREIDX(op, regno, exword);
      break;

    case PCIDXB:
      stringifyPCIDXB(op, regno, exword);
      break;
    case PCPOSTIDX:
      stringifyPCPOSTIDX(op, regno, exword);
      break;
    case PCPREIDX:
      stringfyPCPREIDX(op, regno, exword);
      break;
  }
  return TRUE;
}

// 奇数アドレスへのワード以上アクセスなら未定義命令
static boolean reject_odd(disasm* code, operand* op) {
  if ((ULONG)op->opval & 1 && code->size2 != BYTESIZE &&
      code->size2 != UNKNOWN && !Dis.acceptAddressError) {
    return TRUE;
  }
  return FALSE;
}

// Mode=0b111 Reg=0b000
static boolean setAbsShort(disasm* code, operand* op, codeptr ptr) {
  op->ea = AbShort;
  op->eaadrs = code->pc + code->bytes;
  op->opval = (address)extl(peekw(ptr + code->bytes));
  if (reject_odd(code, op)) return FALSE;
  code->bytes += 2;

  if (Dis.needString) {
    LONG d32 = op->opval;
    char* p = op->operand;
    *p++ = '(';
    if (d32 < 0) {
      *p++ = '-';
      d32 = -d32;  // -1...-$8000
    }
    p = itox4d(p, (ULONG)d32);
    *p++ = ')';
    *p++ = '\0';
  }
  return TRUE;
}

// Mode=0b111 Reg=0b001
boolean setAbsLong(disasm* code, operand* op, codeptr ptr, boolean move16) {
  op->ea = AbLong;
  op->eaadrs = code->pc + code->bytes;
  op->opval = (address)peekl(ptr + code->bytes);

  // move16 は16バイト(ライン)境界からの転送なので奇数アドレスチェック不要
  if (!move16 && reject_odd(code, op)) return FALSE;

  op->labelchange1 = LABELCHANGE_DEPEND;
  code->bytes += 4;

  if (Dis.needString) {
    char* p = op->operand;
    *p++ = '(';
    p = itox8d(p, (ULONG)op->opval);
    *p++ = ')';

    // move16 は (abs).w がないのでサイズ不要
    if (!move16) p = write_size_l_if_d16(p, (LONG)op->opval);

    *p++ = '\0';
  }
  return TRUE;
}

// Mode=0b111 Reg=0b010
static boolean setPcDisp(disasm* code, operand* op, WORD d16) {
  op->ea = PCDISP;
  op->eaadrs = code->pc + code->bytes;
  op->opval = op->eaadrs + (LONG)d16;
  if (reject_odd(code, op)) return FALSE;
  op->labelchange1 = LABELCHANGE_ALLOW;
  op->flags |= OPFLAG_PC_RELATIVE;
  code->bytes += 2;

  if (Dis.needString) {
    char* p = op->operand;
    *p++ = '(';

    p = write0_d16(p, d16);
    *p++ = ',';
    p = write_pc(p);

    *p++ = ')';
    *p = '\0';
  }
  return TRUE;
}

// Mode=0b110 Reg=0b011 ... brief extension word format
static boolean setPCIDX(disasm* code, operand* op, UWORD exword) {
  BYTE d8 = ex_brief_d8(exword);

  // 68000-68010 はスケールファクタ 未対応
  int ix_scale = ex_ix_scale(exword);
  if (ix_scale != 0 && reject(code, M000 | M010)) return FALSE;

  op->ea = PCIDX;
  op->eaadrs = code->pc + code->bytes;
  op->opval = op->eaadrs + d8;
  op->labelchange1 = LABELCHANGE_ALLOW;
  op->flags |= OPFLAG_PC_RELATIVE;
  op->ixReg = ex_ix_typenum(exword);
  op->ixSizeScale = ex_ix_sizescale(exword);
  code->bytes += 2;

  if (Dis.needString) {
    char* p = op->operand;

    *p++ = '(';
    p = write0_d8(p, d8);
    *p++ = ',';
    p = write_pc(p);
    *p++ = ',';
    p = write_ix(p, exword);
    *p++ = ')';
    *p = '\0';
  }
  return TRUE;
}

// ワードの符号付き即値(link.w)
void setImmSignedWord(disasm* code, operand* op, codeptr ptr) {
  WORD d16 = peekw(ptr + code->bytes);

  op->ea = IMMED;
  op->eaadrs = code->pc + code->bytes;
  op->opval = (address)extl(d16);
  code->bytes += 2;

  if (Dis.needString) {
    char* p = op->operand;
    *p++ = '#';
    write0_d16(p, d16);
  }
}

// ロングワードの符号付き即値(link.l)
void setImmSignedLong(disasm* code, operand* op, codeptr ptr) {
  LONG d32 = peekl(ptr + code->bytes);

  op->ea = IMMED;
  op->eaadrs = code->pc + code->bytes;
  op->labelchange1 = LABELCHANGE_DEPEND;
  op->opval = (address)d32;
  code->bytes += 4;

  if (Dis.needString) {
    char* p = op->operand;
    *p++ = '#';
    if (d32 < 0) {
      *p++ = '-';
      d32 = -d32;
    }
    itox8d(p, (ULONG)d32);
  }
}

static boolean is_ill_formed_undef_byte(UBYTE undefbyte, UBYTE imm) {
  if (undefbyte == 0x00) return FALSE;
  if (undefbyte == 0xff && imm >= 0x80) return FALSE;

  return TRUE;
}

static boolean setImmByte(disasm* code, operand* op, codeptr ptr) {
  UBYTE* bytes = (UBYTE*)(ptr + code->bytes);
  UBYTE undefbyte = bytes[0], imm = bytes[1];

  // 不定バイト(即値ワードの上位バイト)のチェック
  if (!Dis.f && is_ill_formed_undef_byte(undefbyte, imm)) {
    return FALSE;
  }

  op->opval = (address)(ULONG)imm;
  code->bytes += 2;

  if (Dis.needString) {
    char* p = op->operand;
    *p++ = '#';
    if (undefbyte == 0xff && imm >= 0x80) {
      *p++ = '-';  // おそらく負数として記述されていたので負数として再現する
      imm = -imm;
    }
    itox2d(p, (ULONG)imm);
  }
  return TRUE;
}

boolean setImm(disasm* code, operand* op, codeptr ptr, opesize size) {
  op->ea = IMMED;
  op->eaadrs = code->pc + code->bytes;

  switch (size) {
    default:
      break;

    case BYTESIZE:
      return setImmByte(code, op, ptr);

    case WORDSIZE:
      op->opval = (address)(ULONG)peekw(ptr + code->bytes);
      code->bytes += 2;

      if (Dis.needString) {
        char* p = op->operand;
        *p++ = '#';
        itox4d(p, (ULONG)op->opval);
      }
      return TRUE;

    case LONGSIZE:
      op->labelchange1 = LABELCHANGE_DEPEND;
      op->opval = (address)peekl(ptr + code->bytes);
      code->bytes += 4;

      if (Dis.needString) {
        char* p = op->operand;
        *p++ = '#';
        itox8d(p, (ULONG)op->opval);
      }
      return TRUE;

    case QUADSIZE:  // 8バイト(MMU命令専用)
      if (Dis.needString) {
        char* p = op->operand;
        *p++ = '#';
        stringifyQuadWord(p, (quadword*)(ptr + code->bytes));
      }
      code->bytes += 8;
      return TRUE;

    case SINGLESIZE:  // 単精度実数(4バイト)
      op->labelchange1 = LABELCHANGE_DEPEND;
      op->opval = (address)peekl(ptr + code->bytes);

      if (Dis.needString) {
        char* p = op->operand;
        *p++ = '#';
        fpconv_s(p, ptr + code->bytes);
      }
      code->bytes += 4;
      return TRUE;

    case DOUBLESIZE:  // 倍精度実数(8バイト)
      if (Dis.needString) {
        char* p = op->operand;
        *p++ = '#';
        fpconv_d(p, ptr + code->bytes);
      }
      code->bytes += 8;
      return TRUE;

    case EXTENDSIZE:  // 拡張精度実数(12バイト)
      if (reject_no_fpsp(code)) return FALSE;

      if (Dis.needString) {
        char* p = op->operand;
        *p++ = '#';
        fpconv_x(p, ptr + code->bytes);
      }
      code->bytes += 12;
      return TRUE;

    case PACKEDSIZE:  // パックドデシマル(12バイト)
      if (reject_no_fpsp(code)) return FALSE;

      if (Dis.needString) {
        char* p = op->operand;
        *p++ = '#';
        fpconv_p(p, ptr + code->bytes);
      }
      code->bytes += 12;
      return TRUE;
  }

  return FALSE;
}

void setCcr(operand* op) {
  op->ea = CcrSr;

  if (Dis.needString) {
    write0_ccr(op->operand);
  }
}

void setSr(operand* op) {
  op->ea = CcrSr;

  if (Dis.needString) {
    write0_sr(op->operand);
  }
}

static boolean setCcrSr(disasm* code, operand* op) {
  switch (code->size) {
    default:
      break;

    case BYTESIZE:
      setCcr(op);
      return TRUE;
    case WORDSIZE:
      setSr(op);
      return TRUE;
  }

  return FALSE;
}

// Mode=0b111 Reg=mode
static boolean setEAmode111(disasm* code, operand* op, codeptr ptr, int eareg,
                            int mode) {
  switch (eareg) {
    default:
      break;

    case 0:
      if ((mode & ABSW) == 0) break;
      return setAbsShort(code, op, ptr);

    case 1:
      if ((mode & ABSL) == 0) break;
      return setAbsLong(code, op, ptr, FALSE);

    case 2:
      if ((mode & DISPPC) == 0) break;
      return setPcDisp(code, op, (WORD)peekw(ptr + code->bytes));

    case 3:
      if ((mode & IDXPC) == 0) break;
      {
        UWORD exword = peekw(ptr + code->bytes);
        if (ex_is_full_format(exword))
          return setFullExWordFormat(code, op, ptr, BASEREG_PC, exword);
        else
          return setPCIDX(code, op, exword);
      }
      break;

    case 4:
      if ((mode & IMMEDIATE) != 0) {
        return setImm(code, op, ptr, code->size);
      }
      if ((mode & SRCCR) != 0) {
        return setCcrSr(code, op);
      }
      break;
  }

  return FALSE;
}

// 命令の実効アドレス部を解読し、対応する文字列を返す
// code->bytes, code->size をセットしてから呼び出すこと
// ptr にオペコードのアドレス
boolean setEA(disasm* code, operand* op, codeptr ptr, int mode) {
  int eamod;  // 実効アドレスモード
  int eareg;  // 実効アドレスレジスタ
  WORD word1 = peekw(ptr);
  UWORD exword;

  if (mode & MOVEOPT) {  // move 命令のディスティネーションの場合
    eamod = (word1 >> 6) & 7;
    eareg = (word1 >> 9) & 7;
  } else {
    eamod = (word1 >> 3) & 7;
    eareg = (word1 >> 0) & 7;
  }

  switch (eamod) {
    default:
      break;

    case 0:
      if ((mode & DATAREG) == 0) break;
      setDn(op, eareg);
      return TRUE;

    case 1:
      if ((mode & ADRREG) == 0) break;
      setAn(op, eareg);
      return TRUE;

    case 2:
      if ((mode & ADRIND) == 0) break;
      setAnIndirect(op, eareg);
      return TRUE;

    case 3:
      if ((mode & POSTINC) == 0) break;
      setAnPostinc(op, eareg);
      return TRUE;

    case 4:
      if ((mode & PREDEC) == 0) break;
      setAnPredec(op, eareg);
      return TRUE;

    case 5:
      if ((mode & DISPAREG) == 0) break;
      setAnDisp(code, op, eareg, (WORD)peekw(ptr + code->bytes), TRUE);
      return TRUE;

    case 6:
      if ((mode & IDXAREG) == 0) break;
      exword = peekw(ptr + code->bytes);
      if (ex_is_full_format(exword))
        return setFullExWordFormat(code, op, ptr, (BaseReg)eareg, exword);
      else
        return setAregIDX(code, op, eareg, exword);

    case 7:
      return setEAmode111(code, op, ptr, eareg, mode);
  }

  return FALSE;
}

// EOF
