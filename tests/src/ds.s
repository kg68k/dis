  move.b (byte,pc),d0
  move (word,pc),d0
  move.l (long,pc),d0

.cpu 68020
  pmove (quad_),srp
  pmove.q (quad_),srp
  pmove.d (quad_),srp

  nop
  fmove.s (single,pc),fp0
  fmove.d (double,pc),fp0
  fmove.x (extend,pc),fp0
  fmove.p (packed,pc),fp0
  
  .dc $ff00

.bss
byte: .ds.b 2
.even
word: .ds 2
long: .ds.l 2
quad_: .ds.b 8*2
	
single: .ds.s 2
double: .ds.d 2
extend: .ds.x 2
packed: .ds.p 2
