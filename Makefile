# Makefile for dis (convert source code from UTF-8 to Shift_JIS)
#   Do not use non-ASCII characters in this file.

MKDIR_P = mkdir -p
U8TOSJ = u8tosj

SRC_DIR = src
DOC_DIR = docs
BLD_DIR = build


SRCS = $(filter-out $(SRC_DIR)/ports, $(wildcard $(SRC_DIR)/*)) $(wildcard $(SRC_DIR)/avl/*)
SJ_SRCS = $(subst $(SRC_DIR)/,$(BLD_DIR)/,$(SRCS))

DOCS = $(wildcard $(DOC_DIR)/*)
SJ_DOCS = $(addprefix $(BLD_DIR)/,$(DOCS))


.PHONY: all directories clean

all: directories $(SJ_DOCS) $(SJ_SRCS)

directories: $(BLD_DIR) $(BLD_DIR)/docs $(BLD_DIR)/avl

$(BLD_DIR) $(BLD_DIR)/docs $(BLD_DIR)/avl:
	$(MKDIR_P) $@


$(BLD_DIR)/docs/%: $(DOC_DIR)/%
	$(U8TOSJ) < $^ >! $@

$(BLD_DIR)/%: $(SRC_DIR)/%
	$(U8TOSJ) < $^ >! $@


clean:
	-rm -f $(SJ_DOCS) $(SJ_SRCS)
	-rmdir $(BLD_DIR)/docs $(BLD_DIR)/avl $(BLD_DIR)


# EOF
