CC=gcc
CFLAGS=-I.
DEPS = 
OBJ = chip8.o
OUT = chip8
.PHONY: clean

%.o: %.c $(DEPS) clean
	$(CC) -c -o $@ $< $(CFLAGS)

$(OUT): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

run:
	./$(OUT)

clean:
	rm -f $(OBJ) $(OUT)
