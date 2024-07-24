#ifndef UTILS_PATH_H
#define UTILS_PATH_H

#include "murpkg.h"

extern char REPO_PATH[PATH_MAX];
extern char REPO_FILE[PATH_MAX];
extern char TEMP_PATH[PATH_MAX];
extern char REPO_TEMP[PATH_MAX];
extern char PACKAGES_INFO_TEMP[PATH_MAX];
extern char PKGBUILD_INFO_TEMP[PATH_MAX];

extern char msys2_shell_dot_cmd_PATH[PATH_MAX];
extern char printsrcinfo_PATH[PATH_MAX];

extern char rm_PATH[PATH_MAX];
extern char git_PATH[PATH_MAX];
extern char ls_PATH[PATH_MAX];
extern char pacman_PATH[PATH_MAX];
extern char pactree_PATH[PATH_MAX];
extern char echo_PATH[PATH_MAX];
extern char find_PATH[PATH_MAX];
extern char makepkg_PATH[PATH_MAX];
extern char dos2unix_PATH[PATH_MAX];

int init_relative_name_Binary_and_path();

#endif