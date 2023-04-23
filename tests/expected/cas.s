;=============================================
;  Filename obj/cas.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $00000028 byte(s)
;  Data size    $00000000 byte(s)
;  Bss  size    $00000000 byte(s)
;  2 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/cas.x dised/cas.s
;=============================================

	.include	doscall.mac
	.include	iocscall.mac
	.include	fefunc.mac

	.cpu	68000

	.text

L000000:
	.cpu	68020
	cas	d0,d7,(a0)
	cas	d7,d0,($12345678)
	nop
	cas2	d0:d7,d6:d1,(d0):(d7)
	cas2	d0:d7,d6:d1,(a0):(sp)
	cas2	d0:d7,d6:d1,(d0):(sp)
	cas2	d0:d7,d6:d1,(sp):(d0)
	DOS	_EXIT

L000028:

	.end	L000000
