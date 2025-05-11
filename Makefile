CC=gcc
CFLAGS=-O0 -std=c89 -Wall -Wextra -W -pedantic -I.
CFILES=$(wildcard *.c)
BIN=mpu6050

all:
	$(CC) $(CFLAGS) $(CFILES) -o $(BIN)

clean:
	@rm -f $(BIN)