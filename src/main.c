// ソースコードジェネレータ
// メインルーチン
// Copyright (C) 1989,1990 K.Abe, 1994 R.ShimiZu
// All rights reserved.
// Copyright (C) 1997-2023 TcbnErik

#include <limits.h>  // PATH_MAX
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>  // time difftime

#ifdef _MSC_VER
#include <io.h>  // isatty()
#else
#include <unistd.h>
#endif

#ifdef __LIBC__
#include <sys/xstart.h>  // char* _comline
#define COMMAND_LINE _comline
#else
static char* make_cmdline(char** argv);
#define COMMAND_LINE make_cmdline(argv + 1)
#define NEED_MAKE_CMDLINE
#endif

#ifdef QUICK_YES_NO
#ifdef __HUMAN68K__
#include <conio.h>  // getch()
#else
#include <curses.h>
#endif
#endif /* QUICK_YES_NO */

#ifndef PATH_MAX
#define PATH_MAX _MAX_PATH
#endif

#include "analyze.h"
#include "disasm.h"
#include "eastr.h"  // initEAString()
#include "estruct.h"
#include "etc.h"
#include "generate.h"
#include "global.h"
#include "human.h"
#include "labelfile.h"
#include "opstr.h"  // initOpString()
#include "option.h"
#include "osk.h"
#include "table.h"
#include "version.h"

static void decideOutputFilename(const char* fileType, char** nameptr);

DisVars Dis;

// オプション初期化
static void initDisVars(void) {
  DisVars* d = &Dis;

  d->mpu = M000;
  // d->fpu = 0;
  // d->mmu = 0;

  d->execFile = NULL;
  d->outputFile = NULL;
  d->inputLabelFile = NULL;
  d->outputLabelFile = NULL;
  d->tableFile = NULL;

  d->doscallMac = "doscall.mac";
  d->iocscallMac = "iocscall.mac";
  d->fefuncMac = "fefunc.mac";

  d->stringLengthMin = 3;
  d->stringWidth = STRING_WIDTH_DEFAULT;
  d->dataWidth = DATA_WIDTH_DEFAULT;
  d->compressLen = 64;

  d->commentStr[0] = ';';
  d->labelChar = 'L';

  d->outputSymbol = OUTPUT_SYMBOL_NORMAL;
  d->symbolColonNum = SYMBOL_COLON_AUTO;
  d->backtrackReason = BACKTRACK_REASON_PROGRAM;

  d->undefReg = TRUE;
  d->undefExReg = TRUE;
  d->undefExScale = TRUE;
  d->undefExSize = TRUE;

  d->fpsp = TRUE;
  d->isp = TRUE;

  d->reltblZero = RELTBL_ZERO_HEAD;

  d->UndefTab = 3;
  d->Mtab = 5;
  d->Xtab = 7;
  d->Atab = 8;
  d->dbccSize = WORDSIZE;
  d->quoteChar = '\'';

  initReltblArray(&d->reltblArray);
}

// 実行ファイルの形式を判別する
static uint8_t getFileType(FILE* fp, const char* filename) {
  UBYTE head[2];
  char* ext = strrchr(filename, '.');

  // 拡張子が".r"ならR形式
  if (ext && strcasecmp(ext, ".r") == 0) return FILETYPE_R;

  if (fread(head, 1, sizeof(head), fp) == sizeof(head)) {
    UWORD h = peekw(head);

    fseek(fp, 0, SEEK_SET);

    if (h == XHEAD_ID_VALUE) return FILETYPE_X;
    if (h == ZHEAD_ID_VALUE) return FILETYPE_Z;
    if (h == OSKHEAD_ID_VALUE) return FILETYPE_OSK;
  }

  err("対応していないファイル形式です。\n");
}

// 実行ファイルのコード部を読み込む
static void readExeFile(FILE* fp) {
  // 命令デコード中にデータセクション末尾を超えてもいいように余分を確保する
  // move ([bd.l,an,ix],od.l),([bd.l,an,ix],od.l) で22バイトなのでそれ以上必要
  const size_t margin = 32;

  size_t bytes = Dis.text + Dis.data + Dis.offset + Dis.symbol;
  UBYTE* buffer = Malloc(bytes + margin);

  if (fread(buffer, 1, bytes, fp) != bytes) err("ファイルサイズが異常です。\n");

  memset(buffer + bytes, 0, margin);

  Dis.Top = buffer;
  Dis.Ofst = buffer - (ULONG)Dis.base;
}

// 実行ファイルを読み込む
static void loadExeFile(const char* filename) {
  struct stat st;
  FILE* fp = fopen(filename, "rb");

  if (fp == NULL) err("%s をオープンできません。\n", filename);
  if (fstat(fileno(fp), &st) != 0)
    err("%s のファイル情報が取得できません。\n", filename);
  Dis.fileDate = st.st_mtime;

  eprintf("%s を読み込みます。\n", filename);

  if (Dis.fileType == FILETYPE_AUTO) Dis.fileType = getFileType(fp, filename);

#ifdef OSKDIS
  loadHeaderOsk(fp, st.st_size);
#else
  loadHeaderHuman(fp, st.st_size);
#endif

  readExeFile(fp);
  fclose(fp);
}

typedef struct {
  int bit;
  char* name;
} TargetTable;

static void printTarget(char* str, int bits, int size, const TargetTable* tbl);

// 対象 MPU/MMU/FPU の表示
static void printTargetISA(void) {
  static const TargetTable mpu[] = {
      {M000, "68000"}, {M010, "68010"}, {M020, "68020"}, {M030, "68030"},
      {M040, "68040"}, {M060, "68060"}, {MISP, "060ISP"}  //
  };
  static const TargetTable mmu[] = {
      {MMU851, "68851"},
      {MMU030, "68030"},
      {MMU040, "68040"},
      {MMU060, "68060"}  //
  };
  static const TargetTable fpu[] = {
      {F881, "68881"},   {F882, "68882"}, {F040, "68040"},
      {F4SP, "040FPSP"}, {F060, "68060"}, {F6SP, "060FPSP"}  //
  };

  printTarget("MPU", Dis.mpu, _countof(mpu), mpu);
  printTarget("MMU", Dis.mmu, _countof(mmu), mmu);
  printTarget("FPU", Dis.fpu, _countof(fpu), fpu);
}

static void printTarget(char* str, int bits, int size, const TargetTable* tbl) {
  if (bits) {
    const char* comma = "";
    eprintf("対象%s: ", str);
    do {
      if (bits & tbl->bit) {
        eprintf("%s%s", comma, tbl->name);
        comma = ",";
      }
      tbl++;
    } while (--size);
    eputc('\n');
  }
}

/*

  絶対に必要なラベル(セクション先頭とプログラム末尾)を登録する

*/
static void register_default_labels(void) {
  regist_label(Dis.beginTEXT, DATLABEL | UNKNOWN);
  regist_label(Dis.beginDATA, DATLABEL | UNKNOWN);
  regist_label(Dis.beginBSS, DATLABEL | UNKNOWN);

  /* unregist されないように SYMLABEL を指定 */
  regist_label(Dis.LAST, DATLABEL | UNKNOWN | SYMLABEL);
}

// データ領域の中からプログラム領域を探す
static boolean researchDataLoop(boolean tweak) {
  int change = FALSE;

  do {
    int found = research_data(tweak);
    if (found == 0) break;
    change = TRUE;
    eprintf("{%d}", found);
  } while (!Dis.l);

  return change;
}

// 相対オフセットテーブルの宛先からプログラム領域を探す
static boolean analyzeReltblLoop(int countChar) {
  int change = FALSE;

  while (1) {
    if (analyzeRegisteredReltbl(&Dis.reltblArray) == 0) break;
    if (countChar) {
      eprintf(
          "\n相対オフセットテーブルの宛先からプログラム領域を探しています(%c)"
          "。",
          countChar);
      countChar = 0;
    }
    if (!researchDataLoop(FALSE)) break;
    change = TRUE;
  }

  return change;
}

// データ領域の中から各種の領域を繰り返し探す
static void analyzeLoop(void) {
  const uint8_t progInData = !Dis.p;

  if (progInData) {
    eprintf("\nデータ領域の中からプログラム領域を探しています(1)。");
    researchDataLoop(FALSE);
  }

  analyzeReltblLoop('1');

  if (Dis.stringLengthMin == 0) return;
  do {
    int found;
    boolean tweak = FALSE;

    eprintf("\n文字列を探しています。");
    found = search_string(Dis.stringLengthMin);
    if (found == 0)
      tweak = TRUE;
    else
      eprintf("{%d}", found);

    // 文字列が見つかってもデータ領域からプログラムは探さない
    if (!progInData) break;

    eprintf("\nデータ領域の中からプログラム領域を探しています(2)。");
    if (!researchDataLoop(tweak)) break;

    if (!analyzeReltblLoop('2') && tweak) break;
  } while (!Dis.l);
}

// 解析して生成
static void analyze_and_generate(const char* argv0, char* argv[]) {
  if (!Dis.g) {
    Dis.actions->analyzeProgram();
    eputc('\n');
  }

  eprintf("データ領域解析中です。");
  setReasonVerbose(BACKTRACK_REASON_ALL);
  analyze_data();
  analyzeLoop();

  if (!Dis.c) {
    eprintf("\nラベルチェック中。");
    search_operand_label();
  }

  if (Dis.e) {
    eprintf("\nラベルファイルを作成中です。");
    make_labelfile(Dis.execFile, Dis.outputLabelFile);
  }

  eprintf("\nソースを作成中です。");
  generate(Dis.execFile, Dis.outputFile, Dis.fileDate, argv0, COMMAND_LINE);
}

int main(int argc, char* argv[]) {
  time_t start_time = time(NULL);

  setbuf(stderr, NULL);

  initDisVars();
  analyze_args(argc, argv);
  fputs(Title, stderr);

  decideOutputFilename("ソースコード", &Dis.outputFile);
  if (Dis.e) decideOutputFilename("ラベル", &Dis.outputLabelFile);

  printTargetISA();

  loadExeFile(Dis.execFile);
  Dis.actions->readIncludeFiles();
  init_labelbuf();
  Dis.actions->makeRelocateTable();

  register_default_labels();
  if (Dis.g) {
    eprintf("ラベルファイルを読み込み中です。\n");
    read_labelfile(Dis.inputLabelFile);
  }

  if (Dis.T) {
    eprintf("テーブル記述ファイルを読み込み中です。\n");
    read_tablefile(Dis.tableFile);
  }

  Dis.actions->parseSymbolTable(&Dis.symtblArray);

  initEAString();
  initOpString();

  if (Dis.v) {
    eprintf("逆アセンブルリストを出力します。");
    disasmlist(Dis.outputFile);
  } else {
    const char* argv0 = Dis.deterministic ? "dis" : argv[0];
    analyze_and_generate(argv0, argv);
  }

  eprintf("\n終了しました。\n");
  {
    time_t finish_time = time(NULL);
    eprintf("所要時間: %d秒\n", (int)difftime(finish_time, start_time));
  }

  return 0;
}

/*

  出力ファイルの存在チェック

  返値: 0 = 出力可能
        1 = ファイル名を変更する

*/
static int check_exist_output_file(char* filename) {
  FILE* fp;

  /* --overwrite 指定時は無条件に上書き. */
  if (Dis.overwrite) return 0;

  /* 標準出力に出力するなら検査不要. */
  if (strcmp("-", filename) == 0) return 0;

  /* ファイルが存在しなければOK. */
  if ((fp = fopen(filename, "r")) == NULL) return 0;

  /* 端末デバイスへの出力ならOK. */
  if (isatty(fileno(fp))) {
    fclose(fp);
    return 0;
  }

  /* いずれでもなければキー入力. */
  fclose(fp);

#ifdef QUICK_YES_NO /* version 2.79 互換 */
  {
    int key;
    eprintf(
        "%s が既に存在します。\n"
        "Over Write ( Y or N )? ",
        filename);
    do key = toupper(getch());
    while (key != 'Y' && key != 'N');
    eprintf("%c\n", key);
    if (key == 'Y') return 0;
  }
#else  /* 安全の為 "yes" を入力させる. */
  {
    char buf[256];
    eprintf(
        "%s が既に存在します。\n"
        "上書きしますか？(Yes/No/Rename): ",
        filename);

    if (fgets(buf, sizeof buf, stdin)) {
      /* y, yes なら上書き */
      if (strcasecmp(buf, "y\n") == 0 || strcasecmp(buf, "yes\n") == 0)
        return 0;
      /* r, rename ならファイル名変更 */
      if (strcasecmp(buf, "r\n") == 0 || strcasecmp(buf, "rename\n") == 0)
        return 1;
    }
  }
#endif /* QUICK_YES_NO */

  // 上書きしないならプログラム終了
  errorExit();
}

// 出力ファイル名を変更する
static void change_filename(char** nameptr, const char* file) {
  char buf[PATH_MAX + 1];
  size_t len;

  eprintf("%sファイル名:", file);
  if (fgets(buf, sizeof buf, stdin) && (len = strlen(buf)) > 1) {
    buf[len - 1] = '\0';
    *nameptr = strcpy(Malloc(len + 1), buf);
    return;
  }
  errorExit();
}

// 出力ファイルに書き込めるどうかのチェック
//   書き込めない場合はエラー終了する
static void check_open_output_file(char* filename) {
  struct stat st;

  if (!strcmp("-", filename)) return;

  // ファイルがなければ、(恐らく)書き込みオープンできる
  if (stat(filename, &st) < 0) return;

  if ((st.st_mode & S_IWRITE) == 0) err("%s に書き込めません。\n", filename);
}

// 出力ファイル名を決定する
//   同名ファイルがあれば上書きするか別のファイル名に出力するか選択
//   上書きしない、書き込めない場合はプログラムをエラー終了する
static void decideOutputFilename(const char* fileType, char** nameptr) {
  while (check_exist_output_file(*nameptr)) {
    change_filename(nameptr, fileType);
  }
  check_open_output_file(*nameptr);
}

// LIBC 以外で使用する関数
#ifdef NEED_MAKE_CMDLINE
static char* make_cmdline(char** argv) {
  char** ap;
  char* buf;
  size_t size = 0;
  int needSpace = FALSE;

  for (ap = argv; *ap;) {
    size += strlen(*ap++) + 1;
  }
  buf = Malloc(size);
  buf[0] = '\0';

  for (ap = argv; *ap;) {
    if (needSpace) strcat(buf, " ");
    needSpace = TRUE;
    strcat(buf, *ap++);
  }

  return buf;
}
#endif

// EOF
