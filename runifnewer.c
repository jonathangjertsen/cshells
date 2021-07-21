#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>

#ifndef WIN32
#include <unistd.h>
#endif

#ifdef WIN32
#define stat _stat
#endif

#include "shell_utils.h"

#define ARGNO_SRC 1
#define ARGNO_DEST 2
#define ARGNO_CMD 3
#define MIN_ARGS 4

static const char USAGE[] = "Usage: runifnewer <src> <dest> <command...>\n\n";

int main(int argc, char **argv)
{
    for (int i = 0; i < argc; i++)
    {
        char *arg = argv[i];
        if ((strcmp(arg, "-h") == 0) || (strcmp(arg, "--help") == 0))
        {
            printf(USAGE);
            return 0;
        }
    }

    if (argc < MIN_ARGS)
    {
        printf(USAGE);
        return 1;
    }

    line_buffer dest = {};
    line_buffer src = {};
    line_buffer command = {};

    strcpy(dest, argv[ARGNO_DEST]);
    strcpy(src, argv[ARGNO_SRC]);
    for (int i = ARGNO_CMD; i < argc; i++)
    {
        if (strcmp(argv[i], "[SRC]") == 0)
        {
            add_arg(command, src);
        }
        else if (strcmp(argv[i], "[DEST]") == 0)
        {
            add_arg(command, dest);
        }
        else
        {
            add_arg(command, argv[i]);
        }
    }

    struct stat stat_dest;
    struct stat stat_src;
    stat(dest, &stat_dest);
    if (stat(src, &stat_src))
    {
        printf("Not found: %s\n", src);
        return 1;
    }
    int dest_mtime = stat_dest.st_mtime;
    int src_mtime = stat_src.st_mtime;
    if (src_mtime > dest_mtime)
    {
        return system(command);
    }
    return 0;
}
