  btst #0,d0
  btst d0,(a0)
  btst #$ff,d0
  btst #-1,d0
  rts

  bclr #0,d0
  bclr d0,(a0)
  bclr #$ff,d0
  bclr #-1,d0
  rts

  bset #0,d0
  bset d0,(a0)
  bset #$ff,d0
  bset #-1,d0
  rts

  bchg #0,d0
  bchg d0,(a0)
  bchg #$ff,d0
  bchg #-1,d0
  rts

;btst #0,d0 with illegal undef byte
;dis -f  ->  forced disassembly (binary mismatch when reassembled)
  .dc $0800,$0100
  .dc $0800,$80ff
  rts
