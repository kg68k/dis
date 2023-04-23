;dis -A

  move.l #0.l,d0
  move.l #127.l,d0
  move.l #128.l,d0
  move.l #-128.l,d0
  move.l #-129.l,d0
  nop

  addi #0.w,(a0)
  addi #1.w,(a0)
  addi #8.w,(a0)
  addi #9.w,(a0)
  nop

  subi #0.w,(a0)
  subi #1.w,(a0)
  subi #8.w,(a0)
  subi #9.w,(a0)
  nop

  add #0.w,d0
  add #1.w,d0
  add #8.w,d0
  add #9.w,d0
  nop

  sub #0.w,d0
  sub #1.w,d0
  sub #8.w,d0
  sub #9.w,d0
  nop

  adda #0.w,a0
  adda #1.w,a0
  adda #8.w,a0
  adda #9.w,a0
  nop

  suba #0.w,a0
  suba #1.w,a0
  suba #8.w,a0
  suba #9.w,a0
  nop

  .dc $ff00
