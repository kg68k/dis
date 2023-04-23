;=============================================
;  Filename obj/cinvpush.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $00000070 byte(s)
;  Data size    $00000000 byte(s)
;  Bss  size    $00000000 byte(s)
;  9 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/cinvpush.x dised/cinvpush_R0.s -R0
;=============================================

	.include	doscall.mac
	.include	iocscall.mac
	.include	fefunc.mac

	.cpu	68000

	.text

L000000:
	.cpu	68040
	cinvl	ic,(a0)
	cinvl	dc,(a1)
	cinvl	bc,(a6)
	cinvl	nc,(sp)
	nop
	rts

L00000c:
	cinvp	ic,(a0)
	cinvp	dc,(a1)
	cinvp	bc,(a6)
	cinvp	nc,(sp)
	nop
	rts

L000018:
	cinva	ic
	cinva	dc
	cinva	bc
	cinva	nc
	nop
	rts

L000024:
	cpushl	ic,(a0)
	cpushl	dc,(a1)
	cpushl	bc,(a6)
	cpushl	nc,(sp)
	nop
	rts

L000030:
	cpushp	ic,(a0)
	cpushp	dc,(a1)
	cpushp	bc,(a6)
	cpushp	nc,(sp)
	nop
	rts

L00003c:
	cpusha	ic
	cpusha	dc
	cpusha	bc
	cpusha	nc
	nop
	rts

L000048:
	cinva	bc
	cinva	bc
	cinva	bc
	cinva	bc
	cinva	bc
	cinva	bc
	cinva	bc
	cinva	bc
	nop
	rts

L00005c:
	cpusha	bc
	cpusha	bc
	cpusha	bc
	cpusha	bc
	cpusha	bc
	cpusha	bc
	cpusha	bc
	cpusha	bc
	nop
	rts

L000070:

	.end	L000000
