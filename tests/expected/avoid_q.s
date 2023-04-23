;=============================================
;  Filename obj/avoid_q.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $0000008e byte(s)
;  Data size    $00000000 byte(s)
;  Bss  size    $00000000 byte(s)
;  2 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/avoid_q.x dised/avoid_q.s -A
;=============================================

	.include	doscall.mac
	.include	iocscall.mac
	.include	fefunc.mac

	.cpu	68000

	.text

L000000:
	move.l	#0.l,d0
	move.l	#$7f.l,d0
	move.l	#$80,d0
	move.l	#$ffffff80.l,d0
	move.l	#$ffffff7f,d0
	nop
	add	#0,(a0)
	add	#1.w,(a0)
	add	#8.w,(a0)
	add	#9,(a0)
	nop
	sub	#0,(a0)
	sub	#1.w,(a0)
	sub	#8.w,(a0)
	sub	#9,(a0)
	nop
	add	#0,d0
	add	#1.w,d0
	add	#8.w,d0
	add	#9,d0
	nop
	sub	#0,d0
	sub	#1.w,d0
	sub	#8.w,d0
	sub	#9,d0
	nop
	add	#0,a0
	add	#1.w,a0
	add	#8.w,a0
	add	#9,a0
	nop
	sub	#0,a0
	sub	#1.w,a0
	sub	#8.w,a0
	sub	#9,a0
	nop
	DOS	_EXIT

L00008e:

	.end	L000000
