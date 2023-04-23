;=============================================
;  Filename obj/cpu32.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $0000003c byte(s)
;  Data size    $00000000 byte(s)
;  Bss  size    $00000000 byte(s)
;  11 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/cpu32.x dised/cpu32.s
;=============================================

	.include	doscall.mac
	.include	iocscall.mac
	.include	fefunc.mac

	.cpu	68000

	.text

L000000:
	.cpu	68060
	lpstop	#$2000
	rts

L000008:
	.dc.b	'JúNu'
L00000c:
	.dc.b	$f8,$10,$79,$00,$4e,$75
L000012:
	.dc.b	$f8,$10,$7d,$00,$4e,$75
L000018:
	.dc.b	$f8,$00,$78,$01,$4e,$75
L00001e:
	.dc.b	$f8,$00,$7c,$01,$4e,$75
L000024:
	.dc.b	$f8,$10,$71,$00,$4e,$75
L00002a:
	.dc.b	$f8,$10,$75,$00,$4e,$75
L000030:
	.dc.b	$f8,$00,$70,$01,$4e,$75
L000036:
	.dc.b	$f8,$00,$74,$01,$4e,$75
L00003c:

	.end	L000000
