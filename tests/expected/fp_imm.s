;=============================================
;  Filename obj/fp_imm.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $000002c4 byte(s)
;  Data size    $00000000 byte(s)
;  Bss  size    $00000000 byte(s)
;  14 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/fp_imm.x dised/fp_imm.s
;=============================================

	.include	doscall.mac
	.include	iocscall.mac
	.include	fefunc.mac

	.cpu	68000

	.text

SingleZero::
	.cpu	68040
	fmove.s	#0f0,fp0
	nop
	fmove.s	#0f-0,fp0
	rts

SingleDenorm::
	fmove.s	#!00000001,fp0
	fmove.s	#!00400000,fp0
	nop
	fmove.s	#!80000001,fp0
	fmove.s	#!80400000,fp0
	rts

SingleInf::
	fmove.s	#!7f800000,fp0
	nop
	fmove.s	#!ff800000,fp0
	rts

SingleNaN::
	fmove.s	#!7f800001,fp0
	fmove.s	#!7fc00000,fp0
	nop
	fmove.s	#!ff800001,fp0
	fmove.s	#!ffc00000,fp0
	rts

DoubleZero::
	fmove.d	#0f0,fp0
	nop
	fmove.d	#0f-0,fp0
	rts

DoubleDenorm::
	fmove.d	#!00000000_00000001,fp0
	fmove.d	#!00000000_80000000,fp0
	fmove.d	#!00000001_00000000,fp0
	fmove.d	#!00080000_00000000,fp0
	nop
	fmove.d	#!80000000_00000001,fp0
	fmove.d	#!80000000_80000000,fp0
	fmove.d	#!80000001_00000000,fp0
	fmove.d	#!80080000_00000000,fp0
	rts

DoubleInf::
	fmove.d	#!7ff00000_00000000,fp0
	nop
	fmove.d	#!fff00000_00000000,fp0
	rts

DoubleNaN::
	fmove.d	#!7ff00000_00000001,fp0
	fmove.d	#!7ff00000_80000000,fp0
	fmove.d	#!7ff00001_00000000,fp0
	fmove.d	#!7ff80000_00000000,fp0
	nop
	fmove.d	#!fff00000_00000001,fp0
	fmove.d	#!fff00000_80000000,fp0
	fmove.d	#!fff00001_00000000,fp0
	fmove.d	#!fff80000_00000000,fp0
	rts

ExtendedZero::
	fmove	#0f0,fp0
	nop
	fmove	#0f-0,fp0
	rts

ExtendedDenorm::
	fmove	#!00000000_00000000_00000001,fp0
	fmove	#!00000000_00000000_80000000,fp0
	fmove	#!00000000_00000001_00000000,fp0
	fmove	#!00000000_80000000_00000000,fp0
	nop
	fmove	#!80000000_00000000_00000001,fp0
	fmove	#!80000000_00000000_80000000,fp0
	fmove	#!80000000_00000001_00000000,fp0
	fmove	#!80000000_80000000_00000000,fp0
	rts

ExtendedInf::
	fmove	#!7fff0000_00000000_00000000,fp0
	nop
	fmove	#!ffff0000_00000000_00000000,fp0
	rts

ExtendedNaN::
	fmove	#!7fff0000_00000000_00000001,fp0
	fmove	#!7fff0000_00000000_80000000,fp0
	fmove	#!7fff0000_00000001_00000000,fp0
	fmove	#!7fff0000_80000000_00000000,fp0
	nop
	rts

ExtendedReservedNotZero::
	fmove	#!00000001_00000000_00000000,fp0
	fmove	#!00008000_00000000_00000000,fp0
	nop
	fmove	#!80000001_00000000_00000000,fp0
	fmove	#!80008000_00000000_00000000,fp0
	rts

L0002c4:

	.end	SingleZero
