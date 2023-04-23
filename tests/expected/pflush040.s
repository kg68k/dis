;=============================================
;  Filename obj/pflush040.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $00000018 byte(s)
;  Data size    $00000000 byte(s)
;  Bss  size    $00000000 byte(s)
;  7 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/pflush040.x dised/pflush040.s
;=============================================

	.include	doscall.mac
	.include	iocscall.mac
	.include	fefunc.mac

	.cpu	68000

	.text

L000000:
	.cpu	68040
	pflusha
	rts

L000004:
	.dc.b	$f5,$19,$4e,$75
L000008:
	.dc.b	$f5,$1c,$4e,$75

L00000c:
	pflushan
	rts

L000010:
	.dc.b	$f5,$11,$4e,$75
L000014:
	.dc.b	$f5,$14,$4e,$75
L000018:

	.end	L000000
