  lea (@f,pc),a0
  rts

@@:
  .dc.b 1f-$
  .dc.b 2f-$
  .dc.b 0
  .even

1: rts
2: rts

