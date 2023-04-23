  lea (a,pc),a0
  nop
  .dc $ff00

a:
  .dc.b 'a',0
  .align 4,$0000

p:
  movem.l d0-d7/a0-a6,-(sp)
  nop
  movem.l (sp)+,d0-d7/a0-a6
  rts
