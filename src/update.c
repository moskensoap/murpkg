#include "murpkg.h"

int git_pull_repo(const char *path)
{
    char command[4 * PATH_MAX + strlen(path)];
    snprintf(command, sizeof(command), "cd %s && %s reset --hard HEAD > /dev/null 2>&1 && %s clean -fd && %s pull", path, git_PATH, git_PATH, git_PATH);
    if (system(command) != 0)
    {
        perror("system");
        return 1;
    }
    return 0;
}

int update()
{
    // for every REPO_NAME in REPO_FILE do git_pull_repo(REPO_PATH_NAME/REPO_NAME)
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

            printf("Updating %s\n", REPO_NAME);

            if (git_pull_repo(REPO_PATH_NAME) != 0)
            {
                perror("git_pull_repo");
                free(line);
                fclose(file);
                return 1;
            }
        }
    }

    free(line);
    fclose(file);
    return 0;
}