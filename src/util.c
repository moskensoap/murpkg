#include "murpkg.h"

/**
 * Checks if a file exists at the specified path.
 *
 * @param path The path of the file to check.
 * @return 1 if the file exists, 0 otherwise.
 */
int file_exists(const char *path) {
    struct stat buffer;
    return (stat(path, &buffer) == 0);
}



/**
 * Creates a directory at the specified path.
 *
 * @param path The path of the directory to be created.
 * @return 0 if the directory is successfully created, -1 otherwise.
 */
int create_directory(const char *path) {
    char tmp[PATH_MAX];
    char *p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp), "%s", path);
    len = strlen(tmp);

    if(tmp[len - 1] == '/') {
        tmp[len - 1] = 0;
    }

    for(p = tmp + 1; *p; p++) {
        if(*p == '/') {
            *p = 0;

            if(mkdir(tmp, S_IRWXU) != 0 && errno != EEXIST) {
                return -1;
            }
            *p = '/';
        }
    }

    if(mkdir(tmp, S_IRWXU) != 0 && errno != EEXIST) {
        return -1;
    }

    return 0;
}