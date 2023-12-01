# Makefile for practicum-2-client
.SILENT:

# Compiler and compiler flags
CC = gcc
CFLAGS = -Wall -g

# Source files and target executable
SRC_RFS = rfs.c client_lib.c utilities/directory-lib.c
TARGET_RFS = rfs

all: server rfs

rfs: $(SRC_RFS)
	$(CC) $(CFLAGS) $(SRC_RFS) -o $(TARGET_RFS) -I .		

clean:
	rm -f $(TARGET_RFS)