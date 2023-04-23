;dis -m68020,68851 -R0  -->  forced disassembly (binary mismatch when reassembled)
;                            (exclude NG instruction)
;dis -m68020,68851 -R1  -->  undefined instruction

.cpu 68020
  .dc $f010,$8720  ;ok - ptestr sfc,(a0),#1,a1
  rts
  .dc $f010,$8220  ;NG - ptestr sfc,(a0),#0 [,a1] ... always undefined inst.
  rts

  .dc $f010,$8720  ;ok - ptestr sfc,(a0),#1,a1
  rts
  .dc $f010,$8620  ;ng - ptestr sfc,(a0),#1 [,a1]
  rts

  .dc $f000,$2400  ;ok - pflusha
  rts
  .dc $f011,$2400  ;ng - pflusha [(a1)]
  rts

  .dc $f011,$3800  ;ok - pflush sfc,#0,(a1)
  rts
  .dc $f011,$3000  ;ng - pflush sfc,#0 [,(a1)]
  rts
