#include "murpkg.h"

int install_packages(int argc, char **argv)
{
    if (argc < 3)
    {
        info_help_install();
        return 1;
    }

    PackageList read_pkg_list;
    if (init_package_list(&read_pkg_list) != 0)
    {
        return 1;
    }
    if (read_package_from_file(&read_pkg_list, PACKAGES_INFO_TEMP) != 0)
    {
        return 1;
    }

    // malloc int flag_temp[read_pkg_list.package_count] and initialize it to 0
    int *flag_temp = (int *)malloc(read_pkg_list.package_count * sizeof(int));
    if (flag_temp == NULL)
    {
        perror("malloc");
        if (free_package_list(&read_pkg_list) != 0)
        {
            return 1;
        }
        return 1;
    }
    for (int i = 0; i < read_pkg_list.package_count; i++)
    {
        flag_temp[i] = 0;
    }

    // check if argv[2] to argv[argc-1] are in pkgname of read_pkg_list or in "mingw_arch/pkgname" of read_pkg_list
    // if not, print error message and return 1
    // if yes, set flag_temp[i] to 1
    for (int i = 2; i < argc; i++)
    {
        int found = 0;
        for (int j = 0; j < read_pkg_list.package_count; j++)
        {
            if (strcmp(argv[i], read_pkg_list.packages[j].pkgname) == 0)
            {
                flag_temp[j] = 1;
                found = 1;
                break;
            }
            char *mingw_pkgname = (char *)malloc(strlen(read_pkg_list.packages[j].mingw_arch) + strlen(read_pkg_list.packages[j].pkgname) + 2);
            if (mingw_pkgname == NULL)
            {
                perror("malloc");
                free(flag_temp);
                if (free_package_list(&read_pkg_list) != 0)
                {
                    return 1;
                }
                return 1;
            }
            sprintf(mingw_pkgname, "%s/%s", read_pkg_list.packages[j].mingw_arch, read_pkg_list.packages[j].pkgname);
            if (strcmp(argv[i], mingw_pkgname) == 0)
            {
                flag_temp[j] = 1;
                found = 1;
                free(mingw_pkgname);
                break;
            }
            free(mingw_pkgname);
        }
        if (found == 0)
        {
            fprintf(stderr, "error: target not found: %s\n", argv[i]);
            free(flag_temp);
            if (free_package_list(&read_pkg_list) != 0)
            {
                return 1;
            }
            return 1;
        }
    }

    int do_flag = 0;
    do
    {
        do_flag = 0;
        // for each i and j, if flag_temp[i] is 1, find package[k].pkgname as the same as package[i].depends[j], set flag_temp[k] to 1
        for (int i = 0; i < read_pkg_list.package_count; i++)
        {
            if (flag_temp[i] == 1)
            {
                for (int j = 0; j < read_pkg_list.packages[i].depends_count; j++)
                {
                    for (int k = 0; k < read_pkg_list.package_count; k++)
                    {
                        if (strcmp(read_pkg_list.packages[i].depends[j], read_pkg_list.packages[k].pkgname) == 0 && flag_temp[k] == 0)
                        {
                            flag_temp[k] = 1;
                            do_flag = 1;
                        }
                    }
                }
            }
        }
    } while (do_flag == 1);

    // set flag[i] to 0, and only fullname from popen "pacman -Qm" is in package[i].fullname, set flag[i] to 1
    for (int i = 0; i < read_pkg_list.package_count; i++)
    {
        read_pkg_list.packages[i].flag = 0;
    }

    char command[2 * PATH_MAX];
    snprintf(command, sizeof(command), "%s -Qm", pacman_PATH);
    FILE *fp = popen(command, "r");
    if (fp == NULL)
    {
        perror("popen");
        free(flag_temp);
        if (free_package_list(&read_pkg_list) != 0)
        {
            return -1;
        }
        return 1;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char fullname[PATH_MAX];

    while ((read = getline(&line, &len, fp)) != -1)
    {
        if (line[read - 1] == '\n')
        {
            line[read - 1] = '\0';
        }

        strcpy(fullname, line);

        // check if fullname is the same as fullname in read_pkg_list, if yes, set flag[i] to 1
        for (int i = 0; i < read_pkg_list.package_count; i++)
        {
            if (strcmp(fullname, read_pkg_list.packages[i].fullname) == 0)
            {
                read_pkg_list.packages[i].flag = 1;
                break;
            }
        }
    }

    free(line);
    pclose(fp);

    // flag_temp[i] is 1 means package[i] should be installed but already installed packages whos flag[i] is 1 should be skipped
    // flag_temp = flag_temp - flag, and if flag_temp[i] equals 1, package[i] should be installed
    for (int i = 0; i < read_pkg_list.package_count; i++)
    {
        flag_temp[i] = flag_temp[i] - read_pkg_list.packages[i].flag;
    }
    int install_count = 0;
    for (int i = 0; i < read_pkg_list.package_count; i++)
    {
        if (flag_temp[i] == 1)
        {
            install_count++;
        }
    }
    int install_order[install_count];
    for (int i = 0, j = 0; i < read_pkg_list.package_count; i++)
    {
        if (flag_temp[i] == 1)
        {
            install_order[j] = i;
            j++;
        }
    }
    // if package[install_order[i]].level < package[install_order[j]].level, swap install_order[i] and install_order[j]
    for (int i = 0; i < install_count; i++)
    {
        for (int j = i + 1; j < install_count; j++)
        {
            if (read_pkg_list.packages[install_order[i]].level < read_pkg_list.packages[install_order[j]].level)
            {
                int temp = install_order[i];
                install_order[i] = install_order[j];
                install_order[j] = temp;
            }
        }
    }

    // print "The following packages will be installed:\n"
    // for i = 0 to install_count, print package[install_order[i]].fullname
    printf("\033[1;37mThe following packages will be installed:\033[0m\n");
    for (int i = 0; i < install_count; i++)
    {
        printf("    %s\n", read_pkg_list.packages[install_order[i]].fullname);
    }
    // print "Proceed with installation? [Y/n]"
    // get the fisrt char of the input
    // if it is Y or y or \n then do the following else return 0
    printf("\033[1;37mProceed with installation? [Y/n] \033[0m");
    char c = getchar();
    if (c != 'Y' && c != 'y' && c != '\n')
    {
        free(flag_temp);
        if (free_package_list(&read_pkg_list) != 0)
        {
            return 1;
        }
        return 0;
    }

    // for i = 0 to install_count, install package[install_order[i]] via system(packages[install_order[i]].install_cmds)
    for (int i = 0; i < install_count; i++)
    {
        if (system(read_pkg_list.packages[install_order[i]].install_cmds) != 0)
        {
            printf("Error: %s\n", read_pkg_list.packages[install_order[i]].install_cmds);
            free(flag_temp);
            if (free_package_list(&read_pkg_list) != 0)
            {
                return 1;
            }
            return 1;
        }
    }

    free(flag_temp);
    if (free_package_list(&read_pkg_list) != 0)
    {
        return 1;
    }

    return 0;
}