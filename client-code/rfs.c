#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include "client_lib.h"


int main(int argc, char *argv[])
{
  
  char cmd[812];
  char remote_file_name[812];
  char local_file_name[812];
  char version_info[812];

   printf("%d\n",argc);
  
  if(argc == 5){
    strncpy(cmd, argv[1], sizeof(cmd) - 1);
    cmd[sizeof(cmd) - 1] = '\0';

    strncpy(version_info, argv[4], sizeof(version_info) - 1);
    version_info[sizeof(version_info) - 1] = '\0'; 

    strncpy(local_file_name, argv[3], sizeof(local_file_name) - 1);
    local_file_name[sizeof(local_file_name) - 1] = '\0'; 

    strncpy(remote_file_name, argv[2], sizeof(remote_file_name) - 1);
    remote_file_name[sizeof(remote_file_name) - 1] = '\0';

  }else if(argc == 4){
    // PULL out the command
    strncpy(cmd, argv[1], sizeof(cmd) - 1);
    cmd[sizeof(cmd) - 1] = '\0';

     if(strcmp(cmd , "GET") == 0){
        // GET remote -> local
        strncpy(local_file_name, argv[3], sizeof(local_file_name) - 1);
        local_file_name[sizeof(local_file_name) - 1] = '\0'; 

        strncpy(remote_file_name, argv[2], sizeof(remote_file_name) - 1);
        remote_file_name[sizeof(remote_file_name) - 1] = '\0';
     }else {
        // WRITE local -> remote
        strncpy(local_file_name, argv[2], sizeof(local_file_name) - 1);
        local_file_name[sizeof(local_file_name) - 1] = '\0'; 

        strncpy(remote_file_name, argv[3], sizeof(remote_file_name) - 1);
        remote_file_name[sizeof(remote_file_name) - 1] = '\0';

     }
  }else if(argc == 3){
    //PULL out the command
    strncpy(cmd, argv[1], sizeof(cmd) - 1);
    cmd[sizeof(cmd) - 1] = '\0';

    if(strcmp(cmd , "RM") == 0 || strcmp(cmd , "LS") == 0){
        // All LS and RM cases
        strncpy(remote_file_name, argv[2], sizeof(remote_file_name) - 1);
        remote_file_name[sizeof(remote_file_name) - 1] = '\0';
    }else{
      // WRITE or GET using  1 arg only
        strncpy(local_file_name, argv[2], sizeof(local_file_name) - 1);
        local_file_name[sizeof(local_file_name) - 1] = '\0'; 

        strncpy(remote_file_name, argv[2], sizeof(remote_file_name) - 1);
        remote_file_name[sizeof(remote_file_name) - 1] = '\0';
    }
  }else{
    fprintf(stderr, "Usage: %s WRITE/GET/RM/LS local-file-path remote-file-path\n or just remote-file-path", argv[0]);
    return -1;
  }

  int socket_desc;
  struct sockaddr_in server_addr;

  // Create socket:
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);

  if (socket_desc < 0)
  {
    perror("Unable to create socket");
    return -1;
  }

  printf("Socket created successfully\n");

  // Set port and IP the same as server-side:
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(8081);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  // Send connection request to server:
  if (connect(socket_desc, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
  {
    perror("Unable to connect");
    return -1;
  }

  printf("Connected with server successfully\n");

  if(strcmp(cmd , "WRITE") == 0){
    // Send the "WRITE" command to the server:
    char full_local_path[1024];
    snprintf(full_local_path, sizeof(full_local_path), "client-file-system/%s", local_file_name);
    send_write_command(socket_desc, full_local_path, remote_file_name);
  }else if(strcmp(cmd , "GET") == 0){
    char full_local_path[1024];
    snprintf(full_local_path, sizeof(full_local_path), "client-file-system/%s", local_file_name);
    if(argc == 4){
      send_get_command(socket_desc, full_local_path, remote_file_name, NULL);
    }else{
      send_get_command(socket_desc, full_local_path, remote_file_name , version_info);
    }
  }else if(strcmp(cmd , "RM") == 0) {
    send_rm_command(socket_desc, remote_file_name);
  }else {
    send_ls_command(socket_desc, remote_file_name);
  }

  // Close the socket:
  close(socket_desc);

  return 0;
}



