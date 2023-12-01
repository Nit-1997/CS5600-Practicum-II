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


/**
 * Checks if path exists and manages directory creation.
 * The base_dir parameter specifies the base directory.
 */
int mkdirp(const char *base_dir, const char *path) {
    char *full_path = NULL;
    char *p = NULL;
    char *sp = NULL;
    int status;

    // Combine the base directory and the given path
    if (asprintf(&full_path, "%s/%s", base_dir, path) == -1) {
        perror("Error combining paths");
        return -1;
    }

    status = 0;
    p = full_path;

    while (status == 0 && (sp = strchr(p, '/')) != NULL) {
        if (sp != p) {
            *sp = '\0';
            // Check if the path already exists and is not a directory
            struct stat st;
            if (stat(full_path, &st) != 0 || !S_ISDIR(st.st_mode)) {
                status = mkdir(full_path, S_IRWXU);
            }
            *sp = '/';
        }
        p = sp + 1;
    }

    if (status == 0) {
        // Check if the final path already exists and is not a directory
        struct stat st;
        if (stat(full_path, &st) != 0 || !S_ISDIR(st.st_mode)) {
            status = mkdir(full_path, S_IRWXU);
        }
    }

    free(full_path);
    return status;
}