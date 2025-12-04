// ソースコードジェネレータ
// 自動解析モジュール
// Copyright (C) 1989,1990 K.Abe, 1994 R.ShimiZu
// All rights reserved.
// Copyright (C) 2025 TcbnErik

#include <stdio.h>
#include <string.h>

#ifdef __HUMAN68K__
#include <sys/xstart.h>
#endif

#include "analyze.h"
#include "disasm.h"
#include "estruct.h"
#include "etc.h"  // charout, peek[wl]
#include "global.h"
#include "label.h"   // regist_label etc.
#include "offset.h"  // depend_address , nearadrs
#include "osk.h"

typedef enum {
  FLOW_NO_TRACKING,
  FLOW_MOVE_PCIDX,
  FLOW_LEA_PCDISP,
  FLOW_MOVE_ARIDX,
} CodeFlowPhase;

typedef struct {
  uint8_t phase;  // CodeFlowPhase
  uint8_t baseReg;
  uint8_t ixReg;
  uint8_t reserved;
  address target;
} CodeFlow;

// static 関数プロトタイプ
static void printReason(const char* message, address adrs);
static boolean analyzeJump(DisParam* disp, CodeFlow* flow, address start,
                           analyze_mode mode, address limit);
static int analyzeRelativeTable(address table, opesize size, boolean isProgram);
static void regist_data(disasm*, operand*);
static void unregist_data(disasm*, operand*);

void setReasonVerbose(uint8_t lv) {
  Dis.reasonVerbose = (Dis.backtrackReason >= lv) ? TRUE : FALSE;
}

/*
  right_opecode() 下請け

  アドレスに依存可能なオペランドが依存していれば
  neardepend を次の依存アドレスに進める。
  依存不可能か、依存していなければそのまま。
*/
static address skip_depend_ea(address neardepend, disasm* code, operand* op) {
  switch (op->ea) {
    case AbLong:
      if (neardepend == op->eaadrs) return nearadrs(neardepend + 1);
      break;

    case IMMED:
      if (neardepend == op->eaadrs &&
          (code->size == LONGSIZE || code->size == SINGLESIZE))
        return nearadrs(neardepend + 1);
      break;

    case PCIDXB:
      if (op->flags & OPFLAG_PC_RELATIVE) {
        break;  // (bd.l,pc)のアドレス依存は認めない (bd.l,zpc)なら認める
      }
      // FALLTHRU
    case AregIDXB:
      if (neardepend == op->eaadrs && op->exbd == 4)
        return nearadrs(neardepend + 1);
      break;

    case AregPOSTIDX:
    case AregPREIDX:
    case PCPOSTIDX:
    case PCPREIDX:
      if ((op->flags & OPFLAG_PC_RELATIVE) == 0) {
        // ([bd.l,pc])のアドレス依存は認めない ([bd.l,zpc])なら認める
        if (neardepend == op->eaadrs && op->exbd == 4)
          neardepend = nearadrs(neardepend + 1);
      }
      if (neardepend == op->eaadrs2 && op->exod == 4)
        return nearadrs(neardepend + 1);
      break;

    default:
      break;
  }
  return neardepend;
}

/*

  right opecode ?

*/
static boolean right_opecode(address neardepend, disasm* code, address pc) {
  if (pc <= neardepend) return TRUE;

  neardepend = skip_depend_ea(neardepend, code, &code->op1);
  neardepend = skip_depend_ea(neardepend, code, &code->op2);
  neardepend = skip_depend_ea(neardepend, code, &code->op3);
  neardepend = skip_depend_ea(neardepend, code, &code->op4);

  return (pc <= neardepend);
}

/*

  既に PROLABEL として登録済みかどうか調べる

*/
static boolean prolabel(address adrs) {
  lblbuf* ptr = search_label(adrs);

  return (ptr && isPROLABEL(ptr->mode)) ? TRUE : FALSE;
}

/*

  既に DATLABEL として登録済みかどうか調べる

*/
static boolean datlabel(address adrs) {
  lblbuf* ptr = search_label(adrs);

  return (ptr && isDATLABEL(ptr->mode)) ? TRUE : FALSE;
}

// 指定したアドレスより後のデータラベルのアドレスを返す
//   そのようなラベルが無ければ Dis.availableTextEnd を返す
static address limitadrs(address adrs) {
  lblbuf* lptr = next(adrs + 1);

  return MIN(lptr->label, Dis.availableTextEnd);
}

static void printUndefinedInstruction(address pc) {
  if (Dis.reasonVerbose) {
    codeptr ptr = Dis.Ofst + pc;
    eprintf("\n" PRI_ADRS ": 未定義命令(" PRI_OPCODE " " PRI_OPCODE ")。", pc,
            peekw(ptr), peekw(ptr + 2));
  }
}

// プログラム領域の終端をラベル登録する
static void foundProgramEnd(address adrs) {
  if (search_label(adrs) == NULL) {
    regist_label(adrs, DATLABEL | (lblmode)UNKNOWN);
  }
}

// インデックスレジスタが Dn.W*1 か?
static int isIxDnWordScale1(operand* op) {
  return (op->ixReg <= 7) && (op->ixSizeScale == 0);
}

// インデックスレジスタが Dn.w*1 または Dn.w*2 か?
static int isIxDnWordScale12(operand* op) {
  return (op->ixReg <= 7) && (op->ixSizeScale <= 1);
}

// インデックスレジスタが Dn.[wl]*1 または Dn.[wl]*2 か?
static int isIxDnScale12(operand* op) {
  return (op->ixReg <= 7) && ((op->ixSizeScale & 3) <= 1);
}

static boolean updateCodeFlowMovePCIDX(disasm* code, CodeFlow* flow) {
  if (code->size != WORDSIZE || code->op2.ea != DregD) return FALSE;
  if (!isIxDnScale12(&code->op1)) return FALSE;

  // MOVE (d8,PC,Dm.w*s),Dn
  flow->phase = FLOW_MOVE_PCIDX;
  flow->target = code->op1.opval;
  flow->ixReg = code->op2.baseReg;
  return TRUE;
}

static boolean updateCodeFlowMoveARIDX(disasm* code, CodeFlow* flow) {
  if (flow->phase != FLOW_LEA_PCDISP) return FALSE;

  if (code->size != WORDSIZE || code->op2.ea != DregD) return FALSE;
  if (!isIxDnScale12(&code->op1)) return FALSE;
  if (flow->baseReg != code->op1.baseReg) return FALSE;

  // MOVE (d8,An,Dm.w*s),Dn
  flow->phase = FLOW_MOVE_ARIDX;
  flow->ixReg = code->op2.baseReg;
  return TRUE;
}

static boolean updateCodeFlowMove(disasm* code, CodeFlow* flow) {
  switch (code->op1.ea) {
    default:
      return FALSE;

    case PCIDX:
      return updateCodeFlowMovePCIDX(code, flow);
    case AregIDX:
      return updateCodeFlowMoveARIDX(code, flow);
  }
}

static boolean updateCodeFlowLea(disasm* code, CodeFlow* flow) {
  if (code->op1.ea != PCDISP) return FALSE;

  // LEA (d16,PC),An
  flow->phase = FLOW_LEA_PCDISP;
  flow->target = code->op1.opval;
  flow->baseReg = code->op2.baseReg;
  return TRUE;
}

// 相対オフセットテーブル判別用の情報を更新する
static void updateCodeFlow(disasm* code, CodeFlow* flow) {
  switch (code->opecodeOffset) {
    default:
      break;

    case GET_OPECODE_OFFSET(move):
      if (updateCodeFlowMove(code, flow)) return;
      break;
    case GET_OPECODE_OFFSET(lea):
      if (updateCodeFlowLea(code, flow)) return;
      break;
  }

  flow->phase = FLOW_NO_TRACKING;
}

static void analyzeOtherLea(disasm* code, CodeFlow* flow) {
  if (code->op1.ea != PCIDX) return;

  // 直前の命令が MOVE (table,PC,Dm.w*s),Dn で、
  if (flow->phase != FLOW_MOVE_PCIDX) return;

  // 今の命令が LEA (table,PC,Dn.w),An なら、
  if (flow->target != code->op1.opval) return;
  if (flow->ixReg != code->op1.ixReg || !isIxDnWordScale1(&code->op1)) return;

  // 相対遷移表を発見
  registerReltblOrder(&Dis.reltblArray, flow->target, RELTABLE, FALSE,
                      DATLABEL | FORCE);
}

static void analyzeOtherPeaPCIDX(disasm* code, CodeFlow* flow) {
  // 直前の命令が MOVE (talbe,PC,Dm.w*s),Dn で、
  if (flow->phase != FLOW_MOVE_PCIDX) return;

  // 今の命令が PEA (table,PC,Dn.w) なら、
  if (flow->target != code->op1.opval) return;
  if (flow->ixReg != code->op1.ixReg || !isIxDnWordScale1(&code->op1)) return;

  // 相対遷移表を発見
  registerReltblOrder(&Dis.reltblArray, flow->target, RELTABLE, FALSE,
                      DATLABEL | FORCE);
}

static void analyzeOtherPeaARIDX(disasm* code, CodeFlow* flow) {
  // 直前の命令が MOVE (An,Dm.w*s),Dn で、
  if (flow->phase != FLOW_MOVE_ARIDX) return;

  // 今の命令が PEA (An,Dn.w) なら、
  if (code->op1.opval != 0) return;
  if (flow->baseReg != code->op1.baseReg) return;
  if (flow->ixReg != code->op1.ixReg || !isIxDnWordScale1(&code->op1)) return;

  // 相対遷移表を発見
  registerReltblOrder(&Dis.reltblArray, flow->target, RELTABLE, FALSE,
                      DATLABEL | FORCE);
}

static void analyzeOtherPea(disasm* code, CodeFlow* flow) {
  switch (code->op1.ea) {
    default:
      break;

    case PCIDX:
      analyzeOtherPeaPCIDX(code, flow);
      break;

    case AregIDX:
      analyzeOtherPeaARIDX(code, flow);
      break;
  }
}

// その他の命令の解析: ADDA
static void analyzeOtherAdda(disasm* code, CodeFlow* flow) {
  // 直前の命令が LEA (table,PC),An で、
  if (flow->phase != FLOW_LEA_PCDISP) return;

  // 今の命令が ADDA (table,PC,Dn.w*s),An なら、
  if (code->size != WORDSIZE || code->op1.ea != PCIDX) return;
  if (flow->target != code->op1.opval || flow->baseReg != code->op2.baseReg)
    return;
  if (!isIxDnWordScale12(&code->op1)) return;

  // 相対遷移表を発見
  registerReltblOrder(&Dis.reltblArray, flow->target, RELTABLE, FALSE,
                      DATLABEL | FORCE);
}

// その他の命令の解析
static void analyzeOther(disasm* code, CodeFlow* flow) {
  switch (code->opecodeOffset) {
    default:
      break;

    case GET_OPECODE_OFFSET(lea):
      analyzeOtherLea(code, flow);
      break;
    case GET_OPECODE_OFFSET(pea):
      analyzeOtherPea(code, flow);
      break;
    case GET_OPECODE_OFFSET(adda):
      analyzeOtherAdda(code, flow);
      break;
  }
}

enum {
  OPCODE2_ORI_B_IMM_D0 = 0x0000,
};
enum {
  OPCODE4_ORI_B_IMM0_D0 = 0x00000000,
};

// 解析サブ
//   プログラム領域と認められる場合はTRUEを返す
//
// 成功条件
// ・すでにプログラム領域と記録されていた
// ・プログラム領域のラベルに到達した
// ・rtsなどのリターン命令
// ・-Gオプション指定時に、bsrなどのコール命令直後がデータ領域
static boolean analyzeInner(address start, analyze_mode mode) {
  address limit;
  address neardepend;
  DisParam disp;
  disasm* code = &disp.code;
  CodeFlow flow;
  int orib = 0;  // ori.b #??,d0 = $0000????
  void (*regoplbl)(disasm* code) = Dis.actions->registerOperandLabel;

  if (Dis.availableTextEnd <= start) {
    printReason("PCが有効なセクションを外れた。", start);
    return FALSE;
  }
  if (isOdd(start)) {
    printReason("PCが奇数。", start);
    return FALSE;
  }
  if (depend_address(start) || depend_address(start - 2)) {
    printReason("PCがアドレスに依存している。", start);
    return FALSE;
  }
  if (peekl(start + Dis.Ofst) == OPCODE4_ORI_B_IMM0_D0) {
    printReason("PCが ori.b #0,d0 から始まっている。", start);
    return FALSE;
  }

  if (!regist_label(start, PROLABEL)) {  // これは ch_lblmod では駄目
    return TRUE;
  }

// スタックチェック
#ifdef __HUMAN68K__
  {
    void* sp;
    __asm("move.l sp,%0" : "=g"(sp));

    if (sp < _SSTA) {
      eputc('\n');
      _stack_over();
    }
  }
#endif

  flow.phase = FLOW_NO_TRACKING;

  limit = limitadrs(start);
  neardepend = nearadrs(start);

  setDisParamPcPtr(&disp, start, Dis.Ofst);
  disp.pcEnd = Dis.availableTextEnd;

  while (1) {
    address previous_pc;

    if (neardepend == disp.pc) {  // アドレス依存チェック
      printReason("PCがアドレスに依存している。", disp.pc);
      not_program(start, MIN(disp.pc, limit));
      return FALSE;
    }

    if (peekw(disp.ptr) == OPCODE2_ORI_B_IMM_D0) {  // ori.b #??,d0
      if (orib >= 2 || peekw(disp.ptr + 2) == OPCODE2_ORI_B_IMM_D0) {
        const char* s = (orib >= 2) ? "ori.b #??,d0 が連続している。"
                                    : "ori.b #0,d0 を発見。";
        printReason(s, disp.pc);
        not_program(start, MIN(disp.pc, limit));
        return FALSE;
      }
      orib++;
    } else
      orib = 0;

    if (limit == disp.pc) {
      lblbuf* ptr = search_label(limit);
      if (ptr) {
        // 既に解析済みのプログラム領域とぶつかれば、この領域もプログラム領域
        if (isPROLABEL(ptr->mode)) return TRUE;

        // ラベルファイルで命令列アボートの指定があれば、ここまでをプログラム領域と認める
        if (isABORTLABEL(ptr->mode)) return TRUE;
      }
    }

    if ((disp.pc > limit) ||  // 最初は code.op.opval は初期化されないけど大丈夫
        (disp.pc <= code->op1.opval && code->op1.opval < limit) ||
        (disp.pc <= code->op2.opval && code->op2.opval < limit) ||
        (disp.pc <= code->op3.opval && code->op3.opval < limit) ||
        (disp.pc <= code->op4.opval && code->op4.opval < limit)) {
      limit = limitadrs(disp.pc);
    }

    previous_pc = disp.pc;

    dis(&disp);

    if (Dis.availableTextEnd < disp.pc) {
      printReason("PCが有効なセクションを外れた。", disp.pc);
      not_program(start, MIN(previous_pc, limit));
      return FALSE;
    }

    if (neardepend < disp.pc) {
      if (!right_opecode(neardepend, code, disp.pc)) {
        printReason("アドレス依存のオペランドが正当でない。", previous_pc);
        not_program(start, MIN(previous_pc, limit));
        return FALSE;
      }
      neardepend = nearadrs(disp.pc);
    }

    if (code->opeType == OTHER ||
        ((code->opeType == JMPOP || code->opeType == JSROP) &&
         AregPOSTIDX <= code->jmpea && code->jmpea != PCIDXB)) {
      regist_data(code, &code->op1);
      regist_data(code, &code->op2);
      regist_data(code, &code->op3);
      regist_data(code, &code->op4);
    }

    if (regoplbl != NULL) regoplbl(code);

    switch (code->opeType) {
      default:
        break;

      case OTHER:
        analyzeOther(code, &flow);
        break;

      case JMPOP:  // JMP, BRA
        if (!analyzeJump(&disp, &flow, start, mode, limit)) return FALSE;
        foundProgramEnd(disp.pc);
        return TRUE;

      case JSROP:  // JSR, BSR
        if (!analyzeJump(&disp, &flow, start, mode, limit)) return FALSE;

        if (Dis.argAfterCall && datlabel(disp.pc)) {
          // -G : サブルーチンコールの直後に引数を置くことを認める
          return TRUE;
        }
        break;

      case RTSOP:
        foundProgramEnd(disp.pc);
        return TRUE;

      case BCCOP:  // Bcc
        if (!analyzeJump(&disp, &flow, start, mode, limit)) return FALSE;
        break;

      case UNDEF:
        printUndefinedInstruction(previous_pc);
        not_program(start, MIN(previous_pc, limit));
        return FALSE;
    }
    updateCodeFlow(code, &flow);
  }
}

static void printReason(const char* s, address adrs) {
  if (Dis.reasonVerbose) eprintf("\n" PRI_ADRS ": %s", adrs, s);
}

// 解析する
//
// input:
//   start : analyze start address
//   mode  : ANALYZE_IGNOREFAULT
//             分岐先での未定義命令等を無視する
//           ANALYZE_NORMAL
//             分岐先で未定義命令等を見付けたら、呼び出し側はデータ領域
// return:
//   FALSE : startからはプログラム領域と認められなかった
boolean analyze(address start, analyze_mode mode) {
  boolean success;
  charout('>');

  success = analyzeInner(start, mode);

  if (!success) {
    charout('?');
    ch_lblmod(start, DATLABEL | FORCE | (lblmode)UNKNOWN);
    return FALSE;
  }
  charout('<');
  return TRUE;
}

// 奇数アドレスへの分岐
static boolean branchToOdd(DisParam* disp, address start, address limit) {
  disasm* code = &disp->code;

  if (Dis.reasonVerbose) {
    eprintf("\n" PRI_ADRS ": 奇数アドレス(" PRI_ADRS ")へ分岐。", code->pc,
            code->jmp);
  }

  if (Dis.acceptAddressError) {
    // -j: 奇数アドレスへの分岐を未定義命令と「しない」
    regist_label(code->jmp, DATLABEL | (lblmode)UNKNOWN);
    return TRUE;
  }

  // 奇数アドレスへの分岐があればプログラム領域ではない
  ch_lblmod(start, DATLABEL | FORCE | (lblmode)UNKNOWN);
  not_program(start, MIN(code->pc, limit));
  return FALSE;
}

// 分岐命令等の処理
static boolean branch_job(DisParam* disp, address start, analyze_mode mode,
                          address limit) {
  disasm* code = &disp->code;
  address opval = code->jmp;

  if (opval == (address)-1) return TRUE;
  if (isOdd(opval)) return branchToOdd(disp, start, limit);

  // 分岐先が既にプログラム領域と判明している
  if (prolabel(opval)) {
    regist_label(opval, PROLABEL);  // to increment label count
    return TRUE;
  }

  // 分岐先の領域を解析する
  if (!analyze(opval, mode) && mode != ANALYZE_IGNOREFAULT && !Dis.i) {
    // 分岐先がプログラム領域でなければ、呼び出し元も同じ
    ch_lblmod(start, DATLABEL | FORCE | (lblmode)UNKNOWN);
    not_program(start, MIN(disp->pc, limit));
    return FALSE;
  }

  /* 分岐先はプログラム領域だった */
  return TRUE;
}

static void analyzeJumpARIDX(disasm* code, CodeFlow* flow) {
  // 直前の命令が MOVE (An,Dm.w*s),Dn で、
  if (flow->phase != FLOW_MOVE_ARIDX) return;

  // 今の命令が JMP|JSR (An,Dn.w) なら、
  if (code->op1.opval != 0) return;
  if (code->op1.baseReg != flow->baseReg) return;
  if (code->op1.ixReg != flow->ixReg || !isIxDnWordScale1(&code->op1)) return;

  // 相対遷移表(ジャンプテーブル)を発見
  registerReltblOrder(&Dis.reltblArray, flow->target, RELTABLE, TRUE,
                      DATLABEL | FORCE);
}

// 相対遷移表(ジャンプテーブル)を発見した場合はTRUEを返す
static boolean analyzeJumpPCIDX(disasm* code, CodeFlow* flow) {
  // 直前の命令が MOVE (table,PC,Dm.w*s),Dn で、
  if (flow->phase != FLOW_MOVE_PCIDX) return FALSE;

  // 今の命令が JMP|JSR (table,PC,Dn.w) なら、
  if (code->jmp != flow->target) return FALSE;
  if (code->op1.ixReg != flow->ixReg || !isIxDnWordScale1(&code->op1))
    return FALSE;

  // 相対遷移表(ジャンプテーブル)を発見
  registerReltblOrder(&Dis.reltblArray, code->jmp, RELTABLE, TRUE,
                      DATLABEL | FORCE);
  return TRUE;
}

// 分岐命令(JMP, JSR, BRA, Bcc)の解析
//   プログラム領域と認められる場合はTRUEを返す
static boolean analyzeJump(DisParam* disp, CodeFlow* flow, address start,
                           analyze_mode mode, address limit) {
  disasm* code = &disp->code;

  switch (code->jmpea) {
    default:
      // 分岐先が得られないなら問題なしとみなす
      break;

    case AbLong:  // jmp (label)
      if (!INPROG(code->jmp, code->op1.eaadrs)) break;
      // FALLTHRU
    case PCDISP:  // jmp (label,pc) や bsr label
      return branch_job(disp, start, mode, limit);

    case AregIDX:  // jmp (a0,d0.w) など
      analyzeJumpARIDX(code, flow);
      break;

    case PCIDX:  // jmp (label,pc,d0.w) など
      if (analyzeJumpPCIDX(code, flow)) break;

      // テーブルでなければ、不明サイズのデータとしてラベル登録する
      // 実際の分岐先は実行時のインデックスレジスタの値次第で、ラベルの場所が
      // プログラムではない可能性もあるので、解析はせずラベル登録だけに留める。
      // (README.txtの「参照されないラベル」と同様のケース)
      regist_label(code->jmp, DATLABEL | (lblmode)UNKNOWN);
      break;
  }

  return TRUE;
}

// 相対オフセットテーブルの値として正しいかどうかを返す
static boolean isValidOffset(address table, address label, address ptr,
                             int bytes, boolean isProgram) {
  if (table == label) {
    // オフセット値0の場合
    switch (Dis.reltblZero) {
      default:
      case RELTBL_ZERO_REJECT:
        return FALSE;
      case RELTBL_ZERO_HEAD:
        return table == ptr;  // テーブル先頭の項目のみ許可
      case RELTBL_ZERO_ALL:
        return TRUE;
    }
  }

  if (isProgram) {
    // 相対ジャンプテーブルなら宛先は偶数のみ
    if (isOdd(label)) return FALSE;

    if (label < Dis.beginTEXT || Dis.availableTextEnd < label) return FALSE;
  } else {
    if (label < Dis.beginTEXT || Dis.LAST < label) return FALSE;
  }

  // テーブル内を指すようならオフセット値ではない
  if (table <= label && label < (ptr + bytes)) return FALSE;

  return TRUE;
}

#ifdef DEBUG_RELTBL_ADDRESS
static void printReltblAddress(address table, address ptr, ULONG offs,
                               address label) {
  if (label != DEBUG_RELTBL_ADDRESS) return;

  eprintf("\n%s:%d: table=" PRI_ADRS ", ptr=" PRI_ADRS ", offs=" PRI_ULHEX
          ", label=" PRI_ADRS ". ",
          __FILE__, __LINE__, table, ptr, offs, label);
}
#endif

// 相対オフセットテーブルを解析し、登録した宛先アドレス数を返す
//   テーブル終了位置の検出
//   宛先アドレスのラベル登録
//
// BUG: 相対オフセットテーブルと判断した場合、すぐにテーブル中のエントリを
//      登録してしまうので、あとから not_program といっても残ってしまう。
//      (滅多にあることではないが)
//
static int analyzeRelativeTable(address table, opesize size,
                                boolean isProgram) {
  uint8_t isLong = (size == RELLONGTABLE);
  ULONG bytes = isLong ? 4 : 2;
  address tableend = next(table + 1)->label;
  address ptr = table;
  int count;

  for (count = 0; ptr < tableend; ++count) {
    ULONG offs =
        isLong ? peekl(ptr + Dis.Ofst) : (ULONG)extl(peekw(ptr + Dis.Ofst));
    address label = table + offs;

    if (!isValidOffset(table, label, ptr, bytes, isProgram)) {
      // おかしなアドレスを指していればテーブルの項目ではない
      // (別のデータ形式かもしれないし、プログラム領域かもしれない)
      // ただしアドレス値だけで判定しているので確実とは言えない
      if (table == ptr)
        regist_label(ptr, DATLABEL | (lblmode)UNKNOWN | FORCE);
      else
        regist_label(ptr, DATLABEL | (lblmode)UNKNOWN);
      break;
    }

#ifdef DEBUG_RELTBL_ADDRESS
    printReltblAddress(table, ptr, offs, label);
#endif
    if (table != label) {
      regist_label(label, DATLABEL | (lblmode)UNKNOWN);
    }
    ptr += bytes;

    // テーブル末尾が以下のような構造の場合に末尾を検出できるように、
    // 毎回「この位置以降で一番近いラベル」を再取得する
    // .dc foo-tabletop
    // foo: rts
    tableend = next(ptr)->label;
  }

  return count;
}

// 解析する予定の相対オフセットテーブルの情報
typedef struct {
  address table;
  uint8_t size;  // opesize: RELTABLE, RELLONGTABLE
  uint8_t isProgram;
} ReltblOrder;

// 相対オフセットテーブルの記録バッファを初期化する
void initReltblArray(ArrayBuffer* rtbuf) {
  initArrayBuffer(rtbuf, sizeof(ReltblOrder));
}

// 指定したアドレスの相対オフセットテーブルのサイズを返す
//   テーブルではない場合は 0 を返す
static opesize getReltblOpesize(address table) {
  lblbuf* ptr = search_label(table);
  if (ptr) {
    opesize size = lblbufOpesize(ptr);
    if (size == RELTABLE || size == RELLONGTABLE) return size;
  }
  return (opesize)0;
}

// 記録した相対オフセットテーブルを全て解析し、登録した宛先アドレス数を返す
//
// addReltblAddress()を呼び出さないこと。
int analyzeRegisteredReltbl(ArrayBuffer* rtbuf) {
  int registered = 0;
  size_t count;
  ReltblOrder* order = getArrayBufferRawPointer(rtbuf, &count);

  if (order != NULL) {
    size_t len = count;
    size_t i;
    for (i = 0; i < len; ++order, ++i) {
      opesize size = getReltblOpesize(order->table);

      if (size == 0) {
        eprintf("\n" PRI_ADRS
                ": 相対オフセットテーブル以外に変更されています。",
                order->table);
        continue;
      }

      if (size != order->size) {
        eprintf("\n" PRI_ADRS
                ": 相対オフセットテーブルのサイズが変更されています。",
                order->table);
      }
      registered += analyzeRelativeTable(order->table, size, order->isProgram);
    }
  }

  freeArrayBuffer(rtbuf);
  return registered;
}

// 相対オフセットテーブルの値を持つアドレスなら真を返す
static int hasReltblData(address table) {
  return (Dis.beginTEXT <= table && table < Dis.beginBSS);
}

// 相対オフセットテーブルとして登録
//   order->table: テーブルのアドレス
//   order->size: RELTABLE, RELLONGTABLE
//   order->isProgram: TRUEなら宛先はプログラム
//   mode: 0以外ならラベル登録も行う(通常は DATLABEL を指定する)
//
// いますぐ解析するとテーブル以降のデータやプログラムをテーブルの一部として
// 誤認する可能性があるので、ここではテーブルアドレスの記録だけを行う。
// テーブルの解析は他の解析が終わって判明した限りのラベルが登録されてからとなる。
void registerReltblOrder(ArrayBuffer* rtbuf, address table, opesize size,
                         boolean isProgram, lblmode mode) {
  if (mode) regist_label(table, mode | (lblmode)size);

  if (hasReltblData(table)) {
    ReltblOrder* p = getArrayBufferNewPlace(rtbuf);
    *p = (ReltblOrder){table, size, isProgram};
  }
}

/*

  table から ロングワードテーブルとして登録

*/
extern void z_table(address table) {
  address ptr = table;
  address tableend = next(table + 1)->label;

  while (ptr + 4 <= tableend) {
    address label = (address)peekl(ptr + Dis.Ofst);

    regist_label(label, DATLABEL | (lblmode)UNKNOWN);
    ptr += 4;
    tableend = next(ptr)->label;
  }
}

/*

  from から to までの領域をプログラムで無かった事にする

*/
extern void not_program(address from, address to) {
  DisParam disp;
  disasm* code = &disp.code;

  ch_lblmod(from, DATLABEL | FORCE | (lblmode)UNKNOWN);

  setDisParamPcPtr(&disp, from, Dis.Ofst);
  disp.pcEnd = to;

  while (disp.pc < to) {
    dis(&disp);

    unregist_data(code, &code->op1);
    unregist_data(code, &code->op2);
    unregist_data(code, &code->op3);
    unregist_data(code, &code->op4);
  }
}

/*

  データとして登録する

*/
static void regist_data(disasm* code, operand* op) {
  switch (op->ea) {
    default:
      break;

    case IMMED:
      if (code->size2 == LONGSIZE && INPROG(op->opval, op->eaadrs))
        regist_label(op->opval, DATLABEL | (lblmode)UNKNOWN);
      break;

    case AbLong:
      if (INPROG(op->opval, op->eaadrs))
        regist_label(op->opval, DATLABEL | (lblmode)code->size2);
      break;

    case PCDISP:
    case PCIDX:
      regist_label(op->opval, DATLABEL | (lblmode)code->size2);
      break;

    case AregIDXB:
      if (op->exbd == 4 && INPROG(op->opval, op->eaadrs))
        regist_label(op->opval, DATLABEL | (lblmode)code->size2);
      break;

    case AregPOSTIDX:
    case AregPREIDX:
      if (op->exbd == 4 && INPROG(op->opval, op->eaadrs))
        regist_label(op->opval, DATLABEL | (lblmode)LONGSIZE);
      if (op->exod == 4 && INPROG(op->opval2, op->eaadrs2))
        regist_label(op->opval2, DATLABEL | (lblmode)code->size2);
      break;

    case PCIDXB:
      if ((op->flags & OPFLAG_PC_RELATIVE) ||
          (op->exbd == 4 && INPROG(op->opval, op->eaadrs)))
        regist_label(op->opval, DATLABEL | (lblmode)code->size2);
      break;

    case PCPOSTIDX:
    case PCPREIDX:
      if ((op->flags & OPFLAG_PC_RELATIVE) ||
          (op->exbd == 4 && INPROG(op->opval, op->eaadrs)))
        regist_label(op->opval, DATLABEL | (lblmode)LONGSIZE);
      if (op->exod == 4 && INPROG(op->opval2, op->eaadrs2))
        regist_label(op->opval2, DATLABEL | (lblmode)code->size2);
      break;
  }
}

/*

  データラベルとしての登録を取り消す

*/
static void unregist_data(disasm* code, operand* op) {
  switch (op->ea) {
    default:
      break;

    case IMMED:
      if (code->size2 != LONGSIZE) break;
      /* FALLTHRU */
    case AbLong:
      if (INPROG(op->opval, op->eaadrs)) unregist_label(op->opval);
      break;

    case PCDISP:
    case PCIDX:
      unregist_label(op->opval);
      break;

    case AregIDXB:
      if (op->exbd == 4 && INPROG(op->opval, op->eaadrs))
        unregist_label(op->opval);
      break;

    case AregPOSTIDX:
    case AregPREIDX:
      if (op->exbd == 4 && INPROG(op->opval, op->eaadrs))
        unregist_label(op->opval);
      if (op->exod == 4 && INPROG(op->opval2, op->eaadrs2))
        unregist_label(op->opval2);
      break;

    case PCIDXB:
      if ((op->flags & OPFLAG_PC_RELATIVE) ||
          (op->exbd == 4 && INPROG(op->opval, op->eaadrs)))
        unregist_label(op->opval);
      break;

    case PCPOSTIDX:
    case PCPREIDX:
      if ((op->flags & OPFLAG_PC_RELATIVE) ||
          (op->exbd == 4 && INPROG(op->opval, op->eaadrs)))
        unregist_label(op->opval);
      if (op->exod == 4 && INPROG(op->opval2, op->eaadrs2))
        unregist_label(op->opval2);
      break;
  }
}

/*

  登録済みラベルのモードを変える

*/
extern boolean ch_lblmod(address pc, lblmode mode) {
  lblbuf* ptr = search_label(pc);

  if (regist_label(pc, mode)) {
    if (ptr) unregist_label(pc);
    return TRUE;
  }

  return FALSE;
}

// EOF
