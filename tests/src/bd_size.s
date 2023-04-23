.cpu 68020
  tst (dummy1,pc)
  nop

  tst ([0.w,pc])  ;Lxxxxxx.w
  tst ([0.l,pc])  ;Lxxxxxx.l
  nop

  tst (126.b,pc,d0.l)  ;Lxxxxxx
  tst (126.w,pc,d0.l)  ;Lxxxxxx.w
  tst (126.l,pc,d0.l)  ;Lxxxxxx.l

  tst (128.w,pc,d0.l)  ;Lxxxxxx.w
  tst (128.l,pc,d0.l)  ;Lxxxxxx.l

  tst (32766.w,pc,d0.l)  ;Lxxxxxx
  tst (32768.l,pc,d0.l)  ;Lxxxxxx.l
  tst (32770.l,pc,d0.l)  ;Ideally Lxxxxxx, but an assembly error occurs
  nop

  tst ([126.w,pc])  ;Lxxxxxx
  tst ([126.l,pc])  ;Lxxxxxx.l

  tst ([32766.l,pc])  ;Lxxxxxx.l
  tst ([32768.l,pc])  ;Lxxxxxx.l
  tst ([32770.l,pc])  ;Lxxxxxx.l
  nop
  bra negative


  .ds.b 124
dummy1:
  .ds.b 32768-124


negative:
  tst (-128.b,pc,d0.l)  ;Lxxxxxx
  tst (-128.w,pc,d0.l)  ;Lxxxxxx.w
  tst (-128.l,pc,d0.l)  ;Lxxxxxx.l

  tst (-130.w,pc,d0.l)  ;Lxxxxxx
  tst (-130.l,pc,d0.l)  ;Lxxxxxx.l

  tst (-32766.w,pc,d0.l)  ;Lxxxxxx
  tst (-32768.l,pc,d0.l)  ;Lxxxxxx.l
  tst (-32770.l,pc,d0.l)  ;Lxxxxxx
  nop

  tst ([-128.w,pc])  ;Lxxxxxx.w
  tst ([-128.l,pc])  ;Lxxxxxx.l

  tst ([-32766.l,pc])  ;Lxxxxxx.l
  tst ([-32768.l,pc])  ;Lxxxxxx.l
  tst ([-32770.l,pc])  ;Lxxxxxx
  nop

  .dc $ff00
