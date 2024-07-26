#include "murpkg.h"

int autoremove()
{
    char command_pacman_autoremove[3 * PATH_MAX];
    snprintf(command_pacman_autoremove, sizeof(command_pacman_autoremove), "%s -Rns $(%s -Qdtq)", pacman_PATH, pacman_PATH);
    if (system(command_pacman_autoremove) != 0)
    {
        printf("Error: %s\n", command_pacman_autoremove);
        return 1;
    }
    return 0;
}