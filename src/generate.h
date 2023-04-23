// ソースコードジェネレータ
// ソースコードジェネレートモジュール ヘッダ
// Copyright (C) 1989,1990 K.Abe
// All rights reserved.
// Copyright (C) 1997-2023 TcbnErik

#ifndef GENERATE_H
#define GENERATE_H

#include <time.h>  // time_t

#include "disasm.h"
#include "estruct.h"
#include "label.h"
#include "output.h"

typedef struct {
  opesize size;
  int length;
} SizeLength;

extern char* make_proper_symbol(char*, address);
extern void com(const char* fmt, ...) GCC_PRINTF(1, 2);
extern void comBlank(void);
extern void outputHeaderCpu(void);
extern void generate(char* xfilename, char* sfilename, time_t filedate,
                     const char* argv0, char* cmdline);
extern lblbuf* generateSection(char* sfilename, char* section, address end,
                               lblbuf* lptr, int type, int* currentType);
extern void disasmlist(char*);
extern SizeLength getSizeBytes(opesize size);
extern SizeLength getSizeLength(SizeLength sb, int bytes);
extern void dataoutDc(address pc, codeptr store, opesize size, ULONG bytes,
                      ULONG bytesPerUnit);
extern void dataoutDcByte(address pc, address pcend);
extern void label_line_out(address adrs, lblmode mode, int sectType,
                           boolean last, LineType lineType);
extern const char* ctimez(time_t* clock);
extern void otherDirective(const char* s);
extern void otherDirective2(const char* s1, const char* s2);

#endif

// EOF
