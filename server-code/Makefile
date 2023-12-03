# Makefile for practicum-2-server
.SILENT:

# Compiler and compiler flags
CC = gcc
CFLAGS = -Wall -g

# Source files and target executable
SRC_SERVER = server.c handlers/write_handler.c handlers/get_handler.c handlers/rm_handler.c handlers/ls_handler.c utilities/directory-lib.c
TARGET_SERVER = server 

server: $(SRC_SERVER)
	$(CC) $(CFLAGS) $(SRC_SERVER) -o $(TARGET_SERVER) -I .

rfs: $(SRC_RFS)
	$(CC) $(CFLAGS) $(SRC_RFS) -o $(TARGET_RFS) -I .		

clean:
	rm -f $(TARGET_SERVER)