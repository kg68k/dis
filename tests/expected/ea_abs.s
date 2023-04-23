;=============================================
;  Filename obj/ea_abs.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $0000003e byte(s)
;  Data size    $00000000 byte(s)
;  Bss  size    $00000000 byte(s)
;  2 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/ea_abs.x dised/ea_abs.s
;=============================================

	.include	doscall.mac
	.include	iocscall.mac
	.include	fefunc.mac

	.cpu	68000

	.text

L000000:
	pea	(0)
	pea	(1)
	pea	($7fff)
	pea	(-$8000)
	pea	(-1)
	pea	(-$8000)
	pea	(0).l
	pea	(1).l
	pea	($7fff).l
	pea	($8000)
	pea	($ffffffff).l
	pea	($ffff8000).l
	DOS	_EXIT

L00003e:

	.end	L000000
