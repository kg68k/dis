;dis -m68060
;dis -mcpu32

.cpu 68060

  lpstop #$2000
  rts

.cpu 68000

;bgnd
  .dc $4afa
  rts

;tbls.b (a0),d7
  .dc $f810,$7900
  rts

;tblsn.b (a0),d7
  .dc $f810,$7d00
  rts

;tbls.b d0:d1,d7
  .dc $f800,$7801
  rts

;tblsn.b d0:d1,d7
  .dc $f800,$7c01
  rts

;tblu.b (a0),d7
  .dc $f810,$7100
  rts

;tblun.b (a0),d7
  .dc $f810,$7500
  rts

;tblu.b d0:d1,d7
  .dc $f800,$7001
  rts

;tblun.b d0:d1,d7
  .dc $f800,$7401
  rts

.end
