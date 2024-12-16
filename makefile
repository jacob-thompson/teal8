CC=gcc
CFLAGS=-I. -Wall -Wno-unused-command-line-argument
SDLFLAGS=`sdl2-config --libs --cflags`
SDLFLAGS+=-I/opt/homebrew/Cellar/sdl2/*/include
SDLFLAGS+=-I/opt/homebrew/Cellar/sdl2_image/*/include
SDLFLAGS+=-lSDL2_image
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
	./$(OUT)

clean:
	rm -f $(OBJ) $(OUT)
