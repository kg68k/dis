  btst #0,d0
  btst #31,d7
  btst #0,(a0)
  btst #7,(a0)
  rts

  ori #0,d0
  ori #$8000,d1
  ori #$ffff,d7
  rts

  stop #0
  stop #$ffff
  rts

.cpu 68020
  rtd #0
  rtd #$ffff
  rts

  trapcc #0
  trapcc #$ffff
  trapcc.l #0
  trapcc.l #$ffffffff
  rts

  pack d0,d7,#0
  pack d7,d0,#$ffff
  unpk d0,d7,#0
  unpk d7,d0,#$ffff
  rts

.cpu 68060
  lpstop #0
  lpstop #$ffff
  rts

.cpu 68020
  ptrapcc #0
  ptrapcc #$ffff
  ptrapcc.l #0
  ptrapcc.l #$ffffffff
  rts

  ftrapeq #0
  ftrapeq #$ffff
  ftrapeq.l #0
  ftrapeq.l #$ffffffff
  rts

  link a6,#0
  link a6,#$7fff
  link a6,#-1
  link a6,#-$8000
  link.l a6,#0
  link.l a6,#$7fffffff
  link.l a6,#-1
  link.l a6,#-$80000000
  rts

.cpu 68020
  callm #0,(a0)
  rtm d0
  callm #$ff,(a0)
  rtm d7

  trap #0
  moveq #0,d0
  trap #15
  rts

  bkpt #0
  bkpt #7
  rts

  moveq #0,d0
  moveq #$7f,d1
  moveq #$80,d6
  moveq #$ff,d7
  rts

  fmovecr #0,fp0
  fmovecr #$7f,fp7
  rts

  ptestr sfc,(a0),#0
  ptestr sfc,(a0),#7
  rts

  addq #1,d0
  addq #8,d7
  subq #1,d0
  subq #8,d7
  rts

  lsl #1,d0
  lsl #8,d0
  rts
