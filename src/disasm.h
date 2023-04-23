// ソースコードジェネレータ
// 逆アセンブルモジュールヘッダ
// Copyright (C) 1989,1990 K.Abe, 1994 R.ShimiZu
// All rights reserved.
// Copyright (C) 1997-2023 TcbnErik

#ifndef DISASM_H
#define DISASM_H

#include "estruct.h"

// mputypes
#define M000 (1 << 0)  // 68000
#define M010 (1 << 1)  // 68010
#define M020 (1 << 2)  // 68020
#define M030 (1 << 3)  // 68030
#define M040 (1 << 4)  // 68040
#define M060 (1 << 6)  // 68060
#define MISP (1 << 7)  // 060ISP(software emulation)

// fputypes
#define F881 (1 << 0)  // 68881
#define F882 (1 << 1)  // 68882
#define F88x (F881 | F882)
#define F040 (1 << 2)  // 68040
#define F4SP (1 << 3)  // 040FPSP(software emulation)
#define F060 (1 << 4)  // 68060
#define F6SP (1 << 5)  // 060FPSP(software emulation)

// mmutypes (same bit position as mputypes)
#define MMU851 M020  // 68020 + 68851
#define MMU030 M030  // 68030 internal MMU
#define MMU040 M040  // 68040 internal MMU
#define MMU060 M060  // 68060 internal MMU

enum {
  CPID_MMU = 0,
  CPID_FPU = 1,
  CPID_CACHE_MMU = 2,
  CPID_MOVE16 = 3,
  CPID_CPU32 = 4,
};

// 68040以降のFFLUSH命令
typedef enum {
  PFOPMODE_PFLUSHN = 0,
  PFOPMODE_PFLUSH = 1,
  PFOPMODE_PFLUSHAN = 2,
  PFOPMODE_PFLUSHA = 3,
} PflushOpMode;

// FTRAPcc, PTRAPccのオペランドサイズ
enum {
  CPTRAP_WORD = 2,
  CPTRAP_LONG = 3,
  CPTRAP_UNSIZED = 4,
};

typedef uint8_t opetype;
enum {
  OTHER,       // 普通の命令
  JMPOP,       // 分岐命令
  JSROP,       // サブルーチンコール命令
  RTSOP,       // リターン命令
  BCCOP,       // 条件分岐命令
  UNDEF = 15,  // 未定義
};

typedef enum {
  DregD,     // データレジスタ直接
  AregD,     // アドレスレジスタ直接
  AregID,    // アドレスレジスタ間接
  AregIDPI,  // ポストインクリメントアドレスレジスタ間接
  AregIDPD,  // プリデクリメントアドレスレジスタ間接
  AregDISP,  // ディスプレースメント付アドレスレジスタ間接
  AregIDX,   // インデックス付アドレスレジスタ間接
  AbShort = 8,  // 絶対ショートアドレス
  AbLong,       // 絶対ロングアドレス
  PCDISP,  // ディスプレースメント付プログラムカウンタ相対
  PCIDX,   // インデックス付プログラムカウンタ相対
  IMMED,   // イミディエイトデータ
  CcrSr = 16,  // CCR / SR 形式

  AregIDXB,  // インデックス&ベースディスプレースメント付きアドレスレジスタ間接
  AregPOSTIDX,  // ポストインデックス付きメモリ間接
  AregPREIDX,   // プリインデックス付きメモリ間接
  PCIDXB,  // インデックス&ベースディスプレースメント付きプログラムカウンタ間接
  PCPOSTIDX,  // ポストインデックス付きPCメモリ間接
  PCPREIDX,   // プリインデックス付きPCメモリ間接

  RegList,  // レジスタリスト

  CtrlReg,  // 制御レジスタ

  RegPairD,   // レジスタペア(直接) dx:dy
  RegPairID,  // レジスタペア(間接) (rx):(ry)
  BitField,   // ビットフィールドの {offset:width}

  MMUreg,  // MMUレジスタ

  FPregD,     // FPn
  FPCRSR,     // FPSR,FPCR,FPIAR
  FPPairD,    // レジスタペア(直接) FPx:FPy
  KFactor,    // K-Factor {offset:width}
  FPregList,  // FP レジスタリスト

} adrmode;

// operand::flags
enum {
  // 直前のオペランドと "," なしで結合する(ビットフィールド、k-factor)
  OPFLAG_COMBINE = 0x01,

  // PC相対のディスプレースメントを持つ
  OPFLAG_PC_RELATIVE = 0x80,
};

// operand::labelchange1
enum {
  LABELCHANGE_DISALLOW = 0,
  LABELCHANGE_ALLOW,
  LABELCHANGE_DEPEND,           // アドレス依存データのみ
  LABELCHANGE_LABEL_ONLY = -1,  // ラベルのみ(BRA,Bcc,DBcc など)
};

typedef struct {
  char operand[64];     // オペランド文字列
  adrmode ea;           // 実効アドレスモード
  address opval;        // オペランドの値
  address opval2;       // オペランドの値(od用)
  address eaadrs;       // オペランドの存在アドレス
  address eaadrs2;      // オペランドの存在アドレス(od用)
  int8_t labelchange1;  // ラベル化可能
  uint8_t flags;        // OPFLAG_xxx
  uint8_t exbd;         // bd のサイズ(0,2,4) 0ならサプレス
  uint8_t exod;         // od のサイズ(0,2,4) 0ならサプレス

  // DregD, AregD, AregIDX, PCIDX の場合のみ有効
  uint8_t baseReg;  // ベースレジスタ番号(0-7)
  uint8_t ixReg;    // インデックスレジスタ番号(0-15)
  uint8_t ixSizeScale;  // インデックスレジスタのサイズとスケールファクタ

  uint8_t reserved[1];
} operand;

// disasm::codeflags
enum {
  // -M オプションでコメントを付ける命令(cmpi, move #imm など)
  CODEFLAG_NEED_COMMENT = 0x02,

  // JMPOP のうち、-B オプション指定時のみ空行を入れる命令(bra、fbra)
  CODEFLAG_BRAOP = 0x04,

  // -b0 オプション指定時にサイズを省略できない命令(bra.l d16 など)
  CODEFLAG_NEED_OPESIZE = 0x08,
};

typedef struct {
  WORD opecodeOffset;  // 命令(OpString 上のオフセット)
  mputypes mputypes;  // この命令を実行可能なMPUの種類(M000|M010|...)
  int8_t fpuid;  // 浮動小数点命令のコプロセッサID(0-7,-1なら通常命令)
  opesize size;   // サイズ(lea, pea は long) (0 = .b .w .l .s nothing)
  opesize size2;  // サイズ(lea, pea, moveq, bset, ... は UNKNOWN)
  opesize default_size;  // その命令のデフォルトのサイズ
  int bytes;             // 命令のバイト数
  opetype opeType;       // 命令の種類
  uint8_t codeflags;
  uint8_t reserved[2];

  address pc;     // 命令の開始PC
  address jmp;    // ジャンプ先アドレス (分岐命令なら)
  adrmode jmpea;  // 実効アドレスモード (分岐命令なら)
  operand op1;
  operand op2;
  operand op3;
  operand op4;
} disasm;

typedef struct {
  address pc;
  address pcEnd;
  codeptr ptr;

  disasm code;
} DisParam;

static inline void setDisParamPcPtr(DisParam* disp, address pc,
                                    codeptr disOfst) {
  disp->pc = pc;
  disp->ptr = disOfst + pc;
}

extern int dis(DisParam* disp);

#endif  // DISASM_H

// EOF
