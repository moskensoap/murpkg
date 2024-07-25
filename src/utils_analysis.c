#include "murpkg.h"
#define mingw_arch_number 6
#define windows_PATH_MAX 260

int analyze_repo_info()
{
    PackageList pkg_list;
    if (init_package_list(&pkg_list) != 0)
    {
        printf("init_package_list failed\n");
        return -1;
    }

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

            printf("analyzing info of %s\n", REPO_NAME);

            int ret = get_repo_info(REPO_PATH_NAME, &pkg_list);

            if (ret != 0)
            {
                perror("get_repo_info");
                free(line);
                fclose(file);
                if (free_package_list(&pkg_list) != 0)
                {
                    printf("free_package_list failed\n");
                    return -1;
                }
                return ret;
            }
        }
    }

    free(line);
    fclose(file);

    int ret = generate_package_level(&pkg_list);
    if (ret != 0)
    {
        perror("generate_package_level");
        if (free_package_list(&pkg_list) != 0)
        {
            printf("free_package_list failed\n");
            return -1;
        }
        return ret;
    }

    if (write_package_to_file(&pkg_list, PACKAGES_INFO_TEMP) != 0)
    {
        printf("write_package_to_file failed\n");
        if (free_package_list(&pkg_list) != 0)
        {
            printf("free_package_list failed\n");
            return -1;
        }
        return -1;
    }

    if (free_package_list(&pkg_list) != 0)
    {
        printf("free_package_list failed\n");
        return -1;
    }
    return 0;
}

int get_repo_info(const char *path, PackageList *pkg_list)
{
    // path is the path of a repository, which contains many packages in thier own directories, each of which contains a PKGBUILD file
    // pkg_list is a pointer to a PackageList struct, which contains an array of Package structs and the number of packages in the array
    // this function reads the PKGBUILD files in the repository and adds the information of each package to the pkg_list
    // the information of a package includes its name, version, release, architecture, mingw architecture, full name, description, absolute directory, install commands, level, flag, and dependencies
    // the function returns 0 on success and -1 on failure of pkg_list functions and 1 on failure of other functions

    // get all directories under path and store them in variable dirs
    char dirs[3 * PATH_MAX];
    snprintf(dirs, sizeof(dirs), "%s %s -mindepth 1 -maxdepth 1 -type d", find_PATH, path);
    FILE *fp = popen(dirs, "r");
    if (fp == NULL)
    {
        perror("popen");
        return 1;
    }

    // for each directory in dirs, if it contains a PKGBUILD file, call get_PKGBUILD_info(FILE *file, PackageList *pkg_list) to get the package information and add it to pkg_list
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, fp)) != -1)
    {
        if (line[read - 1] == '\n')
        {
            line[read - 1] = '\0';
        }

        char PKGBUILD_PATH[3 * PATH_MAX];
        snprintf(PKGBUILD_PATH, sizeof(PKGBUILD_PATH), "%s/PKGBUILD", line);

        FILE *file = fopen(PKGBUILD_PATH, "r");
        if (file == NULL)
        {
            // continue if PKGBUILD file does not exist
            continue;
        }

        int ret = get_PKGBUILD_info(line, file, pkg_list);
        if (ret == 2)
        {
            printf("skipping %s\n", line);
            fclose(file);
            continue;
        }
        else if (ret != 0)
        {
            perror("get_PKGBUILD_info");
            free(line);
            fclose(file);
            pclose(fp);
            return 1;
        }

        fclose(file);
    }

    pclose(fp);
    free(line);

    printf("successfully analyzed %s\n", path);

    return 0;
}

int get_PKGBUILD_info(const char *path, FILE *file, PackageList *pkg_list)
{
    printf("analyzing PKGBUILD of %s\n", path);
    // file is a pointer to a PKGBUILD file already opened in read mode
    // pkg_list is a pointer to a PackageList struct
    // this function reads the PKGBUILD file and adds the information of the package to the pkg_list
    // the information of a package includes its name, version, release, architecture, mingw architecture, full name, description, absolute directory, install commands, level, flag, and dependencies
    // the function returns 0 on success and -1 on failure of pkg_list functions and 1 on failure of fopen related function and 2 on failure of no mingw_arch value found in PKGBUILD

    // mingw_arch is a variable to store the mingw architecture of the package
    // mingw_arch=("ucrt64" 'mingw64' "mingw32" 'clang64' 'clang32' 'clangarm64') is a example of valid values for mingw_arch

    // read file line by line to search if there is "mingw_arch" in the file
    // if "mingw_arch" is found, store the value[i] of "mingw_arch" (in the same line of "mingw_arch") in mingw_arch_detect[i]
    // if "mingw_arch" is not found, store only "msys" in mingw_arch_detect[0]
    char mingw_arch_list[mingw_arch_number][windows_PATH_MAX] = {"ucrt64", "mingw64", "mingw32", "clang64", "clang32", "clangarm64"};
    char mingw_arch_detect[mingw_arch_number][windows_PATH_MAX] = {"msys"};
    int mingw_arch_index = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, file)) != -1)
    {
        if (line[read - 1] == '\n')
        {
            line[read - 1] = '\0';
        }

        if (strstr(line, "mingw_arch") != NULL)
        {
            for (int i = 0; i < mingw_arch_number; i++)
            {
                if (strstr(line, mingw_arch_list[i]) != NULL)
                {
                    strcpy(mingw_arch_detect[mingw_arch_index], mingw_arch_list[i]);
                    mingw_arch_index++;
                }
            }
            if (mingw_arch_index == 0)
            {
                printf("mingw_arch value not found in PKGBUILD of %s\n", path);
                return 2;
            }
            else
            {
                break;
            }
        }
    }

    // for each values in mingw_arch_detect[]
    // if value is "msys", run "makepkg --printsrcinfo" and read the output
    /* example output of "makepkg --printsrcinfo" command
    $ makepkg --printsrcinfo
    pkgbase = mur__php
        pkgver = 8.2.20
        pkgrel = 1
        url = https://www.php.net
        arch = x86_64
        license = PHP
        license = License
        makedepends = gcc
        makedepends = make
        makedepends = unzip
        noextract = php-8.2.20-Win32-vs16-x64.zip
        options = !strip
        source = https://windows.php.net/downloads/releases/archives/php-8.2.20-Win32-vs16-x64.zip
        source = main.c
        source = makefile
        source = php.ini
        source = composer.ini
        source = extra.ini
        source = link.c
        source = linkmakefile
        source = setversion-php.c
        sha256sums = ad2636858768864ed4caecf1687f5a827b9c7ceafb28d06072fe2dd703372962
        sha256sums = 31915a579a045dbdd4e0a178e51a37d745a9915713121afd9d68369f5581864a
        sha256sums = e45cdbda7a078597075fb0a9e1dbe3762a3313c73cbc81d34024a61320a81a50
        sha256sums = d7266e25a4be7bbcad241af04d763876556bbbdbbf69febe9b7c84bdfb3b2412
        sha256sums = d686f99df940ee02f53d321d3224f530eeae05fb5a11300ca1987895ff5784b4
        sha256sums = 0e79011c0eec3eca19ef04a50225c4bf602c4775962a23d6c6ea3077a762341f
        sha256sums = 5e5789ad02ad6639ddd6807fb0c87dbd0ee591500e59913c5f19333fdada1a93
        sha256sums = 490009190e4fdb5f11e5c716966fc26c96fcb69eb23e594c123ba28f03e4b4a6
        sha256sums = 2ea4134d4745f4dc4c095e52567723b48a5388ed3f1a3cacb7928a92acbf1db3

    pkgname = mur__php
        pkgdesc = php-8.2.20 for Windows x64 Thread Safe

    pkgname = mur__php-development
        pkgdesc = php config for development
        depends = mur__php

    pkgname = mur__php-production
        pkgdesc = php config for production
        depends = mur__php

    pkgname = mur__php-extension-composer
        pkgdesc = php config for composer
        depends = mur__php

    pkgname = mur__php-extension-extra
        pkgdesc = php config for extra
        depends = mur__php
        depends = mur__php-extension-composer

    */
    // if value is not "msys", run "MINGW_ARCH=value makepkg-mingw --printsrcinfo" and read the output
    /*
    $ MINGW_ARCH=mingw64 makepkg-mingw --printsrcinfo
    ==>  MINGW_ARCH: mingw64
     -> Building mingw64...
    pkgbase = mingw-w64-mur__helixlink
        pkgdesc = Creat /mingw64/bin/hx.exe linking to /mingw64/lib/helix/hx.exe (mingw-w64)
        pkgver = 1.0
        pkgrel = 1
        url = https://github.com/moskensoap/MUR-packages
        arch = any
        license = MIT
        makedepends = mingw-w64-x86_64-cc
        depends = mingw-w64-x86_64-helix
        source = main.c
        source = LICENSE
        sha256sums = e0857f6e83cc20a94474506855a1d97204a4364c767e036cec073d2f84e97309
        sha256sums = ae1ac950060baad6e25e65f23e92bad0e0cb9e8de258cafcec5d25cba861af4a

    pkgname = mingw-w64-x86_64-mur__helixlink
    $ MINGW_ARCH=ucrt64 makepkg-mingw --printsrcinfo
    ==>  MINGW_ARCH: ucrt64
    -> Building ucrt64...
    pkgbase = mingw-w64-mur__helixlink
        pkgdesc = Creat /ucrt64/bin/hx.exe linking to /ucrt64/lib/helix/hx.exe (mingw-w64)
        pkgver = 1.0
        pkgrel = 1
        url = https://github.com/moskensoap/MUR-packages
        arch = any
        license = MIT
        makedepends = mingw-w64-ucrt-x86_64-cc
        depends = mingw-w64-ucrt-x86_64-helix
        source = main.c
        source = LICENSE
        sha256sums = e0857f6e83cc20a94474506855a1d97204a4364c767e036cec073d2f84e97309
        sha256sums = ae1ac950060baad6e25e65f23e92bad0e0cb9e8de258cafcec5d25cba861af4a

    pkgname = mingw-w64-ucrt-x86_64-mur__helixlink
    */

    // abstract the package information from the output of "makepkg --printsrcinfo" or "MINGW_ARCH=value makepkg-mingw --printsrcinfo"
    // for each line read: name=pkgname, version=pkgver, rel=pkgrel, arch=arch, mingw_arch=mingw_arch, fullname=pkgname, desc=pkgdesc (priority under its own pkgname), abs_dir=path, level=0, flag=0

    // if value in mingw_arch_detect[] is "msys", install_cmds=cd path && (pacman -U pkgname-pkgver-pkgrel-arch.pkg.tar.zst || (/msys2_shell.cmd -defterm -here -no-start -msys -c "makepkg --cleanbuild --syncdeps --force --noconfirm" && pacman -U pkgname-pkgver-pkgrel-arch.pkg.tar.zst))
    // if value in mingw_arch_detect[] is not "msys", install_cmd=cd path && (pacman -U pkgname-pkgver-pkgrel-arch.pkg.tar.zst || (/msys2_shell.cmd -defterm -here -no-start -$value -c "makepkg-mingw --cleanbuild --syncdeps --force --noconfirm" && pacman -U pkgname-pkgver-pkgrel-arch.pkg.tar.zst))

    // and then call add_package(PackageList *pkg_list, const char *name, const char *version, const char *rel, const char *arch, const char *mingw_arch, const char *fullname, const char *desc, const char *abs_dir, const char *install_cmds, long long int level, int flag) to add the package information to pkg_list

    // for each value of depends and makedepends store it as dependency and call add_dependency(Package *pkg, const char *dependency) to add it to the package
    char printsrcinfo[8 * PATH_MAX];
    char name[PATH_MAX], version[PATH_MAX], rel[PATH_MAX], arch[PATH_MAX], mingw_arch[PATH_MAX], fullname[4 * PATH_MAX], desc[PATH_MAX], abs_dir[PATH_MAX], install_cmds[15 * PATH_MAX], dependency[PATH_MAX];
    long long int level = 0;
    int flag = 0;
    strncpy(abs_dir, path, sizeof(abs_dir));
    abs_dir[sizeof(abs_dir) - 1] = '\0';
    char *start = NULL;

    if (create_directory_for_file(PKGBUILD_INFO_TEMP) != 0)
    {
        return 1;
    }

    if (mingw_arch_index == 0)
    {

        snprintf(printsrcinfo, sizeof(printsrcinfo), "cd %s && %s . -maxdepth 1 -type f -exec %s {} + > /dev/null 2>&1 && %s ./PKGBUILD > %s", path, find_PATH, dos2unix_PATH, printsrcinfo_PATH, PKGBUILD_INFO_TEMP);
        if (system(printsrcinfo) != 0)
        {
            return 1;
        }
        FILE *fp = fopen(PKGBUILD_INFO_TEMP, "r");
        if (fp == NULL)
        {
            perror("fopen");
            return 1;
        }
        strcpy(mingw_arch, "msys");

        // read the output of "makepkg --printsrcinfo"
        // store value after "pkgdesc =" to desc
        // store value after "pkgver =" to version
        // store value after "pkgrel =" to rel
        // store value after "arch =" to arch, if arch is "i686", change it to "x86_64"
        // for each "pkgname =", store value after "pkgname =" to name
        // if between two "pkgname =" there is "pkgdesc =", store value after "pkgdesc =" to desc
        // if between two "pkgname =" there is "arch =", store value after "arch =" to arch, if arch is "i686", change it to "x86_64"
        // count the number of "pkgname =" and store it in count
        int count = 0;
        while ((read = getline(&line, &len, fp)) != -1)
        {
            if (line[read - 1] == '\n')
            {
                line[read - 1] = '\0';
            }
            if (strstr(line, "pkgname =") != NULL)
            {
                count++;
            }
        }
        fseek(fp, 0, SEEK_SET);
        for (int i = 0; i < count; i++)
        {
            int j = 0, arch_flag = 0;
            while ((read = getline(&line, &len, fp)) != -1)
            {
                if (line[read - 1] == '\n')
                {
                    line[read - 1] = '\0';
                }
                if ((start = strstr(line, "pkgname =")) != NULL)
                {
                    j++;
                    if (i == j - 1)
                    {
                        strcpy(name, start + 10);
                    }
                }
                if ((start = strstr(line, "pkgdesc =")) != NULL && (j == 0 || i == j - 1))
                {
                    strcpy(desc, start + 10);
                }
                if ((start = strstr(line, "pkgver =")) != NULL && (j == 0 || i == j - 1))
                {
                    strcpy(version, start + 9);
                }
                if ((start = strstr(line, "pkgrel =")) != NULL && (j == 0 || i == j - 1))
                {
                    strcpy(rel, start + 9);
                }
                if ((start = strstr(line, "arch =")) != NULL && ((j == 0 && arch_flag == 0) || (i == j - 1 && arch_flag <= 1)))
                {
                    arch_flag++;

                    strcpy(arch, start + 7);

                    if (strcmp(arch, "i686") == 0)
                    {
                        strcpy(arch, "x86_64");
                    }
                }
            }
            snprintf(fullname, sizeof(fullname), "%s %s-%s", name, version, rel);
            snprintf(install_cmds, sizeof(install_cmds), "cd %s && (%s -U %s-%s-%s-%s.pkg.tar.zst --noconfirm || (%s -defterm -here -no-start -%s -c \"%s --cleanbuild --syncdeps --force --noconfirm\" && %s -U %s-%s-%s-%s.pkg.tar.zst --noconfirm))", abs_dir, pacman_PATH, name, version, rel, arch, msys2_shell_dot_cmd_PATH, mingw_arch, makepkg_PATH, pacman_PATH, name, version, rel, arch);

            // printf all the information of the package
            // printf("name: %s; version: %s; rel: %s; arch: %s; mingw_arch: %s; fullname: %s; desc: %s; abs_dir: %s; install_cmds: %s; level: %lld; flag: %d\n", name, version, rel, arch, mingw_arch, fullname, desc, abs_dir, install_cmds, level, flag);

            if (add_package(pkg_list, name, version, rel, arch, mingw_arch, fullname, desc, abs_dir, install_cmds, level, flag) != 0)
            {
                return -1;
            }

            fseek(fp, 0, SEEK_SET);
            int k = 0;
            while ((read = getline(&line, &len, fp)) != -1)
            {
                if (line[read - 1] == '\n')
                {
                    line[read - 1] = '\0';
                }
                if (strstr(line, "pkgname =") != NULL)
                {
                    k++;
                }
                if ((start = strstr(line, "depends =")) != NULL && (k == 0 || i == k - 1))
                {
                    strcpy(dependency, start + 10);

                    if (contain_underline_underline(dependency))
                    {
                        if (add_dependency(&pkg_list->packages[pkg_list->package_count - 1], dependency) != 0)
                        {
                            return -1;
                        }
                    }
                }

                if ((start = strstr(line, "makedepends =")) != NULL && (k == 0 || i == k - 1))
                {
                    strcpy(dependency, start + 14);

                    if (contain_underline_underline(dependency))
                    {
                        if (add_dependency(&pkg_list->packages[pkg_list->package_count - 1], dependency) != 0)
                        {
                            return -1;
                        }
                    }
                }
            }

            fseek(fp, 0, SEEK_SET);
        }

        fclose(fp);
    }
    else
    {
        for (int ii = 0; ii < mingw_arch_index; ii++)
        {
            snprintf(printsrcinfo, sizeof(printsrcinfo), "%s -defterm -here -no-start -%s -c 'cd %s && %s . -maxdepth 1 -type f -exec %s {} + > /dev/null 2>&1 && %s ./PKGBUILD > %s'", msys2_shell_dot_cmd_PATH, mingw_arch_detect[ii], path, find_PATH, dos2unix_PATH, printsrcinfo_PATH, PKGBUILD_INFO_TEMP);
            if (system(printsrcinfo) != 0)
            {
                return 1;
            }
            FILE *fp = fopen(PKGBUILD_INFO_TEMP, "r");
            if (fp == NULL)
            {
                perror("fopen");
                return 1;
            }
            strcpy(mingw_arch, mingw_arch_detect[ii]);

            // read the output of "MINGW_ARCH=value makepkg-mingw --printsrcinfo"
            int count = 0;
            while ((read = getline(&line, &len, fp)) != -1)
            {
                if (line[read - 1] == '\n')
                {
                    line[read - 1] = '\0';
                }
                if (strstr(line, "pkgname =") != NULL)
                {
                    count++;
                }
            }
            fseek(fp, 0, SEEK_SET);
            for (int i = 0; i < count; i++)
            {
                int j = 0, arch_flag = 0;
                while ((read = getline(&line, &len, fp)) != -1)
                {
                    if (line[read - 1] == '\n')
                    {
                        line[read - 1] = '\0';
                    }
                    if ((start = strstr(line, "pkgname =")) != NULL)
                    {
                        j++;
                        if (i == j - 1)
                        {
                            strcpy(name, start + 10);
                        }
                    }
                    if ((start = strstr(line, "pkgdesc =")) != NULL && (j == 0 || i == j - 1))
                    {
                        strcpy(desc, start + 10);
                    }
                    if ((start = strstr(line, "pkgver =")) != NULL && (j == 0 || i == j - 1))
                    {
                        strcpy(version, start + 9);
                    }
                    if ((start = strstr(line, "pkgrel =")) != NULL && (j == 0 || i == j - 1))
                    {
                        strcpy(rel, start + 9);
                    }
                    if ((start = strstr(line, "arch =")) != NULL && ((j == 0 && arch_flag == 0) || (i == j - 1 && arch_flag <= 1)))
                    {
                        arch_flag++;

                        strcpy(arch, start + 7);

                        if (strcmp(arch, "i686") == 0)
                        {
                            strcpy(arch, "x86_64");
                        }
                    }
                }
                snprintf(fullname, sizeof(fullname), "%s %s-%s", name, version, rel);
                snprintf(install_cmds, sizeof(install_cmds), "cd %s && (%s -U %s-%s-%s-%s.pkg.tar.zst --noconfirm || (%s -defterm -here -no-start -%s -c \"%s-mingw --cleanbuild --syncdeps --force --noconfirm\" && %s -U %s-%s-%s-%s.pkg.tar.zst --noconfirm))", abs_dir, pacman_PATH, name, version, rel, arch, msys2_shell_dot_cmd_PATH, mingw_arch, makepkg_PATH, pacman_PATH, name, version, rel, arch);

                // printf all the information of the package
                // printf("name: %s; version: %s; rel: %s; arch: %s; mingw_arch: %s; fullname: %s; desc: %s; abs_dir: %s; install_cmds: %s; level: %lld; flag: %d\n", name, version, rel, arch, mingw_arch, fullname, desc, abs_dir, install_cmds, level, flag);

                if (add_package(pkg_list, name, version, rel, arch, mingw_arch, fullname, desc, abs_dir, install_cmds, level, flag) != 0)
                {
                    return -1;
                }

                fseek(fp, 0, SEEK_SET);
                int k = 0;
                while ((read = getline(&line, &len, fp)) != -1)
                {
                    if (line[read - 1] == '\n')
                    {
                        line[read - 1] = '\0';
                    }
                    if (strstr(line, "pkgname =") != NULL)
                    {
                        k++;
                    }
                    if ((start = strstr(line, "depends =")) != NULL && (k == 0 || i == k - 1))
                    {
                        strcpy(dependency, start + 10);

                        if (contain_underline_underline(dependency))
                        {
                            if (add_dependency(&pkg_list->packages[pkg_list->package_count - 1], dependency) != 0)
                            {
                                return -1;
                            }
                        }
                    }

                    if ((start = strstr(line, "makedepends =")) != NULL && (k == 0 || i == k - 1))
                    {
                        strcpy(dependency, start + 14);

                        if (contain_underline_underline(dependency))
                        {
                            if (add_dependency(&pkg_list->packages[pkg_list->package_count - 1], dependency) != 0)
                            {
                                return -1;
                            }
                        }
                    }
                }

                fseek(fp, 0, SEEK_SET);
            }
            fclose(fp);
        }
    }

    free(line);
    return 0;
}

// ##########################################################################################################################
// ##########################################################################################################################
// ##########################################################################################################################
// ##########################################################################################################################
int get_package_installed_flag(PackageList *pkg_list)
{
    char command[2 * PATH_MAX];
    snprintf(command, sizeof(command), "%s -Qmq", pacman_PATH);
    FILE *fp = popen(command, "r");
    if (fp == NULL)
    {
        perror("popen");
        return 1;
    }

    // set initial value of flag to 0 for all packages
    for (int i = 0; i < pkg_list->package_count; i++)
    {
        pkg_list->packages[i].flag = 0;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char name[PATH_MAX];

    while ((read = getline(&line, &len, fp)) != -1)
    {
        if (line[read - 1] == '\n')
        {
            line[read - 1] = '\0';
        }

        strcpy(name, line);

        for (int i = 0; i < pkg_list->package_count; i++)
        {
            if (strcmp(pkg_list->packages[i].pkgname, name) == 0)
            {
                pkg_list->packages[i].flag = 1;
            }
        }
    }

    free(line);
    pclose(fp);
    return 0;
}

int generate_package_level(PackageList *pkg_list)
{
    // malloc long long int level_temp[package_count] with initial value 0
    // malloc int flag_temp[package_count] with initial value 0
    // set pkg_list->packages[i].flag=1 for all
    long long int *level_temp = (long long int *)malloc(pkg_list->package_count * sizeof(long long int));
    if (level_temp == NULL)
    {
        perror("malloc");
        return 1;
    }
    int *flag_temp = (int *)malloc(pkg_list->package_count * sizeof(int));
    if (flag_temp == NULL)
    {
        perror("malloc");
        free(level_temp);
        return 1;
    }
    for (int i = 0; i < pkg_list->package_count; i++)
    {
        level_temp[i] = 0;
        flag_temp[i] = 0;
        pkg_list->packages[i].flag = 1;
        // set the level of all packages to 0
        pkg_list->packages[i].level = 0;
    }

    //"level" is a transitive order relation indicating the dependency importance of being relied on by other packages

    int sum = 0;
    do
    {
        sum = 0;
        for (int i = 0; i < pkg_list->package_count; i++)
        {
            flag_temp[i] = 0;
            sum += pkg_list->packages[i].flag;
        }

        for (int i = 0; i < pkg_list->package_count; i++)
        {
            if (pkg_list->packages[i].flag != 0)
            {
                for (int j = 0; j < pkg_list->packages[i].depends_count; j++)
                {
                    for (int k = 0; k < pkg_list->package_count; k++)
                    {
                        if (strcmp(pkg_list->packages[k].pkgname, pkg_list->packages[i].depends[j]) == 0)
                        {
                            flag_temp[k] = 1;
                            level_temp[k] += pkg_list->packages[i].level + 1;
                        }
                    }
                }
            }
        }

        for (int i = 0; i < pkg_list->package_count; i++)
        {
            pkg_list->packages[i].flag = flag_temp[i];
            pkg_list->packages[i].level = level_temp[i];
        }

    } while (sum != 0);

    // reset flag to 0 for all packages
    for (int i = 0; i < pkg_list->package_count; i++)
    {
        pkg_list->packages[i].flag = 0;
    }

    free(level_temp);
    free(flag_temp);
    return 0;
}

int renew_package_installed_flag()
{
    PackageList read_pkg_list;
    if (init_package_list(&read_pkg_list) != 0)
    {
        printf("init_package_list failed\n");
        return -1;
    }
    if (read_package_from_file(&read_pkg_list, PACKAGES_INFO_TEMP) != 0)
    {
        printf("read_package_from_file failed\n");
        return -1;
    }

    if (get_package_installed_flag(&read_pkg_list) != 0)
    {
        printf("get_package_installed_flag failed\n");
        return -1;
    }

    if (write_package_to_file(&read_pkg_list, PACKAGES_INFO_TEMP) != 0)
    {
        printf("write_package_to_file failed\n");
        return -1;
    }

    if (free_package_list(&read_pkg_list) != 0)
    {
        printf("free_package_list failed\n");
        return -1;
    }

    return 0;
}