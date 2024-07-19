#ifndef REMOVE_H
#define REMOVE_H

char *read_pactree_output(const char *command);
char *extract_dependencies(char *output);
int remove_one_package(const char *name);

#endif 