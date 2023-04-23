// ソースコードジェネレータ
// 浮動小数点命令逆アセンブル
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

#include "fpu.h"

#include "disasm.h"
#include "ea.h"
#include "global.h"
#include "opstr.h"

// 一般命令で受け付けるオペランド形式(!=0なら値はなんでもいい)
enum {
  FPM_FPN = 1,
  FPN_ONLY = 2,
};

typedef struct {
  fputypes fputypes;
  uint8_t fpm_fpn, fpn_only;
  uint8_t opnum;
} Type000Spec;
static Type000Spec type000table[128] = {
    {F88x | F040 | F060, FPM_FPN, 0, FPU_T0_FMOVE},
    {F88x | F4SP | F060, FPM_FPN, FPN_ONLY, FPU_T0_FINT},
    {F88x | F4SP | F6SP, FPM_FPN, FPN_ONLY, FPU_T0_FSINH},
    {F88x | F4SP | F060, FPM_FPN, FPN_ONLY, FPU_T0_FINTRZ},
    {F88x | F040 | F060, FPM_FPN, FPN_ONLY, FPU_T0_FSQRT},
    {0, 0, 0, 0},
    {F88x | F4SP | F6SP, FPM_FPN, FPN_ONLY, FPU_T0_FLOGNP1},
    {0, 0, 0, 0},
    {F88x | F4SP | F6SP, FPM_FPN, FPN_ONLY, FPU_T0_FETOXM1},
    {F88x | F4SP | F6SP, FPM_FPN, FPN_ONLY, FPU_T0_FTANH},
    {F88x | F4SP | F6SP, FPM_FPN, FPN_ONLY, FPU_T0_FATAN},
    {0, 0, 0, 0},
    {F88x | F4SP | F6SP, FPM_FPN, FPN_ONLY, FPU_T0_FASIN},
    {F88x | F4SP | F6SP, FPM_FPN, FPN_ONLY, FPU_T0_FATANH},
    {F88x | F4SP | F6SP, FPM_FPN, FPN_ONLY, FPU_T0_FSIN},
    {F88x | F4SP | F6SP, FPM_FPN, FPN_ONLY, FPU_T0_FTAN},
    {F88x | F4SP | F6SP, FPM_FPN, FPN_ONLY, FPU_T0_FETOX},
    {F88x | F4SP | F6SP, FPM_FPN, FPN_ONLY, FPU_T0_FTWOTOX},
    {F88x | F4SP | F6SP, FPM_FPN, FPN_ONLY, FPU_T0_FTENTOX},
    {0, 0, 0, 0},
    {F88x | F4SP | F6SP, FPM_FPN, FPN_ONLY, FPU_T0_FLOGN},
    {F88x | F4SP | F6SP, FPM_FPN, FPN_ONLY, FPU_T0_FLOG10},
    {F88x | F4SP | F6SP, FPM_FPN, FPN_ONLY, FPU_T0_FLOG2},
    {0, 0, 0, 0},
    {F88x | F040 | F060, FPM_FPN, FPN_ONLY, FPU_T0_FABS},
    {F88x | F4SP | F6SP, FPM_FPN, FPN_ONLY, FPU_T0_FCOSH},
    {F88x | F040 | F060, FPM_FPN, FPN_ONLY, FPU_T0_FNEG},
    {0, 0, 0, 0},
    {F88x | F4SP | F6SP, FPM_FPN, FPN_ONLY, FPU_T0_FACOS},
    {F88x | F4SP | F6SP, FPM_FPN, FPN_ONLY, FPU_T0_FCOS},
    {F88x | F4SP | F6SP, FPM_FPN, FPN_ONLY, FPU_T0_FGETEXP},
    {F88x | F4SP | F6SP, FPM_FPN, FPN_ONLY, FPU_T0_FGETMAN},
    {F88x | F040 | F060, FPM_FPN, 0, FPU_T0_FDIV},
    {F88x | F4SP | F6SP, FPM_FPN, 0, FPU_T0_FMOD},
    {F88x | F040 | F060, FPM_FPN, 0, FPU_T0_FADD},
    {F88x | F040 | F060, FPM_FPN, 0, FPU_T0_FMUL},
    {F88x | F4SP | F060, FPM_FPN, 0, FPU_T0_FSGLDIV},
    {F88x | F4SP | F6SP, FPM_FPN, 0, FPU_T0_FREM},
    {F88x | F4SP | F6SP, FPM_FPN, 0, FPU_T0_FSCALE},
    {F88x | F4SP | F060, FPM_FPN, 0, FPU_T0_FSGLMUL},
    {F88x | F040 | F060, FPM_FPN, 0, FPU_T0_FSUB},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {F88x | F4SP | F6SP, FPM_FPN, 0, FPU_T0_FSINCOS},
    {F88x | F4SP | F6SP, FPM_FPN, 0, FPU_T0_FSINCOS},
    {F88x | F4SP | F6SP, FPM_FPN, 0, FPU_T0_FSINCOS},
    {F88x | F4SP | F6SP, FPM_FPN, 0, FPU_T0_FSINCOS},
    {F88x | F4SP | F6SP, FPM_FPN, 0, FPU_T0_FSINCOS},
    {F88x | F4SP | F6SP, FPM_FPN, 0, FPU_T0_FSINCOS},
    {F88x | F4SP | F6SP, FPM_FPN, 0, FPU_T0_FSINCOS},
    {F88x | F4SP | F6SP, FPM_FPN, 0, FPU_T0_FSINCOS},
    {F88x | F040 | F060, FPM_FPN, 0, FPU_T0_FCMP},
    {0, 0, 0, 0},
    {F88x | F040 | F060, 0, FPN_ONLY, FPU_T0_FTST},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {F040 | F060, FPM_FPN, 0, FPU_T0_FSMOVE},
    {F040 | F060, FPM_FPN, FPN_ONLY, FPU_T0_FSSQRT},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {F040 | F060, FPM_FPN, 0, FPU_T0_FDMOVE},
    {F040 | F060, FPM_FPN, FPN_ONLY, FPU_T0_FDSQRT},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {F040 | F060, FPM_FPN, FPN_ONLY, FPU_T0_FSABS},
    {0, 0, 0, 0},
    {F040 | F060, FPM_FPN, FPN_ONLY, FPU_T0_FSNEG},
    {0, 0, 0, 0},
    {F040 | F060, FPM_FPN, FPN_ONLY, FPU_T0_FDABS},
    {0, 0, 0, 0},
    {F040 | F060, FPM_FPN, FPN_ONLY, FPU_T0_FDNEG},
    {0, 0, 0, 0},
    {F040 | F060, FPM_FPN, 0, FPU_T0_FSDIV},
    {0, 0, 0, 0},
    {F040 | F060, FPM_FPN, 0, FPU_T0_FSADD},
    {F040 | F060, FPM_FPN, 0, FPU_T0_FSMUL},
    {F040 | F060, FPM_FPN, 0, FPU_T0_FDDIV},
    {0, 0, 0, 0},
    {F040 | F060, FPM_FPN, 0, FPU_T0_FDADD},
    {F040 | F060, FPM_FPN, 0, FPU_T0_FDMUL},
    {F040 | F060, FPM_FPN, 0, FPU_T0_FSSUB},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {F040 | F060, FPM_FPN, 0, FPU_T0_FDSUB},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
};

// destination format
enum {
  DF_LONG = 0,
  DF_SINGLE = 1,
  DF_EXTEND = 2,
  DF_PACKED_STATIC = 3,
  DF_WORD = 4,
  DF_DOUBLE = 5,
  DF_BYTE = 6,
  DF_PACKED_DYNAMIC = 7,
};

static const uint8_t formats[8] = {
    LONGSIZE,
    SINGLESIZE,  //
    EXTENDSIZE,
    PACKEDSIZE,  // static k-Factor
    WORDSIZE,
    DOUBLESIZE,  //
    BYTESIZE,
    PACKEDSIZE  // dynamic k-Factor
};

// <ea>が0か調べる(op000専用)
//   op000ならbit7-6==0なので省力化している
static boolean is_ea_0(UWORD word1) {
  return ((UBYTE)word1 == 0) ? TRUE : FALSE;
}

// 68060でFPSP無しなら未定義命令(6888x/68040なら常に有効)
static boolean reject_no_060fpsp(disasm* code) {
  fputypes fpu = Dis.fpu;
  if ((fpu & F6SP) == 0) {
    if ((fpu & ~F060) == 0) return TRUE;
    code->mputypes &= ~M060;
  }
  return FALSE;
}

static boolean reject_fpu(disasm* code, fputypes types) {
  fputypes t = types & Dis.fpu;
  mputypes m = 0;

  if (t & F88x) m |= (M020 | M030);
  if (t & (F040 | F4SP)) m |= M040;
  if (t & (F060 | F6SP)) m |= M060;
  code->mputypes = m;

  return (m == 0) ? TRUE : FALSE;
}

// FDBcc, FTRAPcc, FSccの第2ワードが正しいか？
//   FBccは第1ワードに埋め込まれているので対象外。
static boolean is_valid_fpcond(UWORD word2) {
  return (word2 & 0xffe0) ? FALSE : TRUE;
}

static int extract_fpcond(UWORD word1) { return word1 & 0x003f; }

static int extract_kfactor(UWORD word2) { return word2 & 0x007f; }

// (R/M==0) Fop.X FPm,FPn
static boolean op000cls001(codeptr ptr, disasm* code, UWORD word1) {
  UWORD word2 = peekw(ptr + 2);
  Type000Spec spec = type000table[word2 & 0x007f];
  int src = (word2 >> 10) & 7;
  int dst = (word2 >> 7) & 7;

  if (!is_ea_0(word1)) return FALSE;
  if (reject_fpu(code, spec.fputypes)) return FALSE;

  // FTST.X FPm は未使用レジスタフィールドが0でないといけない
  // (ただし、ソースと同じ値の場合も許容する)
  if (!spec.fpm_fpn && Dis.undefReg) {
    if (dst != 0 && dst != src) return FALSE;
  }

  code->bytes = 4;
  code->size = code->size2 = code->default_size = EXTENDSIZE;
  OPECODE(fpuType000[spec.opnum]);
  setFPn(&code->op1, src);

  if (spec.opnum == FPU_T0_FSINCOS) {
    setPairFPn(&code->op2, word2 & 7, dst);
  } else if (spec.fpm_fpn) {
    if (!spec.fpn_only || src != dst) {
      // Fop FPm,FPm を Fop FPm と省略表記できない場合
      setFPn(&code->op2, dst);
    }
  }

  return TRUE;
}

static boolean fmovecr(disasm* code, UWORD word1, UWORD word2) {
  if (!is_ea_0(word1) || reject_no_fpsp(code)) return FALSE;

  code->bytes = 4;
  code->size = code->size2 = code->default_size = EXTENDSIZE;
  OPECODE(fmovecr);
  setImmEmbedHex(&code->op1, word2 & 0x007f);
  setFPn(&code->op2, word2 >> 7);

  return TRUE;
}

static boolean fop_ea_fpn(codeptr ptr, disasm* code, UWORD word2) {
  // fmt==7はfmovecrなので不要
  static const int modes[7] = {
      DATA, DATA, MEMORY, MEMORY, DATA, MEMORY, DATA  //
  };

  Type000Spec spec = type000table[word2 & 0x007f];
  int fmt = (word2 >> 10) & 7;
  int dst = (word2 >> 7) & 7;
  opesize size = formats[fmt];

  if (!spec.fpm_fpn && dst != 0) return FALSE;
  if (reject_fpu(code, spec.fputypes)) return FALSE;
  if (size == PACKEDSIZE && reject_no_fpsp(code)) return FALSE;

  OPECODE(fpuType000[spec.opnum]);
  code->bytes = 4;
  code->default_size = EXTENDSIZE;
  code->size = code->size2 = size;
  if (!setEA(code, &code->op1, ptr, modes[fmt])) return FALSE;

  if (spec.opnum == FPU_T0_FSINCOS) {
    setPairFPn(&code->op2, word2 & 7, dst);
  } else if (spec.fpm_fpn) {
    setFPn(&code->op2, dst);
  }

  return TRUE;
}

// (R/M==1) Fop.<fmt> <ea>,FPn
static boolean op000cls010(codeptr ptr, disasm* code, UWORD word1) {
  UWORD word2 = peekw(ptr + 2);
  int fmt = (word2 >> 10) & 7;

  if (fmt == 7) {
    return fmovecr(code, word1, word2);
  }
  return fop_ea_fpn(ptr, code, word2);
}

// (R/M==1) FMOVE.<fmt> FPm,<ea>
static boolean fmove_fpm_ea(codeptr ptr, disasm* code) {
  static const int modes[8] = {
      (DATA & CHANGE),
      (DATA & CHANGE),  //
      (DATA & CHANGE) ^ DATAREG,
      (DATA & CHANGE) ^ DATAREG,  //
      (DATA & CHANGE),
      (DATA & CHANGE) ^ DATAREG,  //
      (DATA & CHANGE),
      (DATA & CHANGE) ^ DATAREG  //
  };

  UWORD word2 = peekw(ptr + 2);
  int fmt = (word2 >> 10) & 7;
  int kfactor = extract_kfactor(word2);

  switch (fmt) {
    case DF_PACKED_DYNAMIC:
      if ((kfactor & 0x0f) != 0) return FALSE;
      // FALLTHRU
    case DF_PACKED_STATIC:
      if (reject_no_fpsp(code)) return FALSE;
      break;
    default:
      if (kfactor != 0) return FALSE;
      break;
  }

  OPECODE(fpuType000[FPU_T0_FMOVE]);
  code->default_size = EXTENDSIZE;
  code->size = code->size2 = formats[fmt];
  code->bytes = 4;
  setFPn(&code->op1, word2 >> 7);

  if (!setEA(code, &code->op2, ptr, modes[fmt])) return FALSE;
  if (fmt == DF_PACKED_STATIC) {
    setStaticKFactor(&code->op3, kfactor);
  } else if (fmt == DF_PACKED_DYNAMIC) {
    setDynamicKFactor(&code->op3, kfactor >> 4);
  }

  return TRUE;
}

static boolean is_valid_fpcrlist(UWORD word2) {
  if ((word2 & 0x1c00) == 0) return FALSE;
  if (word2 & 0x03ff) return FALSE;

  return TRUE;
}

// 0-7について、1のビットの数を返す
static int popcount7(unsigned int n) {
  unsigned int a = n & 1;
  n >>= 1;
  return a + n - (n >> 1);
}

static const int fpcr_modes[8] = {
    0, ALL, DATA, MEMORY, DATA, MEMORY, MEMORY, MEMORY  //
};

// FMOVE <ea>,fpcr  FMOVEM <ea>,fpcrlist
static boolean fmove_to_fpcr(codeptr ptr, disasm* code) {
  UWORD word2 = peekw(ptr + 2);
  int reglist = (word2 >> 10) & 7;
  int count = popcount7(reglist);
  int mode = fpcr_modes[reglist];

  if (!is_valid_fpcrlist(word2)) return FALSE;

  code->bytes = 4;
  code->size = code->size2 = code->default_size = LONGSIZE;

  if (!setEA(code, &code->op1, ptr, mode)) return FALSE;
  if (code->op1.ea == IMMED && count >= 2 && reject_no_fpsp(code)) return FALSE;

  OPECODE2(count == 1, fpuType000[FPU_T0_FMOVE], fmovem);

  if (count == 1 || code->op1.ea != IMMED) {
    setFPCRSRlist(&code->op2, reglist);
  } else {
    if (!setEA(code, &code->op2, ptr, mode)) return FALSE;
    if (count == 2) {
      // fmovem #imm,#imm,reglist
      setFPCRSRlist(&code->op3, reglist);
    } else {
      // fmovem #imm,#imm,#imm,reglist
      if (!setEA(code, &code->op3, ptr, mode)) return FALSE;
      setFPCRSRlist(&code->op4, reglist);
    }
  }

  return TRUE;
}

// FMOVE fpcr,<ea>  FMOVEM fpcrlist,<ea>
static boolean fmove_from_fpcr(codeptr ptr, disasm* code) {
  UWORD word2 = peekw(ptr + 2);
  int reglist = (word2 >> 10) & 7;
  int count = popcount7(reglist);

  if (!is_valid_fpcrlist(word2)) return FALSE;

  code->bytes = 4;
  code->size = code->size2 = code->default_size = LONGSIZE;
  OPECODE2(count == 1, fpuType000[FPU_T0_FMOVE], fmovem);
  setFPCRSRlist(&code->op1, reglist);
  return setEA(code, &code->op2, ptr, fpcr_modes[reglist] & CHANGE);
}

enum {
  FMOVEM_DYNAMIC = 1 << 0,
  FMOVEM_POSTINC_CTRL = 1 << 1,
};

static boolean is_valid_reglist(disasm* code, int mode, UWORD word2) {
  if (mode & FMOVEM_DYNAMIC) {
    // dynamic register list
    if ((word2 & 0x078f) || reject_no_fpsp(code)) return FALSE;
  } else {
    // static register list
    if ((word2 & 0x0700) || (word2 & 0x00ff) == 0) return FALSE;
  }

  return TRUE;
}

// FMOVEM <ea>,<list>
static boolean fmovem_ea_fp(codeptr ptr, disasm* code) {
  UWORD word2 = peekw(ptr + 2);
  int mode = (word2 >> 11) & 3;

  // fmovem -(an),list は無いのだから、これも不可のはず(実機未検証)
  if ((mode & FMOVEM_POSTINC_CTRL) == 0) return FALSE;

  if (!is_valid_reglist(code, mode, word2)) return FALSE;

  OPECODE(fmovem);
  code->bytes = 4;
  code->size = code->size2 = EXTENDSIZE;
  if (!setEA(code, &code->op1, ptr, CONTROL | POSTINC)) return FALSE;
  if (mode & FMOVEM_DYNAMIC)
    setDn(&code->op2, word2 >> 4);
  else
    setFPreglist(&code->op2, word2 & 0x00ff, FALSE);

  return TRUE;
}

// FMOVEM <list>,<ea>
static boolean fmovem_fp_ea(codeptr ptr, disasm* code, UWORD word1) {
  UWORD word2 = peekw(ptr + 2);
  int mode = (word2 >> 11) & 3;

  // fmovem list,-(an) は predecrement addressing mode で、それ以外は
  // postincrement or control addressing mode のはず(実機未検証)
  boolean predec = (mode & FMOVEM_POSTINC_CTRL) ? FALSE : TRUE;
  if (is_predecrement(word1) != predec) return FALSE;

  if ((mode & FMOVEM_POSTINC_CTRL) && is_predecrement(word1)) return FALSE;

  if (!is_valid_reglist(code, mode, word2)) return FALSE;

  OPECODE(fmovem);
  code->bytes = 4;
  code->size = code->size2 = EXTENDSIZE;
  if (mode & FMOVEM_DYNAMIC)
    setDn(&code->op1, word2 >> 4);
  else
    setFPreglist(&code->op1, word2 & 0x00ff, is_predecrement(word1));
  return setEA(code, &code->op2, ptr, CONTROL | PREDEC);
}

// 一般命令
static boolean op000(codeptr ptr, disasm* code, UWORD word1) {
  int opclass = peekb(ptr + 2) >> 5;

  switch (opclass) {
    default:
      break;

    case 0:
      return op000cls001(ptr, code, word1);
    case 2:
      return op000cls010(ptr, code, word1);
    case 3:
      return fmove_fpm_ea(ptr, code);
    case 4:
      return fmove_to_fpcr(ptr, code);
    case 5:
      return fmove_from_fpcr(ptr, code);
    case 6:
      return fmovem_ea_fp(ptr, code);
    case 7:
      return fmovem_fp_ea(ptr, code, word1);
  }

  return FALSE;
}

static boolean fdbcc(codeptr ptr, disasm* code, UWORD word1, int cond) {
  OPECODE(fdbcc[cond]);

  // DBcc と違い、FDBcc は Unsized
  code->default_size = NOTHING;

  setDn(&code->op1, word1);
  setrelative4(code, &code->op2, extl(peekw(ptr + 4)));
  code->jmp = code->op2.opval;
  code->jmpea = code->op2.ea = PCDISP;
  code->op2.labelchange1 = LABELCHANGE_LABEL_ONLY;
  code->opeType = BCCOP;
  code->bytes = 6;

  return TRUE;
}

static boolean ftrapcc(codeptr ptr, disasm* code, int mode, int cond) {
  OPECODE(ftrapcc[cond]);
  code->bytes = 4;
  if (mode != CPTRAP_UNSIZED) {
    code->size = (mode == CPTRAP_WORD) ? WORDSIZE : LONGSIZE;
    return setImm(code, &code->op1, ptr, code->size);
  }

  return TRUE;
}

static boolean fscc(codeptr ptr, disasm* code, int cond) {
  OPECODE(fscc[cond]);
  code->bytes = 4;
  code->size = code->size2 = code->default_size = BYTESIZE;
  return setEA(code, &code->op1, ptr, DATA & CHANGE);
}

// FDBcc, FScc, FTRAPcc
static boolean op001(codeptr ptr, disasm* code, UWORD word1) {
  UWORD word2 = peekw(ptr + 2);
  int cond = extract_fpcond(word2);
  int eamode, opmode;

  if (!is_valid_fpcond(word2) || reject_no_060fpsp(code)) return FALSE;

  // FScc で未使用の<ea>にFDBccとFTRAPccが割り当てられているので先に調べる
  eamode = (word1 >> 3) & 7;
  if (eamode == 1) {
    return fdbcc(ptr, code, word1, cond);
  }
  opmode = word1 & 7;
  if (eamode == 7 && CPTRAP_WORD <= opmode && opmode <= CPTRAP_UNSIZED) {
    return ftrapcc(ptr, code, opmode, cond);
  }

  return fscc(ptr, code, cond);
}

static boolean fbcc(codeptr ptr, disasm* code, UWORD word1, opesize size) {
  int cond = extract_fpcond(word1);

  if (cond >= FPCOND_NUM) return FALSE;

  OPECODE(fbcc[cond]);
  code->opeType = BCCOP;
  if (cond == FPCOND_T) {
    code->opeType = JMPOP;
    code->codeflags += CODEFLAG_BRAOP;
  }
  code->default_size = NOTHING;
  code->size = code->size2 = size;
  if (size == WORDSIZE) {
    // FBcc.W
    code->bytes = 4;
    setrelative(code, &code->op1, extl(peekw(ptr + 2)));
  } else {
    // FBcc.L
    LONG d32 = peekl(ptr + 2);
    code->bytes = 6;
    setrelative(code, &code->op1, d32);
    if (-32768 <= d32 && d32 <= 32767) code->codeflags += CODEFLAG_NEED_OPESIZE;
  }
  code->jmp = code->op1.opval;
  code->jmpea = code->op1.ea = PCDISP;
  code->op1.labelchange1 = LABELCHANGE_LABEL_ONLY;

  return TRUE;
}

static boolean fnop(disasm* code) {
  OPECODE(fnop);
  code->default_size = NOTHING;
  code->bytes = 4;

  return TRUE;
}

static boolean fsave(codeptr ptr, disasm* code) {
  OPECODE(fsave);
  code->default_size = NOTHING;
  return setEA(code, &code->op1, ptr, CTRLCHG | PREDEC);
}

static boolean frestore(codeptr ptr, disasm* code) {
  OPECODE(frestore);
  code->default_size = NOTHING;
  return setEA(code, &code->op1, ptr, CONTROL | POSTINC);
}

static boolean decode(codeptr ptr, disasm* code, UWORD word1) {
  int optype = (word1 >> 6) & 7;

  switch (optype) {
    default:
      break;

    case 0:
      return op000(ptr, code, word1);
    case 1:
      return op001(ptr, code, word1);
    case 2:
      if ((word1 & 0xff) == 0x80 && peekw(ptr + 2) == 0) return fnop(code);
      return fbcc(ptr, code, word1, WORDSIZE);
    case 3:
      return fbcc(ptr, code, word1, LONGSIZE);
    case 4:
      return fsave(ptr, code);
    case 5:
      return frestore(ptr, code);
  }

  return FALSE;
}

// 浮動小数点命令(既定でコプロセッサID=1)
boolean disfp(codeptr ptr, disasm* code) {
  UWORD word1 = peekw(ptr);

  if (decode(ptr, code, word1)) {
    code->mputypes &= ~(M000 | M010);
    code->fpuid = (word1 >> 9) & 7;
    return TRUE;
  }

  return FALSE;
}

// EOF
