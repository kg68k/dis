;=============================================
;  Filename obj/lab_oorange.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $0000000a byte(s)
;  Data size    $00000000 byte(s)
;  Bss  size    $00000000 byte(s)
;  2 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/lab_oorange.x dised/lab_oorange.s
;=============================================

	.include	doscall.mac
	.include	iocscall.mac
	.include	fefunc.mac

	.cpu	68000

	.text

L000000:
	lea	(L000000-$100,pc),a0
	lea	(L00000a+$100,pc),a1
	DOS	_EXIT

L00000a:

	.end	L000000
