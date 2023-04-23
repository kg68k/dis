;dis -B

.cpu 68020
  nop
  rte

  nop
  rtd #4

  nop
  rts

  nop
  rtr

  nop
  jmp (a0)
@@:
  nop
  bra @b
@@
  nop
  fbra @b

  nop
  .dc $ff00
