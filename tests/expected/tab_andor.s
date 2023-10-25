;=============================================
;  Filename obj/tab_andor.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $0000000e byte(s)
;  Data size    $00000000 byte(s)
;  Bss  size    $00000000 byte(s)
;  6 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/tab_andor.x dised/tab_andor.s -gsrc/tab_andor.lab -Tsrc/tab_andor.tab
;=============================================

	.include	doscall.mac
	.include	iocscall.mac
	.include	fefunc.mac

	.cpu	68000

	.text

L000000:
	lea	(L000006,pc),a0
	rts

L000006:
	.dc.b	L00000a-$
	.dc.b	L00000c-$
L000008:
	.dc.b	$00,$00

L00000a:
	rts

L00000c:
	rts

L00000e:

	.end	L000000
