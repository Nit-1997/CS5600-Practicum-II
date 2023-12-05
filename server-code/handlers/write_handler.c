#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <libgen.h>
#include "write_handler.h"
#include "../utilities/directory-lib.h"


void write_controller(char *client_message , int client_sock){
    // Extract the remote path and file content from the command:
    char command[10];   
    char filename[100];  
    char content[1000];  


    // Use sscanf to extract the values
    if (sscanf(client_message, "%s %s\n", command, filename) != 2) {
        fprintf(stderr, "Invalid input format.\n");
        return 1;
    }

    char *contentStart = strchr(client_message, '\n');
    if (contentStart == NULL) {
        fprintf(stderr, "Invalid input format. No newline found.\n");
        return 1;
    }
    contentStart++;  // Move to the first character after the newline

    // Copy the content character by character
    size_t i = 0;
    while (contentStart[i] != '\0') {
        content[i] = contentStart[i];
        i++;
    }
    content[i] = '\0';  // Null-terminate the content
    
    char full_remote_path[1024];
    snprintf(full_remote_path, sizeof(full_remote_path), "server-file-system/%s", filename);

    // Handle the "WRITE" command:
    handle_write_command(client_sock, full_remote_path, content);
}

void handle_write_command(int client_sock, const char *remote_path, const char *file_content) {
    char server_message[8196];

    // Clean buffer:
    memset(server_message, '\0', sizeof(server_message));

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

    do {

        snprintf(versioned_filename, sizeof(versioned_filename), "%s_v%d", basename_result, version);
        // Use a separate variable for the new path
        char new_remote_path[1024];
        snprintf(new_remote_path, sizeof(new_remote_path), "%s/%s", dirname_result, versioned_filename);

        strcpy(remote_path, new_remote_path);

        version++;
    } while (access(remote_path, F_OK) != -1);

    // Open or create the remote file for writing:
    int remote_file;

    // Create directories if they don't exist
    if (mkdirp(base_directory, dirname_result) == 0) {
        remote_file = open(remote_path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    } else {
        perror("Error creating directories\n");
        strcpy(server_message, "Error creating directories\n");
        remote_file = -1;  // Set remote_file to an invalid value
    }

    if (remote_file >= 0) {
        // Write the content to the remote file:
         
         // Acquire a write lock when writing to the file.
         if (flock(remote_file, LOCK_EX) == -1) {
            perror("Error acquiring file lock\n");
            strcpy(server_message, "Error acquiring file lock\n");
            close(remote_file);
         } else {
            if (write(remote_file, file_content, strlen(file_content)) < 0) {
                perror("Error writing to remote file\n");
                strcpy(server_message, "Error writing to remote file\n");
            } else {
                strcpy(server_message, "File successfully stored on remote server\n");
            }

            // Release the file lock
            if (flock(remote_file, LOCK_UN) == -1) {
                perror("Error releasing file lock\n");
                // Handle the error if needed
            }

            close(remote_file);
         }
    }

    // Respond to the client with the result:
    if (send(client_sock, server_message, strlen(server_message), 0) < 0) {
        perror("Can't send response\n");
    }

    // Print the server's response:
    printf("Server's response: %s\n", server_message);
}


