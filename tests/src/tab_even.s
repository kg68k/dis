  lea (table,pc),a0
  .dc $ff00

table:
  .dc.b 1,'a'
  .even  ;no align

  .dc.b 2,'ab'
  .even

dummy:
  .dc.b 2,'xy'
  .dc.b $ff

.end
