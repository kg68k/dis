// ソースコードジェネレータ
// Human68k
// Copyright (C) 2025 TcbnErik

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

#include "human.h"

#include <stdio.h>
#include <stdlib.h>  // getenv()
#include <time.h>

#include "analyze.h"
#include "estruct.h"
#include "etc.h"
#include "generate.h"
#include "global.h"
#include "include.h"
#include "label.h"
#include "offset.h"
#include "opstr.h"
#include "output.h"
#include "symbol.h"
#include "version.h"

// X形式実行ファイルのヘッダ
enum {
  XHEAD_ID = 0x00,      // UBYTE[2] "HU"
  XHEAD_BASE = 0x04,    // address
  XHEAD_EXEC = 0x08,    // address
  XHEAD_TEXT = 0x0c,    // ULONG
  XHEAD_DATA = 0x10,    // ULONG
  XHEAD_BSS = 0x14,     // ULONG
  XHEAD_OFFSET = 0x18,  // ULONG
  XHEAD_SYMBOL = 0x1c,  // ULONG
  XHEAD_BIND = 0x3c,    // ULONG

  XHEAD_SIZE = 0x40
};

// Z形式実行ファイルのヘッダ
enum {
  ZHEAD_ID = 0x00,    // UBYTE[2] "\x60\x1a"
  ZHEAD_TEXT = 0x02,  // ULONG
  ZHEAD_DATA = 0x06,  // ULONG
  ZHEAD_BSS = 0x0a,   // ULONG
  ZHEAD_BASE = 0x16,  // address

  ZHEAD_SIZE = 0x1c
};

// デバイスヘッダの構造
enum {
  DH_NEXT = 0x00,       // ULONG
  DH_TYPE = 0x04,       // UWORD
  DH_STRATEGY = 0x06,   // address
  DH_INTERRUPT = 0x0a,  // address
  DH_NAME = 0x0e,       // UBYTE[8]
  DH_SIZE = 0x16,
};

// ファイル情報を書き出す
static void outputFileInfoHuman(char* filename, time_t filedate,
                                const char* argv0, char* comline) {
  com("=============================================");
  com("  Filename %s", filename);
  if (!Dis.deterministic) {
    com("  Time Stamp %s", ctimez(&filedate));
  }
  comBlank();
  com("  Base address " PRI_ADRS, Dis.base);
  com("  Exec address " PRI_ADRS, Dis.exec);
  com("  Text size    " PRI_ULHEX " byte(s)", Dis.text);
  com("  Data size    " PRI_ULHEX " byte(s)", Dis.data);
  com("  Bss  size    " PRI_ULHEX " byte(s)", Dis.bss);

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
  com("=============================================");
  outputBlank();
}

// dis_headerで指定されたファイルの中身を書き出す
static void outputHeaderFromFile(FILE* fp) {
  char buffer[256];

  while (fgets(buffer, sizeof buffer, fp)) {
    removeTailLf(buffer);
    outputDirective(LINETYPE_OTHER, (address)0, buffer);
  }
}

// 外部定義シンボルを書き出す
static void outputHeaderXdef(void) {
  uint8_t c = Dis.symbolColonNum;
  char* colon = (c == SYMBOL_COLON_OMIT)  ? ""
                : (c == SYMBOL_COLON_ONE) ? ":"
                                          : "::";

  if (outputSymbolSection(&Dis.symtblArray, colon)) outputBlank();
}

static void outputInclude(const char* path);

// .include疑似命令(またはdis_headerの中身)を書き出す
static void outputIncludeHuman(FILE* fpHeader) {
  if (fpHeader) {
    outputHeaderFromFile(fpHeader);
  } else {
    outputInclude(Dis.doscallMacPath);
    outputInclude(Dis.iocscallMacPath);
    outputInclude(Dis.fefuncMacPath);
    outputInclude(Dis.sxcallMacPath);
  }
  outputBlank();
}

// 有効なパス名からファイル名部分を取り出す
static const char* getBasename(const char* path) {
  const char* s = strrchr(path, '/');
  if (s == NULL) s = strrchr(path, ':');
  return (s == NULL) ? path : s + 1;
}

static void outputInclude(const char* path) {
  if (path == NULL) return;

  if (Dis.stripIncludePath) path = getBasename(path);

  otherDirective2(OpString.include_, path);
}

// ヘッダを書き出す
static void outputHeaderHuman(char* filename, time_t filedate,
                              const char* argv0, char* comline,
                              FILE* fpHeader) {
  outputFileInfoHuman(filename, filedate, argv0, comline);
  outputIncludeHuman(fpHeader);
  outputHeaderXdef();
  outputHeaderCpu();
}

// .end疑似命令を書き出す
static void outputEnd(void) {
  SymbolLabelFormula slfml;
  makeSymLabFormula(&slfml, Dis.exec);

  outputBlank();
  {
    const char* array[] = {OpString.end, "\t", slfml.symbol, slfml.expr};
    outputDirectiveArray(LINETYPE_OTHER, 0, _countof(array), array);
  }
}

// .text/.data/.bss/.stackセクションを出力する
static void generateHuman(char* sfilename) {
  int type = XDEF_TEXT;
  lblbuf* lptr = next(Dis.beginTEXT);

  lptr = generateSection(sfilename, OpString.text, Dis.beginDATA, lptr,
                         XDEF_TEXT, &type);
  lptr = generateSection(sfilename, OpString.data, Dis.beginBSS, lptr,
                         XDEF_DATA, &type);
  lptr = generateSection(sfilename, OpString.bss, Dis.beginSTACK, lptr,
                         XDEF_BSS, &type);
  lptr = generateSection(sfilename, OpString.stack, Dis.LAST, lptr, XDEF_STACK,
                         &type);

  // 末尾にある属性0のシンボルを出力する
  label_line_out(lptr->label, lptr->mode, 0, TRUE, LINETYPE_OTHER);

  outputEnd();
}

////////////////////////////

static const IncludeSpec doscallSpec = {
    (1 << 8) - 1,    0xff00,  //
    &Dis.dosLabel,   &Dis.doscallMacPath,
    &Dis.doscallMac, OpString.doscall  //
};
static const IncludeSpec fefuncSpec = {
    (1 << 8) - 1,     0xfe00,  //
    &Dis.fefuncLabel, &Dis.fefuncMacPath,
    &Dis.fefuncMac,   OpString.fefunc  //
};
static const IncludeSpec iocscallSpec = {
    (1 << 8) - 1,     0x0000,  //
    &Dis.iocsLabel,   &Dis.iocscallMacPath,
    &Dis.iocscallMac, OpString.iocscall  //
};
static const IncludeSpec sxcallSpec = {
    (1 << 12) - 1, 0xa000,  //
    &Dis.sxLabel,  &Dis.sxcallMacPath,
    NULL,          OpString.sxcall  //
};

// 各種インクルードファイルを読み込む
static void readIncludeFilesHuman(void) {
  IncludeEnvs env = {getenv(ENV_dis_include), getenv(ENV_include)};

  readInludeFileFromEnv(&doscallSpec, &env);
  readInludeFileFromEnv(&iocscallSpec, &env);
  readInludeFileFromEnv(&fefuncSpec, &env);

  if (Dis.sxWindow) {
    const char* sxmac = getenv(ENV_dis_sxmac);
    if (sxmac && sxmac[0]) {
      if (!readIncludeFile(&sxcallSpec, sxmac))
        err("%s をオープンできません。\n", sxmac);
    }
  }
}

// デバイスドライバの解析
static void analyzeDeviceDriver(void) {
  address prev = (address)-1;
  address dev = Dis.base;  // テキストセクション先頭が最初のデバイスヘッダ

  setReasonVerbose(BACKTRACK_REASON_PROGRAM);

  do {
    codeptr ptr = Dis.Ofst + dev;

    lblbuf* lblptr = search_label(dev);
    if (lblptr && isDEVLABEL(lblptr->mode)) {
      eprintf("\n警告: デバイスヘッダが循環しています(" PRI_ADRS " -> " PRI_ADRS
              ")。",
              prev, dev);
      break;
    }

    regist_label(dev + DH_NEXT,
                 DATLABEL | (lblmode)LONGSIZE | FORCE | DEVLABEL);
    regist_label(dev + DH_TYPE, DATLABEL | (lblmode)WORDSIZE | FORCE | HIDDEN);

    // ストラテジルーチンと割り込みルーチンはリロケート情報があるので登録不要
    // ```
    // regist_label(dev + DH_STRATEGY, DATLABEL | LONGSIZE | FORCE);
    // regist_label(dev + DH_INTERRUPT, DATLABEL | LONGSIZE | FORCE);
    // ```

    regist_label(dev + DH_NAME, DATLABEL | (lblmode)STRING | FORCE | HIDDEN);
    regist_label(dev + DH_SIZE, DATLABEL | (lblmode)UNKNOWN);

    analyze((address)peekl(ptr + DH_STRATEGY), ANALYZE_IGNOREFAULT);
    analyze((address)peekl(ptr + DH_INTERRUPT), ANALYZE_IGNOREFAULT);

    prev = dev;
    dev = peekl(ptr + DH_NEXT);
  } while (isEven(dev) && (Dis.base < dev) &&
           (dev < Dis.beginBSS));  // SCSIDRV.SYS 対策
}

// プログラム領域を解析する
static void analyzeProgramHuman(void) {
  eprintf("プログラム領域解析中です。");
  setReasonVerbose(BACKTRACK_REASON_PROGRAM);

  if (Dis.d) {
    analyzeDeviceDriver();
    if (Dis.exec != Dis.base)
      analyze(Dis.exec, Dis.i ? ANALYZE_IGNOREFAULT : ANALYZE_NORMAL);
  } else
    analyze(Dis.exec, ANALYZE_IGNOREFAULT);
}

static void datagenWTableHuman(GCC_UNUSED address pc,
                               GCC_UNUSED address pcend) {
  // 処理不要
}

static void modifyMnemonicHuman(GCC_UNUSED OpStringTable* ops) {
  // 処理不要
}

// X/R/Z形式実行ファイルの処理関数
static const Actions actionsHuman = {
    NULL,                    // registerOperandLabel
    NULL,                    // decodeTrap
    NULL,                    // d16AnToLabel
    datagenWTableHuman,      //
    makeRelocateTableHuman,  //
    parseSymbolTableHuman,   //
    analyzeProgramHuman,     //
    readIncludeFilesHuman,   //
    modifyMnemonicHuman,     //
    outputHeaderHuman,       //
    generateHuman            //
};

// X形式実行ファイルのヘッダの正当性を調べる
static void validateXHeader(UBYTE* head) {
  const ULONG MAIN_MEMORY_MAX_BYTES = 12 * 1024 * 1024;
  const ULONG HIGH_MEMORY_MAX_BYTES = 256 * 1024 * 1024;
  ULONG total = Dis.text + Dis.data + Dis.bss;

  // ハイメモリの最大量でもロードできない値なら実行ファイルとは考えられない
  //   オーバーフロー対策で個別の値でも比較
  if ((total > HIGH_MEMORY_MAX_BYTES) || (Dis.text > HIGH_MEMORY_MAX_BYTES) ||
      (Dis.data > HIGH_MEMORY_MAX_BYTES) || (Dis.bss > HIGH_MEMORY_MAX_BYTES)) {
    err("X形式実行ファイルではありません。\n");
  }

  // ハイメモリ専用実行ファイルかもしれないので警告のみ
  if (total > MAIN_MEMORY_MAX_BYTES) {
    eprintf(
        "実行時メモリ必要量が12MiBを超えるため、"
        "X形式実行ファイルではない可能性があります。\n");
  }

  if (peekl(head + XHEAD_BIND) != 0) {
    err("このファイルはバインドされています。\n"
        "unbindコマンド等で展開してからソースジェネレートして下さい。\n");
  }
}

// X形式実行ファイルのヘッダを読み込む
static void loadXHeader(FILE* fp) {
  UBYTE head[XHEAD_SIZE];

  if (fread(head, 1, sizeof(head), fp) != sizeof(head) ||
      peekw(head + XHEAD_ID) != XHEAD_ID_VALUE) {
    err("X形式実行ファイルではありません。\n");
  }

  Dis.base = (address)peekl(head + XHEAD_BASE);
  Dis.exec = (address)peekl(head + XHEAD_EXEC);
  Dis.text = peekl(head + XHEAD_TEXT);
  Dis.data = peekl(head + XHEAD_DATA);
  Dis.bss = peekl(head + XHEAD_BSS);
  Dis.offset = peekl(head + XHEAD_OFFSET);
  Dis.symbol = peekl(head + XHEAD_SYMBOL);

  validateXHeader(head);
}

// Z形式実行ファイルのヘッダを読み込む
static void loadZHeader(FILE* fp) {
  UBYTE head[ZHEAD_SIZE];

  if (fread(head, 1, sizeof(head), fp) != sizeof(head) ||
      peekw(head + ZHEAD_ID) != ZHEAD_ID_VALUE) {
    err("Z形式実行ファイルではありません。\n");
  }

  Dis.exec = Dis.base = (address)peekl(head + ZHEAD_BASE);
  Dis.text = peekl(head + ZHEAD_TEXT);
  Dis.data = peekl(head + ZHEAD_DATA);
  Dis.bss = peekl(head + ZHEAD_BSS);
}

// (Human68k) 実行ファイルのヘッダを読み込む
void loadHeaderHuman(FILE* fp, ULONG fileSize) {
  switch (Dis.fileType) {
    default:
      internalError(__FILE__, __LINE__, "Dis.fileType");

    case FILETYPE_DUMP:
    case FILETYPE_R:
      Dis.text = fileSize;
      break;
    case FILETYPE_X:
      loadXHeader(fp);
      break;
    case FILETYPE_Z:
      loadZHeader(fp);
      break;

    case FILETYPE_OSK:
      err("OS-9/X680x0のモジュールには対応していません。\n");
      break;
  }

  Dis.beginTEXT = Dis.base;
  Dis.beginDATA = Dis.beginTEXT + Dis.text;
  Dis.beginBSS = Dis.beginDATA + Dis.data;
  Dis.LAST = Dis.beginSTACK = Dis.beginBSS + Dis.bss;
  Dis.availableTextEnd = (Dis.D ? Dis.beginBSS : Dis.beginDATA);

  Dis.actions = &actionsHuman;
}

// EOF
