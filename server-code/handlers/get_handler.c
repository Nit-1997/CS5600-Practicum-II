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
#include <sys/file.h>
#include <libgen.h>
#include "get_handler.h"


void get_controller(char *client_message , int client_sock){
    // Extract the remote path and file content from the command:
    char command[10];   
    char filename[100];  
    char version[20]; 

    // Use sscanf to extract the values
    if (sscanf(client_message, "%s %s\n", command, filename) != 2) {
        fprintf(stderr, "Invalid input format.\n");
        return 1;
    }

    char *versionStart = strchr(client_message, '\n');

    char full_remote_path[1024];
    snprintf(full_remote_path, sizeof(full_remote_path), "server-file-system/%s", filename);


    if (versionStart == NULL) {
        // simple case where you do not specify any specific version
        handle_get_command(client_sock , full_remote_path ,  NULL);
    }else{
        // case in which you specify a specific version
        versionStart++;  // Move to the first character after the newline

        // Copy the version character by character
        size_t i = 0;
        while (versionStart[i] != '\0') {
            version[i] = versionStart[i];
            i++;
        }
        version[i] = '\0';  // Null-terminate the version
        handle_get_command(client_sock , full_remote_path , version);
    }

}

char* findLatestVersion(const char *remote_path){
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

    char new_remote_path[1024];

    do {
        snprintf(versioned_filename, sizeof(versioned_filename), "%s_v%d", basename_result, version);
        // Use a separate variable for the new path
        snprintf(new_remote_path, sizeof(new_remote_path), "%s/%s", dirname_result, versioned_filename);

        version++;
    } while (access(new_remote_path, F_OK) != -1);

    // latest version
    version = version - 2;
    snprintf(versioned_filename, sizeof(versioned_filename), "%s_v%d", basename_result, version);
    // Use a separate variable for the new path
    snprintf(new_remote_path, sizeof(new_remote_path), "%s/%s", dirname_result, versioned_filename);

    // Copy the new path to remote_path
    strcpy(remote_path, new_remote_path);

    return remote_path;
} 

const char* findSpecificVersion(const char *remote_path , const char* version){
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
    char new_remote_path[1024];
    char versioned_filename[256];
    snprintf(versioned_filename, sizeof(versioned_filename), "%s_v%s", basename_result, version);
    snprintf(new_remote_path, sizeof(new_remote_path), "%s/%s", dirname_result, versioned_filename);

    strcpy(remote_path, new_remote_path);

    return remote_path;
} 


void handle_get_command(int client_sock, const char *remote_path , const char* version) {

    char server_message[8196];
    char client_response[8196];

    // Clean buffers:
    memset(server_message, '\0', sizeof(server_message));
    memset(client_response, '\0', sizeof(client_response));

    //update the remote path with specific version
    if(version == NULL){
        strcpy(remote_path, findLatestVersion(remote_path));
    }else{
        strcpy(remote_path, findSpecificVersion(remote_path , version));
    }

    
    // Read the content from the remote file:
    FILE *remote_file = fopen(remote_path, "rb");
    if (remote_file == NULL) {
        perror("Error opening remote file for reading\n");
        return;
    }

    // Use LOCK_SH for a shared (read) lock on the remote file
    int fd = fileno(remote_file);
    if (flock(fd, LOCK_SH) == -1) {
        perror("Error acquiring file lock\n");
        fclose(remote_file);
        return;
    }

    fseek(remote_file, 0, SEEK_END);
    long file_size = ftell(remote_file);
    fseek(remote_file, 0, SEEK_SET);

    char *file_content = malloc(file_size + 1);
    fread(file_content, 1, file_size, remote_file);
    file_content[file_size] = '\0';

    // Release the read lock
    if (flock(fd, LOCK_UN) == -1) {
        perror("Error releasing file lock\n");
        // Handle the error if needed
    }

    fclose(remote_file); // Close the file using fclose

    snprintf(server_message, sizeof(server_message), "%s", file_content);

    // Send the response to the client:
    if (write(client_sock, server_message, strlen(server_message)) < 0)
    {
        perror("Unable to send response to client\n");
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






