;=============================================
;  Filename obj/bitfield.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $0000005c byte(s)
;  Data size    $00000000 byte(s)
;  Bss  size    $00000000 byte(s)
;  2 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/bitfield.x dised/bitfield.s
;=============================================

	.include	doscall.mac
	.include	iocscall.mac
	.include	fefunc.mac

	.cpu	68000

	.text

L000000:
	.cpu	68020
	bftst	d7{d0:d0}
	bftst	d0{d7:d7}
	bftst	d7{d1:1}
	bftst	d0{d1:32}
	bftst	d7{0:d1}
	bftst	d7{31:d1}
	bftst	d7{0:32}
	nop
	bfextu	d7{d0:d0},d0
	bfextu	d0{d7:d7},d0
	bfextu	d7{d1:1},d0
	bfextu	d0{d1:32},d0
	bfextu	d7{0:d1},d0
	bfextu	d7{31:d1},d0
	bfextu	d7{0:32},d0
	nop
	bfins	d0,d7{d0:d0}
	bfins	d0,d0{d7:d7}
	bfins	d0,d7{d1:1}
	bfins	d0,d0{d1:32}
	bfins	d0,d7{0:d1}
	bfins	d0,d7{31:d1}
	bfins	d0,d7{0:32}
	nop
	DOS	_EXIT

L00005c:

	.end	L000000
