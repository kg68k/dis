// ソースコードジェネレータ
// 出力ルーチン下請け ヘッダ
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

#ifndef OUTPUT_H
#define OUTPUT_H

#include "estruct.h"

#define TAB_WIDTH 8

typedef enum {
  LINETYPE_BLANK,
  LINETYPE_OTHER,
  LINETYPE_TEXT,
  LINETYPE_DATA,
} LineType;

extern void outputBlank(void);
extern void outputDirective(LineType type, address pc, const char* s);
extern void outputDirective2(LineType type, address pc, const char* s1,
                             const char* s2);
extern void outputDirectiveArray(LineType type, address pc, size_t length,
                                 const char* array[]);
extern void outputText(address pc, const char* s);
extern void outputText2(address pc, const char* s1, const char* s2);
extern void outputData(address pc, const char* s);
extern void outputData2(address pc, const char* s1, const char* s2);
extern void outputData3(address pc, const char* s1, const char* s2,
                        const char* s3);

extern char* writeTabAndCommentChar(char* buffer, int tabs);

extern void openOutputFile(char* basename, size_t splitByte, const char* ext);
extern void closeOutputFile(boolean freeBuf);

#endif

// EOF
