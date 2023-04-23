;=============================================
;  Filename obj/ea_label.x
;
;  Base address $00000000
;  Exec address $00000000
;  Text size    $00000970 byte(s)
;  Data size    $00000000 byte(s)
;  Bss  size    $00000000 byte(s)
;  30 Labels
;  Commandline dis --deterministic --overwrite -m680x0,68851 obj/ea_label.x dised/ea_label.s
;=============================================

	.include	doscall.mac
	.include	iocscall.mac
	.include	fefunc.mac

	.cpu	68000

	.text

L000000:
	nop
	DOS	_EXIT

L000004:
	.dc.l	L000064
	.dc.l	L00006a
	.dc.l	L000078
	.dc.l	L00007e
	.dc.l	L000084
	.dc.l	L0000b8
	.dc.l	L0000ca
	.dc.l	L0000e6
	.dc.l	L000150
	.dc.l	L0001da
	.dc.l	L000284
	.dc.l	L00032e
	.dc.l	L000398
	.dc.l	L000422
	.dc.l	L0004cc
	.dc.l	L000576
	.dc.l	L0005e0
	.dc.l	L000626
	.dc.l	L00066c
	.dc.l	L0006c2
	.dc.l	L00076c
	.dc.l	L0007d6
	.dc.l	L00081c
	.dc.l	L0008c6

L000064:
	move	($6800),d0
	rts

L00006a:
	move	($beefcafe),d0
	move	(L0000b4),d0
	rts

L000078:
	move	(L0000b2,pc),d0
	rts

L00007e:
	move	(L0000b2,pc,d0.l),d0
	rts

L000084:
	move	(L0000b2,pc),d0
	.cpu	68020
	move	(L0000b2.w,pc,d0.l),d0
	move	(L0000b2.l,pc),d0
	move	(L0000b2.l,pc,d0.l),d0
	move	(L0000b2,zpc),d0
	move	(L0000b2,zpc,d0.l),d0
	rts

L0000b0:
	bra	L0000b8

L0000b2:
	.dc	$cafe
L0000b4:
	.dc	$1234,$5678

L0000b8:
	move	#$6800,d0
	move.l	#$beefcafe,d0
	move.l	#L0000b4,d0
	rts

L0000ca:
	move	(a0,d0.l),d0
	move	($6800,a0,d0.l),d0
	move	($beefcafe,a0,d0.l),d0
	move	(L0000b4,a0,d0.l),d0
	rts

L0000e6:
	move	([za0],d0.l),d0
	move	([za0],d0.l,$6800),d0
	move	([za0],d0.l,$beefcafe),d0
	move	([za0],d0.l,L0000b4),d0
	move	([za0]),d0
	move	([za0],$6800),d0
	move	([za0],$beefcafe),d0
	move	([za0],L0000b4),d0
	move	([a0],d0.l),d0
	move	([a0],d0.l,$6800),d0
	move	([a0],d0.l,$beefcafe),d0
	move	([a0],d0.l,L0000b4),d0
	move	([a0]),d0
	move	([a0],$6800),d0
	move	([a0],$beefcafe),d0
	move	([a0],L0000b4),d0
	rts

L000150:
	move	([$6800],d0.l),d0
	move	([$6800],d0.l,$6800),d0
	move	([$6800],d0.l,$beefcafe),d0
	move	([$6800],d0.l,L0000b4),d0
	move	([$6800]),d0
	move	([$6800],$6800),d0
	move	([$6800],$beefcafe),d0
	move	([$6800],L0000b4),d0
	move	([$6800,a0],d0.l),d0
	move	([$6800,a0],d0.l,$6800),d0
	move	([$6800,a0],d0.l,$beefcafe),d0
	move	([$6800,a0],d0.l,L0000b4),d0
	move	([$6800,a0]),d0
	move	([$6800,a0],$6800),d0
	move	([$6800,a0],$beefcafe),d0
	move	([$6800,a0],L0000b4),d0
	rts

L0001da:
	move	([$beefcafe],d0.l),d0
	move	([$beefcafe],d0.l,$6800),d0
	move	([$beefcafe],d0.l,$beefcafe),d0
	move	([$beefcafe],d0.l,L0000b4),d0
	move	([$beefcafe]),d0
	move	([$beefcafe],$6800),d0
	move	([$beefcafe],$beefcafe),d0
	move	([$beefcafe],L0000b4),d0
	move	([$beefcafe,a0],d0.l),d0
	move	([$beefcafe,a0],d0.l,$6800),d0
	move	([$beefcafe,a0],d0.l,$beefcafe),d0
	move	([$beefcafe,a0],d0.l,L0000b4),d0
	move	([$beefcafe,a0]),d0
	move	([$beefcafe,a0],$6800),d0
	move	([$beefcafe,a0],$beefcafe),d0
	move	([$beefcafe,a0],L0000b4),d0
	rts

L000284:
	move	([L0000b4],d0.l),d0
	move	([L0000b4],d0.l,$6800),d0
	move	([L0000b4],d0.l,$beefcafe),d0
	move	([L0000b4],d0.l,L0000b4),d0
	move	([L0000b4]),d0
	move	([L0000b4],$6800),d0
	move	([L0000b4],$beefcafe),d0
	move	([L0000b4],L0000b4),d0
	move	([L0000b4,a0],d0.l),d0
	move	([L0000b4,a0],d0.l,$6800),d0
	move	([L0000b4,a0],d0.l,$beefcafe),d0
	move	([L0000b4,a0],d0.l,L0000b4),d0
	move	([L0000b4,a0]),d0
	move	([L0000b4,a0],$6800),d0
	move	([L0000b4,a0],$beefcafe),d0
	move	([L0000b4,a0],L0000b4),d0
	rts

L00032e:
	move	([d0.l]),d0
	move	([d0.l],$6800),d0
	move	([d0.l],$beefcafe),d0
	move	([d0.l],L0000b4),d0
	move	([za0]),d0
	move	([za0],$6800),d0
	move	([za0],$beefcafe),d0
	move	([za0],L0000b4),d0
	move	([a0,d0.l]),d0
	move	([a0,d0.l],$6800),d0
	move	([a0,d0.l],$beefcafe),d0
	move	([a0,d0.l],L0000b4),d0
	move	([a0]),d0
	move	([a0],$6800),d0
	move	([a0],$beefcafe),d0
	move	([a0],L0000b4),d0
	rts

L000398:
	move	([$6800,d0.l]),d0
	move	([$6800,d0.l],$6800),d0
	move	([$6800,d0.l],$beefcafe),d0
	move	([$6800,d0.l],L0000b4),d0
	move	([$6800]),d0
	move	([$6800],$6800),d0
	move	([$6800],$beefcafe),d0
	move	([$6800],L0000b4),d0
	move	([$6800,a0,d0.l]),d0
	move	([$6800,a0,d0.l],$6800),d0
	move	([$6800,a0,d0.l],$beefcafe),d0
	move	([$6800,a0,d0.l],L0000b4),d0
	move	([$6800,a0]),d0
	move	([$6800,a0],$6800),d0
	move	([$6800,a0],$beefcafe),d0
	move	([$6800,a0],L0000b4),d0
	rts

L000422:
	move	([$beefcafe,d0.l]),d0
	move	([$beefcafe,d0.l],$6800),d0
	move	([$beefcafe,d0.l],$beefcafe),d0
	move	([$beefcafe,d0.l],L0000b4),d0
	move	([$beefcafe]),d0
	move	([$beefcafe],$6800),d0
	move	([$beefcafe],$beefcafe),d0
	move	([$beefcafe],L0000b4),d0
	move	([$beefcafe,a0,d0.l]),d0
	move	([$beefcafe,a0,d0.l],$6800),d0
	move	([$beefcafe,a0,d0.l],$beefcafe),d0
	move	([$beefcafe,a0,d0.l],L0000b4),d0
	move	([$beefcafe,a0]),d0
	move	([$beefcafe,a0],$6800),d0
	move	([$beefcafe,a0],$beefcafe),d0
	move	([$beefcafe,a0],L0000b4),d0
	rts

L0004cc:
	move	([L0000b4,d0.l]),d0
	move	([L0000b4,d0.l],$6800),d0
	move	([L0000b4,d0.l],$beefcafe),d0
	move	([L0000b4,d0.l],L0000b4),d0
	move	([L0000b4]),d0
	move	([L0000b4],$6800),d0
	move	([L0000b4],$beefcafe),d0
	move	([L0000b4],L0000b4),d0
	move	([L0000b4,a0,d0.l]),d0
	move	([L0000b4,a0,d0.l],$6800),d0
	move	([L0000b4,a0,d0.l],$beefcafe),d0
	move	([L0000b4,a0,d0.l],L0000b4),d0
	move	([L0000b4,a0]),d0
	move	([L0000b4,a0],$6800),d0
	move	([L0000b4,a0],$beefcafe),d0
	move	([L0000b4,a0],L0000b4),d0
	rts

L000576:
	move	([zpc],d0.l),d0
	move	([zpc],d0.l,$6800),d0
	move	([zpc],d0.l,$beefcafe),d0
	move	([zpc],d0.l,L0000b4),d0
	move	([zpc]),d0
	move	([zpc],$6800),d0
	move	([zpc],$beefcafe),d0
	move	([zpc],L0000b4),d0
	move	([pc],d0.l),d0
	move	([pc],d0.l,$6800),d0
	move	([pc],d0.l,$beefcafe),d0
	move	([pc],d0.l,L0000b4),d0
	move	([pc]),d0
	move	([pc],$6800),d0
	move	([pc],$beefcafe),d0
	move	([pc],L0000b4),d0
	rts

L0005e0:
	move	([$6800,zpc],d0.l),d0
	move	([$6800,zpc],d0.l,$6800),d0
	move	([$6800,zpc],d0.l,$beefcafe),d0
	move	([$6800,zpc],d0.l,L0000b4),d0
	move	([$6800,zpc]),d0
	move	([$6800,zpc],$6800),d0
	move	([$6800,zpc],$beefcafe),d0
	move	([$6800,zpc],L0000b4),d0
	rts

L000626:
	move	([L0000b4,pc],d0.l),d0
	move	([L0000b4,pc],d0.l,$6800),d0
	move	([L0000b4,pc],d0.l,$beefcafe),d0
	move	([L0000b4,pc],d0.l,L0000b4),d0
	move	([L0000b4,pc]),d0
	move	([L0000b4,pc],$6800),d0
	move	([L0000b4,pc],$beefcafe),d0
	move	([L0000b4,pc],L0000b4),d0
	rts

L00066c:
	move	([$beefcafe,zpc],d0.l),d0
	move	([$beefcafe,zpc],d0.l,$6800),d0
	move	([$beefcafe,zpc],d0.l,$beefcafe),d0
	move	([$beefcafe,zpc],d0.l,L0000b4),d0
	move	([$beefcafe,zpc]),d0
	move	([$beefcafe,zpc],$6800),d0
	move	([$beefcafe,zpc],$beefcafe),d0
	move	([$beefcafe,zpc],L0000b4),d0
	rts

L0006c2:
	move	([L0000b4,zpc],d0.l),d0
	move	([L0000b4,zpc],d0.l,$6800),d0
	move	([L0000b4,zpc],d0.l,$beefcafe),d0
	move	([L0000b4,zpc],d0.l,L0000b4),d0
	move	([L0000b4,zpc]),d0
	move	([L0000b4,zpc],$6800),d0
	move	([L0000b4,zpc],$beefcafe),d0
	move	([L0000b4,zpc],L0000b4),d0
	move	([L0000b4.l,pc],d0.l),d0
	move	([L0000b4.l,pc],d0.l,$6800),d0
	move	([L0000b4.l,pc],d0.l,$beefcafe),d0
	move	([L0000b4.l,pc],d0.l,L0000b4),d0
	move	([L0000b4.l,pc]),d0
	move	([L0000b4.l,pc],$6800),d0
	move	([L0000b4.l,pc],$beefcafe),d0
	move	([L0000b4.l,pc],L0000b4),d0
	rts

L00076c:
	move	([zpc,d0.l]),d0
	move	([zpc,d0.l],$6800),d0
	move	([zpc,d0.l],$beefcafe),d0
	move	([zpc,d0.l],L0000b4),d0
	move	([zpc]),d0
	move	([zpc],$6800),d0
	move	([zpc],$beefcafe),d0
	move	([zpc],L0000b4),d0
	move	([pc,d0.l]),d0
	move	([pc,d0.l],$6800),d0
	move	([pc,d0.l],$beefcafe),d0
	move	([pc,d0.l],L0000b4),d0
	move	([pc]),d0
	move	([pc],$6800),d0
	move	([pc],$beefcafe),d0
	move	([pc],L0000b4),d0
	rts

L0007d6:
	move	([L0000b4,pc,d0.l]),d0
	move	([L0000b4,pc,d0.l],$6800),d0
	move	([L0000b4,pc,d0.l],$beefcafe),d0
	move	([L0000b4,pc,d0.l],L0000b4),d0
	move	([L0000b4,pc]),d0
	move	([L0000b4,pc],$6800),d0
	move	([L0000b4,pc],$beefcafe),d0
	move	([L0000b4,pc],L0000b4),d0
	rts

L00081c:
	move	([$beefcafe,zpc,d0.l]),d0
	move	([$beefcafe,zpc,d0.l],$6800),d0
	move	([$beefcafe,zpc,d0.l],$beefcafe),d0
	move	([$beefcafe,zpc,d0.l],L0000b4),d0
	move	([$beefcafe,zpc]),d0
	move	([$beefcafe,zpc],$6800),d0
	move	([$beefcafe,zpc],$beefcafe),d0
	move	([$beefcafe,zpc],L0000b4),d0
	move	([L000000-$41102c90,pc,d0.l]),d0
	move	([L000000-$41102c88,pc,d0.l],$6800),d0
	move	([L000000-$41102c7e,pc,d0.l],$beefcafe),d0
	move	([L000000-$41102c72,pc,d0.l],L0000b4),d0
	move	([L000000-$41102c66,pc]),d0
	move	([L000000-$41102c5e,pc],$6800),d0
	move	([L000000-$41102c54,pc],$beefcafe),d0
	move	([L000000-$41102c48,pc],L0000b4),d0
	rts

L0008c6:
	move	([L0000b4,zpc,d0.l]),d0
	move	([L0000b4,zpc,d0.l],$6800),d0
	move	([L0000b4,zpc,d0.l],$beefcafe),d0
	move	([L0000b4,zpc,d0.l],L0000b4),d0
	move	([L0000b4,zpc]),d0
	move	([L0000b4,zpc],$6800),d0
	move	([L0000b4,zpc],$beefcafe),d0
	move	([L0000b4,zpc],L0000b4),d0
	move	([L0000b4.l,pc,d0.l]),d0
	move	([L0000b4.l,pc,d0.l],$6800),d0
	move	([L0000b4.l,pc,d0.l],$beefcafe),d0
	move	([L0000b4.l,pc,d0.l],L0000b4),d0
	move	([L0000b4.l,pc]),d0
	move	([L0000b4.l,pc],$6800),d0
	move	([L0000b4.l,pc],$beefcafe),d0
	move	([L0000b4.l,pc],L0000b4),d0
	rts

L000970:

	.end	L000000
