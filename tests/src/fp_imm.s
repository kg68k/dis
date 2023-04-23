.cpu 68020

SingleZero::  ;e == 0 && m == 0
  fmove.s #!00000000,fp0
  nop
  fmove.s #!80000000,fp0
  rts

SingleDenorm::  ;e == 0 && m != 0
  fmove.s #!00000001,fp0
  fmove.s #!00400000,fp0
  nop
  fmove.s #!80000001,fp0
  fmove.s #!80400000,fp0
  rts

SingleInf::  ;e == max && m == 0
  fmove.s #!7f800000,fp0
  nop
  fmove.s #!ff800000,fp0
  rts

SingleNaN::  ;e == max && m != 0
  fmove.s #!7f800001,fp0
  fmove.s #!7fc00000,fp0
  nop
  fmove.s #!ff800001,fp0
  fmove.s #!ffc00000,fp0
  rts


DoubleZero::  ;e == 0 && m == 0
  fmove.d #!00000000_00000000,fp0
  nop
  fmove.d #!80000000_00000000,fp0
  rts

DoubleDenorm::  ;e == 0 && m != 0
  fmove.d #!00000000_00000001,fp0
  fmove.d #!00000000_80000000,fp0
  fmove.d #!00000001_00000000,fp0
  fmove.d #!00080000_00000000,fp0
  nop
  fmove.d #!80000000_00000001,fp0
  fmove.d #!80000000_80000000,fp0
  fmove.d #!80000001_00000000,fp0
  fmove.d #!80080000_00000000,fp0
  rts

DoubleInf::  ;e == max && m == 0
  fmove.d #!7ff00000_00000000,fp0
  nop
  fmove.d #!fff00000_00000000,fp0
  rts

DoubleNaN::  ;e == max && m != 0
  fmove.d #!7ff00000_00000001,fp0
  fmove.d #!7ff00000_80000000,fp0
  fmove.d #!7ff00001_00000000,fp0
  fmove.d #!7ff80000_00000000,fp0
  nop
  fmove.d #!fff00000_00000001,fp0
  fmove.d #!fff00000_80000000,fp0
  fmove.d #!fff00001_00000000,fp0
  fmove.d #!fff80000_00000000,fp0
  rts


ExtendedZero::  ;e == 0 && m == 0
  fmove.x #!00000000_00000000_00000000,fp0
  nop
  fmove.x #!80000000_00000000_00000000,fp0
  rts

ExtendedDenorm::  ;e == 0 && m != 0
  fmove.x #!00000000_00000000_00000001,fp0
  fmove.x #!00000000_00000000_80000000,fp0
  fmove.x #!00000000_00000001_00000000,fp0
  fmove.x #!00000000_80000000_00000000,fp0
  nop
  fmove.x #!80000000_00000000_00000001,fp0
  fmove.x #!80000000_00000000_80000000,fp0
  fmove.x #!80000000_00000001_00000000,fp0
  fmove.x #!80000000_80000000_00000000,fp0
  rts

ExtendedInf::  ;e == max && m == 0
  fmove.x #!7fff0000_00000000_00000000,fp0
  nop
  fmove.x #!ffff0000_00000000_00000000,fp0
  rts

ExtendedNaN::  ;e == max && m != 0
  fmove.x #!7fff0000_00000000_00000001,fp0
  fmove.x #!7fff0000_00000000_80000000,fp0
  fmove.x #!7fff0000_00000001_00000000,fp0
  fmove.x #!7fff0000_80000000_00000000,fp0
  nop
  rts

ExtendedReservedNotZero::
  fmove.x #!00000001_00000000_00000000,fp0
  fmove.x #!00008000_00000000_00000000,fp0
  nop
  fmove.x #!80000001_00000000_00000000,fp0
  fmove.x #!80008000_00000000_00000000,fp0
  rts

.end
