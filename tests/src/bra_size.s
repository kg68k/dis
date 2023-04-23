.cpu 68020
  move.l (dummy1,pc,d0.w*4),d0
  move.l (dummy2,pc,d0.w*4),d0
  move.l (dummy3,pc,d0.w*4),d0
  move.l (dummy4,pc,d0.w*4),d0
  nop
  bra mid

bsr_w_$2m32768:
  nop
  rts

bsr_l_$2m32768:
  nop
  rts

bsr_l_$2m32770:
  rts

dummy1:
  .ds.b 32608

bsr_s_$2m128:
  rts

bsr_w_$2m128:
  nop
  rts

bsr_l_$2m128:
  nop
  rts

bsr_w_$2m130:
  nop
  rts

bsr_l_$2m130:
  nop
  rts

dummy2:
  .ds.b 56

mid:
  bsr.s ($+2)+126  ;bsr
  bsr.w ($+2)+126  ;bsr.w
  bsr.l ($+2)+126  ;bsr.l

  bsr.w ($+2)+128  ;bsr.w
  bsr.l ($+2)+128  ;bsr.l

  bsr.w ($+2)+130  ;bsr
  bsr.l ($+2)+130  ;bsr.l
  nop

  bsr.w ($+2)+32766  ;bsr
  bsr.l ($+2)+32766  ;bsr.l

  bsr.l ($+2)+32768  ;bsr.l
  nop

  bsr.s ($+2)-128  ;bsr
  bsr.w ($+2)-128  ;bsr.w
  bsr.l ($+2)-128  ;bsr.l

  bsr.w ($+2)-130  ;bsr
  bsr.l ($+2)-130  ;bsr.l

  bsr.w ($+2)-32768  ;bsr
  bsr.l ($+2)-32768  ;bsr.l

  bsr.l ($+2)-32770  ;bsr

  .dc $ff00

dummy3:
  .ds.b 36

bsr_s_$2p126:
  rts

bsr_w_$2p126:
  nop
  rts

bsr_l_$2p126:
  nop
  nop
  nop
  rts

bsr_w_$2p128:
  nop
  rts

bsr_l_$2p128:
  nop
  nop
  nop
  rts

bsr_w_$2p130:
  nop
  rts

bsr_l_$2p130:
  nop
  rts

dummy4:
  .ds.b 32640

bsr_w_$2p32766:
  nop
  rts

bsr_l_$2p32766:
  nop
  nop
  nop
  rts

bsr_l_$2p32768:
  rts
