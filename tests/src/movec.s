.cpu 68040
  movec d0,sfc
  movec d0,dfc
  movec d0,cacr
  movec d0,tc
  movec d0,itt0
  movec d0,itt1
  movec d0,dtt0
  movec d0,dtt1
.cpu 68060
  movec d0,buscr
  nop
  movec d0,usp
  movec d0,vbr
.cpu 68030
  movec d0,caar
  movec d0,msp
  movec d0,isp
.cpu 68040
  movec d0,mmusr
  movec d0,urp
  movec d0,srp
.cpu 68060
  movec d0,pcr
  nop

  move d0,ccr
  move ccr,d0
  move d0,sr
  move sr,d0
  nop

  move a0,usp
  move usp,a0
  nop


  .dc $ff00

;illegal code
  nop
  rts
  .dc $4e7b,$0fff
  rts
  .dc $4e7b,$0009
  rts
  .dc $4e7b,$07ff
  rts
  .dc $4e7b,$0809
  rts

