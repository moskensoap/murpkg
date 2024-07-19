#include "murpkg.h"

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
    char command[10 * PATH_MAX];
    snprintf(command, sizeof(command), "cd %s && git rev-parse --is-inside-work-tree 2>/dev/null", path);

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
                char command_rm_rf[4 * PATH_MAX];
                snprintf(command_rm_rf, sizeof(command_rm_rf), "/usr/bin/rm -rf %s", REPO_PATH_NAME);
                if (system(command_rm_rf) != 0)
                {
                    perror("system");
                    free(line);
                    fclose(file);
                    return 1;
                }

                char command_cd_gitclone[3 * PATH_MAX];
                snprintf(command_cd_gitclone, sizeof(command_cd_gitclone), "cd %s && /usr/bin/git clone %s", REPO_PATH, line);
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

int init()
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