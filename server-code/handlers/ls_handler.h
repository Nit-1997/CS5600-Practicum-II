/**
 * LS controller is the entry point that takes the request and processes it, passes it to the DAO layer.  
*/
void ls_controller(char *client_message , int client_sock);

/**
 * Data access layer , interacts with the file system , lists all the file version, parses information and sends it back to client.
*/
void handle_ls_command(int client_sock, const char *remote_path);