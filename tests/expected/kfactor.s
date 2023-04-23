;=============================================
;  Filename obj/kfactor.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $00000020 byte(s)
;  Data size    $00000000 byte(s)
;  Bss  size    $00000000 byte(s)
;  2 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/kfactor.x dised/kfactor.s
;=============================================

	.include	doscall.mac
	.include	iocscall.mac
	.include	fefunc.mac

	.cpu	68000

	.text

L000000:
	.cpu	68020
	fmove.p	fp0,(a0){d0}
	fmove.p	fp7,(a0){d7}
	nop
	fmove.p	fp0,(a0){#-64}
	fmove.p	fp0,(a0){#-1}
	fmove.p	fp0,(a0){#0}
	fmove.p	fp0,(a0){#-1}
	fmove.p	fp0,(a0){#-63}
	DOS	_EXIT

L000020:

	.end	L000000
