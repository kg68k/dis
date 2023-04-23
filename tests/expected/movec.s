;=============================================
;  Filename obj/movec.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $0000007a byte(s)
;  Data size    $00000000 byte(s)
;  Bss  size    $00000000 byte(s)
;  7 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/movec.x dised/movec.s
;=============================================

	.include	doscall.mac
	.include	iocscall.mac
	.include	fefunc.mac

	.cpu	68000

	.text

L000000:
	.cpu	68010
	movec	d0,sfc
	movec	d0,dfc
	.cpu	68020
	movec	d0,cacr
	.cpu	68040
	movec	d0,tc
	movec	d0,itt0
	movec	d0,itt1
	movec	d0,dtt0
	movec	d0,dtt1
	.cpu	68060
	movec	d0,buscr
	nop
	movec	d0,usp
	movec	d0,vbr
	.cpu	68020
	movec	d0,caar
	movec	d0,msp
	movec	d0,isp
	.cpu	68040
	movec	d0,mmusr
	movec	d0,urp
	movec	d0,srp
	.cpu	68060
	movec	d0,pcr
	nop
	move	d0,ccr
	move	ccr,d0
	move	d0,sr
	move	sr,d0
	nop
	move	a0,usp
	move	usp,a0
	nop
	DOS	_EXIT

L00005e:
	nop
	rts

L000062:
	.dc.b	$4e,$7b,$0f,$ff,$4e,$75
L000068:
	.dc.b	$4e,$7b,$00,$09,$4e,$75
L00006e:
	.dc.b	$4e,$7b,$07,$ff,$4e,$75
L000074:
	.dc.b	$4e,$7b,$08,$09,$4e,$75
L00007a:

	.end	L000000
