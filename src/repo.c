#include "murpkg.h"

int repo_init()
{
    if (file_exists(REPO_PATH) == 0)
    {
        if (create_directory(REPO_PATH) != 0)
        {
            perror("mkdir");
            return 1;
        }
    }

    FILE *file = fopen(REPO_FILE, "w");
    if (file == NULL)
    {
        perror("fopen");
        return 1;
    }
    fprintf(file, "MUR\n");
    fprintf(file, "https://github.com/moskensoap/MUR-packages.git\n");
    fclose(file);

    char command_cd_gitclone[4 * PATH_MAX];
    snprintf(command_cd_gitclone, sizeof(command_cd_gitclone), "cd %s && %s -rf ./* && %s clone https://github.com/moskensoap/MUR-packages.git", REPO_PATH, rm_PATH, git_PATH);
    if (system(command_cd_gitclone) != 0)
    {
        perror("system");
        return 1;
    }
    return 0;
}

int repo_list()
{
    if (check_REPO_FILE_existence_and_init() != 0)
    {
        return 1;
    }
    printf("%s contents:\n", REPO_FILE);
    printf("name\t\turl\n");
    printf("--------------------------------------->\n");
    // odd lines are names, even lines are urls, even lines print begin with \t
    FILE *file = fopen(REPO_FILE, "r");
    if (file == NULL)
    {
        perror("fopen");
        return 1;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int odd = 1;
    while ((read = getline(&line, &len, file)) != -1)
    {
        if (odd == 1)
        {
            printf("%s", line);
            odd = 0;
        }
        else
        {
            printf("\t%s", line);
            odd = 1;
        }
    }
    free(line);
    fclose(file);

    printf("--------------------------------------->\n\n");
    printf("Directories in %s:\n", REPO_PATH);
    printf("--------------------------------------->\n");
    // cd REPO_PATH && ls -d */
    char command_cd_ls[3 * PATH_MAX];
    snprintf(command_cd_ls, sizeof(command_cd_ls), "cd %s && %s -d */", REPO_PATH, ls_PATH);
    if (system(command_cd_ls) != 0)
    {
        perror("system");
        return 1;
    }
    printf("--------------------------------------->\n\n");
    return 0;
}

int repo_add(char *name, char *url)
{
    // if name comtains / or . or .. or whitespace, return 1
    if (strchr(name, '/') != NULL || strchr(name, '.') != NULL || strchr(name, ' ') != NULL || strcmp(name, "..") == 0)
    {
        fprintf(stderr, "Error: invalid name\n");
        return 1;
    }

    if (check_REPO_FILE_existence_and_init() != 0)
    {
        return 1;
    }
    FILE *file = fopen(REPO_FILE, "a");
    if (file == NULL)
    {
        perror("fopen");
        return 1;
    }
    fprintf(file, "%s\n", name);
    fprintf(file, "%s\n", url);
    fclose(file);

    char command_cd_gitclone[3 * PATH_MAX + strlen(url)];
    snprintf(command_cd_gitclone, sizeof(command_cd_gitclone), "cd %s && %s clone %s", REPO_PATH, git_PATH, url);
    if (system(command_cd_gitclone) != 0)
    {
        perror("system");
        repo_remove(name);
        return 1;
    }

    char reponame[PATH_MAX];
    if (url_to_reponame(url, reponame) != 0)
    {
        fprintf(stderr, "Error: could not extract reponame from url\n");
        repo_remove(name);
        return 1;
    }
    // combine REPO_PATH and reponame to REPO_PATH_NAME
    char REPO_PATH_NAME[3 * PATH_MAX];
    snprintf(REPO_PATH_NAME, sizeof(REPO_PATH_NAME), "%s/%s", REPO_PATH, reponame);

    if (is_git_repo(REPO_PATH_NAME) == 0)
    {
        fprintf(stderr, "Error: %s is not a git repository\n", REPO_PATH_NAME);
        repo_remove(name);
        return 1;
    }

    return 0;
}

int repo_remove(char *name)
{
    if (check_REPO_FILE_existence_and_init() != 0)
    {
        return 1;
    }
    if (file_exists(TEMP_PATH) == 0)
    {
        if (create_directory(TEMP_PATH) != 0)
        {
            perror("mkdir");
            return 1;
        }
    }

    FILE *file = fopen(REPO_FILE, "r");
    if (file == NULL)
    {
        perror("fopen");
        return 1;
    }

    FILE *temp = fopen(REPO_TEMP, "w");
    if (temp == NULL)
    {
        perror("fopen");
        fclose(file);
        return 1;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int odd = 1;
    int found = 0;
    while ((read = getline(&line, &len, file)) != -1)
    {
        if (line[read - 1] == '\n')
        {
            line[read - 1] = '\0';
        }

        if (odd == 1)
        {
            if (strcmp(line, name) == 0)
            {
                found = 1;
            }
            else
            {
                fprintf(temp, "%s\n", line);
            }
            odd = 0;
        }
        else
        {
            if (found == 1)
            {
                char *last_slash = strrchr(line, '/');
                char *last_dot = strrchr(line, '.');
                if (last_slash != NULL && last_dot != NULL && last_dot > last_slash)
                {
                    char REPO_NAME[PATH_MAX];
                    strncpy(REPO_NAME, last_slash + 1, last_dot - last_slash - 1);
                    REPO_NAME[last_dot - last_slash - 1] = '\0';
                    char command_rm[4 * PATH_MAX];
                    snprintf(command_rm, sizeof(command_rm), "cd %s && %s -rf %s", REPO_PATH, rm_PATH, REPO_NAME);
                    if (system(command_rm) != 0)
                    {
                        perror("system");
                        free(line);
                        fclose(file);
                        fclose(temp);
                        return 1;
                    }
                }
                found = 0;
            }
            else
            {
                fprintf(temp, "%s\n", line);
            }
            odd = 1;
        }
    }

    free(line);
    fclose(file);
    fclose(temp);

    if (remove(REPO_FILE) != 0)
    {
        perror("remove");
        return 1;
    }
    if (rename(REPO_TEMP, REPO_FILE) != 0)
    {
        perror("rename");
        return 1;
    }

    return 0;
}