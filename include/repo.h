#ifndef REPO_H
#define REPO_H


int repo_init();
int repo_list();
int repo_add(char *name, char *url);
int repo_remove(char *name);

#endif