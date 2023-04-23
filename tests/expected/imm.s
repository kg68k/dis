;=============================================
;  Filename obj/imm.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $00000118 byte(s)
;  Data size    $00000000 byte(s)
;  Bss  size    $00000000 byte(s)
;  20 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/imm.x dised/imm.s
;=============================================

	.include	doscall.mac
	.include	iocscall.mac
	.include	fefunc.mac

	.cpu	68000

	.text

L000000:
	btst	#0,d0
	btst	#$1f,d7
	btst	#0,(a0)
	btst	#7,(a0)
	rts

L000012:
	ori	#0,d0
	ori	#$8000,d1
	ori	#$ffff,d7
	rts

L000020:
	stop	#0
	stop	#$ffff
	rts

L00002a:
	.cpu	68010
	rtd	#0

L00002e:
	rtd	#$ffff

L000032:
	rts

L000034:
	.cpu	68020
	trapcc	#0
	trapcc	#$ffff
	trapcc.l	#0
	trapcc.l	#$ffffffff
	rts

L00004a:
	pack	d0,d7,#0
	pack	d7,d0,#$ffff
	unpk	d0,d7,#0
	unpk	d7,d0,#$ffff
	rts

L00005c:
	.cpu	68060
	lpstop	#0
	lpstop	#$ffff
	rts

L00006a:
	.cpu	68020
	ptrapcc	#0
	ptrapcc	#$ffff
	ptrapcc.l	#0
	ptrapcc.l	#$ffffffff
	rts

L000088:
	ftrapeq	#0
	ftrapeq	#$ffff
	ftrapeq.l	#0
	ftrapeq.l	#$ffffffff
	rts

L0000a6:
	link	a6,#0
	link	a6,#$7fff
	link	a6,#-1
	link	a6,#-$8000
	link.l	a6,#0
	link.l	a6,#$7fffffff
	link.l	a6,#-1
	link.l	a6,#-$80000000
	rts

L0000d0:
	callm	#0,(a0)
	rtm	d0
	callm	#$ff,(a0)
	rtm	d7
	trap	#0
	IOCS	_B_KEYINP
	rts

L0000e4:
	bkpt	#0
	bkpt	#7
	rts

L0000ea:
	moveq	#0,d0
	moveq	#$7f,d1
	moveq	#$80,d6
	moveq	#$ff,d7
	rts

L0000f4:
	fmovecr	#0,fp0
	fmovecr	#$7f,fp7
	rts

L0000fe:
	ptestr	sfc,(a0),#0
	ptestr	sfc,(a0),#7
	rts

L000108:
	addq	#1,d0
	addq	#8,d7
	subq	#1,d0
	subq	#8,d7
	rts

L000112:
	lsl	#1,d0
	lsl	#8,d0
	rts

L000118:

	.end	L000000
