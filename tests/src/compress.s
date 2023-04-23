;dis -m680x0 -W2

.cpu 68020
  move.b (byte1,pc),d0
  move.b (byte2,pc),d0
  move.w (word1,pc),d0
  move.w (word2,pc),d0
  move.l (long1,pc),d0
  move.l (long2,pc),d0
  nop
  fmove.s (single1,pc),fp0
  fmove.s (single2,pc),fp0
  fmove.d (double1,pc),fp0
  fmove.d (double2,pc),fp0
  fmove.x (extend1,pc),fp0
  fmove.x (extend2,pc),fp0
  fmove.p (packed1,pc),fp0
  fmove.p (packed2,pc),fp0
  .dc $ff00


byte1: .dcb.b 4,$a5
byte2: .ds.b 4

word1: .dcb.w 4,$cafe
word2: .ds.w 4

long1: .dcb.l 4,$deadbeef
long2: .ds.l 4

single1: .dcb.s 4,1.5
single2: .ds.s 4

double1: .dcb.d 4,2.3456
double2: .ds.d 4

extend1: .dcb.x 4,3.4567
extend2: .ds.x 4

packed1: .dcb.p 4,4.5678
packed2: .ds.p 4

.end
