// ソースコードジェネレータ
// ラベル管理モジュール
// Copyright (C) 1989,1990 K.Abe
// All rights reserved.
// Copyright (C) 1997-2023 TcbnErik

#include "label.h"

#include <stdio.h>
#include <stdlib.h>

#include "etc.h"
#include "global.h"

#define AVL_USERDATA lblbuf
#define AVL_INCLUDE
#define AVL_NO_DESTROY_TREE
#define AVL_NO_SEARCH_PREVIOUS
#define AVL_NO_CHECK_TREE
#include "./avl/avl.h"

#define AVL_COMPARE(label1, label2) \
  (LONG)((ULONG)(label1)->label - (ULONG)(label2)->label)

#include "./avl/avl.c"

static avl_root_node* LabelRoot;
static lblbuf Nomore = {(address)-1, NULL, DATLABEL | UNKNOWN, 0, 1};

static int compare(lblbuf* label1, lblbuf* label2) {
  return (ULONG)label1->label - (ULONG)label2->label;
}

static void print(lblbuf* label) { printf(PRI_ADRS, label->label); }

/*

  ラベルバッファを初期化する

*/
extern void init_labelbuf(void) {
  LabelRoot = AVL_create_tree(compare, (void (*)(AVL_USERDATA*))free, print);
}

// アドレス登録(デバッグ表示つき)
boolean registerLabelDebug(address adrs, lblmode mode, address target,
                           const char* file, int line) {
  boolean result = registerLabel(adrs, mode);

  if (adrs == target) {
    const char* b = result ? "TRUE" : "FALSE";
    lblbuf* lptr = search_label(adrs);
    eprintf("\n%s:%d: registerLabel(adrs=" PRI_ADRS
            ", mode=$%06x) -> %s (mode=$%06x). ",
            file, line, adrs, (unsigned int)mode, b, (unsigned int)lptr->mode);
  }

  return result;
}

// 新規ラベル登録
static boolean registerNewLabel(address adrs, lblmode mode) {
  lblbuf* lptr = Malloc(sizeof(lblbuf));

  lptr->label = adrs;
  lptr->avl = AVL_insert(LabelRoot, lptr);
  lptr->mode = mode;
  lptr->shift = 0;
  lptr->count = 1;

  return TRUE;
}

/*

  adrs を mode 型のエントリアドレスとして登録する
  既に登録されていたら（登録されていたアドレスの型を oldmode とする）
        modeがプログラム && oldmodeがプログラム  FALSE
        modeがプログラム && oldmodeがデータ      登録し直し TRUE
        modeがデータ     && oldmodeがプログラム  FALSE
        modeがデータ     && oldmodeがデータ
        mode == KNOWN && oldmode == UNKNOWN      登録し直し
        oldmode != mode                          登録し直し
        mode == UNKNOWN && oldmode == KNOWN      なにもしない FALSE を返す
        返り値は adrs から解析する必要があるかを表す(プログラムの場合)

*/
boolean registerLabel(address adrs, lblmode mode) {
  lblbuf* lptr;
  const lblmode protect = ENDTABLE | SYMLABEL | DEVLABEL;

  if (adrs < Dis.beginTEXT || adrs > Dis.LAST) return FALSE;

  // プログラム領域として受理できないアドレスを処理
  if (isPROLABEL(mode)) {
    if (isOdd(adrs)) return FALSE;
    if (adrs > Dis.availableTextEnd) {
      regist_label(adrs, DATLABEL | UNKNOWN);
      return FALSE;
    }
  }

  // 未登録なら新規作成
  lptr = search_label(adrs);
  if (lptr == NULL) return registerNewLabel(adrs, mode);

  // 登録済み
  lptr->count++;

  if (mode & (TABLE | ENDTABLE | DEVLABEL)) {
    if (isTABLE(mode)) {
      lptr->mode = (lptr->mode & protect) | mode;
      return FALSE;
    }
    if (isENDTABLE(mode)) {
      // テーブルの終端を登録するために呼び出された場合はこのアドレスの
      // 属性やこの領域の中身については関与していないので、ENDTABLE
      // をセットするだけにする。
      lptr->mode |= ENDTABLE;
      return FALSE;
    }

    else {  // isDEVLABEL(mode)
      lptr->mode = (lptr->mode & protect) | mode;
      return FALSE;
    }
  }

  if (lptr->mode & FORCE) {
    if (isPROLABEL(lptr->mode) || isPROLABEL(mode)) return FALSE;
  }
  if (mode & FORCE) {
    lptr->mode = (lptr->mode & (TABLE | protect)) | mode;
    return TRUE;
  }

  if (isPROLABEL(lptr->mode)) {
    // プログラム -> プログラム(TRUE)またはデータ(FALSE)
    lptr->mode &= ~HIDDEN;
    return isPROLABEL(mode) ? TRUE : FALSE;
  }

  if (isPROLABEL(mode)) {
    // データ -> プログラム(TRUE)
    lptr->mode = (lptr->mode & protect) | PROLABEL;
    return TRUE;
  }

  // データ -> データ(TRUE)
  if (lblbufOpesize(lptr) == UNKNOWN || lblmodeOpesize(mode) != UNKNOWN) {
    // データサイズが指定されたら更新する
    lptr->mode = (lptr->mode & (FORCE | TABLE | protect)) | mode;
  }
  return TRUE;
}

static avl_node* search_label2(address adrs) {
  lblbuf search_data;

  search_data.label = adrs;
  return AVL_search(LabelRoot, &search_data);
}

/*

  ラベルの登録を取り消す

*/
extern void unregist_label(address adrs) {
  avl_node* del = search_label2(adrs);

  if (del != NULL) {
    lblbuf* ptr = (lblbuf*)AVL_get_data(del);

    if (--(ptr->count) == 0 && !(ptr->mode & SYMLABEL))
      AVL_delete(LabelRoot, del);
  }
}

/*

  ラベルバッファからadrsを捜す
  あればバッファに対するポインタ
  なければ NULL を返す

*/
extern lblbuf* search_label(address adrs) {
  lblbuf search_data;

  search_data.label = adrs;
  return AVL_get_data_safely(AVL_search(LabelRoot, &search_data));
}

/*

  adrs の次のラベルバッファへのポインタを返す
  返り値->label が -1 ならバッファエンプティ
  adrs と等しいラベルがあればそれが返ってくるので注意！

*/
extern lblbuf* next(address adrs) {
  lblbuf search_data;
  avl_node* node;

  if (adrs == (address)-1) return &Nomore;

  search_data.label = adrs;

  if ((node = AVL_search_next(LabelRoot, &search_data)) == NULL)
    return &Nomore;
  else
    return AVL_get_data(node);
}

/*

  lptr の次のラベルバッファへのポインタを返す
  返り値->label が -1 ならバッファエンプティ

*/
extern lblbuf* Next(lblbuf* lptr) {
  lblbuf* rc = AVL_get_data_safely(AVL_next(lptr->avl));

  return rc ? rc : &Nomore;
}

/*

  adrs から PROLABEL を探す
  adrs が PROLABEL ならそれが返ってくる

*/
extern lblbuf* next_prolabel(address adrs) {
  avl_node* node_ptr;
  lblbuf search_data;

  if (adrs == (address)-1) return &Nomore;

  search_data.label = adrs;
  node_ptr = AVL_search_next(LabelRoot, &search_data);
  while (node_ptr && !isPROLABEL(((lblbuf*)AVL_get_data(node_ptr))->mode))
    node_ptr = AVL_next(node_ptr);

  if (node_ptr == NULL) return &Nomore;

  return (lblbuf*)AVL_get_data(node_ptr);
}

/*

  adrs から DATLABEL を探す
  adrs が DATLABEL ならそれが返ってくる

*/
extern lblbuf* next_datlabel(address adrs) {
  avl_node* node_ptr;
  lblbuf search_data;
  lblbuf* lblptr = &Nomore;

  if (adrs == (address)-1) return &Nomore;

  search_data.label = adrs;
  node_ptr = AVL_search_next(LabelRoot, &search_data);
  while (node_ptr && (!isDATLABEL((lblptr = AVL_get_data(node_ptr))->mode) ||
                      lblptr->shift))
    node_ptr = AVL_next(node_ptr);

  if (node_ptr == NULL) return &Nomore;

  return lblptr;
}

extern int get_Labelnum(void) { return AVL_data_number(LabelRoot); }

// EOF
