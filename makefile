CC = cc

GIT_VERSION := "$(shell git describe --abbrev=4 --dirty --always --tags)"

CFLAGS = -Wall -Wno-unused-function -DTEAL8VERSION=\"$(GIT_VERSION)\"
LDFLAGS =
LDLIBS =

SDL_CFLAGS = $(shell sdl2-config --cflags)
SDL_LDFLAGS = $(shell sdl2-config --libs)

CURL_CFLAGS = $(shell curl-config --cflags)
CURL_LIBS = $(shell curl-config --libs)

OPENSSL_CFLAGS = $(shell pkg-config --cflags openssl)
OPENSSL_LDFLAGS = $(shell pkg-config --libs openssl)

CFLAGS += $(SDL_CFLAGS) $(CURL_CFLAGS) $(OPENSSL_CFLAGS)
LDFLAGS += $(SDL_LDFLAGS) $(OPENSSL_LDFLAGS)
LDLIBS += $(CURL_LIBS)

IDIR = include
_DEPS = emulator.h cJSON.h file.h display.h audio.h stack.h timers.h
DEPS = $(patsubst %, $(IDIR)/%, $(_DEPS))

BDIR = build
_OBJ = emulator.o cJSON.o file.o display.o audio.o stack.o chip8.o
OBJ = $(patsubst %, $(BDIR)/%, $(_OBJ))

OUT = bin/teal8

.PHONY: clean test

$(BDIR)/%.o: src/%.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OUT): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)

test:
	./$(OUT) roms/test/quirks

clean:
	rm -f $(OBJ) $(OUT)
