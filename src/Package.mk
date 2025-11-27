# Makefile for dis (create archive file)
#  usage: mkdir pkgtmp; make -C pkgtmp -f ../Package.mk

SRC_DIR ?= ../../src
ROOT_DIR = $(SRC_DIR)/..
BUILD_DIR = ..

CP_P = cp -p
U8TOSJ = u8tosj

DIS_ZIP = $(BUILD_DIR)/dis.zip

DIRS = docs include

DOCS_ = analysis.txt CHANGELOG.txt gpl-3.0.txt labelfile.txt README.txt table.txt tablefile.txt
DOCS = $(addprefix docs/,$(DOCS_))

INCLUDES_ = doscall.mac fefunc.mac iocscall.mac sxcall.mac
INCLUDES = $(addprefix include/,$(INCLUDES_))

PROGRAM = dis.x

FILES = $(DOCS) $(INCLUDES) $(PROGRAM)

.PHONY: all

all: $(DIS_ZIP)

$(DIRS):
	mkdir $@

docs/CHANGELOG.txt: $(ROOT_DIR)/CHANGELOG.md
	$(U8TOSJ) < $^ >! $@

docs/gpl-3.0.txt: $(ROOT_DIR)/docs/gpl-3.0.txt
	rm -f $@
	$(CP_P) $^ $@

docs/%: $(ROOT_DIR)/docs/%
	$(U8TOSJ) < $^ >! $@

include/%: $(ROOT_DIR)/include/%
	$(U8TOSJ) < $^ >! $@

$(PROGRAM): $(BUILD_DIR)/$(PROGRAM)
	rm -f $@
	$(CP_P) $^ $@
	strip $@

$(DIS_ZIP): $(DIRS) $(FILES)
	rm -f $@
	zip -9 $@ $^


# EOF
