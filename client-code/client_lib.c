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
#include "client_lib.h"
#include "utilities/directory-lib.h"


void send_get_command(int socket_desc, const char *local_path, const char *remote_path , const char* version)
{  
  char client_message[8196];
  char server_response[8196];

  // Clean buffers:
  memset(client_message, '\0', sizeof(client_message));
  memset(server_response, '\0', sizeof(server_response));

  if(version == NULL){
    // Construct the "WRITE" command with remote path and file content:
    snprintf(client_message, sizeof(client_message), "GET %s", remote_path);
  }else{
    snprintf(client_message, sizeof(client_message), "GET %s\n%s", remote_path , version);
  }

  printf("Sending this client message to the server\n");
  fflush(stdout);
  printf("%s" , client_message);
  fflush(stdout);

  // Send the command to the server:
  if (write(socket_desc, client_message, strlen(client_message)) < 0)
  {
    perror("Unable to send command to server");
    return;
  }

  // Receive the server's response:
  if (read(socket_desc, server_response, sizeof(server_response)) < 0)
  {
    perror("Error while receiving server's response");
    return;
  }

  printf("Server's response: %s\n", server_response);

  char dir_path[256];
  char filename[256];

  strncpy(dir_path, local_path, sizeof(dir_path));
  strncpy(filename, local_path, sizeof(filename));

  char *dirname_result = dirname(dir_path);
  char *basename_result = basename(filename);

  // Get the current working directory
  char base_directory[256];
  if (getcwd(base_directory, sizeof(base_directory)) == NULL)
  {
        perror("Error getting current working directory");
        return;
  }

  // Open or create the remote file for writing:
  int local_file;

  // Create directories if they don't exist
  if (mkdirp(base_directory, dirname_result) == 0) {
      local_file = open(local_path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
  } else {
        perror("Error creating directories");
        strcpy(server_response, "Error creating directories");
        local_file = -1;  // Set local_file to an invalid value
    }

    if (local_file >= 0) {
        // Write the content to the local file:
        if (write(local_file, server_response, strlen(server_response)) < 0) {
            perror("Error writing to remote file");
            strcpy(client_message, "Error writing to local file");
        } else {
            strcpy(client_message, "File successfully stored on local file system");
        }

        close(local_file);
    }

    // Respond to the client with the result:
    if (send(socket_desc, client_message, strlen(client_message), 0) < 0) {
        perror("Can't send response\n");
    }

    // Print the server's response:
    printf("Client's message: %s\n", client_message);   
}

void send_rm_command(int socket_desc, const char *remote_path)
{
  char client_message[8196];
  char server_response[8196];

  // Clean buffers:
  memset(client_message, '\0', sizeof(client_message));
  memset(server_response, '\0', sizeof(server_response));


  // Construct the "WRITE" command with remote path and file content:
  snprintf(client_message, sizeof(client_message), "RM %s", remote_path);

  printf("Sending this client message to the server\n");
  printf("%s" , client_message);

  // Send the command to the server:
  if (send(socket_desc, client_message, strlen(client_message), 0) < 0)
  {
    perror("Unable to send command to server");
    return;
  }

  // Receive the server's response:
  if (recv(socket_desc, server_response, sizeof(server_response), 0) < 0)
  {
    perror("Error while receiving server's response");
    return;
  }

  printf("Server's response: %s\n", server_response);
}

void send_ls_command(int socket_desc, const char *remote_path)
{
  char client_message[8196];
  char server_response[8196];

  // Clean buffers:
  memset(client_message, '\0', sizeof(client_message));
  memset(server_response, '\0', sizeof(server_response));


  // Construct the "LS" command with remote path and file content:
  snprintf(client_message, sizeof(client_message), "LS %s", remote_path);

  printf("Sending this client message to the server\n");
  printf("%s" , client_message);

  // Send the command to the server:
  if (send(socket_desc, client_message, strlen(client_message), 0) < 0)
  {
    perror("Unable to send command to server");
    return;
  }

  // Receive the server's response:
  if (recv(socket_desc, server_response, sizeof(server_response), 0) < 0)
  {
    perror("Error while receiving server's response");
    return;
  }

  printf("Server's response: \n%s\n", server_response);
}


void send_write_command(int socket_desc, const char *local_path, const char *remote_path)
{
  char client_message[8196];
  char server_response[8196];

  // Clean buffers:
  memset(client_message, '\0', sizeof(client_message));
  memset(server_response, '\0', sizeof(server_response));

  // Read the content from the local file:
  FILE *local_file = fopen(local_path, "rb");

  if (local_file == NULL)
  {
    perror("Error opening local file");
    return;
  }

  fseek(local_file, 0, SEEK_END);
  long file_size = ftell(local_file);
  fseek(local_file, 0, SEEK_SET);

  char *file_content = malloc(file_size + 1);
  fread(file_content, 1, file_size, local_file);
  fclose(local_file);
  file_content[file_size] = '\0';

  // Construct the "WRITE" command with remote path and file content:
  snprintf(client_message, sizeof(client_message), "WRITE %s\n%s", remote_path, file_content);

  printf("Sending this client message to the server\n");
  printf("%s" , client_message);

  // Send the command to the server:
  if (send(socket_desc, client_message, strlen(client_message), 0) < 0)
  {
    perror("Unable to send command to server");
    free(file_content);
    return;
  }

  // Receive the server's response:
  if (recv(socket_desc, server_response, sizeof(server_response), 0) < 0)
  {
    perror("Error while receiving server's response");
    free(file_content);
    return;
  }

  printf("Server's response: %s\n", server_response);

  free(file_content);
}


