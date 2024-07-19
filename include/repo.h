#ifndef REPO_H
#define REPO_H

extern const char *REPO_PATH;
extern const char *REPO_FILE;
extern const char *TEMP_PATH;
extern const char *REPO_TEMP;


int repo_init();
int repo_list();
int repo_add(char *name, char *url);
int repo_remove(char *name);

#endif