#include "murpkg.h"

const char *version = "0.6.6";

void info_help()
{
    printf("Usage:\n\n");
    printf("\tmurpkg command [arguments]\n\n");
    printf("The commands are:\n\n");
    printf("\trepo init\tInitialize MUR (MSYS2 User Repository).\n");
    printf("\trepo list\tList all available repositories.\n");
    printf("\trepo add\tAdd a new repository.\n");
    printf("\trepo remove\tRemove an existing repository.\n");
    printf("\tupdate\t\tUpdate the package database.\n");
    printf("\tlist\t\tList all installed packages.\n");
    printf("\tsearch\t\tSearch for a package in the repositories.\n");
    printf("\tshow\t\tShow detailed information about an installed package or packages.\n");
    printf("\tinstall\t\tInstall a package or packages.\n");
    printf("\treinstall\tReinstall a package or packages.\n");
    printf("\tupgrade\t\tUpgrade all installed packages to the latest version.\n");
    printf("\tremove\t\tRemove a package or packages.\n");
    printf("\tautoremove\tAutomatically remove unneeded packages.\n");
    printf("\tclean\t\tClean up the build cache and temporary files.\n");
    printf("\thelp\t\tShow help message.\n");
    printf("\tversion\t\tShow the version of murpkg.\n");
}

void info_version()
{
    printf("murpkg version %s\n", version);
    printf("Copyright (c) 2024 moskensoap\n");
    printf("This project is licensed under the MIT License.\n");
}

void info_help_repo_init()
{
    printf("Usage:\n\n");
    printf("\tmurpkg repo init\n\n");
    printf("Description:\n\n");
    printf("\tInitialize the first repo MUR (https://github.com/moskensoap/MUR-packages.git).\n\n");
    printf("\tThis command will git clone MUR to /home/.mur/murpkg/repo/MUR-packages.\n");
}

void info_help_repo_list()
{
    printf("Usage:\n\n");
    printf("\tmurpkg repo list\n\n");
    printf("Description:\n\n");
    printf("\tList all available repositories.\n\n");
    printf("Format:\n\n");
    printf("\tNAME\t\t\tURL\n");
}

void info_help_repo_add()
{
    printf("Usage:\n\n");
    printf("\tmurpkg repo add <name> <url>\n\n");
    printf("Description:\n\n");
    printf("\tAdd a new repository from GitHub, GitLab, or other Git hosting services.\n\n");
    printf("Format:\n\n");
    printf("\t<name> must not contain slashes (/), periods (.), or whitespace.\n\n");
    printf("\t<url> must be in the format: https://[hostname]/ownername/repositoryname.git\n\n");
    printf("Note:\n\n");
    printf("\tmurpkg only supports repositories where PKGBUILD pkgname contains double underscores (__).\n");
}

void info_help_repo_remove()
{
    printf("Usage:\n\n");
    printf("\tmurpkg repo remove <name>\n");
}

void info_help_update()
{
    printf("Usage:\n\n");
    printf("\tmurpkg update\n\n");
    printf("Description:\n\n");
    printf("\tUpdate the package database.\n");
}

void info_help_list()
{
    printf("Usage:\n\n");
    printf("\tmurpkg list\n\n");
    printf("\tmurpkg list version\n\n");
    printf("Description:\n\n");
    printf("\tList all installed packages with name and version.\n\n");
    printf("Usage:\n\n");
    printf("\tmurpkg list name\n\n");
    printf("Description:\n\n");
    printf("\tList all installed packages with name only.\n");
}

void info_help_search()
{
    printf("Usage:\n\n");
    printf("\tmurpkg search\n\n");
    printf("Description:\n\n");
    printf("\tSearch for all packages in the repositories.\n\n");

    printf("Usage:\n\n");
    printf("\tmurpkg search <package>\n\n");
    printf("Description:\n\n");
    printf("\tSearch for a package in the repositories.\n");
}

void info_help_show()
{
    printf("Usage:\n\n");
    printf("\tmurpkg show <package>\n\n");
    printf("Description:\n\n");
    printf("\tShow detailed information about a installed package.\n\n");

    printf("Usage:\n\n");
    printf("\tmurpkg show <package1> <package2> <package3> ...\n\n");
    printf("Description:\n\n");
    printf("\tShow detailed information about multiple installed packages.\n");
}

void info_help_install()
{
    printf("Usage:\n\n");
    printf("\tmurpkg install <package>\n\n");
    printf("Description:\n\n");
    printf("\tInstall a package.\n\n");

    printf("Usage:\n\n");
    printf("\tmurpkg install <package1> <package2> <package3> ...\n\n");
    printf("Description:\n\n");
    printf("\tInstall multiple packages.\n");
}

void info_help_reinstall()
{
    printf("Usage:\n\n");
    printf("\tmurpkg reinstall <package>\n\n");
    printf("Description:\n\n");
    printf("\tReinstall a package.\n\n");

    printf("Usage:\n\n");
    printf("\tmurpkg reinstall <package1> <package2> <package3> ...\n\n");
    printf("Description:\n\n");
    printf("\tReinstall multiple packages.\n");
}

void info_help_upgrade()
{
    printf("Usage:\n\n");
    printf("\tmurpkg upgrade\n\n");
    printf("Description:\n\n");
    printf("\tUpgrade all installed packages to the latest version.\n");
}

void info_help_remove()
{
    printf("Usage:\n\n");
    printf("\tmurpkg remove <package>\n\n");
    printf("Description:\n\n");
    printf("\tRemove a package.\n\n");

    printf("Usage:\n\n");
    printf("\tmurpkg remove <package1> <package2> <package3> ...\n\n");
    printf("Description:\n\n");
    printf("\tRemove multiple packages.\n");
}

void info_help_autoremove()
{
    printf("Usage:\n\n");
    printf("\tmurpkg autoremove\n\n");
    printf("Description:\n\n");
    printf("\tAutomatically remove unneeded packages.\n");
}

void info_help_clean()
{
    printf("Usage:\n\n");
    printf("\tmurpkg clean\n\n");
    printf("Description:\n\n");
    printf("\tClean up the build cache and temporary files.\n");
}

void info_help_help()
{
    printf("Usage:\n\n");
    printf("\tmurpkg help\n\n");
    printf("Description:\n\n");
    printf("\tShow this help message.\n\n");
    printf("Usage:\n\n");
    printf("\tmurpkg help <command>\n\n");
    printf("Description:\n\n");
    printf("\tShow help message for a specific command.\n\n");
    printf("commands:\n\n");
    printf("\trepo init\trepo list\trepo add\trepo remove\n");
    printf("\tupdate\t\tlist\t\tsearch\t\tshow\n");
    printf("\tinstall\t\treinstall\tupgrade\t\tremove\n");
    printf("\tautoremove\tclean\t\thelp\t\tversion\n");
}

void info_help_version()
{
    printf("Usage:\n\n");
    printf("\tmurpkg version\n\n");
    printf("Description:\n\n");
    printf("\tShow the version of murpkg.\n");
}
