;=============================================
;  Filename obj/tab_even.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $00000010 byte(s)
;  Data size    $00000000 byte(s)
;  Bss  size    $00000000 byte(s)
;  4 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/tab_even.x dised/tab_even.s -gsrc/tab_even.lab -Tsrc/tab_even.tab
;=============================================

	.include	doscall.mac
	.include	iocscall.mac
	.include	fefunc.mac

	.cpu	68000

	.text

L000000:
	lea	(L000006,pc),a0
	DOS	_EXIT

L000006:
	.dc.b	1
	.dc.b	'a'
	.even

	.dc.b	2
	.dc.b	'ab'
	.even
L00000c:
	.dc.b	$02,$78,$79,$ff
L000010:

	.end	L000000
