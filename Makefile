# Makefile for dis (convert source code from UTF-8 to Shift_JIS)
#   Do not use non-ASCII characters in this file.

MKDIR_P = mkdir -p
U8TOSJ = u8tosj

SRC_DIR = src
DOC_DIR = docs
INC_DIR = include
BLD_DIR = build


SRCS = $(filter-out $(SRC_DIR)/avl,$(wildcard $(SRC_DIR)/*)) $(wildcard $(SRC_DIR)/avl/*)
SJ_SRCS = $(subst $(SRC_DIR)/,$(BLD_DIR)/,$(SRCS))

DOCS = $(filter-out $(DOC_DIR)/ports.md,$(wildcard $(DOC_DIR)/*)) $(DOC_DIR)/CHANGELOG.txt
SJ_DOCS = $(addprefix $(BLD_DIR)/,$(DOCS))

INCS = $(wildcard $(INC_DIR)/*)
SJ_INCS = $(addprefix $(BLD_DIR)/,$(INCS))

.PHONY: all directories clean

all: directories $(SJ_DOCS) $(SJ_INCS) $(SJ_SRCS)

directories: $(BLD_DIR) $(BLD_DIR)/avl $(BLD_DIR)/docs $(BLD_DIR)/include

$(BLD_DIR) $(BLD_DIR)/avl $(BLD_DIR)/$(DOC_DIR) $(BLD_DIR)/$(INC_DIR):
	$(MKDIR_P) $@

$(BLD_DIR)/$(DOC_DIR)/CHANGELOG.txt: CHANGELOG.md
	$(U8TOSJ) < $^ >! $@

$(BLD_DIR)/$(DOC_DIR)/%: $(DOC_DIR)/%
	$(U8TOSJ) < $^ >! $@

$(BLD_DIR)/$(INC_DIR)/%: $(INC_DIR)/%
	$(U8TOSJ) < $^ >! $@

$(BLD_DIR)/%: $(SRC_DIR)/%
	$(U8TOSJ) < $^ >! $@


clean:
	-rm -f $(SJ_INCS) $(SJ_DOCS) $(SJ_SRCS)
	-rmdir $(BLD_DIR)/$(INC_DIR) $(BLD_DIR)/$(DOC_DIR) $(BLD_DIR)/avl $(BLD_DIR)


# EOF
