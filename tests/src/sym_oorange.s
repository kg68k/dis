;dis -s2

XXX:: .equ 0
AAA:: .equ aaa-$100
BBB:: .equ bbb-$100
YYY:: .equ $12345678
CCC:: .equ ccc+$100
DDD:: .equ ddd+$100
ZZZ:: .equ $ffffffff

aaa::
  nop
bbb::
  lea (AAA,pc),a0
  lea (BBB,pc),a0
  lea (CCC,pc),a0
  lea (DDD,pc),a0
ccc::
  .dc $ff00
ddd::

