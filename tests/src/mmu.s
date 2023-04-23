MPU_68030: .macro
.cpu 68030
  pmovefd (a0),srp
.endm

.cpu 68040
  pflush (a0)
  pflushn (a7)
  pflusha
  pflushan
  nop
  ptestr (a0)
  ptestw (a7)
  nop

.cpu 68060
  plpar (a0)
  plpaw (a7)
  nop
  rts

  MPU_68030
.cpu 68020
  ploadr sfc,(a0)
  ploadr dfc,(a0)
  ploadr d0,(a0)
  ploadr d7,(a0)
  ploadr #0,(a0)
  ploadr #7,(a0)
  ploadr #8,(a0)
  ploadr #15,(a0)
  nop
  rts

  MPU_68030
.cpu 68020
  ploadw sfc,(a0)
  ploadw dfc,(a0)
  ploadw d0,(a0)
  ploadw d7,(a0)
  ploadw #0,(a0)
  ploadw #7,(a0)
  ploadw #8,(a0)
  ploadw #15,(a0)
  nop
  rts

  MPU_68030
.cpu 68020
  pflush sfc,#0
  pflush dfc,#7,(a0)
  pflush #1,#0
  pflush #6,#7,(a0)
  pflush sfc,#8
  pflush dfc,#15,(a0)
  pflush #8,#0
  pflush #15,#7,(a0)
  nop
  rts

  MPU_68030
.cpu 68020
  pflushs sfc,#0
  pflushs dfc,#7,(a0)
  pflushs #1,#0
  pflushs #6,#7,(a0)
  pflushs sfc,#8
  pflushs dfc,#15,(a0)
  pflushs #8,#0
  pflushs #15,#7,(a0)
  nop
  rts

  MPU_68030
.cpu 68020
  ptestr #0,(a0),#7
  ptestr #7,(a7),#0
  ptestr #0,(a0),#7,sp
; ptestr #7,(a7),#0,a0  ;F-line exception
  ptestw #0,(a0),#7
  ptestw #7,(a7),#0
  ptestw #0,(a0),#7,sp
; ptestw #7,(a7),#0,a0  ;F-line exception
  nop
  rts

  MPU_68030
.cpu 68020
  pmove psr,(a0)
  pmove pcsr,(a0)
  pmove bad0,(a0)
  pmove bad7,(a0)
  pmove bac0,(a0)
  pmove bac7,(a0)
  nop
  rts

.cpu 68030
  pmove tt0,(a0)
  pmove tt1,(a0)
  pmove tc,(a0)
.cpu 68020
  pmove drp,(a0)
.cpu 68030
  pmove srp,(a0)
  pmove crp,(a0)
.cpu 68020
  pmove cal,(a0)
  pmove val,(a0)
  pmove scc,(a0)
  pmove ac,(a0)
  nop
  rts

.cpu 68020
  pmove #$12345678,#$9abcdef0,crp
  rts

