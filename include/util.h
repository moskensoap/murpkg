#ifndef UTIL_H
#define UTIL_H

int file_exists(const char *path);
int create_directory(const char *path);
int is_git_repo(const char *path);
int url_to_reponame(const char *url, char *reponame);

int check_REPO_FILE_existence_and_init();
int check_repo_status_and_reclone_if_needed();
int init();


#endif