#ifndef UTILS_ANALYSIS_H
#define UTILS_ANALYSIS_H

#include "utils.h"

int analyze_repo_info();

int get_repo_info(const char *path, PackageList *pkg_list);

int get_PKGBUILD_info(const char *path, FILE *file, PackageList *pkg_list);

#endif