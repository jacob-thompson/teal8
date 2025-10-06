ifeq (run, $(firstword $(MAKECMDGOALS)))
  # get arguments for `make run`
  RUN_ARGS := $(wordlist 2, $(words $(MAKECMDGOALS)), $(MAKECMDGOALS))
  $(eval $(RUN_ARGS):;@:)
endif

CC = cc

CFLAGS = -Wall -Wno-unused-function
LDFLAGS =
LDLIBS =

SDL_CFLAGS = $(shell sdl2-config --cflags)
SDL_LDFLAGS = $(shell sdl2-config --libs) # adjusts library path and links to SDL2

CURL_CFLAGS = $(shell curl-config --cflags)
CURL_LIBS = $(shell curl-config --libs)

OPENSSL_CFLAGS = $(shell pkg-config --cflags openssl)
OPENSSL_LDFLAGS = $(shell pkg-config --libs openssl)

CFLAGS += $(SDL_CFLAGS) $(CURL_CFLAGS) $(OPENSSL_CFLAGS)
LDFLAGS += $(SDL_LDFLAGS) $(OPENSSL_LDFLAGS)
LDLIBS += $(CURL_LIBS)

IDIR = include
_DEPS = emulator.h cJSON.h file.h display.h stack.h timers.h
DEPS = $(patsubst %, $(IDIR)/%, $(_DEPS))

BDIR = build
_OBJ = emulator.o cJSON.o file.o display.o stack.o chip8.o
OBJ = $(patsubst %, $(BDIR)/%, $(_OBJ))

OUT = chip8

.PHONY: clean run test

$(BDIR)/%.o: src/%.c $(DEPS) clean
	$(CC) $(CFLAGS) -c -o $@ $<

$(OUT): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)

run:
	./$(OUT) $(RUN_ARGS)

test:
	./$(OUT) quirks

clean:
	rm -f $(OBJ) $(OUT)
