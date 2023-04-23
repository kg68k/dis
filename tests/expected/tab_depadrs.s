;=============================================
;  Filename obj/tab_depadrs.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $00000022 byte(s)
;  Data size    $00000000 byte(s)
;  Bss  size    $00000000 byte(s)
;  5 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/tab_depadrs.x dised/tab_depadrs.s -gsrc/tab_depadrs.lab -Tsrc/tab_depadrs.tab
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
	.dc	1
	.dc	0
	.dc.l	L000006
	.dc	2
	.dc	0
	.dc.l	L000006

L000016:
	clr	(L000006)
	rts

L00001e:
	moveq	#0,d0
	rts

L000022:

	.end	L000000
