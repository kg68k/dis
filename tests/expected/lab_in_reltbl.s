;=============================================
;  Filename obj/lab_in_reltbl.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $0000001e byte(s)
;  Data size    $00000000 byte(s)
;  Bss  size    $00000000 byte(s)
;  8 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/lab_in_reltbl.x dised/lab_in_reltbl.s
;=============================================

	.include	doscall.mac
	.include	iocscall.mac
	.include	fefunc.mac

	.cpu	68000

	.text

L000000:
	move	(L00000a,pc,d0.w),d0
	jsr	(L00000a,pc,d0.w)
	DOS	_EXIT

L00000a:
	.dc	L000012-L00000a
	.dc	L000014-L00000a
	.dc	L000016-L00000a
	.dc.b	$00
xxx::
	.dc.b	$e

L000012:
	rts

L000014:
	rts

L000016:
	rts

L000018:
	lea	(xxx,pc),a0
	rts

L00001e:

	.end	L000000
