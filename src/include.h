// ソースコードジェネレータ
// インクルードファイル ヘッダ
// Copyright (C) 1989,1990 K.Abe
// All rights reserved.
// Copyright (C) 1997-2023 TcbnErik

#ifndef INCLUDE_H
#define INCLUDE_H

#include "estruct.h"

typedef struct {
  unsigned int callnoMask;  // (1 << n) - 1 であること
  ULONG high;  // コール番号以外の値(例えばDOSコールなら0xff00)
  char*** labelListPtr;  // 確保した配列を記録する変数へのポインタ
  const char** fullpathPtr;  // 読み込んだファイル名を記録する変数へのポインタ
  const char** filenamePtr;  // ファイル名が記録されている変数へのポインタ
  char* macroPtr;  // マクロ名を書き込むバッファ
} IncludeSpec;

typedef struct {
  char* dis_include;
  char* include;
} IncludeEnvs;

extern void readInludeFileFromEnv(const IncludeSpec* spec, IncludeEnvs* env);
extern int readIncludeFile(const IncludeSpec* spec, const char* filename);

#endif

// EOF
