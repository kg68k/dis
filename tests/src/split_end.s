;dis -S1

  move.l (@f,pc),d0
  nop
  .dc $ff00

@@:  .ds.b 1024-8

