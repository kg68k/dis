;dis -e
;edit labelfile
;dis -g

  moveq #0,d0
  move (table,pc,d0.w),d0
  nop  ;avoid auto reltbl detection
  jsr (table,pc,d0.w)
  .dc $ff00

table:
  .dc 1f-table
  .dc 2f-table

test:
  ori.b #10,(a0)
  rts
1:
  rts
2:
  rts
