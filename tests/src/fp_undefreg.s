;dis -m68020,68851 -R0  -->  forced disassembly (binary mismatch when reassembled)
;                            (exclude NG instruction)
;dis -m68020,68851 -R1  -->  undefined instruction

.cpu 68020
  ftst fp0
  rts

  .dc $f204,$003a    ;NG <ea> != 0
  rts

  .dc $f200,$013a    ;ng <dst> != 0
  rts

  .dc $f200,$0f3a    ;ng <dst> != <src>
  rts


  fmove fp0,fp0
  rts

  .dc $f201,$0000  ;NG <ea> != 0
  rts


  fmovecr #0,fp0
  rts

  .dc $f201,$5c00  ;NG <ea> != 0
  rts

  .dc $f208,$5c00  ;NG <ea> != 0
  rts

  .dc $f23f,$5c00  ;NG <ea> != 0
  rts


  fmove.p fp0,(a0){d7}
  rts

  .dc $f210,$7c7f  ;NG <k-factor unused bits> != 0
  rts


  fmove.x fp0,(a0)
  rts

  .dc $f210,$6840  ;NG <k-factor> != 0
  rts

  .dc $f210,$6801  ;NG <k-factor> != 0
  rts
