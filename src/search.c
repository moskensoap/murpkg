#include "murpkg.h"

int search_all()
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

    // red bold
    printf("\033[1;31mpackage_count: %d\033[0m\n\n", read_pkg_list.package_count);
    for (int i = 0; i < read_pkg_list.package_count; i++)
    {
        // purple bold, white bold, green bold, green bold
        printf("\033[1;35m%s/\033[1;37m%s \033[1;32m%s-\033[1;32m%s\033[0m",
               read_pkg_list.packages[i].mingw_arch,
               read_pkg_list.packages[i].pkgname,
               read_pkg_list.packages[i].pkgver,
               read_pkg_list.packages[i].pkgrel);

        if (read_pkg_list.packages[i].flag)
        {
            // blue bold
            printf("\033[1;34m [installed]\033[0m\n");
        }
        else
        {
            // new line
            printf("\n");
        }

        // white
        printf("\033[0;37m    %s\033[0m\n", read_pkg_list.packages[i].pkgdesc);
    }

    if (free_package_list(&read_pkg_list) != 0)
    {
        return -1;
    }

    return 0;
}

int search_one(const char *name)
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
    // for every package in read_pkg_list, if package.pkgname == name, print its all info
    for (int i = 0; i < read_pkg_list.package_count; i++)
    {
        if (strcmp(read_pkg_list.packages[i].pkgname, name) == 0)
        {
            /*
            typedef struct
            {
            char *pkgname;
            char *pkgver;
            char *pkgrel;
            char *arch;
            char *mingw_arch;
            char *fullname;
            char *pkgdesc;
            char *abs_dir;
            char *install_cmds;
            long long int level;
            int flag;
            char **depends;
            int depends_count;
            } Package;

            typedef struct
            {
            Package *packages;
            int package_count;
            } PackageList;
            */
            printf("\n\033[1;31mExplicit package info:\n\033[0m");
            // printf pkgname: $pkgname\n pkgver: $pkgver\n and so on
            printf("pkgname: %s\npkgver: %s\npkgrel: %s\narch: %s\nmingw_arch: %s\nfullname: %s\npkgdesc: %s\nabs_dir: %s\ninstall_cmds: %s\nlevel: %lld\nflag: %d\n",
                   read_pkg_list.packages[i].pkgname,
                   read_pkg_list.packages[i].pkgver,
                   read_pkg_list.packages[i].pkgrel,
                   read_pkg_list.packages[i].arch,
                   read_pkg_list.packages[i].mingw_arch,
                   read_pkg_list.packages[i].fullname,
                   read_pkg_list.packages[i].pkgdesc,
                   read_pkg_list.packages[i].abs_dir,
                   read_pkg_list.packages[i].install_cmds,
                   read_pkg_list.packages[i].level,
                   read_pkg_list.packages[i].flag);

            // for every dependency in read_pkg_list.packages[i].depends, print dependency
            for (int j = 0; j < read_pkg_list.packages[i].depends_count; j++)
            {
                printf("depends: %s\n", read_pkg_list.packages[i].depends[j]);
            }
        }
    }

    printf("\n\033[1;31mFuzzy package info:\n\033[0m");
    // for every package in read_pkg_list, if package.pkgname contains *name*, print package info
    for (int i = 0; i < read_pkg_list.package_count; i++)
    {
        if (strstr(read_pkg_list.packages[i].pkgname, name) != NULL)
        {
            // purple bold, white bold, green bold, green bold
            printf("\033[1;35m%s/\033[1;37m%s \033[1;32m%s-\033[1;32m%s\033[0m",
                   read_pkg_list.packages[i].mingw_arch,
                   read_pkg_list.packages[i].pkgname,
                   read_pkg_list.packages[i].pkgver,
                   read_pkg_list.packages[i].pkgrel);

            if (read_pkg_list.packages[i].flag)
            {
                // blue bold
                printf("\033[1;34m [installed]\033[0m\n");
            }
            else
            {
                // new line
                printf("\n");
            }

            // white
            printf("\033[0;37m    %s\033[0m\n", read_pkg_list.packages[i].pkgdesc);
        }
    }

    if (free_package_list(&read_pkg_list) != 0)
    {
        return -1;
    }

    return 0;
}