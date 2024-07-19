#include "murpkg.h"

int clean()
{
    //fisrt, /usr/bin/rm -rf REPO_PATH/*
    //then, for every even line in REPO_FILE store the line as url and cd REPO_PATH && /usr/bin/git clone url
    if (file_exists(REPO_FILE))
    {
        //printf clean the cache of the repo? (Y/n)
        //get the fisrt char of the input
        //if it is Y or y or \n then do the following else return 0
        printf("clean the cache of the repo? (Y/n) ");
        char c = getchar();
        if (c != 'Y' && c != 'y' && c != '\n')
        {
            return 0;
        }

        char command_rm[2 * PATH_MAX];
        snprintf(command_rm, sizeof(command_rm), "/usr/bin/rm -rf %s/*", REPO_PATH);
        if (system(command_rm) != 0)
        {
            perror("system");
            return 1;
        }

        printf("-----------------\n");
        printf("Cache cleaned\n");
        printf("-----------------\n");

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
                char command_cd_gitclone[2 * PATH_MAX];
                snprintf(command_cd_gitclone, sizeof(command_cd_gitclone), "cd %s && /usr/bin/git clone %s", REPO_PATH, line);
                if (system(command_cd_gitclone) != 0)
                {
                    perror("system");
                    free(line);
                    fclose(file);
                    return 1;
                }
            }
            odd = !odd;
        }

        free(line);
        fclose(file);

        printf("-----------------\n");
        printf("repo ready\n");
        printf("-----------------\n");
    }
    else
    {
        printf("No %s file found\n", REPO_FILE);
    }
    return 0;
}