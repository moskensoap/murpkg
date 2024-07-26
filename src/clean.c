#include "murpkg.h"

int clean()
{
    // if TEMP_PATH exists, /usr/bin/rm -rf TEMP_PATH
    if (file_exists(TEMP_PATH))
    {
        printf("Clean up the temporary files? (Y/n) ");
        char c = getchar();
        if (c == 'Y' || c == 'y' || c == '\n')
        {
            char command_rm[3 * PATH_MAX];
            snprintf(command_rm, sizeof(command_rm), "%s -rf %s", rm_PATH, TEMP_PATH);
            if (system(command_rm) != 0)
            {
                printf("Error: %s\n", command_rm);
                return 1;
            }

        printf("\n");
        printf("\033[1;34m===>\033[1;37m Temporary files cleaned\033[0m\n");
        printf("\n");
        }
    }

    // fisrt, /usr/bin/rm -rf REPO_PATH/*
    // then, for every even line in REPO_FILE store the line as url and cd REPO_PATH && /usr/bin/git clone url
    if (file_exists(REPO_FILE))
    {
        // printf clean the cache of the repo? (Y/n)
        // get the fisrt char of the input
        // if it is Y or y or \n then do the following else return 0
        printf("Clean up the build cache? (Y/n) ");
        char c = getchar();
        if (c != 'Y' && c != 'y' && c != '\n')
        {
            return 0;
        }

        char command_rm[3 * PATH_MAX];
        snprintf(command_rm, sizeof(command_rm), "%s -rf %s/*", rm_PATH, REPO_PATH);
        if (system(command_rm) != 0)
        {
            printf("Error: %s\n", command_rm);
            return 1;
        }

        printf("\n");
        printf("\033[1;34m===>\033[1;37m build cache cleaned\033[0m\n");
        printf("\n");

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
            if (line[read - 1] == '\n')
            {
                line[read - 1] = '\0';
            }

            if (odd == 0)
            {
                char command_cd_gitclone[3 * PATH_MAX + strlen(line)];
                snprintf(command_cd_gitclone, sizeof(command_cd_gitclone), "cd %s && %s clone %s", REPO_PATH, git_PATH, line);
                if (system(command_cd_gitclone) != 0)
                {
                    printf("Error: %s\n", command_cd_gitclone);
                    free(line);
                    fclose(file);
                    return 1;
                }
            }
            odd = !odd;
        }

        free(line);
        fclose(file);

        printf("\n");
        printf("\033[1;34m===>\033[1;37m Repositories initialized\033[0m\n");
    }
    else
    {
        printf("File not found: %s\n", REPO_FILE);
        if (init_repo() != 0)
        {
            return 1;
        }
    }

    return 0;
}