#include "murpkg.h"

char TEMP_BIN_PATH[PATH_MAX];

int main() {
    ssize_t count = readlink("/proc/self/exe", murpkgPath, sizeof(murpkgPath) - 1);

    if (count == -1)
    {
        perror("readlink");
        return 1;
    }

    murpkgPath[count] = '\0'; // Null-terminate the murpkgPath

    // change all characters to lowercase in murpkgPath
    for (int i = 0; murpkgPath[i]; i++)
    {
        murpkgPath[i] = tolower(murpkgPath[i]);
    }


    strcpy(TEMP_BIN_PATH, murpkgPath);
    char binPath[PATH_MAX], realPath[PATH_MAX];
    sprintf(binPath, "%s", "struct");
    sprintf(realPath, "%s", "package.bin");

    char *replacePtr = strstr(TEMP_BIN_PATH, binPath);
    if (replacePtr != NULL)
    {
        memcpy(replacePtr, realPath, strlen(realPath));
        replacePtr += strlen(realPath);
        *replacePtr = '\0';
    }
    else
    {
        perror("strstr");
        return 1;
    }






    PackageList pkg_list;

    if (init_package_list(&pkg_list) != 0) {
        printf("init_package_list failed\n");
        return -1;
    }

    if (add_package(&pkg_list, "pkgname1", "1.0", "1", "x86_64", "mingw", "pkgname1-full", "A test package", "/abs/dir", "install commands", 1, 0) != 0) {
        printf("add_package failed\n");
        return -1;
    }

    if (add_dependency(&pkg_list.packages[0], "lib1") != 0) {
        printf("add_dependency failed\n");
        return -1;
    }

    if (add_dependency(&pkg_list.packages[0], "lib2") != 0) {
        printf("add_dependency failed\n");
        return -1;
    }

    if (write_package_to_file(&pkg_list, TEMP_BIN_PATH) != 0) {
        printf("write_package_to_file failed\n");
        return -1;
    }

    PackageList read_pkg_list;
    if (init_package_list(&read_pkg_list) != 0) {
        printf("init_package_list failed\n");
        return -1;
    }

    if (read_package_from_file(&read_pkg_list, TEMP_BIN_PATH) != 0) {
        printf("read_package_from_file failed\n");
        return -1;
    }

    printf("package_count: %d\n", read_pkg_list.package_count);
    for (int i = 0; i < read_pkg_list.package_count; i++) {
        printf("pkgname: %s\n", read_pkg_list.packages[i].pkgname);
        printf("pkgver: %s\n", read_pkg_list.packages[i].pkgver);
        printf("depends_count: %d\n", read_pkg_list.packages[i].depends_count);
        for (int j = 0; j < read_pkg_list.packages[i].depends_count; j++) {
            printf("depends: %s\n", read_pkg_list.packages[i].depends[j]);
        }
    }

    if (free_package_list(&pkg_list) != 0) {
        printf("free_package_list failed\n");
        return -1;
    }

    if (free_package_list(&read_pkg_list) != 0) {
        printf("free_package_list of reading failed\n");
        return -1;
    }

    return 0;
}