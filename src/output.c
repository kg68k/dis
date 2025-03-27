// ソースコードジェネレータ
// 出力ルーチン下請け
// Copyright (C) 2025 TcbnErik

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

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#ifdef _MSC_VER
#include <io.h>  // write()
#else
#include <unistd.h>
#endif

#include "disasm.h"
#include "estruct.h"
#include "etc.h"
#include "global.h"
#include "hex.h"
#include "output.h"

#define BLOCK_SIZE_MAX (1024 * 1024)
#define BLOCK_SIZE_MIN (16 * 1024)
#define MAX_LINE_BYTES 1024

typedef struct {
  FILE* fp;
  char* basename;
  char* filename;

  char* buffer;
  char* write;
  size_t blockSize;
  size_t bufferSize;  // blockSize + MAX_LINE_BYTES

  size_t splitByte;
  boolean isSplitMode;  // 指定サイズごとにファイルを .000 .001 と切り換える
  int fileBlockNum;     // 切り換え中のファイルの .000～ の番号
  size_t limit;         // 次に切り換えるサイズ(アドレス)

  int lineCount;  // -a

  LineType prevLineType;
} OutputFile;

static OutputFile outputFile;

// static 関数プロトタイプ
static char* appendAdrsCommentInner(address pc, char* buffer, char* end);
static void switchOutputFileInner(address pc);
static void openOutputFile2(void);
static char* createNumberedFilename(const char* basename, int n);
static void writeFileFromBuffer(size_t length);

// 必要なら出力ファイルを切り換える(インライン展開用)
static void switchOutputFile(address pc) {
  if (outputFile.isSplitMode) switchOutputFileInner(pc);
}

// 必要ならアドレスのコメントを追加する(インライン展開用)
static char* appendAdrsComment(address pc, char* buffer, char* end) {
  if (Dis.addressCommentLine == 0) return end;

  return appendAdrsCommentInner(pc, buffer, end);
}

// 必要ならファイル出力して、書き込みバッファの空きを確保する
static void ensureBufferCapacity(void) {
  size_t len = outputFile.write - outputFile.buffer;

  if (len >= outputFile.blockSize) {
    writeFileFromBuffer(outputFile.blockSize);
  }
}

// 前の行が空行でなければ、空行を出力する
void outputBlank(void) {
  if (outputFile.prevLineType == LINETYPE_BLANK) return;

  outputFile.prevLineType = LINETYPE_BLANK;
  *outputFile.write++ = '\n';
}

// 必要なら空行を出力する(次の行の種類を指定)
static void outputBlankBefore(LineType type, address pc) {
  if (type == LINETYPE_TEXT || type == LINETYPE_DATA) {
    // その他orデータ -> テキスト、その他orテキスト -> データに
    // 切り替われば空行を出力する
    if (outputFile.prevLineType != type) outputBlank();

    if (outputFile.prevLineType == LINETYPE_BLANK) {
      // 直前に空行を出力していれば、ファイル切り換えを試みる
      switchOutputFile(pc);
    }
  }
  outputFile.prevLineType = type;
}

// PCを進めない行(疑似命令等)を出力する
//   type==LINETYPE_BLANK or LINETYPE_OTHERの場合はpcは0でもよい。
void outputDirective(LineType type, address pc, const char* s) {
  char* p;

  outputBlankBefore(type, pc);

  ensureBufferCapacity();
  p = strcpy2(outputFile.write, s);
  *p++ = '\n';
  *p = '\0';
  outputFile.write = p;
}

// PCを進めない行を出力する(文字列×2)
void outputDirective2(LineType type, address pc, const char* s1,
                      const char* s2) {
  char* p;

  outputBlankBefore(type, pc);

  ensureBufferCapacity();
  p = strcpy2(outputFile.write, s1);
  p = strcpy2(p, s2);
  *p++ = '\n';
  *p = '\0';
  outputFile.write = p;
}

// PCを進めない行を出力する(配列指定)
void outputDirectiveArray(LineType type, address pc, size_t length,
                          const char* array[]) {
  char* p;
  size_t i;

  outputBlankBefore(type, pc);

  ensureBufferCapacity();
  p = outputFile.write;
  for (i = 0; i < length; ++i) p = strcpy2(p, array[i]);
  *p++ = '\n';
  *p = '\0';
  outputFile.write = p;
}

// 文字列を出力
static void outputString(LineType type, address pc, const char* s) {
  char* head;
  char* p;

  outputBlankBefore(type, pc);
  ensureBufferCapacity();
  head = outputFile.write;
  p = strcpy2(head, s);
  p = appendAdrsComment(pc, head, p);
  *p++ = '\n';
  *p = '\0';
  outputFile.write = p;
}

// 文字列(×2)を出力
static void outputString2(LineType type, address pc, const char* s1,
                          const char* s2) {
  char* head;
  char* p;

  outputBlankBefore(type, pc);
  ensureBufferCapacity();
  head = outputFile.write;
  p = strcpy2(head, s1);
  p = strcpy2(p, s2);
  p = appendAdrsComment(pc, head, p);
  *p++ = '\n';
  *p = '\0';
  outputFile.write = p;
}

// 文字列(×3)を出力
static void outputString3(LineType type, address pc, const char* s1,
                          const char* s2, const char* s3) {
  char* head;
  char* p;

  outputBlankBefore(type, pc);
  ensureBufferCapacity();
  head = outputFile.write;
  p = strcpy2(head, s1);
  p = strcpy2(p, s2);
  p = strcpy2(p, s3);
  p = appendAdrsComment(pc, head, p);
  *p++ = '\n';
  *p = '\0';
  outputFile.write = p;
}

// プログラム行を出力する
void outputText(address pc, const char* s) {
  outputString(LINETYPE_TEXT, pc, s);
}

// プログラム行を出力する(文字列×2)
void outputText2(address pc, const char* s1, const char* s2) {
  outputString2(LINETYPE_TEXT, pc, s1, s2);
}

// データ行を出力する
void outputData(address pc, const char* s) {
  outputString(LINETYPE_DATA, pc, s);
}

// データ行を出力する(文字列×2)
void outputData2(address pc, const char* s1, const char* s2) {
  outputString2(LINETYPE_DATA, pc, s1, s2);
}

// データ行を出力する(文字列×3)
void outputData3(address pc, const char* s1, const char* s2, const char* s3) {
  outputString3(LINETYPE_DATA, pc, s1, s2, s3);
}

// 文字列の末尾にTABとコメント文字を書き込む
//   TABの個数は指定のタブ位置になるまで(最低1個)
//   文字列末尾のアドレスを返す
char* writeTabAndCommentChar(char* buffer, int tabs) {
  char* p = buffer;
  char c;
  int width = 0;

  // 文字列の桁数を数える
  while ((c = *p++) != '\0') {
    if (c == '\t')
      width = (width | (TAB_WIDTH - 1)) + 1;
    else if (c == '\n')
      width = 0;
    else
      width += 1;
  }
  p -= 1;

  tabs -= (width / TAB_WIDTH);  // 必要なTABの個数
  tabs -= 1;                    // 最低1個は書き込む分の補正

  for (; tabs > 0; tabs--) *p++ = '\t';
  *p++ = '\t';
  *p++ = Dis.commentStr[0];

  return p;
}

// アドレスのコメントを追加する
static char* appendAdrsCommentInner(address pc, char* buffer, char* end) {
  char* p;

  outputFile.lineCount += 1;
  if (outputFile.lineCount < Dis.addressCommentLine) return end;

  // 指定行数に達した
  outputFile.lineCount = 0;

  p = writeTabAndCommentChar(buffer, Dis.Atab);
  return itox6(p, (ULONG)pc);
}

// 必要なら出力ファイルを切り換える
static void switchOutputFileInner(address pc) {
  if ((size_t)(pc - Dis.beginTEXT) < outputFile.limit) return;

  closeOutputFile(FALSE);

  outputFile.limit += outputFile.splitByte;
  outputFile.filename =
      createNumberedFilename(outputFile.basename, ++outputFile.fileBlockNum);
  openOutputFile2();
}

// 可変容量メモリ確保
static void* allocMemory(size_t max, size_t min, size_t add, size_t* base,
                         size_t* total) {
  size_t size;
  for (size = max; size >= min; size >>= 1) {
    size_t bufSize = size + add;
    void* buf = malloc(bufSize);

    if (buf) {
      *base = size;
      *total = bufSize;
      return buf;
    }
  }

  return NULL;
}

// 書き込みバッファを確保する
static void allocBuffer(void) {
  void* buf;
  size_t blockSize, bufSize;

  if (outputFile.buffer != NULL) return;

  buf = allocMemory(BLOCK_SIZE_MAX, BLOCK_SIZE_MIN, MAX_LINE_BYTES, &blockSize,
                    &bufSize);
  if (buf == NULL) notEnoughMemory();

  outputFile.blockSize = blockSize;
  outputFile.bufferSize = bufSize;
  outputFile.write = outputFile.buffer = buf;
}

// 書き込みバッファを解放する
static void freeBuffer(void) {
  free(outputFile.buffer);
  outputFile.write = outputFile.buffer = NULL;
}

// 出力ファイルを開く
//   splitByte>0のときは分割モードで、extが拡張子として追加される
//   (ext==NULLのときは ".001", ".002", ...)
void openOutputFile(char* basename, size_t splitByte, const char* ext) {
  allocBuffer();

  outputFile.basename = basename;
  outputFile.filename = NULL;
  outputFile.isSplitMode = FALSE;
  outputFile.lineCount = 0;
  outputFile.prevLineType = LINETYPE_BLANK;

  if (strcmp(basename, "-") == 0) {
    // ファイル名が - なら標準出力に書き出す
    outputFile.fp = stdout;
    return;
  }

  if (splitByte == 0) {
    outputFile.filename = Strdup(basename);
  } else {
    if (ext == NULL) {
      // .001 .002 ...
      outputFile.isSplitMode = TRUE;
      outputFile.splitByte = splitByte;
      outputFile.limit = splitByte;
      outputFile.fileBlockNum = 0;
      outputFile.filename = createNumberedFilename(outputFile.basename, 0);
    } else {
      // .dat .bss
      outputFile.filename = StrdupCat(basename, ext);
    }
  }

  openOutputFile2();
}

static void openOutputFile2(void) {
  outputFile.fp = fopen(outputFile.filename, "wb");
  if (outputFile.fp == NULL)
    err("\n%s をオープンできません。\n", outputFile.filename);
}

static char* createNumberedFilename(const char* basename, int n) {
  size_t bufSize = strlen(basename) + 16;
  char* buf = Malloc(bufSize);

  snprintf(buf, bufSize, "%s.%03x", basename, n);
  return buf;
}

// 出力ファイルを閉じる
void closeOutputFile(boolean freeBuf) {
  writeFileFromBuffer(outputFile.write - outputFile.buffer);
  if (freeBuf) freeBuffer();

  fclose(outputFile.fp);
  outputFile.fp = NULL;

  Mfree(outputFile.filename);
  outputFile.filename = NULL;
}

// 書き込みバッファ内のデータをファイルに書き出す
static void writeFileFromBuffer(size_t length) {
  char* buffer = outputFile.buffer;
  size_t wrote;
  size_t dataLen, restLen;

  if (length == 0) return;

  dataLen = outputFile.write - buffer;
  if (dataLen < length) internalError(__FILE__, __LINE__, "dataLen < length");

  wrote = (size_t)write(fileno(outputFile.fp), buffer, (unsigned int)length);
  if (wrote != length) {
    fclose(outputFile.fp);
    err("\nディスクが一杯です。\n");
  }

  restLen = dataLen - length;
  if (restLen != 0) {
    // 残りのデータをバッファ先頭に移動する
    memcpy(buffer, buffer + length, restLen);
  }
  outputFile.write = buffer + restLen;
}

// EOF
