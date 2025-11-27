# Makefile for dis-tests
#   Do not use non-ASCII characters in this file.

AS = has060
ASFLAGS = -w2
LD ?= hlk
LDFLAGS =
DIFF = diff
DIFFFLAGS = -Naur
MKDIR_P = mkdir -p

DIS = ../build/dis.x
DISFLAGS = --deterministic --overwrite -m680x0,68851

SRC_DIR = src
EXP_DIR = expected
OBJ_DIR = obj
DIS_DIR = dised
DIFF_DIR = diff

TEST1 = $(subst $(SRC_DIR)/,,$(basename $(wildcard $(SRC_DIR)/*.s)))
TEST2 = $(TEST1) blankline_B cinvpush_R0 cpu32_mcpu32 pflush040_R0

SRCS = $(addprefix $(SRC_DIR)/,$(addsuffix .s,$(TEST1)))
OBJS = $(addprefix $(OBJ_DIR)/,$(addsuffix .o,$(TEST1)))
BINS = $(addprefix $(OBJ_DIR)/,$(addsuffix .x,$(TEST1)))
DISS = $(addprefix $(DIS_DIR)/,$(addsuffix .s,$(TEST2)))
DIFFS = $(addprefix $(DIFF_DIR)/,$(addsuffix .dif,$(TEST2)))
DIFFS_OK = $(addprefix $(DIFF_DIR)/,$(addsuffix .ok,$(TEST2)))


.PHONY: all rerun exe dis dif clean

.PRECIOUS: $(OBJ_DIR)/%.x $(DIS_DIR)/%.s


all: $(OBJ_DIR) $(DIS_DIR) $(DIFF_DIR) rerun

rerun:
	make dif

exe: $(BINS)

dis: $(DISS)

dif: $(DIFFS_OK)


$(OBJ_DIR) $(DIS_DIR) $(DIFF_DIR):
	$(MKDIR_P) $@


$(OBJ_DIR)/%.x: $(SRC_DIR)/%.s $(OBJ_DIR)
	$(AS) $(ASFLAGS) -o $(OBJ_DIR)/$*.o $<
	$(LD) $(LDFLAGS) -o $@ $(OBJ_DIR)/$*.o


$(DIS_DIR)/avoid_q.s: $(OBJ_DIR)/avoid_q.x $(DIS) $(DIS_DIR)
	$(DIS) $(DISFLAGS) $< $@ -A

$(DIS_DIR)/blankline_B.s: $(OBJ_DIR)/blankline.x $(DIS) $(DIS_DIR)
	$(DIS) $(DISFLAGS) $< $@ -B

$(DIS_DIR)/cinvpush_R0.s: $(OBJ_DIR)/cinvpush.x $(DIS) $(DIS_DIR)
	$(DIS) $(DISFLAGS) $< $@ -R0

$(DIS_DIR)/compress.s: $(OBJ_DIR)/compress.x $(DIS) $(DIS_DIR)
	$(DIS) $(DISFLAGS) $< $@ -W2

$(DIS_DIR)/cpu32_mcpu32.s: $(OBJ_DIR)/cpu32.x $(DIS) $(DIS_DIR)
	$(DIS) $(DISFLAGS) $< $@ -mcpu32

$(DIS_DIR)/devhead_loop.s: $(OBJ_DIR)/devhead_loop.x $(DIS) $(DIS_DIR)
	$(DIS) $(DISFLAGS) $< $@ -d

$(DIS_DIR)/imm_comment.s: $(OBJ_DIR)/imm_comment.x $(DIS) $(DIS_DIR)
	$(DIS) $(DISFLAGS) $< $@ -M

$(DIS_DIR)/lab_rw.s: $(OBJ_DIR)/lab_rw.x $(DIS) $(DIS_DIR) $(SRC_DIR)/lab_rw.lab
	$(DIS) $(DISFLAGS) $< $@ -g$(SRC_DIR)/lab_rw.lab

$(DIS_DIR)/pflush040_R0.s: $(OBJ_DIR)/pflush040.x $(DIS) $(DIS_DIR)
	$(DIS) $(DISFLAGS) $< $@ -R0

$(DIS_DIR)/split_end.s: $(OBJ_DIR)/split_end.x $(DIS) $(DIS_DIR)
	rm -vf $@ $@.000
	$(DIS) $(DISFLAGS) $< $@ -S1
	mv $@.000 $@

$(DIS_DIR)/sym_oorange.s: $(OBJ_DIR)/sym_oorange.x $(DIS) $(DIS_DIR)
	$(DIS) $(DISFLAGS) $< $@ -s2

$(DIS_DIR)/tab_andor.s: $(OBJ_DIR)/tab_andor.x $(DIS) $(DIS_DIR) \
	$(SRC_DIR)/tab_andor.lab $(SRC_DIR)/tab_andor.tab
	$(DIS) $(DISFLAGS) $< $@ -g$(SRC_DIR)/tab_andor.lab -T$(SRC_DIR)/tab_andor.tab

$(DIS_DIR)/tab_lbl_nobreak.s: $(OBJ_DIR)/tab_lbl_nobreak.x $(DIS) $(DIS_DIR) \
	$(SRC_DIR)/tab_lbl_nobreak.lab $(SRC_DIR)/tab_lbl_nobreak.tab
	$(DIS) $(DISFLAGS) $< $@ -g$(SRC_DIR)/tab_lbl_nobreak.lab -T$(SRC_DIR)/tab_lbl_nobreak.tab

$(DIS_DIR)/tab_break_sec.s: $(OBJ_DIR)/tab_break_sec.x $(DIS) $(DIS_DIR) \
	$(SRC_DIR)/tab_break_sec.lab $(SRC_DIR)/tab_break_sec.tab
	$(DIS) $(DISFLAGS) $< $@ -g$(SRC_DIR)/tab_break_sec.lab -T$(SRC_DIR)/tab_break_sec.tab

$(DIS_DIR)/tab_depadrs.s: $(OBJ_DIR)/tab_depadrs.x $(DIS) $(DIS_DIR) \
	$(SRC_DIR)/tab_depadrs.lab $(SRC_DIR)/tab_depadrs.tab
	$(DIS) $(DISFLAGS) $< $@ -g$(SRC_DIR)/tab_depadrs.lab -T$(SRC_DIR)/tab_depadrs.tab

$(DIS_DIR)/tab_even.s: $(OBJ_DIR)/tab_even.x $(DIS) $(DIS_DIR) \
	$(SRC_DIR)/tab_even.lab $(SRC_DIR)/tab_even.tab
	$(DIS) $(DISFLAGS) $< $@ -g$(SRC_DIR)/tab_even.lab -T$(SRC_DIR)/tab_even.tab

$(DIS_DIR)/tab_stuck1.s: $(OBJ_DIR)/tab_stuck1.x $(DIS) $(DIS_DIR) \
	$(SRC_DIR)/tab_stuck1.lab $(SRC_DIR)/tab_stuck1.tab
	rm -vf $@
	-$(DIS) $(DISFLAGS) $< $@ -g$(SRC_DIR)/tab_stuck1.lab -T$(SRC_DIR)/tab_stuck1.tab
	touch $@

$(DIS_DIR)/tab_stuck2.s: $(OBJ_DIR)/tab_stuck2.x $(DIS) $(DIS_DIR) \
	$(SRC_DIR)/tab_stuck2.lab $(SRC_DIR)/tab_stuck2.tab
	rm -vf $@
	-$(DIS) $(DISFLAGS) $< $@ -g$(SRC_DIR)/tab_stuck2.lab -T$(SRC_DIR)/tab_stuck2.tab
	touch $@

$(DIS_DIR)/%.s: $(OBJ_DIR)/%.x $(DIS) $(DIS_DIR)
	$(DIS) $(DISFLAGS) $< $@

$(DIFF_DIR)/%.ok: $(EXP_DIR)/%.s $(DIS_DIR)/%.s $(DIFF_DIR)
	rm -vf $@
	$(DIFF) $(DIFFFLAGS) $(EXP_DIR)/$*.s $(DIS_DIR)/$*.s >! $(DIFF_DIR)/$(*F).dif
	mv $(DIFF_DIR)/$(*F).dif $@


clean:
	-rm $(OBJS) $(BINS) $(DISS) $(DIFFS) $(DIFFS_OK)
	-rmdir $(OBJ_DIR) $(DIS_DIR) $(DIFF_DIR)


# EOF
