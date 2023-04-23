.cpu 68020
  fmove.p fp0,(a0){d0}
  fmove.p fp7,(a0){d7}
  nop

  fmove.p fp0,(a0){#-64}
  fmove.p fp0,(a0){#-1}
  fmove.p fp0,(a0){#-0}
  fmove.p fp0,(a0){#-1}
  fmove.p fp0,(a0){#-63}

  .dc $ff00  
