# Makefile for compiling NFA.c

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -g

# Source files
SRC = NFA.c
HEADER = NFA.h

# Output executable
OUT = a1

all: $(OUT)

$(OUT): $(SRC) $(HEADER)
	$(CC) $(CFLAGS) -o $(OUT) $(SRC)

clean:
	rm -f $(OUT)
