// ソースコードジェネレータ
// 再配置テーブル管理モジュール
// Copyright (C) 1989,1990 K.Abe
// All rights reserved.
// Copyright (C) 1997-2023 TcbnErik

#include "offset.h"

#include <stdio.h>

#include "etc.h"
#include "global.h"

static address* Reltable = NULL; /* 再配置テーブルの頭 */
static int Relnum;               /* 再配置テーブルの要素数 */

/*

  再配置テーブルから再配置すべきアドレスのテーブルを作る

*/
void makeRelocateTableHuman(void) {
  address destadrs = Dis.beginTEXT;

  /* オフセット表へのポインタ (not always even!) */
  UBYTE* ptr = (UBYTE*)(Dis.Top + Dis.text + Dis.data);

  /* オフセット表のword数 */
  size_t units = Dis.offset / sizeof(UWORD);
  size_t remain;

  eprintf("リロケート情報を展開します。\n");

  Relnum = 0;
  Reltable = Malloc(units * sizeof(address));

  for (remain = units; remain > 0;) {
    UWORD w = (ptr[0] << 8) + ptr[1];
    ptr += 2;
    remain -= 1;

    if (w == 0x0001) {
      /* 0x0001 0x???? 0x???? : ロングワード距離 */
      if (remain < 2) {
        break; /* リロケート情報のサイズが不正 */
      }
      destadrs += (ptr[0] << 24) + (ptr[1] << 16) + (ptr[2] << 8) + ptr[3];
      ptr += 4;
      remain -= 2;
    } else {
      /* 0x???? ( != 0x0001 ) : ワード距離 */
      destadrs += w & 0xfffe;
    }
    Reltable[Relnum++] = destadrs;
  }
}

/*

  adrs がプログラムのロードされたアドレスに依存しているなら TRUE
  label1:  pea.l  (label2).l
  このとき depend_address(label1 + 2) == TRUE (label2が依存している)

*/
extern boolean depend_address(address adrs) {
  address* ptr = Reltable;
  int step = Relnum >> 1;

  while (step > 4) {
    if (*(ptr + step) <= adrs) /* binary search */
      ptr += step;
    step >>= 1;
  }
  for (; ptr < Reltable + Relnum; ptr++) {
    if (*ptr == adrs) {
      return TRUE;
    } else if (adrs < *ptr)
      return FALSE;
  }
  return FALSE;
}

/*

  adrs かそれより後で、最も近いアドレス依存のアドレスを返す
  そのような領域が無ければ 0xffffffff = -1 を返す

*/
extern address nearadrs(address adrs) {
  address rc = (address)-1;
  int step = Relnum >> 1;
  address* ptr = Reltable;

  while (step > 4) {
    if (*(ptr + step) < adrs) /* binary search */
      ptr += step;
    step >>= 1;
  }

  for (; ptr < Reltable + Relnum; ptr++) {
    if (adrs <= *ptr) {
      rc = *ptr;
      break;
    }
  }

  return rc;
}

// EOF
