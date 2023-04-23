// ソースコードジェネレータ
// テーブル行構文解析・評価 ヘッダ
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

#ifndef EVAL_H
#define EVAL_H

#include "estruct.h"

enum {
  PARSE_CONST_PC,
  PARSE_CONST_TABLETOP,

  PARSE_CONST_NUM
};

typedef struct {
  char* text;
  char* buffer;
  int bufLen;
  ULONG constValues[PARSE_CONST_NUM];
} ParseTblParam;

typedef enum {
  PARSE_SUCCESS = 0,
  PARSE_SYNTAX_ERROR = 1,
  PARSE_RUNTIME_ERROR,
} ParseResult;

typedef struct {
  ParseResult pr;
  opesize id;
  const char* idstr;
  const char* error;
  ULONG count;
  int bytes;
  ULONG value;  // 多目的データ受け渡し用
} ParseTblResult;

extern ParseResult parseTableLine(ParseTblParam* param, ParseTblResult* result);

#endif

// EOF
