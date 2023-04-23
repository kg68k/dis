;dis -m68020
;
; {start} in comment is an address-dependent label with relocate data.

.cpu 68020

start:

;ok
  move (start,zpc),d0
  rts

;ng ... move ({start}.l,pc),d0
  .dc $41fb,$0170
  .dc.l start
  rts

;ok
  lea ([start,zpc,d0.l]),a0
  rts

;ng ... lea ([{start}.l,pc,d0.l]),a0
  .dc $41fb,$0931
  .dc.l start
  rts

;ok
  lea ([start,zpc],d0.l),a0
  rts

;ng ... lea ([{start}.l,pc],d0.l),a0
  .dc $41fb,$0935
  .dc.l start
  rts

  .dc $ff00

