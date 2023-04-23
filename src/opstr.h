// ソースコードジェネレータ
// オペコード文字列 ヘッダ
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

#ifndef OPSTR_H
#define OPSTR_H

#include <stddef.h>
#include <string.h>

#include "estruct.h"

// DOSコール、IOCSコール、FEFUNCコール、SXコールのマクロ名(+NUL)の最大長
#define MAX_MACRO_LEN 16

// FPUコンディションコード
//   FP Predicateは6ビットだが、定義済みは0～31だけで32～63は未定義
enum {
  FPCOND_F = 0,
  FPCOND_T = 15,
  FPCOND_NUM = 32,
};

// FPU命令(一般命令)
//   OpStringTable::fpuType000[] 参照用の通し番号(オペコードとは異なる)
enum {
  FPU_T0_FMOVE,
  FPU_T0_FINT,
  FPU_T0_FSINH,
  FPU_T0_FINTRZ,
  FPU_T0_FSQRT,
  FPU_T0_FLOGNP1,
  FPU_T0_FETOXM1,
  FPU_T0_FTANH,
  FPU_T0_FATAN,
  FPU_T0_FASIN,
  FPU_T0_FATANH,
  FPU_T0_FSIN,
  FPU_T0_FTAN,
  FPU_T0_FETOX,
  FPU_T0_FTWOTOX,
  FPU_T0_FTENTOX,
  FPU_T0_FLOGN,
  FPU_T0_FLOG10,
  FPU_T0_FLOG2,
  FPU_T0_FABS,
  FPU_T0_FCOSH,
  FPU_T0_FNEG,
  FPU_T0_FACOS,
  FPU_T0_FCOS,
  FPU_T0_FGETEXP,
  FPU_T0_FGETMAN,
  FPU_T0_FDIV,
  FPU_T0_FMOD,
  FPU_T0_FADD,
  FPU_T0_FMUL,
  FPU_T0_FSGLDIV,
  FPU_T0_FREM,
  FPU_T0_FSCALE,
  FPU_T0_FSGLMUL,
  FPU_T0_FSUB,
  FPU_T0_FSINCOS,
  FPU_T0_FCMP,
  FPU_T0_FTST,
  FPU_T0_FSMOVE,
  FPU_T0_FSSQRT,
  FPU_T0_FDMOVE,
  FPU_T0_FDSQRT,
  FPU_T0_FSABS,
  FPU_T0_FSNEG,
  FPU_T0_FDABS,
  FPU_T0_FDNEG,
  FPU_T0_FSDIV,
  FPU_T0_FSADD,
  FPU_T0_FSMUL,
  FPU_T0_FDDIV,
  FPU_T0_FDADD,
  FPU_T0_FDMUL,
  FPU_T0_FSSUB,
  FPU_T0_FDSUB,
  FPU_T0_NUM
};

typedef struct {
  // 疑似命令
  char dc[PACKEDSIZE + 1][8];
  char dcb[PACKEDSIZE + 1][12];
  char ds[PACKEDSIZE + 1][8];

  char include_[12];
  char cpu[8];
  char fpid[8];
  char equ[8];
  char xdef[8];

  char text[8];
  char data[8];
  char bss[6];
  char stack[8];
  char end[6];

  char even[8];
  char r68Align[8];

#ifdef OSKDIS
  char psect[8];
  char vsect[8];
  char vsect_remote[14];
  char ends[6];

  char tcall[6];
  char os9[4];
#endif

  char dcWord[4];  // タブ、サイズなし

  // MPU命令
  char bcc[16][4];
  char dbcc[16][8];
  char scc[16][4];
  char trapcc[16][8];

  char bit[4][8];
  char bitfield[8][8];
  char cache[8][8];
  char pflush[4][10];
  char plpa[2][8];
  char ptest[2][8];
  char shift[8][8];
  char tbl[4][8];

  char dbf[4];

  char abcd[6];
  char add[4];
  char adda[6];
  char addi[6];
  char addiAsAdd[6];
  char addq[6];
  char addx[6];
  char and[4];
  char andi[6];
  char andiAsAnd[6];
  char bgnd[6];
  char bkpt[6];
  char callm[6];
  char cas[4];
  char cas2[6];
  char chk[4];
  char chk2[6];
  char clr[4];
  char cmp[4];
  char cmp2[6];
  char cmpa[6];
  char cmpi[6];
  char cmpiAsCmp[6];
  char cmpm[6];
  char divs[6];
  char divsl[6];
  char divu[6];
  char divul[6];
  char eor[4];
  char eori[6];
  char exg[4];
  char ext[4];
  char extb[6];
  char illegal[8];
  char jmp[4];
  char jsr[4];
  char lea[4];
  char link[6];
  char lpstop[8];
  char move[6];
  char move16[8];
  char movea[6];
  char movec[6];
  char movem[6];
  char movep[6];
  char moveq[6];
  char moves[6];
  char muls[6];
  char mulsl[6];
  char mulu[6];
  char mulul[6];
  char nbcd[6];
  char neg[4];
  char negx[6];
  char nop[4];
  char not [4];
  char or [4];
  char ori[4];
  char oriAsOr[4];
  char pack[6];
  char pea[4];
  char reset[6];
  char rtd[4];
  char rte[4];
  char rtm[4];
  char rtr[4];
  char rts[4];
  char sbcd[6];
  char stop[6];
  char sub[4];
  char suba[6];
  char subi[6];
  char subiAsSub[6];
  char subq[6];
  char subx[6];
  char swap[6];
  char tas[4];
  char trap[6];
  char trapv[6];
  char tst[4];
  char unlk[6];
  char unpk[6];

  // FPU命令
  char fdbcc[FPCOND_NUM][8];
  char ftrapcc[FPCOND_NUM][10];
  char fscc[FPCOND_NUM][8];
  char fbcc[FPCOND_NUM][8];

  char fpuType000[FPU_T0_NUM][8];

  char fdbf[6];

  char fmovecr[8];
  char fmovem[8];
  char fnop[6];
  char fsave[6];
  char frestore[10];

  // MMU命令
  char pdbcc[16][8];
  char ptrapcc[16][8];
  char pscc[16][8];
  char pbcc[16][8];

  char pload[2][8];

  char pvalid[8];
  char pmove[8];
  char pmovefd[8];
  char pflusha[8];
  char pflushs[8];
  char pflushr[8];
  char psave[8];
  char prestore[12];

  // ファンクションコール(マクロ)
  char doscall[MAX_MACRO_LEN];
  char iocscall[MAX_MACRO_LEN];
  char fefunc[MAX_MACRO_LEN];
  char sxcall[MAX_MACRO_LEN];

} OpStringTable;

extern OpStringTable OpString;

extern void initOpString(void);

// オペコード文字列をセットする
//   disasm* code が宣言されているスコープで使用すること。
#define OPECODE(member)                                          \
  do {                                                           \
    code->opecodeOffset = (WORD)offsetof(OpStringTable, member); \
  } while (0)

// オペコード文字列をセットする(cond ? m1 : m2)
//   cond が真の場合は m1 を、偽の場合は m2 をセットする。
#define OPECODE2(cond, m1, m2)                                          \
  do {                                                                  \
    code->opecodeOffset = (WORD)((cond) ? offsetof(OpStringTable, m1)   \
                                        : offsetof(OpStringTable, m2)); \
  } while (0)

// オペコード文字列を取得する
#define GET_OPECODE(code) ((char *)&OpString + (code)->opecodeOffset)

// オペコード文字列のオフセット値を取得する
#define GET_OPECODE_OFFSET(member) offsetof(OpStringTable, member)

#endif

// EOF
