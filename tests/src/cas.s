.cpu 68020
  cas d0,d7,(a0)
  cas d7,d0,($12345678)
  nop

  cas2 d0:d7,d6:d1,(d0):(d7)
  cas2 d0:d7,d6:d1,(a0):(a7)
  cas2 d0:d7,d6:d1,(d0):(a7)
  cas2 d0:d7,d6:d1,(a7):(d0)
  .dc $ff00
