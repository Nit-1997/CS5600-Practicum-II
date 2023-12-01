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
#include "write_handler.h"


void write_controller(char *client_message , int client_sock){
    // Extract the remote path and file content from the command:
    char command[10];   // Variable for "WRITE command"
    char filename[100];  // Variable for "filename"
    char content[1000];  // Variable for the remaining content


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

    // Open or create the remote file for writing:
    int remote_file;

    // Create directories if they don't exist
    if (mkdirp(dirname_result) == 0) {
        remote_file = open(remote_path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    } else {
        perror("Error creating directories");
        strcpy(server_message, "Error creating directories");
        remote_file = -1;  // Set remote_file to an invalid value
    }

    if (remote_file >= 0) {
        // Write the content to the remote file:
        if (write(remote_file, file_content, strlen(file_content)) < 0) {
            perror("Error writing to remote file");
            strcpy(server_message, "Error writing to remote file");
        } else {
            strcpy(server_message, "File successfully stored on remote server");
        }

        close(remote_file);
    }

    // Respond to the client with the result:
    if (send(client_sock, server_message, strlen(server_message), 0) < 0) {
        perror("Can't send response\n");
    }

    // Print the server's response:
    printf("Server's response: %s\n", server_message);
}

/**
 * Checks if path exists and manages directory creation.
*/
int mkdirp(const char *path) {
    char *p = NULL;
    char *sp = NULL;
    int status;

    char *copy = strdup(path);

    status = 0;
    p = copy;

    while (status == 0 && (sp = strchr(p, '/')) != NULL) {
        if (sp != p) {
            *sp = '\0';
            // Check if the path already exists and is not a directory
            struct stat st;
            if (stat(copy, &st) != 0 || !S_ISDIR(st.st_mode)) {
                status = mkdir(copy, S_IRWXU);
            }
            *sp = '/';
        }
        p = sp + 1;
    }

    if (status == 0) {
        // Check if the final path already exists and is not a directory
        struct stat st;
        if (stat(copy, &st) != 0 || !S_ISDIR(st.st_mode)) {
            status = mkdir(copy, S_IRWXU);
        }
    }

    free(copy);
    return status;
}
