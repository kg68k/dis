;=============================================
;  Filename obj/imm_comment.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $000000b4 byte(s)
;  Data size    $00000000 byte(s)
;  Bss  size    $00000000 byte(s)
;  2 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/imm_comment.x dised/imm_comment.s -M
;=============================================

	.include	doscall.mac
	.include	iocscall.mac
	.include	fefunc.mac

	.cpu	68000

	.text

L000000:
	moveq	#$61,d0			;'a'
	move.b	#$61,d0			;'a'
	move	#$6162,d0		;'ab'
	move	#$78,d0			;'x'
	move.l	#$61626364,d0		;'abcd'
	move.l	#$78797a,d0		;'xyz'
	move.l	#$78797a00,d0		;'xyz'<<8
	move.l	#$787900,d0		;'xy'<<8
	cmp.b	#$61,d0			;'a'
	cmp	#$6162,d0		;'ab'
	cmp	#$78,d0			;'x'
	cmp.l	#$61626364,d0		;'abcd'
	cmp.l	#$78797a,d0		;'xyz'
	cmp.l	#$78797a00,d0		;'xyz'<<8
	cmp.l	#$787900,d0		;'xy'<<8
	cmpi.b	#$61,d0			;'a'
	cmpi	#$6162,d0		;'ab'
	cmpi	#$78,d0			;'x'
	cmpi.l	#$61626364,d0		;'abcd'
	cmpi.l	#$78797a,d0		;'xyz'
	cmpi.l	#$78797a00,d0		;'xyz'<<8
	cmpi.l	#$787900,d0		;'xy'<<8
	cmpa	#$6162,a0		;'ab'
	cmpa	#$78,a0			;'x'
	cmpa.l	#$61626364,a0		;'abcd'
	cmpa.l	#$78797a,a0		;'xyz'
	cmpa.l	#$78797a00,a0		;'xyz'<<8
	cmpa.l	#$787900,a0		;'xy'<<8
	add.b	#$61,d0			;'a'
	add	#$6162,d0
	add.l	#$61626364,d0
	sub.b	#$61,d0			;'a'
	sub	#$6162,d0
	sub.l	#$61626364,d0
	.cpu	68020
	pack	d0,d0,#$6162		;'ab'
	unpk	d0,d0,#$6162		;'ab'
	DOS	_EXIT

L0000b4:

	.end	L000000
