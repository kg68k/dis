;=============================================
;  Filename obj/relbdpc.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $0000003e byte(s)
;  Data size    $00000000 byte(s)
;  Bss  size    $00000000 byte(s)
;  8 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/relbdpc.x dised/relbdpc.s
;=============================================

	.include	doscall.mac
	.include	iocscall.mac
	.include	fefunc.mac

	.cpu	68000

	.text

L000000:
	.cpu	68020
	move	(L000000,zpc),d0
	rts

L00000a:
	.dc.b	$41,$fb,$01,$70
	.dc.l	L000000
	.dc.b	$4e,$75

L000014:
	lea	([L000000,zpc,d0.l]),a0
	rts

L00001e:
	.dc.b	$41,$fb,$09,$31
	.dc.l	L000000
	.dc.b	$4e,$75

L000028:
	lea	([L000000,zpc],d0.l),a0
	rts

L000032:
	.dc.b	$41,$fb,$09,$35
	.dc.l	L000000
	.dc.b	$4e,$75

L00003c:
	DOS	_EXIT

L00003e:

	.end	L000000
