;=============================================
;  Filename obj/reltbl.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $0000018e byte(s)
;  Data size    $00000000 byte(s)
;  Bss  size    $00000002 byte(s)
;  69 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/reltbl.x dised/reltbl.s
;=============================================

	.include	doscall.mac
	.include	iocscall.mac
	.include	fefunc.mac

	.cpu	68000

	.text

L000000:
	DOS	_EXIT

MOVEw_JMPJSR::
	move	(L00000a,pc,d0.w),d0
	jmp	(L00000a,pc,d0.w)

L00000a:
	.dc	L00000c-L00000a

L00000c:
	rts

MOVEl_JMPJSR::
	move	(L000016,pc,d0.l),d0
	jmp	(L000016,pc,d0.w)

L000016:
	.dc	L000018-L000016

L000018:
	rts

L00001a:
	move	(L000022,pc,d0.w),d0
	jmp	(L000022,pc,d1.w)

L000022:
	.dc	$0002,$4e75

L000026:
	move	(L000022,pc,d0.w),d0
	jmp	(L00002e,pc,d0.w)

L00002e:
	.dc.b	$00,$02,$4e,$75

LEA_MOVEw_JMPJSR::
	lea	(L00003e,pc),a0
	move	(a0,d0.w),d0
	jmp	(a0,d0.w)

L00003e:
	.dc	L000040-L00003e

L000040:
	rts

LEA_MOVEl_JMPJSR::
	lea	(L00004e,pc),a0
	move	(a0,d0.l),d0
	jmp	(a0,d0.w)

L00004e:
	.dc	L000050-L00004e

L000050:
	rts

L000052:
	lea	(L00005e,pc),a0
	move	(a0,d0.w),d0
	jmp	(a0,d1.w)

L00005e:
	.dc.b	$00,$02,$4e,$75

L000062:
	lea	(L00006e,pc),a0
	move	(a1,d0.w),d0
	jmp	(a0,d0.w)

L00006e:
	.dc.b	$00,$02,$4e,$75

L000072:
	lea	(L00007e,pc),a0
	move	(a0,d0.w),d0
	jmp	(a1,d0.w)

L00007e:
	.dc.b	$00,$02,$4e,$75

LEA_MOVEw_PEA::
	lea	(L000090,pc),a0
	move	(a0,d0.w),d0
	pea	(a0,d0.w)
	rts

L000090:
	.dc	L000092-L000090

L000092:
	rts

LEA_MOVEl_PEA::
	lea	(L0000a2,pc),a0
	move	(a0,d0.l),d0
	pea	(a0,d0.w)
	rts

L0000a2:
	.dc	L0000a4-L0000a2

L0000a4:
	rts

L0000a6:
	lea	(L0000b4,pc),a0
	move	(a0,d0.w),d0
	pea	(a0,d1.w)
	rts

L0000b4:
	.dc.b	$00,$02,$4e,$75

L0000b8:
	lea	(L0000c6,pc),a0
	move	(a1,d0.w),d0
	pea	(a0,d0.w)
	rts

L0000c6:
	.dc.b	$00,$02,$4e,$75

L0000ca:
	lea	(L0000d8,pc),a0
	move	(a0,d0.w),d0
	pea	(a1,d0.w)
	rts

L0000d8:
	.dc.b	$00,$02,$4e,$75

LEA_ADDA::
	lea	(L0000e6,pc),a0
	adda	(L0000e6,pc,d0.w),a0
	rts

L0000e6:
	.dc	L0000e8-L0000e6

L0000e8:
	rts

L0000ea:
	lea	(L0000f4,pc),a0
	adda	(L0000f4,pc,d0.w),a1
	rts

L0000f4:
	.dc	$0002,$4e75

L0000f8:
	lea	(L0000f4,pc),a0
	adda	(L000102,pc,d0.w),a0
	rts

L000102:
	.dc	$0002,$4e75

MOVEw_LEA::
	move	(L000110,pc,d0.w),d0
	lea	(L000110,pc,d0.w),a0
	rts

L000110:
	.dc	L000112-L000110

L000112:
	rts

MOVEl_LEA::
	move	(L00011e,pc,d0.l),d0
	lea	(L00011e,pc,d0.w),a0
	rts

L00011e:
	.dc	L000120-L00011e

L000120:
	rts

L000122:
	move	(L00012c,pc,d0.w),d0
	lea	(L00012c,pc,d1.w),a0
	rts

L00012c:
	.dc	$0002,$4e75

L000130:
	move	(L00012c,pc,d0.w),d0
	lea	(L00013a,pc,d0.w),a0
	rts

L00013a:
	.dc.b	$00,$02,$4e,$75

MOVEw_PEA::
	move	(L000148,pc,d0.w),d0
	pea	(L000148,pc,d0.w)
	rts

L000148:
	.dc	L00014a-L000148

L00014a:
	rts

MOVEl_PEA::
	move	(L000156,pc,d0.l),d0
	pea	(L000156,pc,d0.w)
	rts

L000156:
	.dc	L000158-L000156

L000158:
	rts

L00015a:
	move	(L000164,pc,d0.w),d0
	pea	(L000164,pc,d1.w)
	rts

L000164:
	.dc	$0002,$4e75

L000168:
	move	(L000164,pc,d0.w),d0
	pea	(L000172,pc,d0.w)
	rts

L000172:
	.dc.b	$00,$02,$4e,$75

OFFSET_0::
	move	(L00017e,pc,d0.w),d0
	jmp	(L00017e,pc,d0.w)

L00017e:
	.dc	0
	.dc	L000184-L00017e
L000182:
	.dc.b	$00,$00

L000184:
	rts

IN_BSS::
	move	(L00018e,pc,d0.w),d0
	jmp	(L00018e,pc,d0.w)

	.bss

L00018e:
	.ds	1
L000190:

	.end	L000000
