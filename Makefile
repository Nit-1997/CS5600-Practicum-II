# Makefile for practicum-2
.SILENT:

# Compiler and compiler flags
CC = gcc
CFLAGS = -Wall -g

# Source files and target executable
SRC_SERVER = server.c write_handler.c
TARGET_SERVER = server 

SRC_RFS = rfs.c client_lib.c
TARGET_RFS = rfs

all: server rfs

server: $(SRC_SERVER)
	$(CC) $(CFLAGS) $(SRC_SERVER) -o $(TARGET_SERVER) -I .

rfs: $(SRC_RFS)
	$(CC) $(CFLAGS) $(SRC_RFS) -o $(TARGET_RFS) -I .		

clean:
	rm -f $(TARGET_SERVER) $(TARGET_RFS)