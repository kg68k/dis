// ソースコードジェネレータ
// ラベル管理モジュールヘッダ
// Copyright (C) 1989,1990 K.Abe
// All rights reserved.
// Copyright (C) 2025 TcbnErik

#ifndef LABEL_H
#define LABEL_H

#include "estruct.h"

// アドレスの属性
//   下位8ビットはopesize(データラベルの時のみ意味を持つ)
typedef enum {
  PROLABEL = 0x0000 << 16,  // 第16ビット=0: プログラム
  DATLABEL = 0x0001 << 16,  // 第16ビット=1: データ
  FORCE = 0x0002 << 16,     // 1なら現在の登録内容を強制する
  HIDDEN = 0x0004 << 16,  // 1ならそのラベルはソース中に現れない
  TABLE = 0x0008 << 16,   // 1でテーブル開始
  ENDTABLE = 0x0010 << 16,  // 1でテーブル終了
  SYMLABEL = 0x0020 << 16,  // 1でシンボル情報あり
  DEVLABEL = 0x0040 << 16,  // 1でデバイスヘッダ
  ABORTLABEL = 0x0080 << 16,  // 1で命令列中断(データ用)

  // OSK専用
  CODEPTR = 0x0400 << 16,  // 1でコードポインタ
  DATAPTR = 0x0800 << 16,  // 1でデータポインタ
} lblmode;

struct _avl_node;
typedef struct {
  address label;          // アドレス
  struct _avl_node* avl;  // AVL-tree-library side node
  lblmode mode;           // 属性
  int16_t shift;          // ラベルアドレスとのずれ。通常0
  uint16_t count;         // 登録回数
} lblbuf;

extern void init_labelbuf(void);
extern boolean registerLabelDebug(address adrs, lblmode mode, address target,
                                  const char* file, int line);
extern boolean registerLabel(address adrs, lblmode mode);
extern void unregist_label(address adrs);
extern lblbuf* search_label(address adrs);
extern lblbuf* next(address adrs);
extern lblbuf* Next(lblbuf*);
extern lblbuf* next_prolabel(address adrs);
extern lblbuf* next_datlabel(address adrs);
extern int get_Labelnum(void);

#ifdef DEBUG_REGIST_LABEL
#define regist_label(adrs, mode) \
  registerLabelDebug(adrs, mode, DEBUG_REGIST_LABEL, __FILE__, __LINE__)
#else
#define regist_label(adrs, mode) registerLabel(adrs, mode)
#endif

#define isPROLABEL(a) (!isDATLABEL(a))
#define isDATLABEL(a) ((a)&DATLABEL)
#define isHIDDEN(a) ((a)&HIDDEN)
#define isTABLE(a) ((a)&TABLE)
#define isENDTABLE(a) ((a)&ENDTABLE)
#define isDEVLABEL(a) ((a)&DEVLABEL)
#define isABORTLABEL(a) ((a)&ABORTLABEL)

static inline opesize lblbufOpesize(lblbuf* lptr) { return lptr->mode & 0xff; }
static inline opesize lblmodeOpesize(lblmode mode) { return mode & 0xff; }

#endif

// EOF
