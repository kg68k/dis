// ソースコードジェネレータ
// OS-9/X680x0(OSK)
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

#include "osk.h"

#include <stdlib.h>  // getenv()

#include "analyze.h"
#include "disasm.h"
#include "estruct.h"
#include "etc.h"
#include "generate.h"
#include "global.h"
#include "hex.h"
#include "include.h"
#include "label.h"
#include "opstr.h"
#include "output.h"
#include "symbol.h"
#include "version.h"

#ifdef OSKDIS

// OS-9/X680x0モジュールのヘッダ
enum {
  OSKHEAD_ID = 0x00,       // UWORD 0x4afc
  OSKHEAD_SYSREV = 0x02,   // UWORD リビジョンID
  OSKHEAD_MODSIZE = 0x04,  // ULONG モジュールサイズ
  OSKHEAD_OWNER = 0x08,    // ULONG オーナID
  OSKHEAD_NAME = 0x0c,     // address モジュール名オフセット
  OSKHEAD_ACCS = 0x10,     // UWORD アクセス許可
  OSKHEAD_TYPE = 0x12,     // UWORD モジュールタイプ／言語
  OSKHEAD_ATTR = 0x14,     // UWORD 属性／リビジョン
  OSKHEAD_EDITION = 0x16,  // UWORD エディション
  OSKHEAD_USAGE = 0x18,    // address 使い方コメントのオフセット
  OSKHEAD_SYMBOL = 0x1c,   // address シンボルテーブル
  OSKHEAD_RESV = 0x20,     // BYTE[14] 予約済み
  OSKHEAD_PARITY = 0x2e,   // UWORD ヘッダパリティ
  OSKHEAD_EXEC = 0x30,     // address 実行オフセット
  OSKHEAD_EXCPT = 0x34,    // address ユーザトラップエントリ
  OSKHEAD_MEM = 0x38,      // ULONG メモリサイズ
  OSKHEAD_STACK = 0x3c,    // ULONG スタックサイズ
  OSKHEAD_IDATA = 0x40,    // address 初期化データオフセット
  OSKHEAD_IREFS = 0x44,    // address 初期化参照オフセット
  OSKHEAD_INIT = 0x48,     // address 初期化実行エントリ(TRAPLIB)
  OSKHEAD_TERM = 0x4c,     // address 終了実行エントリ(TRAPLIB)

  OSKHEAD_SIZE = 0x50
};

// OS9 ファンクションコールでプログラムを終了させるもの
enum {
  F_Exit = 0x0006,   // F$Exit
  F_RTE = 0x001e,    // F$RTE
  F_NProc = 0x002d,  // F$NProc
};

enum {
  OSK_BASEREG = 6,
};

static lblbuf* idatagen(address end, lblbuf* lptr);
static void vsectbssgen(address pc, address nlabel, lblmode mode);

static void registerVsectLabelOp(disasm* code, operand* op) {
  if (op->ea == AregDISP && op->baseReg == OSK_BASEREG) {
    const ULONG offset = 0x8000;
    address adrs = Dis.beginBSS + op->opval + offset;
    lblbuf* lptr = search_label(adrs);
    lblmode wkmode = lptr ? (lptr->mode & (CODEPTR | DATAPTR)) : DATLABEL;

    regist_label(adrs, wkmode | code->size2 | FORCE);
  }
}

// オペランドが(d16,A6)ならvsectとして登録する
static void registerVsectLabel(disasm* code) {
  if (Dis.oskModType != OSKTYPE_PROGRAM && Dis.oskModType != OSKTYPE_TRAP)
    return;

  registerVsectLabelOp(code, &code->op1);
  registerVsectLabelOp(code, &code->op2);
  registerVsectLabelOp(code, &code->op3);
  registerVsectLabelOp(code, &code->op4);
}

// ワードテーブルとして登録する
static void w_table(address table) {
  address ptr = table;
  address tableend = next(table + 1)->label;

  regist_label(table, DATLABEL | WTABLE);

  while (ptr < tableend) {
    address label = (WORD)peekw(ptr + Dis.Ofst);

    regist_label(label, DATLABEL | UNKNOWN);
    ptr += 2;
    tableend = next(ptr)->label;
  }
}

// IDataから初期化データを登録する
static void analyze_idata(void) {
  codeptr ptr = Dis.Ofst + (ULONG)Dis.oskIdata;

  regist_label(Dis.oskIdata, DATLABEL | LONGSIZE | FORCE);
  Dis.beginDATA = Dis.beginBSS + peekl(ptr + 0);
  regist_label(Dis.beginDATA, DATLABEL | UNKNOWN);
  regist_label(Dis.beginDATA + peekl(ptr + 4), DATLABEL | UNKNOWN);
}

static void analyze_irefs_sub(int codelbl, codeptr ptr, ULONG offset,
                              int mode) {
  ULONG value0 = peekl(ptr);

  if (value0 <= offset && offset < (value0 + peekl(ptr + 4))) {
    codeptr w = ptr + 8 + offset - value0;
    regist_label(peekl(w) + codelbl ? 0 : Dis.beginBSS, mode);
  }
}

typedef struct {
  UWORD base;
  UWORD cnt;
} RefsTbl;

// IRefsを解析する
static void analyze_irefs(void) {
  const codeptr idatp = Dis.Ofst + (ULONG)Dis.oskIdata;
  RefsTbl* offset = (RefsTbl*)(Dis.Ofst + (ULONG)Dis.oskIrefs);

  regist_label(Dis.oskIrefs, DATLABEL | WORDSIZE | FORCE);

  while (offset->cnt) {  // コードポインタ
    ULONG wk = Dis.beginBSS + ((ULONG)offset->base << 16);
    UWORD* p = (UWORD*)&offset[1];
    int i;

    for (i = 0; i < offset->cnt; i++) {
      ULONG o = peekw((codeptr)p);
      charout('#');
      regist_label(wk + o, DATLABEL | LONGSIZE | CODEPTR | FORCE);
      regist_label(wk + o + 4, DATLABEL | UNKNOWN);
      analyze_irefs_sub(TRUE, idatp, o, DATLABEL | UNKNOWN);
      p++;
    }
    offset = (RefsTbl*)p;
  }
  offset++;
  while (offset->cnt) {  // データポインタ
    ULONG wk = Dis.beginBSS + ((ULONG)offset->base << 16);
    UWORD* p = (UWORD*)&offset[1];
    int i;

    for (i = 0; i < offset->cnt; i++) {
      ULONG o = peekw((codeptr)p);
      charout('#');
      regist_label(wk + o, DATLABEL | LONGSIZE | DATAPTR | FORCE);
      regist_label(wk + o + 4, DATLABEL | UNKNOWN);
      analyze_irefs_sub(FALSE, idatp, o, DATLABEL | UNKNOWN);
      p++;
    }
    offset = (RefsTbl*)p;
  }
}

static void setOskCallLabel(codeptr ptr, disasm* code, operand* op, UWORD word2,
                            char** labels) {
  op->ea = IMMED;

  if (Dis.needString) {
    char* label = (labels && (word2 <= 0xff)) ? labels[word2] : NULL;

    if (label)
      strcpy(op->operand, label);
    else
      itox4d(op->operand, word2);
  }
}

// trap命令(ファンクションコール)
static void oskTrap(codeptr ptr, disasm* code) {
  UWORD word2 = peekw(ptr + 2);
  ULONG n = peekb(ptr + 1) & 15;

  if (n == 0) {
    OPECODE(os9);
    setOskCallLabel(ptr, code, &code->op1, word2, Dis.OS9label);

    if (word2 == F_Exit || word2 == F_RTE || word2 == F_NProc)
      code->opeType = RTSOP;
  } else {
    char* call = NULL;
    char** labels = NULL;

    OPECODE(tcall);

    if (n == 13) {
      call = "CIO$Trap";
      labels = Dis.CIOlabel;
    } else if (n == 15) {
      call = "T$Math";
      labels = Dis.MATHlabel;
    }

    code->op1.ea = IMMED;
    if (Dis.needString) {
      if (call)
        strcpy(code->op1.operand, call);
      else
        itod2(code->op1.operand, n);
    }
    setOskCallLabel(ptr, code, &code->op2, word2, labels);
  }
  code->bytes = 4;
}

// ファイル情報を書き出す
static void outputOskFileInfo(char* filename, time_t filedate,
                              const char* argv0, char* comline) {
  com("=============================================");
  com("  Filename %s", filename);
  if (!Dis.deterministic) {
    com("  Time Stamp %s", ctimez(&filedate));
  }
  comBlank();
  com("  %d Labels", get_Labelnum());
  if (!Dis.deterministic) {
    time_t t = time(NULL);
    com("  Code Generate date %s", ctimez(&t));
  }
  if (Dis.dis_opt != NULL) {
    com("  Environment %s", Dis.dis_opt);
  }
  com("  Commandline %s %s", argv0, comline);
  if (!Dis.deterministic) {
    com("          %s", ProgramAndVersion);
  }

  com("********************************************");
  com("  Revision    %04x", (int)Dis.oskSysRev);
  com("  Module Size %d bytes", (int)Dis.oskSize);
  com("  Owner       %d.%d", (int)Dis.oskOwner >> 16,
      (int)Dis.oskOwner & 0xffff);
  com("  Name Offset %08x", (int)Dis.oskName);
  com("  Permit      %04x", (int)Dis.oskAccs);
  com("  Type/Lang   %04x", (int)Dis.oskType);
  com("  Attribute   %04x", (int)Dis.oskAttr);
  com("  Edition     %d", (int)Dis.oskEdition);
  com("  Entry       %08x", (int)Dis.exec);
  com("  Excpt       %08x", (int)Dis.oskExcpt);

  if (Dis.oskModType == OSKTYPE_DRIVER || Dis.oskModType == OSKTYPE_TRAP ||
      Dis.oskModType == OSKTYPE_PROGRAM) {
    com("  Memory Size %d", (int)Dis.oskMem);
  }
  if (Dis.oskModType == OSKTYPE_PROGRAM || Dis.oskModType == OSKTYPE_TRAP) {
    com("  Stack Size  %d", (int)Dis.oskStack);
    com("  M$IData     %08x", (int)Dis.oskIdata);
    com("  M$IRefs     %08x", (int)Dis.oskIrefs);
  }
  if (Dis.oskModType == OSKTYPE_TRAP) {
    com("  M$Init      %08x", (int)Dis.oskInit);
    com("  M$Term      %08x", (int)Dis.oskTerm);
  }
  com("********************************************");
  outputBlank();
}

static void outputOskPsect(void) {
  char buf[128], buf2[32];
  char* name = (char*)(Dis.Ofst + Dis.oskName);  // モジュール名
  UWORD type = Dis.oskModType;
  int hasStack = (type == OSKTYPE_PROGRAM || type == OSKTYPE_TRAP);
  int stack = hasStack ? Dis.oskStack : 0;

  snprintf(buf, sizeof(buf), "%s\t%s,$%04x,$%04x,%d,%d,L%06x", OpString.psect,
           name, (int)Dis.oskType, (int)Dis.oskAttr, (int)Dis.oskEdition, stack,
           (int)Dis.exec);

  buf2[0] = '\0';
  if (Dis.oskExcpt) {
    snprintf(buf2, sizeof(buf2), ",L%06x", (int)Dis.oskExcpt);
  }
  otherDirective2(buf, buf2);
}

// ヘッダを書き出す
static void outputHeaderOsk(char* filename, time_t filedate, const char* argv0,
                            char* comline, FILE* fpHeader) {
  outputOskFileInfo(filename, filedate, argv0, comline);
  outputOskPsect();
  outputHeaderCpu();
}

// 各セクションを出力する
static void generateOsk(char* sfilename) {
  int type = XDEF_TEXT;
  lblbuf* lptr = generateSection(sfilename, NULL, Dis.beginBSS,
                                 next(Dis.beginTEXT), XDEF_TEXT, &type);
  lblmode nmode = lptr->mode;
  address pc = lptr->label;

  outputBlank();
  otherDirective(OpString.vsect);
  outputBlank();

  while (pc <= Dis.beginDATA) {
    lblmode mode = nmode;
    address nlabel;

    lptr = Next(lptr);
    nlabel = lptr->label;
    nmode = lptr->mode;

    if (pc == Dis.beginDATA) break;  // 泥縄

    vsectbssgen(pc, nlabel, mode);
    pc = nlabel;
  }

  lptr = next(Dis.beginDATA);  // vsect の初期化データ領域をポイント
  switch (Dis.oskModType) {
    codeptr idatsiz;
    case OSKTYPE_PROGRAM:
    case OSKTYPE_TRAP:
      idatsiz = Dis.Ofst + (ULONG)Dis.oskIdata + 4;
      lptr = idatagen(Dis.beginDATA + peekl(idatsiz), lptr);
      // vsect の初期化データを出力
      break;

    default:
      lptr = idatagen(Dis.LAST, lptr);
      break;
  }
  pc = lptr->label;

  outputBlank();
  otherDirective(OpString.ends);  // end vsect

  if (pc < Dis.LAST) {
    outputBlank();
    otherDirective(OpString.vsect_remote);
    outputBlank();

    while (pc <= Dis.LAST) {
      lblmode mode = nmode;
      address nlabel;

      lptr = Next(lptr);
      nlabel = lptr->label;
      nmode = lptr->mode;

      if (pc == Dis.LAST) break;  // 泥縄

      vsectbssgen(pc, nlabel, mode);
      pc = nlabel;
    }
    outputBlank();
    otherDirective(OpString.ends);  // end vsect remote
  }

  outputBlank();
  otherDirective(OpString.ends);  // end psect
  outputBlank();
  otherDirective(OpString.end);
}

// 初期化データを出力する(下請け2)
static void idatagen_subInner(address pc, codeptr wkpc, lblmode mode,
                              ULONG byte) {
  char buf[64];

  switch (lblmodeOpesize(mode)) {
    default:
    case BYTESIZE:
      break;

    case WORDSIZE:
      if (byte != 2) break;

      snprintf(buf, sizeof(buf), "$%04x", (int)peekw(wkpc));
      outputData2(pc, OpString.dc[WORDSIZE], buf);
      return;

    case LONGSIZE:
      if (byte != 4) break;

      if (mode & CODEPTR) {
        ULONG x = peekl(wkpc);
        const char* fmt = (x == 0)             ? "_btext\t* $%08x"
                          : (x == Dis.oskName) ? "_bname\t* $%08x"
                                               : "L%06x";
        snprintf(buf, sizeof(buf), fmt, (int)x);
      } else if (mode & DATAPTR) {
        snprintf(buf, sizeof(buf), "L%06x", (int)(peekl(wkpc) + Dis.beginBSS));
      } else {
        snprintf(buf, sizeof(buf), "$%08x", (int)peekl(wkpc));
      }
      outputData2(pc, OpString.dc[LONGSIZE], buf);
      return;
  }

  dataoutDc(pc, wkpc, BYTESIZE, byte, 1);
}

// 初期化データを出力する(下請け)
static codeptr idatagen_sub(address pc, codeptr wkpc, address pcend,
                            lblmode mode) {
  while (pc < pcend) {
    ULONG byte = pcend - pc;

    idatagen_subInner(pc, wkpc, mode, byte);
    pc += byte;
    wkpc += byte;
  }
  return wkpc;
}

// 初期化データを出力する
// input:
//   end     : block end address
//   lptr    : lblbuf* ( contains block start address )
static lblbuf* idatagen(address end, lblbuf* lptr) {
  address pc = lptr->label;
  codeptr wkpc = Dis.Ofst + (ULONG)Dis.oskIdata + 8;

  while (pc < end) {
    lblmode mode = lptr->mode;

    do {
      lptr = Next(lptr);
    } while (lptr->shift);

    label_line_out(pc, mode, 0, FALSE, LINETYPE_DATA);

    wkpc = idatagen_sub(pc, wkpc, lptr->label, mode);
    pc = lptr->label;
  }

  return lptr;
}

// vsectラベル + BSS出力
static void vsectbssgen(address pc, address nlabel, lblmode mode) {
  char buf[64];
  int bytes = MIN(nlabel, Dis.LAST) - pc;
  SizeLength sl;
  SymbolLabelFormula slfml;
  const char* colon = "";

  charout('$');

  slfml.symbol = "";
  slfml.expr[0] = '\0';
  if (!isHIDDEN(mode)) {
    makeSymLabFormula(&slfml, pc);
    colon = ":";
  }
  sl = getSizeLength(getSizeBytes(lblmodeOpesize(mode)), bytes);
  snprintf(buf, sizeof(buf), "%s%s%d", colon, OpString.ds[sl.size], sl.length);

  outputData3(pc, slfml.symbol, slfml.expr, buf);
}

// ワードテーブルの出力
// input:
//   pc : word table begin address
//   pcend : word table end address
static void wgen(address pc, address pcend) {
  const address limit = pcend - sizeof(UWORD);

  charout('w');

  while (pc <= limit) {
    address adrs = peekw(pc + Dis.Ofst);

    if (adrs == (address)0) {
      outputData2(pc, OpString.dc[WORDSIZE], "0");
    } else {
      SymbolLabelFormula slfml;
      makeSymLabFormula(&slfml, adrs);
      outputData3(pc, OpString.dc[WORDSIZE], slfml.symbol, slfml.expr);
    }
    pc += sizeof(UWORD);
  }

  dataoutDcByte(pc, pcend);  // 半端に残ったデータを出力
}

// (d16,An)のディスプレースメントをラベルに置き換える
static char* operandToVsectLabel(char* p, operand* op) {
  if ((Dis.oskModType == OSKTYPE_PROGRAM || Dis.oskModType == OSKTYPE_TRAP) &&
      op->baseReg == OSK_BASEREG) {
    const ULONG offset = 0x8000;
    address adrs = Dis.beginBSS + op->opval + offset;
    SymbolLabelFormula slfml;

    makeBareLabel(&slfml, adrs);
    *p++ = '(';
    p = strcpy2(p, slfml.expr);
    return strcpy2(p, strchr(op->operand, ','));
  }

  return strcpy2(p, op->operand);
}

static const IncludeSpec os9defsSpec = {  //
    (1 << 8) - 1, 0, &Dis.OS9label, &Dis.os9MacPath, NULL, NULL};
static const IncludeSpec mathdefsSpec = {  //
    (1 << 8) - 1, 0, &Dis.MATHlabel, &Dis.mathMacPath, NULL, NULL};
static const IncludeSpec ciodefsSpec = {  //
    (1 << 8) - 1, 0, &Dis.CIOlabel, &Dis.cioMacPath, NULL, NULL};

static void readIncludeFileOsk(const IncludeSpec* spec, const char* path,
                               const char* filename) {
  char* fullpath = Malloc(strlen(path) + 1 + strlen(filename) + 1);
  strcat(strcat(strcpy(fullpath, path), "/"), filename);
  if (!readIncludeFile(spec, fullpath)) {
    eprintf("%s をオープンできません。\n", filename);
    return;
  }
  *spec->fullpathPtr = fullpath;
}

// 各種インクルードファイルを読み込む
//   環境変数 DISDEFS で指定されたディレクトリから
//   os9defs.d mathdefs.d ciodefs.d を読み込む
static void readIncludeFilesOsk(void) {
  char* dis_inc = getenv("DISDEFS");

  if (dis_inc == NULL) {
    eprintf("環境変数 DISDEFS が設定されていません。\n");
    return;
  }
  readIncludeFileOsk(&os9defsSpec, dis_inc, "os9defs.d");
  readIncludeFileOsk(&mathdefsSpec, dis_inc, "mathdefs.d");
  readIncludeFileOsk(&ciodefsSpec, dis_inc, "ciodefs.d");
}

static void analyzeOptional(address adrs) {
  if (adrs != (address)0) analyze(adrs, ANALYZE_IGNOREFAULT);
}

// プログラム領域を解析する
static void analyzeProgramOsk(void) {
  regist_label(Dis.base + Dis.text + Dis.data + Dis.bss, DATLABEL | UNKNOWN);
  regist_label(Dis.oskName, DATLABEL | STRING | FORCE);
  regist_label(Dis.oskSize - 4, DATLABEL | BYTESIZE | FORCE);  // CRC

  switch (Dis.oskModType) {
    case OSKTYPE_PROGRAM:
    case OSKTYPE_TRAP:
      eprintf("初期化データ領域解析中です。");
      analyze_idata();  // 初期データオフセットを解析
      analyze_irefs();  // 初期データ参照テーブルを解析
      eputc('\n');
    default:
      break;
  }

  eprintf("プログラム領域解析中です。");
  setReasonVerbose(BACKTRACK_REASON_PROGRAM);

  analyzeOptional(Dis.oskExcpt);

  switch (Dis.oskModType) {
    case OSKTYPE_TRAP:
      analyzeOptional(Dis.oskInit);
      analyzeOptional(Dis.oskTerm);
      regist_label((address)0x000048, DATLABEL | ZTABLE);
      z_table((address)0x000048);
      // FALLTHRU
    case OSKTYPE_PROGRAM:
    case OSKTYPE_SUBROUTINE:
    case OSKTYPE_SYSTEM:
      analyze(Dis.exec, ANALYZE_IGNOREFAULT);
      break;

    case OSKTYPE_FILEMAN:
      registerReltblOrder(&Dis.reltblArray, Dis.exec, RELTABLE, TRUE,
                          DATLABEL | FORCE);
      break;

    case OSKTYPE_DRIVER:
      w_table(Dis.exec);  // 実行エントリテーブル
      break;
  }
}

#define OFFS(member) GET_OPECODE_OFFSET(member)

// オペコード文字列をr68(OS-9/X680x0用アセンブラ)形式に書き換える
static void modifyMnemonicOsk(OpStringTable* ops) {
  static const uint16_t list[] = {
      OFFS(dcWord),    //
      OFFS(include_),  //
      OFFS(cpu),       //
      OFFS(fpid),      //
      OFFS(equ),       //
      OFFS(xdef),      //
      OFFS(text),      //
      OFFS(data),      //
      OFFS(bss),       //
      OFFS(stack),     //
      OFFS(end),       //
  };
  size_t i;

  // "." を取り除く
  for (i = 0; i < _countof(list); ++i) {
    char* p = (char*)ops + list[i];
    strcpy(p + 1, p + 2);
  }
  for (i = 0; i < _countof(ops->dc); ++i) {
    char* p = ops->dc[i];
    strcpy(p + 1, p + 2);
  }
  for (i = 0; i < _countof(ops->ds); ++i) {
    char* p = ops->ds[i];
    strcpy(p + 1, p + 2);
  }
  strcpy(ops->dcWord, ops->dcWord + 1);

  // .even を align に差し替える
  strcpy(ops->even, ops->r68Align);
}

static void makeRelocateTableOsk(void) {
  // 処理不要
}

static void parseSymbolTableOsk(ArrayBuffer* stbuf) {
  // 処理不要
}

// OS-9/X680x0モジュールの処理関数
static const Actions actionsOsk = {
    registerVsectLabel,    //
    oskTrap,               //
    operandToVsectLabel,   //
    wgen,                  //
    makeRelocateTableOsk,  //
    parseSymbolTableOsk,   //
    analyzeProgramOsk,     //
    readIncludeFilesOsk,   //
    modifyMnemonicOsk,     //
    outputHeaderOsk,       //
    generateOsk            //
};

// OS-9/X680x0モジュールのヘッダを読み込む
static void loadHeaderOskModule(FILE* fp, ULONG fileSize) {
  UBYTE head[OSKHEAD_SIZE];

  if (fread(head, 1, sizeof(head), fp) != sizeof(head) ||
      peekw(head + OSKHEAD_ID) != OSKHEAD_ID_VALUE) {
    err("OS-9/680x0のモジュールではありません。\n");
  }

  Dis.oskSysRev = peekw(head + OSKHEAD_SYSREV);
  Dis.oskSize = peekl(head + OSKHEAD_MODSIZE);
  Dis.oskOwner = peekl(head + OSKHEAD_OWNER);
  Dis.oskName = peekl(head + OSKHEAD_NAME);
  Dis.oskAccs = peekw(head + OSKHEAD_ACCS);
  Dis.oskType = peekw(head + OSKHEAD_TYPE);
  Dis.oskAttr = peekw(head + OSKHEAD_ATTR);
  Dis.oskEdition = peekw(head + OSKHEAD_EDITION);
  Dis.oskUsage = peekl(head + OSKHEAD_USAGE);
  Dis.oskSymbol = peekl(head + OSKHEAD_SYMBOL);
  Dis.oskExcpt = peekl(head + OSKHEAD_EXCPT);
  Dis.oskMem = peekl(head + OSKHEAD_MEM);
  Dis.oskStack = peekl(head + OSKHEAD_STACK);
  Dis.oskIdata = peekl(head + OSKHEAD_IDATA);
  Dis.oskIrefs = peekl(head + OSKHEAD_IREFS);
  Dis.oskInit = peekl(head + OSKHEAD_INIT);
  Dis.oskTerm = peekl(head + OSKHEAD_TERM);

  Dis.oskModType = (Dis.oskType >> 8) & 0x0f;
  switch (Dis.oskModType) {
    default:
      err("このモジュールタイプはサポートしていません。\n");

    case OSKTYPE_PROGRAM:
    case OSKTYPE_TRAP:
      Dis.base = 0x0048;
      break;
    case OSKTYPE_SUBROUTINE:
    case OSKTYPE_SYSTEM:
    case OSKTYPE_FILEMAN:
    case OSKTYPE_DRIVER:
      Dis.base = 0x003c;
      break;
  }

  Dis.exec = peekl(head + OSKHEAD_EXEC);
  Dis.text = fileSize - (ULONG)Dis.base;
  Dis.bss = peekl(head + OSKHEAD_MEM);

  fseek(fp, Dis.base, SEEK_SET);
}

// 実行ファイルのヘッダを読み込む
void loadHeaderOsk(FILE* fp, ULONG fileSize) {
  switch (Dis.fileType) {
    default:
      err("Human68kの実行ファイルには対応していません。\n");

    case FILETYPE_DUMP:
      Dis.text = fileSize;
      break;

    case FILETYPE_OSK:
      loadHeaderOskModule(fp, fileSize);
      break;
  }

  // OS-9/680x0のアセンブラ(r68)用の設定
  Dis.compressLen = 0;  // dcb疑似命令を使えない
  Dis.oldSyntax = TRUE;
  Dis.dbccSize = NOTHING;  // DBccにサイズを付けるとエラーになる
  Dis.quoteChar = '"';

  Dis.findLinkW = TRUE;

  Dis.beginTEXT = Dis.base;                 // psect 先頭
  Dis.beginBSS = Dis.beginTEXT + Dis.text;  // vsect 先頭
  Dis.beginDATA = Dis.beginBSS + Dis.bss;   // 初期化 vsect 先頭
  Dis.LAST = Dis.beginDATA + Dis.data;
  Dis.availableTextEnd = Dis.beginBSS;

  Dis.actions = &actionsOsk;
}

#endif

// EOF
