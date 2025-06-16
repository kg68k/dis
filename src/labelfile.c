// ソースコードジェネレータ
// ラベルファイルモジュール
// Copyright (C) 1989,1990 K.Abe
// All rights reserved.
// Copyright (C) 2024 TcbnErik

#include <ctype.h> /* toupper */
#include <stdio.h>
#include <string.h>

#include "analyze.h"
#include "estruct.h"
#include "etc.h"
#include "global.h"
#include "label.h"
#include "symbol.h"
#include "version.h"

/************************ ラベルファイルへの出力 ************************/

/*

  ラベルファイルのヘッダを書く

*/
static void make_header(FILE* fp, char* filename) {
  fprintf(fp,
          "*********************************************\n"
          "*\n"
          "*  Label file for %s\n"
          "*\n",
          filename);
  if (!Dis.deterministic) {
    fprintf(fp,
            "*\t%s\n"
            "*\n",
            ProgramAndVersion);
  }
  fprintf(fp, "*********************************************\n");
}

/*

  ラベルファイルを出力

*/
extern void make_labelfile(char* xfilename, char* labelfilename) {
  FILE* fp;
  lblbuf* nadrs;

  if ((fp = fopen(labelfilename, "w")) == NULL) {
    err("%s をオープンできません。\n", labelfilename);
  }

  make_header(fp, xfilename);
  nadrs = next(Dis.beginTEXT);

  while (nadrs->label != (address)-1) {
    opesize mode = lblbufOpesize(nadrs);

    fprintf(fp, "%06" PRIx32 "\t", (ULONG)nadrs->label);

    if (isPROLABEL(nadrs->mode))
      fputc('P', fp);
    else if (mode == RELTABLE || mode == RELLONGTABLE) {
      fputc('R', fp);
      fputc(mode == RELTABLE ? 'W' : 'L', fp);
    } else {
      static const char labelchar[ZTABLE + 1] = {
          'B', 'W', 'L', 'Q', 'U', /* 整数 */
          'F', 'D', 'X', 'P', 'U', /* 小数 */
          'S', 'U', 'U', 'Z'       /* その他 */
      };

      fputc('D', fp);
      fputc(mode > ZTABLE ? 'U' : labelchar[mode], fp);
    }
    if (nadrs->mode & FORCE) fputc('F', fp);
    if (isHIDDEN(nadrs->mode)) fputc('H', fp);

    {
      symbol* symbolptr = symbol_search(nadrs->label);

      if (symbolptr) {
        symlist* sym = &symbolptr->first;
        char space = '\t';

        do {
          fprintf(fp, "%c%s", space, sym->sym);
          space = ' ';
          sym = sym->next;
        } while (sym);
      }
    }

    fputc('\n', fp);
    nadrs = Next(nadrs);
  }

  fclose(fp);
}

/*********************** ラベルファイルからの入力 ***********************/

typedef enum {
  PASS_UPPER_FIRST,   // 大文字のみ判定(disが出力した属性)
  PASS_LOWER_SECOND,  // 小文字のみ判定(ユーザが編集した属性)
} LabelFilePass;

static char* skipspace(char* ptr) {
  while (isblank(*ptr)) ptr += 1;
  return ptr;
}

static char* untilspace(char* ptr) {
  while (*ptr && !isblank(*ptr)) ptr += 1;
  return ptr;
}

static inline char upper(char c) { return toupper(c); }

static int error(const char* filename, int line, const char* msg) {
  eprintf("%s:%d: %s\n", filename, line, msg);
  return 1;
}

// データ系の属性文字列を変換する
static int parseLabelAttrD(const char* filename, int line, const char** pStr,
                           lblmode* pAttr) {
  lblmode attr = DATLABEL;
  const char* p = *pStr;

  if (*p == '\0')
    return error(filename, line, "属性Dにはサイズの指定が必要です。");

  switch (upper(*p++)) {
    default:
      return error(filename, line, "属性Dのサイズが不正です。");

    case 'B':
      attr |= BYTESIZE;
      break;
    case 'W':
      attr |= WORDSIZE;
      break;
    case 'L':
      attr |= LONGSIZE;
      break;
    case 'Q':
      attr |= QUADSIZE;
      break;
    case 'F':
      attr |= SINGLESIZE;
      break;
    case 'D':
      attr |= DOUBLESIZE;
      break;
    case 'X':
      attr |= EXTENDSIZE;
      break;
    case 'P':
      attr |= PACKEDSIZE;
      break;
    case 'S':
      attr |= STRING;
      break;
    case 'Z':
      attr |= ZTABLE;
      break;
    case 'U':
      attr |= UNKNOWN;
      break;

      // 過去のバージョンでは相対オフセットテーブルをDR、DRL
      // と規定していたので、互換性のために残す
    case 'R':
      attr |= RELTABLE;
      if (upper(*p) == 'L') {
        p += 1;
        attr ^= (RELTABLE ^ RELLONGTABLE);
      }
      break;
  }

  *pStr = p;
  *pAttr = attr;
  return 0;
}

// 属性文字列をopesizeに変換する
static int parseLabelAttribute(const char* filename, int line, const char* p,
                               lblmode* pAttr) {
  *pAttr = DATLABEL;

  if (*p == '\0') return error(filename, line, "属性が指定されていません。");

  switch (upper(*p++)) {
    default:
      return error(filename, line, "不正な属性です。");

    case 'P':
      *pAttr = PROLABEL;
      break;

    case 'R':
      switch (upper(*p++)) {
        default:
          return error(filename, line, "属性Rにはサイズの指定が必要です。");
        case 'W':
          *pAttr |= RELTABLE;
          break;
        case 'L':
          *pAttr |= RELLONGTABLE;
          break;
      }
      break;

    case 'D': {
      int e = parseLabelAttrD(filename, line, &p, pAttr);
      if (e) return e;
    } break;
  }

  // 追加の属性 f, h
  while (*p && !isblank(*p)) {
    switch (upper(*p++)) {
      default:
        return error(filename, line, "不正な追加の属性です。");

      case 'F':
        *pAttr |= FORCE;
        break;
      case 'H':
        *pAttr |= HIDDEN;
        break;
    }
  }

  return 0;
}

typedef struct {
  address adrs;
  lblmode attr;
  boolean isLower;
  char* symptr;
} LabelLine;

// ラベルファイルの1行を解析する
static int get_line(const char* filename, int line, char* linebuf,
                    LabelLine* lblline) {
  char* atr = skipspace(untilspace(linebuf));

  lblline->adrs = (address)atox(linebuf);
  lblline->attr = PROLABEL;
  lblline->isLower = islower(*atr) ? TRUE : FALSE;
  lblline->symptr = skipspace(untilspace(atr));

  return parseLabelAttribute(filename, line, atr, &lblline->attr);
}

static void regLabel(address adrs, lblmode attr, int line,
                     const char* filename) {
  if (!regist_label(adrs, attr))
    eprintf("%s:%d: " PRI_ADRS " ???.\n", filename, line, adrs);
}

// 相対オフセットテーブルの解析を呼び出す
static void analyzeReltbl(address table, opesize size) {
  // 現在のラベルファイルの仕様上、テーブルの宛先がプログラムとデータの
  // どちらかであるかは指定できない
  boolean isProgram = FALSE;

  registerReltblOrder(&Dis.reltblArray, table, size, isProgram, 0);
}

// 小文字属性の処理
//   ユーザが指定した情報に従って解析を行う
static void workLower(address adrs, lblmode attr, int line,
                      const char* filename) {
  lblbuf* lptr;
  opesize size;

  if (isPROLABEL(attr)) {
    if (!analyze(adrs, ANALYZE_IGNOREFAULT)) {
      eprintf("%s:%d: " PRI_ADRS "からはプログラムと見なせません。\n", filename,
              line, adrs);
    }
    return;
  }

  size = lblmodeOpesize(attr);
  lptr = search_label(adrs);
  if (lptr && lblbufOpesize(lptr) != size) {
    // ラベルファイルの現在の行より上にある小文字属性による解析で
    // 最初のパスで登録したラベルの属性が変更される可能性あり。
    // 違う属性のままテーブル解析を呼び出すのはまずいかもしれないので
    // 念のため属性を再登録する。
    ch_lblmod(adrs, attr);
  }

  switch (size) {
    default:
      break;

    case RELTABLE:
    case RELLONGTABLE:
      analyzeReltbl(adrs, size);
      break;
    case ZTABLE:
      z_table(adrs);
      break;
  }
}

// ラベルファイルに記述されたシンボル(複数可)を登録する
static void addSymbols(char* s, address adrs) {
  const int section = 0;

  while (*s && *s != '*') {
    char* symend = untilspace(s);
    size_t len = symend - s;
    char* buf = Malloc(len + 1);

    memcpy(buf, s, len);
    buf[len] = '\0';
    add_symbol(adrs, section, buf);

    s = skipspace(symend);
  }
}

static void parseLabelFile(FILE* fp, const char* filename, LabelFilePass pass) {
  int line;
  int errorCount = 0;

  for (line = 1;; line++) {
    char linebuf[1024];
    LabelLine lblline;

    if (fgets(linebuf, sizeof(linebuf), fp) == NULL) break;
    removeTailLf(linebuf);
    if (!isxdigit(linebuf[0])) continue;

    errorCount += get_line(filename, line, linebuf, &lblline);

    if (pass == PASS_UPPER_FIRST) {
      // 最初のパスで大文字小文字に関わらずラベルとシンボルを登録する
      regLabel(lblline.adrs, lblline.attr, line, filename);
      addSymbols(lblline.symptr, lblline.adrs);
    } else {
      if (lblline.isLower)
        workLower(lblline.adrs, lblline.attr, line, filename);
    }
  }

  if (errorCount) err("ラベルファイルに%d個のエラーがあります。\n", errorCount);
}

// ラベルファイルを読み込む
void read_labelfile(char* filename) {
  FILE* fp = fopen(filename, "rt");

  if (fp == NULL) err("%s をオープンできません。\n", filename);

  parseLabelFile(fp, filename, PASS_UPPER_FIRST);
  rewind(fp);
  parseLabelFile(fp, filename, PASS_LOWER_SECOND);
  fclose(fp);
}

// EOF
