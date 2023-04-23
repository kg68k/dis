;=============================================
;  Filename obj/reglist.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $000010da byte(s)
;  Data size    $00000000 byte(s)
;  Bss  size    $00000000 byte(s)
;  2 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/reglist.x dised/reglist.s
;=============================================

	.include	doscall.mac
	.include	iocscall.mac
	.include	fefunc.mac

	.cpu	68000

	.text

L000000:
	movem	d0,(a0)
	movem	d1,(a0)
	movem	d0-d1,(a0)
	movem	d2,(a0)
	movem	d0/d2,(a0)
	movem	d1-d2,(a0)
	movem	d0-d2,(a0)
	movem	d3,(a0)
	movem	d0/d3,(a0)
	movem	d1/d3,(a0)
	movem	d0-d1/d3,(a0)
	movem	d2-d3,(a0)
	movem	d0/d2-d3,(a0)
	movem	d1-d3,(a0)
	movem	d0-d3,(a0)
	movem	d4,(a0)
	movem	d0/d4,(a0)
	movem	d1/d4,(a0)
	movem	d0-d1/d4,(a0)
	movem	d2/d4,(a0)
	movem	d0/d2/d4,(a0)
	movem	d1-d2/d4,(a0)
	movem	d0-d2/d4,(a0)
	movem	d3-d4,(a0)
	movem	d0/d3-d4,(a0)
	movem	d1/d3-d4,(a0)
	movem	d0-d1/d3-d4,(a0)
	movem	d2-d4,(a0)
	movem	d0/d2-d4,(a0)
	movem	d1-d4,(a0)
	movem	d0-d4,(a0)
	movem	d5,(a0)
	movem	d0/d5,(a0)
	movem	d1/d5,(a0)
	movem	d0-d1/d5,(a0)
	movem	d2/d5,(a0)
	movem	d0/d2/d5,(a0)
	movem	d1-d2/d5,(a0)
	movem	d0-d2/d5,(a0)
	movem	d3/d5,(a0)
	movem	d0/d3/d5,(a0)
	movem	d1/d3/d5,(a0)
	movem	d0-d1/d3/d5,(a0)
	movem	d2-d3/d5,(a0)
	movem	d0/d2-d3/d5,(a0)
	movem	d1-d3/d5,(a0)
	movem	d0-d3/d5,(a0)
	movem	d4-d5,(a0)
	movem	d0/d4-d5,(a0)
	movem	d1/d4-d5,(a0)
	movem	d0-d1/d4-d5,(a0)
	movem	d2/d4-d5,(a0)
	movem	d0/d2/d4-d5,(a0)
	movem	d1-d2/d4-d5,(a0)
	movem	d0-d2/d4-d5,(a0)
	movem	d3-d5,(a0)
	movem	d0/d3-d5,(a0)
	movem	d1/d3-d5,(a0)
	movem	d0-d1/d3-d5,(a0)
	movem	d2-d5,(a0)
	movem	d0/d2-d5,(a0)
	movem	d1-d5,(a0)
	movem	d0-d5,(a0)
	movem	d6,(a0)
	movem	d0/d6,(a0)
	movem	d1/d6,(a0)
	movem	d0-d1/d6,(a0)
	movem	d2/d6,(a0)
	movem	d0/d2/d6,(a0)
	movem	d1-d2/d6,(a0)
	movem	d0-d2/d6,(a0)
	movem	d3/d6,(a0)
	movem	d0/d3/d6,(a0)
	movem	d1/d3/d6,(a0)
	movem	d0-d1/d3/d6,(a0)
	movem	d2-d3/d6,(a0)
	movem	d0/d2-d3/d6,(a0)
	movem	d1-d3/d6,(a0)
	movem	d0-d3/d6,(a0)
	movem	d4/d6,(a0)
	movem	d0/d4/d6,(a0)
	movem	d1/d4/d6,(a0)
	movem	d0-d1/d4/d6,(a0)
	movem	d2/d4/d6,(a0)
	movem	d0/d2/d4/d6,(a0)
	movem	d1-d2/d4/d6,(a0)
	movem	d0-d2/d4/d6,(a0)
	movem	d3-d4/d6,(a0)
	movem	d0/d3-d4/d6,(a0)
	movem	d1/d3-d4/d6,(a0)
	movem	d0-d1/d3-d4/d6,(a0)
	movem	d2-d4/d6,(a0)
	movem	d0/d2-d4/d6,(a0)
	movem	d1-d4/d6,(a0)
	movem	d0-d4/d6,(a0)
	movem	d5-d6,(a0)
	movem	d0/d5-d6,(a0)
	movem	d1/d5-d6,(a0)
	movem	d0-d1/d5-d6,(a0)
	movem	d2/d5-d6,(a0)
	movem	d0/d2/d5-d6,(a0)
	movem	d1-d2/d5-d6,(a0)
	movem	d0-d2/d5-d6,(a0)
	movem	d3/d5-d6,(a0)
	movem	d0/d3/d5-d6,(a0)
	movem	d1/d3/d5-d6,(a0)
	movem	d0-d1/d3/d5-d6,(a0)
	movem	d2-d3/d5-d6,(a0)
	movem	d0/d2-d3/d5-d6,(a0)
	movem	d1-d3/d5-d6,(a0)
	movem	d0-d3/d5-d6,(a0)
	movem	d4-d6,(a0)
	movem	d0/d4-d6,(a0)
	movem	d1/d4-d6,(a0)
	movem	d0-d1/d4-d6,(a0)
	movem	d2/d4-d6,(a0)
	movem	d0/d2/d4-d6,(a0)
	movem	d1-d2/d4-d6,(a0)
	movem	d0-d2/d4-d6,(a0)
	movem	d3-d6,(a0)
	movem	d0/d3-d6,(a0)
	movem	d1/d3-d6,(a0)
	movem	d0-d1/d3-d6,(a0)
	movem	d2-d6,(a0)
	movem	d0/d2-d6,(a0)
	movem	d1-d6,(a0)
	movem	d0-d6,(a0)
	movem	d7,(a0)
	movem	d0/d7,(a0)
	movem	d1/d7,(a0)
	movem	d0-d1/d7,(a0)
	movem	d2/d7,(a0)
	movem	d0/d2/d7,(a0)
	movem	d1-d2/d7,(a0)
	movem	d0-d2/d7,(a0)
	movem	d3/d7,(a0)
	movem	d0/d3/d7,(a0)
	movem	d1/d3/d7,(a0)
	movem	d0-d1/d3/d7,(a0)
	movem	d2-d3/d7,(a0)
	movem	d0/d2-d3/d7,(a0)
	movem	d1-d3/d7,(a0)
	movem	d0-d3/d7,(a0)
	movem	d4/d7,(a0)
	movem	d0/d4/d7,(a0)
	movem	d1/d4/d7,(a0)
	movem	d0-d1/d4/d7,(a0)
	movem	d2/d4/d7,(a0)
	movem	d0/d2/d4/d7,(a0)
	movem	d1-d2/d4/d7,(a0)
	movem	d0-d2/d4/d7,(a0)
	movem	d3-d4/d7,(a0)
	movem	d0/d3-d4/d7,(a0)
	movem	d1/d3-d4/d7,(a0)
	movem	d0-d1/d3-d4/d7,(a0)
	movem	d2-d4/d7,(a0)
	movem	d0/d2-d4/d7,(a0)
	movem	d1-d4/d7,(a0)
	movem	d0-d4/d7,(a0)
	movem	d5/d7,(a0)
	movem	d0/d5/d7,(a0)
	movem	d1/d5/d7,(a0)
	movem	d0-d1/d5/d7,(a0)
	movem	d2/d5/d7,(a0)
	movem	d0/d2/d5/d7,(a0)
	movem	d1-d2/d5/d7,(a0)
	movem	d0-d2/d5/d7,(a0)
	movem	d3/d5/d7,(a0)
	movem	d0/d3/d5/d7,(a0)
	movem	d1/d3/d5/d7,(a0)
	movem	d0-d1/d3/d5/d7,(a0)
	movem	d2-d3/d5/d7,(a0)
	movem	d0/d2-d3/d5/d7,(a0)
	movem	d1-d3/d5/d7,(a0)
	movem	d0-d3/d5/d7,(a0)
	movem	d4-d5/d7,(a0)
	movem	d0/d4-d5/d7,(a0)
	movem	d1/d4-d5/d7,(a0)
	movem	d0-d1/d4-d5/d7,(a0)
	movem	d2/d4-d5/d7,(a0)
	movem	d0/d2/d4-d5/d7,(a0)
	movem	d1-d2/d4-d5/d7,(a0)
	movem	d0-d2/d4-d5/d7,(a0)
	movem	d3-d5/d7,(a0)
	movem	d0/d3-d5/d7,(a0)
	movem	d1/d3-d5/d7,(a0)
	movem	d0-d1/d3-d5/d7,(a0)
	movem	d2-d5/d7,(a0)
	movem	d0/d2-d5/d7,(a0)
	movem	d1-d5/d7,(a0)
	movem	d0-d5/d7,(a0)
	movem	d6-d7,(a0)
	movem	d0/d6-d7,(a0)
	movem	d1/d6-d7,(a0)
	movem	d0-d1/d6-d7,(a0)
	movem	d2/d6-d7,(a0)
	movem	d0/d2/d6-d7,(a0)
	movem	d1-d2/d6-d7,(a0)
	movem	d0-d2/d6-d7,(a0)
	movem	d3/d6-d7,(a0)
	movem	d0/d3/d6-d7,(a0)
	movem	d1/d3/d6-d7,(a0)
	movem	d0-d1/d3/d6-d7,(a0)
	movem	d2-d3/d6-d7,(a0)
	movem	d0/d2-d3/d6-d7,(a0)
	movem	d1-d3/d6-d7,(a0)
	movem	d0-d3/d6-d7,(a0)
	movem	d4/d6-d7,(a0)
	movem	d0/d4/d6-d7,(a0)
	movem	d1/d4/d6-d7,(a0)
	movem	d0-d1/d4/d6-d7,(a0)
	movem	d2/d4/d6-d7,(a0)
	movem	d0/d2/d4/d6-d7,(a0)
	movem	d1-d2/d4/d6-d7,(a0)
	movem	d0-d2/d4/d6-d7,(a0)
	movem	d3-d4/d6-d7,(a0)
	movem	d0/d3-d4/d6-d7,(a0)
	movem	d1/d3-d4/d6-d7,(a0)
	movem	d0-d1/d3-d4/d6-d7,(a0)
	movem	d2-d4/d6-d7,(a0)
	movem	d0/d2-d4/d6-d7,(a0)
	movem	d1-d4/d6-d7,(a0)
	movem	d0-d4/d6-d7,(a0)
	movem	d5-d7,(a0)
	movem	d0/d5-d7,(a0)
	movem	d1/d5-d7,(a0)
	movem	d0-d1/d5-d7,(a0)
	movem	d2/d5-d7,(a0)
	movem	d0/d2/d5-d7,(a0)
	movem	d1-d2/d5-d7,(a0)
	movem	d0-d2/d5-d7,(a0)
	movem	d3/d5-d7,(a0)
	movem	d0/d3/d5-d7,(a0)
	movem	d1/d3/d5-d7,(a0)
	movem	d0-d1/d3/d5-d7,(a0)
	movem	d2-d3/d5-d7,(a0)
	movem	d0/d2-d3/d5-d7,(a0)
	movem	d1-d3/d5-d7,(a0)
	movem	d0-d3/d5-d7,(a0)
	movem	d4-d7,(a0)
	movem	d0/d4-d7,(a0)
	movem	d1/d4-d7,(a0)
	movem	d0-d1/d4-d7,(a0)
	movem	d2/d4-d7,(a0)
	movem	d0/d2/d4-d7,(a0)
	movem	d1-d2/d4-d7,(a0)
	movem	d0-d2/d4-d7,(a0)
	movem	d3-d7,(a0)
	movem	d0/d3-d7,(a0)
	movem	d1/d3-d7,(a0)
	movem	d0-d1/d3-d7,(a0)
	movem	d2-d7,(a0)
	movem	d0/d2-d7,(a0)
	movem	d1-d7,(a0)
	movem	d0-d7,(a0)
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	movem	a0,(a0)
	movem	a1,(a0)
	movem	a0-a1,(a0)
	movem	a2,(a0)
	movem	a0/a2,(a0)
	movem	a1-a2,(a0)
	movem	a0-a2,(a0)
	movem	a3,(a0)
	movem	a0/a3,(a0)
	movem	a1/a3,(a0)
	movem	a0-a1/a3,(a0)
	movem	a2-a3,(a0)
	movem	a0/a2-a3,(a0)
	movem	a1-a3,(a0)
	movem	a0-a3,(a0)
	movem	a4,(a0)
	movem	a0/a4,(a0)
	movem	a1/a4,(a0)
	movem	a0-a1/a4,(a0)
	movem	a2/a4,(a0)
	movem	a0/a2/a4,(a0)
	movem	a1-a2/a4,(a0)
	movem	a0-a2/a4,(a0)
	movem	a3-a4,(a0)
	movem	a0/a3-a4,(a0)
	movem	a1/a3-a4,(a0)
	movem	a0-a1/a3-a4,(a0)
	movem	a2-a4,(a0)
	movem	a0/a2-a4,(a0)
	movem	a1-a4,(a0)
	movem	a0-a4,(a0)
	movem	a5,(a0)
	movem	a0/a5,(a0)
	movem	a1/a5,(a0)
	movem	a0-a1/a5,(a0)
	movem	a2/a5,(a0)
	movem	a0/a2/a5,(a0)
	movem	a1-a2/a5,(a0)
	movem	a0-a2/a5,(a0)
	movem	a3/a5,(a0)
	movem	a0/a3/a5,(a0)
	movem	a1/a3/a5,(a0)
	movem	a0-a1/a3/a5,(a0)
	movem	a2-a3/a5,(a0)
	movem	a0/a2-a3/a5,(a0)
	movem	a1-a3/a5,(a0)
	movem	a0-a3/a5,(a0)
	movem	a4-a5,(a0)
	movem	a0/a4-a5,(a0)
	movem	a1/a4-a5,(a0)
	movem	a0-a1/a4-a5,(a0)
	movem	a2/a4-a5,(a0)
	movem	a0/a2/a4-a5,(a0)
	movem	a1-a2/a4-a5,(a0)
	movem	a0-a2/a4-a5,(a0)
	movem	a3-a5,(a0)
	movem	a0/a3-a5,(a0)
	movem	a1/a3-a5,(a0)
	movem	a0-a1/a3-a5,(a0)
	movem	a2-a5,(a0)
	movem	a0/a2-a5,(a0)
	movem	a1-a5,(a0)
	movem	a0-a5,(a0)
	movem	a6,(a0)
	movem	a0/a6,(a0)
	movem	a1/a6,(a0)
	movem	a0-a1/a6,(a0)
	movem	a2/a6,(a0)
	movem	a0/a2/a6,(a0)
	movem	a1-a2/a6,(a0)
	movem	a0-a2/a6,(a0)
	movem	a3/a6,(a0)
	movem	a0/a3/a6,(a0)
	movem	a1/a3/a6,(a0)
	movem	a0-a1/a3/a6,(a0)
	movem	a2-a3/a6,(a0)
	movem	a0/a2-a3/a6,(a0)
	movem	a1-a3/a6,(a0)
	movem	a0-a3/a6,(a0)
	movem	a4/a6,(a0)
	movem	a0/a4/a6,(a0)
	movem	a1/a4/a6,(a0)
	movem	a0-a1/a4/a6,(a0)
	movem	a2/a4/a6,(a0)
	movem	a0/a2/a4/a6,(a0)
	movem	a1-a2/a4/a6,(a0)
	movem	a0-a2/a4/a6,(a0)
	movem	a3-a4/a6,(a0)
	movem	a0/a3-a4/a6,(a0)
	movem	a1/a3-a4/a6,(a0)
	movem	a0-a1/a3-a4/a6,(a0)
	movem	a2-a4/a6,(a0)
	movem	a0/a2-a4/a6,(a0)
	movem	a1-a4/a6,(a0)
	movem	a0-a4/a6,(a0)
	movem	a5-a6,(a0)
	movem	a0/a5-a6,(a0)
	movem	a1/a5-a6,(a0)
	movem	a0-a1/a5-a6,(a0)
	movem	a2/a5-a6,(a0)
	movem	a0/a2/a5-a6,(a0)
	movem	a1-a2/a5-a6,(a0)
	movem	a0-a2/a5-a6,(a0)
	movem	a3/a5-a6,(a0)
	movem	a0/a3/a5-a6,(a0)
	movem	a1/a3/a5-a6,(a0)
	movem	a0-a1/a3/a5-a6,(a0)
	movem	a2-a3/a5-a6,(a0)
	movem	a0/a2-a3/a5-a6,(a0)
	movem	a1-a3/a5-a6,(a0)
	movem	a0-a3/a5-a6,(a0)
	movem	a4-a6,(a0)
	movem	a0/a4-a6,(a0)
	movem	a1/a4-a6,(a0)
	movem	a0-a1/a4-a6,(a0)
	movem	a2/a4-a6,(a0)
	movem	a0/a2/a4-a6,(a0)
	movem	a1-a2/a4-a6,(a0)
	movem	a0-a2/a4-a6,(a0)
	movem	a3-a6,(a0)
	movem	a0/a3-a6,(a0)
	movem	a1/a3-a6,(a0)
	movem	a0-a1/a3-a6,(a0)
	movem	a2-a6,(a0)
	movem	a0/a2-a6,(a0)
	movem	a1-a6,(a0)
	movem	a0-a6,(a0)
	movem	sp,(a0)
	movem	a0/sp,(a0)
	movem	a1/sp,(a0)
	movem	a0-a1/sp,(a0)
	movem	a2/sp,(a0)
	movem	a0/a2/sp,(a0)
	movem	a1-a2/sp,(a0)
	movem	a0-a2/sp,(a0)
	movem	a3/sp,(a0)
	movem	a0/a3/sp,(a0)
	movem	a1/a3/sp,(a0)
	movem	a0-a1/a3/sp,(a0)
	movem	a2-a3/sp,(a0)
	movem	a0/a2-a3/sp,(a0)
	movem	a1-a3/sp,(a0)
	movem	a0-a3/sp,(a0)
	movem	a4/sp,(a0)
	movem	a0/a4/sp,(a0)
	movem	a1/a4/sp,(a0)
	movem	a0-a1/a4/sp,(a0)
	movem	a2/a4/sp,(a0)
	movem	a0/a2/a4/sp,(a0)
	movem	a1-a2/a4/sp,(a0)
	movem	a0-a2/a4/sp,(a0)
	movem	a3-a4/sp,(a0)
	movem	a0/a3-a4/sp,(a0)
	movem	a1/a3-a4/sp,(a0)
	movem	a0-a1/a3-a4/sp,(a0)
	movem	a2-a4/sp,(a0)
	movem	a0/a2-a4/sp,(a0)
	movem	a1-a4/sp,(a0)
	movem	a0-a4/sp,(a0)
	movem	a5/sp,(a0)
	movem	a0/a5/sp,(a0)
	movem	a1/a5/sp,(a0)
	movem	a0-a1/a5/sp,(a0)
	movem	a2/a5/sp,(a0)
	movem	a0/a2/a5/sp,(a0)
	movem	a1-a2/a5/sp,(a0)
	movem	a0-a2/a5/sp,(a0)
	movem	a3/a5/sp,(a0)
	movem	a0/a3/a5/sp,(a0)
	movem	a1/a3/a5/sp,(a0)
	movem	a0-a1/a3/a5/sp,(a0)
	movem	a2-a3/a5/sp,(a0)
	movem	a0/a2-a3/a5/sp,(a0)
	movem	a1-a3/a5/sp,(a0)
	movem	a0-a3/a5/sp,(a0)
	movem	a4-a5/sp,(a0)
	movem	a0/a4-a5/sp,(a0)
	movem	a1/a4-a5/sp,(a0)
	movem	a0-a1/a4-a5/sp,(a0)
	movem	a2/a4-a5/sp,(a0)
	movem	a0/a2/a4-a5/sp,(a0)
	movem	a1-a2/a4-a5/sp,(a0)
	movem	a0-a2/a4-a5/sp,(a0)
	movem	a3-a5/sp,(a0)
	movem	a0/a3-a5/sp,(a0)
	movem	a1/a3-a5/sp,(a0)
	movem	a0-a1/a3-a5/sp,(a0)
	movem	a2-a5/sp,(a0)
	movem	a0/a2-a5/sp,(a0)
	movem	a1-a5/sp,(a0)
	movem	a0-a5/sp,(a0)
	movem	a6-sp,(a0)
	movem	a0/a6-sp,(a0)
	movem	a1/a6-sp,(a0)
	movem	a0-a1/a6-sp,(a0)
	movem	a2/a6-sp,(a0)
	movem	a0/a2/a6-sp,(a0)
	movem	a1-a2/a6-sp,(a0)
	movem	a0-a2/a6-sp,(a0)
	movem	a3/a6-sp,(a0)
	movem	a0/a3/a6-sp,(a0)
	movem	a1/a3/a6-sp,(a0)
	movem	a0-a1/a3/a6-sp,(a0)
	movem	a2-a3/a6-sp,(a0)
	movem	a0/a2-a3/a6-sp,(a0)
	movem	a1-a3/a6-sp,(a0)
	movem	a0-a3/a6-sp,(a0)
	movem	a4/a6-sp,(a0)
	movem	a0/a4/a6-sp,(a0)
	movem	a1/a4/a6-sp,(a0)
	movem	a0-a1/a4/a6-sp,(a0)
	movem	a2/a4/a6-sp,(a0)
	movem	a0/a2/a4/a6-sp,(a0)
	movem	a1-a2/a4/a6-sp,(a0)
	movem	a0-a2/a4/a6-sp,(a0)
	movem	a3-a4/a6-sp,(a0)
	movem	a0/a3-a4/a6-sp,(a0)
	movem	a1/a3-a4/a6-sp,(a0)
	movem	a0-a1/a3-a4/a6-sp,(a0)
	movem	a2-a4/a6-sp,(a0)
	movem	a0/a2-a4/a6-sp,(a0)
	movem	a1-a4/a6-sp,(a0)
	movem	a0-a4/a6-sp,(a0)
	movem	a5-sp,(a0)
	movem	a0/a5-sp,(a0)
	movem	a1/a5-sp,(a0)
	movem	a0-a1/a5-sp,(a0)
	movem	a2/a5-sp,(a0)
	movem	a0/a2/a5-sp,(a0)
	movem	a1-a2/a5-sp,(a0)
	movem	a0-a2/a5-sp,(a0)
	movem	a3/a5-sp,(a0)
	movem	a0/a3/a5-sp,(a0)
	movem	a1/a3/a5-sp,(a0)
	movem	a0-a1/a3/a5-sp,(a0)
	movem	a2-a3/a5-sp,(a0)
	movem	a0/a2-a3/a5-sp,(a0)
	movem	a1-a3/a5-sp,(a0)
	movem	a0-a3/a5-sp,(a0)
	movem	a4-sp,(a0)
	movem	a0/a4-sp,(a0)
	movem	a1/a4-sp,(a0)
	movem	a0-a1/a4-sp,(a0)
	movem	a2/a4-sp,(a0)
	movem	a0/a2/a4-sp,(a0)
	movem	a1-a2/a4-sp,(a0)
	movem	a0-a2/a4-sp,(a0)
	movem	a3-sp,(a0)
	movem	a0/a3-sp,(a0)
	movem	a1/a3-sp,(a0)
	movem	a0-a1/a3-sp,(a0)
	movem	a2-sp,(a0)
	movem	a0/a2-sp,(a0)
	movem	a1-sp,(a0)
	movem	a0-sp,(a0)
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	movem.l	d7/a0,-(a0)
	movem.l	d7/a0,(a0)
	movem.l	d0-d7/a0-sp,-(sp)
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	.cpu	68020
	fmovem.x	fp7,(a0)
	fmovem.x	fp6,(a0)
	fmovem.x	fp6-fp7,(a0)
	fmovem.x	fp5,(a0)
	fmovem.x	fp5/fp7,(a0)
	fmovem.x	fp5-fp6,(a0)
	fmovem.x	fp5-fp7,(a0)
	fmovem.x	fp4,(a0)
	fmovem.x	fp4/fp7,(a0)
	fmovem.x	fp4/fp6,(a0)
	fmovem.x	fp4/fp6-fp7,(a0)
	fmovem.x	fp4-fp5,(a0)
	fmovem.x	fp4-fp5/fp7,(a0)
	fmovem.x	fp4-fp6,(a0)
	fmovem.x	fp4-fp7,(a0)
	fmovem.x	fp3,(a0)
	fmovem.x	fp3/fp7,(a0)
	fmovem.x	fp3/fp6,(a0)
	fmovem.x	fp3/fp6-fp7,(a0)
	fmovem.x	fp3/fp5,(a0)
	fmovem.x	fp3/fp5/fp7,(a0)
	fmovem.x	fp3/fp5-fp6,(a0)
	fmovem.x	fp3/fp5-fp7,(a0)
	fmovem.x	fp3-fp4,(a0)
	fmovem.x	fp3-fp4/fp7,(a0)
	fmovem.x	fp3-fp4/fp6,(a0)
	fmovem.x	fp3-fp4/fp6-fp7,(a0)
	fmovem.x	fp3-fp5,(a0)
	fmovem.x	fp3-fp5/fp7,(a0)
	fmovem.x	fp3-fp6,(a0)
	fmovem.x	fp3-fp7,(a0)
	fmovem.x	fp2,(a0)
	fmovem.x	fp2/fp7,(a0)
	fmovem.x	fp2/fp6,(a0)
	fmovem.x	fp2/fp6-fp7,(a0)
	fmovem.x	fp2/fp5,(a0)
	fmovem.x	fp2/fp5/fp7,(a0)
	fmovem.x	fp2/fp5-fp6,(a0)
	fmovem.x	fp2/fp5-fp7,(a0)
	fmovem.x	fp2/fp4,(a0)
	fmovem.x	fp2/fp4/fp7,(a0)
	fmovem.x	fp2/fp4/fp6,(a0)
	fmovem.x	fp2/fp4/fp6-fp7,(a0)
	fmovem.x	fp2/fp4-fp5,(a0)
	fmovem.x	fp2/fp4-fp5/fp7,(a0)
	fmovem.x	fp2/fp4-fp6,(a0)
	fmovem.x	fp2/fp4-fp7,(a0)
	fmovem.x	fp2-fp3,(a0)
	fmovem.x	fp2-fp3/fp7,(a0)
	fmovem.x	fp2-fp3/fp6,(a0)
	fmovem.x	fp2-fp3/fp6-fp7,(a0)
	fmovem.x	fp2-fp3/fp5,(a0)
	fmovem.x	fp2-fp3/fp5/fp7,(a0)
	fmovem.x	fp2-fp3/fp5-fp6,(a0)
	fmovem.x	fp2-fp3/fp5-fp7,(a0)
	fmovem.x	fp2-fp4,(a0)
	fmovem.x	fp2-fp4/fp7,(a0)
	fmovem.x	fp2-fp4/fp6,(a0)
	fmovem.x	fp2-fp4/fp6-fp7,(a0)
	fmovem.x	fp2-fp5,(a0)
	fmovem.x	fp2-fp5/fp7,(a0)
	fmovem.x	fp2-fp6,(a0)
	fmovem.x	fp2-fp7,(a0)
	fmovem.x	fp1,(a0)
	fmovem.x	fp1/fp7,(a0)
	fmovem.x	fp1/fp6,(a0)
	fmovem.x	fp1/fp6-fp7,(a0)
	fmovem.x	fp1/fp5,(a0)
	fmovem.x	fp1/fp5/fp7,(a0)
	fmovem.x	fp1/fp5-fp6,(a0)
	fmovem.x	fp1/fp5-fp7,(a0)
	fmovem.x	fp1/fp4,(a0)
	fmovem.x	fp1/fp4/fp7,(a0)
	fmovem.x	fp1/fp4/fp6,(a0)
	fmovem.x	fp1/fp4/fp6-fp7,(a0)
	fmovem.x	fp1/fp4-fp5,(a0)
	fmovem.x	fp1/fp4-fp5/fp7,(a0)
	fmovem.x	fp1/fp4-fp6,(a0)
	fmovem.x	fp1/fp4-fp7,(a0)
	fmovem.x	fp1/fp3,(a0)
	fmovem.x	fp1/fp3/fp7,(a0)
	fmovem.x	fp1/fp3/fp6,(a0)
	fmovem.x	fp1/fp3/fp6-fp7,(a0)
	fmovem.x	fp1/fp3/fp5,(a0)
	fmovem.x	fp1/fp3/fp5/fp7,(a0)
	fmovem.x	fp1/fp3/fp5-fp6,(a0)
	fmovem.x	fp1/fp3/fp5-fp7,(a0)
	fmovem.x	fp1/fp3-fp4,(a0)
	fmovem.x	fp1/fp3-fp4/fp7,(a0)
	fmovem.x	fp1/fp3-fp4/fp6,(a0)
	fmovem.x	fp1/fp3-fp4/fp6-fp7,(a0)
	fmovem.x	fp1/fp3-fp5,(a0)
	fmovem.x	fp1/fp3-fp5/fp7,(a0)
	fmovem.x	fp1/fp3-fp6,(a0)
	fmovem.x	fp1/fp3-fp7,(a0)
	fmovem.x	fp1-fp2,(a0)
	fmovem.x	fp1-fp2/fp7,(a0)
	fmovem.x	fp1-fp2/fp6,(a0)
	fmovem.x	fp1-fp2/fp6-fp7,(a0)
	fmovem.x	fp1-fp2/fp5,(a0)
	fmovem.x	fp1-fp2/fp5/fp7,(a0)
	fmovem.x	fp1-fp2/fp5-fp6,(a0)
	fmovem.x	fp1-fp2/fp5-fp7,(a0)
	fmovem.x	fp1-fp2/fp4,(a0)
	fmovem.x	fp1-fp2/fp4/fp7,(a0)
	fmovem.x	fp1-fp2/fp4/fp6,(a0)
	fmovem.x	fp1-fp2/fp4/fp6-fp7,(a0)
	fmovem.x	fp1-fp2/fp4-fp5,(a0)
	fmovem.x	fp1-fp2/fp4-fp5/fp7,(a0)
	fmovem.x	fp1-fp2/fp4-fp6,(a0)
	fmovem.x	fp1-fp2/fp4-fp7,(a0)
	fmovem.x	fp1-fp3,(a0)
	fmovem.x	fp1-fp3/fp7,(a0)
	fmovem.x	fp1-fp3/fp6,(a0)
	fmovem.x	fp1-fp3/fp6-fp7,(a0)
	fmovem.x	fp1-fp3/fp5,(a0)
	fmovem.x	fp1-fp3/fp5/fp7,(a0)
	fmovem.x	fp1-fp3/fp5-fp6,(a0)
	fmovem.x	fp1-fp3/fp5-fp7,(a0)
	fmovem.x	fp1-fp4,(a0)
	fmovem.x	fp1-fp4/fp7,(a0)
	fmovem.x	fp1-fp4/fp6,(a0)
	fmovem.x	fp1-fp4/fp6-fp7,(a0)
	fmovem.x	fp1-fp5,(a0)
	fmovem.x	fp1-fp5/fp7,(a0)
	fmovem.x	fp1-fp6,(a0)
	fmovem.x	fp1-fp7,(a0)
	fmovem.x	fp0,(a0)
	fmovem.x	fp0/fp7,(a0)
	fmovem.x	fp0/fp6,(a0)
	fmovem.x	fp0/fp6-fp7,(a0)
	fmovem.x	fp0/fp5,(a0)
	fmovem.x	fp0/fp5/fp7,(a0)
	fmovem.x	fp0/fp5-fp6,(a0)
	fmovem.x	fp0/fp5-fp7,(a0)
	fmovem.x	fp0/fp4,(a0)
	fmovem.x	fp0/fp4/fp7,(a0)
	fmovem.x	fp0/fp4/fp6,(a0)
	fmovem.x	fp0/fp4/fp6-fp7,(a0)
	fmovem.x	fp0/fp4-fp5,(a0)
	fmovem.x	fp0/fp4-fp5/fp7,(a0)
	fmovem.x	fp0/fp4-fp6,(a0)
	fmovem.x	fp0/fp4-fp7,(a0)
	fmovem.x	fp0/fp3,(a0)
	fmovem.x	fp0/fp3/fp7,(a0)
	fmovem.x	fp0/fp3/fp6,(a0)
	fmovem.x	fp0/fp3/fp6-fp7,(a0)
	fmovem.x	fp0/fp3/fp5,(a0)
	fmovem.x	fp0/fp3/fp5/fp7,(a0)
	fmovem.x	fp0/fp3/fp5-fp6,(a0)
	fmovem.x	fp0/fp3/fp5-fp7,(a0)
	fmovem.x	fp0/fp3-fp4,(a0)
	fmovem.x	fp0/fp3-fp4/fp7,(a0)
	fmovem.x	fp0/fp3-fp4/fp6,(a0)
	fmovem.x	fp0/fp3-fp4/fp6-fp7,(a0)
	fmovem.x	fp0/fp3-fp5,(a0)
	fmovem.x	fp0/fp3-fp5/fp7,(a0)
	fmovem.x	fp0/fp3-fp6,(a0)
	fmovem.x	fp0/fp3-fp7,(a0)
	fmovem.x	fp0/fp2,(a0)
	fmovem.x	fp0/fp2/fp7,(a0)
	fmovem.x	fp0/fp2/fp6,(a0)
	fmovem.x	fp0/fp2/fp6-fp7,(a0)
	fmovem.x	fp0/fp2/fp5,(a0)
	fmovem.x	fp0/fp2/fp5/fp7,(a0)
	fmovem.x	fp0/fp2/fp5-fp6,(a0)
	fmovem.x	fp0/fp2/fp5-fp7,(a0)
	fmovem.x	fp0/fp2/fp4,(a0)
	fmovem.x	fp0/fp2/fp4/fp7,(a0)
	fmovem.x	fp0/fp2/fp4/fp6,(a0)
	fmovem.x	fp0/fp2/fp4/fp6-fp7,(a0)
	fmovem.x	fp0/fp2/fp4-fp5,(a0)
	fmovem.x	fp0/fp2/fp4-fp5/fp7,(a0)
	fmovem.x	fp0/fp2/fp4-fp6,(a0)
	fmovem.x	fp0/fp2/fp4-fp7,(a0)
	fmovem.x	fp0/fp2-fp3,(a0)
	fmovem.x	fp0/fp2-fp3/fp7,(a0)
	fmovem.x	fp0/fp2-fp3/fp6,(a0)
	fmovem.x	fp0/fp2-fp3/fp6-fp7,(a0)
	fmovem.x	fp0/fp2-fp3/fp5,(a0)
	fmovem.x	fp0/fp2-fp3/fp5/fp7,(a0)
	fmovem.x	fp0/fp2-fp3/fp5-fp6,(a0)
	fmovem.x	fp0/fp2-fp3/fp5-fp7,(a0)
	fmovem.x	fp0/fp2-fp4,(a0)
	fmovem.x	fp0/fp2-fp4/fp7,(a0)
	fmovem.x	fp0/fp2-fp4/fp6,(a0)
	fmovem.x	fp0/fp2-fp4/fp6-fp7,(a0)
	fmovem.x	fp0/fp2-fp5,(a0)
	fmovem.x	fp0/fp2-fp5/fp7,(a0)
	fmovem.x	fp0/fp2-fp6,(a0)
	fmovem.x	fp0/fp2-fp7,(a0)
	fmovem.x	fp0-fp1,(a0)
	fmovem.x	fp0-fp1/fp7,(a0)
	fmovem.x	fp0-fp1/fp6,(a0)
	fmovem.x	fp0-fp1/fp6-fp7,(a0)
	fmovem.x	fp0-fp1/fp5,(a0)
	fmovem.x	fp0-fp1/fp5/fp7,(a0)
	fmovem.x	fp0-fp1/fp5-fp6,(a0)
	fmovem.x	fp0-fp1/fp5-fp7,(a0)
	fmovem.x	fp0-fp1/fp4,(a0)
	fmovem.x	fp0-fp1/fp4/fp7,(a0)
	fmovem.x	fp0-fp1/fp4/fp6,(a0)
	fmovem.x	fp0-fp1/fp4/fp6-fp7,(a0)
	fmovem.x	fp0-fp1/fp4-fp5,(a0)
	fmovem.x	fp0-fp1/fp4-fp5/fp7,(a0)
	fmovem.x	fp0-fp1/fp4-fp6,(a0)
	fmovem.x	fp0-fp1/fp4-fp7,(a0)
	fmovem.x	fp0-fp1/fp3,(a0)
	fmovem.x	fp0-fp1/fp3/fp7,(a0)
	fmovem.x	fp0-fp1/fp3/fp6,(a0)
	fmovem.x	fp0-fp1/fp3/fp6-fp7,(a0)
	fmovem.x	fp0-fp1/fp3/fp5,(a0)
	fmovem.x	fp0-fp1/fp3/fp5/fp7,(a0)
	fmovem.x	fp0-fp1/fp3/fp5-fp6,(a0)
	fmovem.x	fp0-fp1/fp3/fp5-fp7,(a0)
	fmovem.x	fp0-fp1/fp3-fp4,(a0)
	fmovem.x	fp0-fp1/fp3-fp4/fp7,(a0)
	fmovem.x	fp0-fp1/fp3-fp4/fp6,(a0)
	fmovem.x	fp0-fp1/fp3-fp4/fp6-fp7,(a0)
	fmovem.x	fp0-fp1/fp3-fp5,(a0)
	fmovem.x	fp0-fp1/fp3-fp5/fp7,(a0)
	fmovem.x	fp0-fp1/fp3-fp6,(a0)
	fmovem.x	fp0-fp1/fp3-fp7,(a0)
	fmovem.x	fp0-fp2,(a0)
	fmovem.x	fp0-fp2/fp7,(a0)
	fmovem.x	fp0-fp2/fp6,(a0)
	fmovem.x	fp0-fp2/fp6-fp7,(a0)
	fmovem.x	fp0-fp2/fp5,(a0)
	fmovem.x	fp0-fp2/fp5/fp7,(a0)
	fmovem.x	fp0-fp2/fp5-fp6,(a0)
	fmovem.x	fp0-fp2/fp5-fp7,(a0)
	fmovem.x	fp0-fp2/fp4,(a0)
	fmovem.x	fp0-fp2/fp4/fp7,(a0)
	fmovem.x	fp0-fp2/fp4/fp6,(a0)
	fmovem.x	fp0-fp2/fp4/fp6-fp7,(a0)
	fmovem.x	fp0-fp2/fp4-fp5,(a0)
	fmovem.x	fp0-fp2/fp4-fp5/fp7,(a0)
	fmovem.x	fp0-fp2/fp4-fp6,(a0)
	fmovem.x	fp0-fp2/fp4-fp7,(a0)
	fmovem.x	fp0-fp3,(a0)
	fmovem.x	fp0-fp3/fp7,(a0)
	fmovem.x	fp0-fp3/fp6,(a0)
	fmovem.x	fp0-fp3/fp6-fp7,(a0)
	fmovem.x	fp0-fp3/fp5,(a0)
	fmovem.x	fp0-fp3/fp5/fp7,(a0)
	fmovem.x	fp0-fp3/fp5-fp6,(a0)
	fmovem.x	fp0-fp3/fp5-fp7,(a0)
	fmovem.x	fp0-fp4,(a0)
	fmovem.x	fp0-fp4/fp7,(a0)
	fmovem.x	fp0-fp4/fp6,(a0)
	fmovem.x	fp0-fp4/fp6-fp7,(a0)
	fmovem.x	fp0-fp5,(a0)
	fmovem.x	fp0-fp5/fp7,(a0)
	fmovem.x	fp0-fp6,(a0)
	fmovem.x	fp0-fp7,(a0)
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	fmovem.x	fp0,-(a0)
	fmovem.x	fp1,-(a0)
	fmovem.x	fp0-fp1,-(a0)
	fmovem.x	fp2,-(a0)
	fmovem.x	fp0/fp2,-(a0)
	fmovem.x	fp1-fp2,-(a0)
	fmovem.x	fp0-fp2,-(a0)
	fmovem.x	fp3,-(a0)
	fmovem.x	fp0/fp3,-(a0)
	fmovem.x	fp1/fp3,-(a0)
	fmovem.x	fp0-fp1/fp3,-(a0)
	fmovem.x	fp2-fp3,-(a0)
	fmovem.x	fp0/fp2-fp3,-(a0)
	fmovem.x	fp1-fp3,-(a0)
	fmovem.x	fp0-fp3,-(a0)
	fmovem.x	fp4,-(a0)
	fmovem.x	fp0/fp4,-(a0)
	fmovem.x	fp1/fp4,-(a0)
	fmovem.x	fp0-fp1/fp4,-(a0)
	fmovem.x	fp2/fp4,-(a0)
	fmovem.x	fp0/fp2/fp4,-(a0)
	fmovem.x	fp1-fp2/fp4,-(a0)
	fmovem.x	fp0-fp2/fp4,-(a0)
	fmovem.x	fp3-fp4,-(a0)
	fmovem.x	fp0/fp3-fp4,-(a0)
	fmovem.x	fp1/fp3-fp4,-(a0)
	fmovem.x	fp0-fp1/fp3-fp4,-(a0)
	fmovem.x	fp2-fp4,-(a0)
	fmovem.x	fp0/fp2-fp4,-(a0)
	fmovem.x	fp1-fp4,-(a0)
	fmovem.x	fp0-fp4,-(a0)
	fmovem.x	fp5,-(a0)
	fmovem.x	fp0/fp5,-(a0)
	fmovem.x	fp1/fp5,-(a0)
	fmovem.x	fp0-fp1/fp5,-(a0)
	fmovem.x	fp2/fp5,-(a0)
	fmovem.x	fp0/fp2/fp5,-(a0)
	fmovem.x	fp1-fp2/fp5,-(a0)
	fmovem.x	fp0-fp2/fp5,-(a0)
	fmovem.x	fp3/fp5,-(a0)
	fmovem.x	fp0/fp3/fp5,-(a0)
	fmovem.x	fp1/fp3/fp5,-(a0)
	fmovem.x	fp0-fp1/fp3/fp5,-(a0)
	fmovem.x	fp2-fp3/fp5,-(a0)
	fmovem.x	fp0/fp2-fp3/fp5,-(a0)
	fmovem.x	fp1-fp3/fp5,-(a0)
	fmovem.x	fp0-fp3/fp5,-(a0)
	fmovem.x	fp4-fp5,-(a0)
	fmovem.x	fp0/fp4-fp5,-(a0)
	fmovem.x	fp1/fp4-fp5,-(a0)
	fmovem.x	fp0-fp1/fp4-fp5,-(a0)
	fmovem.x	fp2/fp4-fp5,-(a0)
	fmovem.x	fp0/fp2/fp4-fp5,-(a0)
	fmovem.x	fp1-fp2/fp4-fp5,-(a0)
	fmovem.x	fp0-fp2/fp4-fp5,-(a0)
	fmovem.x	fp3-fp5,-(a0)
	fmovem.x	fp0/fp3-fp5,-(a0)
	fmovem.x	fp1/fp3-fp5,-(a0)
	fmovem.x	fp0-fp1/fp3-fp5,-(a0)
	fmovem.x	fp2-fp5,-(a0)
	fmovem.x	fp0/fp2-fp5,-(a0)
	fmovem.x	fp1-fp5,-(a0)
	fmovem.x	fp0-fp5,-(a0)
	fmovem.x	fp6,-(a0)
	fmovem.x	fp0/fp6,-(a0)
	fmovem.x	fp1/fp6,-(a0)
	fmovem.x	fp0-fp1/fp6,-(a0)
	fmovem.x	fp2/fp6,-(a0)
	fmovem.x	fp0/fp2/fp6,-(a0)
	fmovem.x	fp1-fp2/fp6,-(a0)
	fmovem.x	fp0-fp2/fp6,-(a0)
	fmovem.x	fp3/fp6,-(a0)
	fmovem.x	fp0/fp3/fp6,-(a0)
	fmovem.x	fp1/fp3/fp6,-(a0)
	fmovem.x	fp0-fp1/fp3/fp6,-(a0)
	fmovem.x	fp2-fp3/fp6,-(a0)
	fmovem.x	fp0/fp2-fp3/fp6,-(a0)
	fmovem.x	fp1-fp3/fp6,-(a0)
	fmovem.x	fp0-fp3/fp6,-(a0)
	fmovem.x	fp4/fp6,-(a0)
	fmovem.x	fp0/fp4/fp6,-(a0)
	fmovem.x	fp1/fp4/fp6,-(a0)
	fmovem.x	fp0-fp1/fp4/fp6,-(a0)
	fmovem.x	fp2/fp4/fp6,-(a0)
	fmovem.x	fp0/fp2/fp4/fp6,-(a0)
	fmovem.x	fp1-fp2/fp4/fp6,-(a0)
	fmovem.x	fp0-fp2/fp4/fp6,-(a0)
	fmovem.x	fp3-fp4/fp6,-(a0)
	fmovem.x	fp0/fp3-fp4/fp6,-(a0)
	fmovem.x	fp1/fp3-fp4/fp6,-(a0)
	fmovem.x	fp0-fp1/fp3-fp4/fp6,-(a0)
	fmovem.x	fp2-fp4/fp6,-(a0)
	fmovem.x	fp0/fp2-fp4/fp6,-(a0)
	fmovem.x	fp1-fp4/fp6,-(a0)
	fmovem.x	fp0-fp4/fp6,-(a0)
	fmovem.x	fp5-fp6,-(a0)
	fmovem.x	fp0/fp5-fp6,-(a0)
	fmovem.x	fp1/fp5-fp6,-(a0)
	fmovem.x	fp0-fp1/fp5-fp6,-(a0)
	fmovem.x	fp2/fp5-fp6,-(a0)
	fmovem.x	fp0/fp2/fp5-fp6,-(a0)
	fmovem.x	fp1-fp2/fp5-fp6,-(a0)
	fmovem.x	fp0-fp2/fp5-fp6,-(a0)
	fmovem.x	fp3/fp5-fp6,-(a0)
	fmovem.x	fp0/fp3/fp5-fp6,-(a0)
	fmovem.x	fp1/fp3/fp5-fp6,-(a0)
	fmovem.x	fp0-fp1/fp3/fp5-fp6,-(a0)
	fmovem.x	fp2-fp3/fp5-fp6,-(a0)
	fmovem.x	fp0/fp2-fp3/fp5-fp6,-(a0)
	fmovem.x	fp1-fp3/fp5-fp6,-(a0)
	fmovem.x	fp0-fp3/fp5-fp6,-(a0)
	fmovem.x	fp4-fp6,-(a0)
	fmovem.x	fp0/fp4-fp6,-(a0)
	fmovem.x	fp1/fp4-fp6,-(a0)
	fmovem.x	fp0-fp1/fp4-fp6,-(a0)
	fmovem.x	fp2/fp4-fp6,-(a0)
	fmovem.x	fp0/fp2/fp4-fp6,-(a0)
	fmovem.x	fp1-fp2/fp4-fp6,-(a0)
	fmovem.x	fp0-fp2/fp4-fp6,-(a0)
	fmovem.x	fp3-fp6,-(a0)
	fmovem.x	fp0/fp3-fp6,-(a0)
	fmovem.x	fp1/fp3-fp6,-(a0)
	fmovem.x	fp0-fp1/fp3-fp6,-(a0)
	fmovem.x	fp2-fp6,-(a0)
	fmovem.x	fp0/fp2-fp6,-(a0)
	fmovem.x	fp1-fp6,-(a0)
	fmovem.x	fp0-fp6,-(a0)
	fmovem.x	fp7,-(a0)
	fmovem.x	fp0/fp7,-(a0)
	fmovem.x	fp1/fp7,-(a0)
	fmovem.x	fp0-fp1/fp7,-(a0)
	fmovem.x	fp2/fp7,-(a0)
	fmovem.x	fp0/fp2/fp7,-(a0)
	fmovem.x	fp1-fp2/fp7,-(a0)
	fmovem.x	fp0-fp2/fp7,-(a0)
	fmovem.x	fp3/fp7,-(a0)
	fmovem.x	fp0/fp3/fp7,-(a0)
	fmovem.x	fp1/fp3/fp7,-(a0)
	fmovem.x	fp0-fp1/fp3/fp7,-(a0)
	fmovem.x	fp2-fp3/fp7,-(a0)
	fmovem.x	fp0/fp2-fp3/fp7,-(a0)
	fmovem.x	fp1-fp3/fp7,-(a0)
	fmovem.x	fp0-fp3/fp7,-(a0)
	fmovem.x	fp4/fp7,-(a0)
	fmovem.x	fp0/fp4/fp7,-(a0)
	fmovem.x	fp1/fp4/fp7,-(a0)
	fmovem.x	fp0-fp1/fp4/fp7,-(a0)
	fmovem.x	fp2/fp4/fp7,-(a0)
	fmovem.x	fp0/fp2/fp4/fp7,-(a0)
	fmovem.x	fp1-fp2/fp4/fp7,-(a0)
	fmovem.x	fp0-fp2/fp4/fp7,-(a0)
	fmovem.x	fp3-fp4/fp7,-(a0)
	fmovem.x	fp0/fp3-fp4/fp7,-(a0)
	fmovem.x	fp1/fp3-fp4/fp7,-(a0)
	fmovem.x	fp0-fp1/fp3-fp4/fp7,-(a0)
	fmovem.x	fp2-fp4/fp7,-(a0)
	fmovem.x	fp0/fp2-fp4/fp7,-(a0)
	fmovem.x	fp1-fp4/fp7,-(a0)
	fmovem.x	fp0-fp4/fp7,-(a0)
	fmovem.x	fp5/fp7,-(a0)
	fmovem.x	fp0/fp5/fp7,-(a0)
	fmovem.x	fp1/fp5/fp7,-(a0)
	fmovem.x	fp0-fp1/fp5/fp7,-(a0)
	fmovem.x	fp2/fp5/fp7,-(a0)
	fmovem.x	fp0/fp2/fp5/fp7,-(a0)
	fmovem.x	fp1-fp2/fp5/fp7,-(a0)
	fmovem.x	fp0-fp2/fp5/fp7,-(a0)
	fmovem.x	fp3/fp5/fp7,-(a0)
	fmovem.x	fp0/fp3/fp5/fp7,-(a0)
	fmovem.x	fp1/fp3/fp5/fp7,-(a0)
	fmovem.x	fp0-fp1/fp3/fp5/fp7,-(a0)
	fmovem.x	fp2-fp3/fp5/fp7,-(a0)
	fmovem.x	fp0/fp2-fp3/fp5/fp7,-(a0)
	fmovem.x	fp1-fp3/fp5/fp7,-(a0)
	fmovem.x	fp0-fp3/fp5/fp7,-(a0)
	fmovem.x	fp4-fp5/fp7,-(a0)
	fmovem.x	fp0/fp4-fp5/fp7,-(a0)
	fmovem.x	fp1/fp4-fp5/fp7,-(a0)
	fmovem.x	fp0-fp1/fp4-fp5/fp7,-(a0)
	fmovem.x	fp2/fp4-fp5/fp7,-(a0)
	fmovem.x	fp0/fp2/fp4-fp5/fp7,-(a0)
	fmovem.x	fp1-fp2/fp4-fp5/fp7,-(a0)
	fmovem.x	fp0-fp2/fp4-fp5/fp7,-(a0)
	fmovem.x	fp3-fp5/fp7,-(a0)
	fmovem.x	fp0/fp3-fp5/fp7,-(a0)
	fmovem.x	fp1/fp3-fp5/fp7,-(a0)
	fmovem.x	fp0-fp1/fp3-fp5/fp7,-(a0)
	fmovem.x	fp2-fp5/fp7,-(a0)
	fmovem.x	fp0/fp2-fp5/fp7,-(a0)
	fmovem.x	fp1-fp5/fp7,-(a0)
	fmovem.x	fp0-fp5/fp7,-(a0)
	fmovem.x	fp6-fp7,-(a0)
	fmovem.x	fp0/fp6-fp7,-(a0)
	fmovem.x	fp1/fp6-fp7,-(a0)
	fmovem.x	fp0-fp1/fp6-fp7,-(a0)
	fmovem.x	fp2/fp6-fp7,-(a0)
	fmovem.x	fp0/fp2/fp6-fp7,-(a0)
	fmovem.x	fp1-fp2/fp6-fp7,-(a0)
	fmovem.x	fp0-fp2/fp6-fp7,-(a0)
	fmovem.x	fp3/fp6-fp7,-(a0)
	fmovem.x	fp0/fp3/fp6-fp7,-(a0)
	fmovem.x	fp1/fp3/fp6-fp7,-(a0)
	fmovem.x	fp0-fp1/fp3/fp6-fp7,-(a0)
	fmovem.x	fp2-fp3/fp6-fp7,-(a0)
	fmovem.x	fp0/fp2-fp3/fp6-fp7,-(a0)
	fmovem.x	fp1-fp3/fp6-fp7,-(a0)
	fmovem.x	fp0-fp3/fp6-fp7,-(a0)
	fmovem.x	fp4/fp6-fp7,-(a0)
	fmovem.x	fp0/fp4/fp6-fp7,-(a0)
	fmovem.x	fp1/fp4/fp6-fp7,-(a0)
	fmovem.x	fp0-fp1/fp4/fp6-fp7,-(a0)
	fmovem.x	fp2/fp4/fp6-fp7,-(a0)
	fmovem.x	fp0/fp2/fp4/fp6-fp7,-(a0)
	fmovem.x	fp1-fp2/fp4/fp6-fp7,-(a0)
	fmovem.x	fp0-fp2/fp4/fp6-fp7,-(a0)
	fmovem.x	fp3-fp4/fp6-fp7,-(a0)
	fmovem.x	fp0/fp3-fp4/fp6-fp7,-(a0)
	fmovem.x	fp1/fp3-fp4/fp6-fp7,-(a0)
	fmovem.x	fp0-fp1/fp3-fp4/fp6-fp7,-(a0)
	fmovem.x	fp2-fp4/fp6-fp7,-(a0)
	fmovem.x	fp0/fp2-fp4/fp6-fp7,-(a0)
	fmovem.x	fp1-fp4/fp6-fp7,-(a0)
	fmovem.x	fp0-fp4/fp6-fp7,-(a0)
	fmovem.x	fp5-fp7,-(a0)
	fmovem.x	fp0/fp5-fp7,-(a0)
	fmovem.x	fp1/fp5-fp7,-(a0)
	fmovem.x	fp0-fp1/fp5-fp7,-(a0)
	fmovem.x	fp2/fp5-fp7,-(a0)
	fmovem.x	fp0/fp2/fp5-fp7,-(a0)
	fmovem.x	fp1-fp2/fp5-fp7,-(a0)
	fmovem.x	fp0-fp2/fp5-fp7,-(a0)
	fmovem.x	fp3/fp5-fp7,-(a0)
	fmovem.x	fp0/fp3/fp5-fp7,-(a0)
	fmovem.x	fp1/fp3/fp5-fp7,-(a0)
	fmovem.x	fp0-fp1/fp3/fp5-fp7,-(a0)
	fmovem.x	fp2-fp3/fp5-fp7,-(a0)
	fmovem.x	fp0/fp2-fp3/fp5-fp7,-(a0)
	fmovem.x	fp1-fp3/fp5-fp7,-(a0)
	fmovem.x	fp0-fp3/fp5-fp7,-(a0)
	fmovem.x	fp4-fp7,-(a0)
	fmovem.x	fp0/fp4-fp7,-(a0)
	fmovem.x	fp1/fp4-fp7,-(a0)
	fmovem.x	fp0-fp1/fp4-fp7,-(a0)
	fmovem.x	fp2/fp4-fp7,-(a0)
	fmovem.x	fp0/fp2/fp4-fp7,-(a0)
	fmovem.x	fp1-fp2/fp4-fp7,-(a0)
	fmovem.x	fp0-fp2/fp4-fp7,-(a0)
	fmovem.x	fp3-fp7,-(a0)
	fmovem.x	fp0/fp3-fp7,-(a0)
	fmovem.x	fp1/fp3-fp7,-(a0)
	fmovem.x	fp0-fp1/fp3-fp7,-(a0)
	fmovem.x	fp2-fp7,-(a0)
	fmovem.x	fp0/fp2-fp7,-(a0)
	fmovem.x	fp1-fp7,-(a0)
	fmovem.x	fp0-fp7,-(a0)
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	fmove	fpiar,(a0)
	fmove	fpsr,(a0)
	fmovem	fpsr/fpiar,(a0)
	fmove	fpcr,(a0)
	fmovem	fpcr/fpiar,(a0)
	fmovem	fpcr/fpsr,(a0)
	fmovem	fpcr/fpsr/fpiar,(a0)
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	DOS	_EXIT

L0010da:

	.end	L000000
