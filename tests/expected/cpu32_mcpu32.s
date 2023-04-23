;=============================================
;  Filename obj/cpu32.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $0000003c byte(s)
;  Data size    $00000000 byte(s)
;  Bss  size    $00000000 byte(s)
;  11 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/cpu32.x dised/cpu32_mcpu32.s -mcpu32
;=============================================

	.include	doscall.mac
	.include	iocscall.mac
	.include	fefunc.mac

	.cpu	68000

	.text

L000000:
	.cpu	68060
	lpstop	#$2000
	rts

L000008:
	bgnd
	rts

L00000c:
	tbls.b	(a0),d7
	rts

L000012:
	tblsn.b	(a0),d7
	rts

L000018:
	tbls.b	d0:d1,d7
	rts

L00001e:
	tblsn.b	d0:d1,d7
	rts

L000024:
	tblu.b	(a0),d7
	rts

L00002a:
	tblun.b	(a0),d7
	rts

L000030:
	tblu.b	d0:d1,d7
	rts

L000036:
	tblun.b	d0:d1,d7
	rts

L00003c:

	.end	L000000
