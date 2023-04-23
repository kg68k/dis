;=============================================
;  Filename obj/devhead_loop.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $0000004e byte(s)
;  Data size    $00000000 byte(s)
;  Bss  size    $00000000 byte(s)
;  16 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/devhead_loop.x dised/devhead_loop.s -d
;=============================================

	.include	doscall.mac
	.include	iocscall.mac
	.include	fefunc.mac

	.cpu	68000

	.text

L000000:
	.dc.l	L00001a
	.dc	$8000
	.dc.l	L000016
	.dc.l	L000018
	.dc.b	'/*?--?*/'

L000016:
	rts

L000018:
	rts

L00001a:
	.dc.l	L000034
	.dc	$8000
	.dc.l	L000030
	.dc.l	L000032
	.dc.b	'/*?--?*/'

L000030:
	rts

L000032:
	rts

L000034:
	.dc.l	L00001a
	.dc	$8000
	.dc.l	L00004a
	.dc.l	L00004c
	.dc.b	'/*?--?*/'

L00004a:
	rts

L00004c:
	rts

L00004e:

	.end	L000000
