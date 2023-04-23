;=============================================
;  Filename obj/sym_oorange.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $00000014 byte(s)
;  Data size    $00000000 byte(s)
;  Bss  size    $00000000 byte(s)
;  4 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/sym_oorange.x dised/sym_oorange.s -s2
;=============================================

	.include	doscall.mac
	.include	iocscall.mac
	.include	fefunc.mac

XXX::		.equ	0
		.xdef	aaa
AAA::		.equ	aaa-$100
		.xdef	bbb
BBB::		.equ	aaa-$fe
YYY::		.equ	$12345678
		.xdef	ccc
CCC::		.equ	ddd+$fe
		.xdef	ddd
DDD::		.equ	ddd+$100
ZZZ::		.equ	$ffffffff

	.cpu	68000

	.text

aaa::
	nop
bbb::
	lea	(AAA,pc),a0
	lea	(BBB,pc),a0
	lea	(CCC,pc),a0
	lea	(DDD,pc),a0
ccc::
	DOS	_EXIT

ddd::

	.end	aaa
