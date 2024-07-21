#include "murpkg.h"

int show_one_package(const char *name)
{
    // system run pacman -Qi <name>
    char command[2 * PATH_MAX];
    snprintf(command, sizeof(command), "%s -Qi ", pacman_PATH);

    strcat(command, name);

    if (system(command) != 0)
    {
        perror("system");
        return 1;
    }
    return 0;
}

int show_packages(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Error: show: missing operand\n");
        return 1;
    }
    if (argc == 3)
    {
        return show_one_package(argv[2]);
    }
    if (argc > 3)
    {
        for (int i = 2; i < argc; i++)
        {
            if (show_one_package(argv[i]) != 0)
            {
                return 1;
            }
        }
    }
    return 0;
}