.cpu 68020
  nop
  .dc $ff00

table:
  .irp x,10f,11f,12f,13f,14f,15f,16f,17f,18f,19f,20f,21f,22f,23f,24f,25f,26f,27f,28f,29f,30f,31f,32f,33f
    .dc.l x
  .endm


;abs.w
10:
  move ($6800).w,d0
  rts

;abs.l
11:
  move ($beefcafe),d0
  move (a).l,d0
  rts

;(d16,pc)
12:
  move (pclabel,pc),d0
  rts

;(d8,pc,ix)
13:
  move (pclabel,pc,d0.l),d0
  rts

;(bd,pc,ix)
14:
  move (pclabel.w,pc),d0
  move (pclabel.w,pc,d0.l),d0
  move (pclabel.l,pc),d0
  move (pclabel.l,pc,d0.l),d0
  move (pclabel.l,zpc),d0
  move (pclabel.l,zpc,d0.l),d0
  rts

  bra @f
pclabel: .dc $cafe
a: .dc.l $12345678
@@:


;#imm
15:
  move #$6800,d0
  move.l #$beefcafe,d0
  move.l #a,d0
  rts

;(bd,an,ix)
16:
  move (a0,d0.l),d0
  move ($6800,a0,d0.l),d0
  move ($beefcafe,a0,d0.l),d0
  move (a,a0,d0.l),d0
  rts

;([an],ix,od)
17:
  move ([za0],d0.l),d0
  move ([za0],d0.l,$6800),d0
  move ([za0],d0.l,$beefcafe),d0
  move ([za0],d0.l,a),d0
  move ([za0]),d0
  move ([za0],$6800),d0
  move ([za0],$beefcafe),d0
  move ([za0],a),d0
  move ([a0],d0.l),d0
  move ([a0],d0.l,$6800),d0
  move ([a0],d0.l,$beefcafe),d0
  move ([a0],d0.l,a),d0
  move ([a0]),d0
  move ([a0],$6800),d0
  move ([a0],$beefcafe),d0
  move ([a0],a),d0
  rts

;([bd.w,an],ix,od)
18:
  move ([$6800,za0],d0.l),d0
  move ([$6800,za0],d0.l,$6800),d0
  move ([$6800,za0],d0.l,$beefcafe),d0
  move ([$6800,za0],d0.l,a),d0
  move ([$6800,za0]),d0
  move ([$6800,za0],$6800),d0
  move ([$6800,za0],$beefcafe),d0
  move ([$6800,za0],a),d0
  move ([$6800,a0],d0.l),d0
  move ([$6800,a0],d0.l,$6800),d0
  move ([$6800,a0],d0.l,$beefcafe),d0
  move ([$6800,a0],d0.l,a),d0
  move ([$6800,a0]),d0
  move ([$6800,a0],$6800),d0
  move ([$6800,a0],$beefcafe),d0
  move ([$6800,a0],a),d0
  rts

;([bd.l,an],ix,od)
19:
  move ([$beefcafe,za0],d0.l),d0
  move ([$beefcafe,za0],d0.l,$6800),d0
  move ([$beefcafe,za0],d0.l,$beefcafe),d0
  move ([$beefcafe,za0],d0.l,a),d0
  move ([$beefcafe,za0]),d0
  move ([$beefcafe,za0],$6800),d0
  move ([$beefcafe,za0],$beefcafe),d0
  move ([$beefcafe,za0],a),d0
  move ([$beefcafe,a0],d0.l),d0
  move ([$beefcafe,a0],d0.l,$6800),d0
  move ([$beefcafe,a0],d0.l,$beefcafe),d0
  move ([$beefcafe,a0],d0.l,a),d0
  move ([$beefcafe,a0]),d0
  move ([$beefcafe,a0],$6800),d0
  move ([$beefcafe,a0],$beefcafe),d0
  move ([$beefcafe,a0],a),d0
  rts

;([bd.l,an],ix,od) label
20:
  move ([a,za0],d0.l),d0
  move ([a,za0],d0.l,$6800),d0
  move ([a,za0],d0.l,$beefcafe),d0
  move ([a,za0],d0.l,a),d0
  move ([a,za0]),d0
  move ([a,za0],$6800),d0
  move ([a,za0],$beefcafe),d0
  move ([a,za0],a),d0
  move ([a,a0],d0.l),d0
  move ([a,a0],d0.l,$6800),d0
  move ([a,a0],d0.l,$beefcafe),d0
  move ([a,a0],d0.l,a),d0
  move ([a,a0]),d0
  move ([a,a0],$6800),d0
  move ([a,a0],$beefcafe),d0
  move ([a,a0],a),d0
  rts

;([an,ix],od)
21:
  move ([za0,d0.l]),d0
  move ([za0,d0.l],$6800),d0
  move ([za0,d0.l],$beefcafe),d0
  move ([za0,d0.l],a),d0
  move ([za0]),d0
  move ([za0],$6800),d0
  move ([za0],$beefcafe),d0
  move ([za0],a),d0
  move ([a0,d0.l]),d0
  move ([a0,d0.l],$6800),d0
  move ([a0,d0.l],$beefcafe),d0
  move ([a0,d0.l],a),d0
  move ([a0]),d0
  move ([a0],$6800),d0
  move ([a0],$beefcafe),d0
  move ([a0],a),d0
  rts

;([bd.w,an],ix,od)
22:
  move ([$6800,za0,d0.l]),d0
  move ([$6800,za0,d0.l],$6800),d0
  move ([$6800,za0,d0.l],$beefcafe),d0
  move ([$6800,za0,d0.l],a),d0
  move ([$6800,za0]),d0
  move ([$6800,za0],$6800),d0
  move ([$6800,za0],$beefcafe),d0
  move ([$6800,za0],a),d0
  move ([$6800,a0,d0.l]),d0
  move ([$6800,a0,d0.l],$6800),d0
  move ([$6800,a0,d0.l],$beefcafe),d0
  move ([$6800,a0,d0.l],a),d0
  move ([$6800,a0]),d0
  move ([$6800,a0],$6800),d0
  move ([$6800,a0],$beefcafe),d0
  move ([$6800,a0],a),d0
  rts

;([bd.l,an],ix,od)
23:
  move ([$beefcafe,za0,d0.l]),d0
  move ([$beefcafe,za0,d0.l],$6800),d0
  move ([$beefcafe,za0,d0.l],$beefcafe),d0
  move ([$beefcafe,za0,d0.l],a),d0
  move ([$beefcafe,za0]),d0
  move ([$beefcafe,za0],$6800),d0
  move ([$beefcafe,za0],$beefcafe),d0
  move ([$beefcafe,za0],a),d0
  move ([$beefcafe,a0,d0.l]),d0
  move ([$beefcafe,a0,d0.l],$6800),d0
  move ([$beefcafe,a0,d0.l],$beefcafe),d0
  move ([$beefcafe,a0,d0.l],a),d0
  move ([$beefcafe,a0]),d0
  move ([$beefcafe,a0],$6800),d0
  move ([$beefcafe,a0],$beefcafe),d0
  move ([$beefcafe,a0],a),d0
  rts

;([bd.l,an],ix,od) label
24:
  move ([a,za0,d0.l]),d0
  move ([a,za0,d0.l],$6800),d0
  move ([a,za0,d0.l],$beefcafe),d0
  move ([a,za0,d0.l],a),d0
  move ([a,za0]),d0
  move ([a,za0],$6800),d0
  move ([a,za0],$beefcafe),d0
  move ([a,za0],a),d0
  move ([a,a0,d0.l]),d0
  move ([a,a0,d0.l],$6800),d0
  move ([a,a0,d0.l],$beefcafe),d0
  move ([a,a0,d0.l],a),d0
  move ([a,a0]),d0
  move ([a,a0],$6800),d0
  move ([a,a0],$beefcafe),d0
  move ([a,a0],a),d0
  rts

;([pc],ix,od)
25:
  move ([zpc],d0.l),d0
  move ([zpc],d0.l,$6800),d0
  move ([zpc],d0.l,$beefcafe),d0
  move ([zpc],d0.l,a),d0
  move ([zpc]),d0
  move ([zpc],$6800),d0
  move ([zpc],$beefcafe),d0
  move ([zpc],a),d0
  move ([pc],d0.l),d0
  move ([pc],d0.l,$6800),d0
  move ([pc],d0.l,$beefcafe),d0
  move ([pc],d0.l,a),d0
  move ([pc]),d0
  move ([pc],$6800),d0
  move ([pc],$beefcafe),d0
  move ([pc],a),d0
  rts

;([bd.w,pc],ix,od)
26:
  move ([$6800,zpc],d0.l),d0
  move ([$6800,zpc],d0.l,$6800),d0
  move ([$6800,zpc],d0.l,$beefcafe),d0
  move ([$6800,zpc],d0.l,a),d0
  move ([$6800,zpc]),d0
  move ([$6800,zpc],$6800),d0
  move ([$6800,zpc],$beefcafe),d0
  move ([$6800,zpc],a),d0
  rts

;([bd.w,pc],ix,od) label
27:
  move ([a.w,pc],d0.l),d0
  move ([a.w,pc],d0.l,$6800),d0
  move ([a.w,pc],d0.l,$beefcafe),d0
  move ([a.w,pc],d0.l,a),d0
  move ([a.w,pc]),d0
  move ([a.w,pc],$6800),d0
  move ([a.w,pc],$beefcafe),d0
  move ([a.w,pc],a),d0
  rts

;([bd.l,pc],ix,od)
28:
  move ([$beefcafe,zpc],d0.l),d0
  move ([$beefcafe,zpc],d0.l,$6800),d0
  move ([$beefcafe,zpc],d0.l,$beefcafe),d0
  move ([$beefcafe,zpc],d0.l,a),d0
  move ([$beefcafe,zpc]),d0
  move ([$beefcafe,zpc],$6800),d0
  move ([$beefcafe,zpc],$beefcafe),d0
  move ([$beefcafe,zpc],a),d0
  rts

;([bd.l,pc],ix,od) label
29:
  move ([a.l,zpc],d0.l),d0
  move ([a.l,zpc],d0.l,$6800),d0
  move ([a.l,zpc],d0.l,$beefcafe),d0
  move ([a.l,zpc],d0.l,a),d0
  move ([a.l,zpc]),d0
  move ([a.l,zpc],$6800),d0
  move ([a.l,zpc],$beefcafe),d0
  move ([a.l,zpc],a),d0
  move ([a.l,pc],d0.l),d0
  move ([a.l,pc],d0.l,$6800),d0
  move ([a.l,pc],d0.l,$beefcafe),d0
  move ([a.l,pc],d0.l,a),d0
  move ([a.l,pc]),d0
  move ([a.l,pc],$6800),d0
  move ([a.l,pc],$beefcafe),d0
  move ([a.l,pc],a),d0
  rts

;([pc,ix],od)
30:
  move ([zpc,d0.l]),d0
  move ([zpc,d0.l],$6800),d0
  move ([zpc,d0.l],$beefcafe),d0
  move ([zpc,d0.l],a),d0
  move ([zpc]),d0
  move ([zpc],$6800),d0
  move ([zpc],$beefcafe),d0
  move ([zpc],a),d0
  move ([pc,d0.l]),d0
  move ([pc,d0.l],$6800),d0
  move ([pc,d0.l],$beefcafe),d0
  move ([pc,d0.l],a),d0
  move ([pc]),d0
  move ([pc],$6800),d0
  move ([pc],$beefcafe),d0
  move ([pc],a),d0
  rts

;([bd.w,pc],ix,od)
31:
  move ([a.w,pc,d0.l]),d0
  move ([a.w,pc,d0.l],$6800),d0
  move ([a.w,pc,d0.l],$beefcafe),d0
  move ([a.w,pc,d0.l],a),d0
  move ([a.w,pc]),d0
  move ([a.w,pc],$6800),d0
  move ([a.w,pc],$beefcafe),d0
  move ([a.w,pc],a),d0
  rts

;([bd.l,pc],ix,od)
32:
  move ([$beefcafe,zpc,d0.l]),d0
  move ([$beefcafe,zpc,d0.l],$6800),d0
  move ([$beefcafe,zpc,d0.l],$beefcafe),d0
  move ([$beefcafe,zpc,d0.l],a),d0
  move ([$beefcafe,zpc]),d0
  move ([$beefcafe,zpc],$6800),d0
  move ([$beefcafe,zpc],$beefcafe),d0
  move ([$beefcafe,zpc],a),d0
  move ([$beefcafe,pc,d0.l]),d0
  move ([$beefcafe,pc,d0.l],$6800),d0
  move ([$beefcafe,pc,d0.l],$beefcafe),d0
  move ([$beefcafe,pc,d0.l],a),d0
  move ([$beefcafe,pc]),d0
  move ([$beefcafe,pc],$6800),d0
  move ([$beefcafe,pc],$beefcafe),d0
  move ([$beefcafe,pc],a),d0
  rts

;([bd.l,pc],ix,od) label
33:
  move ([a.l,zpc,d0.l]),d0
  move ([a.l,zpc,d0.l],$6800),d0
  move ([a.l,zpc,d0.l],$beefcafe),d0
  move ([a.l,zpc,d0.l],a),d0
  move ([a.l,zpc]),d0
  move ([a.l,zpc],$6800),d0
  move ([a.l,zpc],$beefcafe),d0
  move ([a.l,zpc],a),d0
  move ([a.l,pc,d0.l]),d0
  move ([a.l,pc,d0.l],$6800),d0
  move ([a.l,pc,d0.l],$beefcafe),d0
  move ([a.l,pc,d0.l],a),d0
  move ([a.l,pc]),d0
  move ([a.l,pc],$6800),d0
  move ([a.l,pc],$beefcafe),d0
  move ([a.l,pc],a),d0
  rts
