#include "murpkg.h"

const char *REPO_PATH = "/home/.mur/murpkg/repo/";
const char *REPO_FILE = "/home/.mur/murpkg/repo.txt";
const char *TEMP_PATH = "/home/.mur/murpkg/temp/";
const char *TEMP_FILE = "/home/.mur/murpkg/temp/repo_temp.txt";

void repo_init()
{
    if (file_exists(REPO_PATH) == 0)
    {
        if (create_directory(REPO_PATH) != 0)
        {
            perror("mkdir");
            exit(EXIT_FAILURE);
        }
    }

    FILE *file = fopen(REPO_FILE, "w");
    if (file == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    fprintf(file, "MUR\n");
    fprintf(file, "https://github.com/moskensoap/MUR-packages.git\n");
    fclose(file);

    char command_cd_gitclone[2 * PATH_MAX];
    snprintf(command_cd_gitclone, sizeof(command_cd_gitclone), "cd %s && rm -rf ./* && git clone https://github.com/moskensoap/MUR-packages.git", REPO_PATH);
    if (system(command_cd_gitclone) != 0)
    {
        perror("system");
        exit(EXIT_FAILURE);
    }
}
void repo_list()
{
    // odd lines are names, even lines are urls, even lines print begin with \t
    FILE *file = fopen(REPO_FILE, "r");
    if (file == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
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
}

void repo_add(char *name, char *url)
{
    FILE *file = fopen(REPO_FILE, "a");
    if (file == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    fprintf(file, "%s\n", name);
    fprintf(file, "%s\n", url);
    fclose(file);

    char command_cd_gitclone[2 * PATH_MAX];
    snprintf(command_cd_gitclone, sizeof(command_cd_gitclone), "cd %s && git clone %s", REPO_PATH, url);
    if (system(command_cd_gitclone) != 0)
    {
        perror("system");
        exit(EXIT_FAILURE);
    }
}

void repo_remove(char *name)
{
    if (file_exists(TEMP_PATH) == 0)
    {
        if (create_directory(TEMP_PATH) != 0)
        {
            perror("mkdir");
            exit(EXIT_FAILURE);
        }
    }

    FILE *file = fopen(REPO_FILE, "r");
    if (file == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    FILE *temp = fopen(TEMP_FILE, "w");
    if (temp == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
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
                    char command_rm[2 * PATH_MAX];
                    snprintf(command_rm, sizeof(command_rm), "cd %s && rm -rf %s", REPO_PATH, REPO_NAME);
                    if (system(command_rm) != 0)
                    {
                        perror("system");
                        exit(EXIT_FAILURE);
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
        exit(EXIT_FAILURE);
    }
    if (rename(TEMP_FILE, REPO_FILE) != 0)
    {
        perror("rename");
        exit(EXIT_FAILURE);
    }
}