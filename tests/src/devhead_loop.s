devhead1:
  .dc.l devhead2
  .dc $8000
  .dc.l strategy1
  .dc.l interrupt1
  .dc.b '/*?--?*/'

strategy1:
  rts
interrupt1:
  rts

devhead2:
  .dc.l devhead3
  .dc $8000
  .dc.l strategy2
  .dc.l interrupt2
  .dc.b '/*?--?*/'

strategy2:
  rts
interrupt2:
  rts

devhead3:
  .dc.l devhead2
  .dc $8000
  .dc.l strategy3
  .dc.l interrupt3
  .dc.b '/*?--?*/'

strategy3:
  rts
interrupt3:
  rts
