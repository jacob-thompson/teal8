ifeq (run, $(firstword $(MAKECMDGOALS)))
  # get arguments for `make run`
  RUN_ARGS := $(wordlist 2, $(words $(MAKECMDGOALS)), $(MAKECMDGOALS))
  $(eval $(RUN_ARGS):;@:)
endif

CC = gcc

CFLAGS = -I.
CFLAGS += -Wall

SDLFLAGS = `sdl2-config --libs --cflags`
ifeq ($(shell uname), Darwin)
CFLAGS += -Wno-unused-command-line-argument
SDLFLAGS += -I/opt/homebrew/Cellar/sdl2/*/include
endif

IDIR = include
_DEPS = emulator.h display.h stack.h timers.h

ifeq ($(OS),Windows_NT)
DEPS = $(patsubst %, $(IDIR)\%, $(_DEPS))
else
DEPS = $(patsubst %, $(IDIR)/%, $(_DEPS))
endif

BDIR = build
_OBJ = emulator.o display.o stack.o chip8.o

ifeq ($(OS),Windows_NT)
OBJ = $(patsubst %, $(BDIR)\%, $(_OBJ))
else
OBJ = $(patsubst %, $(BDIR)/%, $(_OBJ))
endif

ifeq ($(OS),Windows_NT)
OUT = chip8.exe
else
OUT = chip8
endif

.PHONY: clean

$(BDIR)/%.o: src/%.c $(DEPS) clean
	$(CC) -c -o $@ $< $(CFLAGS) $(SDLFLAGS)

$(OUT): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(SDLFLAGS)

run:
	./$(OUT) $(RUN_ARGS)

test:
	./$(OUT) corax+

clean:
ifeq ($(OS), Windows_NT)
	del $(OBJ) $(OUT)
else
	rm -f $(OBJ) $(OUT)
endif
