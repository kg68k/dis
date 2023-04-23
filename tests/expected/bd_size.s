;=============================================
;  Filename obj/bd_size.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $000080de byte(s)
;  Data size    $00000000 byte(s)
;  Bss  size    $00000000 byte(s)
;  32 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/bd_size.x dised/bd_size.s
;=============================================

	.include	doscall.mac
	.include	iocscall.mac
	.include	fefunc.mac

	.cpu	68000

	.text

L000000:
	.cpu	68020
	tst	(L0000f8,pc)
	nop
L000006:
	tst	([L000006+2.w,pc])
L00000c:
	tst	([L00000c+2.l,pc])
	nop
	tst	(L000096,pc,d0.l)
	tst	(L00009a.w,pc,d0.l)
	tst	(L0000a0.l,pc,d0.l)
	tst	(L0000aa.w,pc,d0.l)
	tst	(L0000b0.l,pc,d0.l)
	tst	(L008036,pc,d0.l)
	tst	(L00803e.l,pc,d0.l)
	tst	(L008048,pc,d0.l)
	nop
	tst	([L0000ce,pc])
	tst	([L0000d4.l,pc])
	tst	([L00805c.l,pc])
	tst	([L008066.l,pc])
	tst	([L008070,pc])
	nop
	bra	L00807c

L00007c:
	.dc.b	$00,$00,$00,$00,$00,$00,$00,$00
	.dc.b	$00,$00,$00,$00,$00,$00,$00,$00
	.dc.b	$00,$00,$00,$00,$00,$00,$00,$00
	.dc.b	$00,$00
L000096:
	.dc	$0000,$0000
L00009a:
	.dc	$0000,$0000,$0000
L0000a0:
	.dc	$0000,$0000
L0000a4:
	.dc	$0000,$0000,$0000
L0000aa:
	.dc	$0000,$0000,$0000
L0000b0:
	.dc	$0000,$0000,$0000,$0000
	.dc	$0000,$0000,$0000,$0000
	.dc	$0000,$0000,$0000
L0000c6:
	.dc.b	$00,$00,$00,$00,$00,$00
L0000cc:
	.dc.b	$00,$00
L0000ce:
	.dc.l	0
L0000d2:
	.dc.b	$00,$00
L0000d4:
	.dc.l	$00000000,$00000000
	.dc.l	$00000000,$00000000
	.dc.l	$00000000,$00000000
	.dc.l	$00000000,$00000000
	.dc.l	$00000000
L0000f8:
	.ds	16259
L007ffe:
	.dc	$0000,$0000
L008002:
	.dc	$0000,$0000,$0000
L008008:
	.dc	$0000,$0000,$0000
L00800e:
	.dc	$0000,$0000,$0000
L008014:
	.dc	$0000,$0000,$0000,$0000
	.dc	$0000,$0000,$0000,$0000
	.dc	$0000,$0000,$0000,$0000
	.dc	$0000,$0000,$0000,$0000
	.dc	$0000
L008036:
	.dc.b	$00,$00,$00,$00,$00,$00
L00803c:
	.dc.b	$00,$00
L00803e:
	.dc	$0000,$0000,$0000,$0000
	.dc	$0000
L008048:
	.dc	$0000,$0000,$0000,$0000
	.dc	$0000,$0000,$0000,$0000
	.dc	$0000,$0000
L00805c:
	.dc.b	$00,$00,$00,$00,$00,$00,$00,$00
	.dc.b	$00,$00
L008066:
	.dc.b	$00,$00,$00,$00,$00,$00,$00,$00
	.dc.b	$00,$00
L008070:
	.dc.l	$00000000,$00000000
	.dc.l	$00000000

L00807c:
	tst	(L007ffe,pc,d0.l)
	tst	(L008002.w,pc,d0.l)
	tst	(L008008.l,pc,d0.l)
	tst	(L00800e,pc,d0.l)
	tst	(L008014.l,pc,d0.l)
	tst	(L0000a0,pc,d0.l)
	tst	(L0000a4.l,pc,d0.l)
	tst	(L0000aa,pc,d0.l)
	nop
	tst	([L008036,pc])
	tst	([L00803c.l,pc])
	tst	([L0000c6.l,pc])
	tst	([L0000cc.l,pc])
	tst	([L0000d2,pc])
	nop
	DOS	_EXIT

L0080de:

	.end	L000000
