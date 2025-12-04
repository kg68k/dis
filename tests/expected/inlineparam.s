;=============================================
;  Filename obj/inlineparam.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $00000018 byte(s)
;  Data size    $00000000 byte(s)
;  Bss  size    $00000000 byte(s)
;  4 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/inlineparam.x dised/inlineparam.s -gsrc/inlineparam.lab
;=============================================

	.include	doscall.mac
	.include	iocscall.mac
	.include	fefunc.mac

	.cpu	68000

	.text

L000000:
	bsr	L000012

L000002:
	.dc.b	'Hello, world.',$0d,$0a,$00

L000012:
	DOS	_PRINT
	addq.l	#4,sp
	DOS	_EXIT

L000018:

	.end	L000000
