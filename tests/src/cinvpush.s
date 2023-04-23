.cpu 68040
  cinvl ic,(a0)
  cinvl dc,(a1)
  cinvl bc,(a6)
  cinvl nc,(a7)
  nop
  rts

  cinvp ic,(a0)
  cinvp dc,(a1)
  cinvp bc,(a6)
  cinvp nc,(a7)
  nop
  rts

  cinva ic
  cinva dc
  cinva bc
  cinva nc
  nop
  rts

  cpushl ic,(a0)
  cpushl dc,(a1)
  cpushl bc,(a6)
  cpushl nc,(a7)
  nop
  rts

  cpushp ic,(a0)
  cpushp dc,(a1)
  cpushp bc,(a6)
  cpushp nc,(a7)
  nop
  rts

  cpusha ic
  cpusha dc
  cpusha bc
  cpusha nc
  nop
  rts

;dis -R0  -->  forced disassembly (binary mismatch when reassembled)
;dis -R1  -->  undefined instruction

  cinva bc   ;(a0) $f4d8
  .dc $f4d9  ;(a1)
  .dc $f4da  ;(a2)
  .dc $f4db  ;(a3)
  .dc $f4dc  ;(a4)
  .dc $f4dd  ;(a5)
  .dc $f4de  ;(a6)
  .dc $f4df  ;(a7)
  nop
  rts

  cpusha bc  ;(a0) $f4f8
  .dc $f4f9  ;(a1)
  .dc $f4fa  ;(a2)
  .dc $f4fb  ;(a3)
  .dc $f4fc  ;(a4)
  .dc $f4fd  ;(a5)
  .dc $f4fe  ;(a6)
  .dc $f4ff  ;(a7)
  nop
  rts
