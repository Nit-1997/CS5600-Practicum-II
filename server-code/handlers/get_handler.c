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
#include "get_handler.h"


void get_controller(char *client_message , int client_sock){
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

    handle_get_command(client_sock , full_remote_path);

}

void handle_get_command(int client_sock, const char *remote_path) {

    char server_message[8196];
    char client_response[8196];

    // Clean buffers:
    memset(server_message, '\0', sizeof(server_message));
    memset(client_response, '\0', sizeof(client_response));

    printf("%s\n" , remote_path);
    fflush(stdout);
    // Read the content from the remote file:
    FILE *remote_file = fopen(remote_path, "rb");

    if (remote_file == NULL)
    {
        perror("Error opening remote file\n");
        return;
    }

    fseek(remote_file, 0, SEEK_END);
    long file_size = ftell(remote_file);
    fseek(remote_file, 0, SEEK_SET);

    char *file_content = malloc(file_size + 1);
    fread(file_content, 1, file_size, remote_file);
    fclose(remote_file);
    file_content[file_size] = '\0';
   
    // Construct the "WRITE" command with remote path and file content:
    snprintf(server_message, sizeof(server_message), "%s", file_content);

    printf("Sending this server message to the client\n");
    fflush(stdout);
    printf("%s" , server_message);
    fflush(stdout);

    // Send the command to the server:
    if (write(client_sock, server_message, strlen(server_message)) < 0)
    {
        perror("Unable to send command to server\n");
        free(file_content);
        return;
    }

    // Receive the client's response:
    if (read(client_sock, client_response, sizeof(client_response)) < 0)
    {
        perror("Error while receiving client's response\n");
        free(file_content);
        return;
    }

    printf("client's response: %s\n", client_response);

    free(file_content);
    
}


