// ソースコードジェネレータ
// ソースコードジェネレートモジュール
// Copyright (C) 1989,1990 K.Abe, 1994 R.ShimiZu
// All rights reserved.
// Copyright (C) 2024 TcbnErik

#include "generate.h"

#include <ctype.h>  // isprint
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>  // getenv
#include <string.h>
#include <time.h>  // time ctime

#include "disasm.h"
#include "eastr.h"  // write_size
#include "estruct.h"
#include "etc.h"  // charout, <jctype.h>
#include "eval.h"
#include "fpconv.h"
#include "global.h"
#include "hex.h"
#include "include.h"
#include "label.h"  // search_label etc.
#include "mmu.h"
#include "offset.h"  // depend_address, nearadrs
#include "opstr.h"
#include "osk.h"
#include "output.h"
#include "symbol.h"  // symbol_search
#include "table.h"

/* Macros */
#define MIN_BIT(x) ((x) & -(x))

typedef struct {
  address adrs;
  LONG shift;
} AdrsShift;

// static 関数プロトタイプ
static address textgen(address, address);
static void byteout_for_xoption(address, ULONG, char*);
static void byteout_for_moption(disasm* code, char* buffer);
static char* operandToLabel(char* p, operand* op);
static void syntax_new_to_old(operand* op);

static void dataout(address, ULONG, opesize);
static address datagen(address, address, opesize);
static void datagen_sub(address, address, opesize);
static void strgen(address, address);
static void relgen(address, address, opesize);
static void zgen(address, address);
static address tablegen(address pc, int sectType);
static void tablegen_label(address pc, address pcend, lblbuf* lptr,
                           int sectType);
static void bssgen(address, address, opesize);
static char* mputype_numstr(mputypes m);

static mputypes Current_Mputype;

// ラベルorシンボル名(±オフセット)形式のラベル式をバッファに書き込む
//   文字列末尾のアドレスを返す
char* make_proper_symbol(char* buf, address adrs) {
  SymbolLabelFormula slfml;

  makeSymLabFormula(&slfml, adrs);
  return catSlfml(buf, &slfml);
}

// コメント行を書き出す
void com(const char* fmt, ...) {
  char buf[512];
  va_list ap;

  va_start(ap, fmt);
  vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);

  otherDirective2(Dis.commentStr, buf);
}

// コメント記号だけの行を書き出す
//   com("") で warning: zero-length format string になるのを回避。
void comBlank(void) { otherDirective(Dis.commentStr); }

// .cpu疑似命令を書き出す
void outputHeaderCpu(void) {
  otherDirective2(OpString.cpu, mputype_numstr(Current_Mputype));
}

static FILE* open_header_file(void) {
  FILE* fp;
  const char* fname = Dis.headerFile;

  if (fname == NULL || fname[0] == '\0') {
    if (!Dis.deterministic) fname = getenv(ENV_dis_header);
  }
  if (fname == NULL || fname[0] == '\0') {
    return NULL;
  }

  fp = fopen(fname, "rt");
  if (fp == NULL) {
    err("\nヘッダ記述ファイルをオープンできません: %s\n", fname);
  }

  return fp;
}

/*

  アセンブリソースを出力する

  input:
    xfilename : execute file name
    sfilename : output file name
    filedate  : execute file' time stamp
    argc : same to argc given to main()  // 現在未使用
    argv : same to argv given to main()

*/
extern void generate(char* xfilename, char* sfilename, time_t filedate,
                     const char* argv0, char* cmdline) {
  // ヘッダファイルが存在しない場合に空の出力ファイルが残らないよう、先に開く
  FILE* fpHeader = open_header_file();

  Current_Mputype = MIN_BIT(Dis.mpu);
  Dis.needString = TRUE;
  openOutputFile(sfilename, Dis.outputSplitByte, NULL);

  Dis.actions->outputHeader(xfilename, filedate, argv0, cmdline, fpHeader);
  if (fpHeader) fclose(fpHeader);
  Dis.actions->generate(sfilename);

  closeOutputFile(TRUE);
}

// セクション疑似命令を出力する
static void outputSection(char* section) {
  if (section == NULL) return;

  otherDirective(section);
  outputBlank();
}

// 必要なら出力ファイルを切り換える
static void switchFile(char* sfilename, int type, int* currentType) {
  if (Dis.outputSplitByte == 0) return;
  if (type == XDEF_TEXT) return;

  // .bss -> .stackは切り換えずに同じファイル(".bss")に出力する
  if (type == XDEF_STACK && *currentType == XDEF_BSS) return;

  closeOutputFile(FALSE);
  openOutputFile(sfilename, Dis.outputSplitByte,
                 (type == XDEF_DATA) ? ".dat" : ".bss");
  *currentType = type;
}

// 空のセクションかどうかを返す
//   サイズが0バイト、シンボルが存在しなければ空。
static boolean isEmptySection(address start, address end, int type) {
  if (start != end) return FALSE;
  if (symbol_search2(start, type) != NULL) return FALSE;

  return TRUE;
}

// 1セクションを出力する
// 引数:
//   section: セクション疑似命令("\t.text"～"\t.stack")、OSKDISの場合はNULL
//   end    : 終了アドレス
//   lptr   : 開始アドレス
//   type   : セクション番号(XDEF_TEXT～XDEF_STASCK)
lblbuf* generateSection(char* sfilename, char* section, address end,
                        lblbuf* lptr, int type, int* currentType) {
  lblbuf* nextLabel;
  uint8_t isBSS;

  // 空のセクションなら、セクション疑似命令も出力しない
  if (isEmptySection(lptr->label, end, type)) return lptr;

  outputBlank();
  switchFile(sfilename, type, currentType);
  outputSection(section);

  isBSS = (type >= XDEF_BSS);
  while (lptr->label < end) {
    address pc = lptr->label;
    lblmode mode = lptr->mode;

    nextLabel = lptr;
    do {
      nextLabel = Next(nextLabel);
    } while (nextLabel->shift);  // 命令の中を指すラベルを飛ばす

    label_line_out(pc, mode, type, FALSE,
                   isPROLABEL(mode) ? LINETYPE_TEXT : LINETYPE_DATA);

    if (isPROLABEL(mode)) {
      textgen(pc, nextLabel->label);
    } else if (isTABLE(mode)) {
      pc = tablegen(pc, type);
      if (pc != nextLabel->label) {
        // テーブル終了条件を end[num]、end[breakonly] にするとラベルを越えて
        // テーブルとして認識され、tablegen() でまるごと出力されるので、
        // その場合は pc もテーブル終端まで進める
        nextLabel = search_label(pc);
        if (!nextLabel) break;
      }
    } else if (isBSS) {
      bssgen(pc, nextLabel->label, lblmodeOpesize(mode));
    } else {
      datagen(pc, nextLabel->label, lblmodeOpesize(mode));
    }

    lptr = nextLabel;
  }

  // セクション末尾のラベル(外部定義シンボルのみ)
  label_line_out(lptr->label, lptr->mode, type, TRUE, LINETYPE_OTHER);

  return lptr;
}

// 直後に空行を出力する命令か調べる
//   applyToBra: DisVars::B (-B オプションの値)
static boolean isBlankLineNeeded(disasm* code, uint8_t applyToBra) {
  switch (code->opeType) {
    default:
      break;

    case JMPOP:
      if (applyToBra) return TRUE;
      if ((code->codeflags & CODEFLAG_BRAOP) == 0) return TRUE;
      break;

    case RTSOP:
      return TRUE;
  }

  return FALSE;
}

// オペコードに付けるサイズを決定する
static opesize decideOutputSize(disasm* code) {
  if (code->opeType == OTHER) {
    if (code->size == code->default_size && Dis.N) return NOTHING;
  } else if (code->jmpea == PCDISP) {
    switch (Dis.branchSize) {
      default:
        break;
      case BRANCH_SIZE_AUTO:  // -b0 可能ならサイズ省略
        // JMP, JSRはsize==NOTHINGなので考慮しなくてもNOTHINGが返される
        if ((code->codeflags & CODEFLAG_NEED_OPESIZE) == 0) return NOTHING;
        break;
      case BRANCH_SIZE_OMIT:  // -b1 常にサイズ省略
        return NOTHING;
        break;
    }
  }

  return code->size;
}

// オペランドをラベル化、旧表記化してバッファに書き込む
static char* operandToOldSyntax(char* p, operand* op) {
  char buf[sizeof(op->operand)];

  *operandToLabel(buf, op) = '\0';
  strcpy(op->operand, buf);
  syntax_new_to_old(op);
  return strcpy2(p, op->operand);
}

// オペランドを結合してバッファに書き込む(旧表記)
static char* buildOperandsOldSyntax(char* p, disasm* code) {
  if (code->op1.operand[0] == '\0') return p;
  *p++ = '\t';
  p = operandToOldSyntax(p, &code->op1);

  if (code->op2.operand[0] == '\0') return p;
  if ((code->op2.flags & OPFLAG_COMBINE) == 0) *p++ = ',';
  p = operandToOldSyntax(p, &code->op2);

  if (code->op3.operand[0] == '\0') return p;
  if ((code->op3.flags & OPFLAG_COMBINE) == 0) *p++ = ',';
  p = operandToOldSyntax(p, &code->op3);

  if (code->op4.operand[0] == '\0') return p;
  if ((code->op4.flags & OPFLAG_COMBINE) == 0) *p++ = ',';
  return operandToOldSyntax(p, &code->op3);
}

// オペランドを結合してバッファに書き込む
static char* buildOperands(char* p, disasm* code) {
  if (code->op1.operand[0] == '\0') return p;
  *p++ = '\t';
  p = operandToLabel(p, &code->op1);

  if (code->op2.operand[0] == '\0') return p;
  if ((code->op2.flags & OPFLAG_COMBINE) == 0) *p++ = ',';
  p = operandToLabel(p, &code->op2);

  if (code->op3.operand[0] == '\0') return p;
  if ((code->op3.flags & OPFLAG_COMBINE) == 0) *p++ = ',';
  p = operandToLabel(p, &code->op3);

  if (code->op4.operand[0] == '\0') return p;
  if ((code->op4.flags & OPFLAG_COMBINE) == 0) *p++ = ',';
  return operandToLabel(p, &code->op4);
}

// オペコードとオペランドを結合して出力する
void outputOpcodeAndOperands(disasm* code, const char* header, char* buffer) {
  char* p = buffer;

  *p++ = '\t';
  p = strcpy2(p, GET_OPECODE(code));
  p = write_size(p, decideOutputSize(code));

  p = Dis.oldSyntax ? buildOperandsOldSyntax(p, code) : buildOperands(p, code);
  *p = '\0';

  if (code->opeType == UNDEF) {
    char* u = writeTabAndCommentChar(buffer, Dis.UndefTab);  // Dis.Mtab
    strcpy(u, "undefined inst.");
  } else if ((code->codeflags & CODEFLAG_NEED_COMMENT) && Dis.M) {
    byteout_for_moption(code, buffer);
  }

  if (Dis.x) byteout_for_xoption(code->pc, code->bytes, buffer);

  if (header) {
    outputText2(code->pc, header, buffer);
  } else {
    // -vでない通常モードの処理をすこしでも速くする
    outputText(code->pc, buffer);
  }

  // rts、jmp、bra の直後に空行を出力する
  // ただし、-B オプションが無指定なら bra は除く
  if (isBlankLineNeeded(code, Dis.B)) outputBlank();
}

/*

  テキストブロックを出力する

  input :
    from : text begin address
    pcend : text end address

  return :
    pcend

*/
static address textgen(address from, address pcend) {
  DisParam disp;
  disasm* code = &disp.code;
  char buffer[128 + sizeof(disp.code.op1.operand) * 4];

  charout('.');

  setDisParamPcPtr(&disp, from, Dis.Ofst);
  disp.pcEnd = pcend;

  while (disp.pc < pcend) {
    static int8_t prev_fpuid = CPID_FPU;

    dis(&disp);

    // .cpu 切り換え
    if ((code->mputypes & Current_Mputype) == 0) {
      Current_Mputype = MIN_BIT(code->mputypes & Dis.mpu);
      outputDirective2(LINETYPE_TEXT, disp.pc, OpString.cpu,
                       mputype_numstr(Current_Mputype));
    }

    // .fpid 切り換え
    if ((code->fpuid >= 0) && (code->fpuid != prev_fpuid)) {
      char buf[2] = {'0' + code->fpuid, '\0'};
      prev_fpuid = code->fpuid;
      outputDirective2(LINETYPE_TEXT, disp.pc, OpString.fpid, buf);
    }

    outputOpcodeAndOperands(code, NULL, buffer);
  }

  return pcend;
}

// 単なる逆アセンブル出力(-vオプション指定時)
extern void disasmlist(char* sfilename) {
  DisParam disp;
  address pcend = Dis.beginTEXT + Dis.text;
  char buffer[128 + sizeof(disp.code.op1.operand) * 4];

  Dis.needString = TRUE;
  openOutputFile(sfilename, Dis.outputSplitByte, NULL);

  setDisParamPcPtr(&disp, Dis.beginTEXT, Dis.Ofst);
  disp.pcEnd = pcend;

  while (disp.pc < pcend) {
    char adrs[12];

    itox6(adrs, (ULONG)disp.pc);
    dis(&disp);
    outputOpcodeAndOperands(&disp.code, adrs, buffer);
  }

  closeOutputFile(TRUE);
}

// アドレッシング形式を旧式の表記に変える
static void syntax_new_to_old(operand* op) {
  char* optr = op->operand;
  char* p;

  switch (op->ea) {
    case AbShort: /* (abs)[.w] -> abs[.w] */
    case AbLong:  /* (abs)[.l] -> abs[.l] */
      p = strchr(optr, ')');
      strcpy(p, p + 1);
      strcpy(optr, optr + 1);
      break;

    case AregIDX:  // (d8,an,ix) -> d8(an,ix)
      if ((p = strchr(optr, ',')) == NULL) {
        break;  // ありえないが念のため
      }
      if (strchr(p + 1, ',') == NULL) {
        break;  // (an,ix) ならそのまま
      }
      *p = '(';
      strcpy(optr, optr + 1);
      break;

    case AregDISP:  // (d16,an)   -> d16(an)
    case PCIDX:     // (d8,pc,ix) -> d8(pc,ix)
    case PCDISP:    // (d16,pc)   -> d16(pc)
      if ((p = strchr(optr, ',')) == NULL) {
        break;  // 相対分岐命令
      }
      *p = '(';
      strcpy(optr, optr + 1);
      break;

    default:
      break;
  }
}

// opesizeに対応する.dsのサイズと単位バイト数を返す
SizeLength getSizeBytes(opesize size) {
  switch (size) {
    default:
      break;

    case WORDSIZE:
    case RELTABLE:
      return (SizeLength){WORDSIZE, 2};
    case LONGSIZE:
    case RELLONGTABLE:
      return (SizeLength){LONGSIZE, 4};
    case QUADSIZE:
      return (SizeLength){QUADSIZE, 8};
    case SINGLESIZE:
      return (SizeLength){SINGLESIZE, 4};
    case DOUBLESIZE:
      return (SizeLength){DOUBLESIZE, 8};
    case EXTENDSIZE:
      return (SizeLength){EXTENDSIZE, 12};
    case PACKEDSIZE:
      return (SizeLength){PACKEDSIZE, 12};
  }

  return (SizeLength){BYTESIZE, 1};
}

// .dsのサイズと総バイト数から、正規化した.dsのサイズと単位数を返す
SizeLength getSizeLength(SizeLength sb, int bytes) {
  if (sb.length >= 2) {
    // 単位バイト数の倍数ならそのサイズで出力
    int units = bytes / sb.length;
    if (units * sb.length == bytes) return (SizeLength){sb.size, units};
  }
  return (SizeLength){BYTESIZE, bytes};
}

/*

  データブロックを出力する

  input :
    pc : data begin address
    pcend : data end address
    size : data size

  return :
    pcend

*/
static address datagen(address pc, address pcend, opesize size) {
  SymbolLabelFormula slfml;

  while (pc < pcend) {
    address next_adrs = nearadrs(pc);

    if (pcend <= next_adrs) {
      // ブロック内にアドレス依存データがなければまるごと出力して終わり
      datagen_sub(pc, pcend, size);
      return pcend;
    }

    if (next_adrs != pc) {
      // ブロック先頭からアドレス依存データの手前までを出力
      datagen_sub(pc, next_adrs, size);
      pc = next_adrs;
    }
    // アドレス依存データを .dc.l Lxxxxxx として出力
    makeSymLabFormula(&slfml, (address)peekl(next_adrs + Dis.Ofst));
    outputData3(pc, OpString.dc[LONGSIZE], slfml.symbol, slfml.expr);
    pc += 4;
  }
  return pcend;
}

/*

  データブロックを出力する
  条件 : pc から pcend までにはアドレスに依存しているところはない

  input :
    pc : data begin address
    pcend : data end address
    size : data size

*/
static void datagen_sub(address pc, address pcend, opesize size) {
  switch (size) {
    case STRING:
      strgen(pc, pcend);
      break;
    case RELTABLE:
    case RELLONGTABLE:
      relgen(pc, pcend, size);
      break;
    case ZTABLE:
      zgen(pc, pcend);
      break;

    case WTABLE:
      Dis.actions->datagenWTable(pc, pcend);
      break;

    default:
      dataout(pc, pcend - pc, size);
      break;
  }
}

typedef enum {
  COMPRESS_NONE,
  COMPRESS_DCB,  // .dcb.b (同じ値の繰り返し)
  COMPRESS_DS,   // .ds.b (値がすべて0)
} CompressMode;

// データ領域が圧縮可能かを調べる(バイト単位)
static CompressMode getCompressModeByte(codeptr ptr, size_t bytes) {
  UBYTE value = peekb(ptr);
  int i;

  // [1] [2] [3] がすべて [0] と同じ値であることを確認
  bytes -= 1;
  for (i = 1; i <= 3; i += 1) {
    if (value != peekb(ptr + i)) return COMPRESS_NONE;
    if (--bytes == 0) return (value == 0) ? COMPRESS_DS : COMPRESS_DCB;
  }

  // 残りがあればロングワード単位で比較する
  if (bytes) {
    if (memcmp(ptr, ptr + 4, bytes) != 0) return COMPRESS_NONE;
  }
  return (value == 0) ? COMPRESS_DS : COMPRESS_DCB;
}

// データ領域が圧縮可能かを調べる(ワード単位)
static CompressMode getCompressModeWord(codeptr ptr, size_t bytes) {
  UWORD value = peekw(ptr);

  // [1] が [0] と同じ値であることを確認
  if (value != peekw(ptr + 2)) return COMPRESS_NONE;

  // 残りがあればロングワード単位で比較する
  if (bytes > 4) {
    if (memcmp(ptr, ptr + 4, bytes - 4) != 0) return COMPRESS_NONE;
  }
  return (value == 0) ? COMPRESS_DS : COMPRESS_DCB;
}

// データ領域が圧縮可能かを調べる
//   bytesPerUnit == 1、2または4の倍数であること
static CompressMode getCompressMode(codeptr ptr, size_t bytes, int bytesPerUnit,
                                    int units) {
  if ((int)bytes < Dis.compressLen || Dis.compressLen == 0)
    return COMPRESS_NONE;
  if (units < 2) return COMPRESS_NONE;

  if (bytesPerUnit == 1) return getCompressModeByte(ptr, bytes);
  if (bytesPerUnit == 2) return getCompressModeWord(ptr, bytes);

  // bytesPerUnit >= 4
  if (memcmp(ptr, ptr + bytesPerUnit, bytes - bytesPerUnit) != 0)
    return COMPRESS_NONE;

  // 値が0かどうかを確認する
  do {
    if (peekl(ptr) != 0) return COMPRESS_DCB;
    bytesPerUnit -= 4;
  } while (bytesPerUnit);
  return COMPRESS_DS;
}

// データを文字列化してバッファに書き込む
//   文字列末尾('\0')のアドレスを返す
static char* stringifyData(char* buffer, codeptr store, opesize size) {
  char* p = buffer;

  switch (size) {
    default:
    case BYTESIZE:
      *p++ = '$';
      return itox2_without_0supress(p, peekb(store));

    case WORDSIZE:
      *p++ = '$';
      return itox4_without_0supress(p, peekw(store));

    case LONGSIZE:
      *p++ = '$';
      return itox8_without_0supress(p, peekl(store));

    case QUADSIZE:
      return stringifyQuadWord(p, (quadword*)store);

    case SINGLESIZE:
      return fpconv_s(p, store);

    case DOUBLESIZE:
      return fpconv_d(p, store);

    case EXTENDSIZE:
      return fpconv_x(p, store);

    case PACKEDSIZE:
      return fpconv_p(p, store);
  }
}

// .dc.?を一行文字列化するのに必要なバッファサイズ
#define MAX_DC_B_BUFSIZE (4 * DATA_WIDTH_MAX)  // "$xx,"
#define MAX_DC_REAL_BUFSIZE (64 * 1)
#define MAX_DC_BUFSIZE MAX(MAX_DC_B_BUFSIZE, MAX_DC_REAL_BUFSIZE)

// データブロックの出力(.dc.?)
void dataoutDc(address pc, codeptr store, opesize size, ULONG bytes,
               ULONG bytesPerUnit) {
  ULONG maxBytesPerLine = bytesPerUnit;
  char buffer[MAX_DC_BUFSIZE + 16];

  // バイト、ワード、ロングワードは一行に複数データを出力する
  if (size <= LONGSIZE) {
    // 他のサイズでも複数データ出力する場合、以下の方法では
    // 12バイトサイズの端数切り上げができないので注意。
    ULONG n = (bytesPerUnit - 1);
    maxBytesPerLine = (Dis.dataWidth + n) & ~n;
  }

  do {
    ULONG bytesPerLine = MIN(bytes, maxBytesPerLine);
    ULONG rest = bytesPerLine;
    char* p = buffer;

    do {
      p = stringifyData(p, store, size);
      *p++ = ',';
      store += bytesPerUnit;
      rest -= bytesPerUnit;
    } while (rest);
    *--p = '\0';  // 末尾のカンマを消す

    outputData2(pc, OpString.dc[size], buffer);

    pc += bytesPerLine;
    bytes -= bytesPerLine;
  } while (bytes);
}

// データブロックの出力
static void dataout(address pc, ULONG bytes, opesize size) {
  codeptr store = pc + Dis.Ofst;
  char buffer[128];
  boolean canCompress;
  CompressMode compMode;
  int bytesPerUnit;
  int units;

  charout('#');

  // 頻出パターンを優先して処理する
  if (size == BYTESIZE || size == UNKNOWN) {
    if (bytes == 1) {
      itoxd(buffer, peekb(store), 2);
      outputData2(pc, OpString.dc[BYTESIZE], buffer);
      return;
    }
  } else if (size == LONGSIZE) {
    if (bytes == sizeof(ULONG) && isEven(pc)) {
      itoxd(buffer, peekl(store), 8);
      outputData2(pc, OpString.dc[LONGSIZE], buffer);
      return;
    }
  } else if (size == WORDSIZE) {
    if (bytes == sizeof(UWORD) && isEven(pc)) {
      itoxd(buffer, peekw(store), 4);
      outputData2(pc, OpString.dc[WORDSIZE], buffer);
      return;
    }
  }

  // サイズ不明の場合はバイト単位として圧縮を試みる
  // 圧縮する利点はある(領域のサイズが分かりやすくなる、出力ソースコードのファイルサイズ削減)
  // のに対し、する場合の欠点やしない利点というのが特にないため。
  if (size == UNKNOWN) size = BYTESIZE;

  // BYTESIZEの場合
  canCompress = TRUE;
  bytesPerUnit = 1;
  units = bytes;

  if (size != BYTESIZE) {
    if (isOdd(pc)) {
      size = BYTESIZE;
    } else {
      SizeLength sb = getSizeBytes(size);
      SizeLength sl = getSizeLength(sb, bytes);
      bytesPerUnit = sb.length;  // フォールバックした場合は1に戻すこと
      units = sl.length;
      size = sl.size;
    }

    // フォールバックでBYTESIZEになった場合は圧縮しない
    if (size == BYTESIZE) {
      canCompress = FALSE;
      bytesPerUnit = 1;
    }
  }

  compMode = canCompress ? getCompressMode(store, bytes, bytesPerUnit, units)
                         : COMPRESS_NONE;
  if (compMode == COMPRESS_NONE) {
    dataoutDc(pc, store, size, bytes, bytesPerUnit);
  } else if (compMode == COMPRESS_DCB) {
    char* p = buffer + snprintf(buffer, sizeof(buffer), "%d,", units);
    stringifyData(p, store, size);
    outputData2(pc, OpString.dcb[size], buffer);
  } else {  // COMPRESS_DS
    snprintf(buffer, sizeof(buffer), "%d", units);
    outputData2(pc, OpString.ds[size], buffer);
  }
}

// -rオプションのコメントの出力(文字列の16進数ダンプ)
static void byteout_for_roption(codeptr store, ULONG bytes) {
  char buffer[MAX_DC_B_BUFSIZE + 16];
  static char comment[] = ";\t\t";
  comment[0] = Dis.commentStr[0];

  do {
    ULONG bytesPerLine = MIN(bytes, (size_t)Dis.dataWidth);
    ULONG rest = bytesPerLine;
    char* p = buffer;

    do {
      *p++ = '$';
      p = itox2_without_0supress(p, peekb(store));
      *p++ = ',';
      store += 1;
      rest -= 1;
    } while (rest);
    *--p = '\0';  // 末尾のカンマを消す

    otherDirective2(comment, buffer);

    bytes -= bytesPerLine;
  } while (bytes);
}

void otherDirective(const char* s) {
  outputDirective(LINETYPE_OTHER, (address)0, s);
}

void otherDirective2(const char* s1, const char* s2) {
  outputDirective2(LINETYPE_OTHER, (address)0, s1, s2);
}

#define ENTERSTR()    \
  if (!strmode) {     \
    if (comma) {      \
      *p++ = comma;   \
      column += 1;    \
    }                 \
    *p++ = quoteChar; \
    column += 1;      \
    strmode = TRUE;   \
  }
#define EXITSTR()     \
  if (strmode) {      \
    *p++ = quoteChar; \
    column++;         \
    strmode = FALSE;  \
  }

static boolean is_sb(char* ptr, char quoteChar) {
  return (isprkana(ptr[0]) && ptr[0] != quoteChar);
}

static boolean is_mb_zen(char* ptr) {
  return (iskanji(ptr[0]) && iskanji2(ptr[1]));
}

static boolean is_mb_han(char* ptr) {
  if (ptr[0] == 0x80 || (0xf0 <= ptr[0] && ptr[0] <= 0xf3)) {
    if ((0x20 <= ptr[1] && ptr[1] <= 0x7e) ||
        (0x86 <= ptr[1] && ptr[1] <= 0xfd))
      return TRUE;
  }
  return FALSE;
}

//  文字列の出力
//
//  input:
//    pc   : string begin address
//    pcend: string end address
static void strgen(address pc, address pcend) {
  // 最大消費パターンは、二バイト半角文字が1桁で2バイト
  char buffer[STRING_WIDTH_MAX * 2 + 16];
  char* store = (char*)(Dis.Ofst + pc);
  char* stend = (char*)(Dis.Ofst + pcend);
  char quoteChar = Dis.quoteChar;

  charout('s');

  while (store < stend) {
    codeptr store0 = (codeptr)store;
    address line;
    boolean strmode = FALSE;
    int column = 0;
    char comma = 0;  // ',' ならカンマを出力する
    char* p = buffer;

    while (column < Dis.stringWidth && store < stend) {
      if (is_sb(store, quoteChar)) {
        // ANK 文字
        ENTERSTR();
        *p++ = *store++;
        column += 1;
      } else if (is_mb_zen(store) && store + 1 < stend) {
        // 二バイト全角
        ENTERSTR();
        *p++ = *store++;
        *p++ = *store++;
        column += 2;
      } else if (is_mb_han(store) && store + 1 < stend) {
        // 二バイト半角
        ENTERSTR();
        *p++ = *store++;
        *p++ = *store++;
        column += 1;
      } else {
        char c = *store++;
        EXITSTR();
        if (comma) {
          *p++ = comma;
          column += 1;
        }
        *p++ = '$';
        p = itox2_without_0supress(p, c);
        column += 3;  // strlen("$xx")

        // \n または NUL なら改行する
        if (store != stend && (c == '\n' || c == '\0')) {
          // ただし、後に続く NUL は全て一行に納める
          if (*store == '\0')
            ;
          else
            break;  // 改行
        }
      }
      comma = ',';
    }
    if (strmode) *p++ = quoteChar;
    *p = '\0';

    line = (address)(store0 - Dis.Ofst);
    outputData2(line, OpString.dc[BYTESIZE], buffer);

    if (Dis.r) byteout_for_roption(store0, (ULONG)(store - (char*)store0));
  }
}
#undef ENTERSTR
#undef EXITSTR

// 指定範囲を .dc.b で出力する
//   データブロックの末尾の半端データの出力用
void dataoutDcByte(address pc, address pcend) {
  ULONG bytes = pcend - pc;
  if (bytes) dataoutDc(pc, pc + Dis.Ofst, BYTESIZE, bytes, 1);
}

/*

  リラティブオフセットテーブルの出力

  input :
    pc : relative offset table begin address
    pcend : relative offset table end address
    size: RELTABLE or RELLONGTABLE

*/
static void relgen(address tabletop, address pcend, opesize size) {
  char buf[256], minusTable[128];
  uint8_t isLong = (size == RELLONGTABLE);
  ULONG bytes = isLong ? 4 : 2;
  address pc;

  // 相対オフセットテーブル解析後のプログラム領域解析でテーブルの途中に
  // ラベルが登録されるとテーブルの大きさが半端になることがある対策
  const address limit = pcend - bytes;

  // あらかじめバッファの先頭に .dc 疑似命令を作成しておく
  // -xオプションがあるためOutputData2()で出力することができず、
  // 一行まるごとバッファに書き込む必要がある。
  char* bufp = strcpy2(buf, OpString.dc[isLong ? LONGSIZE : WORDSIZE]);

  // -Lxxxxxx も作成しておく
  minusTable[0] = '-';
  make_proper_symbol(&minusTable[1], tabletop);

  charout(isLong ? 'R' : 'r');

  for (pc = tabletop; pc <= limit; pc += bytes) {
    codeptr store = pc + Dis.Ofst;
    ULONG offs = isLong ? peekl(store) : (ULONG)extl(peekw(store));
    char* p = bufp;

    if (offs == 0) {
      // オフセット値0の場合は .dc 0 にする
      *p++ = '0';
      *p = '\0';
    } else {
      p = make_proper_symbol(p, tabletop + offs);
      strcpy(p, minusTable);
    }

    // 相対テーブルも -x でコメントを出力する
    if (Dis.x) byteout_for_xoption(pc, bytes, buf);

    outputData(pc, buf);
  }

  dataoutDcByte(pc, pcend);  // 半端に残ったデータがあれば出力
}

/*

  ロングワードテーブルの出力

  input :
    pc : longword table begin address
    pcend : longword table end address

*/
static void zgen(address pc, address pcend) {
  const address limit = pcend - sizeof(ULONG);

  charout('z');

  while (pc <= limit) {
    SymbolLabelFormula slfml;
    makeSymLabFormula(&slfml, (address)peekl(pc + Dis.Ofst));

    outputData3(pc, OpString.dc[LONGSIZE], slfml.symbol, slfml.expr);

    pc += sizeof(ULONG);
  }

  dataoutDcByte(pc, pcend);  // 半端に残ったデータがあれば出力
}

static boolean end_by_break;

/*

  ユーザ定義のテーブルの出力

  input :
    fml : ユーザ定義のテーブル１行の構造体
    tabletop : table top address
    pc : pointing address
    notlast : true iff this line is not last line at table
  return :
    next pointing address

*/
static address tablegen_sub(formula* fml, address tabletop, address pc,
                            int notlast, int sectType) {
  char buffer[256];
  ParseTblParam param = {
      .text = NULL,
      .buffer = buffer,
      .bufLen = sizeof(buffer),
      .constValues = {0, tabletop}  //
  };
  int count = 1;
  boolean firstCall = TRUE;

  do {
    ParseTblResult result;

    lblbuf* lptr = next(pc + 1);
    address nextlabel = lptr->label;
    address nextPc;

    // CRID, BREAKID, EVENID(アライン実行時)以外は全て TRUE
    boolean pc_inc_inst = TRUE;

    param.text = fml->expr;
    param.constValues[PARSE_CONST_PC] = pc;
    parseTableLine(&param, &result);
    if (firstCall) {
      count = result.count;
      firstCall = FALSE;
    }
    nextPc = pc + result.bytes;

    if ((nextlabel < nextPc) && (result.id != LASCIIID)) {
      // 複数バイトのデータ中にラベルがある場合は分割して出力する
      tablegen_label(pc, nextPc, lptr, sectType);
    } else {
      switch (result.id) {
        default:
          break;

        case BYTESIZE:
        case WORDSIZE:
        case LONGSIZE:
        case SINGLESIZE:
        case DOUBLESIZE:
        case EXTENDSIZE:
        case PACKEDSIZE:
          outputData2(pc, OpString.dc[result.id], buffer);
          break;

        case BYTEID:
          datagen(pc, nextPc, BYTESIZE);
          break;

        case LASCIIID:
          datagen(pc, pc + 1, BYTESIZE);  // .dc.b 文字列長
          pc += 1;
          if (nextlabel < nextPc) {
            tablegen_label(pc, nextPc, lptr, sectType);
            break;
          }
          // FALLTHRU
        case ASCIIID:
        case ASCIIZID:
          strgen(pc, nextPc);
          break;

        case EVENID:
          outputDirective(LINETYPE_DATA, pc, OpString.even);
          if (result.bytes == 0) pc_inc_inst = FALSE;
          break;

        case CRID:
          outputDirective(LINETYPE_DATA, pc, "");
          pc_inc_inst = FALSE;
          break;

        case BREAKID:
          if (result.value) {
            end_by_break = TRUE;
            goto tableend;
          }
          pc_inc_inst = FALSE;
          break;
      }
    }
    pc = nextPc;

    if ((lptr = search_label(pc)) != NULL) {
      // テーブル解析時にテーブル終了と判定したらそのアドレスをラベル登録するので、
      // ここではそのラベルを見てテーブル終了を判断する
      if (isENDTABLE(lptr->mode)) {
        if (notlast && (fml + 1)->id == EVENID) {
          // if .even here...
          outputDirective(LINETYPE_DATA, pc, OpString.even);
          pc += (pc & 1);
        }
        end_by_break = TRUE;
        goto tableend;
      }

      if (pc_inc_inst) {
        // labelout is not required at end of table
        label_line_out(pc, lptr->mode, sectType, FALSE, LINETYPE_DATA);
      }
    }
  } while (--count > 0);

tableend:
  return pc;
}

/*

  ユーザー定義のテーブルの出力

  input :
    pc : user defined table begin address
    pcend : user defined table end address( not used )
    mode : table attribute( not used )
  return :
    table end address

*/
static address tablegen(address pc, int sectType) {
  int loop, i;
  table* tableptr;

  charout('t');

  if ((tableptr = search_table(pc)) == NULL)
    internalError(__FILE__, __LINE__, "テーブルが無い(!?)。");

  end_by_break = FALSE;
  pc = tableptr->tabletop;

  for (loop = 0; loop < tableptr->loop; loop++) { /* implicit loop */
    for (i = 0; i < tableptr->lines; i++) {       /* line# loop */
      pc = tablegen_sub(&tableptr->formulaptr[i], tableptr->tabletop, pc,
                        i + 1 < tableptr->lines, sectType);

      if (end_by_break) return pc;
    }
  }

  return pc;
}

/*

  テーブル中にラベルが存在した場合のデータブロック出力

  input :
    pc : point address
    pcend : point address' end address
    lptr : テーブル中のラベル

*/
static void tablegen_label(address pc, address pcend, lblbuf* lptr,
                           int sectType) {
  address nlabel = lptr->label;
  lblmode nmode = lptr->mode;

  pc = datagen(pc, nlabel, lblmodeOpesize(nmode));
  while (pc < pcend) {
    lblmode mode = nmode;

    lptr = next(pc + 1);
    while (lptr->shift) lptr = Next(lptr);

    nlabel = lptr->label;
    nmode = lptr->mode;

    label_line_out(pc, mode, sectType, FALSE,
                   isPROLABEL(mode) ? LINETYPE_TEXT : LINETYPE_DATA);
    pc = isPROLABEL(mode)
             ? textgen(pc, MIN(nlabel, pcend))
             : datagen(pc, MIN(nlabel, pcend), lblmodeOpesize(mode));
  }
}

/*

  -M オプションのコメント処理

  input :
    pc : address of 'cmpi.?, move.b, addi.b, subi.? #??' instruction
    buffer : pointer to output buffer

*/
#define IMM_B1 (*(UBYTE*)(store + 1))
#define IMM_W1 (*(UBYTE*)(store + 0))
#define IMM_W2 (*(UBYTE*)(store + 1))
#define IMM_L1 (*(UBYTE*)(store + 0))
#define IMM_L2 (*(UBYTE*)(store + 1))
#define IMM_L3 (*(UBYTE*)(store + 2))
#define IMM_L4 (*(UBYTE*)(store + 3))

static void byteout_for_moption(disasm* code, char* buffer) {
  char* p;
  codeptr store = code->pc + Dis.Ofst;
  opesize size = code->size;

  if (code->size2 == UNKNOWN)
    size = BYTESIZE;  // MOVEQ.Lは命令コードの下位にバイト値がある
  else
    store += 2;

  /* 表示可能な文字か調べる */
  if (size == BYTESIZE) {
    if (!isprint(IMM_B1)) return;
  } else if (size == WORDSIZE) {
    /* 上位バイトは表示可能もしくは 0 */
    if (IMM_W1 && !isprint(IMM_W1)) return;

    /* 下位バイトは必ず表示可能 */
    if (!isprint(IMM_W2)) return;
  } else if (size == LONGSIZE) {
    /* 最上位バイトは表示可能もしくは 0 */
    if (IMM_L1 && !isprint(IMM_L1)) return;

    /* 真ん中の二バイトは必ず表示可能 */
    if (!isprint(IMM_L2) || !isprint(IMM_L3)) return;

    /* 最下位バイトは表示可能もしくは 0 */
    if (IMM_L4 && !isprint(IMM_L4)) return;
  } else { /* pack, unpk */
#ifdef PACK_UNPK_LOOSE
    /* 上位バイトは表示可能もしくは 0 */
    if (IMM_W1 && !isprint(IMM_W1)) return;

    /* 下位バイトは表示可能もしくは 0 */
    if (IMM_W2 && !isprint(IMM_W2)) reutrn;

    /* ただし両方とも 0 ではいけない */
    if (IMM_W1 == 0 && IMM_W2 == 0) return;
#else
    /* 二バイトとも必ず表示可能 */
    if (!isprint(IMM_W1) || !isprint(IMM_W2)) return;
#endif
  }

  p = writeTabAndCommentChar(buffer, Dis.Mtab);
  *p++ = '\'';

  if (size == BYTESIZE) {
    *p++ = IMM_B1;
  } else if (size == WORDSIZE) {
    if (IMM_W1) *p++ = IMM_W1;
    *p++ = IMM_W2;
  } else if (size == LONGSIZE) {
    if (IMM_L1) *p++ = IMM_L1;
    *p++ = IMM_L2;
    *p++ = IMM_L3;
    if (IMM_L4)
      *p++ = IMM_L4;
    else {
      strcpy(p, "'<<8");
      return;
    }
  } else { /* pack, unpk */
#ifdef PACK_UNPK_LOOSE
    if (IMM_W1) *p++ = IMM_W1;
    if (IMM_W2)
      *p++ = IMM_W2;
    else {
      strcpy(p, "'<<8");
      return;
    }
#else
    *p++ = IMM_W1;
    *p++ = IMM_W2;
#endif
  }

  *p++ = '\'';
  *p++ = '\0';
}
#undef IMM_B1
#undef IMM_W
#undef IMM_W1
#undef IMM_W2
#undef IMM_L1
#undef IMM_L2
#undef IMM_L3
#undef IMM_L4

/*

  -x オプションのコメント処理

  input :
    pc : address of instruction
    byte : instruction length in byte
    buffer : pointer to output buffer

*/
static void byteout_for_xoption(address pc, ULONG byte, char* buffer) {
  codeptr store = pc + Dis.Ofst;
  char* p = writeTabAndCommentChar(buffer, Dis.Xtab);
  ULONG i;

  for (i = 0; i < byte; i += 2, store += 2) {
    *p++ = '$';
    p = itox4_without_0supress(p, peekw(store));
    *p++ = ',';
  }
  *--p = '\0';  // 最後のカンマを消す
}

/*

  bss 領域の出力

  input :
    pc : data begin address
    pcend : data end address
    size : data size

*/
static void bssgen(address pc, address nlabel, opesize size) {
  char buf[16];
  int bytes = MIN(nlabel, Dis.LAST) - pc;
  SizeLength sl;

  charout('$');

  sl = getSizeLength(getSizeBytes(size), bytes);
  snprintf(buf, sizeof(buf), "%d", sl.length);
  outputData2(pc, OpString.ds[sl.size], buf);
}

// ラベル化できる場合はラベルを探してTRUEを返す
static boolean getLabel(operand* op, SymbolLabelFormula* slfml) {
  if (op->labelchange1 == LABELCHANGE_DISALLOW) return FALSE;

  if (op->labelchange1 == LABELCHANGE_DEPEND) {
    if (!INPROG(op->opval, op->eaadrs)) return FALSE;
  }

#ifdef DEBUG_NO_LABELCHANGE
  if (op->labelchange1 != LABELCHANGE_LABEL_ONLY) return FALSE;
#endif

  makeSymLabFormula(slfml, op->opval);
  return TRUE;
}

// 文字列の直前にベースディスプレースメントのサイズ(".l")があればそのアドレスを返す
//   なければ引数の文字列をそのまま返す
static char* rewindToBDSize(operand* op, char* s) {
  return ((op->flags & OPFLAG_PC_RELATIVE) && s[-2] == '.') ? s - 2 : s;
}

// オペランド内のアドレスをラベル化してバッファにコピーする
//   書き込んだ文字列の末尾('\0'は書き込まない)を返す。
static char* operandToLabel(char* p, operand* op) {
  SymbolLabelFormula slfml;

  switch (op->ea) {
    default:
      return strcpy2(p, op->operand);

    case AbLong:  // ($xxxx_xxxx) -> (label)
      if (!getLabel(op, &slfml)) return strcpy2(p, op->operand);

      *p++ = '(';
      p = catSlfml(p, &slfml);
      *p++ = ')';
      return p;

    case PCDISP:  // (d16,pc) -> (label,pc)
      if (!getLabel(op, &slfml)) return strcpy2(p, op->operand);

      if (op->labelchange1 == LABELCHANGE_LABEL_ONLY) {
        // Bcc/DBccなどはラベルだけの表記 $xxxx_xxxx -> label
        return catSlfml(p, &slfml);
      }
      *p++ = '(';
      p = catSlfml(p, &slfml);
      return strcpy2(p, strchr(op->operand, ','));

    case PCIDX:  // (d8,pc,ix) -> (label,pc,ix)
      if (!getLabel(op, &slfml)) return strcpy2(p, op->operand);

      *p++ = '(';
      p = catSlfml(p, &slfml);
      return strcpy2(p, strchr(op->operand, ','));

    case IMMED:  // #$xxxx_xxxx -> #label
      if (!getLabel(op, &slfml)) return strcpy2(p, op->operand);

      *p++ = '#';
      return catSlfml(p, &slfml);

    case AregDISP:  // (d16,a6) -> (label,a6)
    {
      char* (*d16AnToLabel)(char* p, operand* op) = Dis.actions->d16AnToLabel;
      if (d16AnToLabel != NULL)
        return d16AnToLabel(p, op);
      else
        return strcpy2(p, op->operand);
    }

    case AregIDXB:  // ($xxxx_xxxx,an,ix) -> (label,an,ix)
      if (!getLabel(op, &slfml)) return strcpy2(p, op->operand);

      *p++ = '(';
      p = catSlfml(p, &slfml);
      return strcpy2(p, strchr(op->operand, ','));

    case AregPOSTIDX:  // ([$xxxx_xxxx,an],ix,od) -> ([label,an],ix,od)
    case AregPREIDX:   // ([$xxxx_xxxx,an,ix],od) -> ([label,an,ix],od)
      // ベースディスプレースメントはロングのみ
      if (!getLabel(op, &slfml)) {
        p = strcpy2(p, op->operand);
      } else {
        *p++ = '(';
        *p++ = '[';
        p = catSlfml(p, &slfml);
        p = strcpy2(p, strpbrk(op->operand, "],"));
      }
      break;

    case PCIDXB:  // (bd,pc,ix) -> (label,pc,ix)
      if (!getLabel(op, &slfml)) return strcpy2(p, op->operand);

      *p++ = '(';
      p = catSlfml(p, &slfml);
      // ワード範囲のbd.lに対する最適化抑制のためのサイズ".l"は残す
      return strcpy2(p, rewindToBDSize(op, strchr(op->operand, ',')));

    case PCPOSTIDX:  // ([bd,pc],ix,od) -> ([label,pc],ix,od)
    case PCPREIDX:   // ([bd,pc,ix],od) -> ([label,pc,ix],od)
      // ベースディスプレースメントはワード(PC相対)、ロング(PC相対)、
      // ロング(リロケート情報あり、zpc)のパターンがある
      if (!getLabel(op, &slfml)) {
        p = strcpy2(p, op->operand);
      } else {
        *p++ = '(';
        *p++ = '[';
        p = catSlfml(p, &slfml);
        // ワード範囲のbd.lに対する最適化抑制のためのサイズ".l"は残す
        p = strcpy2(p, rewindToBDSize(op, strpbrk(op->operand, "],")));
      }
      break;
  }

  // アウターディスプレースメント
  if (op->exod == 4 && INPROG(op->opval2, op->eaadrs2)) {
    makeSymLabFormula(&slfml, op->opval2);
    while (*(--p) != ',') {
      // loop
    }
    p += 1;
    p = catSlfml(p, &slfml);
    *p++ = ')';
  }
  return p;
}

// ラベル定義のコロン文字列を得る
//   既定値の-C2の場合、通常ラベル ":"、外部宣言ラベル "::"
static const char* getColonForLabelDefine(int xdef) {
  switch (Dis.symbolColonNum) {
    default:
    case SYMBOL_COLON_OMIT:
      break;
    case SYMBOL_COLON_ONE:
      return ":";
    case SYMBOL_COLON_AUTO:
      return xdef ? "::" : ":";
    case SYMBOL_COLON_TWO:
      return "::";
  }
  return "";
}

// ラベル定義行出力
// 引数
//   adrs: label address
//   mode: label mode
//   sectType: セクション番号(XDEF_TEXTなど)
//             0の場合はラベルファイルで追加されたシンボルだけを出力する。
//   last: TRUEならソースコード最後尾のラベル
//
// シンボル名が定義されていれば「シンボル名::」、未定義なら「Lxxxxxx:」などの
// 形式で出力する(コロンの数やラベル先頭文字はオプションの状態で変わる)。
// シンボル属性によっては全く出力されないこともある。
//
void label_line_out(address adrs, lblmode mode, int sectType, boolean last,
                    LineType lineType) {
  symbol* symbolptr;
  SymbolLabelFormula slfml;
  const char* colon;

  if (isHIDDEN(mode)) return;

  if ((symbolptr = symbol_search(adrs)) != NULL) {
    symlist* sym = &symbolptr->first;
    do {
      if (sym->type == (UWORD)sectType || (!last && sym->type == 0)) {
        colon = getColonForLabelDefine(sym->type);
        outputDirective2(lineType, adrs, sym->sym, colon);
      }
      sym = sym->next;
    } while (sym);
    return;
  }

  // 各セクションの最後尾はシンボルテーブルで定義されたラベル以外は出力しない
  if (last && sectType) return;

  // シンボル名が無ければ Lxxxxxx: の形式で出力する
  makeBareLabel(&slfml, adrs);
  colon = getColonForLabelDefine(0);
  outputDirective2(lineType, adrs, slfml.expr, colon);
}

const char* ctimez(time_t* clock) {
  char* p = ctime(clock);

  if (p == NULL) return "(invalid time)";
  removeTailLf(p);
  return p;
}

static char* mputype_numstr(mputypes m) {
  if (m & M000) return "68000";
  if (m & M010) return "68010";
  if (m & M020) return "68020";
  if (m & M030) return "68030";
  if (m & M040) return "68040";
  /* if (m & M060) */
  return "68060";
}

// EOF
