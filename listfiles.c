#include <dirent.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "shell_utils.h"

#define OPT_TREEVIEW (1 << 0)

const char BAN_LIST[][sizeof(".pytest_cache")] = {
    ".pytest_cache",
    "node_modules",
    "__pycache__",
    "CMakeFiles",
    "_pytest",
    ".git",
    ".",
    ".."
};

bool is_like_git_cache(const char *name)
{
    int name_len = strlen(name);
    if (name_len != 2)
    {
        return false;
    }
    char c0 = name[0];
    char c1 = name[1];
    bool c0_is_num   = (c0 >= '0' && c0 <= '9');
    bool c0_is_hexal = (c0 >= 'a' && c0 <= 'f');
    bool c1_is_num   = (c1 >= '0' && c1 <= '9');
    bool c1_is_hexal = (c1 >= 'a' && c1 <= 'f');
    return ((c0_is_num || c0_is_hexal) && (c1_is_num || c1_is_hexal));
}

bool iter_dir(const char *dirname, int depth, uint32_t options)
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
            bool banned = false;
            for (int i = 0; i < ARRAY_SIZE(BAN_LIST); i++)
            {
                if (strcmp(file->d_name, BAN_LIST[i]) == 0)
                {
                    banned = true;
                    break;
                }
            }
            banned |= is_like_git_cache(file->d_name);
            if (banned)
            {
                continue;
            }
            line_buffer full_name = {};
            strcpy(full_name, dirname);
            strcat(full_name, "/");
            strcat(full_name, file->d_name);
            line_buffer shown_name = {};
            if (options & OPT_TREEVIEW)
            {
                for (int i = 0; i < depth * 4; i++)
                {
                    shown_name[i] = ' ';
                }
                strcat(shown_name, file->d_name);
            }
            else
            {
                strcpy(shown_name, full_name);
            }
            printf("%s\n", shown_name);
            iter_dir(full_name, depth + 1, options);
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
    uint32_t options = 0;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--tree") == 0)
        {
            options |= OPT_TREEVIEW;
        }
    }
    iter_dir(".", 0, options);
    return 0;
}
