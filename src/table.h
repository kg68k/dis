// ソースコードジェネレータ
// テーブル処理モジュールヘッダ
// Copyright (C) 1989,1990 K.Abe
// All rights reserved.
// Copyright (C) 1997-2023 TcbnErik

#ifndef TABLE_H
#define TABLE_H

#include "etc.h"  // peekl

// p には Ofst を足していない値を渡す
#define PEEK_BYTE(p) ((UBYTE)(Dis.beginBSS <= (p) ? 0 : *((p) + Dis.Ofst)))
#define PEEK_WORD(p)           \
  ((UWORD)(Dis.beginBSS <= (p) \
               ? 0             \
               : (((p) + Dis.Ofst)[0] << 8) + ((p) + Dis.Ofst)[1]))
#if defined(__mc68020__) || defined(__i386__)
#define PEEK_LONG(p) ((ULONG)(Dis.beginBSS <= (p) ? 0 : peekl((p) + Ofst)))
#else
#define PEEK_LONG(p)                                                    \
  ((ULONG)(Dis.beginBSS <= (p) ? 0                                      \
           : ((int)(p)&1)                                               \
               ? (peekl((p) + Dis.Ofst - 1) << 8) + ((p) + Dis.Ofst)[3] \
               : peekl((p) + Dis.Ofst)))
#endif

/*
  テーブルの構造
  １アドレスに１つの table がある
  table.formulaptr[ 0 ... table.lines - 1 ] がテーブルのメンバを保持している
*/

typedef struct {
  opesize id;
  int line;
  char* expr;
} formula;

typedef struct {
  address tabletop;
  int loop;
  int lines;
  formula* formulaptr;  // formula の配列へのポインタ
} table;

extern void read_tablefile(char*);
extern table* search_table(address);

#endif

// EOF
