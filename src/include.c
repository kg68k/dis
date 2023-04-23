// ソースコードジェネレータ
// インクルードファイル読み込みモジュール
// Copyright (C) 1989,1990 K.Abe, 1994 R.ShimiZu
// All rights reserved.
// Copyright (C) 1997-2023 TcbnErik

#include "include.h"

#include <stdio.h>
#include <string.h>

#include "etc.h"
#include "global.h"
#include "opstr.h"

// static 関数プロトタイプ
static boolean tryReadfile(const IncludeSpec* spec, const char* filename,
                           const char* path);

// 各パスからインクルードファイルを探し、見つけたファイルを読み込む
//   1) ./ (-Y オプション指定時のみ)
//   2) $dis_include/
//   3) $include/
//   の順に readfile() を試す。
void readInludeFileFromEnv(const IncludeSpec* spec, IncludeEnvs* env) {
  const char* filename = *spec->filenamePtr;
  int path_flag;

  // --exclude-***call 指定時は読み込まない
  if (filename == NULL || filename[0] == '\0') return;

  path_flag = strchr(filename, ':') || strchr(filename, '/');

  // -Y オプション指定時はカレントディレクトリから検索する
  // ファイル名にパスデリミタが含まれる場合もそのまま検索
  if (path_flag || Dis.Y) {
    if (readIncludeFile(spec, filename)) return;
  }

  // パスデリミタが含まれなければ環境変数 dis_include、include
  // のパスから検索する
  if (!path_flag) {
    if (tryReadfile(spec, filename, env->dis_include)) return;
    if (tryReadfile(spec, filename, env->include)) return;
  }

  if (path_flag || env->dis_include || env->include)
    err("%s をオープンできません。\n", filename);
  else
    err("環境変数 " ENV_dis_include ", " ENV_include
        " が設定されていません。\n");
}

// パスとファイル名を指定して読み込む
static boolean tryReadfile(const IncludeSpec* spec, const char* filename,
                           const char* path) {
  char* fullpath;

  if (path == NULL) return FALSE;

  fullpath = Malloc(strlen(path) + 1 + strlen(filename) + 1);
  strcat(strcat(strcpy(fullpath, path), "/"), filename);

  if (readIncludeFile(spec, fullpath)) {
    // フルパス名はソースコード出力時に使うのでバッファは解放しない
    return TRUE;
  }

  Mfree(fullpath);
  return FALSE;
}

#define IS_EOL(p) \
  ((*(p) < 0x20) || (*(p) == '*') || (*(p) == ';') || (*(p) == '#'))

/*
  includeファイルから読み込んだ文字列をシンボル名、疑似命令名、
  数字(または仮引数名)に分割し、各文字列へのポインタを設定して返す

  文字列の内容が正しくなかった場合は0、正しければ1を返す
*/
static int separate_line(char* ptr, char** symptr, char** psdptr,
                         char** parptr) {
  char* p = ptr;
  char c;

  /* 行頭の空白を飛ばす */
  while ((*p == ' ') || (*p == '\t')) p++;

  /* 空行または注釈行ならエラー */
  if (IS_EOL(p)) return 0;

  /* シンボル名の収得 */
  *symptr = p;

  /* シンボル名の末尾をNULにする */
  p = strpbrk(p, ":. \t");
  if (p == NULL) return 0;
  c = *p;
  *p++ = '\0';

  /* シンボル定義の':'、空白、疑似命令の'.'を飛ばす */
  while (c == ':') c = *p++;
  while ((c == ' ') || (c == '\t')) c = *p++;
  if (c == '.') c = *p++;
  p--;

  /* 疑似命令が記述されていなければエラー */
  if (IS_EOL(p)) return 0;

  /* 疑似命令へのポインタを設定 */
  *psdptr = p;

  /* 疑似命令名の末尾をNULにする */
  p = strpbrk(p, " \t");
  if (p == NULL) return 0;
  *p++ = '\0';

  /* パラメータまでの空白を飛ばす */
  while ((*p == ' ') || (*p == '\t')) p++;

  /* パラメータが記述されていなければエラー */
  if (IS_EOL(p)) return 0;

  /* パラメータへのポインタを設定 */
  *parptr = p;

  /* パラメータの末尾をNULにする */
  while (*p++ > 0x20)
    ;
  *--p = '\0';

  return 1;
}

/*

  アセンブラのequ/macro疑似命令の行を読み取る
  シンボル定義を読み取ったら1を返し、それ以外は0を返す
  (二度目の定義を無視した時や、macro定義なども0)

*/
static int getlabel(const IncludeSpec* spec, char* linebuf) {
  char* symptr;
  char* psdptr;
  char* parptr;

  if (!separate_line(linebuf, &symptr, &psdptr, &parptr)) return 0;

  // コール名: .equ コール番号
  if (strcasecmp(psdptr, "equ") == 0) {
    ULONG val;
    char** p;

    if (*parptr == '$')
      parptr++;
    else if (strncasecmp(parptr, "0x", 2) == 0)
      parptr += 2;
    else
      return 0;

    val = atox(parptr);
    if ((val & ~spec->callnoMask) != spec->high) return 0;

    p = *spec->labelListPtr + (val & spec->callnoMask);
    if (*p) return 0;  // 同じファンクションコールが定義済み

    *p = strcpy(Malloc(strlen(symptr) + 1), symptr);
    return 1;
  }

  // マクロ名: .macro 実引数名
  if (strcasecmp(psdptr, "macro") == 0) {
    if ((strlen(symptr) < MAX_MACRO_LEN) && spec->macroPtr) {
      strcpy(spec->macroPtr, symptr);
    }
  }
  return 0;
}

// ファンクション名への配列を確保・初期化
static void allocateLabelBuffer(const IncludeSpec* spec) {
  size_t size = spec->callnoMask + 1;  // 256 (SXCALLのみ4096)
  char** p = *spec->labelListPtr = Malloc(sizeof(char*) * size);
  size_t i;

  for (i = 0; i < size; i++) *p++ = NULL;
}

// ファイルからシンボル定義を読み込む
//   ファイルのオープンに失敗した場合は0を返し、それ以外は1を返す。
//   シンボルが一個も定義されていなくても警告を表示するだけでアボートはしない
int readIncludeFile(const IncludeSpec* spec, const char* filename) {
  FILE* fp;
  int label_num = 0;
  char linebuf[256];

  if ((fp = fopen(filename, "rt")) == NULL) return 0;

  allocateLabelBuffer(spec);

  eprintf("%s を読み込みます。\n", filename);
  while (fgets(linebuf, sizeof linebuf, fp))
    label_num += getlabel(spec, linebuf);

  fclose(fp);

  if (label_num == 0)
    eprintf("警告: %s にはシンボルが1つも定義されていません。\n", filename);

  *spec->fullpathPtr = filename;
  return 1;
}

// EOF
