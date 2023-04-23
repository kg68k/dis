;=============================================
;  Filename obj/ds.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $00000040 byte(s)
;  Data size    $00000000 byte(s)
;  Bss  size    $00000066 byte(s)
;  10 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/ds.x dised/ds.s
;=============================================

	.include	doscall.mac
	.include	iocscall.mac
	.include	fefunc.mac

	.cpu	68000

	.text

L000000:
	move.b	(L000040,pc),d0
	move	(L000042,pc),d0
	move.l	(L000046,pc),d0
	.cpu	68020
	pmove	(L00004e),srp
	pmove	(L00004e),srp
	pmove	(L00004e),srp
	nop
	.cpu	68040
	fmove.s	(L00005e,pc),fp0
	fmove.d	(L000066,pc),fp0
	fmove	(L000076,pc),fp0
	fmove.p	(L00008e,pc),fp0
	DOS	_EXIT

	.bss

L000040:
	.ds.b	2
L000042:
	.ds	2
L000046:
	.ds.l	2
L00004e:
	.ds.d	2
L00005e:
	.ds.s	2
L000066:
	.ds.d	2
L000076:
	.ds.x	2
L00008e:
	.ds.p	2
L0000a6:

	.end	L000000
