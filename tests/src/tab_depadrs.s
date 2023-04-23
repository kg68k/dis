  lea (table,pc),a0
  .dc $ff00

table:
  .dc 1
  .dc 0
  .dc.l table

  .dc 2
  .dc 0
  .dc.l table

  clr (table)
  rts

  moveq #0,d0
  rts
