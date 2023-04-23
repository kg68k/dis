.cpu 68000
  cmpm (a0)+,(a0)+
  cmpm (a0)+,(a7)+
  cmpm (a7)+,(a7)+
  cmpm (a6)+,(a6)+
  nop

  addx -(a0),-(a0)
  addx -(a0),-(a7)
  addx -(a7),-(a0)
  addx -(a7),-(a7)
  addx d0,d0
  addx d0,d7
  addx d7,d0
  addx d7,d7
  nop

  abcd -(a0),-(a0)
  abcd -(a0),-(a7)
  abcd -(a7),-(a0)
  abcd -(a7),-(a7)
  abcd d0,d0
  abcd d0,d7
  abcd d7,d0
  abcd d7,d7
  nop

.cpu 68020
  pack d0,d0,#'0'
  pack d0,d7,#0
  unpk d7,d0,#-1
  unpk d3,d4,#$80
  nop
  
  pack -(a0),-(a0),#'0'
  pack -(a0),-(a7),#0
  unpk -(a6),-(a0),#-1
  unpk -(a3),-(a4),#$80
  nop

.cpu 68040
  move16 (a0)+,(0)
  move16 (0),(a0)+
  move16 (a0),(0)
  move16 (0),(a0)
  move16 (a0)+,(a0)+
  move16 (a7)+,(a7)+
  nop

  .dc $ff00  
