// ソースコードジェネレータ
// シンボルネーム管理モジュール
// Copyright (C) 1989,1990 K.Abe
// All rights reserved.
// Copyright (C) 2025 TcbnErik

#include "symbol.h"

#include <stdio.h>
#include <string.h>

#include "etc.h"
#include "global.h"
#include "hex.h"  // itoxd()
#include "label.h"
#include "opstr.h"
#include "output.h"

// X形式実行ファイルのシンボルテーブルの構造
enum {
  XSYM_TYPE = 0,
  XSYM_ADRS = 2,
  XSYM_NAME = 6,  // 文字列+even
};

typedef struct {
  codeptr name;
  size_t nameLen;
  address adrs;
  ULONG type;
} SymbolTable;

#define SYM_BUF_UNIT 16    // バッファ拡大時の増加要素数
static int Symbolbufsize;  // シンボルバッファの数
static symbol* Symtable;   // シンボルテーブルの頭
static int Symnum;         // シンボルテーブルの要素数

// "Lxxxxxx" 形式のラベル文字列を作成し、文字列末尾を返す
char* makeBareLabel(SymbolLabelFormula* slfml, address adrs) {
  char* p = slfml->expr;
  *p++ = Dis.labelChar;
  return itox6_without_0supress(p, (ULONG)adrs);
}

// ラベルorシンボル名(±オフセット)形式のラベル文字列を作成する
static void makeSymLabShift(SymbolLabelFormula* slfml, address adrs,
                            LONG shift) {
  char* p = slfml->expr;
  symbol* symptr = symbol_search(adrs);

  if (symptr) {
    slfml->symbol = symptr->first.sym;
    *p = '\0';
  } else {
    slfml->symbol = "";
    p = makeBareLabel(slfml, adrs);
  }

  if (shift) {
    *p++ = '+';
    if (shift < 0) {
      p[-1] = '-';
      shift = -shift;
    }
    itox6d(p, shift);
  }
}

// ラベルorシンボル名のラベル式を作成する
//   プログラム範囲外のアドレスはプログラム先頭または末尾からの
//   ±オフセット形式になる。
void makeInProgSymLabFormula(SymbolLabelFormula* slfml, address adrs) {
  LONG shift = 0;

  if ((LONG)adrs < (LONG)Dis.beginTEXT) {
    shift = (LONG)adrs - (LONG)Dis.beginTEXT;
    adrs = Dis.beginTEXT;
  } else if (Dis.LAST < adrs) {
    shift = (LONG)adrs - (LONG)Dis.LAST;
    adrs = Dis.LAST;
  }
  makeSymLabShift(slfml, adrs, shift);
}

// ラベルorシンボル名(±オフセット)形式のラベル式を作成する
//   ラベルが登録されていなかった場合はFALSEを返す
void makeSymLabFormula(SymbolLabelFormula* slfml, address adrs) {
  lblbuf* lptr;

  if (((LONG)adrs < (LONG)Dis.beginTEXT) || (Dis.LAST < adrs)) {
    if (Dis.outputSymbol == OUTPUT_SYMBOL_ALL) {
      symbol* symptr = symbol_search(adrs);
      if (symptr) {
        // プログラム範囲外でも、-s2かつ該当アドレスのシンボルが存在するなら
        // シンボル定義が出力されるので参照してよい
        slfml->symbol = symptr->first.sym;
        slfml->expr[0] = '\0';
        return;
      }
    }
    // シンボル定義が出力されないなら、beginTEXT/LASTからのオフセット形式にする
    makeInProgSymLabFormula(slfml, adrs);
    return;
  }

  // プログラム範囲内
  lptr = search_label(adrs);
  if (lptr == NULL) {
    slfml->symbol = "";
    makeBareLabel(slfml, adrs);
  } else {
    makeSymLabShift(slfml, adrs - lptr->shift, lptr->shift);
  }
}

// シンボル名の文字列長により必要なタブを返す
static const char* getPaddingTab(SymbolTable* st, const char* colon) {
  size_t n = TAB_WIDTH - strlen(colon);
  return (st->nameLen < n) ? "\t" : "";
}

// 定数シンボルの外部宣言を出力する
//   「xxx:: .equ $nn」の形式で出力する
static int outputSymbolEqu(SymbolTable* st, const char* colon) {
  char buf[16];
  const char* array[] = {
      (char*)st->name, colon, getPaddingTab(st, colon), OpString.equ, buf  //
  };

  itox8d(buf, (ULONG)st->adrs);
  outputDirectiveArray(LINETYPE_OTHER, (address)0, _countof(array), array);
  return 1;
}

// シンボルの外部宣言を出力する
static int outputSymbolXdef(SymbolTable* st, const char* colon) {
  address adrs = st->adrs;

  if (adrs < Dis.beginTEXT || Dis.LAST < adrs) {
    // プログラム範囲外ならラベル定義行が出力されないので
    // foo::  .equ  L000000-$10 のような形式で定義する
    SymbolLabelFormula slfml;
    makeInProgSymLabFormula(&slfml, adrs);

    {
      const char* array[] = {
          (char*)st->name, colon,        getPaddingTab(st, colon),
          OpString.equ,    slfml.symbol, slfml.expr  //
      };
      outputDirectiveArray(LINETYPE_OTHER, (address)0, _countof(array), array);
    }
    return 1;
  } else {
    // .xdef  foo
    const char* array[] = {"\t", OpString.xdef, (char*)st->name};
    outputDirectiveArray(LINETYPE_OTHER, (address)0, _countof(array), array);
  }

  return 1;
}

static int outputSymbolDefine(SymbolTable* st, int xdef, const char* colon) {
  switch ((UWORD)st->type) {
    default:
      break;

    case XDEF_ABS:
      return outputSymbolEqu(st, colon);

    case XDEF_COMMON:
    case XDEF_TEXT:
    case XDEF_DATA:
    case XDEF_BSS:
    case XDEF_STACK:
      if (xdef) return outputSymbolXdef(st, colon);
      break;
  }

  return 0;
}

// ソースコードにシンボル定義を出力する
// 引数:
//   stbuf   シンボルテーブルの情報
//   opXdef  .xdef疑似命令("\t.xdef\t")
//   colon   コロン("::" -Cオプションによって変更)
// 返値:
//   出力したシンボル数(行数)
int outputSymbolSection(ArrayBuffer* stbuf, const char* colon) {
  int lines = 0;
  int xdef = (Dis.outputSymbol == OUTPUT_SYMBOL_ALL);
  size_t count;
  SymbolTable* symbols = getArrayBufferRawPointer(stbuf, &count);

  if (symbols != NULL) {
    const size_t len = count;
    size_t i;
    for (i = 0; i < len; ++symbols, ++i) {
      lines += outputSymbolDefine(symbols, xdef, colon);
    }
  }

  freeArrayBuffer(stbuf);
  return lines;
}

// ラベルファイルで定義されたシンボルの属性をシンボル情報の属性に変更する
//   Human68kのX形式実行ファイル形式に依存。
static void change_sym_type(symbol* symbolptr, UWORD type, char* ptr) {
  symlist* sym = &symbolptr->first;

  do {
    if (strcmp(sym->sym, ptr) == 0) {
      sym->type = type;
      return;
    }
    sym = sym->next;
  } while (sym);
}

// シンボルテーブル上のシンボルの情報を表示する
static void printSymbol(const char* s, UWORD type, address adrs,
                        codeptr symName) {
  eprintf("\n%s($%04" PRIx16 " " PRI_ADRS " \"%s\")。", s, type, adrs, symName);
}

// 1個のアドレス型のシンボル情報を記録する
static void registerAddressSymbol(UWORD type, address adrs, codeptr name) {
  symbol* sym = symbol_search(adrs);

  if (sym == NULL) {
    // 特定アドレスの初回時はラベルを登録する
    // ラベルファイル読み込み時はすでに登録されているかもしれないし、
    // 登録されていない(ラベルファイルから記述が削除されていた)可能性もある。
    regist_label(adrs, DATLABEL | SYMLABEL | (lblmode)UNKNOWN);
  }

  if (Dis.g) {
    // -g指定時はラベルファイルに記載されたシンボル名を使うので
    // シンボル情報のシンボル名は無視する。属性は利用する
    if (sym != NULL) change_sym_type(sym, type, (char*)name);
    return;
  }

  add_symbol(adrs, type, (char*)name);
}

// 1個のシンボル情報を記録する
static void registerSymbol(ArrayBuffer* stbuf, UWORD type, address adrs,
                           codeptr name, size_t nameLen) {
  SymbolTable* p = getArrayBufferNewPlace(stbuf);
  *p = (SymbolTable){name, nameLen, adrs, type};

  // スタックサイズの指定
  if (type == XDEF_STACK && Dis.beginBSS <= adrs && adrs < Dis.beginSTACK)
    Dis.beginSTACK = adrs;

  switch (type) {
    default:
      printSymbol("未対応のシンボル情報です", type, adrs, name);
      break;

    case XDEF_ABS:
      break;

    case XDEF_COMMON:
      type = XDEF_BSS;
      // FALLTHRU
    case XDEF_STACK:
    case XDEF_TEXT:
    case XDEF_DATA:
    case XDEF_BSS:
      registerAddressSymbol(type, adrs, name);
      break;
  }
}

// シンボル名の文字列長を調べる
//   0: 空文字列  -1:NULがない
static int getSymbolNameLength(codeptr name, ULONG rest) {
  UBYTE* p = (UBYTE*)name;

  do {
    if (rest == 0) return -1;
    rest -= 1;
  } while (*p++);

  return (int)(p - 1 - (UBYTE*)name);
}

static void parseSymbolTableLoop(ArrayBuffer* stbuf, ULONG start) {
  codeptr ptr = Dis.Top + start;
  ULONG rest = Dis.symbol;

  while (rest > XSYM_NAME) {
    ULONG size;
    UWORD type = peekw(ptr + XSYM_TYPE);
    address adrs = peekl(ptr + XSYM_ADRS);
    codeptr name = ptr + XSYM_NAME;
    int nameLen = getSymbolNameLength(name, rest - XSYM_NAME);

    if (nameLen <= 0) {
      eprintf(nameLen ? "シンボル名がNULで終わっていません。\n"
                      : "シンボル名が空文字列です。\n");
    } else {
      registerSymbol(stbuf, type, adrs, name, nameLen);
    }

    size = XSYM_NAME + nameLen + 1;
    if (size & 1) size += 1;
    rest -= size;
    ptr += size;
  }
  if (rest > 0)
    eprintf("シンボル情報の末尾に" PRI_ULONG
            "バイトの未対応のデータがあります。\n",
            rest);
}

// Human68kのX形式実行ファイルに付属するシンボルテーブルを解析する
//   Symtable に登録(ラベル名の置換用)。
//   Dis.symtblArray に登録(シンボル定義セクション出力用)。
//
// ラベルファイルの読み込みより後に呼び出される
void parseSymbolTableHuman(ArrayBuffer* stbuf) {
  initArrayBuffer(stbuf, sizeof(SymbolTable));

  if (Dis.symbol == 0) {
    eprintf("シンボルテーブルは残念ながら存在しません。\n");
  } else {
    ULONG start = Dis.text + Dis.data + Dis.offset;

    if (start & 1) {
      eprintf("シンボルテーブルが奇数アドレスからはじまっています。\n");
    } else {
      eprintf("シンボルテーブルを展開します。\n");
      parseSymbolTableLoop(stbuf, start);
    }
  }

  freezeArrayBuffer(stbuf);
}

// type == 0 : labelfileでの定義
extern void add_symbol(address adrs, int type, char* symstr) {
  int i;
  symbol* sym = symbol_search(adrs);

  /* 既に登録済みならシンボル名を追加する */
  if (sym) {
    symlist* ptr = &sym->first;

    while (ptr->next) ptr = ptr->next;

    /* symlist を確保して、末尾に繋げる */
    ptr = ptr->next = Malloc(sizeof(symlist));

    /* 確保した symlist を初期化 */
    ptr->next = NULL;
    ptr->type = (UWORD)type;
    ptr->sym = symstr;

    return;
  }

  if (Symnum == Symbolbufsize) {
    /* バッファを拡大する */
    Symbolbufsize += SYM_BUF_UNIT;
    Symtable = Realloc(Symtable, Symbolbufsize * sizeof(symbol));
  }

  for (i = Symnum - 1; (i >= 0) && (Symtable[i].adrs > adrs); i--)
    Symtable[i + 1] = Symtable[i];

  Symnum++;
  sym = &Symtable[++i];
  sym->adrs = adrs;

  /* 最初のシンボルを記録 */
  sym->first.next = NULL;
  sym->first.type = (UWORD)type;
  sym->first.sym = symstr;
}

// 指定した属性のシンボルを探す
symlist* symbol_search2(address adrs, int type) {
  symbol* symbolptr = symbol_search(adrs);

  if (symbolptr) {
    symlist* sym = &symbolptr->first;

    while (sym->type != (UWORD)type && (sym = sym->next));
    return sym;
  }
  return NULL;
}

/*

  adrs をシンボルテーブルから捜す
  adrs は common text data bss のいずれか
  見つかったらポインタ、でなければ NULL を返す

*/
extern symbol* symbol_search(address adrs) {
  int step;
  symbol* ptr;

  /* 多少は速くなる? */
  if (Symnum == 0) return NULL;

  ptr = Symtable;
  for (step = Symnum >> 1; step > 4; step >>= 1)
    if ((ptr + step)->adrs <= adrs) /* binary search */
      ptr += step;

  for (; ptr < Symtable + Symnum; ptr++) {
    if (ptr->adrs == adrs)
      return ptr;
    else if (adrs < ptr->adrs)
      return NULL;
  }
  return NULL;
}

// EOF
