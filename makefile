CC=gcc
CFLAGS=-I. -Wall -Wno-unused-command-line-argument
SDLFLAGS=`sdl2-config --libs --cflags`
SDLFLAGS+=-I/opt/homebrew/Cellar/sdl2/*/include
SDLFLAGS+=-I/opt/homebrew/Cellar/sdl2_image/*/include
SDLFLAGS+=-lSDL2_image
_DEPS = chip8.h display.h
DEPS = $(patsubst %, include/%, $(_DEPS))
OBJ = build/chip8.o build/display.o build/main.o
OUT = bin/main
.PHONY: clean

build/%.o: src/%.c $(DEPS) clean
	$(CC) -c -o $@ $< $(CFLAGS) $(SDLFLAGS)

$(OUT): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(SDLFLAGS)

run:
	./$(OUT)

clean:
	rm -f $(OBJ) $(OUT)
