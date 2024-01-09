CC=gcc
CFLAG=-O0 -std=c89 -Wall -Wextra -W -pedantic -I.
CFILES=$(wildcard *.c)
BIN=mpu6050

all:
	$(CC) $(CFLAG) $(CFILES) -o $(BIN)

clean:
	rm -rf $(BIN)