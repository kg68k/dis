;=============================================
;  Filename obj/tab_break_sec.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $0000000a byte(s)
;  Data size    $00000002 byte(s)
;  Bss  size    $00000000 byte(s)
;  5 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/tab_break_sec.x dised/tab_break_sec.s -gsrc/tab_break_sec.lab -Tsrc/tab_break_sec.tab
;=============================================

	.include	doscall.mac
	.include	iocscall.mac
	.include	fefunc.mac

	.cpu	68000

	.text

L000000:
	lea	(L000008,pc),a0
	DOS	_EXIT

L000006:
	.dc	0
L000008:
	.dc	0

	.data

L00000a:
	.dc.b	$00,$00
L00000c:

	.end	L000000
