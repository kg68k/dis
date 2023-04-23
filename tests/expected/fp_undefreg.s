;=============================================
;  Filename obj/fp_undefreg.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $0000005a byte(s)
;  Data size    $00000000 byte(s)
;  Bss  size    $00000000 byte(s)
;  16 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/fp_undefreg.x dised/fp_undefreg.s
;=============================================

	.include	doscall.mac
	.include	iocscall.mac
	.include	fefunc.mac

	.cpu	68000

	.text

L000000:
	.cpu	68040
	ftst	fp0
	rts

L000006:
	.dc.b	$f2,$04,$00,$3a,$4e,$75
L00000c:
	.dc.b	$f2,$00,$01,$3a,$4e,$75
L000012:
	.dc.b	$f2,$00,$0f,$3a,$4e,$75

L000018:
	fmove	fp0,fp0
	rts

L00001e:
	.dc.b	$f2,$01,$00,$00,$4e,$75

L000024:
	fmovecr	#0,fp0
	rts

L00002a:
	.dc.b	$f2,$01,$5c,$00,$4e,$75
L000030:
	.dc.b	$f2,$08,$5c,$00,$4e,$75
L000036:
	.dc.b	$f2,$3f,$5c,$00,$4e,$75

L00003c:
	fmove.p	fp0,(a0){d7}
	rts

L000042:
	.dc.b	$f2,$10,$7c,$7f,$4e,$75

L000048:
	fmove	fp0,(a0)
	rts

L00004e:
	.dc.b	$f2,$10,$68,$40,$4e,$75
L000054:
	.dc.b	$f2,$10,$68,$01,$4e,$75
L00005a:

	.end	L000000
