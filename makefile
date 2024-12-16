# If the first argument is "run"...
ifeq (run,$(firstword $(MAKECMDGOALS)))
  # use the rest as arguments for "run"
  RUN_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  # ...and turn them into do-nothing targets
  $(eval $(RUN_ARGS):;@:)
endif

CC=gcc
CFLAGS=-I. -Wall -Wno-unused-command-line-argument
SDLFLAGS=`sdl2-config --libs --cflags`
SDLFLAGS+=-I/opt/homebrew/Cellar/sdl2/*/include
_DEPS = emulator.h display.h
DEPS = $(patsubst %, include/%, $(_DEPS))
OBJ = build/emulator.o build/display.o build/chip8.o
OUT = bin/chip8
.PHONY: clean

build/%.o: src/%.c $(DEPS) clean
	$(CC) -c -o $@ $< $(CFLAGS) $(SDLFLAGS)

$(OUT): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(SDLFLAGS)

run:
	./$(OUT) $(RUN_ARGS)

test:
	./$(OUT) roms/ibm_logo.ch8

clean:
	rm -f $(OBJ) $(OUT)
