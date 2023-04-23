// ソースコードジェネレータ
// 自動解析モジュール ヘッダ
// Copyright (C) 1989,1990 K.Abe
// All rights reserved.
// Copyright (C) 1997-2023 TcbnErik

#ifndef ANALYZE_H
#define ANALYZE_H

#include "estruct.h"
#include "label.h"

typedef enum { ANALYZE_IGNOREFAULT, ANALYZE_NORMAL } analyze_mode;

extern void setReasonVerbose(uint8_t lv);
extern boolean analyze(address, analyze_mode);
extern void not_program(address, address);
extern boolean ch_lblmod(address, lblmode);

extern void initReltblArray(ArrayBuffer* rtbuf);
extern int analyzeRegisteredReltbl(ArrayBuffer* rtbuf);
extern void registerReltblOrder(ArrayBuffer* rtbuf, address table, opesize size,
                                boolean isProgram, lblmode mode);

extern void z_table(address);
extern int research_data(boolean tweak);
extern void analyze_data(void);
extern void search_adrs_table(void);
extern void search_operand_label(void);
extern int search_string(int);

#endif

// EOF
