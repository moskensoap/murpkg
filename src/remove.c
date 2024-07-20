#include "murpkg.h"

#define INITIAL_BUFFER_SIZE PATH_MAX

int remove_package(const char *package)
{
    char command[PATH_MAX + strlen(package) + 1];
    FILE *fp;
    char result[PATH_MAX];
    size_t buffer_size = INITIAL_BUFFER_SIZE;
    char *dependent_packages = (char *)malloc(buffer_size);
    if (dependent_packages == NULL)
    {
        perror("malloc");
        return 1;
    }
    dependent_packages[0] = '\0';

    // Construct the command to remove the package
    snprintf(command, sizeof(command), "echo n | pacman -Rns %s 2>&1", package);

    // Execute the command and open a pipe to read the output
    if ((fp = popen(command, "r")) == NULL)
    {
        perror("popen");
        free(dependent_packages);
        return 1;
    }

    // Read the output and look for dependencies
    while (fgets(result, sizeof(result), fp) != NULL)
    {
        // Check if the line contains "required by"
        char *required_by_ptr = strstr(result, "required by ");
        if (required_by_ptr != NULL)
        {
            required_by_ptr += strlen("required by ");
            char *token = strtok(required_by_ptr, " \n");
            while (token != NULL)
            {
                size_t new_length = strlen(dependent_packages) + strlen(token) + 2; // +1 for space, +1 for '\0'
                if (new_length > buffer_size)
                {
                    buffer_size = new_length;
                    char *new_dependent_packages = (char *)realloc(dependent_packages, buffer_size);
                    if (new_dependent_packages == NULL)
                    {
                        perror("realloc");
                        free(dependent_packages);
                        pclose(fp);
                        return 1;
                    }
                    dependent_packages = new_dependent_packages;
                }
                strcat(dependent_packages, token);
                strcat(dependent_packages, " ");
                token = strtok(NULL, " \n");
            }
        }
    }

    // Close the pipe
    pclose(fp);

    // If there are dependent packages, try to remove them as well
    if (strlen(dependent_packages) > 0)
    {
        // Append the initial package to the dependent packages list
        size_t new_length = strlen(dependent_packages) + strlen(package) + 2; // +1 for space, +1 for '\0'
        if (new_length > buffer_size)
        {
            buffer_size = new_length;
            char *new_dependent_packages = (char *)realloc(dependent_packages, buffer_size);
            if (new_dependent_packages == NULL)
            {
                perror("realloc");
                free(dependent_packages);
                return 1;
            }
            dependent_packages = new_dependent_packages;
        }
        strcat(dependent_packages, package);
        strcat(dependent_packages, " ");

        // Allocate a large buffer for the command
        char *newcommand = (char *)malloc(PATH_MAX + strlen(dependent_packages) + 1);
        if (newcommand == NULL)
        {
            perror("malloc");
            free(dependent_packages);
            return 1;
        }

        snprintf(newcommand, PATH_MAX + strlen(dependent_packages) + 1, "pacman -Rns %s", dependent_packages);

        // Execute the command to remove dependent packages
        int ret = system(newcommand);
        free(newcommand);
        free(dependent_packages);
        if (ret == -1)
        {
            perror("system");
            return 1;
        }
        return WEXITSTATUS(ret);
    }
    else
    {
        free(dependent_packages);
        snprintf(command, sizeof(command), "pacman -Rns %s", package);
        if (system(command) != 0)
        {
            perror("system");
            return 1;
        }
        return 0;
    }
}