#include <dirent.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "shell_utils.h"

#define OPT_TREEVIEW (1 << 0)
#define OPT_RECURSE  (1 << 1)

static const char BAN_LIST[][sizeof(".pytest_cache")] = {
    ".pytest_cache",
    "node_modules",
    "__pycache__",
    "CMakeFiles",
    "_pytest",
    ".git",
    ".",
    ".."
};

static bool is_like_git_cache(const char *name)
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

static bool iter_dir(const char *dirname, int depth, uint32_t options)
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
            if (options & OPT_RECURSE)
            {
                iter_dir(full_name, depth + 1, options);
            }
        }
        closedir(directory);
        return true;
    }
    else
    {
        return false;
    }
}

bool cat(char *file)
{
    FILE *f = fopen(file, "r");
    if (f == NULL)
    {
        return false;
    }

    char buffer[2048];
    while (true)
    {
        size_t nread = fread(buffer, 1, sizeof(buffer), f);
        fwrite(buffer, 1, nread, stdout);
        if (nread < sizeof(buffer))
        {
            break;
        }
    }
    return true;
}

int main(int argc, char **argv)
{
    uint32_t options = 0;
    char *dir_or_file = ".";
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] != '-')
        {
            dir_or_file = argv[i];
        }

        if ((strcmp(argv[i], "-t") == 0) || (strcmp(argv[i], "--tree") == 0))
        {
            options |= OPT_TREEVIEW | OPT_RECURSE;
        }

        if ((strcmp(argv[i], "-R") == 0) || (strcmp(argv[i], "-r") == 0))
        {
            options |= OPT_RECURSE;
        }
    }

    bool was_file = cat(dir_or_file);
    if (!was_file)
    {
        iter_dir(dir_or_file, 0, options);
    }
    return 0;
}
