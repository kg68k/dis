;dis -m68040 -R0  -->  forced disassembly (binary mismatch when reassembled)
;dis -m68040 -R1  -->  undefined instruction

.cpu 68040

  pflusha
  rts

  .dc $f519  ;ng  <reg> != 0
  rts

  .dc $f51c  ;ng  <reg> != 0
  rts

  pflushan
  rts

  .dc $f511  ;ng  <reg> != 0
  rts

  .dc $f514  ;ng  <reg> != 0
  rts

.end
