;=============================================
;  Filename obj/tab_lbl_nobreak.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $0000000a byte(s)
;  Data size    $00000000 byte(s)
;  Bss  size    $00000000 byte(s)
;  4 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/tab_lbl_nobreak.x dised/tab_lbl_nobreak.s -gsrc/tab_lbl_nobreak.lab -Tsrc/tab_lbl_nobreak.tab
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
L00000a:

	.end	L000000
