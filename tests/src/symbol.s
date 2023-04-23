.text
textStart::
  nop
  rts
textEnd::

.data
dataStart::
  .dc.l $12345678
dataEnd::

.bss
bssStart::
  .ds.b 4
bssEnd::

.stack
stackStart::
  .ds.b 1024
stackEnd::

