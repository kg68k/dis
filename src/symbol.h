// ソースコードジェネレータ
// シンボルネーム管理モジュールヘッダ
// Copyright (C) 1989,1990 K.Abe
// All rights reserved.
// Copyright (C) 1997-2023 TcbnErik

#ifndef SYMBOL_H
#define SYMBOL_H

#include "estruct.h"
#include "etc.h"  // strcpy2()

typedef struct {
  char* symbol;   // シンボル名 or ""
  char expr[24];  // "L123456+$12345678"
} SymbolLabelFormula;

typedef struct _symlist {
  struct _symlist* next;
  UWORD type;
  char* sym;
} symlist;

typedef struct {
  address adrs;
  symlist first;
} symbol;

enum {
  XDEF_COMMON = 0x0003,
  XDEF_ABS = 0x0200,
  XDEF_TEXT = 0x0201,
  XDEF_DATA = 0x0202,
  XDEF_BSS = 0x0203,
  XDEF_STACK = 0x0204,
};

extern char* makeBareLabel(SymbolLabelFormula* slfml, address adrs);
extern void makeInProgSymLabFormula(SymbolLabelFormula* slfml, address adrs);
extern void makeSymLabFormula(SymbolLabelFormula* slfml, address adrs);
extern int outputSymbolSection(ArrayBuffer* stbuf, const char* colon);
extern void parseSymbolTableHuman(ArrayBuffer* stbuf);
extern void add_symbol(address, int, char*);
extern symbol* symbol_search(address);
extern symlist* symbol_search2(address adrs, int type);

// ラベル式をバッファに転送して文字列末尾のアドレスを返す
static inline char* catSlfml(char* p, SymbolLabelFormula* slfml) {
  return strcpy2(strcpy2(p, slfml->symbol), slfml->expr);
}

#endif

// EOF
