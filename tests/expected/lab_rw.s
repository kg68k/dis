;=============================================
;  Filename obj/lab_rw.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $0000001c byte(s)
;  Data size    $00000000 byte(s)
;  Bss  size    $00000000 byte(s)
;  6 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/lab_rw.x dised/lab_rw.s -gsrc/lab_rw.lab
;=============================================

	.include	doscall.mac
	.include	iocscall.mac
	.include	fefunc.mac

	.cpu	68000

	.text

L000000:
	moveq	#0,d0
	move	(L00000e,pc,d0.w),d0
	nop
	jsr	(L00000e,pc,d0.w)
	DOS	_EXIT

L00000e:
	.dc	L000018-L00000e
	.dc	L00001a-L00000e

L000012:
	ori.b	#$a,(a0)
	rts

L000018:
	rts

L00001a:
	rts

L00001c:

	.end	L000000
