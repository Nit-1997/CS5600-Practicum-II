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

void *handle_client(void *arg);


int main(void)
{
  int socket_desc, client_sock;
  socklen_t client_size;
  struct sockaddr_in server_addr, client_addr;

  // Create socket:
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_desc < 0)
  {
    perror("Error while creating socket\n");
    return -1;
  }
  printf("Socket created successfully\n");

  // Set port and IP:
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(8081);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  // Bind to the set port and IP:
  if (bind(socket_desc, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
  {
    perror("Error binding to port\n");
    return -1;
  }
  printf("Binding successful\n");

  // Listen for clients:
  if (listen(socket_desc, 5) < 0)
  {
    perror("Error while listening\n");
    return -1;
  }
  printf("\nListening for incoming connections.....\n");

  while (1)
  {
    // Accept an incoming connection:
    client_size = sizeof(client_addr);
    client_sock = accept(socket_desc, (struct sockaddr *)&client_addr, &client_size);

    if (client_sock < 0)
    {
      perror("Can't accept\n");
      return -1;
    }

    // Create a new thread to handle the client:
    pthread_t thread;
    int *client_sock_copy = malloc(sizeof(int));
    if (client_sock_copy == NULL)
    {
      perror("Error allocating memory for client_sock_copy");
      close(client_sock);
      continue;
    }

    *client_sock_copy = client_sock;

    if (pthread_create(&thread, NULL, handle_client, (void *)client_sock_copy) != 0)
    {
      perror("Thread creation failed");
      free(client_sock_copy);
      close(client_sock);
    }
  }

  close(socket_desc);

  return 0;
}

void *handle_client(void *arg)
{
  int client_sock = *((int *)arg);
  free(arg); // Free the memory allocated in the main thread for client_sock_copy

  char client_message[8196];

  // Clean buffer:
  memset(client_message, '\0', sizeof(client_message));

  printf("Client connected at socket %d\n", client_sock);

  // Receive client's message:
  if (recv(client_sock, client_message, sizeof(client_message), 0) < 0)
  {
    perror("Couldn't receive\n");
    close(client_sock);
    return NULL;
  }

  // Check if the received message is a "WRITE" command:
  if (strncmp(client_message, "WRITE", 5) == 0)
  {
     write_controller(client_message , client_sock);
  }
   
  // Closing the socket:
  close(client_sock);

  return NULL;
}


