;=============================================
;  Filename obj/mmu.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $00000132 byte(s)
;  Data size    $00000000 byte(s)
;  Bss  size    $00000000 byte(s)
;  10 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/mmu.x dised/mmu.s
;=============================================

	.include	doscall.mac
	.include	iocscall.mac
	.include	fefunc.mac

	.cpu	68000

	.text

L000000:
	.cpu	68040
	pflush	(a0)
	pflushn	(sp)
	pflusha
	pflushan
	nop
	ptestr	(a0)
	ptestw	(sp)
	nop
	.cpu	68060
	plpar	(a0)
	plpaw	(sp)
	nop
	rts

L000018:
	.cpu	68030
	pmovefd	(a0),srp
	ploadr	sfc,(a0)
	ploadr	dfc,(a0)
	ploadr	d0,(a0)
	ploadr	d7,(a0)
	ploadr	#0,(a0)
	ploadr	#7,(a0)
	.cpu	68020
	ploadr	#8,(a0)
	ploadr	#15,(a0)
	nop
	rts

L000040:
	.cpu	68030
	pmovefd	(a0),srp
	ploadw	sfc,(a0)
	ploadw	dfc,(a0)
	ploadw	d0,(a0)
	ploadw	d7,(a0)
	ploadw	#0,(a0)
	ploadw	#7,(a0)
	.cpu	68020
	ploadw	#8,(a0)
	ploadw	#15,(a0)
	nop
	rts

L000068:
	.cpu	68030
	pmovefd	(a0),srp
	pflush	sfc,#0
	pflush	dfc,#7,(a0)
	pflush	#1,#0
	pflush	#6,#7,(a0)
	.cpu	68020
	pflush	sfc,#8
	pflush	dfc,#15,(a0)
	pflush	#8,#0
	pflush	#15,#7,(a0)
	nop
	rts

L000090:
	.cpu	68030
	pmovefd	(a0),srp
	.cpu	68020
	pflushs	sfc,#0
	pflushs	dfc,#7,(a0)
	pflushs	#1,#0
	pflushs	#6,#7,(a0)
	pflushs	sfc,#8
	pflushs	dfc,#15,(a0)
	pflushs	#8,#0
	pflushs	#15,#7,(a0)
	nop
	rts

L0000b8:
	.cpu	68030
	pmovefd	(a0),srp
	ptestr	#0,(a0),#7
	ptestr	#7,(sp),#0
	ptestr	#0,(a0),#7,sp
	ptestw	#0,(a0),#7
	ptestw	#7,(sp),#0
	ptestw	#0,(a0),#7,sp
	nop
	rts

L0000d8:
	pmovefd	(a0),srp
	pmove	mmusr,(a0)
	.cpu	68020
	pmove	pcsr,(a0)
	pmove	bad0,(a0)
	pmove	bad7,(a0)
	pmove	bac0,(a0)
	pmove	bac7,(a0)
	nop
	rts

L0000f8:
	.cpu	68030
	pmove	tt0,(a0)
	pmove	tt1,(a0)
	pmove	tc,(a0)
	.cpu	68020
	pmove	drp,(a0)
	pmove	srp,(a0)
	pmove	crp,(a0)
	pmove	cal,(a0)
	pmove	val,(a0)
	pmove	scc,(a0)
	pmove	ac,(a0)
	nop
	rts

L000124:
	pmove	#!12345678_9abcdef0,crp
	rts

L000132:

	.end	L000000
