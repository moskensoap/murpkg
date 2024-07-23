#include "murpkg.h"

int upgrade_packages()
{
    if (renew_package_installed_flag() != 0)
    {
        return -1;
    }

    PackageList read_pkg_list;
    if (init_package_list(&read_pkg_list) != 0)
    {
        return -1;
    }
    if (read_package_from_file(&read_pkg_list, PACKAGES_INFO_TEMP) != 0)
    {
        return -1;
    }

    // popen pacman -Qm
    char command[2 * PATH_MAX];
    snprintf(command, sizeof(command), "%s -Qm", pacman_PATH);
    FILE *fp = popen(command, "r");
    if (fp == NULL)
    {
        perror("popen");
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

        // check if fullname is the same as fullname in read_pkg_list, if yes, set flag[i] to 0
        for (int i = 0; i < read_pkg_list.package_count; i++)
        {
            if (strcmp(fullname, read_pkg_list.packages[i].fullname) == 0)
            {
                read_pkg_list.packages[i].flag = 0;
                break;
            }
        }
    }

    // sum the flag
    int sum = 0;
    for (int i = 0; i < read_pkg_list.package_count; i++)
    {
        sum += read_pkg_list.packages[i].flag;
    }

    if (sum == 0)
    {
        printf("All packages are up to date.\n");
        free(line);
        pclose(fp);
        if (free_package_list(&read_pkg_list) != 0)
        {
            return -1;
        }
        return 0;
    }

    // combian the pkgname whos flag is 1 to argv and install_packages(sum+2, argv)
    // argv[0]=murpkg, argv[1]=install, argv[2]=pkgname1, argv[3]=pkgname2, ..., argv[sum+1]=pkgname_sum, argv[sum+2]=NULL
    char **argv = (char **)malloc((sum + 3) * sizeof(char *));
    if (argv == NULL)
    {
        perror("malloc");
        free(line);
        pclose(fp);
        if (free_package_list(&read_pkg_list) != 0)
        {
            return -1;
        }
        return 1;
    }
    argv[0] = "murpkg";
    argv[1] = "install";
    int j = 2;
    for (int i = 0; i < read_pkg_list.package_count; i++)
    {
        if (read_pkg_list.packages[i].flag == 1)
        {
            argv[j] = read_pkg_list.packages[i].pkgname;
            j++;
        }
    }
    argv[j] = NULL;

    if (install_packages(sum + 2, argv) != 0)
    {
        free(argv);
        free(line);
        pclose(fp);
        if (free_package_list(&read_pkg_list) != 0)
        {
            return -1;
        }
        return 1;
    }

    free(line);
    pclose(fp);
    if (free_package_list(&read_pkg_list) != 0)
    {
        return -1;
    }

    return 0;
}