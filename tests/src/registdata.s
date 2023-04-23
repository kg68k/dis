.cpu 68020
  move (2,zpc),d0
  move ([8,zpc]),d0
  nop

  move (foo,zpc),d0
  move ([bar,zpc]),d0
  nop

  .dc $ff00

foo: .dc $beef
bar: .dc.l $12345678

.end
