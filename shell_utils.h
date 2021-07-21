#ifndef SHELL_UTILS_H
#define SHELL_UTILS_H

#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE_SIZE 2048

#define MEMBER_SIZE(x) (sizeof(x[0]))
#define ARRAY_SIZE(x) (sizeof(x)/MEMBER_SIZE(x))

typedef char line_buffer[MAX_LINE_SIZE];

static inline void ensure(bool condition, char *message)
{
    if (!condition)
    {
        printf("Error: %s\n", message);
        exit(1);
    }
}

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
            trim_trailing_whitespace(line);
            puts(line);
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

static inline void concatenate(line_buffer lb, char *string)
{
    int string_len = strlen(string);
    int curr_len = strlen(lb);
    ensure(string_len + curr_len <= sizeof(line_buffer), "Line buffer size exceeded");
    strcat(lb, string);
}

static inline void add_arg(char *args, char *new_arg)
{
    strcat(args, new_arg);
    strcat(args, " ");
}

#endif
