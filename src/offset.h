// ソースコードジェネレータ
// 再配置テーブル管理 ヘッダ
// Copyright (C) 1989,1990 K.Abe
// All rights reserved.
// Copyright (C) 1997-2023 TcbnErik

#ifndef OFFSET_H
#define OFFSET_H

#include "estruct.h"
#include "global.h"

extern void makeRelocateTableHuman(void);
extern boolean depend_address(address);
extern address nearadrs(address);

#define INPROG(opval, eaadrs)                                   \
  (depend_address(eaadrs) ||                                    \
   (Dis.fileType == FILETYPE_Z && Dis.base - 0x100 <= opval) || \
   (Dis.fileType == FILETYPE_DUMP && Dis.base <= opval && opval <= Dis.LAST))

#endif

// EOF
