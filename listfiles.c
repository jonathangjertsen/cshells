#include <dirent.h>
#include <stdio.h>
#include <stdbool.h>

#include "shell_utils.h"

bool iter_dir(const char *dirname, int depth)
{
    DIR *directory = opendir(dirname);
    if (directory)
    {
        while (1)
        {
            struct dirent *file = readdir(directory);
            if (!file)
            {
                break;
            }
            if (strcmp(file->d_name, ".") == 0)
            {
                continue;
            }
            if (strcmp(file->d_name, ".git") == 0)
            {
                continue;
            }
            if (strcmp(file->d_name, "..") == 0)
            {
                continue;
            }
            line_buffer full_name = {};
            strcpy(full_name, dirname);
            strcat(full_name, "/");
            strcat(full_name, file->d_name);
            printf("%s\n", full_name);
            iter_dir(full_name, depth + 1);
        }
        closedir(directory);
        return true;
    }
    else
    {
        return false;
    }
}

int main(int argc, char **argv)
{
    ensure(argc == 1, "Expect 0 args");
    iter_dir(".", 0);
    return 0;
}
