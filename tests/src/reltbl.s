  .dc $ff00

RELTBL: .macro
@@:
  .dc 1f-@b
1:
  rts
.endm

MOVEw_JMPJSR::
  move (@f,pc,d0.w),d0
  jmp (@f,pc,d0.w)
  RELTBL

MOVEl_JMPJSR::
  move (@f,pc,d0.l),d0
  jmp (@f,pc,d0.w)
  RELTBL

;ng
  move (@f,pc,d0.w),d0
  jmp (@f,pc,d1.w)  ;d0 != d1
  RELTBL

;ng
  move (@b,pc,d0.w),d0
  jmp (@f,pc,d0.w)  ;@b != @f
  RELTBL

LEA_MOVEw_JMPJSR::
  lea (@f,pc),a0
  move (a0,d0.w),d0
  jmp (a0,d0.w)
  RELTBL

LEA_MOVEl_JMPJSR::
  lea (@f,pc),a0
  move (a0,d0.l),d0
  jmp (a0,d0.w)
  RELTBL

;ng
  lea (@f,pc),a0
  move (a0,d0.w),d0
  jmp (a0,d1.w)  ;d0 != d1
  RELTBL

;ng
  lea (@f,pc),a0
  move (a1,d0.w),d0  ;a0 != a1
  jmp (a0,d0.w)
  RELTBL

;ng
  lea (@f,pc),a0
  move (a0,d0.w),d0
  jmp (a1,d0.w)  ;a0 != a1
  RELTBL

LEA_MOVEw_PEA::
  lea (@f,pc),a0
  move (a0,d0.w),d0
  pea (a0,d0.w)
  rts
  RELTBL

LEA_MOVEl_PEA::
  lea (@f,pc),a0
  move (a0,d0.l),d0
  pea (a0,d0.w)
  rts
  RELTBL

;ng
  lea (@f,pc),a0
  move (a0,d0.w),d0
  pea (a0,d1.w)  ;d0 != d1
  rts
  RELTBL

;ng
  lea (@f,pc),a0
  move (a1,d0.w),d0  ;a0 != a1
  pea (a0,d0.w)
  rts
  RELTBL

;ng
  lea (@f,pc),a0
  move (a0,d0.w),d0
  pea (a1,d0.w)  ;a0 != a1
  rts
  RELTBL

LEA_ADDA::
  lea (@f,pc),a0
  adda (@f,pc,d0.w),a0
  rts
  RELTBL

;ng
  lea (@f,pc),a0
  adda (@f,pc,d0.w),a1  ;a0 != a1
  rts
  RELTBL

;ng
  lea (@b,pc),a0
  adda (@f,pc,d0.w),a0  ;@b != @f
  rts
  RELTBL

MOVEw_LEA::
  move (@f,pc,d0.w),d0
  lea (@f,pc,d0.w),a0
  rts
  RELTBL

MOVEl_LEA::
  move (@f,pc,d0.l),d0
  lea (@f,pc,d0.w),a0
  rts
  RELTBL

;ng
  move (@f,pc,d0.w),d0
  lea (@f,pc,d1.w),a0  ;d0 != d1
  rts
  RELTBL

;ng
  move (@b,pc,d0.w),d0
  lea (@f,pc,d0.w),a0  ;@b != @f
  rts
  RELTBL

MOVEw_PEA::
  move (@f,pc,d0.w),d0
  pea (@f,pc,d0.w)
  rts
  RELTBL

MOVEl_PEA::
  move (@f,pc,d0.l),d0
  pea (@f,pc,d0.w)
  rts
  RELTBL

;ng
  move (@f,pc,d0.w),d0
  pea (@f,pc,d1.w)  ;d0 != d1
  rts
  RELTBL

;ng
  move (@b,pc,d0.w),d0
  pea (@f,pc,d0.w)  ;@b != @f
  rts
  RELTBL

OFFSET_0::
  move (@f,pc,d0.w),d0
  jmp (@f,pc,d0.w)
@@:
  .dc 0
  .dc 1f-@b
  .dc 0
1:
  rts

IN_BSS::
;ng
  move (@f,pc,d0.w),d0
  jmp (@f,pc,d0.w)
.bss
@@:
  .ds 1

