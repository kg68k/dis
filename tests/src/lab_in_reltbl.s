  move (@f,pc,d0.w),d0
  jsr (@f,pc,d0.w)
  .dc $ff00

@@:
  .dc 1f-@b
  .dc 2f-@b
  .dc 3f-@b
  .dc.b 0
xxx::
  .dc.b 4f-@b

  .even
1: rts
2: rts
3: rts
4: lea (xxx,pc),a0
   rts
