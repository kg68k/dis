// ソースコードジェネレータ
// 自動解析モジュール２
// Copyright (C) 1989,1990 K.Abe
// All rights reserved.
// Copyright (C) 2025 TcbnErik

#include <stdio.h>

#include "analyze.h"
#include "disasm.h"
#include "estruct.h"
#include "etc.h" /* charout, peek[wl] */
#include "global.h"
#include "label.h"  /* regist_label etc. */
#include "offset.h" /* depend_address , nearadrs */

// 指定アドレスの直前でプログラムが終了しているとみなせるか調べる
//   分岐命令、条件分岐命令、サブルーチンコール命令、リターン命令なら終了とみなす。
//
//   -yオプション指定時に使う。注目しているデータ領域の直後のラベル(=データ領域の
//   終了アドレス)を指定することで、その領域がプログラム領域の可能性が高いかどうか
//   を簡易的に調べる。
static boolean isProgramEndsAt(address end) {
  DisParam disp;
  int len;

  if (isOdd(end)) return FALSE;

  // 2バイト前が2バイト長の命令(rts等)か
  // 4バイト前が4バイト長の命令(bra.w label等)か
  // 6バイト前が6バイト長の命令(jmp (label).l等)か
  for (len = 2; len <= 6; len += 2) {
    address pc = end - len;

    if (pc < Dis.beginTEXT) break;

    setDisParamPcPtr(&disp, pc, Dis.Ofst);
    disp.pcEnd = Dis.availableTextEnd;

    if (dis(&disp) == len) {
      opetype t = disp.code.opeType;
      if (t != OTHER && t != UNDEF) return TRUE;
    }
  }

  return FALSE;
}

// 短い文字列の直後にプログラムがある構造を判別する
//
// 例) .dc.b 'a',0
//     .quad
//     movem.l d0-d7/a0-a6,-(sp)
// -n1なら文字列化＋プログラム検出できるが、文字列化しすぎるので……。
//
static boolean analyzeProgAfterStr(address pc, address end) {
  UBYTE* ptr;

  if ((end - pc) < 8) return FALSE;

  ptr = (UBYTE*)(Dis.Ofst + pc);
  if (ptr[0] == 0 || (ptr[2] | ptr[3]) || ptr[4] == 0) return FALSE;

  pc += 4;  // ここがプログラムかもしれない
  if (!analyze(pc, ANALYZE_NORMAL)) {
    unregist_label(pc);
    return FALSE;
  }
  return TRUE;
}

// 指定したラベル間がプログラム領域か判別する
//   プログラム領域と認識した場合は1を返す
static int analyzeDataAsProgram(lblbuf* start, lblbuf* end, boolean tweak) {
  address pc = start->label;
  lblmode mode = start->mode;
  analyze_mode analyzeMode;

  if (isOdd(pc) || isPROLABEL(mode) || isTABLE(mode)) return 0;
  if ((mode & FORCE) && !tweak) return 0;
  if (lblmodeOpesize(mode) != UNKNOWN) return 0;
  if (depend_address(pc)) return 0;

  if (Dis.y && !isProgramEndsAt(end->label)) return 0;

  if (mode & FORCE) {
    // -n3(既定値)で検出できないプログラム領域の判別
    // あまりにも決め打ちすぎるので、オプション化したほうがよいかも
    if (analyzeProgAfterStr(pc, end->label)) return 1;
    return 0;
  }

  if ((ULONG)(end->label - pc) == 2 && isDATLABEL(end->mode) &&
      peekw(Dis.Ofst + pc) == 0xffff) {
    // データ直前の0xffffはDOS _CHANGE_PRではなく-1という値とみなす
    return 0;
  }

  // 以下の解析により end->mode が変更される可能性があるので注意
  analyzeMode = Dis.i ? ANALYZE_IGNOREFAULT : ANALYZE_NORMAL;
  if (analyze(pc, analyzeMode)) return 1;

  return 0;
}

// データエリア中のプログラム領域を判別する
//   プログラムと認識したアドレスの数を返す
int research_data(boolean tweak) {
  lblbuf* lptr = next(Dis.beginTEXT);
  const address pcEnd = Dis.availableTextEnd;
  int found = 0;

  while (lptr->label < pcEnd) {
    lblbuf* nextlbl = Next(lptr);
    found += analyzeDataAsProgram(lptr, nextlbl, tweak);
    lptr = nextlbl;
  }

  return found;
}

/*

  データ領域にあるアドレス依存のデータを登録する
  データ領域にある 0x4e75(rts) の次のアドレスを登録する

*/
extern void analyze_data(void) {
  address data_from, adrs;
  address data_to = Dis.beginTEXT;

  do {
    data_from = next_datlabel(data_to)->label;
    data_to = MIN(next_prolabel(data_from)->label, Dis.beginBSS);
    charout('#');

    /* アドレス依存のデータがあれば、そのアドレスを登録する */
    for (adrs = data_from; (adrs = nearadrs(adrs)) < data_to && adrs < data_to;
         adrs += 4) {
      regist_label((address)peekl(adrs + Dis.Ofst),
                   DATLABEL | (lblmode)UNKNOWN);
    }

    if (Dis.h && data_from < Dis.availableTextEnd) {
      for (adrs = data_from + ((int)data_from & 1); adrs < data_to; adrs += 2) {
        UWORD word1 = peekw(adrs + Dis.Ofst);

        if (word1 == 0x4e75) {
          // rts の次のアドレスを登録する
          regist_label(adrs + 2, DATLABEL | (lblmode)UNKNOWN);
        }

        if (Dis.findLinkW) {
          // link 命令に注目
          if ((word1 & 0xfff8) == 0x4e50)
            regist_label(adrs, DATLABEL | (lblmode)UNKNOWN);
        }
      }
    }

  } while (data_to < Dis.beginBSS && adrs != (address)-1);
}

/*

  アドレステーブルを捜す

*/
extern void search_adrs_table(void) {
  lblbuf* lptr = next(Dis.beginTEXT);
  address pc = lptr->label;
  lblmode nmode = lptr->mode;

  while (pc < Dis.beginBSS) {
    address nlabel;
    lblmode mode = nmode;

    lptr = next(pc + 1);
    nlabel = lptr->label;
    nmode = lptr->mode;
    if (isDATLABEL(mode)) {
      address labeltop;
      int count;

      pc = MIN(nearadrs(pc), nlabel);
      labeltop = pc;
      for (count = 0; depend_address(pc) && pc < nlabel; count++) pc += 4;
      if (count >= 3) {
        int i;
        for (i = 0; i < count; i++) {
          address label = (address)peekl(labeltop + i * 4 + Dis.Ofst);
          analyze(label, (Dis.i ? ANALYZE_IGNOREFAULT : ANALYZE_NORMAL));
        }
      }
    }
    pc = nlabel;
  }
}

// EOF
