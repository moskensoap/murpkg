#ifndef UTIL_H
#define UTIL_H

int file_exists(const char *path);
int create_directory(const char *path);
int is_git_repo(const char *path);
int url_to_reponame(const char *url, char *reponame);

int init();
int check_REPO_FILE_existence_and_init();
int check_repo_status_and_reclone_if_needed();

int contain_underline_underline(const char *str);

extern const char *STRUCT_TEMP;


typedef struct {
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


typedef struct {
    Package *packages;
    int package_count;
} PackageList;


void init_package(Package *pkg);


void init_package_list(PackageList *pkg_list);


void add_package(PackageList *pkg_list, const char *name, const char *version, const char *rel, const char *arch, const char *mingw_arch, const char *fullname, const char *desc, const char *abs_dir, const char *install_cmds, long long int level, int flag);


void add_dependency(Package *pkg, const char *dependency);


void free_package(Package *pkg);


void free_package_list(PackageList *pkg_list);


void write_package_to_file(PackageList *pkg_list, const char *filename);


void read_package_from_file(PackageList *pkg_list, const char *filename);


#endif