#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include "client_lib.h"


void send_get_command(int socket_desc, const char *local_path, const char *remote_path)
{
    printf("Will send get command in future ...");
}

void send_rm_command(int socket_desc, const char *remote_path)
{
   printf("Will send rm command in future ...");
}

void send_ls_command(int socket_desc, const char *remote_path)
{
   printf("Will send ls command in future ...");
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