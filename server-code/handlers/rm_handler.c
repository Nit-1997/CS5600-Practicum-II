#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <libgen.h>
#include "rm_handler.h"


void rm_controller(char *client_message , int client_sock){
    // Extract the remote path and file content from the command:
    char command[10];   // Variable for "GET command"
    char filename[100];  // Variable for "filename"

    // Use sscanf to extract the values
    if (sscanf(client_message, "%s %s\n", command, filename) != 2) {
        fprintf(stderr, "Invalid input format.\n");
        return 1;
    }

    char full_remote_path[1024];
    snprintf(full_remote_path, sizeof(full_remote_path), "server-file-system/%s", filename);

    handle_rm_command(client_sock , full_remote_path);
}

void handle_rm_command(int client_sock, const char *remote_path) {

    char server_message[8196];

    // Attempt to remove the file
    if (remove(remote_path) == 0) {
        // File removal successful, send success message to client
        snprintf(server_message, sizeof(server_message), "File %s removed successfully.\n", remote_path);
    } else {
        // File removal failed, send error message to client
        snprintf(server_message, sizeof(server_message), "Error removing file %s.\n", remote_path);
    }

    // Send the command result to the client
    if (send(client_sock, server_message, strlen(server_message), 0) < 0) {
        perror("Unable to send command result to client\n");
    }
    
}

