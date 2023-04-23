;=============================================
;  Filename obj/ea_misc.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $00000076 byte(s)
;  Data size    $00000000 byte(s)
;  Bss  size    $00000000 byte(s)
;  2 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/ea_misc.x dised/ea_misc.s
;=============================================

	.include	doscall.mac
	.include	iocscall.mac
	.include	fefunc.mac

	.cpu	68000

	.text

L000000:
	cmpm	(a0)+,(a0)+
	cmpm	(a0)+,(sp)+
	cmpm	(sp)+,(sp)+
	cmpm	(a6)+,(a6)+
	nop
	addx	-(a0),-(a0)
	addx	-(a0),-(sp)
	addx	-(sp),-(a0)
	addx	-(sp),-(sp)
	addx	d0,d0
	addx	d0,d7
	addx	d7,d0
	addx	d7,d7
	nop
	abcd	-(a0),-(a0)
	abcd	-(a0),-(sp)
	abcd	-(sp),-(a0)
	abcd	-(sp),-(sp)
	abcd	d0,d0
	abcd	d0,d7
	abcd	d7,d0
	abcd	d7,d7
	nop
	.cpu	68020
	pack	d0,d0,#$30
	pack	d0,d7,#0
	unpk	d7,d0,#$ffff
	unpk	d3,d4,#$80
	nop
	pack	-(a0),-(a0),#$30
	pack	-(a0),-(sp),#0
	unpk	-(a6),-(a0),#$ffff
	unpk	-(a3),-(a4),#$80
	nop
	.cpu	68040
	move16	(a0)+,(0)
	move16	(0),(a0)+
	move16	(a0),(0)
	move16	(0),(a0)
	move16	(a0)+,(a0)+
	move16	(sp)+,(sp)+
	nop
	DOS	_EXIT

L000076:

	.end	L000000
