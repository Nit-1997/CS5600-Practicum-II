void send_get_command(int socket_desc, const char *local_path, const char *remote_path);
void send_rm_command(int socket_desc, const char *remote_path);
void send_write_command(int socket_desc, const char *local_path, const char *remote_path);
void send_ls_command(int socket_desc, const char *remote_path);