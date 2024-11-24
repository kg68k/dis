// ソースコードジェネレータ
// 文字列判定
// Copyright (C) 1989,1990 K.Abe
// All rights reserved.
// Copyright (C) 1997-2023 TcbnErik

#include <stdio.h>
#include <string.h>

#include "estruct.h"
#include "etc.h" /* charout, <jctype.h> */
#include "global.h"
#include "label.h"  /* regist_label etc. */
#include "offset.h" /* depend_address, nearadrs */

static void foundstr(address pc) {
  charout('s');
  regist_label(pc, DATLABEL | STRING);
}

/*

  文字列かどうかを調べる
  *newend = last string address

*/
static boolean is_string(address from, address end, address* newend,
                         int str_min) {
  UBYTE* store = (UBYTE*)(from + Dis.Ofst);
  UBYTE* stend = (UBYTE*)(end + Dis.Ofst);

  *newend = end;

  // .dc.l 1000000000 が .dc.b ';毀',0 にならないようにする
  if (isEven(from) && (end - from) >= sizeof(ULONG)) {
    if (peekl(store) == 1000000000) return FALSE;
  }

  while (store < stend) {
    char c = *store++;

    if (isprkana(c)) /* ANK 文字 */
      ;
    else if (iskanji(c)) { /* 全角文字 */
      if (!iskanji2(*store++)) return FALSE;
    } else if (c == 0x80) { /* 半角ひらがな */
      c = *store++;
      if (c < 0x86 || 0xfc < c) return FALSE;
    } else {
      switch (c) {
        case 0x07: /* BEL */
        case 0x09: /* TAB */
        case 0x0d: /* CR  */
        case 0x0a: /* LF  */
        case 0x1a: /* EOF */
        case 0x1b: /* ESC */
          break;

        case 0x00: /* NUL */
          if (from + str_min < store - Dis.Ofst) {
            while (store < stend && !*store) store++;
            *newend = MIN((address)(store - Dis.Ofst), end);
            return TRUE;
          }
          return FALSE;
        default:
          return FALSE;
      }
    }
  }

  if (from + str_min <= store - Dis.Ofst) {
    *newend = MIN((address)(store - Dis.Ofst), end);
    return TRUE;
  }
  return FALSE;
}

/*

  pc から nlabel までのデータ領域中の文字列をチェック

*/
static int check_data_area(address pc, address nlabel, int str_min) {
  int num_of_str = 0;

  while (pc < nlabel) {
    address ltemp, nlabel2;

    nlabel2 = ltemp = MIN(nearadrs(pc), nlabel);
    while (pc < nlabel2) {
      if (is_string(pc, ltemp, &ltemp, str_min)) {
        foundstr(pc);
        num_of_str++;
        while (depend_address(ltemp) && ltemp + 4 <= nlabel2) ltemp += 4;
        regist_label(ltemp, DATLABEL | UNKNOWN);
        pc = ltemp;
        ltemp = MIN(nearadrs(pc), nlabel);
      } else
        pc = nlabel2;
    }
    while (depend_address(pc)) pc += 4;
  }

  return num_of_str;
}

/*

  文字列サーチ
  文字列として認識した数を返す

*/
extern int search_string(int str_min) {
  lblbuf* nadrs = next(Dis.beginTEXT);
  lblmode nmode = nadrs->mode;
  address pc = nadrs->label; /* 最初 */
  int num_of_str = 0;

  while (pc < Dis.beginBSS) {
    lblmode mode = nmode;
    address nlabel;

    charout('.');
    nadrs = Next(nadrs);
    nlabel = nadrs->label;
    nmode = nadrs->mode;

    if (isDATLABEL(mode) && lblmodeOpesize(mode) == UNKNOWN)
      num_of_str += check_data_area(pc, nlabel, str_min);

    pc = nlabel;
  }

  return num_of_str;
}

/* EOF */
