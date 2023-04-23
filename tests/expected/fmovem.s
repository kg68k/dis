;=============================================
;  Filename obj/fmovem.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $00000098 byte(s)
;  Data size    $00000000 byte(s)
;  Bss  size    $00000000 byte(s)
;  18 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/fmovem.x dised/fmovem.s
;=============================================

	.include	doscall.mac
	.include	iocscall.mac
	.include	fefunc.mac

	.cpu	68000

	.text

L000000:
	.cpu	68040
	fmove.b	#0,fp0
	fmove.b	#1,fp1
	fmove.b	#$14,fp2
	fmove.w	#$12c,fp3
	fmove.w	#$fa0,fp4
	fmove.l	#$c350,fp5
	fmove.l	#$927c0,fp6
	fmove.l	#$6acfc0,fp7
	rts

L000038:
	fmovem.x	fp0-fp3,(a0)
	rts

L00003e:
	.dc.b	$f2,$10,$e0,$f0,$4e,$75

L000044:
	fmovem.x	d0,(a0)
	rts

L00004a:
	.dc.b	$f2,$10,$e8,$00,$4e,$75

L000050:
	fmovem.x	fp0-fp3,-(a0)
	rts

L000056:
	.dc.b	$f2,$20,$f0,$0f,$4e,$75

L00005c:
	fmovem.x	d0,-(a0)
	rts

L000062:
	.dc.b	$f2,$20,$f8,$00,$4e,$75

L000068:
	fmovem.x	(a0),fp0-fp3
	rts

L00006e:
	.dc.b	$f2,$10,$c0,$f0,$4e,$75

L000074:
	fmovem.x	(a0),d0
	rts

L00007a:
	.dc.b	$f2,$10,$c8,$00,$4e,$75

L000080:
	fmovem.x	(a0),fp0-fp3
	rts

L000086:
	.dc.b	$f2,$10,$c0,$f0,$4e,$75

L00008c:
	fmovem.x	(a0),d0
	rts

L000092:
	.dc.b	$f2,$10,$c8,$00,$4e,$75
L000098:

	.end	L000000
