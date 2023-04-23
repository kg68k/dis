start:
  lea (start-$100,pc),a0
  lea (last+$100,pc),a1
  .dc $ff00
last:
