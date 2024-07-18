#ifndef REPO_H
#define REPO_H



void repo_init();
void repo_list();
void repo_add(char *name, char *url);
void repo_remove(char *name);

#endif