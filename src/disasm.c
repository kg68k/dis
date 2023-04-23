// ソースコードジェネレータ
// 逆アセンブルモジュール
// Copyright (C) 1989,1990 K.Abe, 1994 R.ShimiZu
// All rights reserved.
// Copyright (C) 1997-2023 TcbnErik

#include "disasm.h"

#include <string.h>

#include "ea.h"
#include "estruct.h"
#include "etc.h" /* peek[wl] */
#include "fpu.h"
#include "global.h"
#include "hex.h"
#include "mmu.h"
#include "opstr.h"
#include "osk.h"

#define BYTE1 (*(UBYTE*)ptr)
#define BYTE2 (*(UBYTE*)(ptr + 1))
#define BYTE3 (*(UBYTE*)(ptr + 2))
#define BYTE4 (*(UBYTE*)(ptr + 3))
#define WORD1 (peekw(ptr))
#define WORD2 (peekw(ptr + 2))
#define WORD3 (peekw(ptr + 4))
#define LONG05 (peekl(ptr + 2))

static boolean moveope(codeptr ptr, disasm* code, opesize size);
static boolean packope(codeptr ptr, disasm* code);
static boolean addsubope(codeptr ptr, disasm* code, boolean isAdd);
static boolean bcdope(codeptr ptr, disasm* code);

#define SX_WINDOW_EXIT 0xa352

#define DOS_EXIT 0x00
#define DOS_EXIT2 0x4c
#define DOS_KEEPPR 0x31
#define DOS_KILL_PR 0xf9

#define SETSIZE() (code->size = code->size2 = (WORD1 >> 6) & 3)

// アドレスレジスタ直接でバイトサイズなら未定義命令
static boolean reject_bytesize(disasm* code, UWORD word1) {
  return (is_areg_direct(word1) && code->size == BYTESIZE) ? TRUE : FALSE;
}

// 68060でISP無しなら未定義命令
static boolean reject_060_no_isp(disasm* code) {
  mputypes mpu = Dis.mpu;
  if ((mpu & MISP) == 0) {
    if ((mpu & ~M060) == 0) return TRUE;
    code->mputypes &= ~M060;
  }
  return FALSE;
}

// 内蔵MMUが無効なら未定義命令
//   mputype=M040, mmutype=MMU040 または mputype=M060, mmutype=MMU060
//   の組み合わせで呼び出すこと
static boolean reject_no_mmu(disasm* code, mputypes mputype, uint8_t mmutype) {
  if (Dis.mmu & mmutype) {
    code->mputypes = mputype;
    return FALSE;
  }
  return TRUE;
}

// 68040/68060のMMUが無効なら未定義命令
static boolean reject_no_mmu46(disasm* code) {
  mputypes m = 0;
  mmutypes mmu = Dis.mmu;
  if (mmu & MMU040) m |= M040;
  if (mmu & MMU060) m |= M060;
  if (m == 0) return TRUE;

  code->mputypes = m;
  return FALSE;
}

// and or (eor はアドレッシングモードが微妙に異なる)
static boolean logicope(codeptr ptr, disasm* code) {
  SETSIZE();
  if (BYTE1 & 1) {
    setDn(&code->op1, BYTE1 >> 1);
    return setEA(code, &code->op2, ptr, MEMORY & CHANGE);
  } else {
    if (!setEA(code, &code->op1, ptr, DATA)) return FALSE;
    setDn(&code->op2, BYTE1 >> 1);
  }
  return TRUE;
}

// mul, div
static boolean muldivope(codeptr ptr, disasm* code) {
  code->size = code->size2 = WORDSIZE;
  if (!setEA(code, &code->op1, ptr, DATA)) return FALSE;
  setDn(&code->op2, BYTE1 >> 1);
  return TRUE;
}

// ロングフォーム mul, div
static boolean longmuldivope(codeptr ptr, disasm* code) {
  code->size = code->size2 = LONGSIZE;
  code->bytes = 4;
  if (!setEA(code, &code->op1, ptr, DATA)) return FALSE;
  setPairDn(&code->op2, WORD2 & 7, (BYTE3 >> 4) & 7);
  return TRUE;
}

// cas, cas2
static boolean casope(codeptr ptr, disasm* code) {
  UWORD word1 = WORD1, word2 = WORD2;
  int size = (BYTE1 >> 1) & 3;

  if (reject(code, M000 | M010)) return FALSE;

  // casで使用されない<ea>にcas2を割り当てているので、casより先に調べる
  if ((word1 & 0xfdff) == 0x0cfc) {
    UWORD word3 = WORD3;

    if (((word2 | word3) & 0x0e38) != 0) return FALSE;
    if (reject_060_no_isp(code)) return FALSE;

    OPECODE(cas2);
    setPairDn(&code->op1, word2 & 7, word3 & 7);                // Dc1:Dc2
    setPairDn(&code->op2, (word2 >> 6) & 7, (word3 >> 6) & 7);  // Du1:Du2
    setPairID(&code->op3, (word2 >> 12) & 15,
              (word3 >> 12) & 15);  // (Rn1):(Rn2)
    code->bytes = 6;
    code->size = code->size2 = (size == 2) ? WORDSIZE : LONGSIZE;
    return TRUE;
  }

  if (size != 0 && (word2 & 0xfe38) == 0x0000) {
    opesize sz = (size == 1) ? BYTESIZE : (size == 2) ? WORDSIZE : LONGSIZE;

    OPECODE(cas);
    setDn(&code->op1, word2);       // Dc
    setDn(&code->op2, word2 >> 6);  // Du
    code->bytes = 4;
    code->size = code->size2 = sz;
    return setEA(code, &code->op3, ptr, DATA & CHANGE);
  }

  return FALSE;
}

// moves
static boolean movesope(codeptr ptr, disasm* code) {
  if ((WORD2 & 0x07ff) != 0) return FALSE;
  if (reject(code, M000)) return FALSE;

  OPECODE(moves);
  SETSIZE();
  code->bytes = 4;
  if (BYTE3 & 0x08) {
    setRn(&code->op1, BYTE3 >> 4);
    return setEA(code, &code->op2, ptr, CHANGE ^ DATAREG ^ ADRREG);
  } else {
    if (!setEA(code, &code->op1, ptr, CHANGE ^ DATAREG ^ ADRREG)) return FALSE;
    setRn(&code->op2, BYTE3 >> 4);
    return TRUE;
  }
}

static boolean oriope(codeptr ptr, disasm* code) {
  if (!setEA(code, &code->op2, ptr, SRCCR | (DATA & CHANGE))) return FALSE;
  // to ccr はバイトサイズ
  if (code->op2.ea == CcrSr) code->default_size = code->size;

  OPECODE2(code->op2.ea == DregD, ori, oriAsOr);
  return TRUE;
}

static boolean andiope(codeptr ptr, disasm* code) {
  if (!setEA(code, &code->op2, ptr, SRCCR | (DATA & CHANGE))) return FALSE;
  // to ccr はバイトサイズ
  if (code->op2.ea == CcrSr) code->default_size = code->size;

  OPECODE2(code->op2.ea == DregD, andi, andiAsAnd);
  return TRUE;
}

static boolean subiope(codeptr ptr, disasm* code) {
  if (!setEA(code, &code->op2, ptr, DATA & CHANGE)) return FALSE;

  OPECODE2(code->op2.ea == DregD, subi, subiAsSub);
  if (code->size == BYTESIZE) code->codeflags += CODEFLAG_NEED_COMMENT;

  // 1 <= imm <= 8 なら、最適化抑止のためサイズを付ける
  addsize_if_1to8(&code->op1, code->size);
  return TRUE;
}

static boolean addiope(codeptr ptr, disasm* code) {
  if (!setEA(code, &code->op2, ptr, DATA & CHANGE)) return FALSE;

  OPECODE2(code->op2.ea == DregD, addi, addiAsAdd);
  if (code->size == BYTESIZE) code->codeflags += CODEFLAG_NEED_COMMENT;

  // 1 <= imm <= 8 なら、最適化抑止のためサイズを付ける
  addsize_if_1to8(&code->op1, code->size);
  return TRUE;
}

static boolean eoriope(codeptr ptr, disasm* code) {
  if (!setEA(code, &code->op2, ptr, SRCCR | (DATA & CHANGE))) return FALSE;
  // to ccr はバイトサイズ
  if (code->op2.ea == CcrSr) code->default_size = code->size;

  OPECODE(eori);
  return TRUE;
}

static boolean cmpiope(codeptr ptr, disasm* code) {
  int mode = DATA & CHANGE;
  if (extract_ea_basic(WORD1) >= PCDISP && (Dis.mpu & ~(M000 | M010))) {
    // PC間接は68020以降専用
    code->mputypes &= ~(M000 | M010);
    mode = DATA ^ IMMEDIATE;
  }
  if (!setEA(code, &code->op2, ptr, mode)) return FALSE;

  OPECODE2(code->op2.ea == DregD, cmpi, cmpiAsCmp);
  code->codeflags += CODEFLAG_NEED_COMMENT;
  return TRUE;
}

static boolean logicaddsub(codeptr ptr, disasm* code) {
  SETSIZE();
  if (!setImm(code, &code->op1, ptr, code->size)) return FALSE;

  switch (BYTE1 >> 1) {
    default:
      break;
    case 0:
      return oriope(ptr, code);
    case 1:
      return andiope(ptr, code);
    case 2:
      return subiope(ptr, code);
    case 3:
      return addiope(ptr, code);
    case 5:
      return eoriope(ptr, code);
    case 6:
      return cmpiope(ptr, code);
  }
  return FALSE;
}

static boolean op00(codeptr ptr, disasm* code) {
  UWORD word1 = WORD1;

  /* movep */
  if ((word1 & 0x0138) == 0x0108) {
    if (reject_060_no_isp(code)) return FALSE;

    OPECODE(movep);
    code->size = code->size2 = (word1 & 0x40) ? LONGSIZE : WORDSIZE;
    if (word1 & 0x80) {
      setDn(&code->op1, BYTE1 >> 1);
      setAnDisp(code, &code->op2, word1 & 7, WORD2, FALSE);
    } else {
      setAnDisp(code, &code->op1, word1 & 7, WORD2, FALSE);
      setDn(&code->op2, BYTE1 >> 1);
    }
    return TRUE;
  }

  /* btst、bchg、bclr、bset */
  {
    /* ビット位置が immediate */
    if ((BYTE1 & 0x0f) == 8) {
      OPECODE(bit[BYTE2 >> 6]);
      code->size = code->default_size = (word1 & 0x38) ? BYTESIZE : LONGSIZE;
      code->size2 = UNKNOWN;
      if (!setImm(code, &code->op1, ptr, BYTESIZE)) return FALSE;
      return setEA(code, &code->op2, ptr, DATA ^ IMMEDIATE);
    }

    /* ビット位置が Dn */
    if (BYTE1 & 1) {
      OPECODE(bit[BYTE2 >> 6]);
      code->size = code->size2 = code->default_size =
          (word1 & 0x38) ? BYTESIZE : LONGSIZE;
      setDn(&code->op1, BYTE1 >> 1); /* btst 以外は可変 */
      return setEA(code, &code->op2, ptr,
                   (word1 & 0xc0) ? DATA & CHANGE : DATA);
    }

    /* どちらでもなければ他の命令 */
  }

  // ori, andi, subi, addi, eori, cmpi
  if (BYTE1 != 0x0e && BYTE2 < 0xc0) return logicaddsub(ptr, code);

  /* cmp2、chk2 */
  if ((word1 & 0x09c0) == 0x00c0 && BYTE1 != 6 && (WORD2 & 0x07ff) == 0x0000) {
    if (reject(code, M000 | M010) || reject_060_no_isp(code)) return FALSE;

    OPECODE2(WORD2 & 0x0800, chk2, cmp2);
    code->size = code->size2 = (BYTE1 >> 1) & 0x03;
    code->bytes = 4;
    if (!setEA(code, &code->op1, ptr, CONTROL)) return FALSE;
    setRn(&code->op2, BYTE3 >> 4);
    return TRUE;
  }

  // 0b0000_1ss0_11mm_mrrr: cas, cas2  ss=01,10,11
  if ((word1 & 0x09c0) == 0x08c0) {
    return casope(ptr, code);
  }

  // 0b0000_1110_ssmm_mrrr: moves  ss=00,01,10
  //   ss=11のケースはcas/cas2に該当するので、ここではチェック不要
  if ((word1 & 0x0f00) == 0x0e00) {
    return movesope(ptr, code);
  }

  /* 68020 の callm、rtm */
  if ((word1 & 0x0fc0) == 0x06c0) {
    if (reject(code, ~M020)) return FALSE;

    // callm で使えないモードに rtm が詰め込まれている
    if ((word1 & 0x0030) == 0) {
      OPECODE(rtm);
      setRn(&code->op1, word1);
      return TRUE;
    }

    // rtm でなければ callm
    if (BYTE3 != 0x00) {
      return FALSE;
    }
    OPECODE(callm);
    if (!setImm(code, &code->op1, ptr, BYTESIZE)) return FALSE;
    return setEA(code, &code->op2, ptr, CONTROL);
  }

  return FALSE;
}

static boolean op01(codeptr ptr, disasm* code) {
  return moveope(ptr, code, BYTESIZE);
}

static boolean op02(codeptr ptr, disasm* code) {
  return moveope(ptr, code, LONGSIZE);
}

static boolean op03(codeptr ptr, disasm* code) {
  return moveope(ptr, code, WORDSIZE);
}

static boolean moveope(codeptr ptr, disasm* code, opesize size) {
  UWORD word1 = WORD1;

  code->size = code->size2 = size;
  if (reject_bytesize(code, word1)) return FALSE;

  if (!setEA(code, &code->op1, ptr, ALL)) return FALSE;

  // movea
  if ((word1 & 0x01c0) == 0x0040) {
    if (code->size == BYTESIZE) return FALSE;

    setAn(&code->op2, BYTE1 >> 1);
    OPECODE(movea);
    // movea.* #imm にはコメントを付ける
    if (code->op1.ea == IMMED) code->codeflags += CODEFLAG_NEED_COMMENT;
    return TRUE;
  }

  if (!setEA(code, &code->op2, ptr, (DATA & CHANGE) | MOVEOPT)) return FALSE;

  OPECODE(move);  // フロー解析用に解析フェーズでも設定する

  if (Dis.needString) {
    // move.* #imm にはコメントを付ける
    if (code->op1.ea == IMMED) {
      code->codeflags += CODEFLAG_NEED_COMMENT;

      // move.l #imm,Dn が moveq.l #imm,Dn に化けないように...
      if ((word1 & 0x31ff) == 0x203c                  // move.l #imm,Dn か?
          && (LONG05 < 0x80 || 0xffffff80 <= LONG05)  // imm が範囲内か?
      )
        addsize(&code->op1, LONGSIZE);
    }
  }

  return TRUE;
}

static boolean resetope(disasm* code) {
  OPECODE(reset);
  return TRUE;
}

static boolean nopope(disasm* code) {
  OPECODE(nop);
  return TRUE;
}

// STOP #<data>  Unsized
static boolean stopope(codeptr ptr, disasm* code) {
  OPECODE(stop);
  return setImm(code, &code->op1, ptr, WORDSIZE);
}

static boolean rteope(disasm* code) {
  OPECODE(rte);
  code->opeType = RTSOP;
  return TRUE;
}

// RTD #<displacement>  Unsized
static boolean rtdope(codeptr ptr, disasm* code) {
  if (reject(code, M000)) return FALSE;

  OPECODE(rtd);
  code->opeType = RTSOP;
  return setImm(code, &code->op1, ptr, WORDSIZE);
}

static boolean rtsope(disasm* code) {
  OPECODE(rts);
  code->opeType = RTSOP;
  return TRUE;
}

static boolean trapvope(disasm* code) {
  OPECODE(trapv);
  return TRUE;
}

static boolean rtrope(disasm* code) {
  OPECODE(rtr);
  code->opeType = RTSOP;
  return TRUE;
}

static boolean movecope(codeptr ptr, disasm* code) {
  int dr = WORD1 & 0x0001;

  if (!setCtrlReg(code, dr ? &code->op2 : &code->op1, WORD2 & 0x0fff))
    return FALSE;

  OPECODE(movec);
  code->size = code->size2 = code->default_size = LONGSIZE;
  setRn(dr ? &code->op1 : &code->op2, BYTE3 >> 4);
  code->bytes += 2;
  return TRUE;
}

static boolean op4e7x(codeptr ptr, disasm* code) {
  switch (BYTE2 & 0x0f) {
    default:
      break;

    case 0x0:
      return resetope(code);
    case 0x1:
      return nopope(code);
    case 0x2:
      return stopope(ptr, code);
    case 0x3:
      return rteope(code);
    case 0x4:
      return rtdope(ptr, code);
    case 0x5:
      return rtsope(code);
    case 0x6:
      return trapvope(code);
    case 0x7:
      return rtrope(code);
    case 0xa:
    case 0xb:
      return movecope(ptr, code);
  }

  return FALSE;
}

static boolean op4x(codeptr ptr, disasm* code) {
  switch ((BYTE1 >> 1) & 3) {
    default:
      return FALSE;
    case 0:
      OPECODE(negx);
      break;
    case 1:
      OPECODE(clr);
      break;
    case 2:
      OPECODE(neg);
      break;
    case 3:
      OPECODE(not );
      break;
  }

  SETSIZE();
  return setEA(code, &code->op1, ptr, DATA & CHANGE);
}

// JMP, JSR
static boolean jmpjsrope(codeptr ptr, disasm* code) {
  if (!setEA(code, &code->op1, ptr, CONTROL)) return FALSE;

  code->jmp = code->op1.opval;
  code->jmpea = code->op1.ea;

  OPECODE(jsr);
  code->opeType = JSROP;
  if (BYTE2 & 0x40) {
    OPECODE(jmp);
    code->opeType = JMPOP;
  }

  return TRUE;
}

static boolean leaope(codeptr ptr, disasm* code) {
  OPECODE(lea);
  code->size = code->default_size = LONGSIZE;
  code->size2 = UNKNOWN;
  if (!setEA(code, &code->op1, ptr, CONTROL)) return FALSE;
  setAn(&code->op2, BYTE1 >> 1);

  return TRUE;
}

static boolean peaope(codeptr ptr, disasm* code) {
  OPECODE(pea);
  code->size = code->default_size = LONGSIZE;
  code->size2 = UNKNOWN;
  if (!setEA(code, &code->op1, ptr, CONTROL)) return FALSE;

  return TRUE;
}

// TST
static boolean tstope(codeptr ptr, disasm* code) {
  int mode = (Dis.mpu & ~(M000 | M010)) ? ALL : DATA & CHANGE;
  adrmode ea;

  OPECODE(tst);
  SETSIZE();  // size が 0b11 なら tas
  if (!setEA(code, &code->op1, ptr, mode)) return FALSE;

  ea = code->op1.ea;
  if (ea == AregD || (PCDISP <= ea && ea <= IMMED)) {
    // アドレスレジスタ直接、PC相対、即値は68020以降専用
    // (full extension word formatはsetEA()内で処理している)
    code->mputypes &= ~(M000 | M010);
  }
  return TRUE;
}

static boolean op04(codeptr ptr, disasm* code) {
  if ((WORD1 & 0xffc0) == 0x4c00) {
    switch (WORD2 & 0x8ff8) {
      case 0x0000:
        if (reject(code, M000 | M010)) return FALSE;

        if (Dis.undefReg  // 未定義フィールドが 0 or Dl と同じなら正常
            && (WORD2 & 7) != 0 && (WORD2 & 7) != ((BYTE3 >> 4) & 7)) {
          return FALSE;
        }

        OPECODE(mulu);
        code->bytes = 4;
        code->size = code->size2 = LONGSIZE;
        if (!setEA(code, &code->op1, ptr, DATA)) return FALSE;
        setDn(&code->op2, BYTE3 >> 4);
        return TRUE;
      case 0x0400:
        if (reject(code, M000 | M010) || reject_060_no_isp(code)) return FALSE;

        OPECODE(mulu);
        return longmuldivope(ptr, code);
      case 0x0800:
        if (reject(code, M000 | M010)) return FALSE;

        if (Dis.undefReg  // 未定義フィールドが 0 or Dl と同じなら正常
            && (WORD2 & 7) != 0 && (WORD2 & 7) != ((BYTE3 >> 4) & 7)) {
          return FALSE;
        }

        OPECODE(muls);
        code->bytes = 4;
        code->size = code->size2 = LONGSIZE;
        if (!setEA(code, &code->op1, ptr, DATA)) return FALSE;
        setDn(&code->op2, BYTE3 >> 4);
        return TRUE;
      case 0x0c00:
        if (reject(code, M000 | M010) || reject_060_no_isp(code)) return FALSE;

        OPECODE(muls);
        return longmuldivope(ptr, code);
      default:
        break;
    }
  }

  if ((WORD1 & 0xffc0) == 0x4c40) {
    switch (WORD2 & 0x8cf8) {
      case 0x0000:
        if (reject(code, M000 | M010)) return FALSE;

        if ((BYTE3 >> 4) == BYTE4) {
          OPECODE(divu);
          code->bytes = 4;
          code->size = code->size2 = LONGSIZE;
          if (!setEA(code, &code->op1, ptr, DATA)) return FALSE;
          setDn(&code->op2, WORD2);
          return TRUE;
        } else {
          OPECODE(divul);
          return longmuldivope(ptr, code);
        }
      case 0x0400:
        if (reject(code, M000 | M010) || reject_060_no_isp(code)) return FALSE;

        OPECODE(divu);
        return longmuldivope(ptr, code);
      case 0x0800:
        if (reject(code, M000 | M010)) return FALSE;

        if ((BYTE3 >> 4) == BYTE4) {
          OPECODE(divs);
          code->bytes = 4;
          code->size = code->size2 = LONGSIZE;
          if (!setEA(code, &code->op1, ptr, DATA)) return FALSE;
          setDn(&code->op2, WORD2);
          return TRUE;
        } else {
          OPECODE(divsl);
          return longmuldivope(ptr, code);
        }
      case 0x0c00:
        if (reject(code, M000 | M010) || reject_060_no_isp(code)) return FALSE;

        OPECODE(divs);
        return longmuldivope(ptr, code);
      default:
        break;
    }
  }

  if ((WORD1 & 0x0ff8) == 0x9c0) {
    if (reject(code, M000 | M010)) return FALSE;

    OPECODE(extb);
    code->size = code->size2 = LONGSIZE;
    setDn(&code->op1, WORD1);
    return TRUE;
  }

  if ((WORD1 & 0x1c0) == 0x1c0) return leaope(ptr, code);

  if ((WORD1 & 0x140) == 0x100) {
    OPECODE(chk);
    if ((WORD1 & 0x80) == 0x80)
      code->size = code->size2 = WORDSIZE;
    else if ((WORD1 & 0x80) == 0x00) {
      if (reject(code, M000 | M010)) return FALSE;
      code->size = code->size2 = LONGSIZE;
    } else {
      return FALSE;
    }
    if (!setEA(code, &code->op1, ptr, DATA)) return FALSE;
    setDn(&code->op2, BYTE1 >> 1);
    return TRUE;
  }

  if (BYTE1 == 0x4e) {
    if ((BYTE2 & 0xf0) == 0x70) return op4e7x(ptr, code);

    if ((WORD1 & 0xf0) == 0x40) {
      void (*decodeTrap)(codeptr ptr, disasm * code) = Dis.actions->decodeTrap;

      if (decodeTrap != NULL)
        decodeTrap(ptr, code);
      else {
        OPECODE(trap);
        setImmEmbed(&code->op1, WORD1 & 15);
      }
      return TRUE;
    }

    if ((WORD1 & 0x00f8) == 0x0050) {  // link.w
      OPECODE(link);
      code->size = WORDSIZE;
      setAn(&code->op1, WORD1);
      setImmSignedWord(code, &code->op2, ptr);
      return TRUE;
    }
    if ((WORD1 & 0xf8) == 0x58) {
      OPECODE(unlk);
      setAn(&code->op1, WORD1);
      return TRUE;
    }

    if ((WORD1 & 0xff80) == 0x4e80) return jmpjsrope(ptr, code);  // JMP, JSR

    /* move to/from usp */
    if ((WORD1 & 0xf0) == 0x60) {
      OPECODE(move);
      code->size = code->size2 = code->default_size = LONGSIZE;
      if (WORD1 & 0x0008) {
        setUsp(&code->op1);
        setAn(&code->op2, WORD1);
      } else {
        setAn(&code->op1, WORD1);
        setUsp(&code->op2);
      }
      return TRUE;
    }
  }

  if ((WORD1 & 0xfb8) == 0x880) {
    OPECODE(ext);
    code->size = code->size2 = (WORD1 & 0x40 ? LONGSIZE : WORDSIZE);
    setDn(&code->op1, WORD1);
    return TRUE;
  }

  if ((WORD1 & 0xb80) == 0x880) {
    if (WORD2 == 0) { /* register field empty ? */
      return FALSE;
    }
    OPECODE(movem);
    code->size = code->size2 = (WORD1 & 0x40 ? LONGSIZE : WORDSIZE);
    code->bytes = 4;
    if (BYTE1 & 4) {
      if (!setEA(code, &code->op1, ptr, CONTROL | POSTINC)) return FALSE;
      setReglist(&code->op2, WORD2, is_predecrement(WORD1));
    } else {
      setReglist(&code->op1, WORD2, is_predecrement(WORD1));
      return setEA(code, &code->op2, ptr, CTRLCHG | PREDEC);
    }
    return TRUE;
  }

  switch (WORD1 & 0xff8) {
    case 0x840:
      OPECODE(swap);
      code->size = code->size2 = WORDSIZE;
      setDn(&code->op1, WORD1);
      return TRUE;
    case 0x848:
      if (reject(code, M000)) return FALSE;

      OPECODE(bkpt);
      setImmEmbed(&code->op1, WORD1 & 7);
      return TRUE;
    case 0x808:  // link.l
      if (reject(code, M000 | M010)) return FALSE;

      OPECODE(link);
      code->size = LONGSIZE;
      setAn(&code->op1, WORD1);
      setImmSignedLong(code, &code->op2, ptr);
      return TRUE;
  }

  switch (WORD1 & 0xfc0) {
    case 0x4c0:
      OPECODE(move);  // move to ccr
      code->size = code->size2 = WORDSIZE;
      if (!setEA(code, &code->op1, ptr, DATA)) return FALSE;
      setCcr(&code->op2);
      return TRUE;
    case 0x6c0:
      OPECODE(move);  // move to sr
      code->size = code->size2 = WORDSIZE;
      if (!setEA(code, &code->op1, ptr, DATA)) return FALSE;
      setSr(&code->op2);
      return TRUE;
    case 0x0c0:
      OPECODE(move);  // move from sr
      code->size = code->size2 = WORDSIZE;
      setSr(&code->op1);
      return setEA(code, &code->op2, ptr, DATA);
    case 0x2c0:  // move from ccr
      if (reject(code, M000)) return FALSE;

      OPECODE(move);
      code->size = code->size2 = WORDSIZE;
      setCcr(&code->op1);
      return setEA(code, &code->op2, ptr, DATA);
    case 0x840:
      return peaope(ptr, code);
    case 0xac0:
      if (WORD1 == 0x4afc) {
        OPECODE(illegal);
        return TRUE;
      } else if (WORD1 == 0x4afa && Dis.cpu32) {
        OPECODE(bgnd);
        return TRUE;
      }
      OPECODE(tas);
      code->size = code->size2 = code->default_size = BYTESIZE;
      return setEA(code, &code->op1, ptr, DATA & CHANGE);
    case 0x800:
      OPECODE(nbcd);
      code->size = code->size2 = code->default_size = BYTESIZE;
      return setEA(code, &code->op1, ptr, DATA & CHANGE);
  }

  if (BYTE1 == 0x4a) return tstope(ptr, code);

  if ((BYTE1 & 1) == 0) {
    // negx, clr, neg, not
    if (BYTE1 <= 0x46) return op4x(ptr, code);
  }

  return FALSE;
}

static boolean op05(codeptr ptr, disasm* code) {
  /* DBcc */
  if ((BYTE2 >> 3) == 0x19) {
    OPECODE(dbcc[BYTE1 & 0x0f]);
    setDn(&code->op1, WORD1);
    setrelative(code, &code->op2, extl(WORD2));
    code->jmp = code->op2.opval;
    code->jmpea = code->op2.ea = PCDISP;
    code->op2.labelchange1 = LABELCHANGE_LABEL_ONLY;

    code->size = Dis.dbccSize;
    code->size2 = WORDSIZE;

    code->bytes = 4;
    code->opeType = BCCOP;
    return TRUE;
  }

  /* 68020 以降の trapcc */
  if ((WORD1 & 0xf8) == 0xf8 && (0xfa <= BYTE2) && (BYTE2 <= 0xfc)) {
    if (reject(code, M000 | M010)) return FALSE;

    OPECODE(trapcc[BYTE1 & 0x0f]);
    if (BYTE2 == 0xfa)
      return setImm(code, &code->op1, ptr, code->size = WORDSIZE);
    else if (BYTE2 == 0xfb)
      return setImm(code, &code->op1, ptr, code->size = LONGSIZE);
    return TRUE;
  }

  /* Scc */
  if ((WORD1 & 0xc0) == 0xc0) {
    OPECODE(scc[BYTE1 & 0x0f]);
    code->size = code->size2 = code->default_size = BYTESIZE;
    return setEA(code, &code->op1, ptr, DATA & CHANGE);
  }

  // addq, subq
  SETSIZE();
  if (reject_bytesize(code, WORD1)) return FALSE;

  OPECODE2(BYTE1 & 1, subq, addq);
  setImm18(&code->op1, BYTE1 >> 1);
  return setEA(code, &code->op2, ptr, CHANGE);
}

static boolean op06(codeptr ptr, disasm* code) {
  UBYTE d8 = BYTE2;
  int cc = BYTE1 & 0x0f;

  OPECODE(bcc[cc]);
  switch (cc) {
    case 0:  // BRA
      code->codeflags += CODEFLAG_BRAOP;
      code->opeType = JMPOP;
      break;
    case 1:  // BSR
      code->opeType = JSROP;
      break;
    default:  // Bcc
      code->opeType = BCCOP;
      break;
  }

  if (d8 == 0x00) {
    WORD d16 = WORD2;
    code->bytes = 4;
    code->size = code->size2 = WORDSIZE;
    setrelative(code, &code->op1, extl(d16));
    if (-128 <= d16 && d16 <= 128) code->codeflags += CODEFLAG_NEED_OPESIZE;
  } else if (d8 == 0xff && (Dis.mpu & ~(M000 | M010))) {
    LONG d32 = LONG05;
    code->mputypes &= ~(M000 | M010);
    code->bytes = 6;
    code->size = code->size2 = LONGSIZE;
    setrelative(code, &code->op1, d32);
    if (-32768 <= d32 && d32 <= 32768) code->codeflags += CODEFLAG_NEED_OPESIZE;
  } else {
    code->size = code->size2 = SHORTSIZE;
    setrelative(code, &code->op1, extbl(d8));
  }
  code->default_size = NOTHING;
  code->jmp = code->op1.opval;
  code->jmpea = code->op1.ea = PCDISP;
  code->op1.labelchange1 = LABELCHANGE_LABEL_ONLY;

  return TRUE;
}

static boolean op07(codeptr ptr, disasm* code) {
  if (BYTE1 & 1) return FALSE;

  code->size = code->default_size = LONGSIZE;
  code->size2 = UNKNOWN;
  if (Dis.needString) {
    OPECODE(moveq);
    // moveq も -M でコメントを付ける
    code->codeflags += CODEFLAG_NEED_COMMENT;
  }
  setImmEmbedHex(&code->op1, BYTE2);
  setDn(&code->op2, BYTE1 >> 1);

  return TRUE;
}

static boolean op08(codeptr ptr, disasm* code) {
  UWORD word1 = WORD1;
  UWORD w = word1 & 0x01f0;

  if (w == 0x0100) {  // 0b1000_yyy1_0000_rxxx
    OPECODE(sbcd);
    return bcdope(ptr, code);
  }

  if (w == 0x0140) {  // 0b1000_yyy1_0100_rxxx
    OPECODE(pack);
    return packope(ptr, code);
  }
  if (w == 0x0180) {  // 0b1000_yyy1_1000_rxxx
    OPECODE(unpk);
    return packope(ptr, code);
  }

  if ((word1 & 0x00c0) == 0x00c0) {
    if (word1 & 0x0100)
      OPECODE(divs);  // 0b1000_rrr1_11mm_mrrr
    else
      OPECODE(divu);  // 0b1000_rrr0_11mm_mrrr
    return muldivope(ptr, code);
  }

  // 0b1000_dddo_oomm_mrrr
  OPECODE(or);
  return logicope(ptr, code);
}

// pack, unpack
static boolean packope(codeptr ptr, disasm* code) {
  int reg_y, reg_x;

  if (reject(code, M000 | M010)) return FALSE;

  reg_y = (BYTE1 >> 1) & 7;
  reg_x = WORD1 & 7;
  if (WORD1 & 0x0008) {
    setAnPredec(&code->op1, reg_x);
    setAnPredec(&code->op2, reg_y);
  } else {
    setDn(&code->op1, reg_x);
    setDn(&code->op2, reg_y);
  }
  if (!setImm(code, &code->op3, ptr, WORDSIZE)) return FALSE;

  // pack、unpk にはコメントを付ける
  code->codeflags += CODEFLAG_NEED_COMMENT;

  return TRUE;
}

static boolean op09(codeptr ptr, disasm* code) {
  return addsubope(ptr, code, FALSE);
}

static boolean op0a(codeptr ptr, disasm* code) {
  const char* label = Dis.sxLabel ? Dis.sxLabel[WORD1 & 0xfff] : NULL;

  if (label) {
    if (Dis.needString) {
      OPECODE(sxcall);
      strcpy(code->op1.operand, label);
    }
    if (WORD1 == SX_WINDOW_EXIT) {
      code->opeType = RTSOP;
    }
    code->size = code->size2 = code->default_size = NOTHING;
    return TRUE;
  }

  if (Dis.acceptUnusedTrap) {
    // 未使用の A-line を未定義命令と見なさない
    if (Dis.needString) {
      OPECODE(dcWord);
      itox4d(code->op1.operand, WORD1);
    }
    code->size = code->size2 = code->default_size = WORDSIZE;
    return TRUE;
  }

  return FALSE;
}

static boolean op0b(codeptr ptr, disasm* code) {
  // CMPA
  if ((WORD1 & 0xc0) == 0xc0) {
    code->size = code->size2 = (BYTE1 & 1) ? LONGSIZE : WORDSIZE;
    if (!setEA(code, &code->op1, ptr, ALL)) return FALSE;
    setAn(&code->op2, BYTE1 >> 1);
    OPECODE(cmpa);

    // cmpa.* #imm にはコメントを付ける
    if (code->op1.ea == IMMED) code->codeflags += CODEFLAG_NEED_COMMENT;

    return TRUE;
  }

  // CMPM
  if ((WORD1 & 0x138) == 0x108) {
    SETSIZE();
    OPECODE(cmpm);
    setAnPostinc(&code->op1, WORD1 & 7);
    setAnPostinc(&code->op2, (BYTE1 >> 1) & 7);
    return TRUE;
  }

  // CMP
  if ((BYTE1 & 1) == 0) {
    SETSIZE();
    if (reject_bytesize(code, WORD1)) return FALSE;

    if (!setEA(code, &code->op1, ptr, ALL)) return FALSE;
    setDn(&code->op2, BYTE1 >> 1);
    OPECODE(cmp);

    // cmp.* #imm にはコメントを付ける
    if (code->op1.ea == IMMED) code->codeflags += CODEFLAG_NEED_COMMENT;

    return TRUE;
  }

  /* eor */
  if (BYTE1 & 1) {
    OPECODE(eor);
    SETSIZE();
    setDn(&code->op1, BYTE1 >> 1);
    return setEA(code, &code->op2, ptr, DATA & CHANGE);
  }

  return FALSE;
}

static boolean op0c(codeptr ptr, disasm* code) {
  UWORD word1 = WORD1;

  if ((word1 & 0x01f0) == 0x0100) {  // 0b1100_xxx1_0000_ryyy
    OPECODE(abcd);
    return bcdope(ptr, code);
  }

  // 0b1100_xxx1_oooo_oyyy
  {
    int tmp = word1 & 0x01f8;

    if (tmp == 0x140 || tmp == 0x148 || tmp == 0x188) {
      OPECODE(exg);
      code->size = code->size2 = code->default_size = LONGSIZE;
      switch (tmp) {
        case 0x140:
          setDn(&code->op1, BYTE1 >> 1);
          setDn(&code->op2, WORD1);
          break;
        case 0x148:
          setAn(&code->op1, BYTE1 >> 1);
          setAn(&code->op2, WORD1);
          break;
        case 0x188:
          setDn(&code->op1, BYTE1 >> 1);
          setAn(&code->op2, WORD1);
          break;
      }
      return TRUE;
    }
  }

  if ((word1 & 0x00c0) == 0x00c0) {
    if (word1 & 0x0100)
      OPECODE(muls);  // 0b1100_rrr1_11mm_mrrr
    else
      OPECODE(mulu);  // 0b1100_rrr0_11mm_mrrr
    return muldivope(ptr, code);
  }

  // 0b1100_dddo_oomm_mrrr
  OPECODE(and);
  return logicope(ptr, code);
}

static boolean op0d(codeptr ptr, disasm* code) {
  return addsubope(ptr, code, TRUE);
}

static boolean bitfieldope(codeptr ptr, disasm* code) {
  enum {
    BF_ONLY,
    BF_DREG,
    DREG_BF,
  };
  static const uint8_t addressing[] = {
      BF_ONLY,  // bftst
      BF_DREG,  // bfextu
      BF_ONLY,  // bfchg
      BF_DREG,  // bfexts
      BF_ONLY,  // bfclr
      BF_DREG,  // bfffo
      BF_ONLY,  // bfset
      DREG_BF,  // bfins
  };
  UWORD word2 = WORD2;

  if ((word2 & 0x8000) || reject(code, M000 | M010)) return FALSE;

  code->bytes = 4;
  OPECODE(bitfield[BYTE1 & 7]);

  switch (addressing[BYTE1 & 7]) {
    case BF_ONLY:  // bf... ea{m:n}
      if (!setEA(code, &code->op1, ptr,
                 DATAREG | ((BYTE1 & 7) ? CTRLCHG : CONTROL)))
        return FALSE;
      setBitField(&code->op2, word2);
      break;
    case BF_DREG:  // bf... ea{m:n},dn
      if (!setEA(code, &code->op1, ptr, CONTROL | DATAREG)) return FALSE;
      setBitField(&code->op2, word2);
      setDn(&code->op3, BYTE3 >> 4);
      break;
    case DREG_BF:  // bf... dn,ea{m:n}
      setDn(&code->op1, BYTE3 >> 4);
      if (!setEA(code, &code->op2, ptr, CTRLCHG | DATAREG)) return FALSE;
      setBitField(&code->op3, word2);
      break;
  }

  return TRUE;
}

static boolean op0e(codeptr ptr, disasm* code) {
  UWORD word1 = WORD1;

  if ((word1 & 0xf8c0) == 0xe8c0) return bitfieldope(ptr, code);

  // ビットフィールド命令でなければ、シフト・ローテート命令

  // op.W <ea>
  if ((word1 & 0x00c0) == 0x00c0) {
    OPECODE(shift[BYTE1 & 7]);
    code->size = code->size2 = WORDSIZE;
    return setEA(code, &code->op1, ptr, MEMORY & CHANGE);
  }

  SETSIZE();
  if (word1 & 0x0020)
    setDn(&code->op1, BYTE1 >> 1);  // op.* dm,dn
  else
    setImm18(&code->op1, BYTE1 >> 1);  // op.* #q,dn
  setDn(&code->op2, word1);
  OPECODE(shift[((word1 & 0x18) >> 2) + (BYTE1 & 1)]);
  return TRUE;
}

typedef enum {
  C_SCOPE_ILLEGAL = 0,
  C_SCOPE_LINE = 1,
  C_SCOPE_PAGE = 2,
  C_SCOPE_ALL = 3,
} CacheScope;

// 68040/68060 cinv, cpush命令
static boolean cinvpushope(codeptr ptr, disasm* code) {
  UWORD word1 = WORD1;
  int regno = word1 & 7;
  CacheScope scope = (CacheScope)((word1 >> 3) & 3);

  // cinva, cpushaはレジスタを使用しないので、普通はレジスタ番号が0になる
  if (scope == C_SCOPE_ILLEGAL ||
      (scope == C_SCOPE_ALL && regno != 0 && Dis.undefReg)) {
    return FALSE;
  }

  code->mputypes &= (M040 | M060);
  OPECODE(cache[(word1 >> 3) & 7]);
  setCacheKind(&code->op1, (word1 >> 6) & 3);
  if (scope != C_SCOPE_ALL) setAnIndirect(&code->op2, regno);
  return TRUE;
}

// 68040/68060 pflush, pflushn, pflusha, pflushan命令
static boolean pflush46ope(codeptr ptr, disasm* code) {
  UWORD word1 = WORD1;
  PflushOpMode opmode = (PflushOpMode)((word1 >> 3) & 3);
  int regno = word1 & 7;

  if (reject_no_mmu46(code) || (word1 & 0x0020) ||
      (opmode >= PFOPMODE_PFLUSHAN && regno != 0 && Dis.undefReg)) {
    return FALSE;
  }

  OPECODE(pflush[opmode]);
  if (opmode <= PFOPMODE_PFLUSH) setAnIndirect(&code->op1, regno);
  return TRUE;
}

// 68040 ptestr, ptestw命令
static boolean ptest040ope(codeptr ptr, disasm* code) {
  UWORD word1 = WORD1;

  if ((word1 & 0x0018) != 0x0008 || reject_no_mmu(code, M040, MMU040))
    return FALSE;

  OPECODE(ptest[(word1 >> 5) & 1]);
  setAnIndirect(&code->op1, word1 & 7);
  return TRUE;
}

// 68060 plpaw, plpar命令
static boolean plpaope(codeptr ptr, disasm* code) {
  UWORD word1 = WORD1;

  if ((word1 & 0x0038) != 0x0008 || reject_no_mmu(code, M060, MMU060))
    return FALSE;

  OPECODE(plpa[(word1 >> 6) & 1]);
  setAnIndirect(&code->op1, word1 & 7);
  return TRUE;
}

// 68040/68060の拡張命令(コプロセッサID=2)
static boolean cp010(codeptr ptr, disasm* code) {
  int optype = (WORD1 >> 6) & 7;

  switch (optype) {
    case 0:
    case 1:
    case 2:
    case 3:
      return cinvpushope(ptr, code);
    case 4:
      return pflush46ope(ptr, code);
    case 5:
      return ptest040ope(ptr, code);
    case 6:
    case 7:
      return plpaope(ptr, code);
  }

  return FALSE;
}

static boolean move16ope(codeptr ptr, disasm* code) {
  UWORD word1 = WORD1;

  if (word1 & 0x0020) {
    if ((word1 & 0x0018) != 0x0000 || (WORD2 & 0x8fff) != 0x8000) {
      return FALSE;
    }

    // (Ax)+,(Ay)+
    OPECODE(move16);
    code->mputypes &= (M040 | M060);
    setAnPostinc(&code->op1, word1 & 7);
    setAnPostinc(&code->op2, (BYTE3 >> 4) & 7);
    code->bytes = 4;
    return TRUE;
  }

  OPECODE(move16);
  code->mputypes &= (M040 | M060);

  switch ((word1 >> 3) & 3) {
    case 0:  // (Ay)+,(xxx).L
      setAnPostinc(&code->op1, WORD1 & 7);
      setAbsLong(code, &code->op2, ptr, TRUE);
      break;
    case 1:  // (xxx).L,(Ay)+
      setAbsLong(code, &code->op1, ptr, TRUE);
      setAnPostinc(&code->op2, WORD1 & 7);
      break;
    case 2:  // (Ay),(xxx).L
      setAnIndirect(&code->op1, WORD1 & 7);
      setAbsLong(code, &code->op2, ptr, TRUE);
      break;
    case 3:  // (xxx).L,(Ay)
      setAbsLong(code, &code->op1, ptr, TRUE);
      setAnIndirect(&code->op2, WORD1 & 7);
      break;
  }
  return TRUE;
}

// 68040/68060の拡張命令(コプロセッサID=3)
static boolean cp011(codeptr ptr, disasm* code) {
  int optype = (WORD1 >> 6) & 7;

  if (optype == 0) {
    return move16ope(ptr, code);
  }

  return FALSE;
}

// CPU32/68060 LPSTOP
static boolean lpstopope(codeptr ptr, disasm* code) {
  if (Dis.mpu & M060) {
    code->mputypes = M060;  // -m68060,cpu32なら.cpu 68060にする
  } else if (!Dis.cpu32) {
    return FALSE;
  }

  OPECODE(lpstop);
  code->bytes = 4;
  code->size = WORDSIZE;
  return setImm(code, &code->op1, ptr, WORDSIZE);
}

// CPU32 TBLS, TBLSN, TBLU, TBLUN
static boolean tblope(codeptr ptr, disasm* code) {
  if (!Dis.cpu32) return FALSE;

  OPECODE(tbl[(BYTE3 >> 2) & 3]);
  code->bytes = 4;
  code->size = code->size2 = (BYTE4 >> 6);

  if (BYTE3 & 1) {  // <ea>,dx
    if (!setEA(code, &code->op1, ptr, CONTROL)) return FALSE;
  } else {  // dym:dyn,dx
    setPairDn(&code->op1, WORD1 & 7, BYTE4 & 7);
  }
  setDn(&code->op2, BYTE3 >> 4);
  return TRUE;
}

// CPU32/68060の拡張命令(コプロセッサID=4)
static boolean cp100(codeptr ptr, disasm* code) {
  UWORD word1 = WORD1, word2 = WORD2;

  if (word1 == 0xf800 && word2 == 0x01c0) return lpstopope(ptr, code);

  if ((word1 & 0xffc0) == 0xf800 && (word2 & 0x8238) == 0 &&
      ((word2 >> 6) & 3) != 3) {
    return tblope(ptr, code);
  }

  return FALSE;
}

// F line Emulator ($ffxx:DOSコール、$fexx:FPACKコール)
static boolean flineHuman(codeptr ptr, disasm* code) {
  switch (BYTE1) {
    default:
      break;

    case 0xff:
      if (Dis.dosLabel && Dis.dosLabel[BYTE2]) {
        if (BYTE2 == DOS_EXIT || BYTE2 == DOS_EXIT2 || BYTE2 == DOS_KEEPPR ||
            BYTE2 == DOS_KILL_PR) {
          code->opeType = RTSOP;
        }
        if (Dis.needString) {
          OPECODE(doscall);
          strcpy(code->op1.operand, Dis.dosLabel[BYTE2]);
        }
        code->size = code->size2 = code->default_size = NOTHING;
        return TRUE;
      }
      break;

    case 0xfe:
      if (Dis.fefuncLabel && Dis.fefuncLabel[BYTE2]) {
        if (Dis.needString) {
          OPECODE(fefunc);
          strcpy(code->op1.operand, Dis.fefuncLabel[BYTE2]);
        }
        code->size = code->size2 = code->default_size = NOTHING;
        return TRUE;
      }
      break;
  }

  return FALSE;
}

static boolean op0f(codeptr ptr, disasm* code) {
  int cpid = (BYTE1 >> 1) & 7;

  // コプロセッサ命令
  switch (cpid) {
    default:
      break;

    case CPID_MMU:
      if (Dis.mmu & (MMU851 | MMU030)) return cp000(ptr, code);
      break;
    case CPID_CACHE_MMU:
      if (Dis.mpu & (M040 | M060)) return cp010(ptr, code);
      break;
    case CPID_MOVE16:
      if (Dis.mpu & (M040 | M060)) return cp011(ptr, code);
      break;
    case CPID_CPU32:
      if ((Dis.mpu & M060) || Dis.cpu32) return cp100(ptr, code);
      break;
  }
  if (Dis.fpu && Dis.fpuidTable[cpid]) {
    return disfp(ptr, code);  // 浮動小数点命令
  }

  // DOSCALL, FEFUNC
  if (flineHuman(ptr, code)) return TRUE;

  if (Dis.acceptUnusedTrap) {
    // 未使用の F-line を未定義命令と見なさない
    if (Dis.needString) {
      OPECODE(dcWord);
      itox4d(code->op1.operand, WORD1);
    }
    code->size = code->size2 = code->default_size = WORDSIZE;
    return TRUE;
  }

  return FALSE;
}

// ADD, ADDA, ADDX, SUB, SUBA, SUBX
static boolean addsubope(codeptr ptr, disasm* code, boolean isAdd) {
  // ADDA, SUBA
  if ((WORD1 & 0xc0) == 0xc0) {
    code->size = code->size2 = (BYTE1 & 1) ? LONGSIZE : WORDSIZE;
    if (!setEA(code, &code->op1, ptr, ALL)) return FALSE;

    OPECODE2(isAdd, adda, suba);  // フロー解析用に解析フェーズでも設定する
    setAn(&code->op2, BYTE1 >> 1);

    if (Dis.needString) {
      // バイトサイズは存在しないのでコメントは不要

      // adda(suba) #imm,an (1 <= imm <= 8) を add(sub) #imm,an と出力するとき、
      // addq(subq) #imm,dn への最適化を抑止するため即値にサイズを付ける
      if (code->op1.ea == IMMED && Dis.mnemonicAbbreviation) {
        addsize_if_1to8(&code->op1, code->size);
      }
    }
    return TRUE;
  }

  // ADDX, SUBX
  if ((WORD1 & 0x130) == 0x100) {
    SETSIZE();
    OPECODE2(isAdd, addx, subx);
    if (WORD1 & 0x0008) {
      setAnPredec(&code->op1, WORD1 & 7);
      setAnPredec(&code->op2, (BYTE1 >> 1) & 7);
    } else {
      setDn(&code->op1, WORD1);
      setDn(&code->op2, BYTE1 >> 1);
    }
    return TRUE;
  }

  // ADD, SUB
  SETSIZE();
  OPECODE2(isAdd, add, sub);
  if (BYTE1 & 1) {
    // ADD(SUB) Dn,<ea>
    setDn(&code->op1, BYTE1 >> 1);
    return setEA(code, &code->op2, ptr, MEMORY & CHANGE);
  }
  // ADD(SUB) <ea>,Dn
  if (reject_bytesize(code, WORD1)) return FALSE;

  if (!setEA(code, &code->op1, ptr, ALL)) return FALSE;
  setDn(&code->op2, BYTE1 >> 1);

  if (Dis.needString && code->op1.ea == IMMED) {
    // add.b、sub.b #imm にはコメントを付ける
    if (code->size == BYTESIZE) code->codeflags += CODEFLAG_NEED_COMMENT;

    // add(sub) #imm,dn (1 <= imm <= 8) から addq(subq) #imm,dn
    // への最適化を抑止するため、即値にサイズを付ける
    addsize_if_1to8(&code->op1, code->size);
  }
  return TRUE;
}

// abcd, sbcd
static boolean bcdope(codeptr ptr, disasm* code) {
  code->size = code->size2 = code->default_size = BYTESIZE;
  if (WORD1 & 0x0008) {
    setAnPredec(&code->op1, WORD1 & 7);
    setAnPredec(&code->op2, (BYTE1 >> 1) & 7);
  } else {
    setDn(&code->op1, WORD1);
    setDn(&code->op2, BYTE1 >> 1);
  }

  return TRUE;
}

static void init_opval_eaadrs(operand* op) {
  op->opval = (address)-1;
  op->opval2 = (address)-1;
  op->eaadrs = (address)-1;
  op->eaadrs2 = (address)-1;
}

static void init_operand(operand* op) {
  op->operand[0] = '\0';
  op->ea = (adrmode)-1;
  init_opval_eaadrs(op);
  op->labelchange1 = LABELCHANGE_DISALLOW;
  op->flags = 0;
  op->exbd = -1;
  op->exod = -1;
  op->baseReg = 0;
  op->ixReg = 0;
}

// オペコードの最上位4ビットで分岐
static boolean decode(codeptr ptr, disasm* code) {
  switch (BYTE1 >> 4) {
    case 0x0:
      return op00(ptr, code);
    case 0x1:
      return op01(ptr, code);
    case 0x2:
      return op02(ptr, code);
    case 0x3:
      return op03(ptr, code);
    case 0x4:
      return op04(ptr, code);
    case 0x5:
      return op05(ptr, code);
    case 0x6:
      return op06(ptr, code);
    case 0x7:
      return op07(ptr, code);
    case 0x8:
      return op08(ptr, code);
    case 0x9:
      return op09(ptr, code);
    case 0xa:
      return op0a(ptr, code);
    case 0xb:
      return op0b(ptr, code);
    case 0xc:
      return op0c(ptr, code);
    case 0xd:
      return op0d(ptr, code);
    case 0xe:
      return op0e(ptr, code);
    case 0xf:
      return op0f(ptr, code);
  }

  return FALSE;
}

// moveq #imm,d0 + trap #15 を IOCS マクロに変更する
static void moveqTrap15ToIocs(codeptr ptr, disasm* code) {
  char* label = Dis.iocsLabel ? Dis.iocsLabel[BYTE2] : NULL;
  if (label == NULL) return;

  OPECODE(iocscall);
  init_operand(&code->op1);
  strcpy(code->op1.operand, label);
  code->op2.operand[0] = '\0';
  code->codeflags &= ~CODEFLAG_NEED_COMMENT;  // moveq のコメントは付けない
  code->bytes = 4;
  code->size = code->size2 = code->default_size = NOTHING;
}

// 逆アセンブルモジュール
//   引数
//     disp->ptr: 命令の格納アドレス
//     disp->pc: 命令のPC
//   戻り値
//     命令のバイト数(2以上の偶数)
//     disp->ptr: 次の命令の格納アドレス
//     disp->pc: 次の命令のPC
//     disp->code: 逆アセンブル結果
//
int dis(DisParam* disp) {
  disasm* code = &disp->code;
  codeptr ptr = disp->ptr;
  boolean success;

  code->opecodeOffset = 0;
  code->size2 = code->size = NOTHING;
  code->default_size = WORDSIZE;
  code->bytes = 2;
  code->opeType = OTHER;
  code->mputypes = ~0;
  code->fpuid = -1;
  code->codeflags = 0;
  code->pc = disp->pc;

  init_operand(&code->op1);
  init_operand(&code->op2);
  init_operand(&code->op3);
  init_operand(&code->op4);

  success = decode(ptr, code);
  if (success) {
    // moveq #xx,d0 + trap #15 なら IOCS マクロに変更する
    if (BYTE1 == 0x70 && WORD2 == 0x4e4f && Dis.needString &&
        ((disp->pc + 4) <= disp->pcEnd)) {
      moveqTrap15ToIocs(ptr, code);
    }

    // 命令列がプログラム終端を越えていたら未定義命令とする
    if ((code->bytes > 2) && (disp->pcEnd < (disp->pc + code->bytes))) {
      success = FALSE;
      init_opval_eaadrs(&code->op1);
      init_opval_eaadrs(&code->op2);
      init_opval_eaadrs(&code->op3);
      init_opval_eaadrs(&code->op4);
    }
  }

  if (!success) {
    code->opeType = UNDEF;
    code->bytes = 2;
    code->size = code->size2 = code->default_size = WORDSIZE;

    if (Dis.needString) {
      code->mputypes = ~0;
      code->fpuid = -1;

      code->op1.ea = code->op2.ea = code->op3.ea = code->op4.ea = (adrmode)-1;

      code->op1.labelchange1 = code->op2.labelchange1 = code->op3.labelchange1 =
          code->op4.labelchange1 = LABELCHANGE_DISALLOW;

      code->op2.operand[0] = code->op3.operand[0] = code->op4.operand[0] = '\0';

      OPECODE(dcWord);
      itox4d(code->op1.operand, WORD1);
    }
  }

  disp->ptr += code->bytes;
  disp->pc += code->bytes;
  return code->bytes;
}

// EOF
