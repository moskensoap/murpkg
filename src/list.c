#include "murpkg.h"

int list_name_version(){
    if(system("pacman -Qm") != 0){
        perror("system");
        return 1;
    }
    return 0;
}

int list_name(){
    if(system("pacman -Qmq") != 0){
        perror("system");
        return 1;
    }
    return 0;
}