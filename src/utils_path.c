#include "murpkg.h"

// const char *REPO_PATH = "/home/.mur/murpkg/repo/";
// const char *REPO_FILE = "/home/.mur/murpkg/repo.txt";
// const char *TEMP_PATH = "/home/.mur/murpkg/.temp/";
// const char *REPO_TEMP = "/home/.mur/murpkg/.temp/repo_temp.txt";
// const char *STRUCT_TEMP = "/home/.mur/murpkg/.temp/struct_temp";

char REPO_PATH[PATH_MAX];
char REPO_FILE[PATH_MAX];
char TEMP_PATH[PATH_MAX];
char REPO_TEMP[PATH_MAX];
char STRUCT_TEMP[PATH_MAX];

char rm_PATH[PATH_MAX];
char git_PATH[PATH_MAX];
char ls_PATH[PATH_MAX];
char pacman_PATH[PATH_MAX];
char pactree_PATH[PATH_MAX];

int init_git_PATH()
{
    FILE *fp;
    char path[PATH_MAX];
    int git_installed = 0;

    // Run the command "git --version"
    fp = popen("git --version", "r");
    if (fp == NULL)
    {
        perror("popen");
        return 1;
    }

    // Read the command output
    if (fgets(path, sizeof(path) - 1, fp) != NULL)
    {
        // If we successfully read output, Git is installed
        git_installed = 1;
    }

    // Close the process
    pclose(fp);

    // Check the result and execute different code
    if (git_installed)
    {
        // git_PATH="git"
        strcpy(git_PATH, "git");
    }
    else
    {
        // git_PATH="/usr/bin/git"
        if (replaceBinary(git_PATH, murpkgfileName, "git") != 0)
        {
            return 1;
        }
    }
    return 0;
}

int init_relative_name_Binary_and_path()
{
    if (replacePath(REPO_PATH, murpkgfileName, "/home/.mur/murpkg/repo/") != 0)
    {
        return 1;
    }
    if (replacePath(REPO_FILE, murpkgfileName, "/home/.mur/murpkg/repo.txt") != 0)
    {
        return 1;
    }
    if (replacePath(TEMP_PATH, murpkgfileName, "/home/.mur/murpkg/.temp/") != 0)
    {
        return 1;
    }
    if (replacePath(REPO_TEMP, murpkgfileName, "/home/.mur/murpkg/.temp/repo_temp.txt") != 0)
    {
        return 1;
    }
    if (replacePath(STRUCT_TEMP, murpkgfileName, "/home/.mur/murpkg/.temp/struct_temp") != 0)
    {
        return 1;
    }
    if (replaceBinary(rm_PATH, murpkgfileName, "rm") != 0)
    {
        return 1;
    }
    if (init_git_PATH() != 0)
    {
        return 1;
    }
    if (replaceBinary(ls_PATH, murpkgfileName, "ls") != 0)
    {
        return 1;
    }
    if (replaceBinary(pacman_PATH, murpkgfileName, "pacman") != 0)
    {
        return 1;
    }
    if (replaceBinary(pactree_PATH, murpkgfileName, "pactree") != 0)
    {
        return 1;
    }

    return 0;
}