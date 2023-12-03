/**
 * Get controller is the entry point that takes the request and processes it, passes it to the DAO layer.  
*/
void get_controller(char *client_message , int client_sock);

/**
 * Data access layer , interacts with the file system , reads the file , and sends the content back to client over the socket.
*/
void handle_get_command(int client_sock, const char *remote_path , const char* version);