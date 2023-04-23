;=============================================
;  Filename obj/split_end.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $00000400 byte(s)
;  Data size    $00000000 byte(s)
;  Bss  size    $00000000 byte(s)
;  3 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/split_end.x dised/split_end.s -S1
;=============================================

	.include	doscall.mac
	.include	iocscall.mac
	.include	fefunc.mac

	.cpu	68000

	.text

L000000:
	move.l	(L000008,pc),d0
	nop
	DOS	_EXIT

L000008:
	.ds.l	254
L000400:

	.end	L000000
