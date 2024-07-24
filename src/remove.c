#include "murpkg.h"

#define INITIAL_BUFFER_SIZE PATH_MAX

int remove_package(const char *package)
{
    char command[3 * PATH_MAX + strlen(package) + 1];
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
    snprintf(command, sizeof(command), "%s n | %s -Rns %s 2>&1", echo_PATH, pacman_PATH, package);

    // Execute the command and open a pipe to read the output
    if ((fp = popen(command, "r")) == NULL)
    {
        perror("popen");
        free(dependent_packages);
        return 1;
    }

    int flag_required_by = 0;
    do
    {
        flag_required_by = 0;
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
                    flag_required_by = 1;
                }
            }
        }

        if (flag_required_by != 0)
        {
            {
                pclose(fp);
                char *required_by_command = (char *)malloc(3 * PATH_MAX + strlen(dependent_packages) + 1);
                if (required_by_command == NULL)
                {
                    perror("malloc");
                    free(dependent_packages);
                    return 1;
                }

                snprintf(required_by_command, 3 * PATH_MAX + strlen(dependent_packages) + 1, "%s n | %s -Rns %s 2>&1", echo_PATH, pacman_PATH, dependent_packages);

                if ((fp = popen(required_by_command, "r")) == NULL)
                {
                    perror("popen");
                    free(dependent_packages);
                    return 1;
                }

                free(required_by_command);
            }
        }
    } while (flag_required_by != 0);
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
        char *newcommand = (char *)malloc(2 * PATH_MAX + strlen(dependent_packages) + 1);
        if (newcommand == NULL)
        {
            perror("malloc");
            free(dependent_packages);
            return 1;
        }

        snprintf(newcommand, 2 * PATH_MAX + strlen(dependent_packages) + 1, "%s -Rns %s", pacman_PATH, dependent_packages);

        // Execute the command to remove dependent packages
        int ret = system(newcommand);
        free(newcommand);
        free(dependent_packages);
        if (ret != 0)
        {
            perror("system");
            return 1;
        }
    }
    else
    {
        free(dependent_packages);
        snprintf(command, sizeof(command), "%s -Rns %s", pacman_PATH, package);
        if (system(command) != 0)
        {
            perror("system");
            return 1;
        }
    }

    if (renew_package_installed_flag() != 0)
    {
        return 1;
    }

    return 0;
}

int remove_packages(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Error: remove: missing operand\n");
        return 1;
    }
    if (argc == 3)
    {
        return remove_package(argv[2]);
    }
    if (argc > 3)
    {
        char *combined_argv = concatenate_arguments(argc, argv);
        if (combined_argv == NULL)
        {
            printf("Error: concatenate_arguments\n");
            return 1;
        }
        if (remove_package(combined_argv) != 0)
        {
            free(combined_argv);
            return 1;
        }
        free(combined_argv);
        return 0;
    }
    return 0;
}