;=============================================
;  Filename obj/bitop.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $0000004a byte(s)
;  Data size    $00000000 byte(s)
;  Bss  size    $00000000 byte(s)
;  6 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/bitop.x dised/bitop.s
;=============================================

	.include	doscall.mac
	.include	iocscall.mac
	.include	fefunc.mac

	.cpu	68000

	.text

L000000:
	btst	#0,d0
	btst	d0,(a0)
	btst	#$ff,d0
	btst	#-1,d0
	rts

L000010:
	bclr	#0,d0
	bclr	d0,(a0)
	bclr	#$ff,d0
	bclr	#-1,d0
	rts

L000020:
	bset	#0,d0
	bset	d0,(a0)
	bset	#$ff,d0
	bset	#-1,d0
	rts

L000030:
	bchg	#0,d0
	bchg	d0,(a0)
	bchg	#$ff,d0
	bchg	#-1,d0
	rts

L000040:
	.dc.b	$08,$00,$01,$00,$08,$00,$80,$ff
	.dc.b	$4e,$75
L00004a:

	.end	L000000
