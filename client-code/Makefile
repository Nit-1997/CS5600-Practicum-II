# Makefile for practicum-2-client
.SILENT:

# Compiler and compiler flags
CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -pthread -lm

# Source files and target executable
SRC_RFS = rfs.c client_lib.c utilities/directory-lib.c
TARGET_RFS = rfs

rfs: $(SRC_RFS)
	$(CC) $(CFLAGS) $(SRC_RFS) -o $(TARGET_RFS) -I . $(LDFLAGS)		

clean:
	rm -f $(TARGET_RFS)