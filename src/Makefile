# Makefile for dis

TARGET		= generic
PROGRAM		= dis
CC		= gcc
DEFAULTCFLAGS	= -Wall -Wextra -Werror -pipe -O3 -funsigned-char
FUNCCFLAGS	=
LD		= $(CC)
LDFLAGS		=
LIBS		=

# FUNCCFLAGS
#   -DHAVE_STRTOX -DHAVE_JCTYPE_H -DNO_PRINTF_LDBL
#   -DQUICK_YES_NO

ifeq ($(findstring human68k,$(MAKE_VERSION)),human68k)
ifeq ($(notdir $(CURDIR)),src)
$(error do not execute make in src directory)
endif

HUMAN68K	= true

PROGRAM		= dis.x
CC		= gcc2
DEFAULTCFLAGS	= -cc1-stack=600000 -Wall -W -Werror -O2 -fforce-addr -funsigned-char
FUNCCFLAGS	= -DHAVE_STRTOX -DHAVE_JCTYPE_H
LDFLAGS		= -Wl,-d_STACK_SIZE=0x40000
LIBS		= -lld -lhdi -lhmem
endif

.phony: all clean index install tags depend

CFLAGS = $(DEFAULTCFLAGS) $(FUNCCFLAGS)

HDRS		= analyze.h \
		  disasm.h \
		  estruct.h \
		  ea.h \
		  eastr.h \
		  etc.h \
		  eval.h \
		  fpconv.h \
		  fpu.h \
		  generate.h \
		  getopt.h \
		  global.h \
		  hex.h \
		  human.h \
		  include.h \
		  label.h \
		  labelfile.h \
		  mmu.h \
		  offset.h \
		  opstr.h \
		  option.h \
		  osk.h \
		  output.h \
		  symbol.h \
		  table.h

SRCS		= analyze.c \
		  analyze2.c \
		  disasm.c \
		  ea.c \
		  eastr.c \
		  etc.c \
		  eval.c \
		  fpconv.c \
		  fpu.c \
		  generate.c \
		  getopt.c \
		  hex.c \
		  human.c \
		  include.c \
		  label.c \
		  labelcheck.c \
		  labelfile.c \
		  main.c \
		  mmu.c \
		  offset.c \
		  opstr.c \
		  option.c \
		  osk.c \
		  output.c \
		  search.c \
		  symbol.c \
		  table.c \
		  version.c

OBJS		= $(addsuffix .o,$(basename $(SRCS)))

all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(LD) $(LDFLAGS) $(LIBS) $(OBJS) -o $(PROGRAM)

install: $(PROGRAM)
	install -s $(PROGRAM) /usr/local/bin

clean:
	rm -f $(PROGRAM) $(OBJS)

index:
	ctags -wx $(HDRS) $(SRCS)

tags: $(HDRS) $(SRCS)
	etags -et $(HDRS) $(SRCS)

depend:
	$(CC) -MM $(CFLAGS) $(SRCS:.y=.c)

ifdef HUMAN68K
PKGTMP = pkgtmp

$(PKGTMP):
	mkdir $@

archive: $(PKGTMP) $(PROGRAM)
	make -C $(PKGTMP) -f ../Package.mk
endif

###
analyze.o: analyze.c analyze.h estruct.h label.h disasm.h etc.h global.h opstr.h \
  offset.h osk.h
analyze2.o: analyze2.c analyze.h estruct.h label.h disasm.h etc.h global.h \
  opstr.h offset.h
disasm.o: disasm.c disasm.h estruct.h ea.h eastr.h etc.h global.h opstr.h hex.h \
  fpu.h mmu.h osk.h
ea.o: ea.c ea.h disasm.h estruct.h eastr.h etc.h global.h opstr.h hex.h fpconv.h \
  mmu.h
eastr.o: eastr.c eastr.h estruct.h etc.h global.h disasm.h opstr.h
etc.o: etc.c etc.h estruct.h global.h disasm.h opstr.h label.h offset.h symbol.h
eval.o: eval.c eval.h estruct.h disasm.h etc.h global.h opstr.h fpconv.h \
  generate.h label.h output.h hex.h labelfile.h offset.h table.h
fpconv.o: fpconv.c fpconv.h estruct.h etc.h global.h disasm.h opstr.h hex.h
fpu.o: fpu.c fpu.h disasm.h estruct.h ea.h eastr.h etc.h global.h opstr.h hex.h
generate.o: generate.c generate.h disasm.h estruct.h label.h output.h eastr.h \
  etc.h global.h opstr.h eval.h fpconv.h hex.h include.h mmu.h offset.h osk.h \
  symbol.h table.h
getopt.o: getopt.c getopt.h
hex.o: hex.c hex.h global.h disasm.h estruct.h opstr.h
human.o: human.c human.h estruct.h analyze.h label.h etc.h global.h disasm.h \
  opstr.h generate.h output.h include.h offset.h symbol.h version.h
include.o: include.c include.h estruct.h etc.h global.h disasm.h opstr.h
label.o: label.c label.h estruct.h etc.h global.h disasm.h opstr.h ./avl/avl.h \
  ./avl/avl.c
labelcheck.o: labelcheck.c analyze.h estruct.h label.h disasm.h etc.h global.h \
  opstr.h offset.h
labelfile.o: labelfile.c analyze.h estruct.h label.h etc.h global.h disasm.h \
  opstr.h symbol.h version.h
main.o: main.c analyze.h estruct.h label.h disasm.h eastr.h etc.h global.h opstr.h \
  generate.h output.h human.h labelfile.h option.h osk.h table.h version.h
mmu.o: mmu.c mmu.h disasm.h estruct.h ea.h eastr.h etc.h global.h opstr.h hex.h
offset.o: offset.c offset.h estruct.h global.h disasm.h opstr.h etc.h
opstr.o: opstr.c opstr.h estruct.h etc.h global.h disasm.h
option.o: option.c disasm.h estruct.h etc.h global.h opstr.h generate.h label.h \
  output.h getopt.h hex.h option.h version.h
osk.o: osk.c osk.h disasm.h estruct.h analyze.h label.h etc.h global.h opstr.h \
  generate.h output.h hex.h include.h symbol.h version.h
output.o: output.c disasm.h estruct.h etc.h global.h opstr.h hex.h output.h
search.o: search.c estruct.h etc.h global.h disasm.h opstr.h label.h offset.h
symbol.o: symbol.c symbol.h estruct.h etc.h global.h disasm.h opstr.h hex.h \
  label.h output.h
table.o: table.c table.h etc.h estruct.h global.h disasm.h opstr.h eval.h label.h \
  labelfile.h offset.h
version.o: version.c version.h

# EOF
