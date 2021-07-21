#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "shell_utils.h"

#define ARGNO_ARGFILE 1
#define ARGNO_COMMAND 2
#define ARGNO_EXRA 3
#define MIN_ARGS 3

static const char USAGE[] = "Usage: fileargs <argfile> <command> [<extra_args>] [--after] [-v,--verbose]\n\n";

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

    char *argfile = argv[ARGNO_ARGFILE];
    char *base_command = argv[ARGNO_COMMAND];
    line_buffer extra_args = {};

    bool after = false;
    bool verbose = false;
    for (int i = ARGNO_EXRA; i < argc; i++)
    {
        char *arg = argv[i];
        if (strcmp(arg, "--after") == 0)
        {
            after = true;
        }
        else if ((strcmp(arg, "-v") == 0) || (strcmp(arg, "--verbose") == 0))
        {
            verbose = true;
        }
        else
        {
            add_arg(extra_args, arg);
        }
    }
    trim_trailing_whitespace(extra_args);

    line_buffer command = {};
    add_arg(command, base_command);

    line_buffer args = {};

    FILE *fp = fopen(argfile, "r");
    if (fp)
    {
        line_buffer line = {};
        while (fgets(line, sizeof(line), fp))
        {
            trim_trailing_whitespace(line);
            add_arg(args, line);
        }
        fclose(fp);

        trim_trailing_whitespace(args);
        if (after)
        {
            add_arg(command, extra_args);
            add_arg(command, args);
        }
        else
        {
            add_arg(command, args);
            add_arg(command, extra_args);
        }

        if (verbose)
        {
            printf("%s\n", command);
        }

        return system(command);
    }
    else
    {
        printf("No such file: %s\n", argfile);
        return 1;
    }
}
