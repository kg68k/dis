  bsr @f
    .dc.b 'Hello, world.',13,10,0
    .even
  @@:
  .dc $ff09
  addq.l #4,sp
  .dc $ff00

