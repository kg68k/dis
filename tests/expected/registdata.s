;=============================================
;  Filename obj/registdata.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $00000028 byte(s)
;  Data size    $00000000 byte(s)
;  Bss  size    $00000000 byte(s)
;  4 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/registdata.x dised/registdata.s
;=============================================

	.include	doscall.mac
	.include	iocscall.mac
	.include	fefunc.mac

	.cpu	68000

	.text

L000000:
	.cpu	68020
	move	(2,zpc),d0
	move	([8,zpc]),d0
	nop
	move	(L000022,zpc),d0
	move	([L000024,zpc]),d0
	nop
	DOS	_EXIT

L000022:
	.dc	$beef
L000024:
	.dc.l	$12345678
L000028:

	.end	L000000
