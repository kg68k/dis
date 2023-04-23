;=============================================
;  Filename obj/scalefactor.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $00000026 byte(s)
;  Data size    $00000000 byte(s)
;  Bss  size    $00000000 byte(s)
;  2 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/scalefactor.x dised/scalefactor.s
;=============================================

	.include	doscall.mac
	.include	iocscall.mac
	.include	fefunc.mac

	.cpu	68000

	.text

L000000:
	move	(a0,d0.w),d0
	.cpu	68020
	move	(a0,d0.w*2),d0
	move	(a0,d0.w*4),d0
	move	(a0,d0.w*8),d0
	nop
	move	(a0,d0.l),d0
	move	(a0,d0.l*2),d0
	move	(a0,d0.l*4),d0
	move	(a0,d0.l*8),d0
	nop
	DOS	_EXIT

L000026:

	.end	L000000
