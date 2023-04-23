;=============================================
;  Filename obj/bra_size.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $00010004 byte(s)
;  Data size    $00000000 byte(s)
;  Bss  size    $00000000 byte(s)
;  25 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/bra_size.x dised/bra_size.s
;=============================================

	.include	doscall.mac
	.include	iocscall.mac
	.include	fefunc.mac

	.cpu	68000

	.text

L000000:
	.cpu	68020
	move.l	(L00002a,pc,d0.w*4),d0
	move.l	(L007f9c,pc,d0.w*4),d0
	move.l	(L008030,pc,d0.w*4),d0
	move.l	(L008076,pc,d0.w*4),d0
	nop
	bra	L007fd4
L000020:
	nop
	rts

L000024:
	nop
	rts

L000028:
	rts

L00002a:
	.ds.l	8152

L007f8a:
	rts

L007f8c:
	nop
	rts

L007f90:
	nop
	rts

L007f94:
	nop
	rts

L007f98:
	nop
	rts

L007f9c:
	.dc.l	$00000000,$00000000
	.dc.l	$00000000,$00000000
	.dc.l	$00000000,$00000000
	.dc.l	$00000000,$00000000
	.dc.l	$00000000,$00000000
	.dc.l	$00000000,$00000000
	.dc.l	$00000000,$00000000

L007fd4:
	bsr	L008054
	bsr.w	L008056
	bsr.l	L00805a
	bsr.w	L008062
	bsr.l	L008066
	bsr	L00806e
	bsr.l	L008072
	nop
	bsr	L00fff6
	bsr.l	L00fffa
	bsr.l	L010002
	nop
	bsr	L007f8a
	bsr.w	L007f8c
	bsr.l	L007f90
	bsr	L007f94
	bsr.l	L007f98
	bsr	L000020
	bsr.l	L000024
	bsr	L000028
	DOS	_EXIT

L008030:
	.dc.l	$00000000,$00000000
	.dc.l	$00000000,$00000000
	.dc.l	$00000000,$00000000
	.dc.l	$00000000,$00000000
	.dc.l	$00000000

L008054:
	rts

L008056:
	nop
	rts

L00805a:
	nop
	nop
	nop
	rts

L008062:
	nop
	rts

L008066:
	nop
	nop
	nop
	rts

L00806e:
	nop
	rts

L008072:
	nop
	rts

L008076:
	.ds.l	8160

L00fff6:
	nop
	rts

L00fffa:
	nop
	nop
	nop
	rts

L010002:
	rts

L010004:

	.end	L000000
