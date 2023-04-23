;=============================================
;  Filename obj/blankline.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $00000024 byte(s)
;  Data size    $00000000 byte(s)
;  Bss  size    $00000000 byte(s)
;  9 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/blankline.x dised/blankline_B.s -B
;=============================================

	.include	doscall.mac
	.include	iocscall.mac
	.include	fefunc.mac

	.cpu	68000

	.text

L000000:
	nop
	rte

L000004:
	nop
	.cpu	68010
	rtd	#4

L00000a:
	nop
	rts

L00000e:
	nop
	rtr

L000012:
	nop
	jmp	(a0)

L000016:
	nop
	bra	L000016

L00001a:
	nop
	.cpu	68020
	fbra	L00001a

L000020:
	nop
	DOS	_EXIT

L000024:

	.end	L000000
