;movem dreg,(a0)
i:=1
.rept 255
  .dc $4890,i
  i:=i+1
.endm
.rept 16
  nop
.endm

;movem areg,(a0)
i:=1
.rept 255
  .dc $4890,i<<8
  i:=i+1
.endm
.rept 16
  nop
.endm

  movem.l d7/a0,-(a0)
  movem.l d7/a0,(a0)
  movem.l d0-d7/a0-a7,-(sp)
.rept 16
  nop
.endm

;fmovem fpreg,(a0)
i:=1
.rept 255
  .dc $f210,$f000+i
  i:=i+1
.endm
.rept 16
  nop
.endm

;fmovem fpreg,-(a0)
i:=1
.rept 255
  .dc $f220,$e000+i
  i:=i+1
.endm
.rept 16
  nop
.endm

;fmovem fpcr,(a0)
i:=1
.rept 7
  .dc $f210,$a000+(i<<10)
  i:=i+1
.endm
.rept 16
  nop
.endm

  .dc $ff00


