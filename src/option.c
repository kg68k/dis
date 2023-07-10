// ソースコードジェネレータ
// Option analyze , etc
// Copyright (C) 1989,1990 K.Abe, 1994 R.ShimiZu
// All rights reserved.
// Copyright (C) 1997-2023 TcbnErik

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __LIBC__
#include <sys/xglob.h>
static char* toslash(char* s) { return _toslash(s); }
#else
static char* toslash(char* s) { return s; }
#endif

#include "disasm.h"
#include "estruct.h"
#include "etc.h"
#include "generate.h"
#include "getopt.h"
#include "global.h"
#include "hex.h"
#include "option.h"
#include "output.h"
#include "version.h"

#define TO_STRING(n) #n
#define S(n) TO_STRING(n)

#define DETERMINISTIC "deterministic"

// -Q、--deterministic の状態
enum {
  HEAD_OPT_UNSPECIFIED = 0,
  HEAD_OPT_ENGAGED,
  HEAD_OPT_ENABLED,
};

// static 関数プロトタイプ
static void option_switch(int opt);
static void initFpuOption(void);
static char* dupAndToslash(const char* s);
static int ck_atoi(const char* s);
static int getOptionInt(const char* s, int min, int max, int optionChar);
static int getOptionInt0(const char* s, int max, int optionChar);
static int getLongOptionInt0(const char* s, int max, const char* optionName);
static NORETURN void usage(void) GCC_NORETURN;
static void printTitle(void);

static void validateProcessorOption(void) {
  mputypes mpu = Dis.mpu;

  if ((Dis.mmu & MMU851) && !(mpu & M020))
    err("-m68851 には -m68020 が必要です。\n");

  if ((Dis.fpu & F88x) && !(mpu & (M020 | M030)))
    err("-m6888x は -m68020/68030 が必要です。\n");

  if (mpu & (M040 | M060)) {
    if (Dis.fpuidTable[CPID_CACHE_MMU])
      err("-m6888x,%d と -m68040/68060 は併用できません。\n", CPID_CACHE_MMU);

    if (Dis.fpuidTable[CPID_MOVE16])
      err("-m6888x,%d と -m68040/68060 は併用できません。\n", CPID_MOVE16);
  }

  if ((mpu & M060) || Dis.cpu32) {
    if (Dis.fpuidTable[CPID_CPU32])
      err("-m6888x,4 と -m68060/cpu32 は併用できません。\n");
  }
}

// 環境変数に設定されたオプションを解析する
static void analyzeEnvOption(char* argv0, const char* optionList) {
  char *envs, *envp;    // envs=固定, envp=作業用
  char **args, **argp;  // args=固定, argp=作業用
  int count, cnt;
  int c;

  char* s = getenv(ENV_DIS_OPT);
  if (s == NULL) s = getenv(ENV_dis_opt);
  if (s == NULL) return;

  Dis.dis_opt = s;

  // 引数をスペースで分割する
  while (*s == ' ') s++;
  envp = envs = Malloc(strlen(s) + 1);
  for (count = 1; *s; count++) {
    while (*s && (*s != ' ')) *envp++ = *s++;
    *envp++ = '\0';
    while (*s == ' ') s++;
  }

  // 各引数へのポインタ配列を作る
  argp = args = Malloc((count + 1) * sizeof(char*));
  envp = envs;
  *argp++ = argv0;  // 自分自身のプログラム名
  for (cnt = count; --cnt;) {
    *argp++ = envp;
    while (*envp++)
      ;
  }
  *argp = NULL;

  // オプション解析
  // optind = -1;
  while ((c = dis_getopt(count, args, optionList)) != EOF) option_switch(c);

  // 忘れずに解放
  Mfree(args);
  Mfree(envs);
}

// コマンドラインのオプションを解析する
static void analyzeCmdOption(int argc, char* argv[], const char* optionList) {
  int fileind;
  int c;

  optind = -1;
  while ((c = dis_getopt(argc, argv, optionList)) != EOF) option_switch(c);
  fileind = optind;

  while (fileind < argc) {
    char* f = argv[fileind++];

    if (Dis.execFile == NULL)
      Dis.execFile = f;
    else if (Dis.outputFile == NULL)
      Dis.outputFile = f;
    else
      err("ファイル名が多すぎます。\n");
  }
}

// コマンドラインの先頭にのみ指定できるオプションを仮解釈する
static void analyzeHeadOption(int argc, char* argv[]) {
  char* s = (argc >= 2) ? argv[1] : NULL;
  if (s == NULL) return;

  if (s[0] == '-') {
    if (s[1] == 'Q') Dis.Q = HEAD_OPT_ENGAGED;
    if (strcmp(s, "--" DETERMINISTIC) == 0) {
      Dis.deterministic = HEAD_OPT_ENGAGED;
      Dis.Q = HEAD_OPT_ENABLED;
    }
  }
}

/*

  オプションを解析する

*/
void analyze_args(int argc, char* argv[]) {
  static const char optionlist[] =
      "a::b:cde::fg::hijklm:n:o:pq::rs::u::vw:xyz:"
      "ABC::DEFGIK:L:MNP:QR:S::T::UV:W:XYZ::#:-:";

  analyzeHeadOption(argc, argv);

  if (Dis.Q == HEAD_OPT_UNSPECIFIED) {
    analyzeEnvOption(argv[0], optionlist);
  }
  analyzeCmdOption(argc, argv, optionlist);

  if ((Dis.execFile == NULL) || (*Dis.execFile == '\0')) {
    printTitle();
    err("処理対象の実行ファイル名が指定されていません。\n"
        "--help オプションで使用法を表示します。\n");
  }
  if ((Dis.outputFile == NULL) || (*Dis.outputFile == '\0'))
    Dis.outputFile = "-";

  validateProcessorOption();

  if (Dis.isp && (Dis.mpu & M060)) Dis.mpu |= MISP;

  if (Dis.fpsp) {
    fputypes fpu = Dis.fpu;
    if (fpu & F040) fpu |= F4SP;
    if (fpu & F060) fpu |= F6SP;
    Dis.fpu = fpu;
  }

  // -x指定時は-aのタブ位置を右にずらす
  if (Dis.x) Dis.Atab += 2;

  // テーブルファイル読み込み時はラベルファイルも自動的に読み込む
  if (Dis.T) Dis.g = TRUE;

  /* ラベルファイル/テーブルファイル名を作成する */
  {
    char* file = strcmp("-", Dis.outputFile) ? Dis.outputFile : "aout";
    char* buf = dupAndToslash(file);
    size_t len;

    {
      char* p = strrchr(buf, '/');
      p = p ? (p + 1) : buf;
      p = strrchr((*p == '.') ? (p + 1) : p, '.');
      if (p) *p = '\0'; /* 拡張子を削除する */
    }
    len = strlen(buf) + 4 + 1;

    if (Dis.g && Dis.inputLabelFile == NULL) {
      Dis.inputLabelFile = Malloc(len);
      strcat(strcpy(Dis.inputLabelFile, buf), ".lab");
    }
    if (Dis.e && Dis.outputLabelFile == NULL) {
      Dis.outputLabelFile = Malloc(len);
      strcat(strcpy(Dis.outputLabelFile, buf), ".lab");
    }
    if (Dis.T && Dis.tableFile == NULL) {
      Dis.tableFile = Malloc(len);
      strcat(strcpy(Dis.tableFile, buf), ".tab");
    }

    Mfree(buf);
  }
}

static void analyzeOption_a(const char* s) {
  Dis.addressCommentLine = (s ? ck_atoi(s) : 5);
}

static void analyzeOption_b(const char* s, int optionChar) {
  Dis.branchSize = getOptionInt0(s, BRANCH_SIZE_NOT_OMIT, optionChar);
}

enum {
  MPUOPT_MPU,
  MPUOPT_CPU32,
  MPUOPT_FPU,
  MPUOPT_MMU,
};

typedef struct {
  char s[6];
  uint8_t type;
  uint8_t mpu;
  uint8_t fpu;
  uint8_t mmu;
} MpuOption;

static const MpuOption mpuOptions[] = {
    {"68000", MPUOPT_MPU, M000, 0, 0},
    {"68008", MPUOPT_MPU, M000, 0, 0},
    {"68010", MPUOPT_MPU, M010, 0, 0},
    {"68020", MPUOPT_MPU, M020, 0, 0},
    {"68030", MPUOPT_MPU, M030, 0, MMU030},
    {"68040", MPUOPT_MPU, M040, F040, MMU040},
    {"68060", MPUOPT_MPU, M060, F060, MMU060},
    {"680x0", MPUOPT_MPU, M000 | M010 | M020 | M030 | M040 | M060, F040 | F060,
     MMU030 | MMU040 | MMU060},
    {"cpu32", MPUOPT_CPU32, 0, 0, 0},
    {"68881", MPUOPT_FPU, 0, F881, 0},
    {"68882", MPUOPT_FPU, 0, F882, 0},
    {"68851", MPUOPT_MMU, 0, 0, MMU851},
};

static const MpuOption* getMpuOption(const char* s, const char** nextptr) {
  size_t count = _countof(mpuOptions);
  size_t i;

  *nextptr = NULL;

  for (i = 0; i < count; ++i) {
    const MpuOption* mo = &mpuOptions[i];
    size_t len = strlen(mo->s);
    char c;

    if (strncmp(s, mo->s, len) != 0) continue;
    c = s[len];
    if (c != '\0' && c != ',') continue;

    if (c != '\0') *nextptr = &s[len + 1];
    return mo;
  }

  return NULL;
}

// -m オプション
//   -m68000 等は以前の -m の指定を消去するので、複数指定する場合は
//   -m68000,68020 のように ',' で繋げる。
//   -mcpu32 -m6888x -m68851 は消去せず追加の指定として扱う。
//   -m6888x[,id] は他の指定を繋げることができない。
static void analyzeOption_m(const char* s) {
  boolean init = TRUE;

  do {
    const char* next = NULL;
    const MpuOption* mo = getMpuOption(s, &next);

    if (mo == NULL) {
      err("対応していない MPU または FPU です(-m)。\n");
      return;
    }
    s = next;  // ','の次

    if (mo->type == MPUOPT_CPU32) {
      Dis.cpu32 = TRUE;
      continue;
    }

    if (mo->type == MPUOPT_MPU && init) {
      init = FALSE;
      Dis.mpu = 0;
      Dis.mmu = 0;
      initFpuOption();
    }
    Dis.mpu |= mo->mpu;
    Dis.fpu |= mo->fpu;
    Dis.mmu |= mo->mmu;
    if (mo->fpu & (F040 | F060)) Dis.fpuidTable[CPID_FPU] = 1;

    // -m6888x の場合だけ、','の次はコプロセッサID
    if (mo->type == MPUOPT_FPU) {
      int fpuid = s ? ck_atoi(s) : CPID_FPU;

      // 6888x を外付けできる 68020/68030 では id=0 は MMU なので指定できない
      if (fpuid <= CPID_MMU || 7 < fpuid) err("FPU IDが範囲外の値です(-m)。\n");

      Dis.fpuidTable[fpuid] = 1;
      return;
    }
  } while (s && s[0] != '\0');
}

static void analyzeOption_o(const char* s, int optionChar) {
  Dis.stringWidth =
      getOptionInt(s, STRING_WIDTH_MIN, STRING_WIDTH_MAX, optionChar);
}

static void analyzeOption_q(const char* s, int optionChar) {
  Dis.q = TRUE;

  if (s) {
    Dis.quietTable = getOptionInt0(s, 1, optionChar);
  }
}

static void analyzeOption_s(const char* s, int optionChar) {
  Dis.outputSymbol = getOptionInt0(s, OUTPUT_SYMBOL_ALL, optionChar);
}

static void analyzeOption_u(const char* s) {
  Dis.acceptUnusedTrap = TRUE;
  if (s && (ck_atoi(s) == 1)) Dis.sxWindow = TRUE;
}

static void analyzeOption_w(const char* s, int optionChar) {
  Dis.dataWidth = getOptionInt(s, DATA_WIDTH_MIN, DATA_WIDTH_MAX, optionChar);
}

static void analyzeOption_z(const char* s) {
  char* comma = strchr(s, ',');
  address base = (address)atox(s);
  address exec = comma ? (address)atox(comma + 1) : base;

  if (exec < base)
    err("ベースアドレスより小さい実行アドレスは指定できません(-z)。\n");
  if (isOdd(base | exec)) err("奇数アドレスは指定できません(-z)。\n");

  Dis.fileType = FILETYPE_DUMP;
  Dis.base = base;
  Dis.exec = exec;
}

static void analyzeOption_C(const char* s, int optionChar) {
  Dis.symbolColonNum = getOptionInt0(s, SYMBOL_COLON_TWO, optionChar);
}

static void analyzeOption_K(const char* s) {
  uint8_t c = s[0];
  if (!c || s[1]) err("コメントキャラクタは一文字しか指定出来ません(-K)。\n");

  Dis.commentStr[0] = c;
}

static void analyzeOption_L(const char* s) {
  uint8_t c = s[0];
  if (!c || s[1]) err("ラベルの先頭文字は一文字しか指定出来ません(-L)。\n");
  if (!isdigit(c)) err("ラベルの先頭文字に数字は使えません(-L)。\n");

  Dis.labelChar = c;
}

static void analyzeOption_P(const char* s, int optionChar) {
  int n = getOptionInt0(s, 3, optionChar);

  Dis.fpsp = (n & 1) ? TRUE : FALSE;
  Dis.isp = (n & 2) ? TRUE : FALSE;
}

static void analyzeOption_R(const char* s, int optionChar) {
  int n = getOptionInt0(s, 0x0f, optionChar);

  Dis.undefReg = (n & 1) ? TRUE : FALSE;
  Dis.undefExReg = (n & 2) ? TRUE : FALSE;
  Dis.undefExScale = (n & 4) ? TRUE : FALSE;
  Dis.undefExSize = (n & 8) ? TRUE : FALSE;
}

static void analyzeOption_S(const char* s) {
  Dis.outputSplitByte = (s ? ck_atoi(s) : 64) * 1024;
}

static void analyzeOption_V(const char* s, int optionChar) {
  Dis.backtrackReason = getOptionInt0(s, BACKTRACK_REASON_ALL, optionChar);
}

static void analyzeOption_Z(const char* s, int optionChar) {
  Dis.suppressDollar = getOptionInt0(s, 1, optionChar);
  Dis.Z = TRUE;
}

static void analyzeOption_deterministic(const char* optionName) {
  DisVars* d = &Dis;

  if (Dis.deterministic != HEAD_OPT_ENGAGED)
    err("--%s はコマンドラインの先頭に指定する必要があります。\n", optionName);

  d->deterministic = HEAD_OPT_ENABLED;
  d->h = TRUE;
  d->q = TRUE;
  d->N = TRUE;
  d->Z = TRUE;
  d->suppressDollar = 1;
  d->a7ToSp = TRUE;
  d->stripIncludePath = TRUE;
}

static void analyzeOption_reltblZero(const char* s, const char* optionName) {
  Dis.reltblZero = getLongOptionInt0(s, RELTBL_ZERO_ALL, optionName);
}

static void analyzeOption_movemZero(const char* s, const char* optionName) {
  Dis.movemZero = getLongOptionInt0(s, 1, optionName);
}

enum {
  LONGOPT_VERSION,
  LONGOPT_HELP,
  LONGOPT_OVERWRITE,
  LONGOPT_FPSP,
  LONGOPT_ISP,
  LONGOPT_NO_FPU,
  LONGOPT_NO_MMU,
  LONGOPT_SP,
  LONGOPT_OLD_SYNTAX,
  LONGOPT_INREAL,
  LONGOPT_HEADER,
  LONGOPT_DOSCALL_MAC,
  LONGOPT_IOCSCALL_MAC,
  LONGOPT_FEFUNC_MAC,
  LONGOPT_STRIP_PATH,
  LONGOPT_DETERMINISTIC,
  LONGOPT_RELTBL_ZERO,
  LONGOPT_MOVEM_ZERO,
  LONGOPT_FILETYPE,
};

typedef struct {
  char s[22];
  uint8_t type;
  uint8_t hasParam;  // --header= など、引数を持つオプション
  int val;  // --fpsp(TRUE)、--no-fpsp(FALSE) など、コード共用のための値
} LongOption;

static const LongOption longOptions[] = {
    {"version", LONGOPT_VERSION, FALSE, 0},
    {"help", LONGOPT_HELP, FALSE, 0},  //
    {"overwrite", LONGOPT_OVERWRITE, FALSE, 0},
    {"fpsp", LONGOPT_FPSP, FALSE, TRUE},
    {"no-fpsp", LONGOPT_FPSP, FALSE, FALSE},
    {"isp", LONGOPT_ISP, FALSE, TRUE},  //
    {"no-isp", LONGOPT_ISP, FALSE, FALSE},
    {"no-fpu", LONGOPT_NO_FPU, FALSE, 0},  //
    {"no-mmu", LONGOPT_NO_MMU, FALSE, 0},  //
    {"sp", LONGOPT_SP, FALSE, TRUE},       //
    {"a7", LONGOPT_SP, FALSE, FALSE},
    {"old-syntax", LONGOPT_OLD_SYNTAX, FALSE, TRUE},
    {"new-syntax", LONGOPT_OLD_SYNTAX, FALSE, FALSE},
    {"inreal", LONGOPT_INREAL, FALSE, TRUE},
    {"real", LONGOPT_INREAL, FALSE, FALSE},
    {"header", LONGOPT_HEADER, TRUE, 0},  //
    {"include-doscall-mac", LONGOPT_DOSCALL_MAC, TRUE, 0},
    {"exclude-doscall-mac", LONGOPT_DOSCALL_MAC, FALSE, 0},
    {"include-iocscall-mac", LONGOPT_IOCSCALL_MAC, TRUE, 0},
    {"exclude-iocscall-mac", LONGOPT_IOCSCALL_MAC, FALSE, 0},
    {"include-fefunc-mac", LONGOPT_FEFUNC_MAC, TRUE, 0},
    {"exclude-fefunc-mac", LONGOPT_FEFUNC_MAC, FALSE, 0},
    {"strip-include-path", LONGOPT_STRIP_PATH, FALSE, 0},
    {DETERMINISTIC, LONGOPT_DETERMINISTIC, FALSE, 0},
    {"reltbl-zero", LONGOPT_RELTBL_ZERO, TRUE, 0},
    {"movem-zero", LONGOPT_MOVEM_ZERO, TRUE, 0},
    {"x", LONGOPT_FILETYPE, FALSE, FILETYPE_X},
    {"r", LONGOPT_FILETYPE, FALSE, FILETYPE_R},
    {"z", LONGOPT_FILETYPE, FALSE, FILETYPE_Z},
};

static const LongOption* getLongOption(const char* s, const char** paramptr) {
  size_t count = _countof(longOptions);
  size_t i;

  *paramptr = NULL;

  for (i = 0; i < count; ++i) {
    const LongOption* lo = &longOptions[i];

    if (lo->hasParam) {
      size_t len = strlen(lo->s);
      char c;

      if (strncmp(s, lo->s, len) != 0) continue;
      c = s[len];
      if (c != '\0' && c != '=') continue;

      if (c != '\0') *paramptr = &s[len + 1];
      return lo;
    } else {
      if (strcmp(s, lo->s) == 0) return lo;
    }
  }

  return NULL;
}

static boolean analyzeLongOption(const char* s) {
  const char* param = NULL;
  const LongOption* lo = getLongOption(s, &param);

  if (lo == NULL) return FALSE;

  switch (lo->type) {
    case LONGOPT_VERSION:
      puts(ProgramAndVersion);
      exit(EXIT_SUCCESS);
      break;
    case LONGOPT_HELP:
      usage();
      break;
    case LONGOPT_OVERWRITE:
      Dis.overwrite = TRUE;
      break;
    case LONGOPT_FPSP:
      Dis.fpsp = lo->val;
      break;
    case LONGOPT_ISP:
      Dis.isp = lo->val;
      break;
    case LONGOPT_NO_FPU:
      initFpuOption();
      break;
    case LONGOPT_NO_MMU:
      Dis.mmu = 0;
      break;
    case LONGOPT_SP:
      Dis.a7ToSp = lo->val;
      break;
    case LONGOPT_OLD_SYNTAX:
      Dis.oldSyntax = lo->val;
      break;
    case LONGOPT_INREAL:
      Dis.inreal = lo->val;
      break;
    case LONGOPT_HEADER:
      Dis.headerFile = dupAndToslash(param);
      break;
    case LONGOPT_DOSCALL_MAC:
      Dis.doscallMac = dupAndToslash(param);
      break;
    case LONGOPT_IOCSCALL_MAC:
      Dis.iocscallMac = dupAndToslash(param);
      break;
    case LONGOPT_FEFUNC_MAC:
      Dis.fefuncMac = dupAndToslash(param);
      break;
    case LONGOPT_STRIP_PATH:
      Dis.stripIncludePath = TRUE;
      break;
    case LONGOPT_DETERMINISTIC:
      analyzeOption_deterministic(lo->s);
      break;
    case LONGOPT_RELTBL_ZERO:
      analyzeOption_reltblZero(param, lo->s);
      break;
    case LONGOPT_MOVEM_ZERO:
      analyzeOption_movemZero(param, lo->s);
      break;
    case LONGOPT_FILETYPE:
      Dis.fileType = lo->val;
      Dis.exec = Dis.base = (address)0;
      break;
  }
  return TRUE;
}

static char* dupAndToslash(const char* s) {
  if (s == NULL) return NULL;

  return toslash(strcpy(Malloc(strlen(s) + 1), s));
}

static void initFpuOption(void) {
  Dis.fpu = 0;
  memset(Dis.fpuidTable, 0, sizeof(Dis.fpuidTable));
}

static void option_switch(int opt) {
  switch (opt) {
    case 'a':
      analyzeOption_a(optarg);
      break;
    case 'b':
      analyzeOption_b(optarg, opt);
      break;
    case 'c':
      Dis.c = TRUE;
      break;
    case 'd':
      Dis.d = TRUE;
      break;
    case 'e':
      Dis.e = TRUE;
      if (optarg) Dis.outputLabelFile = optarg;
      break;
    case 'f':
      Dis.f = TRUE;
      break;
    case 'g':
      Dis.g = TRUE;
      if (optarg) Dis.inputLabelFile = optarg;
      break;
    case 'h':
      Dis.h = TRUE;
      break;
    case 'i':
      Dis.i = TRUE;
      break;
    case 'j':
      Dis.acceptAddressError = TRUE;
      break;
    case 'k':
      Dis.k = TRUE;
      break;
    case 'l':
      Dis.l = TRUE;
      break;
    case 'm':
      analyzeOption_m(optarg);
      break;
    case 'n':
      Dis.stringLengthMin = ck_atoi(optarg);
      break;
    case 'o':
      analyzeOption_o(optarg, opt);
      break;
    case 'p':
      Dis.p = TRUE;
      break;
    case 'q':
      analyzeOption_q(optarg, opt);
      break;
    case 'r':
      Dis.r = TRUE;
      break;
    case 's':
      analyzeOption_s(optarg, opt);
      break;
    case 'u':
      analyzeOption_u(optarg);
      break;
    case 'v':
      Dis.v = TRUE;
      break;
    case 'w':
      analyzeOption_w(optarg, opt);
      break;
    case 'x':
      Dis.x = TRUE;
      break;
    case 'y':
      Dis.y = TRUE;
      break;
    case 'z':
      analyzeOption_z(optarg);
      break;

    case 'A':
      Dis.mnemonicAbbreviation = TRUE;
      break;
    case 'B':
      Dis.B = TRUE;
      break;
    case 'C':
      analyzeOption_C(optarg, opt);
      break;
    case 'D':
      Dis.D = TRUE;
      break;
    case 'E':
      Dis.E = TRUE;
      break;
    case 'F':
      Dis.dbraToDbf = TRUE;
      break;
    case 'G':
      Dis.argAfterCall = TRUE;
      break;
    case 'I':
      Dis.I = TRUE;
      break;
    case 'K':
      analyzeOption_K(optarg);
      break;
    case 'L':
      analyzeOption_L(optarg);
      break;
    case 'M':
      Dis.M = TRUE;
      break;
    case 'N':
      Dis.N = TRUE;
      break;
    case 'P':
      analyzeOption_P(optarg, opt);
      break;
    case 'Q':
      if (Dis.Q != HEAD_OPT_ENGAGED)
        err("-Q はコマンドラインの先頭に指定する必要があります。\n");
      Dis.Q = HEAD_OPT_ENABLED;
      break;
    case 'R':
      analyzeOption_R(optarg, opt);
      break;
    case 'S':
      analyzeOption_S(optarg);
      break;
    case 'T':
      Dis.T = TRUE;
      if (optarg) Dis.tableFile = optarg;
      break;
    case 'U':
      Dis.U = TRUE;
      /* fall through */
    case 'X':
      toUpperMemory(sizeof(Hex), Hex);
      break;
    case 'V':
      analyzeOption_V(optarg, opt);
      break;
    case 'W':
      Dis.compressLen = ck_atoi(optarg);
      break;
    case 'Y':
      Dis.Y = TRUE;
      break;
    case 'Z':
      analyzeOption_Z(optarg, opt);
      break;
    case '#':
      // デバッグ表示用オプション
      break;
    case '-':
      if (!analyzeLongOption(optarg))
        err("対応していないオプションです: '--%s'\n", optarg);
      break;

    default:
      errorExit();
      break;
  }
  return;
}

static int getIntFromOptarg(const char* s, int min, int max, int oc,
                            const char* os) {
  int n = s ? ck_atoi(s) : 0;

  if (n < min || max < n) err("指定できない値です(-%c%s)。\n", oc, os);
  return n;
}

// オプションの引数を int に変換して返す
//   min～max の範囲でなければエラー終了する。
static int getOptionInt(const char* s, int min, int max, int optionChar) {
  return getIntFromOptarg(s, min, max, optionChar, "");
}

// オプションの引数を int に変換して返す
//   0～max の範囲でなければエラー終了する。
static int getOptionInt0(const char* s, int max, int optionChar) {
  return getIntFromOptarg(s, 0, max, optionChar, "");
}

// ロングオプションの引数を int に変換して返す
//   0～max の範囲でなければエラー終了する。
static int getLongOptionInt0(const char* s, int max, const char* optionName) {
  return getIntFromOptarg(s, 0, max, '-', optionName);
}

static int ck_atoi(const char* s) {
  const char* p = s;
  char c;

  while ((c = *p++) != '\0') {
    if (!isdigit(c)) err("数値の指定が正しくありません。\n");
  }

  return atoi(s);
}

static const char usageText[] =
    "usage: dis [option] 実行ファイル名 [出力ファイル名]\n"
    "\n"
    "ファイル入力オプション:\n"
    "-d              デバイスドライバの時に指定\n"
    "-g[file]        ラベルファイルを読み込む\n"
    "-z base,exec    実行ファイルを base からのバイナリファイルとみなし、exec から解析する\n"
    "-T[file]        テーブル記述ファイルを読み込む\n"
    "-Y              インクルードファイルをカレントディレクトリからも検索する\n"
    "--include-XXX-mac=file  インクルードファイルの指定 (XXX = doscall, iocscall, fefunc)\n"
    "--exclude-XXX-mac       インクルードファイルを読み込まない\n"
    "--header=file   ヘッダをファイルから読み込む(環境変数 " ENV_dis_header " より優先)\n"
#ifdef PRINT_HIDDEN_OPTIONS
    "--x             実行ファイルを X 形式と見なす\n"
    "--r             実行ファイルを R 形式と見なす\n"
    "--z             実行ファイルを Z 形式と見なす\n"
#endif

    "\n"
    "ファイル出力オプション:\n"
    "-e[file]        ラベルファイルの出力\n"
    "-v              単なる逆アセンブルリストの出力\n"
    "-S[num]         出力ファイルを num KB ごとに分割する(num を省略すると 64KB)\n"
    "--overwrite     ファイルを無条件で上書きする\n"

    "\n"
    "命令セットオプション:\n"
    "-m 680x0[,...]  逆アセンブル対象の MPU を指定(68000-68060, 680x0)\n"
    "-m 68851        68851 命令を有効にする(-m68020 指定時のみ有効)\n"
    "-m 6888x[,id]   有効な FPCP とその ID を指定(68881/68882, id=[1]-7)\n"
    "--no-fpsp       未実装浮動小数点命令を無効にする(--fpsp で取り消し)\n"
    "--no-isp        未実装整数命令を無効にする(--isp で取り消し)\n"
    "--no-fpu        内蔵 FPU 命令を無効にする(-m68040～68060 の後に指定)\n"
    "--no-mmu        内蔵 MMU 命令を無効にする(-m68030～68060 の後に指定)\n"

    "\n"
    "解析オプション:\n"
    "-c              ラベルチェックを行わない\n"
    "-f              バイト操作命令の不定バイトのチェック($00 or $ff "
    "?)をしない\n"
    "-h              データ領域中の $4e75(rts) の次のアドレスに注目する\n"
    "-i              分岐先で未定義命令があってもデータ領域と見なさない\n"
    "-j              "
    "アドレスエラーが起こるであろう命令を未定義命令と見なさない\n"
    "-k              命令の中を指すラベルはないものと見なす\n"
    "-l              "
    "プログラム領域が見つからなくなるまで何度も探すことをしない\n"
    "-n num          "
    "文字列として判定する最小の長さ。0なら判定しない(初期値=3)\n"
    "-p              データ領域中のプログラム領域を判別しない\n"
    "-u[num]         未使用の A,F line trap を未定義命令と見なさない(1:SX-Window 対応)\n"
    "-y              全てのデータ領域をプログラム領域でないか確かめることをしない\n"
    "-D              データセクション中にもプログラムを認める\n"
    "-E              バイト操作命令の不定バイトの書き換えチェックをしない\n"
    "-G              サブルーチンコールの直後に引数を置くプログラムを解析する\n"
    "-R num          未使用フィールドのチェック項目の指定(ビット指定、初期値=15)\n"
    "                +1 未使用レジスタフィールド(mul[us].l, ftst.x, c{inv,push}a)のチェック\n"
    "                +2 拡張アドレッシングでのサプレスされたレジスタフィールドのチェック\n"
    "                +4 サプレスされたインデックスレジスタに対するスケーリングのチェック\n"
    "                +8 サプレスされたインデックスレジスタに対するサイズ指定(.l)のチェック\n"
    "--reltbl-zero=num  リラティブオフセットテーブルに0を認める(0:しない 1:[先頭のみ] 2:全域)\n"
    "--movem-zero=num   movem #0 を未定義命令と見なさない(0:[しない] 1:する)\n"

    "\n"
    "アセンブリ表記オプション:\n"
    "-a[num]         num 行ごとにアドレスをコメントで入れる(num を省略すると5行ごと)\n"
    "-b num          相対分岐命令のサイズの出力(0:自動 1:常に省略 2:常に付ける)\n"
    "-o num          文字列領域の桁数(" S(STRING_WIDTH_MIN) "≦num≦" S(STRING_WIDTH_MAX)
                     " 初期値=" S(STRING_WIDTH_DEFAULT) ")\n"
    "-r              文字列に16進数のコメントを付ける\n"
    "-s[num]         シンボルテーブルの出力([0]:しない 1:[通常] 2:全て)\n"
    "-w num          データ領域の横バイト数(" S(DATA_WIDTH_MIN) "≦num≦" S(DATA_WIDTH_MAX)
                     " 初期値=" S(DATA_WIDTH_DEFAULT) ")\n"
    "-x              実際のオペコードを16進数のコメントで付ける\n"
    "-A              cmpi, movea 等を cmp, move 等にする\n"
    "-B              bra の後でも改行する\n"
    "-C[num]         ラベルの後のコロン(0:付けない 1:全てに1つ [2]:通常 3:全てに2つ)\n"
    "-F              dbra, fdbra を dbf, fdbf として出力する\n"
    "-K char         コメントの先頭文字を char にする(初期値=;)\n"
    "-L char         ラベル名の先頭文字を char にする(初期値=L)\n"
    "-M              cmpi, move, addi.b, subi.b #imm および pack, unpk にコメントを付ける\n"
    "-N              サイズがデフォルトなら付けない\n"
    "-U              ニーモニックと16進数を大文字で出力する\n"
    "-W num          同一データを .dcb で出力する最小バイト数。0なら圧縮しない(初期値=64)\n"
    "-X              16進数を大文字で出力する\n"
    "-Z[num]         16進数をゼロサプレスする([0]:通常 1:省略可能な'$'を省略)\n"
    "--inreal        浮動小数点を実数表記ではなく内部表現で出力する(--real で取り消し)\n"
    "--old-syntax    アドレッシングを旧表記で出力する(--new-syntax で取り消し)\n"
    "--sp            a7 レジスタを sp と表記する(--a7 で取り消し)\n"
    "--strip-include-path  .include にパス名を付けない\n"

    "\n"
    "その他:\n"
    "-q[num]         メッセージを出力しない([0]:通常 1:テーブルに関する情報も出力しない)\n"
    "-I              命令の中を差すラベルのアドレスを表示する\n"
    "-Q              環境変数 " ENV_DIS_OPT ", " ENV_dis_opt " を参照しない\n"
    "-V num          バックトラックの原因の表示(0:しない [1]:プログラム領域 2:全ての領域)\n"
    "--deterministic 決定論的逆アセンブルを行う\n"
    "--help          使用法を表示する\n"
    "--version       バージョンを表示する\n"
    "";

static void usage(void) {
  printTitle();
  fputs(usageText, stdout);

  exit(EXIT_SUCCESS);
}

static void printTitle(void) { fputs(Title, stdout); }

// EOF
