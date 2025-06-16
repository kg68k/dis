// ソースコードジェネレータ
// ラベルチェック
// Copyright (C) 1989,1990 K.Abe
// All rights reserved.
// Copyright (C) 2025 TcbnErik

#include <stdio.h>

#include "analyze.h"
#include "disasm.h"
#include "estruct.h"
#include "etc.h" /* charout */
#include "global.h"
#include "label.h"  /* regist_label etc. */
#include "offset.h" /* depend_address , nearadrs */

static int Label_on_instruction_count = 0;
static int Fixed_count = 0;
static int Undefined_instruction_count = 0;

static void oops_undefined(address pc0, address previous_pc, address nlabel) {
  charout('+');
  eprintf("\n警告: 未定義命令です(" PRI_ADRS "-" PRI_ADRS ", " PRI_ADRS ")。",
          pc0, nlabel, previous_pc);

  not_program(pc0, nlabel);

  Undefined_instruction_count++;
}

static void printLabelInOpToData(address pc, address nlabel) {
  if (!Dis.I) return;

  eprintf("\n" PRI_ADRS
          ": ラベルが命令の中を指すためデータ領域に変更します(" PRI_ADRS
          "-" PRI_ADRS ")。",
          nlabel, pc, nlabel);
}

static void printLabelInOpShift(address pc, address nlabel) {
  if (!Dis.I) return;

  eprintf("\n" PRI_ADRS ": 命令の中を指すラベルです(" PRI_ADRS "+$%" PRIx32
          ")。",
          nlabel, pc, (ULONG)(nlabel - pc));
}

static address labelshift(address previous_pc, address pc, address end,
                          lblbuf* nadrs) {
  address nlabel = nadrs->label;

  while (nlabel < pc) {
    charout('!');
    nadrs->shift = nlabel - previous_pc;
    regist_label(previous_pc, PROLABEL);
    nadrs = Next(nadrs);
    nlabel = nadrs->label;
  }

  if (nlabel >= end) /* 泥縄 (^_^;) */
    return next_datlabel(previous_pc)->label;

  return end;
}

/*

  既に DATLABEL として登録済みかどうか調べる

*/
static boolean datlabel(address adrs) {
  lblbuf* ptr = search_label(adrs);

  return (ptr && isDATLABEL(ptr->mode)) ? TRUE : FALSE;
}

/*

  from から end まで逆アセンブルして命令の途中にアクセスしているところを捜す

*/
static address search_change_operand(address from, address end) {
  address previous_pc = from;
  DisParam disp;
  disasm* code = &disp.code;
  boolean was_prog = FALSE;  // 直前の領域がプログラムであったかどうか

  setDisParamPcPtr(&disp, from, Dis.Ofst);
  disp.pcEnd = Dis.availableTextEnd;

  while (end != (address)-1 && disp.pc < end) {
    lblbuf* nadrs = next(disp.pc + 1);
    address nlabel = nadrs->label;
    address pc0 = disp.pc;

    was_prog = TRUE;

    /* ２つのプログラムラベル間を逆アセンブル */
    while (disp.pc < nlabel && disp.pc < end) {
      previous_pc = disp.pc;
      dis(&disp);

      if (code->opeType == UNDEF) { /* 未定義命令なら警告 */
        oops_undefined(pc0, previous_pc, nlabel);
        setDisParamPcPtr(&disp, nlabel, Dis.Ofst);
      }
    }

    /* 命令の途中にアクセスしているなら */
    if (disp.pc != nlabel) {
      Label_on_instruction_count++;
      if (Dis.k ||
          (!Dis.E && code->size == BYTESIZE && code->op1.eaadrs + 1 != nlabel &&
           code->op2.eaadrs + 1 != nlabel && code->op3.eaadrs + 1 != nlabel &&
           code->op4.eaadrs + 1 != nlabel)) {
        charout('*');
        Fixed_count++;
        not_program(pc0, nlabel);
        printLabelInOpToData(pc0, nlabel);
        setDisParamPcPtr(&disp, nlabel, Dis.Ofst);
        was_prog = FALSE;
      } else {
        end = labelshift(previous_pc, disp.pc, end, nadrs);
        printLabelInOpShift(previous_pc, nlabel);
      }
    }
  }
  if (code->opeType != RTSOP && code->opeType != JMPOP && datlabel(disp.pc) &&
      was_prog) {
    charout('-');
    regist_label(disp.pc, PROLABEL);
  }

  return end;
}

/*

  from から end までのデータ領域をチェック

*/
static void search_change_data(address from, address end) {
  address pc, pc0;
  lblbuf* nadrs;

  pc = pc0 = from;
  nadrs = next(pc);

  while (end != (address)-1 && pc < end) {
    address dependadrs = nearadrs(pc);
    address nlabel;

    nadrs = Next(nadrs);
    nlabel = nadrs->label;

    while (dependadrs != (address)-1 && dependadrs + 4 <= nlabel)
      dependadrs = nearadrs(dependadrs + 1);

    while (dependadrs < nlabel && nlabel < dependadrs + 4) {
      charout('!');
      nadrs->shift = nlabel - dependadrs;
      regist_label(dependadrs, DATLABEL | (lblmode)UNKNOWN);
      nadrs = Next(nadrs);
      nlabel = nadrs->label;
    }
    pc = nlabel;
  }
}

extern void search_operand_label(void) {
  lblbuf* nadrs = next(Dis.beginTEXT);
  address pc, pcend = nadrs->label;

  while (nadrs->label < Dis.availableTextEnd) {
    charout('.');
    nadrs = next_prolabel(pcend);

    pc = nadrs->label;
    nadrs = next_datlabel(pc);
    pcend = nadrs->label;

    if (pcend != (address)-1) pcend = search_change_operand(pc, pcend);
  }

  if (Label_on_instruction_count) {
    eprintf("\n命令の中を指すラベル: %d個。", Label_on_instruction_count);
    if (Fixed_count)
      eprintf("\n%d個の領域をデータ領域に変更しました。", Fixed_count);
  }

  nadrs = next(Dis.beginTEXT);
  pcend = nadrs->label;

  while (nadrs->label < Dis.beginBSS) {
    charout(':');
    nadrs = next_datlabel(pcend);
    pc = nadrs->label;
    nadrs = next_prolabel(pc);
    pcend = MIN(nadrs->label, Dis.beginBSS);

    if (pc < Dis.beginBSS) search_change_data(pc, pcend);
  }
  if (Undefined_instruction_count) {
    // ラベルチェックに引っ掛かるとデータ消失 ?
    analyze_data();
  }
}

// EOF
