.cpu 68020
  fsincos fp0,fp1:fp0
  fsincos fp7,fp0:fp7
  fsincos (a0),fp0:fp1
  nop
  .dc $ff00
