#include "murpkg.h"

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        info_help();
        return 0;
    }
    // ######################################version################################################
    if (strcmp(argv[1], "version") == 0)
    {
        info_version();
        return 0;
    }
    // ######################################help################################################
    if (strcmp(argv[1], "help") == 0)
    {
        if (strcmp(argv[2], "repo") == 0)
        {
            if (strcmp(argv[3], "init") == 0)
            {
                info_help_repo_init();
                return 0;
            }
            if (strcmp(argv[3], "list") == 0)
            {
                info_help_repo_list();
                return 0;
            }
            if (strcmp(argv[3], "add") == 0)
            {
                info_help_repo_add();
                return 0;
            }
            if (strcmp(argv[3], "remove") == 0)
            {
                info_help_repo_remove();
                return 0;
            }
        }
        if (strcmp(argv[2], "update") == 0)
        {
            info_help_update();
            return 0;
        }
        if (strcmp(argv[2], "list") == 0)
        {
            info_help_list();
            return 0;
        }
        if (strcmp(argv[2], "search") == 0)
        {
            info_help_search();
            return 0;
        }
        if (strcmp(argv[2], "show") == 0)
        {
            info_help_show();
            return 0;
        }
        if (strcmp(argv[2], "install") == 0)
        {
            info_help_install();
            return 0;
        }
        if (strcmp(argv[2], "upgrade") == 0)
        {
            info_help_upgrade();
            return 0;
        }
        if (strcmp(argv[2], "remove") == 0)
        {
            info_help_remove();
            return 0;
        }
        if (strcmp(argv[2], "autoremove") == 0)
        {
            info_help_autoremove();
            return 0;
        }
        if (strcmp(argv[2], "clean") == 0)
        {
            info_help_clean();
            return 0;
        }
        if (strcmp(argv[2], "help") == 0)
        {
            info_help_help();
            return 0;
        }
        info_help();
        return 0;
    }

    if (strcmp(argv[1], "repo") == 0)
    {
        if (strcmp(argv[2], "init") == 0)
        {
            if (argc != 3)
            {
                info_help_repo_init();
                return 0;
            }

            return repo_init();
        }
        if (strcmp(argv[2], "list") == 0)
        {
            if (argc != 3)
            {
                info_help_repo_list();
                return 0;
            }

            return repo_list();
        }
        if (strcmp(argv[2], "add") == 0)
        {
            if (argc != 5)
            {
                info_help_repo_add();
                return 0;
            }

            return repo_add(argv[3], argv[4]);
        }
        if (strcmp(argv[2], "remove") == 0)
        {
            if (argc != 4)
            {
                info_help_repo_remove();
                return 0;
            }

            return repo_remove(argv[3]);
        }
        info_help();
        return 0;
    }
    // ######################################init################################################
    init();
    // ######################################update################################################
    if (strcmp(argv[1], "update") == 0)
    {
        if (argc != 2)
        {
            info_help_update();
            return 0;
        }

        return update();
    }

    if (strcmp(argv[1], "list") == 0)
    {
        if (argc == 2)
        {
            return list_name_version();
        }
        if (strcmp(argv[2], "version") == 0)
        {
            if (argc != 3)
            {
                info_help_list();
                return 0;
            }

            return list_name_version();
        }
        if (strcmp(argv[2], "name") == 0)
        {
            if (argc != 3)
            {
                info_help_list();
                return 0;
            }

            return list_name();
        }
        info_help();
        return 0;
    }
    // ######################################search################################################
    if (strcmp(argv[1], "search") == 0)
    {
        if (argc == 2)
        {
            return search_all();
        }
        if (argc == 3)
        {
            return search_one(argv[2]);
        }
        info_help_search();
        return 0;
    }
    // ######################################show################################################
    if (strcmp(argv[1], "show") == 0)
    {
        if (argc == 2)
        {
            info_help_show();
            return 0;
        }
        if (argc == 3)
        {
            return show_one_package(argv[2]);
        }
        if (argc > 3)
        {
            return show_packages(argc, argv);
        }
        info_help();
        return 0;
    }
    // ######################################install################################################
    // ######################################upgrade################################################
    // ######################################remove################################################
    // ######################################autoremove################################################
    if(strcmp(argv[1], "autoremove") == 0){
        if(argc != 2){
            info_help_autoremove();
            return 0;
        }
        return autoremove();
    }
    // ######################################clean################################################
    if(strcmp(argv[1], "clean") == 0){
        if(argc != 2){
            info_help_clean();
            return 0;
        }
        return clean();
    }
    // ######################################help################################################
    info_help();
    return 0;
}
