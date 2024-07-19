#include "murpkg.h"

int autoremove(){
    if(system("/usr/bin/pacman -Rns $(/usr/bin/pacman -Qdtq)") != 0){
        perror("system");
        return 1;
    }
    return 0;
}