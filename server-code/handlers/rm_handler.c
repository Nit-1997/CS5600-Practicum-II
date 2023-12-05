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
    char command[10];   
    char filename[100];  

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


    // Extract directory path and filename
    char dir_path[256];
    char filename[256];

    strncpy(dir_path, remote_path, sizeof(dir_path));
    strncpy(filename, remote_path, sizeof(filename));

    char *dirname_result = dirname(dir_path);
    char *basename_result = basename(filename);

    // Get the current working directory
    char base_directory[256];
    if (getcwd(base_directory, sizeof(base_directory)) == NULL)
    {
            perror("Error getting current working directory\n");
            return;
    }

    // Check if the file already exists
    int version = 1;
    char versioned_filename[256];

    char new_remote_path[1024];

    // go through each version and remove it.
    while(1){
        snprintf(versioned_filename, sizeof(versioned_filename), "%s_v%d", basename_result, version);
        // Use a separate variable for the new path
        snprintf(new_remote_path, sizeof(new_remote_path), "%s/%s", dirname_result, versioned_filename);
    
        if(access(new_remote_path, F_OK) != -1){
            // Attempt to remove the file
            if (remove(new_remote_path) == 0) {
                // File removal successful, send success message to client
                snprintf(server_message, sizeof(server_message), "All File versions removed successfully.\n");
                version++;
            } else {
                // File removal failed, send error message to client
                snprintf(server_message, sizeof(server_message), "Error removing file %s.\n", new_remote_path);
                break;
            }
        }else{
            if(version == 1){
                snprintf(server_message, sizeof(server_message), "File does not exist at %s.\n", new_remote_path);
                break;
            }else{
                break;
            }
        }
    }

    // Send the command result to the client
    if (send(client_sock, server_message, strlen(server_message), 0) < 0) {
        perror("Unable to send command result to client\n");
    }
    
}

