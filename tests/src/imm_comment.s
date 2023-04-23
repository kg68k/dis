;dis -M

  moveq.l #'a',d0

  move.b #'a',d0
  move.w #'ab',d0
  move.w #'x',d0
  move.l #'abcd',d0
  move.l #'xyz',d0
  move.l #'xyz'<<8,d0
  move.l #'xy'<<8,d0

  cmp.b #'a',d0
  cmp.w #'ab',d0
  cmp.w #'x',d0
  cmp.l #'abcd',d0
  cmp.l #'xyz',d0
  cmp.l #'xyz'<<8,d0
  cmp.l #'xy'<<8,d0

  cmpi.b #'a',d0
  cmpi.w #'ab',d0
  cmpi.w #'x',d0
  cmpi.l #'abcd',d0
  cmpi.l #'xyz',d0
  cmpi.l #'xyz'<<8,d0
  cmpi.l #'xy'<<8,d0

  cmpa.w #'ab',a0
  cmpa.w #'x',a0
  cmpa.l #'abcd',a0
  cmpa.l #'xyz',a0
  cmpa.l #'xyz'<<8,a0
  cmpa.l #'xy'<<8,a0

  add.b #'a',d0
  add.w #'ab',d0    ;no comment
  add.l #'abcd',d0  ;no comment

  sub.b #'a',d0
  sub.w #'ab',d0    ;no comment
  sub.l #'abcd',d0  ;no comment

.cpu 68020
  pack d0,d0,#'ab'
  unpk d0,d0,#'ab'

 .dc $ff00
