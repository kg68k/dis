// ソースコードジェネレータ
// テーブル処理モジュール
// Copyright (C) 1989,1990 K.Abe
// All rights reserved.
// Copyright (C) 2025 TcbnErik

#include "table.h"

#include <ctype.h> /* isdigit, isxdigit */
#include <stdio.h>
#include <stdlib.h> /* atoi */
#include <string.h>

#include "estruct.h"
#include "etc.h"
#include "eval.h"
#include "global.h"
#include "label.h"
#include "labelfile.h"
#include "offset.h" /* depend_address */

static table* Table;
static int TableCounter;
static int Line_num;

// table.loop mode
enum {
  TIMES_AUTOMATIC = 0,
  TIMES_DECIDE_BY_BREAK = -1,
};

#define eprintfq \
  if (!Dis.quietTable) eprintf

static void bssCheck(int in_bss, const char* idstr, const char* filename,
                     int lineNo) {
  if (!in_bss) return;

  err("%s:%d: ブロックストレージセクションでは識別子%sは使えません。\n",
      filename, lineNo, idstr);
}

static void oddCheck(address pc, opesize size) {
  static const char* const t[] = {
      NULL,                // BYTESIZE
      "ワード",            // WORDSIZE
      "ロングワード",      // LONGSIZE
      NULL,                // QUADSIZE
      NULL,                // SHORTSIZE
      "単精度実数",        // SINGLESIZE
      "倍精度実数",        // DOUBLESIZE
      "拡張精度実数",      // EXTENDSIZE
      "パックドデシマル",  // PACKEDSIZE
  };

  if (isOdd(pc)) {
    eprintf("警告: " PRI_ADRS ": 奇数アドレスで%s指定されています。\n", pc,
            t[size]);
  }
}

// アドレス依存部がロングワード指定か
//
// 例えば元のソースコードで .dc.l label となっている部分は
// テーブルで dc.b と記述されていると元通りのソースコードを
// 出力できないので、そのチェックを行う。
static boolean isNotLongOnDependAddress(address pc, opesize size) {
  if (size == LONGSIZE || size == EVENID || size == BREAKID || size == CRID) {
    // dc.l、even、break、crならアドレス依存していてもよい
    return FALSE;
  }

  // それ以外の識別子は、アドレス依存しているとNG
  return depend_address(pc);
}

static address getEndOfSection(address pc) {
  if (pc < Dis.beginDATA) return Dis.beginDATA;
  if (pc < Dis.beginBSS) return Dis.beginBSS;
  if (pc < Dis.beginSTACK) return Dis.beginSTACK;
  return Dis.LAST;
}

static void interpret(table* table_ptr, const char* filename) {
  formula* cur_expr = table_ptr->formulaptr;
  lblbuf* lblptr = NULL;
  int loop = 0;

  const address tabletop = table_ptr->tabletop;
  address pc = tabletop;
  address pc0 = 0;

  const boolean in_bss = (Dis.beginBSS <= tabletop) ? TRUE : FALSE;
  const address limit = getEndOfSection(tabletop);

  ParseTblParam param = {
      .text = NULL,
      .buffer = NULL,
      .bufLen = 0,
      .constValues = {0, tabletop}  //
  };

  if (Dis.LAST <= tabletop) {
    err(PRI_ADRS
        ": "
        "テーブル開始アドレスとしてスタックセクション以降が指定されています。"
        "\n",
        tabletop);
  }

  // テーブル数のループ
  do {
    address blockTop = pc;
    int i;
    // 行のループ
    for (i = 0; i < table_ptr->lines; i++) {
      int count = 1;
      boolean firstCall = TRUE;

      // 繰り返し(dc.b[2]など)のループ
      // 初回のパース結果から繰り返し回数を得る
      do {
        ParseTblResult result;
        formula* const fml = &cur_expr[i];

        param.text = fml->expr;
        param.constValues[PARSE_CONST_PC] = pc;
        eprintfq("[%d:" PRI_ADRS "]:%s\n", loop + 1, pc, param.text);

        parseTableLine(&param, &result);
        if (result.pr != PARSE_SUCCESS) {
          err("%s:%d: %s\n", filename, fml->line, result.error);
        }

        if (firstCall) {
          count = result.count;
          firstCall = FALSE;
        }
        fml->id = result.id;
        pc0 = pc;

        if (isNotLongOnDependAddress(pc, result.id)) {
          if (table_ptr->loop == TIMES_AUTOMATIC) {
            pc = blockTop;
            goto tableend;
          }
          err(PRI_ADRS ": アドレス依存部でロングワード指定されていません。\n",
              pc);
        }

        switch (result.id) {
          default:
            break;

          case BYTESIZE:
            break;
          case WORDSIZE:
          case LONGSIZE:
          case SINGLESIZE:
          case DOUBLESIZE:
          case EXTENDSIZE:
          case PACKEDSIZE:
            oddCheck(pc, result.id);
            break;

          case BYTEID:
          case ASCIIID:
          case ASCIIZID:
          case LASCIIID:
            bssCheck(in_bss, result.idstr, filename, fml->line);
            break;

          case EVENID:
            if (result.bytes && result.value != 0) {
              // evenで飛ばすバイトの値は$00でなければならない
              if (table_ptr->loop == TIMES_AUTOMATIC) {
                pc = blockTop;
                goto tableend;
              }
              err(PRI_ADRS
                  ": %sに対応する奇数アドレスの値が$00ではありません。\n",
                  pc, result.idstr);
            }
            break;

          case CRID:
            break;

          case BREAKID:
            if (result.value) {  // 式の値が真(非0)ならテーブル終了
              loop++;
              goto tableend;
            }
            break;
        }
        pc += result.bytes;

        // テーブル解析の途中で有効なセクションの末尾に到達したか、
        // ラベルが登録されているアドレスに到達した場合はテーブル終了と判定する。
        // 現在の周回は有効となり、ソースコード生成時は途中までテーブルとして処理される。
        // (現在の周回は無効とすべきかもしれない)
        // なお、{式} を使って自ら登録したラベルも到達判定に使われる。
        if (limit <= pc ||
            (table_ptr->loop == TIMES_AUTOMATIC &&
             ((lblptr = next(pc0 + 1)) == NULL || lblptr->label <= pc))) {
          loop++;

          if (lblptr && lblptr->label < pc) pc = pc0;
          goto tableend;
        }

      } while (--count > 0);
    }

    if (blockTop == pc) {
      // 全ての行を処理してもPCが変わらなければ無限ループになるので中断する。
      err(PRI_ADRS
          "からPCが進みませんでした。テーブルファイルの修正が必要です。\n",
          pc);
    }

    loop++;
  } while ((pc < limit &&
            (table_ptr->loop == TIMES_AUTOMATIC &&
             (lblptr = next(pc0 + 1)) != NULL && pc < lblptr->label)) ||
           table_ptr->loop == TIMES_DECIDE_BY_BREAK ||
           (table_ptr->loop != TIMES_AUTOMATIC && loop < table_ptr->loop));

tableend:
  if (loop == 0) {
    err(PRI_ADRS "はテーブルとして判断できませんでした。\n", tabletop);
  }
  eprintfq("テーブルは" PRI_ADRS "-" PRI_ADRS "(%d個)と判断しました。\n",
           tabletop, pc, loop);

  regist_label(pc, DATLABEL | ENDTABLE | (lblmode)UNKNOWN);

  if (table_ptr->loop == TIMES_AUTOMATIC ||
      table_ptr->loop == TIMES_DECIDE_BY_BREAK) {
    table_ptr->loop = loop;  // テーブルの数が決定した
  }
}

static void interpret_all(const char* filename) {
  int i;
  for (i = 0; i < TableCounter; i++) interpret(&Table[i], filename);
}

/*

  テーブルの処理

*/
static void tablejob(address tabletop, FILE* fp, const char* filename) {
  formula* cur_expr;
  int formula_line;

  eprintfq("テーブル(" PRI_ADRS ")\n", tabletop);

  regist_label(tabletop, DATLABEL | TABLE | FORCE);

  /* テーブルバッファを一つ分確保 */
  Table = Realloc(Table, sizeof(table) * (TableCounter + 1));
  Table[TableCounter].tabletop = tabletop;

  cur_expr = 0; /* for Realloc() */
  formula_line = 0;

  while (1) {
    char linebuf[1024];

    if (fgets(linebuf, sizeof linebuf, fp) == NULL)
      err("\nテーブル中にファイル末尾に達しました。\n");
    removeTailLf(linebuf);

    Line_num++;
    if (linebuf[0] == '#' || linebuf[0] == '*')  // コメント行
      continue;

    if (strncasecmp(linebuf, "end", 3) == 0) {
      char* ptr;

      for (ptr = linebuf + 3; *ptr == ' ' || *ptr == '\t'; ptr++) {
        // "end"の後の空白を飛ばす
      }
      if (*ptr == '[') {
        for (ptr++; *ptr == ' ' || *ptr == '\t'; ptr++) {
          // "["の後の空白を飛ばす
        }
        if (isdigit(*ptr)) {
          Table[TableCounter].loop = atoi(ptr);
          break;
        } else if (strncasecmp(ptr, "breakonly", 9) == 0) {
          /* テーブル数を break のみで判断 */
          Table[TableCounter].loop = TIMES_DECIDE_BY_BREAK;
          break;
        } else if (*ptr == ']') {
          Table[TableCounter].loop = TIMES_AUTOMATIC;
          /* 指定されない -> 自動 */
          break;
        }
        err("\n%s:%d: end[]の指定が正しくありません。\n", filename, Line_num);
      } else
        Table[TableCounter].loop = 1;
      break;
    } else { /* if (strncasecmp (linebuf, "end", 3) != 0) */

      /* 行バッファを一つ分確保して格納する */
      cur_expr = Realloc(cur_expr, sizeof(formula) * (formula_line + 1));
      cur_expr[formula_line].line = Line_num;
      cur_expr[formula_line].expr = Strdup(linebuf);
      formula_line++;
    }
  }

  Table[TableCounter].formulaptr = cur_expr;
  Table[TableCounter].lines = formula_line;
  TableCounter++;
}

/*

  テーブル記述ファイルを読み込む

*/
extern void read_tablefile(char* filename) {
  char linebuf[256];
  FILE* fp = fopen(filename, "rt");

  if (!fp) err("\n%s をオープンできません。\n", filename);

  while (fgets(linebuf, sizeof linebuf, fp)) {
    char c = linebuf[0];

    Line_num++;
    if (c == '#' || c == '*' || c == ';') /* comment */
      continue;
    if (isxdigit(c)) {
      address tabletop = (address)atox(linebuf);
      tablejob(tabletop, fp, filename);
    }
  }
  interpret_all(filename);
  fclose(fp);
}

/*

  テーブルのサーチ

*/
extern table* search_table(address pc) {
  int i;

  for (i = 0; i < TableCounter; i++) {
    if (Table[i].tabletop == pc) return &Table[i];
  }
  return NULL;
}

// EOF
