;=============================================
;  Filename obj/compress.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $000001a4 byte(s)
;  Data size    $00000000 byte(s)
;  Bss  size    $00000000 byte(s)
;  16 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/compress.x dised/compress.s -W2
;=============================================

	.include	doscall.mac
	.include	iocscall.mac
	.include	fefunc.mac

	.cpu	68000

	.text

L000000:
	move.b	(L00004c,pc),d0
	move.b	(L000050,pc),d0
	move	(L000054,pc),d0
	move	(L00005c,pc),d0
	move.l	(L000064,pc),d0
	move.l	(L000074,pc),d0
	nop
	.cpu	68040
	fmove.s	(L000084,pc),fp0
	fmove.s	(L000094,pc),fp0
	fmove.d	(L0000a4,pc),fp0
	fmove.d	(L0000c4,pc),fp0
	fmove	(L0000e4,pc),fp0
	fmove	(L000114,pc),fp0
	fmove.p	(L000144,pc),fp0
	fmove.p	(L000174,pc),fp0
	DOS	_EXIT

L00004c:
	.dcb.b	4,$a5
L000050:
	.ds.b	4
L000054:
	.dcb	4,$cafe
L00005c:
	.ds	4
L000064:
	.dcb.l	4,$deadbeef
L000074:
	.ds.l	4
L000084:
	.dcb.s	4,0f1.5
L000094:
	.ds.s	4
L0000a4:
	.dcb.d	4,0f2.3456
L0000c4:
	.ds.d	4
L0000e4:
	.dcb.x	4,0f3.4567
L000114:
	.ds.x	4
L000144:
	.dcb.p	4,0f4.5678
L000174:
	.ds.p	4
L0001a4:

	.end	L000000
