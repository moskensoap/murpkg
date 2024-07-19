#include "murpkg.h"

/**
 * Reads the output of a command executed using popen and returns it as a dynamically allocated string.
 *
 * @param command The command to be executed.
 * @return A dynamically allocated string containing the output of the command, or NULL if an error occurred.
 */
char *read_pactree_output(const char *command)
{
    FILE *file = popen(command, "r");
    if (file == NULL)
    {
        perror("popen");
        return NULL;
    }

    size_t size = 0;
    size_t buffer_size = 1024;
    char *output = malloc(buffer_size);
    if (output == NULL)
    {
        perror("malloc");
        pclose(file);
        return NULL;
    }

    size_t read_size;
    while ((read_size = fread(output + size, 1, buffer_size - size, file)) > 0)
    {
        size += read_size;
        if (size >= buffer_size)
        {
            buffer_size *= 2;
            output = realloc(output, buffer_size);
            if (output == NULL)
            {
                perror("realloc");
                pclose(file);
                return NULL;
            }
        }
    }
    output[size] = '\0';
    pclose(file);

    return output;
}

/**
 * Extracts the dependencies from the given output string.
 *
 * @param output The output string containing the dependencies.
 * @return A dynamically allocated string containing the extracted dependencies.
 *         The caller is responsible for freeing the memory.
 *         Returns NULL if memory allocation fails.
 */
char *extract_dependencies(char *output)
{
    size_t size = strlen(output) + 1;
    char *dependencies = malloc(size);
    if (dependencies == NULL)
    {
        perror("malloc");
        return NULL;
    }
    dependencies[0] = '\0';

    char *line = strtok(output, "\n");
    while (line != NULL)
    {
        if (contain_underline_underline(line))
        {
            strcat(dependencies, line);
            strcat(dependencies, " ");
        }
        line = strtok(NULL, "\n");
    }

    return dependencies;
}

/**
 * Removes a package and its dependencies from the system.
 *
 * @param name The name of the package to be removed.
 * @return 0 if the package and its dependencies were successfully removed, 1 otherwise.
 */
int remove_one_package(const char *name)
{
    char command_pactree[2 * PATH_MAX];
    snprintf(command_pactree, sizeof(command_pactree), "pactree --unique %s", name);

    char *pactree_output = read_pactree_output(command_pactree);
    if (pactree_output == NULL)
    {
        return 1;
    }

    char *dependency = extract_dependencies(pactree_output);
    free(pactree_output);
    if (dependency == NULL)
    {
        return 1;
    }

    printf("Dependency packages: %s\n", dependency);

    snprintf(command_pactree, sizeof(command_pactree), "pactree --reverse --unique %s", name);
    char *pactree_reverse_output = read_pactree_output(command_pactree);
    if (pactree_reverse_output == NULL)
    {
        free(dependency);
        return 1;
    }

    char *reverse_dependency = extract_dependencies(pactree_reverse_output);
    free(pactree_reverse_output);
    if (reverse_dependency == NULL)
    {
        free(dependency);
        return 1;
    }

    printf("Reverse dependency packages: %s\n", reverse_dependency);

    size_t total_size = strlen(dependency) + strlen(reverse_dependency) + 1;
    char *all_dependency = malloc(total_size);
    if (all_dependency == NULL)
    {
        perror("malloc");
        free(dependency);
        free(reverse_dependency);
        return 1;
    }
    strcpy(all_dependency, dependency);
    strcat(all_dependency, reverse_dependency);

    free(dependency);
    free(reverse_dependency);

    size_t command_size = strlen(all_dependency) + 13;
    char *command_pacman = malloc(command_size);
    if (command_pacman == NULL)
    {
        perror("malloc");
        free(all_dependency);
        return 1;
    }
    snprintf(command_pacman, command_size, "pacman -Rns %s", all_dependency);
    if (system(command_pacman) != 0)
    {
        perror("system");
        free(command_pacman);
        free(all_dependency);
        return 1;
    }

    free(command_pacman);
    free(all_dependency);
    return 0;
}
