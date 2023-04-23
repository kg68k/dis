;=============================================
;  Filename obj/symbol.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $00000004 byte(s)
;  Data size    $00000004 byte(s)
;  Bss  size    $00000404 byte(s)
;  5 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/symbol.x dised/symbol.s
;=============================================

	.include	doscall.mac
	.include	iocscall.mac
	.include	fefunc.mac

	.cpu	68000

	.text

textStart::
	nop
	rts

textEnd::

	.data

dataStart::
	.dc.b	$12,$34,$56,$78
dataEnd::

	.bss

bssStart::
	.ds.b	4
bssEnd::

	.stack

stackStart::
	.ds.b	1024
stackEnd::

	.end	textStart
