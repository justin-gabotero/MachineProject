CC = gcc
CFLAGS = -fPIC -shared
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.so)

all: $(OBJ)

%.so: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f *.so

.PHONY: all clean
