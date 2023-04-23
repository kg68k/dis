.cpu 68020
  fmove.b #0,fp0
  fmove.b #1,fp1
  fmove.b #20,fp2
  fmove.w #300,fp3
  fmove.w #4000,fp4
  fmove.l #50000,fp5
  fmove.l #600000,fp6
  fmove.l #7000000,fp7
  rts

  fmovem fp0-fp3,(a0)  ;mode=%10 static, postinc or ctrl
  rts

  .dc $f210,$e0f0  ;NG mode=%00
  rts

  fmovem d0,(a0)  ;%mode=%11 dynamic, postinc or ctrl
  rts

  .dc $f210,$e800  ;NG %mode=%01
  rts

  fmovem fp0-fp3,-(a0)  ;mode=%00 static, predec
  rts

  .dc $f220,$f00f  ;NG %mode=%10
  rts

  fmovem d0,-(a0)  ;mode=%01 dynamic, predec
  rts

  .dc $f220,$f800  ;NG mode=%11
  rts

  fmovem (a0),fp0-fp3  ;mode=%10 static, postinc or ctrl
  rts

  .dc $f210,$c0f0  ;NG %mode=00
  rts

  fmovem (a0),d0  ;mode=%11 dynamic, postinc or ctrl
  rts

  .dc $f210,$c800  ;NG mode=%01
  rts

  fmovem (a0),fp0-fp3  ;mode=%10 static, postinc or ctrl
  rts

  .dc $f210,$c0f0  ;NG mode=%00
  rts

  fmovem (a0),d0  ;mode=%11 dynamic, postinc or ctrl
  rts

  .dc $f210,$c800  ;NG mode=%01
  rts
