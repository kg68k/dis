// ソースコードジェネレータ
// 大域変数ヘッダ
// Copyright (C) 1989,1990 K.Abe
// All rights reserved.
// Copyright (C) 2025 TcbnErik

#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "disasm.h"
#include "estruct.h"
#include "opstr.h"

// 環境変数
#define ENV_DIS_OPT "DIS_OPT"
#define ENV_dis_opt "dis_opt"
#define ENV_dis_header "dis_header"
#define ENV_dis_include "dis_include"
#define ENV_include "include"

// branchSize
enum {
  BRANCH_SIZE_AUTO,
  BRANCH_SIZE_OMIT,
  BRANCH_SIZE_NOT_OMIT,
};

// outputSymbol
enum {
  OUTPUT_SYMBOL_OFF,
  OUTPUT_SYMBOL_NORMAL,
  OUTPUT_SYMBOL_ALL,
};

// symbolColonNum
enum {
  SYMBOL_COLON_OMIT,
  SYMBOL_COLON_ONE,
  SYMBOL_COLON_AUTO,
  SYMBOL_COLON_TWO,
};

// backtrackReason
enum {
  BACKTRACK_REASON_OFF,
  BACKTRACK_REASON_PROGRAM,
  BACKTRACK_REASON_ALL,
};

// fileType
enum {
  FILETYPE_AUTO,
  FILETYPE_DUMP,
  FILETYPE_X,
  FILETYPE_R,
  FILETYPE_Z,
  FILETYPE_OSK,
};

// reltblZero
enum {
  RELTBL_ZERO_REJECT,
  RELTBL_ZERO_HEAD,
  RELTBL_ZERO_ALL,
};

// dataWidth
#define DATA_WIDTH_MIN 1
#define DATA_WIDTH_MAX 32
#define DATA_WIDTH_DEFAULT 8

// stringWidth
#define STRING_WIDTH_MIN 1
#define STRING_WIDTH_MAX 200
#define STRING_WIDTH_DEFAULT 60

typedef struct {
  void (*registerOperandLabel)(disasm* code);
  void (*decodeTrap)(codeptr ptr, disasm* code);
  char* (*d16AnToLabel)(char* p, operand* op);
  void (*datagenWTable)(address pc, address pcend);
  void (*makeRelocateTable)(void);
  void (*parseSymbolTable)(ArrayBuffer*);
  void (*analyzeProgram)(void);
  void (*readIncludeFiles)(void);
  void (*modifyMnemonic)(OpStringTable* ops);
  void (*outputHeader)(char* filename, time_t filedate, const char* argv0,
                       char* comline, FILE* fpHeader);
  void (*generate)(char* sfilename);
} Actions;

typedef struct {
  // メモリ上に読み込んだ実行ファイルの内容
  codeptr Top;   // 実際にファイルのあるアドレス
  codeptr Ofst;  // 実際のアドレス - 仮想アドレス

  // セクションの情報
  address beginTEXT;  // = Dis.base
  address beginDATA;  // = Dis.base + Dis.text
  address beginBSS;   // = Dis.base + Dis.text + Dis.data
  address beginSTACK;
  address LAST;              // = Dis.base + Dis.text + Dis.data + Dis.bss
  address availableTextEnd;  // beginDATA or beginBSS

  // 実行ファイルの情報
  const Actions* actions;
  address base;
  address exec;
  ULONG text;
  ULONG data;
  ULONG bss;
  ULONG offset;
  ULONG symbol;
  time_t fileDate;

  // ファンクションコール(マクロ)のラベル
  char** dosLabel GCC_ALIGN(4);
  char** iocsLabel;
  char** fefuncLabel;
  char** sxLabel;

  // 環境変数の値
  char* dis_opt;

  // オプション設定
  char* execFile;
  char* outputFile;
  char* inputLabelFile;
  char* outputLabelFile;
  char* tableFile;
  const char* headerFile;

  // インクルードファイル
  const char* includePath;     // --include=<path>
  uint8_t doscall;             // --doscall-mac
  uint8_t iocscall;            // --iocscall-mac
  uint8_t fefunc;              // --fefunc-mac
  uint8_t sxcall;              // --sxcall-mac
  const char* doscallMac;      // --include-doscall-mac=<path>
  const char* iocscallMac;     // --include-iocscall-mac=<path>
  const char* fefuncMac;       // --include-fefunc-mac=<path>
  const char* sxcallMac;       // --include-sxcall-mac=<path>
  const char* doscallMacPath;  // 読み込んだファイルのフルパス名
  const char* iocscallMacPath;
  const char* fefuncMacPath;
  const char* sxcallMacPath;

  int addressCommentLine;  // -a
  int stringLengthMin;     // -n
  int stringWidth;         // -o
  int dataWidth;           // -w
  int outputSplitByte;     // -S
  int compressLen;         // -W

  uint8_t fpuidTable[8] GCC_ALIGN(4);
  mputypes mpu;
  fputypes fpu;
  mmutypes mmu;

  char commentStr[2];  // -K
  char labelChar;      // -L

  uint8_t branchSize;  // -b
  uint8_t c;           // ラベルチェックを行わない
  uint8_t d;           // デバイスドライバの時に指定
  uint8_t e;           // ラベルファイルの出力
  uint8_t f;           // バイト操作命令の不定バイトのチェックをしない
  uint8_t g;           // ラベルファイルを読み込む
  uint8_t h;           // データ領域中の$4e75(rts)の次のアドレスに注目する
  uint8_t i;           // 分岐先で未定義命令があってもデータ領域と見なさない
  uint8_t acceptAddressError;  // -j
  uint8_t k;                   // 命令の中を指すラベルはないものと見なす
  uint8_t l;  // プログラム領域が見つからなくなるまで何度も捜すことをしない
  uint8_t p;  // データ領域中のプログラム領域を判別しない
  uint8_t q;  // メッセージを出力しない
  uint8_t r;  // 文字列に16進数のコメントを付ける
  uint8_t outputSymbol;      // -s
  uint8_t acceptUnusedTrap;  // -u
  uint8_t v;                 // 単なる逆アセンブルリストの出力
  uint8_t x;                 // 実際のオペコードを16進数のコメントで付ける
  uint8_t y;  // 全てのデータ領域をプログラム領域でないか確かめることをしない

  uint8_t mnemonicAbbreviation;  // -A
  uint8_t B;                     // bra の後でも改行する
  uint8_t symbolColonNum;        // -C
  uint8_t D;                     // データセクション中にもプログラムを認める
  uint8_t E;             // バイト操作命令の不定バイトの書き換えチェックをしない
  uint8_t dbraToDbf;     // -F
  uint8_t argAfterCall;  // -G
  uint8_t I;             // 命令の中を差すラベルのアドレスを表示する
  uint8_t M;             // 即値命令にコメントをつける
  uint8_t N;             // サイズがデフォルトなら付けない
  uint8_t Q;             // 環境変数 DIS_OPT, dis_opt を参照しない
  uint8_t T;             // テーブル記述ファイルを読み込む
  uint8_t U;             // ニーモニックを大文字で出力する
  uint8_t backtrackReason;  // -V
  uint8_t Y;                // 16進数を大文字で出力する
  uint8_t Z;                // 16進数をゼロサプレスする

  uint8_t undefReg;      // -R1
  uint8_t undefExReg;    // -R2
  uint8_t undefExScale;  // -R4
  uint8_t undefExSize;   // -R8

  uint8_t a7ToSp;
  uint8_t cpu32;
  uint8_t deterministic;
  uint8_t fileType;
  uint8_t fpsp;
  uint8_t inreal;
  uint8_t isp;
  uint8_t oldSyntax;
  uint8_t overwrite;
  uint8_t quietTable;  // -q1
  uint8_t reltblZero;
  uint8_t stripIncludePath;
  uint8_t suppressDollar;  // -Z1
  uint8_t movemZero;

  // 動作モード等
  ArrayBuffer reltblArray GCC_ALIGN(4);
  ArrayBuffer symtblArray;

  // オプション以外の出力ソースコードの形式
  int UndefTab;  // ";undefined inst." のタブ位置(3)
  int Mtab;      // -Mのタブ位置(5)
  int Xtab;      // -xのタブ位置(7)
  int Atab;      // -aのタブ位置(8、-x指定時は10)
  opesize dbccSize;
  char quoteChar;  // 文字列を囲う記号(' or ")

  uint8_t hasSymbol;  // シンボルテーブル
  uint8_t needString;
  uint8_t reasonVerbose;
  uint8_t findLinkW;

#ifdef OSKDIS
  // ファンクションコール(マクロ)のラベル
  char** OS9label;   // OS9 F$???(I$???)
  char** MATHlabel;  // TCALL T$Math,T$???
  char** CIOlabel;   // TCALL CIO$Trap,C$???

  // 実行ファイルの情報
  UWORD oskModType;  // (oskType >> 8) & 0x0f
  UWORD oskSysRev;
  ULONG oskSize;
  ULONG oskOwner;
  address oskName;
  UWORD oskAccs;
  UWORD oskType;
  UWORD oskAttr;
  UWORD oskEdition;
  address oskUsage;
  address oskSymbol;
  address oskExcpt;
  ULONG oskMem;
  ULONG oskStack;
  address oskIdata;
  address oskIrefs;
  address oskInit;
  address oskTerm;

  // 読み込んだインクルードファイルのフルパス名
  const char* os9MacPath;
  const char* mathMacPath;
  const char* cioMacPath;
#endif

} DisVars;

extern DisVars Dis;

#endif

// EOF
