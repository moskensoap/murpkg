#include "murpkg.h"

int list_name_version(){
    char command_pacman_Qm[2 * PATH_MAX];
    snprintf(command_pacman_Qm, sizeof(command_pacman_Qm), "%s -Qm", pacman_PATH);
    if(system(command_pacman_Qm) != 0){
        printf("Error: %s\n", command_pacman_Qm);
        return 1;
    }
    return 0;
}

int list_name(){
    char command_pacman_Qmq[2 * PATH_MAX];
    snprintf(command_pacman_Qmq, sizeof(command_pacman_Qmq), "%s -Qmq", pacman_PATH);
    if(system(command_pacman_Qmq) != 0){
        printf("Error: %s\n", command_pacman_Qmq);
        return 1;
    }
    return 0;
}