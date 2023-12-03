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
#include "ls_handler.h"


void ls_controller(char *client_message , int client_sock){
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

    handle_ls_command(client_sock , full_remote_path);
}

void handle_ls_command(int client_sock, const char *remote_path) {

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
            perror("Error getting current working directory");
            return;
    }

    // Check if the file already exists
    int version = 1;
    char versioned_filename[256];

    // Array to store versioned filenames
    char versioned_filenames[50][256];  // Right now limited to 50 versions only.
    int num_versions = 0;

    char new_remote_path[1024];

    do {
        snprintf(versioned_filename, sizeof(versioned_filename), "%s_v%d", basename_result, version);
        // Use a separate variable for the new path
        snprintf(new_remote_path, sizeof(new_remote_path), "%s/%s", dirname_result, versioned_filename);
        if (access(new_remote_path, F_OK) != -1) {
             // Store the versioned filename in the array only if it exists
             strncpy(versioned_filenames[num_versions], versioned_filename, sizeof(versioned_filenames[num_versions]));
             num_versions++;
        }
        version++;
    } while (access(new_remote_path, F_OK) != -1);

    strcpy(remote_path, new_remote_path);


    // Concatenate versioned filenames and timestamps into server_message using stats library
    snprintf(server_message, sizeof(server_message), "version info:\n");
    for (int i = 0; i < num_versions; i++) {
        struct stat st;
        // building the fullpath using dirname and filename for stats function.
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", dirname_result, versioned_filenames[i]);

        if (stat(full_path, &st) == 0) {
            char timestamp[20];
            strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&st.st_mtime));
            strncat(server_message, versioned_filenames[i], sizeof(server_message) - strlen(server_message) - 1);
            strncat(server_message, " - Last modified: ", sizeof(server_message) - strlen(server_message) - 1);
            strncat(server_message, timestamp, sizeof(server_message) - strlen(server_message) - 1);
            strncat(server_message, "\n", sizeof(server_message) - strlen(server_message) - 1);
        } else {
            perror("Error getting file information"); // if directory / file version access not granted / does not exist
        }
    }

    // Send the command result to the client
    if (send(client_sock, server_message, strlen(server_message), 0) < 0) {
        perror("Unable to send command result to client\n");
    }
        
}

