;=============================================
;  Filename obj/fsincos.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $00000010 byte(s)
;  Data size    $00000000 byte(s)
;  Bss  size    $00000000 byte(s)
;  2 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/fsincos.x dised/fsincos.s
;=============================================

	.include	doscall.mac
	.include	iocscall.mac
	.include	fefunc.mac

	.cpu	68000

	.text

L000000:
	.cpu	68040
	fsincos	fp0,fp1:fp0
	fsincos	fp7,fp0:fp7
	fsincos	(a0),fp0:fp1
	nop
	DOS	_EXIT

L000010:

	.end	L000000
