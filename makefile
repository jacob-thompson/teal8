# If the first argument is "run"...
ifeq (run,$(firstword $(MAKECMDGOALS)))
  # use the rest as arguments for "run"
  RUN_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  # ...and turn them into do-nothing targets
  $(eval $(RUN_ARGS):;@:)
endif

IDIR=include
BDIR=build
CC=gcc
CFLAGS=-I. -Wall -Wno-unused-command-line-argument
SDLFLAGS=`sdl2-config --libs --cflags`
SDLFLAGS+=-I/opt/homebrew/Cellar/sdl2/*/include
_DEPS=emulator.h display.h stack.h timers.h
DEPS=$(patsubst %, $(IDIR)/%, $(_DEPS))
_OBJ=emulator.o display.o stack.o chip8.o
OBJ=$(patsubst %, $(BDIR)/%, $(_OBJ))
OUT=bin/chip8
.PHONY: clean

$(BDIR)/%.o: src/%.c $(DEPS) clean
	$(CC) -c -o $@ $< $(CFLAGS) $(SDLFLAGS)

$(OUT): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(SDLFLAGS)

run:
	./$(OUT) $(RUN_ARGS)

test:
	./$(OUT) roms/corax+.ch8

clean:
	rm -f $(OBJ) $(OUT)
