/**
 * RM controller is the entry point that takes the request and processes it, passes it to the DAO layer.  
*/
void rm_controller(char *client_message , int client_sock);

/**
 * Data access layer , interacts with the file system , removes the file on server file system.
*/
void handle_rm_command(int client_sock, const char *remote_path);