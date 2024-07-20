#include "murpkg.h"

char murpkgPath[PATH_MAX];
char murpkgfileName[PATH_MAX];

int initialize_murpkgPath_and_murpkgfileName()
{
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

    strcpy(murpkgfileName, "murpkg");

    return 0;
}

int replaceBinary(char *path, char *fileName, char *realName)
{
    strcpy(path, murpkgPath);
    char binPath[PATH_MAX], realPath[PATH_MAX];
    sprintf(binPath, "/local/bin/%s", fileName);
    sprintf(realPath, "/bin/%s", realName);

    char *replacePtr = strstr(path, binPath);
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

    return 0;
}

int replacePath(char *path, char *fileName, char *realName)
{
    strcpy(path, murpkgPath);
    char binPath[PATH_MAX], realPath[PATH_MAX];
    sprintf(binPath, "/usr/local/bin/%s", fileName);
    sprintf(realPath, "%s", realName);

    char *replacePtr = strstr(path, binPath);
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

    return 0;
}

// ###############################################################
// ###############################################################
// ###############################################################
// ###############################################################
char *concatenate_arguments(int argc, char *argv[])
{
    if (argc < 3)
    {
        return NULL; // Not enough arguments to concatenate
    }

    // Calculate the total length needed for the final string
    size_t total_length = 0;
    for (int i = 2; i < argc; ++i)
    {
        total_length += strlen(argv[i]);
        if (i < argc - 1)
        {
            total_length += 1; // Space between words
        }
    }

    // Allocate memory for the final string
    char *result = (char *)malloc(total_length + 1); // +1 for the null terminator
    if (result == NULL)
    {
        return NULL; // Memory allocation failed
    }

    // Copy and concatenate the arguments into the result
    result[0] = '\0'; // Initialize the result string
    for (int i = 2; i < argc; ++i)
    {
        strcat(result, argv[i]);
        if (i < argc - 1)
        {
            strcat(result, " ");
        }
    }

    return result;
}
// ###############################################################
// ###############################################################
// ###############################################################
// ###############################################################

/**
 * Checks if a file exists at the specified path.
 *
 * @param path The path of the file to check.
 * @return 1 if the file exists, 0 otherwise.
 */
int file_exists(const char *path)
{
    struct stat buffer;
    return (stat(path, &buffer) == 0);
}

/**
 * Creates a directory at the specified path.
 *
 * @param path The path of the directory to be created.
 * @return 0 if the directory is successfully created, -1 otherwise.
 */
int create_directory(const char *path)
{
    char tmp[PATH_MAX];
    char *p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp), "%s", path);
    len = strlen(tmp);

    if (tmp[len - 1] == '/')
    {
        tmp[len - 1] = 0;
    }

    for (p = tmp + 1; *p; p++)
    {
        if (*p == '/')
        {
            *p = 0;

            if (mkdir(tmp, S_IRWXU) != 0 && errno != EEXIST)
            {
                return -1;
            }
            *p = '/';
        }
    }

    if (mkdir(tmp, S_IRWXU) != 0 && errno != EEXIST)
    {
        return -1;
    }

    return 0;
}

/**
 * Checks if a given path is a Git repository.
 *
 * @param path The path to check.
 * @return 1 if the path is a Git repository, 0 otherwise.
 */
int is_git_repo(const char *path)
{
    char command[2 * PATH_MAX + strlen(path)];
    snprintf(command, sizeof(command), "cd %s && %s rev-parse --is-inside-work-tree 2>/dev/null", path, git_PATH);

    FILE *fp = popen(command, "r");
    if (fp == NULL)
    {
        perror("popen");
        return 0;
    }

    char result[16];
    if (fgets(result, sizeof(result), fp) != NULL)
    {
        pclose(fp);
        if (strncmp(result, "true", 4) == 0)
        {
            return 1;
        }
    }

    pclose(fp);
    return 0;
}

/**
 * Extracts the repository name from a given URL.
 *
 * @param url The URL from which to extract the repository name.
 * @param reponame The buffer to store the extracted repository name.
 * @return 0 if the repository name is successfully extracted, -1 otherwise.
 */
int url_to_reponame(const char *url, char *reponame)
{
    // take url's substring to reponame between last / and last .
    char *last_slash = strrchr(url, '/');
    char *last_dot = strrchr(url, '.');
    if (last_slash != NULL && last_dot != NULL && last_dot > last_slash)
    {
        strncpy(reponame, last_slash + 1, last_dot - last_slash - 1);
        reponame[last_dot - last_slash - 1] = '\0';
        return 0;
    }
    return -1;
}

int init_repo()
{
    if (check_REPO_FILE_existence_and_init() != 0)
    {
        return 1;
    }
    if (check_repo_status_and_reclone_if_needed() != 0)
    {
        return 1;
    }
    return 0;
}

int check_REPO_FILE_existence_and_init()
{
    if (file_exists(REPO_FILE) == 0)
    {
        printf("repo.txt not exists, do you want to init? (Y/n) ");
        char c = getchar();
        if (c == 'Y' || c == 'y' || c == '\n')
        {
            if (repo_init() != 0)
            {
                return 1;
            }
            printf("%s is initialized.\n\n", REPO_FILE);
        }
        else
        {
            return 1;
        }
    }

    return 0;
}

int check_repo_status_and_reclone_if_needed()
{
    // check even line of REPO_FILE and store it in url and take its substring between last / and last . and store it in REPO_NAME
    // combine REPO_PATH and REPO_NAME and store it in REPO_PATH_NAME
    // and check if it is a git repo
    // if not, /usr/bin/rm -rf REPO_PATH_NAME and cd REPO_PATH && /usr/bin/git clone url
    FILE *file = fopen(REPO_FILE, "r");
    if (file == NULL)
    {
        perror("fopen");
        return 1;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, file)) != -1)
    {
        if (line[read - 1] == '\n')
        {
            line[read - 1] = '\0';
        }

        char *last_slash = strrchr(line, '/');
        char *last_dot = strrchr(line, '.');
        if (last_slash != NULL && last_dot != NULL && last_dot > last_slash)
        {
            char REPO_NAME[PATH_MAX];
            strncpy(REPO_NAME, last_slash + 1, last_dot - last_slash - 1);
            REPO_NAME[last_dot - last_slash - 1] = '\0';

            char REPO_PATH_NAME[3 * PATH_MAX];
            snprintf(REPO_PATH_NAME, sizeof(REPO_PATH_NAME), "%s/%s", REPO_PATH, REPO_NAME);

            if (is_git_repo(REPO_PATH_NAME) == 0)
            {
                char command_rm_rf[5 * PATH_MAX];
                snprintf(command_rm_rf, sizeof(command_rm_rf), "%s -rf %s", rm_PATH, REPO_PATH_NAME);
                if (system(command_rm_rf) != 0)
                {
                    perror("system");
                    free(line);
                    fclose(file);
                    return 1;
                }

                char command_cd_gitclone[3 * PATH_MAX + strlen(line)];
                snprintf(command_cd_gitclone, sizeof(command_cd_gitclone), "cd %s && %s clone %s", REPO_PATH, git_PATH, line);
                if (system(command_cd_gitclone) != 0)
                {
                    perror("system");
                    free(line);
                    fclose(file);
                    return 1;
                }
            }
        }
    }

    free(line);
    fclose(file);

    return 0;
}

int contain_underline_underline(const char *str)
{
    for (int i = 0; i < strlen(str); i++)
    {
        if (str[i] == '_')
        {
            if (str[i + 1] == '_')
            {
                return 1;
            }
        }
    }
    return 0;
}

// ###############################################################
// ###############################################################
// ###############################################################
// ###############################################################

void init_package(Package *pkg)
{
    pkg->pkgname = NULL;
    pkg->pkgver = NULL;
    pkg->pkgrel = NULL;
    pkg->arch = NULL;
    pkg->mingw_arch = NULL;
    pkg->fullname = NULL;
    pkg->pkgdesc = NULL;
    pkg->abs_dir = NULL;
    pkg->install_cmds = NULL;
    pkg->level = 0;
    pkg->flag = 0;
    pkg->depends = NULL;
    pkg->depends_count = 0;
}

void init_package_list(PackageList *pkg_list)
{
    pkg_list->packages = NULL;
    pkg_list->package_count = 0;
}

void add_package(PackageList *pkg_list, const char *name, const char *version, const char *rel, const char *arch, const char *mingw_arch, const char *fullname, const char *desc, const char *abs_dir, const char *install_cmds, long long int level, int flag)
{
    pkg_list->packages = realloc(pkg_list->packages, (pkg_list->package_count + 1) * sizeof(Package));
    Package *new_pkg = &pkg_list->packages[pkg_list->package_count];
    init_package(new_pkg);
    new_pkg->pkgname = strdup(name);
    new_pkg->pkgver = strdup(version);
    new_pkg->pkgrel = strdup(rel);
    new_pkg->arch = strdup(arch);
    new_pkg->mingw_arch = strdup(mingw_arch);
    new_pkg->fullname = strdup(fullname);
    new_pkg->pkgdesc = strdup(desc);
    new_pkg->abs_dir = strdup(abs_dir);
    new_pkg->install_cmds = strdup(install_cmds);
    new_pkg->level = level;
    new_pkg->flag = flag;
    pkg_list->package_count++;
}

void free_package(Package *pkg)
{
    free(pkg->pkgname);
    free(pkg->pkgver);
    free(pkg->pkgrel);
    free(pkg->arch);
    free(pkg->mingw_arch);
    free(pkg->fullname);
    free(pkg->pkgdesc);
    free(pkg->abs_dir);
    free(pkg->install_cmds);
    for (int i = 0; i < pkg->depends_count; i++)
    {
        free(pkg->depends[i]);
    }
    free(pkg->depends);
}

void free_package_list(PackageList *pkg_list)
{
    for (int i = 0; i < pkg_list->package_count; i++)
    {
        free_package(&pkg_list->packages[i]);
    }
    free(pkg_list->packages);
}

void write_package_to_file(PackageList *pkg_list, const char *filename)
{
    FILE *file = fopen(filename, "wb");
    if (!file)
    {
        perror("Failed to open file for writing");
        return;
    }

    fwrite(&pkg_list->package_count, sizeof(int), 1, file);
    for (int i = 0; i < pkg_list->package_count; i++)
    {
        Package *pkg = &pkg_list->packages[i];

        fwrite(&pkg->level, sizeof(long long int), 1, file);
        fwrite(&pkg->flag, sizeof(int), 1, file);
        fwrite(&pkg->depends_count, sizeof(int), 1, file);

        size_t len = strlen(pkg->pkgname) + 1;
        fwrite(&len, sizeof(size_t), 1, file);
        fwrite(pkg->pkgname, len, 1, file);

        len = strlen(pkg->pkgver) + 1;
        fwrite(&len, sizeof(size_t), 1, file);
        fwrite(pkg->pkgver, len, 1, file);

        len = strlen(pkg->pkgrel) + 1;
        fwrite(&len, sizeof(size_t), 1, file);
        fwrite(pkg->pkgrel, len, 1, file);

        len = strlen(pkg->arch) + 1;
        fwrite(&len, sizeof(size_t), 1, file);
        fwrite(pkg->arch, len, 1, file);

        len = strlen(pkg->mingw_arch) + 1;
        fwrite(&len, sizeof(size_t), 1, file);
        fwrite(pkg->mingw_arch, len, 1, file);

        len = strlen(pkg->fullname) + 1;
        fwrite(&len, sizeof(size_t), 1, file);
        fwrite(pkg->fullname, len, 1, file);

        len = strlen(pkg->pkgdesc) + 1;
        fwrite(&len, sizeof(size_t), 1, file);
        fwrite(pkg->pkgdesc, len, 1, file);

        len = strlen(pkg->abs_dir) + 1;
        fwrite(&len, sizeof(size_t), 1, file);
        fwrite(pkg->abs_dir, len, 1, file);

        len = strlen(pkg->install_cmds) + 1;
        fwrite(&len, sizeof(size_t), 1, file);
        fwrite(pkg->install_cmds, len, 1, file);

        for (int j = 0; j < pkg->depends_count; j++)
        {
            len = strlen(pkg->depends[j]) + 1;
            fwrite(&len, sizeof(size_t), 1, file);
            fwrite(pkg->depends[j], len, 1, file);
        }
    }

    fclose(file);
}

void read_package_from_file(PackageList *pkg_list, const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        perror("Failed to open file for reading");
        return;
    }

    fread(&pkg_list->package_count, sizeof(int), 1, file);
    pkg_list->packages = malloc(pkg_list->package_count * sizeof(Package));

    for (int i = 0; i < pkg_list->package_count; i++)
    {
        Package *pkg = &pkg_list->packages[i];
        init_package(pkg);

        fread(&pkg->level, sizeof(long long int), 1, file);
        fread(&pkg->flag, sizeof(int), 1, file);
        fread(&pkg->depends_count, sizeof(int), 1, file);

        size_t len;
        fread(&len, sizeof(size_t), 1, file);
        pkg->pkgname = malloc(len);
        fread(pkg->pkgname, len, 1, file);

        fread(&len, sizeof(size_t), 1, file);
        pkg->pkgver = malloc(len);
        fread(pkg->pkgver, len, 1, file);

        fread(&len, sizeof(size_t), 1, file);
        pkg->pkgrel = malloc(len);
        fread(pkg->pkgrel, len, 1, file);

        fread(&len, sizeof(size_t), 1, file);
        pkg->arch = malloc(len);
        fread(pkg->arch, len, 1, file);

        fread(&len, sizeof(size_t), 1, file);
        pkg->mingw_arch = malloc(len);
        fread(pkg->mingw_arch, len, 1, file);

        fread(&len, sizeof(size_t), 1, file);
        pkg->fullname = malloc(len);
        fread(pkg->fullname, len, 1, file);

        fread(&len, sizeof(size_t), 1, file);
        pkg->pkgdesc = malloc(len);
        fread(pkg->pkgdesc, len, 1, file);

        fread(&len, sizeof(size_t), 1, file);
        pkg->abs_dir = malloc(len);
        fread(pkg->abs_dir, len, 1, file);

        fread(&len, sizeof(size_t), 1, file);
        pkg->install_cmds = malloc(len);
        fread(pkg->install_cmds, len, 1, file);

        pkg->depends = malloc(pkg->depends_count * sizeof(char *));
        for (int j = 0; j < pkg->depends_count; j++)
        {
            fread(&len, sizeof(size_t), 1, file);
            pkg->depends[j] = malloc(len);
            fread(pkg->depends[j], len, 1, file);
        }
    }

    fclose(file);
}