/**
 * Write controller is the entry point that takes the request and processes it, passes it to the DAO layer.  
*/
void write_controller(char *client_message , int client_sock);

/**
 * Data access layer , interacts with the file system , writes the file , if same file exists create a new version.
*/
void handle_write_command(int client_sock, const char *remote_path, const char *file_content);