;=============================================
;  Filename obj/after_str.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $00000018 byte(s)
;  Data size    $00000000 byte(s)
;  Bss  size    $00000000 byte(s)
;  4 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/after_str.x dised/after_str.s
;=============================================

	.include	doscall.mac
	.include	iocscall.mac
	.include	fefunc.mac

	.cpu	68000

	.text

L000000:
	lea	(L000008,pc),a0
	nop
	DOS	_EXIT

L000008:
	.dc.b	$61,$00,$00,$00

L00000c:
	movem.l	d0-d7/a0-a6,-(sp)
	nop
	movem.l	(sp)+,d0-d7/a0-a6
	rts

L000018:

	.end	L000000
