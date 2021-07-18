#ifndef SHELL_UTILS_H
#define SHELL_UTILS_H

#include <ctype.h>
#include <string.h>
#include <stdio.h>

#define MAX_LINE_SIZE 2048

typedef char line_buffer[MAX_LINE_SIZE];

static inline void trim_trailing_whitespace(char *string)
{
    int length = strlen(string);
    for (int i = length-1; i >= 0; i--)
    {
        if (isspace(string[i]))
        {
            string[i] = '\0';
        }
        else
        {
            break;
        }
    }
}

static inline int head(char *command, int max_n_lines)
{
    FILE *fp = popen(command, "r");
    if (fp)
    {
        long int n_lines = 0;
        line_buffer line = {};
        while (fgets(line, sizeof(line), fp))
        {
            printf(line);
            n_lines++;
            if (n_lines >= max_n_lines)
            {
                break;
            }
        }
        return 0;
    }
    else
    {
        printf("Couldn't run '%s'\n", command);
        return -1;
    }
}

static inline void add_arg(char *args, char *new_arg)
{
    strcat(args, new_arg);
    strcat(args, " ");
}

#endif
