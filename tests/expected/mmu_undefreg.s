;=============================================
;  Filename obj/mmu_undefreg.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $00000030 byte(s)
;  Data size    $00000000 byte(s)
;  Bss  size    $00000000 byte(s)
;  9 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/mmu_undefreg.x dised/mmu_undefreg.s
;=============================================

	.include	doscall.mac
	.include	iocscall.mac
	.include	fefunc.mac

	.cpu	68000

	.text

L000000:
	.cpu	68020
	ptestr	sfc,(a0),#1,a1
	rts

L000006:
	.dc.b	$f0,$10,$82,$20,$4e,$75

L00000c:
	ptestr	sfc,(a0),#1,a1
	rts

L000012:
	.dc.b	$f0,$10,$86,$20,$4e,$75

L000018:
	pflusha
	rts

L00001e:
	.dc.b	$f0,$11,$24,$00,$4e,$75

L000024:
	pflush	sfc,#0,(a1)
	rts

L00002a:
	.dc.b	$f0,$11,$30,$00,$4e,$75
L000030:

	.end	L000000
