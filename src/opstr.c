// ソースコードジェネレータ
// オペコード文字列
// Copyright (C) 2023 TcbnErik

// This file is part of dis (source code generator).
//
// This program is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program. If not, see <https://www.gnu.org/licenses/>.

#include "opstr.h"

#include "etc.h"
#include "global.h"

// コンディションコード
enum {
  COND_F = 1,
};

OpStringTable OpString = {
    // 疑似命令
    {"\t.dc.b\t", "\t.dc.w\t", "\t.dc.l\t", "\t.dc.d\t", "",  //
     "\t.dc.s\t", "\t.dc.d\t", "\t.dc.x\t", "\t.dc.p\t"},
    {"\t.dcb.b\t", "\t.dcb.w\t", "\t.dcb.l\t", "\t.dcb.d\t", "",  //
     "\t.dcb.s\t", "\t.dcb.d\t", "\t.dcb.x\t", "\t.dcb.p\t"},
    {"\t.ds.b\t", "\t.ds.w\t", "\t.ds.l\t", "\t.ds.d\t", "",  //
     "\t.ds.s\t", "\t.ds.d\t", "\t.ds.x\t", "\t.ds.p\t"},

    "\t.include\t",
    "\t.cpu\t",
    "\t.fpid\t",
    "\t.equ\t",
    "\t.xdef\t",

    "\t.text",
    "\t.data",
    "\t.bss",
    "\t.stack",
    "\t.end",

    "\t.even",
    "\talign",

#ifdef OSKDIS
    "\tpsect",
    "\tvsect",
    "\tvsect\tremote",
    "\tends",

    "TCALL",
    "OS9",
#endif

    ".dc",

    // MPU命令
    {"bra", "bsr", "bhi", "bls", "bcc", "bcs", "bne", "beq",  //
     "bvc", "bvs", "bpl", "bmi", "bge", "blt", "bgt", "ble"},
    {"dbt", "dbra", "dbhi", "dbls", "dbcc", "dbcs", "dbne", "dbeq",  //
     "dbvc", "dbvs", "dbpl", "dbmi", "dbge", "dblt", "dbgt", "dble"},
    {"st", "sf", "shi", "sls", "scc", "scs", "sne", "seq",  //
     "svc", "svs", "spl", "smi", "sge", "slt", "sgt", "sle"},
    {
        "trapt", "trapf", "traphi", "trapls",    //
        "trapcc", "trapcs", "trapne", "trapeq",  //
        "trapvc", "trapvs", "trappl", "trapmi",  //
        "trapge", "traplt", "trapgt", "traple",  //
    },

    {"btst", "bchg", "bclr", "bset"},
    {"bftst", "bfextu", "bfchg", "bfexts", "bfclr", "bfffo", "bfset", "bfins"},
    {"", "cinvl", "cinvp", "cinva", "", "cpushl", "cpushp", "cpusha"},
    {"pflushn", "pflush", "pflushan", "pflusha"},
    {"plpaw", "plpar"},
    {"ptestw", "ptestr"},
    {"asr", "asl", "lsr", "lsl", "roxr", "roxl", "ror", "rol"},
    {"tblu", "tblun", "tbls", "tblsn"},

    "dbf",

    "abcd",
    "add",
    "adda",
    "addi",
    "addi",
    "addq",
    "addx",
    "and",
    "andi",
    "andi",
    "bgnd",
    "bkpt",
    "callm",
    "cas",
    "cas2",
    "chk",
    "chk2",
    "clr",
    "cmp",
    "cmp2",
    "cmpa",
    "cmpi",
    "cmpi",
    "cmpm",
    "divs",
    "divsl",
    "divu",
    "divul",
    "eor",
    "eori",
    "exg",
    "ext",
    "extb",
    "illegal",
    "jmp",
    "jsr",
    "lea",
    "link",
    "lpstop",
    "move",
    "move16",
    "movea",
    "movec",
    "movem",
    "movep",
    "moveq",
    "moves",
    "muls",
    "mulsl",
    "mulu",
    "mulul",
    "nbcd",
    "neg",
    "negx",
    "nop",
    "not",
    "or",
    "ori",
    "ori",
    "pack",
    "pea",
    "reset",
    "rtd",
    "rte",
    "rtm",
    "rtr",
    "rts",
    "sbcd",
    "stop",
    "sub",
    "suba",
    "subi",
    "subi",
    "subq",
    "subx",
    "swap",
    "tas",
    "trap",
    "trapv",
    "tst",
    "unlk",
    "unpk",

    // FPU命令
    {"fdbra",   "fdbeq",  "fdbogt", "fdboge", "fdbolt", "fdbole",
     "fdbogl",  "fdbor",  "fdbun",  "fdbueq", "fdbugt", "fdbuge",
     "fdbult",  "fdbule", "fdbne",  "fdbt",   "fdbsf",  "fdbseq",
     "fdbgt",   "fdbge",  "fdblt",  "fdble",  "fdbgl",  "fdbgle",  //
     "fdbngle", "fdbngl", "fdbnle", "fdbnlt", "fdbnge", "fdbngt",
     "fdbsne",  "fdbst"},
    {"ftrapf",    "ftrapeq",  "ftrapogt", "ftrapoge", "ftrapolt", "ftrapole",
     "ftrapogl",  "ftrapor",  "ftrapun",  "ftrapueq", "ftrapugt", "ftrapuge",
     "ftrapult",  "ftrapule", "ftrapne",  "ftrapt",   "ftrapsf",  "ftrapseq",
     "ftrapgt",   "ftrapge",  "ftraplt",  "ftraple",  "ftrapgl",  "ftrapgle",
     "ftrapngle", "ftrapngl", "ftrapnle", "ftrapnlt", "ftrapnge", "ftrapngt",
     "ftrapsne",  "ftrapst"},
    {"fsf",    "fseq",  "fsogt", "fsoge", "fsolt", "fsole",
     "fsogl",  "fsor",  "fsun",  "fsueq", "fsugt", "fsuge",
     "fsult",  "fsule", "fsne",  "fst",   "fssf",  "fsseq",
     "fsgt",   "fsge",  "fslt",  "fsle",  "fsgl",  "fsgle",  //
     "fsngle", "fsngl", "fsnle", "fsnlt", "fsnge", "fsngt",
     "fssne",  "fsst"},
    {"fbf",    "fbeq",  "fbogt", "fboge", "fbolt", "fbole",
     "fbogl",  "fbor",  "fbun",  "fbueq", "fbugt", "fbuge",
     "fbult",  "fbule", "fbne",  "fbra",  "fbsf",  "fbseq",
     "fbgt",   "fbge",  "fblt",  "fble",  "fbgl",  "fbgle",  //
     "fbngle", "fbngl", "fbnle", "fbnlt", "fbnge", "fbngt",
     "fbsne",  "fbst"},

    {"fmove",   "fint",    "fsinh",   "fintrz",  "fsqrt",  "flognp1",
     "fetoxm1", "ftanh",   "fatan",   "fasin",   "fatanh", "fsin",
     "ftan",    "fetox",   "ftwotox", "ftentox", "flogn",  "flog10",
     "flog2",   "fabs",    "fcosh",   "fneg",    "facos",  "fcos",
     "fgetexp", "fgetman", "fdiv",    "fmod",    "fadd",   "fmul",
     "fsgldiv", "frem",    "fscale",  "fsglmul", "fsub",   "fsincos",
     "fcmp",    "ftst",    "fsmove",  "fssqrt",  "fdmove", "fdsqrt",
     "fsabs",   "fsneg",   "fdabs",   "fdneg",   "fsdiv",  "fsadd",
     "fsmul",   "fddiv",   "fdadd",   "fdmul",   "fssub",  "fdsub"},

    "fdbf",

    "fmovecr",
    "fmovem",
    "fnop",
    "fsave",
    "frestore",  //

    // MMU命令
    {"pdbbs", "pdbbc", "pdbls", "pdblc", "pdbss", "pdbsc", "pdbas", "pdbac",
     "pdbws", "pdbwc", "pdbis", "pdbic", "pdbgs", "pdbgc", "pdbcs", "pdbcc"},
    {"ptrapbs", "ptrapbc", "ptrapls", "ptraplc",  //
     "ptrapss", "ptrapsc", "ptrapas", "ptrapac",  //
     "ptrapws", "ptrapwc", "ptrapis", "ptrapic",  //
     "ptrapgs", "ptrapgc", "ptrapcs", "ptrapcc"},
    {"psbs", "psbc", "psls", "pslc", "psss", "pssc", "psas", "psac",  //
     "psws", "pswc", "psis", "psic", "psgs", "psgc", "pscs", "pscc"},
    {"pbbs", "pbbc", "pbls", "pblc", "pbss", "pbsc", "pbas", "pbac",  //
     "pbws", "pbwc", "pbis", "pbic", "pbgs", "pbgc", "pbcs", "pbcc"},

    {"ploadw", "ploadr"},

    "pvalid",
    "pmove",
    "pmovefd",
    "pflusha",
    "pflushs",
    "pflushr",
    "psave",
    "prestore",  //

    // ファンクションコール(マクロ)
    "DOS",
    "IOCS",
    "FPACK",
    "SXCALL"  //

};

#define OFFS(member) (uint16_t) GET_OPECODE_OFFSET(member)

typedef struct {
  uint16_t to;
  uint16_t from;
} ToFrom;

static void copyMnemonics(OpStringTable* ops, size_t len, const ToFrom* toFrom);

static void dbraToDbf(OpStringTable* ops) {
  static const ToFrom list[] = {
      {OFFS(dbcc[COND_F]), OFFS(dbf)},
      {OFFS(fdbcc[FPCOND_F]), OFFS(fdbf)},
  };

  copyMnemonics(ops, _countof(list), list);
}

static void abbreviateMnemonic(OpStringTable* ops) {
  static const ToFrom list[] = {
      {OFFS(adda), OFFS(add)},       //
      {OFFS(addiAsAdd), OFFS(add)},  //
      {OFFS(andiAsAnd), OFFS(and)},  //
      {OFFS(cmpiAsCmp), OFFS(cmp)},  //
      {OFFS(cmpm), OFFS(cmp)},       //
      {OFFS(movea), OFFS(move)},     //
      {OFFS(oriAsOr), OFFS(or)},     //
      {OFFS(suba), OFFS(sub)},       //
      {OFFS(subiAsSub), OFFS(sub)},
  };

  copyMnemonics(ops, _countof(list), list);
}

static void copyMnemonics(OpStringTable* ops, size_t len,
                          const ToFrom* toFrom) {
  size_t i;

  for (i = 0; i < len; ++i) {
    char* to = (char*)ops + toFrom[i].to;
    char* from = (char*)ops + toFrom[i].from;
    strcpy(to, from);
  }
}

static void removeDefaultSize(OpStringTable* ops) {
  const size_t tail = strlen(".w\t");
  static const uint16_t list[] = {
      OFFS(dc[WORDSIZE]),   //
      OFFS(dcb[WORDSIZE]),  //
      OFFS(ds[WORDSIZE]),   //
  };
  size_t i;

  for (i = 0; i < _countof(list); ++i) {
    char* p = (char*)ops + list[i];
    p += strlen(p) - tail;
    *p++ = '\t';
    *p++ = '\0';
    *p++ = '\0';
  }
}

void initOpString(void) {
  OpStringTable* ops = &OpString;

  if (Dis.dbraToDbf) dbraToDbf(ops);
  if (Dis.mnemonicAbbreviation) abbreviateMnemonic(ops);
  if (Dis.N) removeDefaultSize(ops);

  Dis.actions->modifyMnemonic(ops);

  if (Dis.U) toUpperMemory(sizeof(OpStringTable), ops);
}

// EOF
