NAME=z8synth
CC=cc
CFLAGS=$(INC)
OUT=./out
SRC=./src/binary
INC=-I $(SRC) -I /usr/include/SDL2

default: main

help:
	@echo "usage: make {default|clean|help}"

out_dir:
	mkdir -p $(OUT)

#########
# GLOBALS
#########
$(OUT)/globals.o: $(SRC)/globals.h
	$(CC) $(CFLAGS) -o $@ -c $(SRC)/globals.c

globals: $(OUT)/globals.o out_dir

#######
# Audio
#######
AUDIO_OBJ=$(patsubst $(SRC)/audio/%.c,$(OUT)/audio/%.o,$(wildcard $(SRC)/audio/*.c))

audio_dir:
	mkdir -p $(OUT)/audio

$(AUDIO_OBJ): $(OUT)/audio/%.o : $(SRC)/audio/%.c $(SRC)/audio/%.h audio_dir globals
	$(CC) $(CFLAGS) -o $@ -c $<

audio: $(AUDIO_OBJ)

######
# Cart
######
CART_OBJ=$(patsubst $(SRC)/cart/%.c,$(OUT)/cart/%.o,$(wildcard $(SRC)/cart/*.c))

cart_dir:
	mkdir -p $(OUT)/cart

$(CART_OBJ): $(OUT)/cart/%.o : $(SRC)/cart/%.c $(SRC)/cart/%.h cart_dir globals
	$(CC) $(CFLAGS) -o $@ -c $<

cart: $(CART_OBJ)

#####
# CLI
#####
cli_dir:
	mkdir -p $(OUT)/cli

CLI_OBJ=$(patsubst $(SRC)/cli/%.c,$(OUT)/cli/%.o,$(wildcard $(SRC)/cli/*.c))

$(CLI_OBJ): $(OUT)/cli/%.o : $(SRC)/cli/%.c $(SRC)/cli/%.h cli_dir globals cart
	$(CC) $(CFLAGS) -o $@ -c $<

cli: $(CLI_OBJ)

######
# Math
######
math_dir:
	mkdir -p $(OUT)/math

MATH_OBJ=$(patsubst $(SRC)/math/%.c,$(OUT)/math/%.o,$(wildcard $(SRC)/math/*.c))

$(MATH_OBJ): $(OUT)/math/%.o : $(SRC)/math/%.c $(SRC)/math/%.h math_dir globals cart
	$(CC) $(CFLAGS) -o $@ -c $<

math: $(MATH_OBJ)

########
# Memory
########
memory_dir:
	mkdir -p $(OUT)/memory

MEMORY_OBJ=$(patsubst $(SRC)/memory/%.c,$(OUT)/memory/%.o,$(wildcard $(SRC)/memory/*.c))

$(MEMORY_OBJ): $(OUT)/memory/%.o : $(SRC)/memory/%.c $(SRC)/memory/%.h memory_dir globals cart
	$(CC) $(CFLAGS) -o $@ -c $<

memory: $(MEMORY_OBJ)

#######
# Synth
#######
# HQ Phasors
PHASOR_HQ_DIR=audio/synth/phasors/hq

phasor_hq_dir:
	mkdir -p $(OUT)/$(PHASOR_HQ_DIR)

PHASOR_HQ_OBJ=$(patsubst $(SRC)/$(PHASOR_HQ_DIR)/%.c,$(OUT)/$(PHASOR_HQ_DIR)/%.o,$(wildcard $(SRC)/$(PHASOR_HQ_DIR)/*.c))

$(PHASOR_HQ_OBJ): $(OUT)/$(PHASOR_HQ_DIR)/%.o : $(SRC)/$(PHASOR_HQ_DIR)/%.c $(SRC)/$(PHASOR_HQ_DIR)/%.h phasor_hq_dir
	$(CC) $(CFLAGS) -o $@ -c $<

phasors: $(PHASOR_HQ_OBJ)

# Aliased Oscillators
OSC_ALIASED_DIR=audio/synth/oscillators/aliased

osc_aliased_dir:
	mkdir -p $(OUT)/$(OSC_ALIASED_DIR)

OSC_ALIASED_OBJ=$(patsubst $(SRC)/$(OSC_ALIASED_DIR)/%.c,$(OUT)/$(OSC_ALIASED_DIR)/%.o,$(wildcard $(SRC)/$(OSC_ALIASED_DIR)/*.c))

$(OSC_ALIASED_OBJ): $(OUT)/$(OSC_ALIASED_DIR)/%.o : $(SRC)/$(OSC_ALIASED_DIR)/%.c $(SRC)/$(OSC_ALIASED_DIR)/%.h osc_aliased_dir math
	$(CC) $(CFLAGS) -o $@ -c $<

# HQ Oscillators
OSC_HQ_DIR=audio/synth/oscillators/hq

osc_hq_dir:
	mkdir -p $(OUT)/$(OSC_HQ_DIR)

OSC_HQ_OBJ=$(patsubst $(SRC)/$(OSC_HQ_DIR)/%.c,$(OUT)/$(OSC_HQ_DIR)/%.o,$(wildcard $(SRC)/$(OSC_HQ_DIR)/*.c))

$(OSC_HQ_OBJ): $(OUT)/$(OSC_HQ_DIR)/%.o : $(SRC)/$(OSC_HQ_DIR)/%.c $(SRC)/$(OSC_HQ_DIR)/%.h osc_hq_dir math
	$(CC) $(CFLAGS) -o $@ -c $<

# Oscllator Helpers
OSC_DIR=audio/synth/oscillators

osc_dir:
	mkdir -p $(OUT)/$(OSC_DIR)

OSC_OBJ=$(patsubst $(SRC)/$(OSC_DIR)/%.c,$(OUT)/$(OSC_DIR)/%.o,$(wildcard $(SRC)/$(OSC_DIR)/*.c))

$(OSC_OBJ): $(OUT)/$(OSC_DIR)/%.o : $(SRC)/$(OSC_DIR)/%.c $(SRC)/$(OSC_DIR)/%.h osc_dir globals $(OSC_ALIASED_OBJ) $(OSC_HQ_OBJ)
	$(CC) $(CFLAGS) -o $@ -c $<

oscillators: $(OSC_OBJ)

# Synth helpers
SYNTH_DIR=audio/synth

synth_dir:
	mkdir -p $(OUT)/$(SYNTH_DIR)

SYNTH_OBJ=$(patsubst $(SRC)/$(SYNTH_DIR)/%.c,$(OUT)/$(SYNTH_DIR)/%.o,$(wildcard $(SRC)/$(SYNTH_DIR)/*.c))

$(SYNTH_OBJ): $(OUT)/$(SYNTH_DIR)/%.o : $(SRC)/$(SYNTH_DIR)/%.c $(SRC)/$(SYNTH_DIR)/%.h synth_dir globals audio memory oscillators time
	$(CC) $(CFLAGS) -o $@ -c $<

synth: $(SYNTH_OBJ)

######
# Time
######
time_dir:
	mkdir -p $(OUT)/time

TIME_OBJ=$(patsubst $(SRC)/time/%.c,$(OUT)/time/%.o,$(wildcard $(SRC)/time/*.c))

$(TIME_OBJ): $(OUT)/time/%.o : $(SRC)/time/%.c $(SRC)/time/%.h time_dir
	$(CC) $(CFLAGS) -o $@ -c $<

time: $(TIME_OBJ)

######
# Main
######
$(OUT)/main.o: $(SRC)/main.h out_dir globals
	mkdir -p $(OUT)
	$(CC) $(CFLAGS) -o $@ -c $(SRC)/main.c

main: $(CART_OBJ) $(CLI_OBJ) $(OUT)/main.o $(OUT)/globals.o
	$(CC) $(CFLAGS) -o $(OUT)/$(NAME) $^ -lm -lSDL2

#######
# Clean
#######
clean:
	$(shell find ./$(OUT) -type f -name "*.o" | xargs -I{} rm {})
	rm -r $(OUT)/cart
	rm -r $(OUT)/cli
	rm $(OUT)/$(NAME)
